#include "pch.h"
#include <Windows.h>
#include <stdio.h>
#include <vector>
#include <optional>
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
bool WaitForGameReady(
    std::optional<PhotoModeCamera>& photoMode, 
    NoHud &noHud, 
    std::optional<GunKeys>& gunKeys, 
    int pollIntervalMs = 500) {
    DEBUG_LOG("[DLL] Waiting for game init...");

    int elapsed = 0;

    // if the feature is disabled, then we set hooksafe to "true", which skips the hooking process
    // bool cameraHookSafe = false;
    bool cameraHookSafe = !photoMode.has_value();

    bool hudHookSafe = false;

    // bool gunKeysSafe = false;
    bool gunKeysSafe = !gunKeys.has_value();

    while (true) {
        if (photoMode && !cameraHookSafe) {
            // will have to review the hook install interface later and see if we can't collapse checkSafe and isSafe to one method
            photoMode->checkSafeToHook();
            if (photoMode->isSafeToHook()) {
                cameraHookSafe = true;
            }
        }

        if (!hudHookSafe) {
            noHud.checkSafeToHook();
            if (noHud.isSafeToHook()) {
                hudHookSafe = true;
            }
        }

        if (gunKeys && !gunKeysSafe) {
            gunKeys->checkSafe();
            if (gunKeys->isSafe()) {
                gunKeysSafe = true;
            }
        }

        if (cameraHookSafe && hudHookSafe && gunKeysSafe) {
            DEBUG_LOG("All hooks safe to install");
            return true;
        }

        Sleep(pollIntervalMs);
        elapsed += pollIntervalMs;
    }

    return false;
}

// global configuration object
Config g_Config;

DWORD WINAPI MainThread(LPVOID param) {

    // check for gunsucked.ini
    if (!g_Config.configExists()) {
        if (!g_Config.writeDefaultConfig()) {
            DEBUG_LOG("Error: Couldn't write gunsucked.ini");
        }
    }

    // load values from config file
    if (!g_Config.loadConfig()) {
        // fallback if config doesn't load
        DEBUG_LOG("Error: Couldn't load gunsucked.ini");
        // display a warning here (win32 message box so it doesn't fail silently)
    }
    else {
        DEBUG_LOG("Config loaded");
    }

    //config.logConfig();

    std::optional<PhotoModeCamera> photoMode;
    // Photo mode requires super slow mode to function, so super slow mode is never optional. If super slow mode is "disabled", then we just ignore the key binding for it.
    SuperSlowMode superSlowMode;
    // same for no hud.
    NoHud noHud;
    std::optional<GunBalance> gunBalance;
    std::optional<GunKeys> gunKeys;
    std::optional<Outfit> outfit;

    if (g_Config.enablePhotoMode) {
        photoMode.emplace();
    }
    if (g_Config.enableGunBalance) {
        gunBalance.emplace();
    }
    if (g_Config.enableGunKeys) {
        gunKeys.emplace();
    }
    if (g_Config.enableOutfitMods) {
        outfit.emplace();
    }

    // Gunbalance must be hooked immediately or else it will override values too late to work.
    if (gunBalance) {
        // load the config before hooking
        gunBalance->loadFromConfig();
        if (!gunBalance->installHook()) {
            DEBUG_LOG("Failed to install gun balance hook - aborting");
            return 1;
        }
    }

    // need to review outfit and figure out what steps are what. what is initialize, what is enable. why do we scan directory separately
    // all of that could be condensed into one initialization function not a dozen
    if (outfit) {
        if (!outfit->initialize()) {
            DEBUG_LOG("Failed to init outfit system");
        }
    }

    // waits until the file handlers are swapped.
    // this should happen pretty quickly
    if (outfit) {
        int outfitTimeout = 0;
        while (!outfit->checkAndEnableLooseFiles()) {
            Sleep(16);
            outfitTimeout += 1;

            // time out after 30 sec to ensure it doesn't silently hang here
            if (outfitTimeout >= 480000) {
                DEBUG_LOG("Outfit: WARNING: couldn't swap file handlers!");
                break;
            }
        }
    }

    // Enable loose file priority (handlers are now ready)
    if (outfit) {
        if (!outfit->checkAndEnableLooseFiles()) {
            DEBUG_LOG("[DLL] Failed to enable loose file priority");
            // Continuing won't break the game, just won't load custom assets
        }

        outfit->checkAndEnableLooseFiles(); // this shouldn't have been called 3 times already, gotta fix this
        outfit->scanDirectory("mods\\outfits");
        outfit->printStatus();

        // outfit hooks need to be installed before the first level load, independent of the other hooks
        // because the other hooks wait for Rayne's obj to be initialized, it's too late if we wait for them to be safe
        // Install Outfit hooks
        if (!outfit->installHooks()) {
            DEBUG_LOG("[DLL] Failed to install outfit hooks - aborting");
            // these returns will cause the dll to fail silently without the debug console enabled, we'll have to show a message box at some point
            return 1;
        }
    }
    
    // Wait for game to initialize before installing these
    if (!WaitForGameReady(photoMode, noHud, gunKeys)) {
        DEBUG_LOG("[DLL] Game validation failed - aborting");
        return 1;
    }

    // Install the camera hook
    if (photoMode) {
        if (!photoMode->installHook()) {
            DEBUG_LOG("[DLL] Failed to install camera hook - aborting");
            return 1;
        }
    }

    // Install NoHud hook
        if (!noHud.installHook()) {
            DEBUG_LOG("Failed to install no hud hook - aborting");
            return 1;
        }

    // capture NoHud for photomode
    if (photoMode) {
        photoMode->captureNoHudRef(&noHud);
        // capture superslow for photomode
        photoMode->captureSuperSlowRef(&superSlowMode);
    }

    // Input list and callbacks
    std::vector<std::unique_ptr<InputBase>> inputs;

    // Debug key - no callback required, handle manually
    // KeyInput debugCheckKey(DEBUG_CHECK_KEY, true);

    // Photo mode keys
    if (photoMode) {
        inputs.push_back(std::make_unique<KeyInput>(g_Config.togglePhotoModeKey, true, [&photoMode]() {
            photoMode->toggle();
            }));

        // Camera position adjustments (XZY order)
        inputs.push_back(std::make_unique<KeyInput>(g_Config.cameraDecrementXKey, false, [&photoMode]() {
            photoMode->adjustPosition(-g_Config.cameraPosIncDecValue, 0, 0);
            }));
        inputs.push_back(std::make_unique<KeyInput>(g_Config.cameraIncrementXKey, false, [&photoMode]() {
            photoMode->adjustPosition(g_Config.cameraPosIncDecValue, 0, 0);
            }));
        inputs.push_back(std::make_unique<KeyInput>(g_Config.cameraDecrementZKey, false, [&photoMode]() {
            photoMode->adjustPosition(0, -g_Config.cameraPosIncDecValue, 0);
            }));
        inputs.push_back(std::make_unique<KeyInput>(g_Config.cameraIncrementZKey, false, [&photoMode]() {
            photoMode->adjustPosition(0, g_Config.cameraPosIncDecValue, 0);
            }));
        inputs.push_back(std::make_unique<KeyInput>(g_Config.cameraDecrementYKey, false, [&photoMode]() {
            photoMode->adjustPosition(0, 0, -g_Config.cameraPosIncDecValue);
            }));
        inputs.push_back(std::make_unique<KeyInput>(g_Config.cameraIncrementYKey, false, [&photoMode]() {
            photoMode->adjustPosition(0, 0, g_Config.cameraPosIncDecValue);
            }));

        // Target position adjustments (XZY order)
        inputs.push_back(std::make_unique<KeyInput>(g_Config.cameraDecrementAnglePitchKey, false, [&photoMode]() {
            photoMode->adjustAngle(-g_Config.cameraAngleIncDecValue, 0, 0);
            }));
        inputs.push_back(std::make_unique<KeyInput>(g_Config.cameraIncrementAnglePitchKey, false, [&photoMode]() {
            photoMode->adjustAngle(g_Config.cameraAngleIncDecValue, 0, 0);
            }));
        inputs.push_back(std::make_unique<KeyInput>(g_Config.cameraDecrementAngleRollKey, false, [&photoMode]() {
            photoMode->adjustAngle(0, -g_Config.cameraAngleIncDecValue, 0);
            }));
        inputs.push_back(std::make_unique<KeyInput>(g_Config.cameraIncrementAngleRollKey, false, [&photoMode]() {
            photoMode->adjustAngle(0, g_Config.cameraAngleIncDecValue, 0);
            }));
        inputs.push_back(std::make_unique<KeyInput>(g_Config.cameraDecrementAngleYawKey, false, [&photoMode]() {
            photoMode->adjustAngle(0, 0, -g_Config.cameraAngleIncDecValue);
            }));
        inputs.push_back(std::make_unique<KeyInput>(g_Config.cameraIncrementAngleYawKey, false, [&photoMode]() {
            photoMode->adjustAngle(0, 0, g_Config.cameraAngleIncDecValue);
            }));

        // FOV adjustments
        inputs.push_back(std::make_unique<KeyInput>(g_Config.cameraDecrementFovKey, false, [&photoMode]() {
            photoMode->adjustFov(-g_Config.cameraFovIncDecValue);
            }));
        inputs.push_back(std::make_unique<KeyInput>(g_Config.cameraIncrementFovKey, false, [&photoMode]() {
            photoMode->adjustFov(g_Config.cameraFovIncDecValue);
            }));
    }
    
    // super slow mode and no hud are always active, as photo mode requires them.
    // if they're "disabled", we skip binding the keys.
    // effectively a fake toggle and should review if we should allow them to be enabled/disabled.

    // Super slow toggle
    if (g_Config.enableSuperSlowMo) {
        inputs.push_back(std::make_unique<KeyInput>(g_Config.toggleSuperSlowModeKey, true, [&superSlowMode]() {
            superSlowMode.toggle();
            }));
    }

    // No hud
    if (g_Config.enableNoHud) {
        inputs.push_back(std::make_unique<KeyInput>(g_Config.toggleHudKey, true, [&noHud]() {
            noHud.toggle();
            }));
    }

    // gun keys
    if (gunKeys) {
        inputs.push_back(std::make_unique<KeyInput>(g_Config.gunSelectBloodShotKey, true, [&gunKeys]() {
            gunKeys->switchWeapon(GunKeys::WeaponModes::BloodShot);
            }));
        inputs.push_back(std::make_unique<KeyInput>(g_Config.gunSelectBloodStreamKey, true, [&gunKeys]() {
            gunKeys->switchWeapon(GunKeys::WeaponModes::BloodStream);
            }));
        inputs.push_back(std::make_unique<KeyInput>(g_Config.gunSelectBloodSprayKey, true, [&gunKeys]() {
            gunKeys->switchWeapon(GunKeys::WeaponModes::BloodSpray);
            }));
        inputs.push_back(std::make_unique<KeyInput>(g_Config.gunSelectBloodBombKey, true, [&gunKeys]() {
            gunKeys->switchWeapon(GunKeys::WeaponModes::BloodBomb);
            }));
        inputs.push_back(std::make_unique<KeyInput>(g_Config.gunSelectBloodFlameKey, true, [&gunKeys]() {
            gunKeys->switchWeapon(GunKeys::WeaponModes::BloodFlame);
            }));
        inputs.push_back(std::make_unique<KeyInput>(g_Config.gunSelectBloodHammerKey, true, [&gunKeys]() {
            gunKeys->switchWeapon(GunKeys::WeaponModes::BloodHammer);
            }));

        inputs.push_back(std::make_unique<KeyInput>(g_Config.gunSelectPreviousWeaponKey, true, [&gunKeys]() {
            gunKeys->switchToPreviousWeapon();
            }));

        if (g_Config.gunMouseWheelDownPreviousWeapon) {
            auto mouseInput = std::make_unique<MouseInput>([&gunKeys]() {
                gunKeys->switchToPreviousWeapon();
                });

            if (mouseInput->initialize()) {
                inputs.push_back(std::move(mouseInput));
            }
        }
    }
    
    DEBUG_LOG("[DLL] Starting mod...");
    
    // for testing the outfit index, remove this later
    // int* outfitIndex = (int*)0x5e339B4;
    // *outfitIndex = 0x0b; // 11

    while (true) {

        // Handles debug key manually (no callback)
        //if (debugCheckKey.isActivated()) {

            //photoMode.PrintState();

            //DisplayMessage message;
            //message.boxedMessage("Hello from GunSucked mod!");

            // for testing the outfit index, remove this later
            // *outfitIndex += 1;
            // message.boxedMessage("overwriting outfit index");

            //PlaySound sound;
            //sound.confirm();
        //}

        // Process inputs with callbacks
        for (auto& input : inputs) {
            input->checkAndExecute();
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