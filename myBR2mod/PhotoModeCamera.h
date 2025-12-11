#pragma once
#include <Windows.h>
#include "Config.h"
#include "MinHook.h"
#pragma comment(lib, "libMinHook.x86.lib")

// Function signature for FUN_005e1a10 in Ghidra
// We hook the rendering code and overwrite the values at the end of the update loop.
typedef void(__stdcall* FN_SetCameraValues)(float* position, float* target, float fov, float* listener);

class PhotoModeCameraHook {
private:
    uintptr_t targetFunctionAddress = 0x005E1A10;

    // must be static - MinHook requires a raw function pointer
    static inline FN_SetCameraValues originalFunction = nullptr;

    // must be static - accessed by static hook function
    static inline bool overrideActive = false;
    static inline float overridePosition[3] = { 0.0f, 0.0f, 0.0f };
    static inline float overrideTarget[3] = { 0.0f, 0.0f, 0.0f };
    static inline float overrideFov = 28.0f;

    bool hookInstalled = false;
    bool hookEnabled = false;

    // must be static - called directly by the game
    static void __cdecl hookedSetCameraValues(float* position, float* target, float fov, float* listener) {
        if (overrideActive) {
            float hookedPosition[3] = {
                overridePosition[0],
                overridePosition[1],
                overridePosition[2]
            };
            float hookedTarget[3] = {
                overrideTarget[0],
                overrideTarget[1],
                overrideTarget[2]
            };

            originalFunction(hookedPosition, hookedTarget, overrideFov, listener);
        }
        else {
            // Pass through original values
            originalFunction(position, target, fov, listener);
        }
    }

public:
    bool install() {
        if (this->hookInstalled) {
            DEBUG_LOG("[CameraHook] Already installed");
            return true;
        }

        MH_STATUS status = MH_Initialize();
        if (status != MH_OK && status != MH_ERROR_ALREADY_INITIALIZED) {
            DEBUG_LOG("[CameraHook] MH_Initialize failed: " << status);
            return false;
        }

        status = MH_CreateHook(
            (LPVOID)this->targetFunctionAddress,
            (LPVOID)&hookedSetCameraValues,
            (LPVOID*)&originalFunction
        );

        if (status != MH_OK) {
            DEBUG_LOG("[CameraHook] MH_CreateHook failed: " << status);
            return false;
        }

        status = MH_EnableHook((LPVOID)this->targetFunctionAddress);
        if (status != MH_OK) {
            DEBUG_LOG("[CameraHook] MH_EnableHook failed: " << status);
            return false;
        }

        this->hookInstalled = true;
        this->hookEnabled = true;
        DEBUG_LOG("[CameraHook] Hook installed successfully at 0x" << std::hex << this->targetFunctionAddress);
        return true;
    }

    void uninstall() {
        if (!this->hookInstalled) return;

        MH_DisableHook((LPVOID)this->targetFunctionAddress);
        MH_RemoveHook((LPVOID)this->targetFunctionAddress);
        MH_Uninitialize();

        this->hookInstalled = false;
        this->hookEnabled = false;
        originalFunction = nullptr;
        DEBUG_LOG("[CameraHook] Hook uninstalled");
    }

    void activateOverride(float posX, float posY, float posZ,
        float targetX, float targetY, float targetZ,
        float fov) {
        overridePosition[0] = posX;
        overridePosition[1] = posY;
        overridePosition[2] = posZ;
        overrideTarget[0] = targetX;
        overrideTarget[1] = targetY;
        overrideTarget[2] = targetZ;
        overrideFov = fov;
        overrideActive = true;
        DEBUG_LOG("[CameraHook] Override activated - Pos(" << posX << ", " << posY << ", " << posZ << ")");
    }

    void deactivateOverride() {
        overrideActive = false;
        DEBUG_LOG("[CameraHook] Override deactivated");
    }

    void setOverridePosition(float x, float y, float z) {
        overridePosition[0] = x;
        overridePosition[1] = y;
        overridePosition[2] = z;
    }

    void setOverrideTarget(float x, float y, float z) {
        overrideTarget[0] = x;
        overrideTarget[1] = y;
        overrideTarget[2] = z;
    }

    void setOverrideFov(float fov) {
        overrideFov = fov;
    }

    bool isInstalled() const { return this->hookInstalled; }
    const bool isOverrideActive() const { return overrideActive; }

    float getOverrideX() const { return overridePosition[0]; }
    float getOverrideY() const { return overridePosition[1]; }
    float getOverrideZ() const { return overridePosition[2]; }
    float getOverrideTargetX() const { return overrideTarget[0]; }
    float getOverrideTargetY() const { return overrideTarget[1]; }
    float getOverrideTargetZ() const { return overrideTarget[2]; }
    float getOverrideFov() const { return overrideFov; }
};

class PhotoModeCamera {
private:
    // The hook that intercepts camera writes to the renderer
    PhotoModeCameraHook hook;

    // Game pointers (XZY order)
    float* cameraX;
    float* cameraZ;
    float* cameraY;
    float* cameraPitch;
    float* cameraYaw;
    float* fov;
    int* cameraMode;
    bool* pushCamera;
    float* timeFactor;

    // Our photo mode camera state (XYZ order - matches hook/renderer)
    float photoX;
    float photoY;
    float photoZ;
    float photoFov;

    // Target point for camera orientation
    float targetX;
    float targetY;
    float targetZ;

    // Original values for restoration
    float originalTimeFactor;

    bool enabled;

    // we must check to see if the camera's vftable is loaded before we attempt to hook the update loop.
    bool safeToHook = false;

    void captureCurrentState() {
        this->photoX = *this->cameraX;  // 0x004 -> X
        this->photoY = *this->cameraZ;  // 0x008 -> this is actually vertical?
        this->photoZ = *this->cameraY;  // 0x00C -> this is actually depth?
        this->photoFov = *this->fov;

        float pitch = *this->cameraPitch;
        float yaw = *this->cameraYaw;

        // Calculate target position based on current orientation
        float distance = 10.0f;
        this->targetX = this->photoX + (distance * cos(pitch) * cos(yaw));
        this->targetY = this->photoY + (distance * sin(pitch));
        this->targetZ = this->photoZ + (distance * cos(pitch) * sin(yaw));

        this->originalTimeFactor = *this->timeFactor;
    }

    void pushStateToHook() {
        this->hook.setOverridePosition(this->photoX, this->photoY, this->photoZ);
        this->hook.setOverrideTarget(this->targetX, this->targetY, this->targetZ);
        this->hook.setOverrideFov(this->photoFov);
    }

public:
    PhotoModeCamera()
        : enabled(false),
        photoX(0), photoY(0), photoZ(0), photoFov(28.0f),
        targetX(0), targetY(0), targetZ(0),
        originalTimeFactor(1.0f)
    {
        this->cameraX = (float*)GameAddresses::CameraX;
        this->cameraZ = (float*)GameAddresses::CameraZ;
        this->cameraY = (float*)GameAddresses::CameraY;
        this->cameraPitch = (float*)GameAddresses::CameraPitch;
        this->cameraYaw = (float*)GameAddresses::CameraYaw;
        this->fov = (float*)GameAddresses::CameraFOV;
        this->cameraMode = (int*)GameAddresses::CameraMode;

        this->pushCamera = (bool*)GameAddresses::PushCamera;
        this->timeFactor = (float*)GameAddresses::TimeFactor;
    }

    ~PhotoModeCamera() {
        if (this->enabled) {
            this->Disable();
        }
        this->hook.uninstall();
    }

    // Call this after game is loaded but before enabling photo mode
    bool InstallHook() {
        if (!this->safeToHook) {
            DEBUG_LOG("[PhotoMode] Not safe to hook - call CheckSafeToHook() first");
            return false;
        }
        return this->hook.install();
    }

    bool IsEnabled() const {
        return this->enabled;
    }

    bool IsHookInstalled() const {
        return this->hook.isInstalled();
    }

    void Enable() {
        if (this->enabled) return;

        if (!this->hook.isInstalled()) {
            DEBUG_LOG("[PhotoMode] Cannot enable - hook not installed");
            return;
        }

        // Capture current camera state
        this->captureCurrentState();

        // Freeze time
        *this->timeFactor = 0.0001f;

        // Activate hook with captured values
        this->hook.activateOverride(
            this->photoX, this->photoY, this->photoZ,
            this->targetX, this->targetY, this->targetZ,
            this->photoFov
        );

        this->enabled = true;
        DEBUG_LOG("[PhotoMode] pitch:" << (*this->cameraPitch) << " yaw: " << (*this->cameraYaw));
    }

    void Disable() {
        if (!this->enabled) return;

        // Deactivate hook override - game regains camera control
        this->hook.deactivateOverride();

        // Restore time
        *this->timeFactor = this->originalTimeFactor;

        this->enabled = false;
        DEBUG_LOG("[PhotoMode] Disabled");
    }

    void Toggle() {
        if (this->enabled) {
            this->Disable();
        }
        else {
            this->Enable();
        }
    }

    // Adjust camera position in world space
    // Parameters use XZY order to match key bindings
    void AdjustPosition(float dx, float dz, float dy) {
        if (!this->enabled) return;

        this->photoX += dx;
        this->photoY += dy;
        this->photoZ += dz;

        // Move target by same amount to maintain orientation
        // Wtf is this? Doesn't this mean target and position will always be locked?
        //this->targetX += dx;
        //this->targetY += dy;
        //this->targetZ += dz;

        this->pushStateToHook();
    }

    // Adjust target position independently (changes where camera looks)
    // Parameters use XZY order to match key bindings
    void AdjustTarget(float dx, float dz, float dy) {
        if (!this->enabled) return;

        this->targetX += dx;
        this->targetY += dy;
        this->targetZ += dz;

        this->hook.setOverrideTarget(this->targetX, this->targetY, this->targetZ);
    }

    void AdjustFOV(float delta) {
        if (!this->enabled) return;

        this->photoFov += delta;
        this->hook.setOverrideFov(this->photoFov);
    }

    // these methods could be private.
    bool IsSafeToHook() {
        return this->safeToHook;
    }

    void CheckSafeToHook() {
        // read the vftable pointer at 0x5E34EE0- it should contain 0x006F9AEC.

        uintptr_t* cameraVfTable = (uintptr_t*)0x5E34EE0;
        // SEH because could be uninitialized
        __try {
            if (*cameraVfTable == 0x006F9AEC) {
                this->safeToHook = true;
            }
            else {
                this->safeToHook = false;
            }
        }
        __except (EXCEPTION_EXECUTE_HANDLER) {
            this->safeToHook = false;
        }
    }

    // Debug output
    void PrintState() const {
        DEBUG_LOG("[PhotoMode] " << (this->enabled ? "ON" : "OFF")
            << " Pos(" << this->photoX << ", " << this->photoY << ", " << this->photoZ << ")"
            << " Target(" << this->targetX << ", " << this->targetY << ", " << this->targetZ << ")"
            << " FOV: " << this->photoFov);
        DEBUG_LOG("[PhotoMode] Hook installed: " << (this->hook.isInstalled() ? "Yes" : "No")
            << " Override active: " << (this->hook.isOverrideActive() ? "Yes" : "No"));
    }
};