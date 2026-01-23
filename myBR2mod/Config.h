#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <filesystem>
#include "INIReader.h"

// logging and console
#define DEBUG_CONSOLE_ENABLED true

#if DEBUG_CONSOLE_ENABLED
#include <iostream>
#define DEBUG_LOG(msg) std::cout << msg << std::endl
#else
#define DEBUG_LOG(msg) ((void)0)
#endif

// game addresses
namespace Rayne2 {

    // CMainCharacter
    // these aren't safe to use without checking them at runtime
    // in the future we'll create a utility class or something that will find Rayne's object on init
    constexpr uintptr_t RayneBasePtr = 0x061276EC;
    uintptr_t RayneBase = *reinterpret_cast<uintptr_t*>(RayneBasePtr);

    // photo mode stuff

    // CGameView
    constexpr uintptr_t CameraBase = 0x05E34EE0;

    // camera pos (XZY order)
    float* const CameraX = (float*)(CameraBase + 0x004);
    float* const CameraZ = (float*)(CameraBase + 0x008);
    float* const CameraY = (float*)(CameraBase + 0x00C);
    float* const CameraPitch = (float*)(CameraBase + 0x010);
    float* const CameraYaw = (float*)(CameraBase + 0x014);
    float* const CameraFOV = (float*)(CameraBase + 0x01C);
    int* const CameraMode = (int*)(CameraBase + 0x214);

    bool* const PushCamera = (bool*)(0x05E3473D);
    // Some misunderstood mechanics behind TimeFactor
    // It's not "slow mo", this value does not change when using Dilated/SlowMo/Time Freeze
    // this must control "delta" for the update loop and slow mo powers are controlled elsewhere
    float* const TimeFactor = (float*)(0x0619FB68);

    // UI and HUD stuff

    // what appears to be an object that handles HUD, UI, and subtitles/message boxes
    void* const UIManager = (void*)0x05E335E8;

    bool* const DrawHud = (bool*)(0x05F2FA58); // 0 for off, 1 for on

    // weapon stuff

    // pointer to object that tracks if weapon modes are unlocked
    constexpr uintptr_t UnlockedWeaponObjPtr = 0x007BF4BC;
    // weapon unlock states are stored in a 1 byte bitmask
    // eg 00000000 = no weapons unlocked, 00000011 = bloodshot and bloodstream unlocked
    constexpr uintptr_t UnlockedWeaponBitmaskOffset = 0x34;

    // these addresses track current, previous, and next weapon from 0-6 and wraps around.
    // they are read only
    // example: if you have Blood Spray equipped:
    // current is 2, previous is 1, next is 3
    // if you have Blood Shot equipped:
    // current is 0, previous is 5, next is 1
    int* const previousWeaponIndex = (int*)0x05F2FAF0;
    int* const currentEquippedWeaponIndex = (int*)0x05F2FAEC;
    int* const nextWeaponIndex = (int*)0x05F2FAF4;

    // Player object
    // Health base + 0x39c8
    // Rage base + 0x8360

    uintptr_t const RayneHealthOffset = 0x39c8;
    uintptr_t const RayneMaxHealthOffset = 0x39cc;
    uintptr_t const RayneRageOffset = 0x8360;
    uintptr_t const RayneMaxRageOffset = 0x8364;

    // uintptr_t const RayneAmmunitionOffset = 0x07FC; // offset is from CMultigun not CMainCharacter so have to find, name and calculate this separately
}

// key bindings

// Camera position controls
#define DECREMENT_X_KEY VK_OEM_4      // [
#define INCREMENT_X_KEY VK_OEM_6      // ]
#define DECREMENT_Z_KEY VK_OEM_1      // ;
#define INCREMENT_Z_KEY VK_OEM_7      // '
#define DECREMENT_Y_KEY VK_OEM_COMMA  // ,
#define INCREMENT_Y_KEY VK_OEM_PERIOD // .

// Target position controls
#define DECREMENT_ANGLE_PITCH_KEY VK_UP
#define INCREMENT_ANGLE_PITCH_KEY VK_DOWN
#define DECREMENT_ANGLE_YAW_KEY VK_LEFT  
#define INCREMENT_ANGLE_YAW_KEY VK_RIGHT
#define DECREMENT_ANGLE_ROLL_KEY VK_DELETE
#define INCREMENT_ANGLE_ROLL_KEY VK_INSERT

#define DECREMENT_FOV_KEY VK_NEXT     // page down
#define INCREMENT_FOV_KEY VK_PRIOR    // page up

// toggle keys
#define TOGGLE_PHOTO_MODE_KEY VK_F7
#define TOGGLE_SUPER_SLOW_MODE_KEY VK_F8
#define TOGGLE_HUD_KEY VK_F9

// Debug
#define DEBUG_CHECK_KEY VK_F6

// tuning values
#define CAMERA_POS_INCREMENT_DECREMENT_VALUE 0.066f
#define ANGLE_INCREMENT_DECREMENT_VALUE 0.016f
#define FOV_INCREMENT_DECREMENT_VALUE 1.0f

#define SUPER_SLOW_MODE_TIME_FACTOR 0.05f

// behaviors
#define PHOTO_MODE_DISABLE_HUD_ON_ENTER true
#define PHOTO_MODE_RESTORE_TIME_FACTOR_ON_EXIT true

// gun hotkeys
#define GUN_SELECT_BLOODSHOT_KEY 0x35 // numrow 5
#define GUN_SELECT_BLOODSTREAM_KEY 0x36 // 6
#define GUN_SELECT_BLOODSPRAY_KEY 0x37 // 7
#define GUN_SELECT_BLOODBOMB_KEY 0x38 // 8
#define GUN_SELECT_BLOODFLAME_KEY 0x39 // 9
#define GUN_SELECT_BLOODHAMMER_KEY 0x30 // 0

// used if player is not using mousewheel down for previous weapon.
// should be a toggle in the .ini file
#define GUN_SELECT_PREVIOUS_WEAPON_KEY VK_OEM_MINUS // -

// if enabled, mousewheel down switches to previous weapon
// mousewheel up is already next weapon by default, so this allows bidirectional weapon mode scrolling
#define ENABLE_MOUSEWHEEL_DOWN_PREVIOUS_WEAPON 1

// how many entries allowed to be added to the outfits menu. Outfits are about 1.4kb each. Wonder if we can allocate them dynamically?
#define OUTFIT_MAX_ENTRIES 1024

// Casual Mode
// enable regenerating health
// Casual mode has some problems (detailed in CasualMode.h) and isn't ready to be added yet.
#define ENABLE_CASUAL_MODE 0

namespace Utilities {
    // Rayne total ammunition = 1000.0
    // all ammunition maximums per weapon mode are calculated as 1000/ammunition cost
    constexpr float ammoCostFromPool(float desiredPoolSize) {
        return 1000.0f / desiredPoolSize;
    }
}

// this is the canonical default config
// any default values should be referenced from here
namespace ConfigDefaults {
    // enable features
    constexpr bool enableGunBalance = true;
    constexpr bool enableGunKeys = true;
    constexpr bool enablePhotoMode = true;
    constexpr bool enableSuperSlowMo = true;
    constexpr bool enableNoHud = true;
    constexpr bool enableOutfitMods = true;

    // camera keybinds
    constexpr int cameraDecrementXKey = VK_OEM_4;
    constexpr int cameraIncrementXKey = VK_OEM_6;

    constexpr int cameraDecrementZKey = VK_OEM_1;
    constexpr int cameraIncrementZKey = VK_OEM_7;

    constexpr int cameraDecrementYKey = VK_OEM_COMMA;
    constexpr int cameraIncrementYKey = VK_OEM_PERIOD;

    constexpr int cameraDecrementAnglePitchKey = VK_UP;
    constexpr int cameraIncrementAnglePitchKey = VK_DOWN;

    constexpr int cameraDecrementAngleYawKey = VK_LEFT;
    constexpr int cameraIncrementAngleYawKey = VK_RIGHT;

    constexpr int cameraDecrementAngleRollKey = VK_DELETE;
    constexpr int cameraIncrementAngleRollKey = VK_INSERT;

    constexpr int cameraDecrementFovKey = VK_NEXT;
    constexpr int cameraIncrementFovKey = VK_PRIOR;

    // toggle keys
    constexpr int togglePhotoModeKey = VK_F7;
    constexpr int toggleSuperSlowModeKey = VK_F8;
    constexpr int toggleHudKey = VK_F9;

    // tuning values
    constexpr float cameraPosIncDecValue = 0.066f;
    constexpr float cameraAngleIncDecValue = 0.016f;
    constexpr float cameraFovIncDecValue = 1.0f;

    constexpr float cameraMaxFov = 999.0f; // fov values are inverted, ie higher number = tighter fov
    constexpr float cameraMinFov = 1.0f; // fov values are inverted, ie lower number = wider fov
    constexpr float cameraReferenceFov = 60.0f; // base value for calculating derived sensitivity based on fov, sensitivity scales above and below this value
    constexpr float cameraSensitivityScale = 0.75f; // how aggressively camera sensitivity scales based on FOV
    // constexpr float cameraMaxSensitivityScale = 0.2f; // the maximum sensitivity can scale to

    constexpr float superSlowModeTimeFactor = 0.05f;

    // behaviors
    constexpr bool photoModeDisableHudOnEnter = true;
    constexpr bool photoModeRestoreTimeFactorOnExit = true;
    // constexpr bool photoModeAllowSensitivityScaling = true; // whether or not sensitivity is scaled by fov

    // gun hotkeys
    constexpr int gunSelectBloodShotKey = 0x35;
    constexpr int gunSelectBloodStreamKey = 0x36;
    constexpr int gunSelectBloodSprayKey = 0x37;
    constexpr int gunSelectBloodBombKey = 0x38;
    constexpr int gunSelectBloodFlameKey = 0x39;
    constexpr int gunSelectBloodHammerKey = 0x30;
    constexpr int gunSelectPreviousWeaponKey = VK_OEM_MINUS;
    constexpr bool gunMouseWheelDownPreviousWeapon = true;

    // gun balance
    constexpr float gunBloodShotAmmoCostLvl1 = Utilities::ammoCostFromPool(180.0f); // default 25
    constexpr float gunBloodShotAmmoCostLvl2 = Utilities::ammoCostFromPool(200.0f); // default 18
    constexpr float gunBloodShotAmmoCostLvl3 = Utilities::ammoCostFromPool(220.0f); // default 15
    constexpr float gunBloodShotDamageLvl1 = 75.0f; // default 50
    constexpr float gunBloodShotDamageLvl2 = 90.0f; // default 55
    constexpr float gunBloodShotDamageLvl3 = 112.5f; // default 60
    constexpr float gunBloodShotFireRateLvl1 = 5.0f; // default 4
    constexpr float gunBloodShotFireRateLvl2 = 5.5f; // default 5
    constexpr float gunBloodShotFireRateLvl3 = 6.5f; // default 6
    constexpr float gunBloodShotRange = 120.0f; // default 80

    constexpr float gunBloodStreamAmmoCostLvl1 = Utilities::ammoCostFromPool(90.0f); // default 25
    constexpr float gunBloodStreamAmmoCostLvl2 = Utilities::ammoCostFromPool(120.0f); // default 22
    constexpr float gunBloodStreamAmmoCostLvl3 = Utilities::ammoCostFromPool(150.0f); // default 20
    constexpr float gunBloodStreamDamageLvl1 = 75.0f; // default 50
    constexpr float gunBloodStreamDamageLvl2 = 80.0f; // default 55
    constexpr float gunBloodStreamDamageLvl3 = 85.0f; // default 60
    constexpr float gunBloodStreamFireRateLvl1 = 12.0f; // default 8
    constexpr float gunBloodStreamFireRateLvl2 = 16.0f; // default 10
    constexpr float gunBloodStreamFireRateLvl3 = 20.0f; // default 12
    constexpr float gunBloodStreamRange = 100.0f; // default 80

    constexpr float gunBloodSprayAmmoCostLvl1 = Utilities::ammoCostFromPool(30.0f); // default 100
    constexpr float gunBloodSprayAmmoCostLvl2 = Utilities::ammoCostFromPool(35.0f); // default 50
    constexpr float gunBloodSprayAmmoCostLvl3 = Utilities::ammoCostFromPool(40.0f); // default 50
    constexpr float gunBloodSprayDamageLvl1 = 20.0f; // default 13.33
    constexpr float gunBloodSprayDamageLvl2 = 33.0f; // default 8.33
    constexpr float gunBloodSprayDamageLvl3 = 45.0f; // default 10
    constexpr float gunBloodSprayFireRateLvl1 = 1.5f; // default 1
    constexpr float gunBloodSprayFireRateLvl2 = 1.66f; // default 1
    constexpr float gunBloodSprayFireRateLvl3 = 1.75f; // default 1
    constexpr float gunBloodSprayRange = 80.0f; // default 80

    constexpr float gunBloodBombAmmoCostLvl1 = Utilities::ammoCostFromPool(20.0f); // default 200
    constexpr float gunBloodBombAmmoCostLvl2 = Utilities::ammoCostFromPool(25.0f); // default 80
    constexpr float gunBloodBombAmmoCostLvl3 = Utilities::ammoCostFromPool(30.0f); // default 67
    constexpr float gunBloodBombDamageLvl1 = 200.0f; // default 100
    constexpr float gunBloodBombDamageLvl2 = 275.0f; // default 50
    constexpr float gunBloodBombDamageLvl3 = 350.0f; // default 50
    constexpr float gunBloodBombFireRateLvl1 = 1.0f; // default 1
    constexpr float gunBloodBombFireRateLvl2 = 1.25f; // default 2
    constexpr float gunBloodBombFireRateLvl3 = 1.5f; // default 1
    constexpr float gunBloodBombRange = 80.0f; // default 50

    constexpr float gunBloodFlameAmmoCostLvl1 = Utilities::ammoCostFromPool(15.0f); // default 200
    constexpr float gunBloodFlameAmmoCostLvl2 = Utilities::ammoCostFromPool(18.0f); // default 80
    constexpr float gunBloodFlameAmmoCostLvl3 = Utilities::ammoCostFromPool(22.0f); // default 67
    constexpr float gunBloodFlameDamageLvl1 = 100.0f; // default 100
    constexpr float gunBloodFlameDamageLvl2 = 120.0f; // default 50
    constexpr float gunBloodFlameDamageLvl3 = 140.0f; // default 50
    constexpr float gunBloodFlameFireRateLvl1 = 1.0f; // default 1
    constexpr float gunBloodFlameFireRateLvl2 = 1.2f; // default 1
    constexpr float gunBloodFlameFireRateLvl3 = 1.4f; // default 1
    constexpr float gunBloodFlameRange = 25.0f; // default 10

    constexpr float gunBloodHammerAmmoCostLvl1 = Utilities::ammoCostFromPool(12.0f); // default 300
    constexpr float gunBloodHammerAmmoCostLvl2 = Utilities::ammoCostFromPool(14.0f); // default 131
    constexpr float gunBloodHammerAmmoCostLvl3 = Utilities::ammoCostFromPool(16.0f); // default 116
    constexpr float gunBloodHammerDamageLvl1 = 400.0f; // default 300
    constexpr float gunBloodHammerDamageLvl2 = 500.0f; // default 325
    constexpr float gunBloodHammerDamageLvl3 = 600.0f; // default 350
    constexpr float gunBloodHammerFireRateLvl1 = 1.0f; // default 1
    constexpr float gunBloodHammerFireRateLvl2 = 1.1f; // default 1
    constexpr float gunBloodHammerFireRateLvl3 = 1.2f; // default 1
    constexpr float gunBloodHammerRange = 250.0f; // default 200

    // internal settings
    constexpr int outfitMaxEntries = 1024;
    const std::string outfitModPath = "mods\\outfits";
};

class Config {
private:

public:

    // features enabled
    bool enableGunBalance;
    bool enableGunKeys;
    bool enablePhotoMode;
    bool enableSuperSlowMo;
    bool enableNoHud;
    bool enableOutfitMods;

    // camera controls
    int cameraDecrementXKey;
    int cameraIncrementXKey;

    int cameraDecrementZKey;
    int cameraIncrementZKey;

    int cameraDecrementYKey;
    int cameraIncrementYKey;

    int cameraDecrementAnglePitchKey;
    int cameraIncrementAnglePitchKey;

    int cameraDecrementAngleYawKey;
    int cameraIncrementAngleYawKey;

    int cameraDecrementAngleRollKey;
    int cameraIncrementAngleRollKey;

    int cameraDecrementFovKey;
    int cameraIncrementFovKey;
    
    // toggle keys
    int togglePhotoModeKey;
    int toggleSuperSlowModeKey;
    int toggleHudKey;
    // int debugCheckKey;

    // tuning values
    float cameraPosIncDecValue;
    float cameraAngleIncDecValue;
    float cameraFovIncDecValue;

    float cameraMinFov;
    float cameraMaxFov;
    float cameraReferenceFov;
    float cameraSensitivityScale;
    
    float superSlowModeTimeFactor;

    // behaviors
    bool photoModeDisableHudOnEnter;
    bool photoModeRestoreTimeFactorOnExit;

    // gun hotkeys
    int gunSelectBloodShotKey;
    int gunSelectBloodStreamKey;
    int gunSelectBloodSprayKey;
    int gunSelectBloodBombKey;
    int gunSelectBloodFlameKey;
    int gunSelectBloodHammerKey;
    int gunSelectPreviousWeaponKey;
    bool gunMouseWheelDownPreviousWeapon;

    // gun balance
    float gunBloodShotAmmoCostLvl1;
    float gunBloodShotAmmoCostLvl2;
    float gunBloodShotAmmoCostLvl3;
    float gunBloodShotDamageLvl1;
    float gunBloodShotDamageLvl2;
    float gunBloodShotDamageLvl3;
    float gunBloodShotFireRateLvl1;
    float gunBloodShotFireRateLvl2;
    float gunBloodShotFireRateLvl3;
    float gunBloodShotRange;

    float gunBloodStreamAmmoCostLvl1;
    float gunBloodStreamAmmoCostLvl2;
    float gunBloodStreamAmmoCostLvl3;
    float gunBloodStreamDamageLvl1;
    float gunBloodStreamDamageLvl2;
    float gunBloodStreamDamageLvl3;
    float gunBloodStreamFireRateLvl1;
    float gunBloodStreamFireRateLvl2;
    float gunBloodStreamFireRateLvl3;
    float gunBloodStreamRange;

    float gunBloodSprayAmmoCostLvl1;
    float gunBloodSprayAmmoCostLvl2;
    float gunBloodSprayAmmoCostLvl3;
    float gunBloodSprayDamageLvl1;
    float gunBloodSprayDamageLvl2;
    float gunBloodSprayDamageLvl3;
    float gunBloodSprayFireRateLvl1;
    float gunBloodSprayFireRateLvl2;
    float gunBloodSprayFireRateLvl3;
    float gunBloodSprayRange;

    float gunBloodBombAmmoCostLvl1;
    float gunBloodBombAmmoCostLvl2;
    float gunBloodBombAmmoCostLvl3;
    float gunBloodBombDamageLvl1;
    float gunBloodBombDamageLvl2;
    float gunBloodBombDamageLvl3;
    float gunBloodBombFireRateLvl1;
    float gunBloodBombFireRateLvl2;
    float gunBloodBombFireRateLvl3;
    float gunBloodBombRange;

    float gunBloodFlameAmmoCostLvl1;
    float gunBloodFlameAmmoCostLvl2;
    float gunBloodFlameAmmoCostLvl3;
    float gunBloodFlameDamageLvl1;
    float gunBloodFlameDamageLvl2;
    float gunBloodFlameDamageLvl3;
    float gunBloodFlameFireRateLvl1;
    float gunBloodFlameFireRateLvl2;
    float gunBloodFlameFireRateLvl3;
    float gunBloodFlameRange;

    float gunBloodHammerAmmoCostLvl1;
    float gunBloodHammerAmmoCostLvl2;
    float gunBloodHammerAmmoCostLvl3;
    float gunBloodHammerDamageLvl1;
    float gunBloodHammerDamageLvl2;
    float gunBloodHammerDamageLvl3;
    float gunBloodHammerFireRateLvl1;
    float gunBloodHammerFireRateLvl2;
    float gunBloodHammerFireRateLvl3;
    float gunBloodHammerRange;

    // internal stuff
    int outfitMaxEntries;
    std::string outfitModPath;

    Config() {
        // ensure initialized - but defaults are also loaded from loadConfig() if INIReader's Get methods fail
         this->setInternalDefaultConfig();
    };

    ~Config() {};

    bool loadConfig() {
        // attempt to load gunsucked.ini
        DEBUG_LOG("Attempting to parse gunsucked.ini...");
        INIReader reader("gunsucked.ini");

        // if unsuccessful, fall back to defaults (should have already been set in constructor via setInternalDefaultConfig()
        int error = reader.ParseError();
        if (error == -1 || error == -2) {
            DEBUG_LOG("Ini parsing error. using defaults");
            return false;
        }
        else if (error > 0) {
            DEBUG_LOG("Ini parsing error: " << reader.ParseErrorMessage());
            return false;
        }

        // the last argument to INIReader's Get methods is a default, so should always reference ConfigDefaults

        // features
        this->enableGunBalance = reader.GetBoolean("features", "enableGunBalance", ConfigDefaults::enableGunBalance);
        this->enableGunKeys = reader.GetBoolean("features", "enableGunKeys", ConfigDefaults::enableGunKeys);
        this->enablePhotoMode = reader.GetBoolean("features", "enablePhotoMode", ConfigDefaults::enablePhotoMode);
        this->enableSuperSlowMo = reader.GetBoolean("features", "enableSuperSlowMo", ConfigDefaults::enableSuperSlowMo);
        this->enableNoHud = reader.GetBoolean("features", "enableNoHud", ConfigDefaults::enableNoHud);
        this->enableOutfitMods = reader.GetBoolean("features", "enableOutfitMods", ConfigDefaults::enableOutfitMods);

        // photo mode
        this->cameraDecrementXKey = (int)reader.GetInteger("photoMode", "cameraDecrementXKey", ConfigDefaults::cameraDecrementXKey);
        this->cameraIncrementXKey = (int)reader.GetInteger("photoMode", "cameraIncrementXKey", ConfigDefaults::cameraIncrementXKey);

        this->cameraDecrementZKey = (int)reader.GetInteger("photoMode", "cameraDecrementZKey", ConfigDefaults::cameraDecrementZKey);
        this->cameraIncrementZKey = (int)reader.GetInteger("photoMode", "cameraIncrementZKey", ConfigDefaults::cameraIncrementZKey);

        this->cameraDecrementYKey = (int)reader.GetInteger("photoMode", "cameraDecrementYKey", ConfigDefaults::cameraDecrementYKey);
        this->cameraIncrementYKey = (int)reader.GetInteger("photoMode", "cameraIncrementYKey", ConfigDefaults::cameraIncrementYKey);

        this->cameraDecrementAnglePitchKey = (int)reader.GetInteger("photoMode", "cameraDecrementAnglePitchKey", ConfigDefaults::cameraDecrementAnglePitchKey);
        this->cameraIncrementAnglePitchKey = (int)reader.GetInteger("photoMode", "cameraIncrementAnglePitchKey", ConfigDefaults::cameraIncrementAnglePitchKey);

        this->cameraDecrementAngleYawKey = (int)reader.GetInteger("photoMode", "cameraDecrementAngleYawKey", ConfigDefaults::cameraDecrementAngleYawKey);
        this->cameraIncrementAngleYawKey = (int)reader.GetInteger("photoMode", "cameraIncrementAngleYawKey", ConfigDefaults::cameraIncrementAngleYawKey);

        this->cameraDecrementAngleRollKey = (int)reader.GetInteger("photoMode", "cameraDecrementAngleRollKey", ConfigDefaults::cameraDecrementAngleRollKey);
        this->cameraIncrementAngleRollKey = (int)reader.GetInteger("photoMode", "cameraIncrementAngleRollKey", ConfigDefaults::cameraIncrementAngleRollKey);

        this->cameraDecrementFovKey = (int)reader.GetInteger("photoMode", "cameraDecrementFovKey", ConfigDefaults::cameraDecrementFovKey);
        this->cameraIncrementFovKey = (int)reader.GetInteger("photoMode", "cameraIncrementFovKey", ConfigDefaults::cameraIncrementFovKey);

        // toggle keys
        this->togglePhotoModeKey = (int)reader.GetInteger("toggleKeys", "togglePhotoModeKey", ConfigDefaults::togglePhotoModeKey);
        this->toggleSuperSlowModeKey = (int)reader.GetInteger("toggleKeys", "toggleSuperSlowModeKey", ConfigDefaults::toggleSuperSlowModeKey);
        this->toggleHudKey = (int)reader.GetInteger("toggleKeys", "toggleHudKey", ConfigDefaults::toggleHudKey);

        // tuning values
        this->cameraPosIncDecValue = (float)reader.GetReal("tuningValues", "cameraPosIncDecValue", ConfigDefaults::cameraPosIncDecValue);
        this->cameraAngleIncDecValue = (float)reader.GetReal("tuningValues", "cameraAngleIncDecValue", ConfigDefaults::cameraAngleIncDecValue);
        this->cameraFovIncDecValue = (float)reader.GetReal("tuningValues", "cameraFovIncDecValue", ConfigDefaults::cameraFovIncDecValue);

        this->cameraMinFov = (float)reader.GetReal("tuningValues", "cameraMinFov", ConfigDefaults::cameraMinFov);
        this->cameraMaxFov = (float)reader.GetReal("tuningValues", "cameraMaxFov", ConfigDefaults::cameraMaxFov);
        this->cameraReferenceFov = (float)reader.GetReal("tuningValues", "cameraReferenceFov", ConfigDefaults::cameraReferenceFov);
        this->cameraSensitivityScale = (float)reader.GetReal("tuningValues", "cameraSensitivityScale", ConfigDefaults::cameraSensitivityScale);

        this->superSlowModeTimeFactor = (float)reader.GetReal("tuningValues", "superSlowModeTimeFactor", ConfigDefaults::superSlowModeTimeFactor);

        // behaviors
        this->photoModeDisableHudOnEnter = reader.GetBoolean("behaviors", "photoModeDisableHudOnEnter", ConfigDefaults::photoModeDisableHudOnEnter);
        this->photoModeRestoreTimeFactorOnExit = reader.GetBoolean("behaviors", "photoModeRestoreTimeFactorOnExit", ConfigDefaults::photoModeRestoreTimeFactorOnExit);

        // gun hotkeys
        this->gunSelectBloodShotKey = (int)reader.GetInteger("gunHotkeys", "gunSelectBloodShotKey", ConfigDefaults::gunSelectBloodShotKey);
        this->gunSelectBloodStreamKey = (int)reader.GetInteger("gunHotkeys", "gunSelectBloodStreamKey", ConfigDefaults::gunSelectBloodStreamKey);
        this->gunSelectBloodSprayKey = (int)reader.GetInteger("gunHotkeys", "gunSelectBloodSprayKey", ConfigDefaults::gunSelectBloodSprayKey);
        this->gunSelectBloodBombKey = (int)reader.GetInteger("gunHotkeys", "gunSelectBloodBombKey", ConfigDefaults::gunSelectBloodBombKey);
        this->gunSelectBloodFlameKey = (int)reader.GetInteger("gunHotkeys", "gunSelectBloodFlameKey", ConfigDefaults::gunSelectBloodFlameKey);
        this->gunSelectBloodHammerKey = (int)reader.GetInteger("gunHotkeys", "gunSelectBloodHammerKey", ConfigDefaults::gunSelectBloodHammerKey);

        this->gunSelectPreviousWeaponKey = (int)reader.GetInteger("gunHotkeys", "gunSelectPreviousWeaponKey", ConfigDefaults::gunSelectPreviousWeaponKey);
        this->gunMouseWheelDownPreviousWeapon = reader.GetBoolean("gunHotkeys", "gunMouseWheelDownPreviousWeapon", ConfigDefaults::gunMouseWheelDownPreviousWeapon);

        // gun balance - BloodShot
        this->gunBloodShotAmmoCostLvl1 = (float)reader.GetReal("gunBalance", "gunBloodShotAmmoCostLvl1", ConfigDefaults::gunBloodShotAmmoCostLvl1);
        this->gunBloodShotAmmoCostLvl2 = (float)reader.GetReal("gunBalance", "gunBloodShotAmmoCostLvl2", ConfigDefaults::gunBloodShotAmmoCostLvl2);
        this->gunBloodShotAmmoCostLvl3 = (float)reader.GetReal("gunBalance", "gunBloodShotAmmoCostLvl3", ConfigDefaults::gunBloodShotAmmoCostLvl3);
        this->gunBloodShotDamageLvl1 = (float)reader.GetReal("gunBalance", "gunBloodShotDamageLvl1", ConfigDefaults::gunBloodShotDamageLvl1);
        this->gunBloodShotDamageLvl2 = (float)reader.GetReal("gunBalance", "gunBloodShotDamageLvl2", ConfigDefaults::gunBloodShotDamageLvl2);
        this->gunBloodShotDamageLvl3 = (float)reader.GetReal("gunBalance", "gunBloodShotDamageLvl3", ConfigDefaults::gunBloodShotDamageLvl3);
        this->gunBloodShotFireRateLvl1 = (float)reader.GetReal("gunBalance", "gunBloodShotFireRateLvl1", ConfigDefaults::gunBloodShotFireRateLvl1);
        this->gunBloodShotFireRateLvl2 = (float)reader.GetReal("gunBalance", "gunBloodShotFireRateLvl2", ConfigDefaults::gunBloodShotFireRateLvl2);
        this->gunBloodShotFireRateLvl3 = (float)reader.GetReal("gunBalance", "gunBloodShotFireRateLvl3", ConfigDefaults::gunBloodShotFireRateLvl3);
        this->gunBloodShotRange = (float)reader.GetReal("gunBalance", "gunBloodShotRange", ConfigDefaults::gunBloodShotRange);

        // gun balance - BloodStream
        this->gunBloodStreamAmmoCostLvl1 = (float)reader.GetReal("gunBalance", "gunBloodStreamAmmoCostLvl1", ConfigDefaults::gunBloodStreamAmmoCostLvl1);
        this->gunBloodStreamAmmoCostLvl2 = (float)reader.GetReal("gunBalance", "gunBloodStreamAmmoCostLvl2", ConfigDefaults::gunBloodStreamAmmoCostLvl2);
        this->gunBloodStreamAmmoCostLvl3 = (float)reader.GetReal("gunBalance", "gunBloodStreamAmmoCostLvl3", ConfigDefaults::gunBloodStreamAmmoCostLvl3);
        this->gunBloodStreamDamageLvl1 = (float)reader.GetReal("gunBalance", "gunBloodStreamDamageLvl1", ConfigDefaults::gunBloodStreamDamageLvl1);
        this->gunBloodStreamDamageLvl2 = (float)reader.GetReal("gunBalance", "gunBloodStreamDamageLvl2", ConfigDefaults::gunBloodStreamDamageLvl2);
        this->gunBloodStreamDamageLvl3 = (float)reader.GetReal("gunBalance", "gunBloodStreamDamageLvl3", ConfigDefaults::gunBloodStreamDamageLvl3);
        this->gunBloodStreamFireRateLvl1 = (float)reader.GetReal("gunBalance", "gunBloodStreamFireRateLvl1", ConfigDefaults::gunBloodStreamFireRateLvl1);
        this->gunBloodStreamFireRateLvl2 = (float)reader.GetReal("gunBalance", "gunBloodStreamFireRateLvl2", ConfigDefaults::gunBloodStreamFireRateLvl2);
        this->gunBloodStreamFireRateLvl3 = (float)reader.GetReal("gunBalance", "gunBloodStreamFireRateLvl3", ConfigDefaults::gunBloodStreamFireRateLvl3);
        this->gunBloodStreamRange = (float)reader.GetReal("gunBalance", "gunBloodStreamRange", ConfigDefaults::gunBloodStreamRange);

        // gun balance - BloodSpray
        this->gunBloodSprayAmmoCostLvl1 = (float)reader.GetReal("gunBalance", "gunBloodSprayAmmoCostLvl1", ConfigDefaults::gunBloodSprayAmmoCostLvl1);
        this->gunBloodSprayAmmoCostLvl2 = (float)reader.GetReal("gunBalance", "gunBloodSprayAmmoCostLvl2", ConfigDefaults::gunBloodSprayAmmoCostLvl2);
        this->gunBloodSprayAmmoCostLvl3 = (float)reader.GetReal("gunBalance", "gunBloodSprayAmmoCostLvl3", ConfigDefaults::gunBloodSprayAmmoCostLvl3);
        this->gunBloodSprayDamageLvl1 = (float)reader.GetReal("gunBalance", "gunBloodSprayDamageLvl1", ConfigDefaults::gunBloodSprayDamageLvl1);
        this->gunBloodSprayDamageLvl2 = (float)reader.GetReal("gunBalance", "gunBloodSprayDamageLvl2", ConfigDefaults::gunBloodSprayDamageLvl2);
        this->gunBloodSprayDamageLvl3 = (float)reader.GetReal("gunBalance", "gunBloodSprayDamageLvl3", ConfigDefaults::gunBloodSprayDamageLvl3);
        this->gunBloodSprayFireRateLvl1 = (float)reader.GetReal("gunBalance", "gunBloodSprayFireRateLvl1", ConfigDefaults::gunBloodSprayFireRateLvl1);
        this->gunBloodSprayFireRateLvl2 = (float)reader.GetReal("gunBalance", "gunBloodSprayFireRateLvl2", ConfigDefaults::gunBloodSprayFireRateLvl2);
        this->gunBloodSprayFireRateLvl3 = (float)reader.GetReal("gunBalance", "gunBloodSprayFireRateLvl3", ConfigDefaults::gunBloodSprayFireRateLvl3);
        this->gunBloodSprayRange = (float)reader.GetReal("gunBalance", "gunBloodSprayRange", ConfigDefaults::gunBloodSprayRange);

        // gun balance - BloodBomb
        this->gunBloodBombAmmoCostLvl1 = (float)reader.GetReal("gunBalance", "gunBloodBombAmmoCostLvl1", ConfigDefaults::gunBloodBombAmmoCostLvl1);
        this->gunBloodBombAmmoCostLvl2 = (float)reader.GetReal("gunBalance", "gunBloodBombAmmoCostLvl2", ConfigDefaults::gunBloodBombAmmoCostLvl2);
        this->gunBloodBombAmmoCostLvl3 = (float)reader.GetReal("gunBalance", "gunBloodBombAmmoCostLvl3", ConfigDefaults::gunBloodBombAmmoCostLvl3);
        this->gunBloodBombDamageLvl1 = (float)reader.GetReal("gunBalance", "gunBloodBombDamageLvl1", ConfigDefaults::gunBloodBombDamageLvl1);
        this->gunBloodBombDamageLvl2 = (float)reader.GetReal("gunBalance", "gunBloodBombDamageLvl2", ConfigDefaults::gunBloodBombDamageLvl2);
        this->gunBloodBombDamageLvl3 = (float)reader.GetReal("gunBalance", "gunBloodBombDamageLvl3", ConfigDefaults::gunBloodBombDamageLvl3);
        this->gunBloodBombFireRateLvl1 = (float)reader.GetReal("gunBalance", "gunBloodBombFireRateLvl1", ConfigDefaults::gunBloodBombFireRateLvl1);
        this->gunBloodBombFireRateLvl2 = (float)reader.GetReal("gunBalance", "gunBloodBombFireRateLvl2", ConfigDefaults::gunBloodBombFireRateLvl2);
        this->gunBloodBombFireRateLvl3 = (float)reader.GetReal("gunBalance", "gunBloodBombFireRateLvl3", ConfigDefaults::gunBloodBombFireRateLvl3);
        this->gunBloodBombRange = (float)reader.GetReal("gunBalance", "gunBloodBombRange", ConfigDefaults::gunBloodBombRange);

        // gun balance - BloodFlame
        this->gunBloodFlameAmmoCostLvl1 = (float)reader.GetReal("gunBalance", "gunBloodFlameAmmoCostLvl1", ConfigDefaults::gunBloodFlameAmmoCostLvl1);
        this->gunBloodFlameAmmoCostLvl2 = (float)reader.GetReal("gunBalance", "gunBloodFlameAmmoCostLvl2", ConfigDefaults::gunBloodFlameAmmoCostLvl2);
        this->gunBloodFlameAmmoCostLvl3 = (float)reader.GetReal("gunBalance", "gunBloodFlameAmmoCostLvl3", ConfigDefaults::gunBloodFlameAmmoCostLvl3);
        this->gunBloodFlameDamageLvl1 = (float)reader.GetReal("gunBalance", "gunBloodFlameDamageLvl1", ConfigDefaults::gunBloodFlameDamageLvl1);
        this->gunBloodFlameDamageLvl2 = (float)reader.GetReal("gunBalance", "gunBloodFlameDamageLvl2", ConfigDefaults::gunBloodFlameDamageLvl2);
        this->gunBloodFlameDamageLvl3 = (float)reader.GetReal("gunBalance", "gunBloodFlameDamageLvl3", ConfigDefaults::gunBloodFlameDamageLvl3);
        this->gunBloodFlameFireRateLvl1 = (float)reader.GetReal("gunBalance", "gunBloodFlameFireRateLvl1", ConfigDefaults::gunBloodFlameFireRateLvl1);
        this->gunBloodFlameFireRateLvl2 = (float)reader.GetReal("gunBalance", "gunBloodFlameFireRateLvl2", ConfigDefaults::gunBloodFlameFireRateLvl2);
        this->gunBloodFlameFireRateLvl3 = (float)reader.GetReal("gunBalance", "gunBloodFlameFireRateLvl3", ConfigDefaults::gunBloodFlameFireRateLvl3);
        this->gunBloodFlameRange = (float)reader.GetReal("gunBalance", "gunBloodFlameRange", ConfigDefaults::gunBloodFlameRange);

        // gun balance - BloodHammer
        this->gunBloodHammerAmmoCostLvl1 = (float)reader.GetReal("gunBalance", "gunBloodHammerAmmoCostLvl1", ConfigDefaults::gunBloodHammerAmmoCostLvl1);
        this->gunBloodHammerAmmoCostLvl2 = (float)reader.GetReal("gunBalance", "gunBloodHammerAmmoCostLvl2", ConfigDefaults::gunBloodHammerAmmoCostLvl2);
        this->gunBloodHammerAmmoCostLvl3 = (float)reader.GetReal("gunBalance", "gunBloodHammerAmmoCostLvl3", ConfigDefaults::gunBloodHammerAmmoCostLvl3);
        this->gunBloodHammerDamageLvl1 = (float)reader.GetReal("gunBalance", "gunBloodHammerDamageLvl1", ConfigDefaults::gunBloodHammerDamageLvl1);
        this->gunBloodHammerDamageLvl2 = (float)reader.GetReal("gunBalance", "gunBloodHammerDamageLvl2", ConfigDefaults::gunBloodHammerDamageLvl2);
        this->gunBloodHammerDamageLvl3 = (float)reader.GetReal("gunBalance", "gunBloodHammerDamageLvl3", ConfigDefaults::gunBloodHammerDamageLvl3);
        this->gunBloodHammerFireRateLvl1 = (float)reader.GetReal("gunBalance", "gunBloodHammerFireRateLvl1", ConfigDefaults::gunBloodHammerFireRateLvl1);
        this->gunBloodHammerFireRateLvl2 = (float)reader.GetReal("gunBalance", "gunBloodHammerFireRateLvl2", ConfigDefaults::gunBloodHammerFireRateLvl2);
        this->gunBloodHammerFireRateLvl3 = (float)reader.GetReal("gunBalance", "gunBloodHammerFireRateLvl3", ConfigDefaults::gunBloodHammerFireRateLvl3);
        this->gunBloodHammerRange = (float)reader.GetReal("gunBalance", "gunBloodHammerRange", ConfigDefaults::gunBloodHammerRange);

        // internal settings
        this->outfitMaxEntries = (int)reader.GetInteger("internalSettings", "outfitMaxEntries", ConfigDefaults::outfitMaxEntries);
        this->outfitModPath = reader.Get("internalSettings", "outfitModPath", ConfigDefaults::outfitModPath);

        return true;
    };

    bool configExists() {
        DEBUG_LOG("Checking for config file...");
        std::filesystem::path configPath = "gunsucked.ini";

        if (std::filesystem::exists(configPath)) {
            DEBUG_LOG("Config file found");
            return true;
        }
        else {
            DEBUG_LOG("No config file found. Writing default...");
            return false;
        }
    };

    // default initialization and a fallback for if loading .ini fails
    void setInternalDefaultConfig() {
        // enable features
        this->enableGunBalance = ConfigDefaults::enableGunBalance; // E0254: type name is not allowed
        this->enableGunKeys = ConfigDefaults::enableGunKeys;
        this->enablePhotoMode = ConfigDefaults::enablePhotoMode;
        this->enableSuperSlowMo = ConfigDefaults::enableSuperSlowMo;
        this->enableNoHud = ConfigDefaults::enableNoHud;
        this->enableOutfitMods = ConfigDefaults::enableOutfitMods;

        // camera keybinds
        this->cameraDecrementXKey = ConfigDefaults::cameraDecrementXKey;
        this->cameraIncrementXKey = ConfigDefaults::cameraIncrementXKey;

        this->cameraDecrementZKey = ConfigDefaults::cameraDecrementZKey;
        this->cameraIncrementZKey = ConfigDefaults::cameraIncrementZKey;

        this->cameraDecrementYKey = ConfigDefaults::cameraDecrementYKey;
        this->cameraIncrementYKey = ConfigDefaults::cameraIncrementYKey;

        this->cameraDecrementAnglePitchKey = ConfigDefaults::cameraDecrementAnglePitchKey;
        this->cameraIncrementAnglePitchKey = ConfigDefaults::cameraIncrementAnglePitchKey;

        this->cameraDecrementAngleYawKey = ConfigDefaults::cameraDecrementAngleYawKey;
        this->cameraIncrementAngleYawKey = ConfigDefaults::cameraIncrementAngleYawKey;

        this->cameraDecrementAngleRollKey = ConfigDefaults::cameraDecrementAngleRollKey;
        this->cameraIncrementAngleRollKey = ConfigDefaults::cameraIncrementAngleRollKey;

        this->cameraDecrementFovKey = ConfigDefaults::cameraDecrementFovKey;
        this->cameraIncrementFovKey = ConfigDefaults::cameraIncrementFovKey;

        // toggle keys
        this->togglePhotoModeKey = ConfigDefaults::togglePhotoModeKey;
        this->toggleSuperSlowModeKey = ConfigDefaults::toggleSuperSlowModeKey;
        this->toggleHudKey = ConfigDefaults::toggleHudKey;

        // tuning values
        this->cameraPosIncDecValue = ConfigDefaults::cameraPosIncDecValue;
        this->cameraAngleIncDecValue = ConfigDefaults::cameraAngleIncDecValue;
        this->cameraFovIncDecValue = ConfigDefaults::cameraFovIncDecValue;

        this->cameraMinFov = ConfigDefaults::cameraMinFov;
        this->cameraMaxFov = ConfigDefaults::cameraMaxFov;
        this->cameraReferenceFov = ConfigDefaults::cameraReferenceFov;
        this->cameraSensitivityScale = ConfigDefaults::cameraSensitivityScale;

        this->superSlowModeTimeFactor = ConfigDefaults::superSlowModeTimeFactor;

        // behaviors
        this->photoModeDisableHudOnEnter = ConfigDefaults::photoModeDisableHudOnEnter;
        this->photoModeRestoreTimeFactorOnExit = ConfigDefaults::photoModeRestoreTimeFactorOnExit;

        // gun hotkeys
        this->gunSelectBloodShotKey = ConfigDefaults::gunSelectBloodShotKey;
        this->gunSelectBloodStreamKey = ConfigDefaults::gunSelectBloodStreamKey;
        this->gunSelectBloodSprayKey = ConfigDefaults::gunSelectBloodSprayKey;
        this->gunSelectBloodBombKey = ConfigDefaults::gunSelectBloodBombKey;
        this->gunSelectBloodFlameKey = ConfigDefaults::gunSelectBloodFlameKey;
        this->gunSelectBloodHammerKey = ConfigDefaults::gunSelectBloodHammerKey;
        this->gunSelectPreviousWeaponKey = ConfigDefaults::gunSelectPreviousWeaponKey;
        this->gunMouseWheelDownPreviousWeapon = ConfigDefaults::gunMouseWheelDownPreviousWeapon;

        // gun balance - BloodShot
        this->gunBloodShotAmmoCostLvl1 = ConfigDefaults::gunBloodShotAmmoCostLvl1;
        this->gunBloodShotAmmoCostLvl2 = ConfigDefaults::gunBloodShotAmmoCostLvl2;
        this->gunBloodShotAmmoCostLvl3 = ConfigDefaults::gunBloodShotAmmoCostLvl3;
        this->gunBloodShotDamageLvl1 = ConfigDefaults::gunBloodShotDamageLvl1;
        this->gunBloodShotDamageLvl2 = ConfigDefaults::gunBloodShotDamageLvl2;
        this->gunBloodShotDamageLvl3 = ConfigDefaults::gunBloodShotDamageLvl3;
        this->gunBloodShotFireRateLvl1 = ConfigDefaults::gunBloodShotFireRateLvl1;
        this->gunBloodShotFireRateLvl2 = ConfigDefaults::gunBloodShotFireRateLvl2;
        this->gunBloodShotFireRateLvl3 = ConfigDefaults::gunBloodShotFireRateLvl3;
        this->gunBloodShotRange = ConfigDefaults::gunBloodShotRange;

        // gun balance - BloodStream
        this->gunBloodStreamAmmoCostLvl1 = ConfigDefaults::gunBloodStreamAmmoCostLvl1;
        this->gunBloodStreamAmmoCostLvl2 = ConfigDefaults::gunBloodStreamAmmoCostLvl2;
        this->gunBloodStreamAmmoCostLvl3 = ConfigDefaults::gunBloodStreamAmmoCostLvl3;
        this->gunBloodStreamDamageLvl1 = ConfigDefaults::gunBloodStreamDamageLvl1;
        this->gunBloodStreamDamageLvl2 = ConfigDefaults::gunBloodStreamDamageLvl2;
        this->gunBloodStreamDamageLvl3 = ConfigDefaults::gunBloodStreamDamageLvl3;
        this->gunBloodStreamFireRateLvl1 = ConfigDefaults::gunBloodStreamFireRateLvl1;
        this->gunBloodStreamFireRateLvl2 = ConfigDefaults::gunBloodStreamFireRateLvl2;
        this->gunBloodStreamFireRateLvl3 = ConfigDefaults::gunBloodStreamFireRateLvl3;
        this->gunBloodStreamRange = ConfigDefaults::gunBloodStreamRange;

        // gun balance - BloodSpray
        this->gunBloodSprayAmmoCostLvl1 = ConfigDefaults::gunBloodSprayAmmoCostLvl1;
        this->gunBloodSprayAmmoCostLvl2 = ConfigDefaults::gunBloodSprayAmmoCostLvl2;
        this->gunBloodSprayAmmoCostLvl3 = ConfigDefaults::gunBloodSprayAmmoCostLvl3;
        this->gunBloodSprayDamageLvl1 = ConfigDefaults::gunBloodSprayDamageLvl1;
        this->gunBloodSprayDamageLvl2 = ConfigDefaults::gunBloodSprayDamageLvl2;
        this->gunBloodSprayDamageLvl3 = ConfigDefaults::gunBloodSprayDamageLvl3;
        this->gunBloodSprayFireRateLvl1 = ConfigDefaults::gunBloodSprayFireRateLvl1;
        this->gunBloodSprayFireRateLvl2 = ConfigDefaults::gunBloodSprayFireRateLvl2;
        this->gunBloodSprayFireRateLvl3 = ConfigDefaults::gunBloodSprayFireRateLvl3;
        this->gunBloodSprayRange = ConfigDefaults::gunBloodSprayRange;

        // gun balance - BloodBomb
        this->gunBloodBombAmmoCostLvl1 = ConfigDefaults::gunBloodBombAmmoCostLvl1;
        this->gunBloodBombAmmoCostLvl2 = ConfigDefaults::gunBloodBombAmmoCostLvl2;
        this->gunBloodBombAmmoCostLvl3 = ConfigDefaults::gunBloodBombAmmoCostLvl3;
        this->gunBloodBombDamageLvl1 = ConfigDefaults::gunBloodBombDamageLvl1;
        this->gunBloodBombDamageLvl2 = ConfigDefaults::gunBloodBombDamageLvl2;
        this->gunBloodBombDamageLvl3 = ConfigDefaults::gunBloodBombDamageLvl3;
        this->gunBloodBombFireRateLvl1 = ConfigDefaults::gunBloodBombFireRateLvl1;
        this->gunBloodBombFireRateLvl2 = ConfigDefaults::gunBloodBombFireRateLvl2;
        this->gunBloodBombFireRateLvl3 = ConfigDefaults::gunBloodBombFireRateLvl3;
        this->gunBloodBombRange = ConfigDefaults::gunBloodBombRange;

        // gun balance - BloodFlame
        this->gunBloodFlameAmmoCostLvl1 = ConfigDefaults::gunBloodFlameAmmoCostLvl1;
        this->gunBloodFlameAmmoCostLvl2 = ConfigDefaults::gunBloodFlameAmmoCostLvl2;
        this->gunBloodFlameAmmoCostLvl3 = ConfigDefaults::gunBloodFlameAmmoCostLvl3;
        this->gunBloodFlameDamageLvl1 = ConfigDefaults::gunBloodFlameDamageLvl1;
        this->gunBloodFlameDamageLvl2 = ConfigDefaults::gunBloodFlameDamageLvl2;
        this->gunBloodFlameDamageLvl3 = ConfigDefaults::gunBloodFlameDamageLvl3;
        this->gunBloodFlameFireRateLvl1 = ConfigDefaults::gunBloodFlameFireRateLvl1;
        this->gunBloodFlameFireRateLvl2 = ConfigDefaults::gunBloodFlameFireRateLvl2;
        this->gunBloodFlameFireRateLvl3 = ConfigDefaults::gunBloodFlameFireRateLvl3;
        this->gunBloodFlameRange = ConfigDefaults::gunBloodFlameRange;

        // gun balance - BloodHammer
        this->gunBloodHammerAmmoCostLvl1 = ConfigDefaults::gunBloodHammerAmmoCostLvl1;
        this->gunBloodHammerAmmoCostLvl2 = ConfigDefaults::gunBloodHammerAmmoCostLvl2;
        this->gunBloodHammerAmmoCostLvl3 = ConfigDefaults::gunBloodHammerAmmoCostLvl3;
        this->gunBloodHammerDamageLvl1 = ConfigDefaults::gunBloodHammerDamageLvl1;
        this->gunBloodHammerDamageLvl2 = ConfigDefaults::gunBloodHammerDamageLvl2;
        this->gunBloodHammerDamageLvl3 = ConfigDefaults::gunBloodHammerDamageLvl3;
        this->gunBloodHammerFireRateLvl1 = ConfigDefaults::gunBloodHammerFireRateLvl1;
        this->gunBloodHammerFireRateLvl2 = ConfigDefaults::gunBloodHammerFireRateLvl2;
        this->gunBloodHammerFireRateLvl3 = ConfigDefaults::gunBloodHammerFireRateLvl3;
        this->gunBloodHammerRange = ConfigDefaults::gunBloodHammerRange;

        // internal settings
        this->outfitMaxEntries = ConfigDefaults::outfitMaxEntries;
        this->outfitModPath = ConfigDefaults::outfitModPath;
    }

    // create gunsucked.ini if it doesn't exist, with default values
    bool writeDefaultConfig() {
        std::ofstream config("gunsucked.ini");

        if (!config.is_open()) {
            return false;  // failed to create
        }

        config << "; key bindings use Win32 virtual key codes" << std::endl;
        config << "; refer to https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes for valid keys" << std::endl;

        config << std::endl;

        config << "[features]" << std::endl;
        // write "true" or "false" to the ini, not "1" or "0"
        config << "enableGunBalance = " << (ConfigDefaults::enableGunBalance ? "true" : "false") << std::endl;
        config << "enableGunKeys = " << (ConfigDefaults::enableGunKeys ? "true" : "false") << std::endl;
        config << "enablePhotoMode = " << (ConfigDefaults::enablePhotoMode ? "true" : "false") << std::endl;
        config << "enableSuperSlowMo = " << (ConfigDefaults::enableSuperSlowMo ? "true" : "false") << std::endl;
        config << "enableNoHud = " << (ConfigDefaults::enableNoHud ? "true" : "false") << std::endl;
        config << "enableOutfitMods = " << (ConfigDefaults::enableOutfitMods ? "true" : "false") << std::endl;

        config << std::endl;

        config << "[photoMode]" << std::endl;

        config << "cameraDecrementXKey = 0x" << std::hex << ConfigDefaults::cameraDecrementXKey << std::endl;
        config << "cameraIncrementXKey = 0x" << std::hex << ConfigDefaults::cameraIncrementXKey << std::endl;

        config << "cameraDecrementZKey = 0x" << std::hex << ConfigDefaults::cameraDecrementZKey << std::endl;
        config << "cameraIncrementZKey = 0x" << std::hex << ConfigDefaults::cameraIncrementZKey << std::endl;

        config << "cameraDecrementYKey = 0x" << std::hex << ConfigDefaults::cameraDecrementYKey << std::endl;
        config << "cameraIncrementYKey = 0x" << std::hex << ConfigDefaults::cameraIncrementYKey << std::endl;

        config << "cameraDecrementAnglePitchKey = 0x" << std::hex << ConfigDefaults::cameraDecrementAnglePitchKey << std::endl;
        config << "cameraIncrementAnglePitchKey = 0x" << std::hex << ConfigDefaults::cameraIncrementAnglePitchKey << std::endl;

        config << "cameraDecrementAngleYawKey = 0x" << std::hex << ConfigDefaults::cameraDecrementAngleYawKey << std::endl;
        config << "cameraIncrementAngleYawKey = 0x" << std::hex << ConfigDefaults::cameraIncrementAngleYawKey << std::endl;

        config << "cameraDecrementAngleRollKey = 0x" << std::hex << ConfigDefaults::cameraDecrementAngleRollKey << std::endl;
        config << "cameraIncrementAngleRollKey = 0x" << std::hex << ConfigDefaults::cameraIncrementAngleRollKey << std::endl;

        config << "cameraDecrementFovKey = 0x" << std::hex << ConfigDefaults::cameraDecrementFovKey << std::endl;
        config << "cameraIncrementFovKey = 0x" << std::hex << ConfigDefaults::cameraIncrementFovKey << std::endl;

        config << std::endl;

        config << "[toggleKeys]" << std::endl;
        config << "togglePhotoModeKey = 0x" << std::hex << ConfigDefaults::togglePhotoModeKey << std::endl;
        config << "toggleSuperSlowModeKey = 0x" << std::hex << ConfigDefaults::toggleSuperSlowModeKey << std::endl;
        config << "toggleHudKey = 0x" << std::hex << ConfigDefaults::toggleHudKey << std::endl;

        config << std::endl;

        config << "[tuningValues]" << std::endl;
        config << "; increment/decrement values, ie, base camera sensitivity" << std::endl;
        config << "cameraPosIncDecValue = " << std::dec << ConfigDefaults::cameraPosIncDecValue << std::endl;
        config << "cameraAngleIncDecValue = " << std::dec << ConfigDefaults::cameraAngleIncDecValue << std::endl;
        config << "cameraFovIncDecValue = " << std::dec << ConfigDefaults::cameraFovIncDecValue << std::endl;

        config << std::endl;

        config << "; FoV values are inverted, ie, higher value = tighter field of view, lower value = wider" << std::endl;
        config << "cameraMinFov = " << std::dec << ConfigDefaults::cameraMinFov << std::endl;
        config << "cameraMaxFov = " << std::dec << ConfigDefaults::cameraMaxFov << std::endl;
        config << "; base fov value for scaling reference, ie fov scales above and below this value" << std::endl;
        config << "cameraReferenceFov = " << std::dec << ConfigDefaults::cameraReferenceFov << std::endl;
        config << "; sensitivity scaling factor, higher = faster scaling" << std::endl;
        config << "cameraSensitivityScale = " << std::dec << ConfigDefaults::cameraSensitivityScale << std::endl;

        config << std::endl;

        config << "; super slow mo time factor. in-game cheat menu uses 0.1-1.0" << std::endl;
        config << "; default is 0.05. values above 1.0 make the game run faster" << std::endl;
        config << "; don't use negative values. weird stuff happens" << std::endl;
        config << "superSlowModeTimeFactor = " << std::dec  << ConfigDefaults::superSlowModeTimeFactor << std::endl;

        config << std::endl;

        config << "[behaviors]" << std::endl;
        config << "photoModeDisableHudOnEnter = " << (ConfigDefaults::photoModeDisableHudOnEnter ? "true" : "false") << std::endl;
        config << "photoModeRestoreTimeFactorOnExit = " << (ConfigDefaults::photoModeRestoreTimeFactorOnExit ? "true" : "false") << std::endl;

        config << std::endl;

        config << "[gunHotkeys]" << std::endl;
        config << "gunSelectBloodShotKey = 0x" << std::hex << ConfigDefaults::gunSelectBloodShotKey << std::endl;
        config << "gunSelectBloodStreamKey = 0x" << std::hex << ConfigDefaults::gunSelectBloodStreamKey << std::endl;
        config << "gunSelectBloodSprayKey = 0x" << std::hex << ConfigDefaults::gunSelectBloodSprayKey << std::endl;
        config << "gunSelectBloodBombKey = 0x" << std::hex << ConfigDefaults::gunSelectBloodBombKey << std::endl;
        config << "gunSelectBloodFlameKey = 0x" << std::hex << ConfigDefaults::gunSelectBloodFlameKey << std::endl;
        config << "gunSelectBloodHammerKey = 0x" << std::hex << ConfigDefaults::gunSelectBloodHammerKey << std::endl;
        config << "gunSelectPreviousWeaponKey = 0x" << std::hex << ConfigDefaults::gunSelectPreviousWeaponKey << std::endl;
        config << "; if true, mousewheel down selects previous weapon mode" << std::endl;
        config << "gunMouseWheelDownPreviousWeapon = " << (ConfigDefaults::gunMouseWheelDownPreviousWeapon ? "true" : "false") << std::endl;

        config << std::endl;

        config << "[gunBalance]" << std::endl;
        config << "; Blood Shot" << std::endl;
        config << "gunBloodShotAmmoCostLvl1 = " << std::dec << ConfigDefaults::gunBloodShotAmmoCostLvl1 << std::endl;
        config << "gunBloodShotAmmoCostLvl2 = " << std::dec << ConfigDefaults::gunBloodShotAmmoCostLvl2 << std::endl;
        config << "gunBloodShotAmmoCostLvl3 = " << std::dec << ConfigDefaults::gunBloodShotAmmoCostLvl3 << std::endl;
        config << "gunBloodShotDamageLvl1 = " << std::dec << ConfigDefaults::gunBloodShotDamageLvl1 << std::endl;
        config << "gunBloodShotDamageLvl2 = " << std::dec << ConfigDefaults::gunBloodShotDamageLvl2 << std::endl;
        config << "gunBloodShotDamageLvl3 = " << std::dec << ConfigDefaults::gunBloodShotDamageLvl3 << std::endl;
        config << "gunBloodShotFireRateLvl1 = " << std::dec << ConfigDefaults::gunBloodShotFireRateLvl1 << std::endl;
        config << "gunBloodShotFireRateLvl2 = " << std::dec << ConfigDefaults::gunBloodShotFireRateLvl2 << std::endl;
        config << "gunBloodShotFireRateLvl3 = " << std::dec << ConfigDefaults::gunBloodShotFireRateLvl3 << std::endl;
        config << "gunBloodShotRange = " << std::dec << ConfigDefaults::gunBloodShotRange << std::endl;

        config << "; Blood Stream" << std::endl;
        config << "gunBloodStreamAmmoCostLvl1 = " << std::dec << ConfigDefaults::gunBloodStreamAmmoCostLvl1 << std::endl;
        config << "gunBloodStreamAmmoCostLvl2 = " << std::dec << ConfigDefaults::gunBloodStreamAmmoCostLvl2 << std::endl;
        config << "gunBloodStreamAmmoCostLvl3 = " << std::dec << ConfigDefaults::gunBloodStreamAmmoCostLvl3 << std::endl;
        config << "gunBloodStreamDamageLvl1 = " << std::dec << ConfigDefaults::gunBloodStreamDamageLvl1 << std::endl;
        config << "gunBloodStreamDamageLvl2 = " << std::dec << ConfigDefaults::gunBloodStreamDamageLvl2 << std::endl;
        config << "gunBloodStreamDamageLvl3 = " << std::dec << ConfigDefaults::gunBloodStreamDamageLvl3 << std::endl;
        config << "gunBloodStreamFireRateLvl1 = " << std::dec << ConfigDefaults::gunBloodStreamFireRateLvl1 << std::endl;
        config << "gunBloodStreamFireRateLvl2 = " << std::dec << ConfigDefaults::gunBloodStreamFireRateLvl2 << std::endl;
        config << "gunBloodStreamFireRateLvl3 = " << std::dec << ConfigDefaults::gunBloodStreamFireRateLvl3 << std::endl;
        config << "gunBloodStreamRange = " << std::dec << ConfigDefaults::gunBloodStreamRange << std::endl;

        config << "; Blood Spray" << std::endl;
        config << "gunBloodSprayAmmoCostLvl1 = " << std::dec << ConfigDefaults::gunBloodSprayAmmoCostLvl1 << std::endl;
        config << "gunBloodSprayAmmoCostLvl2 = " << std::dec << ConfigDefaults::gunBloodSprayAmmoCostLvl2 << std::endl;
        config << "gunBloodSprayAmmoCostLvl3 = " << std::dec << ConfigDefaults::gunBloodSprayAmmoCostLvl3 << std::endl;
        config << "gunBloodSprayDamageLvl1 = " << std::dec << ConfigDefaults::gunBloodSprayDamageLvl1 << std::endl;
        config << "gunBloodSprayDamageLvl2 = " << std::dec << ConfigDefaults::gunBloodSprayDamageLvl2 << std::endl;
        config << "gunBloodSprayDamageLvl3 = " << std::dec << ConfigDefaults::gunBloodSprayDamageLvl3 << std::endl;
        config << "gunBloodSprayFireRateLvl1 = " << std::dec << ConfigDefaults::gunBloodSprayFireRateLvl1 << std::endl;
        config << "gunBloodSprayFireRateLvl2 = " << std::dec << ConfigDefaults::gunBloodSprayFireRateLvl2 << std::endl;
        config << "gunBloodSprayFireRateLvl3 = " << std::dec << ConfigDefaults::gunBloodSprayFireRateLvl3 << std::endl;
        config << "gunBloodSprayRange = " << std::dec << ConfigDefaults::gunBloodSprayRange << std::endl;

        config << "; Blood Bomb" << std::endl;
        config << "gunBloodBombAmmoCostLvl1 = " << std::dec << ConfigDefaults::gunBloodBombAmmoCostLvl1 << std::endl;
        config << "gunBloodBombAmmoCostLvl2 = " << std::dec << ConfigDefaults::gunBloodBombAmmoCostLvl2 << std::endl;
        config << "gunBloodBombAmmoCostLvl3 = " << std::dec << ConfigDefaults::gunBloodBombAmmoCostLvl3 << std::endl;
        config << "gunBloodBombDamageLvl1 = " << std::dec << ConfigDefaults::gunBloodBombDamageLvl1 << std::endl;
        config << "gunBloodBombDamageLvl2 = " << std::dec << ConfigDefaults::gunBloodBombDamageLvl2 << std::endl;
        config << "gunBloodBombDamageLvl3 = " << std::dec << ConfigDefaults::gunBloodBombDamageLvl3 << std::endl;
        config << "gunBloodBombFireRateLvl1 = " << std::dec << ConfigDefaults::gunBloodBombFireRateLvl1 << std::endl;
        config << "gunBloodBombFireRateLvl2 = " << std::dec << ConfigDefaults::gunBloodBombFireRateLvl2 << std::endl;
        config << "gunBloodBombFireRateLvl3 = " << std::dec << ConfigDefaults::gunBloodBombFireRateLvl3 << std::endl;
        config << "gunBloodBombRange = " << std::dec << ConfigDefaults::gunBloodBombRange << std::endl;

        config << "; Blood Flame" << std::endl;
        config << "gunBloodFlameAmmoCostLvl1 = " << std::dec << ConfigDefaults::gunBloodFlameAmmoCostLvl1 << std::endl;
        config << "gunBloodFlameAmmoCostLvl2 = " << std::dec << ConfigDefaults::gunBloodFlameAmmoCostLvl2 << std::endl;
        config << "gunBloodFlameAmmoCostLvl3 = " << std::dec << ConfigDefaults::gunBloodFlameAmmoCostLvl3 << std::endl;
        config << "gunBloodFlameDamageLvl1 = " << std::dec << ConfigDefaults::gunBloodFlameDamageLvl1 << std::endl;
        config << "gunBloodFlameDamageLvl2 = " << std::dec << ConfigDefaults::gunBloodFlameDamageLvl2 << std::endl;
        config << "gunBloodFlameDamageLvl3 = " << std::dec << ConfigDefaults::gunBloodFlameDamageLvl3 << std::endl;
        config << "gunBloodFlameFireRateLvl1 = " << std::dec << ConfigDefaults::gunBloodFlameFireRateLvl1 << std::endl;
        config << "gunBloodFlameFireRateLvl2 = " << std::dec << ConfigDefaults::gunBloodFlameFireRateLvl2 << std::endl;
        config << "gunBloodFlameFireRateLvl3 = " << std::dec << ConfigDefaults::gunBloodFlameFireRateLvl3 << std::endl;
        config << "gunBloodFlameRange = " << std::dec << ConfigDefaults::gunBloodFlameRange << std::endl;

        config << "; Blood Hammer" << std::endl;
        config << "gunBloodHammerAmmoCostLvl1 = " << std::dec << ConfigDefaults::gunBloodHammerAmmoCostLvl1 << std::endl;
        config << "gunBloodHammerAmmoCostLvl2 = " << std::dec << ConfigDefaults::gunBloodHammerAmmoCostLvl2 << std::endl;
        config << "gunBloodHammerAmmoCostLvl3 = " << std::dec << ConfigDefaults::gunBloodHammerAmmoCostLvl3 << std::endl;
        config << "gunBloodHammerDamageLvl1 = " << std::dec << ConfigDefaults::gunBloodHammerDamageLvl1 << std::endl;
        config << "gunBloodHammerDamageLvl2 = " << std::dec << ConfigDefaults::gunBloodHammerDamageLvl2 << std::endl;
        config << "gunBloodHammerDamageLvl3 = " << std::dec << ConfigDefaults::gunBloodHammerDamageLvl3 << std::endl;
        config << "gunBloodHammerFireRateLvl1 = " << std::dec << ConfigDefaults::gunBloodHammerFireRateLvl1 << std::endl;
        config << "gunBloodHammerFireRateLvl2 = " << std::dec << ConfigDefaults::gunBloodHammerFireRateLvl2 << std::endl;
        config << "gunBloodHammerFireRateLvl3 = " << std::dec << ConfigDefaults::gunBloodHammerFireRateLvl3 << std::endl;
        config << "gunBloodHammerRange = " << std::dec << ConfigDefaults::gunBloodHammerRange << std::endl;

        config << std::endl;

        config << "[internalSettings]" << std::endl;
        config << "outfitMaxEntries = " << std::dec << ConfigDefaults::outfitMaxEntries << std::endl;
        config << "outfitModPath = " << ConfigDefaults::outfitModPath << std::endl;

        // write to gunsucked.ini
        config.close();
        return true;
    }

    // for testing if we really loaded values or not
    void logConfig() {
        DEBUG_LOG("cfg: gun balance " << this->enableGunBalance);
        DEBUG_LOG("cfg: gun keys " << this->enableGunKeys);
        DEBUG_LOG("cfg: photo mode " << this->enablePhotoMode);
        DEBUG_LOG("cfg: nohud " << this->enableNoHud);
        DEBUG_LOG("cfg: outfit mods " << this->enableOutfitMods);

        DEBUG_LOG("cfg: super slow time factor " << this->superSlowModeTimeFactor);

        DEBUG_LOG("cfg: outfit max entries " << this->outfitMaxEntries);
        DEBUG_LOG("cfg: outfit mod path " << this->outfitModPath);
    }
};

extern Config g_Config;