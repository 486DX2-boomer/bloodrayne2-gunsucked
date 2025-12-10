// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <Windows.h>
#include <stdio.h>
#include <vector>
#include "Config.h"
#include "KeyInput.h"
#include "PhotoModeCamera.h"
#include "SuperSlowMode.h"

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

    PhotoModeCamera photoMode;
    SuperSlowMode superSlowMode;

    // Build input list with callbacks
    std::vector<KeyInput> inputs;

    // Debug key - no callback, handled manually
    KeyInput debugCheckKey(DEBUG_CHECK_KEY, true);

    // Photo mode toggle
    inputs.push_back(KeyInput(TOGGLE_PHOTO_MODE_KEY, true, [&photoMode]() {
        photoMode.Toggle();
        }));

    // Position adjustments (XZY order)
    inputs.push_back(KeyInput(DECREMENT_X_KEY, false, [&photoMode]() {
        photoMode.AdjustPosition(-CAMERA_POS_INCREMENT_DECREMENT_VALUE, 0, 0);
        }));
    inputs.push_back(KeyInput(INCREMENT_X_KEY, false, [&photoMode]() {
        photoMode.AdjustPosition(CAMERA_POS_INCREMENT_DECREMENT_VALUE, 0, 0);
        }));
    inputs.push_back(KeyInput(DECREMENT_Z_KEY, false, [&photoMode]() {
        photoMode.AdjustPosition(0, -CAMERA_POS_INCREMENT_DECREMENT_VALUE, 0);
        }));
    inputs.push_back(KeyInput(INCREMENT_Z_KEY, false, [&photoMode]() {
        photoMode.AdjustPosition(0, CAMERA_POS_INCREMENT_DECREMENT_VALUE, 0);
        }));
    inputs.push_back(KeyInput(DECREMENT_Y_KEY, false, [&photoMode]() {
        photoMode.AdjustPosition(0, 0, -CAMERA_POS_INCREMENT_DECREMENT_VALUE);
        }));
    inputs.push_back(KeyInput(INCREMENT_Y_KEY, false, [&photoMode]() {
        photoMode.AdjustPosition(0, 0, CAMERA_POS_INCREMENT_DECREMENT_VALUE);
        }));

    // FOV adjustments
    inputs.push_back(KeyInput(DECREMENT_FOV_KEY, false, [&photoMode]() {
        photoMode.AdjustFOV(-FOV_INCREMENT_DECREMENT_VALUE);
        }));
    inputs.push_back(KeyInput(INCREMENT_FOV_KEY, false, [&photoMode]() {
        photoMode.AdjustFOV(FOV_INCREMENT_DECREMENT_VALUE);
        }));

    // Mode cycling
    inputs.push_back(KeyInput(CAMERA_CHANGE_MODE_KEY, true, [&photoMode]() {
        photoMode.CycleMode();
        }));

    // Superslow toggle
    inputs.push_back(KeyInput(TOGGLE_SUPER_SLOW_MODE_KEY, true, [&superSlowMode]() {
        superSlowMode.toggle();
        }));

    while (true) {

        // Handle debug key manually (no callback)
        if (debugCheckKey.IsActivated()) {
            photoMode.PrintState();
        }

        // Process all inputs with callbacks
        for (auto& input : inputs) {
            input.CheckAndExecute();
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