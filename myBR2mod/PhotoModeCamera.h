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

    bool IsEnabled() const {
        return this->enabled;
    }

    void Enable() {
        if (this->enabled) return;

        // Capture current values
        this->originalX = *this->cameraX;
        this->originalZ = *this->cameraZ;
        this->originalY = *this->cameraY;
        this->originalPitch = *this->cameraPitch;
        this->originalYaw = *this->cameraYaw;
        this->originalFov = *this->fov;
        this->originalCameraMode = *this->cameraMode;
        this->originalTimeFactor = *this->timeFactor;

        // Freeze time (small value to avoid overflow issues)
        *this->timeFactor = 0.0001f;

        // Set camera mode (mode 1 for now - may need adjustment)
        *this->cameraMode = 1;

        // Disable camera push/bounds checking
        *this->pushCamera = true;

        this->enabled = true;
        DEBUG_LOG("Photo mode enabled");
    }

    void Disable() {
        if (!this->enabled) return;

        // Restore all original values
        *this->cameraX = this->originalX;
        *this->cameraZ = this->originalZ;
        *this->cameraY = this->originalY;
        *this->cameraPitch = this->originalPitch;
        *this->cameraYaw = this->originalYaw;
        *this->fov = this->originalFov;
        *this->cameraMode = this->originalCameraMode;
        *this->timeFactor = this->originalTimeFactor;

        // Re-enable camera push
        *this->pushCamera = false;

        this->enabled = false;
        DEBUG_LOG("Photo mode disabled");
    }

    void Toggle() {
        if (this->enabled) {
            this->Disable();
        }
        else {
            this->Enable();
        }
    }

    void AdjustPosition(float dx, float dz, float dy) {
        if (!this->enabled) return;

        *this->cameraX += dx;
        *this->cameraZ += dz;
        *this->cameraY += dy;
    }

    void AdjustFOV(float delta) {
        if (!this->enabled) return;

        *this->fov += delta;
    }

    void CycleMode() {
        if (!this->enabled) return;

        // Modes are constrained 0-22 by the game
        *this->cameraMode += 1;
        DEBUG_LOG("Camera mode: " << *this->cameraMode);
    }

    // Debug output
    void PrintState() const {
        DEBUG_LOG("PhotoMode: " << (this->enabled ? "ON" : "OFF")
            << " Pos(" << *this->cameraX << ", " << *this->cameraZ << ", " << *this->cameraY << ")"
            << " FOV: " << *this->fov
            << " Mode: " << *this->cameraMode);
    }
};