// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <Windows.h>
#include <string>
#include <stdio.h>
#include <iostream>
#include "KeyInput.h"

#define DECREMENT_X_KEY VK_OEM_4 // [
#define INCREMENT_X_KEY VK_OEM_6 // ]
#define DECREMENT_Z_KEY VK_OEM_COMMA // ,
#define INCREMENT_Z_KEY VK_OEM_PERIOD // .
#define DECREMENT_Y_KEY VK_OEM_1 // ;
#define INCREMENT_Y_KEY VK_OEM_7 // '

#define CAMERA_POS_INCREMENT_DECREMENT_VALUE 1.0f

#define DECREMENT_FOV_KEY VK_NEXT // page down
#define INCREMENT_FOV_KEY VK_PRIOR // page up

#define FOV_INCREMENT_DECREMENT_VALUE 5.0f

#define CAMERA_CHANGE_MODE_KEY VK_OEM_PLUS // =+

#define TOGGLE_PHOTO_MODE_KEY VK_F7

#define DEBUG_CONSOLE_ENABLED true

typedef struct cameraProps {
    bool photoModeEnabled;

    // whether the camera should be "pushed" so it doesn't go out of bounds
    // false for push, true for no push
    bool* pushCamera;

    // proposal: struct with a Current and Original field, to make it easier to restore properties

    float* cameraX;
    float* cameraZ;
    float* cameraY;
    float* cameraPitch;
    float* cameraYaw;
    int* cameraMode; // we do not know what mode is what, but there appear to be 23 modes, 0-22. This could be bit flags and not an int.

    float* fov; // sensible values are 1-80 but can be set to any value

    // values to restore on photo mode off
    float originalX;
    float originalZ;
    float originalY;
    float originalPitch;
    float originalYaw;

    float originalTimeFactor; // for restoring on photomode exit

    float originalFov;

    int originalCameraMode;
};

void togglePhotoMode(cameraProps* camProps, float* gameTimeFactor) {

    // WARNING: there needs to be a check to make sure the
    // game is loaded/we're not at a menu/etc!

    if (!camProps->photoModeEnabled) {
        // save original values current values
        camProps->originalX = *camProps->cameraX;
        camProps->originalZ = *camProps->cameraZ;
        camProps->originalY = *camProps->cameraY;
        camProps->originalPitch = *camProps->cameraPitch;
        camProps->originalYaw = *camProps->cameraYaw;

        camProps->originalCameraMode = *camProps->cameraMode;

        camProps->originalFov = *camProps->fov;

        // Save and freeze time factor
        camProps->originalTimeFactor = *gameTimeFactor;
        *gameTimeFactor = 0.0001f; // use a very small value- setting to 0.0 overflows the value to negative for some reason

        // Set camera mode
        camProps->originalCameraMode = *camProps->cameraMode;
        *camProps->cameraMode = 1; // we thought 26 (0x1a) was for fixed camera, but setting it just results in the value being 0.
        // It might be helpful to have a key for toggling through camera modes too. Possible values 0-21

        // Disable push camera
        *camProps->pushCamera = true; // true for DISABLED push

        camProps->photoModeEnabled = true;
        std::cout << "photo mode on" << std::endl;

        //std::cout << "origX: "
        //    << std::to_string(camProps->originalX)
        //    << " origZ: "
        //    << std::to_string(camProps->originalZ)
        //    << " origY: "
        //    << std::to_string(camProps->originalY)
        //    << std::endl;
    }
    else {
        // Exiting photo mode - restore original values
        *camProps->cameraX = camProps->originalX;
        *camProps->cameraZ = camProps->originalZ;
        *camProps->cameraY = camProps->originalY;
        *camProps->cameraPitch = camProps->originalPitch;
        *camProps->cameraYaw = camProps->originalYaw;

        // Restore time factor
        *gameTimeFactor = camProps->originalTimeFactor;
        // Restore camera mode
        *camProps->cameraMode = camProps->originalCameraMode;
        // Re-enable push camera
        *camProps->pushCamera = false; // false for ENABLED push

        *camProps->fov = camProps->originalFov;

        camProps->photoModeEnabled = false;
        std::cout << "photo mode off" << std::endl;

        //std::cout << "restoredX: "
        //    << std::to_string(camProps->originalX)
        //    << " restoredZ: "
        //    << std::to_string(camProps->originalZ)
        //    << " restoredY: "
        //    << std::to_string(camProps->originalY)
        //    << std::endl;
    }
}

void SetupConsole() {
    // Allocate a new console
    if (!AllocConsole()) {
        MessageBoxA(nullptr, "Failed to allocate console.", "Error", MB_OK | MB_ICONERROR);
        return;
    }

    // Redirect STDOUT
    FILE* fpOut;
    if (freopen_s(&fpOut, "CONOUT$", "w", stdout) != 0) {
        MessageBoxA(nullptr, "Failed to redirect stdout.", "Error", MB_OK | MB_ICONERROR);
        return;
    }

    //// Redirect STDIN
    //FILE* fpIn;
    //if (freopen_s(&fpIn, "CONIN$", "r", stdin) != 0) {
    //    MessageBoxA(nullptr, "Failed to redirect stdin.", "Error", MB_OK | MB_ICONERROR);
    //    return;
    //}

    //// Redirect STDERR
    //FILE* fpErr;
    //if (freopen_s(&fpErr, "CONOUT$", "w", stderr) != 0) {
    //    MessageBoxA(nullptr, "Failed to redirect stderr.", "Error", MB_OK | MB_ICONERROR);
    //    return;
    //}

    // Test output
    std::cout << "[DLL] Console allocated successfully" << std::endl;
    //printf("[DLL] printf works too.\n");
}

DWORD WINAPI MainThread(LPVOID param) {

    cameraProps camProps;

    float* gameTimeFactor = (float*)0x619FB68;
    // heads up: move this to its own setup function
    // or, make camProps a class.
    camProps.photoModeEnabled = false;
    camProps.pushCamera = (bool*)0x05e3473d; // false for push, true for no push
    camProps.cameraX = (float*)0x05E34EE4;
    camProps.cameraZ = (float*)0x05E34EE8;
    camProps.cameraY = (float*)0x05e34EEC;
    camProps.cameraPitch = (float*)0x05E34EF0;
    camProps.cameraYaw = (float*)0x05E34EF4;
    camProps.cameraMode = (int*)0x05E350F4;
    camProps.fov = (float*)0x05E34EFC;

    KeyInput photoModeToggleKey(TOGGLE_PHOTO_MODE_KEY, true);
    KeyInput debugCheckKey(VK_F6, true);

    KeyInput decrementX(DECREMENT_X_KEY, false);
    KeyInput incrementX(INCREMENT_X_KEY, false);
    KeyInput decrementZ(DECREMENT_Z_KEY, false);
    KeyInput incrementZ(INCREMENT_Z_KEY, false);
    KeyInput decrementY(DECREMENT_Y_KEY, false);
    KeyInput incrementY(INCREMENT_Y_KEY, false);
    KeyInput decrementFov(DECREMENT_FOV_KEY, false);
    KeyInput incrementFov(INCREMENT_FOV_KEY, false);
    KeyInput changeCameraMode(CAMERA_CHANGE_MODE_KEY, true);

    // proposal: add a third argument to KeyInput which is a pointer to a method, binding that method to the keypress

    while (true) {

        if (debugCheckKey.IsActivated()) {
            std::cout << "Debug: debug check" << std::endl;
        }

        if (photoModeToggleKey.IsActivated()) {
            // note: need a photo mode class instead of this naive handling
            togglePhotoMode(&camProps, gameTimeFactor);
        }

        if (camProps.photoModeEnabled) {
            if (decrementX.IsActivated()) {
                *camProps.cameraX -= CAMERA_POS_INCREMENT_DECREMENT_VALUE;
            }
            if (incrementX.IsActivated()) {
                *camProps.cameraX += CAMERA_POS_INCREMENT_DECREMENT_VALUE;
            }

            if (decrementZ.IsActivated()) {
                *camProps.cameraZ -= CAMERA_POS_INCREMENT_DECREMENT_VALUE;
            }
            if (incrementZ.IsActivated()) {
                *camProps.cameraZ += CAMERA_POS_INCREMENT_DECREMENT_VALUE;
            }

            if (decrementY.IsActivated()) {
                *camProps.cameraY -= CAMERA_POS_INCREMENT_DECREMENT_VALUE;
            }
            if (incrementY.IsActivated()) {
                *camProps.cameraY += CAMERA_POS_INCREMENT_DECREMENT_VALUE;
            }

            if (decrementFov.IsActivated()) {
                *camProps.fov -= FOV_INCREMENT_DECREMENT_VALUE;
            }

            if (incrementFov.IsActivated()) {
                *camProps.fov += FOV_INCREMENT_DECREMENT_VALUE;
            }

            if (changeCameraMode.IsActivated()) {
                // cameraMode is constrained from values 0-22
                // We could add a check here to make sure we don't go out of bounds, but the game does this for us.
                *camProps.cameraMode += 1;
            }
        }

        Sleep(16);
    }
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
) {

    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:

        if (DEBUG_CONSOLE_ENABLED) {
            SetupConsole();
        }
        std::cout << "Mod injected successfully" << std::endl;

        // disable DllMain call on thread attach/detach events:
        DisableThreadLibraryCalls(hModule);

        CreateThread(0, 0, MainThread, hModule, 0, 0);

        break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

