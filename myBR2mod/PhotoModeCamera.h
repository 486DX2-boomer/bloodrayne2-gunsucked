#pragma once
#include <Windows.h>
#include <iostream>

#define DEBUG_CONSOLE_ENABLED true

#if DEBUG_CONSOLE_ENABLED
#define DEBUG_LOG(msg) std::cout << msg << std::endl
#else
#define DEBUG_LOG(msg) ((void)0)
#endif

class PhotoModeCamera {
private:
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

    // Original values for restoration (XZY order)
    float originalX;
    float originalZ;
    float originalY;
    float originalPitch;
    float originalYaw;
    float originalFov;
    int originalCameraMode;
    float originalTimeFactor;

    bool enabled;

public:
    PhotoModeCamera()
        : enabled(false),
        originalX(0), originalZ(0), originalY(0),
        originalPitch(0), originalYaw(0), originalFov(0),
        originalCameraMode(0), originalTimeFactor(1.0f)
    {
        // Base address: 0x05E34EE0 (CGameView)
        // Offsets from our reverse engineering (XZY order):
        // +0x004 = cameraX
        // +0x008 = cameraZ
        // +0x00C = cameraY
        // +0x010 = cameraPitch (target X)
        // +0x014 = cameraYaw (target Y)
        // +0x01C = FOV
        // +0x214 = cameraMode

        constexpr uintptr_t cameraBase = 0x05E34EE0;

        cameraX = (float*)(cameraBase + 0x004);
        cameraZ = (float*)(cameraBase + 0x008);
        cameraY = (float*)(cameraBase + 0x00C);
        cameraPitch = (float*)(cameraBase + 0x010);
        cameraYaw = (float*)(cameraBase + 0x014);
        fov = (float*)(cameraBase + 0x01C);
        cameraMode = (int*)(cameraBase + 0x214);

        pushCamera = (bool*)(0x05E3473D);
        timeFactor = (float*)(0x0619FB68);
    }

    bool IsEnabled() const {
        return enabled;
    }

    void Enable() {
        if (enabled) return;

        // Capture current values
        originalX = *cameraX;
        originalZ = *cameraZ;
        originalY = *cameraY;
        originalPitch = *cameraPitch;
        originalYaw = *cameraYaw;
        originalFov = *fov;
        originalCameraMode = *cameraMode;
        originalTimeFactor = *timeFactor;

        // Freeze time (small value to avoid overflow issues)
        *timeFactor = 0.0001f;

        // Set camera mode (mode 1 for now - may need adjustment)
        *cameraMode = 1;

        // Disable camera push/bounds checking
        *pushCamera = true;

        enabled = true;
        DEBUG_LOG("Photo mode enabled");
    }

    void Disable() {
        if (!enabled) return;

        // Restore all original values
        *cameraX = originalX;
        *cameraZ = originalZ;
        *cameraY = originalY;
        *cameraPitch = originalPitch;
        *cameraYaw = originalYaw;
        *fov = originalFov;
        *cameraMode = originalCameraMode;
        *timeFactor = originalTimeFactor;

        // Re-enable camera push
        *pushCamera = false;

        enabled = false;
        DEBUG_LOG("Photo mode disabled");
    }

    void Toggle() {
        if (enabled) {
            Disable();
        }
        else {
            Enable();
        }
    }

    void AdjustPosition(float dx, float dz, float dy) {
        if (!enabled) return;

        *cameraX += dx;
        *cameraZ += dz;
        *cameraY += dy;
    }

    void AdjustFOV(float delta) {
        if (!enabled) return;

        *fov += delta;
    }

    void CycleMode() {
        if (!enabled) return;

        // Modes are constrained 0-22 by the game
        *cameraMode += 1;
        DEBUG_LOG("Camera mode: " << *cameraMode);
    }

    // Debug output
    void PrintState() const {
        DEBUG_LOG("PhotoMode: " << (enabled ? "ON" : "OFF")
            << " Pos(" << *cameraX << ", " << *cameraZ << ", " << *cameraY << ")"
            << " FOV: " << *fov
            << " Mode: " << *cameraMode);
    }
};