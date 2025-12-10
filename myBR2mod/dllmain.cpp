// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <Windows.h>
#include <stdio.h>
#include "Config.h"
#include "KeyInput.h"
#include "PhotoModeCamera.h"

void SetupConsole() {
    if (!AllocConsole()) {
        MessageBoxA(nullptr, "Failed to allocate console.", "Error", MB_OK | MB_ICONERROR);
        return;
    }

    FILE* fpOut;
    if (freopen_s(&fpOut, "CONOUT$", "w", stdout) != 0) {
        MessageBoxA(nullptr, "Failed to redirect stdout.", "Error", MB_OK | MB_ICONERROR);
        return;
    }

    DEBUG_LOG("[DLL] Console allocated successfully");
}

DWORD WINAPI MainThread(LPVOID param) {

    PhotoModeCamera camera;

    // Toggle keys (fire once per press)
    KeyInput photoModeToggleKey(TOGGLE_PHOTO_MODE_KEY, true);
    KeyInput debugCheckKey(DEBUG_CHECK_KEY, true);
    KeyInput changeCameraMode(CAMERA_CHANGE_MODE_KEY, true);

    // Continuous keys (fire while held)
    KeyInput decrementX(DECREMENT_X_KEY, false);
    KeyInput incrementX(INCREMENT_X_KEY, false);
    KeyInput decrementY(DECREMENT_Y_KEY, false);
    KeyInput incrementY(INCREMENT_Y_KEY, false);
    KeyInput decrementZ(DECREMENT_Z_KEY, false);
    KeyInput incrementZ(INCREMENT_Z_KEY, false);
    KeyInput decrementFov(DECREMENT_FOV_KEY, false);
    KeyInput incrementFov(INCREMENT_FOV_KEY, false);

    while (true) {

        if (debugCheckKey.IsActivated()) {
            camera.PrintState();
        }

        if (photoModeToggleKey.IsActivated()) {
            camera.Toggle();
        }

        if (camera.IsEnabled()) {
            // Position adjustments (XZY order)
            if (decrementX.IsActivated()) {
                camera.AdjustPosition(-CAMERA_POS_INCREMENT_DECREMENT_VALUE, 0, 0);
            }
            if (incrementX.IsActivated()) {
                camera.AdjustPosition(CAMERA_POS_INCREMENT_DECREMENT_VALUE, 0, 0);
            }
            if (decrementZ.IsActivated()) {
                camera.AdjustPosition(0, -CAMERA_POS_INCREMENT_DECREMENT_VALUE, 0);
            }
            if (incrementZ.IsActivated()) {
                camera.AdjustPosition(0, CAMERA_POS_INCREMENT_DECREMENT_VALUE, 0);
            }
            if (decrementY.IsActivated()) {
                camera.AdjustPosition(0, 0, -CAMERA_POS_INCREMENT_DECREMENT_VALUE);
            }
            if (incrementY.IsActivated()) {
                camera.AdjustPosition(0, 0, CAMERA_POS_INCREMENT_DECREMENT_VALUE);
            }

            // FOV adjustments
            if (decrementFov.IsActivated()) {
                camera.AdjustFOV(-FOV_INCREMENT_DECREMENT_VALUE);
            }
            if (incrementFov.IsActivated()) {
                camera.AdjustFOV(FOV_INCREMENT_DECREMENT_VALUE);
            }

            // Mode cycling
            if (changeCameraMode.IsActivated()) {
                camera.CycleMode();
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

#if DEBUG_CONSOLE_ENABLED
        SetupConsole();
#endif
        DEBUG_LOG("Mod injected successfully");

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