// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <Windows.h>
#include <stdio.h>
#include <vector>
#include "Config.h"
#include "KeyInput.h"
#include "PhotoModeCamera.h"

// design of super slow mode feature needs a lot of work, disabled for now
//#include "SuperSlowMode.h"

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

// Wait for game to be ready before hooking
bool WaitForGameReady(PhotoModeCamera& photoMode, DWORD timeoutMs = 30000, DWORD pollIntervalMs = 500) {
    DEBUG_LOG("[DLL] Waiting for game to initialize...");

    DWORD elapsed = 0;
    while (elapsed < timeoutMs) {
        photoMode.CheckSafeToHook();
        if (photoMode.IsSafeToHook()) {
            DEBUG_LOG("[DLL] Game ready after " << elapsed << "ms");
            return true;
        }
        Sleep(pollIntervalMs);
        elapsed += pollIntervalMs;
    }

    DEBUG_LOG("[DLL] Timeout waiting for game after " << timeoutMs << "ms");
    return false;
}

DWORD WINAPI MainThread(LPVOID param) {

    PhotoModeCamera photoMode;
    //SuperSlowMode superSlowMode;

    // Wait for game to initialize before installing hook
    if (!WaitForGameReady(photoMode)) {
        DEBUG_LOG("[DLL] Game validation failed - aborting");
        return 1;
    }

    // Install the camera hook
    if (!photoMode.InstallHook()) {
        DEBUG_LOG("[DLL] Failed to install camera hook - aborting");
        return 1;
    }

    // Build input list with callbacks
    std::vector<KeyInput> inputs;

    // Debug key - no callback, handled manually
    KeyInput debugCheckKey(DEBUG_CHECK_KEY, true);

    // Photo mode toggle
    inputs.push_back(KeyInput(TOGGLE_PHOTO_MODE_KEY, true, [&photoMode]() {
        photoMode.Toggle();
        }));

    // Camera position adjustments (XZY order)
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

    // Target position adjustments (XZY order)
    inputs.push_back(KeyInput(DECREMENT_TARGET_X_KEY, false, [&photoMode]() {
        photoMode.AdjustTarget(-TARGET_POS_INCREMENT_DECREMENT_VALUE, 0, 0);
        }));
    inputs.push_back(KeyInput(INCREMENT_TARGET_X_KEY, false, [&photoMode]() {
        photoMode.AdjustTarget(TARGET_POS_INCREMENT_DECREMENT_VALUE, 0, 0);
        }));
    inputs.push_back(KeyInput(DECREMENT_TARGET_Z_KEY, false, [&photoMode]() {
        photoMode.AdjustTarget(0, -TARGET_POS_INCREMENT_DECREMENT_VALUE, 0);
        }));
    inputs.push_back(KeyInput(INCREMENT_TARGET_Z_KEY, false, [&photoMode]() {
        photoMode.AdjustTarget(0, TARGET_POS_INCREMENT_DECREMENT_VALUE, 0);
        }));
    inputs.push_back(KeyInput(DECREMENT_TARGET_Y_KEY, false, [&photoMode]() {
        photoMode.AdjustTarget(0, 0, -TARGET_POS_INCREMENT_DECREMENT_VALUE);
        }));
    inputs.push_back(KeyInput(INCREMENT_TARGET_Y_KEY, false, [&photoMode]() {
        photoMode.AdjustTarget(0, 0, TARGET_POS_INCREMENT_DECREMENT_VALUE);
        }));

    // FOV adjustments
    inputs.push_back(KeyInput(DECREMENT_FOV_KEY, false, [&photoMode]() {
        photoMode.AdjustFOV(-FOV_INCREMENT_DECREMENT_VALUE);
        }));
    inputs.push_back(KeyInput(INCREMENT_FOV_KEY, false, [&photoMode]() {
        photoMode.AdjustFOV(FOV_INCREMENT_DECREMENT_VALUE);
        }));

    // Super slow toggle
    //inputs.push_back(KeyInput(TOGGLE_SUPER_SLOW_MODE_KEY, true, [&superSlowMode]() {
    //    superSlowMode.toggle();
    //    }));

    DEBUG_LOG("[DLL] Main loop starting - Press F7 to toggle photo mode");

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
        DEBUG_LOG("[DLL] Mod injected successfully");

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