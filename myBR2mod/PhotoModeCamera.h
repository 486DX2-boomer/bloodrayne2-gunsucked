#pragma once
#include <Windows.h>
#include "Config.h"

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
        cameraX = (float*)GameAddresses::CameraX;
        cameraZ = (float*)GameAddresses::CameraZ;
        cameraY = (float*)GameAddresses::CameraY;
        cameraPitch = (float*)GameAddresses::CameraPitch;
        cameraYaw = (float*)GameAddresses::CameraYaw;
        fov = (float*)GameAddresses::CameraFOV;
        cameraMode = (int*)GameAddresses::CameraMode;

        pushCamera = (bool*)GameAddresses::PushCamera;
        timeFactor = (float*)GameAddresses::TimeFactor;
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

        // these are constrained 0-22 by the game. 
        // We could check to see if we go over 22, but the game overwrites it for us. appears safe
        *cameraMode += 1;
        DEBUG_LOG("Camera mode: " << *cameraMode);
    }

    // Debug output
    void PrintState() const {
        DEBUG_LOG("Photo mode: " << (enabled ? "on" : "off")
            << " Pos(" << *cameraX << ", " << *cameraZ << ", " << *cameraY << ")"
            << " FOV: " << *fov
            << " mode: " << *cameraMode);
    }
};