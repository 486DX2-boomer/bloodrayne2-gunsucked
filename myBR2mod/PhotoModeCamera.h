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

    // must be static, required by Minhook
    static inline FN_SetCameraValues originalFunction = nullptr;

    // must be static
    static inline bool overrideActive = false;
    static inline float overridePosition[3] = { 0.0f, 0.0f, 0.0f };
    static inline float overrideAngles[3] = { 0.0f, 0.0f, 0.0f };
    static inline float overrideFov = 28.0f;

    bool hookInstalled = false;
    bool hookEnabled = false;

    // must be static
    static void __stdcall hookedSetCameraValues(float* position, float* angles, float fov, float* listener) {
        if (overrideActive) {
            float hookedPosition[3] = {
                overridePosition[0],
                overridePosition[1],
                overridePosition[2]
            };
            float hookedAngles[3] = {
                overrideAngles[0],
                overrideAngles[1],
                overrideAngles[2]
            };

            originalFunction(hookedPosition, hookedAngles, overrideFov, listener);
        }
        else {
            originalFunction(position, angles, fov, listener);
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
        DEBUG_LOG("[CameraHook] Hook installed at 0x" << std::hex << this->targetFunctionAddress);
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
        float anglesPitch, float anglesYaw, float anglesRoll,
        float fov) {
        overridePosition[0] = posX;
        overridePosition[1] = posY;
        overridePosition[2] = posZ;
        overrideAngles[0] = anglesPitch;
        overrideAngles[1] = anglesYaw;
        overrideAngles[2] = anglesRoll;
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

    void setOverrideAngles(float x, float y, float z) {
        overrideAngles[0] = x;
        overrideAngles[1] = y;
        overrideAngles[2] = z;
    }

    void setOverrideFov(float fov) {
        overrideFov = fov;
    }

    bool isInstalled() const { return this->hookInstalled; }
    const bool isOverrideActive() const { return overrideActive; }

    float getOverrideX() const { return overridePosition[0]; }
    float getOverrideY() const { return overridePosition[1]; }
    float getOverrideZ() const { return overridePosition[2]; }
    float getOverrideAnglesPitch() const { return overrideAngles[0]; }
    float getOverrideAnglesYaw() const { return overrideAngles[1]; }
    float getOverrideAnglesRoll() const { return overrideAngles[2]; }
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

    float anglesPitch;
    float anglesYaw;
    float anglesRoll;

    // Original values for restoration
    float originalTimeFactor;

    bool enabled;

    // we must check to see if the camera's vftable is loaded before we attempt to hook the update loop.
    bool safeToHook = false;

    void captureCurrentState() {
        this->photoX = *this->cameraX;  // 0x004 → param_1[0] → 0x06121F34
        this->photoY = *this->cameraZ;  // 0x008 → param_1[1] → 0x06121F38 (vertical)
        this->photoZ = *this->cameraY;  // 0x00C → param_1[2] → 0x06121F3C
        this->photoFov = *this->fov;

        this->anglesPitch = *this->cameraPitch;
        this->anglesYaw = *this->cameraYaw;
        this->anglesRoll = 0.0f;

        this->originalTimeFactor = *this->timeFactor;
    }

    void pushStateToHook() {
        this->hook.setOverridePosition(this->photoX, this->photoY, this->photoZ);
        this->hook.setOverrideAngles(this->anglesPitch, this->anglesYaw, this->anglesRoll);
        this->hook.setOverrideFov(this->photoFov);
    }

public:
    PhotoModeCamera()
        : enabled(false),
        photoX(0), photoY(0), photoZ(0), photoFov(28.0f),
        anglesPitch(0), anglesYaw(0), anglesRoll(0),
        originalTimeFactor(1.0f)
    {
        this->cameraX = (float*)Rayne2::CameraX;
        this->cameraZ = (float*)Rayne2::CameraZ;
        this->cameraY = (float*)Rayne2::CameraY;
        this->cameraPitch = (float*)Rayne2::CameraPitch;
        this->cameraYaw = (float*)Rayne2::CameraYaw;
        this->fov = (float*)Rayne2::CameraFOV;
        this->cameraMode = (int*)Rayne2::CameraMode;

        this->pushCamera = (bool*)Rayne2::PushCamera;
        this->timeFactor = (float*)Rayne2::TimeFactor;
    }

    ~PhotoModeCamera() {
        if (this->enabled) {
            this->disable();
        }
        this->hook.uninstall();
    }

    // Call this after game is loaded but before enabling photo mode
    bool installHook() {
        if (!this->safeToHook) {
            DEBUG_LOG("[PhotoMode] Not safe to hook");
            return false;
        }
        return this->hook.install();
    }

    bool isEnabled() const {
        return this->enabled;
    }

    bool isHookInstalled() const {
        return this->hook.isInstalled();
    }

    void enable() {
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
            this->anglesPitch, this->anglesYaw, this->anglesRoll,
            this->photoFov
        );

        this->enabled = true;
        DEBUG_LOG("[PhotoMode] pitch:" << (*this->cameraPitch) << " yaw: " << (*this->cameraYaw));
    }

    void disable() {
        if (!this->enabled) return;

        // Deactivate hook override - game regains camera control
        this->hook.deactivateOverride();

        // Restore time
        *this->timeFactor = this->originalTimeFactor;

        this->enabled = false;
        DEBUG_LOG("[PhotoMode] disabled");
    }

    void toggle() {
        if (this->enabled) {
            this->disable();
        }
        else {
            this->enable();
        }
    }

    // Adjust camera position in world space
    // Parameters use XZY order to match key bindings
    void adjustPosition(float dx, float dz, float dy) {
        if (!this->enabled) return;

        this->photoX += dx;
        this->photoY += dy;
        this->photoZ += dz;

        this->pushStateToHook();
    }

    void adjustAngle(float dx, float dz, float dy) {
        if (!this->enabled) return;

        this->anglesPitch += dx;
        this->anglesYaw += dy;
        this->anglesRoll += dz;

        this->hook.setOverrideAngles(this->anglesPitch, this->anglesYaw, this->anglesRoll);
    }

    void adjustFov(float delta) {
        if (!this->enabled) return;

        this->photoFov += delta;
        this->hook.setOverrideFov(this->photoFov);
    }

    // these methods could be private.
    bool isSafeToHook() {
        return this->safeToHook;
    }

    void checkSafeToHook() {
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
        DEBUG_LOG("[PhotoMode] " << (this->enabled ? "on" : "off")
            << " Pos(" << this->photoX << ", " << this->photoY << ", " << this->photoZ << ")"
            << " Angles(" << this->anglesPitch << ", " << this->anglesYaw << ", " << this->anglesRoll << ")"
            << " Fov: " << this->photoFov);
        DEBUG_LOG("[PhotoMode] Hook installed: " << (this->hook.isInstalled() ? "yes" : "no")
            << " Override active: " << (this->hook.isOverrideActive() ? "yes" : "no"));
    }
};