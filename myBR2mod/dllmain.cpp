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
#include "DisplayMessage.h"
#include "PlaySound.h"
#include "Outfit.h"
//#include "CasualMode.h"

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
bool WaitForGameReady(PhotoModeCamera& photoMode, NoHud& noHud, GunKeys& gunKeys, Outfit& outfit, int pollIntervalMs = 500) {
    DEBUG_LOG("[DLL] Waiting for game init...");

    int elapsed = 0;

    bool cameraHookSafe = false;
    bool hudHookSafe = false;
    bool gunKeysSafe = false;
    //bool outfitReady = false;

    while (true) {
        photoMode.checkSafeToHook();
        if (photoMode.isSafeToHook()) {
            //DEBUG_LOG("Photo mode ready after " << elapsed << "ms");
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

        // hooks and enable loose file priorities are two different events
        // we don't need to wait for the hooks to be ready to swap the file handlers- we need to do it ASAP
        //if (!outfitReady && outfit.checkAndEnableLooseFiles()) {
        //    DEBUG_LOG("[Outfit] Handlers ready after " << elapsed << "ms");
        //    outfitReady = true;
        //}

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

    // check for gunsucked.ini
    Config config;
    if (!config.configExists()) {
        if (!config.writeDefaultConfig()) {
            DEBUG_LOG("Error: Couldn't write gunsucked.ini");
        }
    }

    // load values from config file

    // fallback if config doesn't load (display a warning)

    PhotoModeCamera photoMode;
    SuperSlowMode superSlowMode;
    NoHud noHud;
    GunBalance gunBalance;
    GunKeys gunKeys;
    Outfit outfit;
//#if ENABLE_CASUAL_MODE
//    CasualMode casualMode; // don't enable casualMode for release yet, not working properly
//#endif

    // Gunbalance must be hooked immediately or else it will override values too late to work.
    if (!gunBalance.installHook()) {
        DEBUG_LOG("Failed to install gun balance hook - aborting");
        return 1;
    }

    // need to review outfit and figure out what steps are what. what is initialize, what is enable. why do we scan directory separately
    // all of that could be condensed into one initialization function not a dozen
    if (!outfit.initialize()) {
        DEBUG_LOG("Failed to init outfit system");
    }

    // waits until the file handlers are swapped.
    // this should happen pretty quickly
    int outfitTimeout = 0;
    while (!outfit.checkAndEnableLooseFiles()) {
        Sleep(16);
        outfitTimeout += 1;

        // time out after 30 sec to ensure it doesn't silently hang here
        if (outfitTimeout >= 480000) {
            DEBUG_LOG("Outfit: WARNING: couldn't swap file handlers!");
            break;
        }
    }

    // Enable loose file priority (handlers are now ready)
    if (!outfit.checkAndEnableLooseFiles()) {
        DEBUG_LOG("[DLL] Failed to enable loose file priority");
        // Continuing won't break the game, just won't load custom assets
    }

    outfit.checkAndEnableLooseFiles(); // this shouldn't have been called 3 times already, gotta fix this
    outfit.scanDirectory("mods\\outfits");
    outfit.printStatus();

    // outfit hooks need to be installed before the first level load, independent of the other hooks
    // because the other hooks wait for Rayne's obj to be initialized, it's too late if we wait for them to be safe
    // Install Outfit hooks
    if (!outfit.installHooks()) {
        DEBUG_LOG("[DLL] Failed to install outfit hooks - aborting");
        return 1;
    }

    // Wait for game to initialize before installing these
    if (!WaitForGameReady(photoMode, noHud, gunKeys, outfit)) {
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
    
    // for testing the outfit index, remove this later
    // int* outfitIndex = (int*)0x5e339B4;
    // *outfitIndex = 0x0b; // 11

    while (true) {

//#if ENABLE_CASUAL_MODE
//        casualMode.update();
//#endif

        // Handles debug key manually (no callback)
        if (debugCheckKey.isActivated()) {

            //photoMode.PrintState();

            DisplayMessage message;
            message.boxedMessage("Hello from GunSucked mod!");

            // for testing the outfit index, remove this later
            // *outfitIndex += 1;
            // message.boxedMessage("overwriting outfit index");

            PlaySound sound;
            sound.confirm();
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