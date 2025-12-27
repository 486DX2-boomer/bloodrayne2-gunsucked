#include "pch.h"
#include <Windows.h>
#include <stdio.h>
#include <vector>
#include "Config.h"
#include "InputBase.h"
#include "KeyInput.h"
#include "MouseInput.h"
#include "PhotoModeCamera.h"
#include "NoHud.h"
#include "SuperSlowMode.h"
#include "GunBalance.h"
#include "GunKeys.h"

void setupConsole() {
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
bool WaitForGameReady(PhotoModeCamera& photoMode, NoHud& noHud, GunKeys& gunKeys, int pollIntervalMs = 500) {
    DEBUG_LOG("[DLL] Waiting for game init...");

    int elapsed = 0;

    bool cameraHookSafe = false;
    bool hudHookSafe = false;
    bool gunKeysSafe = false;

    while (true) {
        photoMode.checkSafeToHook();
        if (photoMode.isSafeToHook()) {
            DEBUG_LOG("Photo mode ready after " << elapsed << "ms");
            cameraHookSafe = true;
        }

        noHud.checkSafeToHook();
        if (noHud.isSafeToHook()) {
            DEBUG_LOG("NoHud ready after " << elapsed << "ms");
            hudHookSafe = true;
        }

        gunKeys.checkSafe();
        if (gunKeys.isSafe()) {
            DEBUG_LOG("GunKeys ready");
            gunKeysSafe = true;
        }

        if (cameraHookSafe && hudHookSafe && gunKeysSafe) {
            DEBUG_LOG("Hooks ready after " << elapsed << "ms");
            return true;
        }

        Sleep(pollIntervalMs);
        elapsed += pollIntervalMs;
    }

    // we removed the timeout check
    //DEBUG_LOG("[DLL] Timeout waiting for game after " << timeoutMs << "ms");
    return false;
}

DWORD WINAPI MainThread(LPVOID param) {

    PhotoModeCamera photoMode;
    SuperSlowMode superSlowMode;
    NoHud noHud;
    GunBalance gunBalance;
    GunKeys gunKeys;

    // Gunbalance must be hooked immediately or else it will override values too late to work.
    if (!gunBalance.installHook()) {
        DEBUG_LOG("Failed to install gun balance hook - aborting");
        return 1;
    }

    // Wait for game to initialize before installing these
    if (!WaitForGameReady(photoMode, noHud, gunKeys)) {
        DEBUG_LOG("[DLL] Game validation failed - aborting");
        return 1;
    }

    // Install the camera hook
    if (!photoMode.installHook()) {
        DEBUG_LOG("[DLL] Failed to install camera hook - aborting");
        return 1;
    }

    // Install NoHud hook
    if (!noHud.installHook()) {
        DEBUG_LOG("Failed to install no hud hook - aborting");
        return 1;
    }
    // capture NoHud for photomode
    photoMode.captureNoHudRef(&noHud);
    // capture superslow for photomode
    photoMode.captureSuperSlowRef(&superSlowMode);

    // Input list and callbacks
    std::vector<std::unique_ptr<InputBase>> inputs;

    // Debug key - no callback required, handle manually
    KeyInput debugCheckKey(DEBUG_CHECK_KEY, true);

    // Photo mode toggle
    inputs.push_back(std::make_unique<KeyInput>(TOGGLE_PHOTO_MODE_KEY, true, [&photoMode]() {
        photoMode.toggle();
        }));

    // Camera position adjustments (XZY order)
    inputs.push_back(std::make_unique<KeyInput>(DECREMENT_X_KEY, false, [&photoMode]() {
        photoMode.adjustPosition(-CAMERA_POS_INCREMENT_DECREMENT_VALUE, 0, 0);
        }));
    inputs.push_back(std::make_unique<KeyInput>(INCREMENT_X_KEY, false, [&photoMode]() {
        photoMode.adjustPosition(CAMERA_POS_INCREMENT_DECREMENT_VALUE, 0, 0);
        }));
    inputs.push_back(std::make_unique<KeyInput>(DECREMENT_Z_KEY, false, [&photoMode]() {
        photoMode.adjustPosition(0, -CAMERA_POS_INCREMENT_DECREMENT_VALUE, 0);
        }));
    inputs.push_back(std::make_unique<KeyInput>(INCREMENT_Z_KEY, false, [&photoMode]() {
        photoMode.adjustPosition(0, CAMERA_POS_INCREMENT_DECREMENT_VALUE, 0);
        }));
    inputs.push_back(std::make_unique<KeyInput>(DECREMENT_Y_KEY, false, [&photoMode]() {
        photoMode.adjustPosition(0, 0, -CAMERA_POS_INCREMENT_DECREMENT_VALUE);
        }));
    inputs.push_back(std::make_unique<KeyInput>(INCREMENT_Y_KEY, false, [&photoMode]() {
        photoMode.adjustPosition(0, 0, CAMERA_POS_INCREMENT_DECREMENT_VALUE);
        }));

    // Target position adjustments (XZY order)
    inputs.push_back(std::make_unique<KeyInput>(DECREMENT_ANGLE_PITCH_KEY, false, [&photoMode]() {
        photoMode.adjustAngle(-ANGLE_INCREMENT_DECREMENT_VALUE, 0, 0);
        }));
    inputs.push_back(std::make_unique<KeyInput>(INCREMENT_ANGLE_PITCH_KEY, false, [&photoMode]() {
        photoMode.adjustAngle(ANGLE_INCREMENT_DECREMENT_VALUE, 0, 0);
        }));
    inputs.push_back(std::make_unique<KeyInput>(DECREMENT_ANGLE_ROLL_KEY, false, [&photoMode]() {
        photoMode.adjustAngle(0, -ANGLE_INCREMENT_DECREMENT_VALUE, 0);
        }));
    inputs.push_back(std::make_unique<KeyInput>(INCREMENT_ANGLE_ROLL_KEY, false, [&photoMode]() {
        photoMode.adjustAngle(0, ANGLE_INCREMENT_DECREMENT_VALUE, 0);
        }));
    inputs.push_back(std::make_unique<KeyInput>(DECREMENT_ANGLE_YAW_KEY, false, [&photoMode]() {
        photoMode.adjustAngle(0, 0, -ANGLE_INCREMENT_DECREMENT_VALUE);
        }));
    inputs.push_back(std::make_unique<KeyInput>(INCREMENT_ANGLE_YAW_KEY, false, [&photoMode]() {
        photoMode.adjustAngle(0, 0, ANGLE_INCREMENT_DECREMENT_VALUE);
        }));

    // FOV adjustments
    inputs.push_back(std::make_unique<KeyInput>(DECREMENT_FOV_KEY, false, [&photoMode]() {
        photoMode.adjustFov(-FOV_INCREMENT_DECREMENT_VALUE);
        }));
    inputs.push_back(std::make_unique<KeyInput>(INCREMENT_FOV_KEY, false, [&photoMode]() {
        photoMode.adjustFov(FOV_INCREMENT_DECREMENT_VALUE);
        }));

    // Super slow toggle
    inputs.push_back(std::make_unique<KeyInput>(TOGGLE_SUPER_SLOW_MODE_KEY, true, [&superSlowMode]() {
        superSlowMode.toggle();
        }));

    // No hud
    inputs.push_back(std::make_unique<KeyInput>(TOGGLE_HUD_KEY, true, [&noHud]() {
        noHud.toggle();
        }));

    // gun keys
    inputs.push_back(std::make_unique<KeyInput>(GUN_SELECT_BLOODSHOT_KEY, true, [&gunKeys]() {
        gunKeys.switchWeapon(GunKeys::WeaponModes::BloodShot);
        }));
    inputs.push_back(std::make_unique<KeyInput>(GUN_SELECT_BLOODSTREAM_KEY, true, [&gunKeys]() {
        gunKeys.switchWeapon(GunKeys::WeaponModes::BloodStream);
        }));
    inputs.push_back(std::make_unique<KeyInput>(GUN_SELECT_BLOODSPRAY_KEY, true, [&gunKeys]() {
        gunKeys.switchWeapon(GunKeys::WeaponModes::BloodSpray);
        }));
    inputs.push_back(std::make_unique<KeyInput>(GUN_SELECT_BLOODBOMB_KEY, true, [&gunKeys]() {
        gunKeys.switchWeapon(GunKeys::WeaponModes::BloodBomb);
        }));
    inputs.push_back(std::make_unique<KeyInput>(GUN_SELECT_BLOODFLAME_KEY, true, [&gunKeys]() {
        gunKeys.switchWeapon(GunKeys::WeaponModes::BloodFlame);
        }));
    inputs.push_back(std::make_unique<KeyInput>(GUN_SELECT_BLOODHAMMER_KEY, true, [&gunKeys]() {
        gunKeys.switchWeapon(GunKeys::WeaponModes::BloodHammer);
        }));

    // used if mousewheel down isn't enabled
    inputs.push_back(std::make_unique<KeyInput>(GUN_SELECT_PREVIOUS_WEAPON_KEY, true, [&gunKeys]() {
        gunKeys.switchToPreviousWeapon();
        }));

#if ENABLE_MOUSEWHEEL_DOWN_PREVIOUS_WEAPON
    auto mouseInput = std::make_unique<MouseInput>([&gunKeys]() {
        gunKeys.switchToPreviousWeapon();
        });

    if (mouseInput->initialize()) {
        inputs.push_back(std::move(mouseInput));
    }
#endif

    DEBUG_LOG("[DLL] Starting hook. Press F7 to toggle photo mode\nPress F8 to toggle super slow mode\nPress F9 to toggle no HUD");

    while (true) {

        // Handle debug key manually (no callback)
        if (debugCheckKey.isActivated()) {
            photoMode.PrintState();
        }

        // Process inputs with callbacks
        for (auto& input : inputs) {
            input->checkAndExecute();
        }

//#ifdef ENABLE_MOUSEWHEEL_DOWN_PREVIOUS_WEAPON
//        mouseHandler.poll();
//        if (mouseHandler.wasMouseWheelDown()) {
//            DEBUG_LOG("Mouse wheel down - swap to previous weapon");
//            gunKeys.switchToPreviousWeapon();
//        }
//#endif

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
        setupConsole();
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