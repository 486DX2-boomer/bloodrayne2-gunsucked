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

    constexpr float superSlowModeTimeFactor = 0.05f;

    // behaviors
    constexpr bool photoModeDisableHudOnEnter = true;
    constexpr bool photoModeRestoreTimeFactorOnExit = true;

    // gun hotkeys
    constexpr int gunSelectBloodShotKey = 0x35;
    constexpr int gunSelectBloodStreamKey = 0x36;
    constexpr int gunSelectBloodSprayKey = 0x37;
    constexpr int gunSelectBloodBombKey = 0x38;
    constexpr int gunSelectBloodFlameKey = 0x39;
    constexpr int gunSelectBloodHammerKey = 0x30;
    constexpr int gunSelectPreviousWeaponKey = VK_OEM_MINUS;
    constexpr bool gunMouseWheelDownPreviousWeapon = true;

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
    int cameraIncrementAnglePitchkey;

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

    // internal stuff
    int outfitMaxEntries;
    std::string outfitModPath;

    Config() {
        // ensure initialized
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
            return false;
        }
        else if (error > 0) {
            DEBUG_LOG(reader.ParseErrorMessage());
            return false;
        }

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
        this->cameraIncrementAnglePitchkey = ConfigDefaults::cameraIncrementAnglePitchKey;

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
        config << "enableGunBalance = " << (ConfigDefaults::enableGunBalance ? "true" : "false") << std::endl;
        config << "enableGunKeys = " << (ConfigDefaults::enableGunKeys ? "true" : "false") << std::endl;
        config << "enablePhotoMode = " << (ConfigDefaults::enablePhotoMode ? "true" : "false") << std::endl;
        config << "enableSuperSlowMo = " << (ConfigDefaults::enableSuperSlowMo ? "true" : "false") << std::endl;
        config << "enableNoHud = " << (ConfigDefaults::enableNoHud ? "true" : "false") << std::endl;
        config << "enableOutfitMods = " << (ConfigDefaults::enableOutfitMods ? "true" : "false") << std::endl;

        config << std::endl;

        config << "[photoMode]" << std::endl;

        config << "decrementXKey = " << ConfigDefaults::cameraDecrementXKey << std::endl;
        config << "incrementXKey = " << ConfigDefaults::cameraIncrementXKey << std::endl;

        config << "decrementZKey = " << ConfigDefaults::cameraDecrementZKey << std::endl;
        config << "incrementZKey = " << ConfigDefaults::cameraIncrementZKey << std::endl;

        config << "decrementYKey = " << ConfigDefaults::cameraDecrementYKey << std::endl;
        config << "incrementYKey = " << ConfigDefaults::cameraIncrementYKey << std::endl;

        config << "decrementAnglePitchKey = " << ConfigDefaults::cameraDecrementAnglePitchKey << std::endl;
        config << "incrementAnglePitchKey = " << ConfigDefaults::cameraIncrementAnglePitchKey << std::endl;

        config << "decrementAngleYawKey = " << ConfigDefaults::cameraDecrementAngleYawKey << std::endl;
        config << "incrementAngleYawKey = " << ConfigDefaults::cameraIncrementAngleYawKey << std::endl;

        config << "decrementAngleRollKey = " << ConfigDefaults::cameraDecrementAngleRollKey << std::endl;
        config << "incrementAngleRollKey = " << ConfigDefaults::cameraIncrementAngleRollKey << std::endl;

        config << "decrementFovKey = " << ConfigDefaults::cameraDecrementFovKey << std::endl;
        config << "incrementFovKey = " << ConfigDefaults::cameraIncrementFovKey << std::endl;

        config << std::endl;

        config << "[toggleKeys]" << std::endl;
        config << "togglePhotoModeKey = " << ConfigDefaults::togglePhotoModeKey << std::endl;
        config << "toggleSuperSlowModeKey = " << ConfigDefaults::toggleSuperSlowModeKey << std::endl;
        config << "toggleHudKey = " << ConfigDefaults::toggleHudKey << std::endl;

        config << std::endl;

        config << "[tuningValues]" << std::endl;
        config << "; increment/decrement values, ie, camera sensitivity" << std::endl;
        config << "cameraPosIncDecValue = " << ConfigDefaults::cameraPosIncDecValue << std::endl;
        config << "cameraAngleIncDecValue = " << ConfigDefaults::cameraAngleIncDecValue << std::endl;
        config << "cameraFovIncDecValue = " << ConfigDefaults::cameraFovIncDecValue << std::endl;

        config << std::endl;

        config << "; super slow mo time factor. in-game cheat menu uses 0.1-1.0" << std::endl;
        config << "; default is 0.05. values above 1.0 make the game run faster" << std::endl;
        config << "; don't use negative values. weird stuff happens" << std::endl;
        config << "superSlowModeTimeFactor = " << ConfigDefaults::superSlowModeTimeFactor << std::endl;

        config << std::endl;

        config << "[behaviors]" << std::endl;
        config << "photoModeDisableHudOnEnter = " << (ConfigDefaults::photoModeDisableHudOnEnter ? "true" : "false") << std::endl;
        config << "photoModeRestoreTimeFactorOnExit = " << (ConfigDefaults::photoModeRestoreTimeFactorOnExit ? "true" : "false") << std::endl;

        config << std::endl;

        config << "[gunHotkeys]" << std::endl;
        config << "selectBloodShotKey = " << ConfigDefaults::gunSelectBloodShotKey << std::endl;
        config << "selectBloodStreamKey = " << ConfigDefaults::gunSelectBloodStreamKey << std::endl;
        config << "selectBloodSprayKey = " << ConfigDefaults::gunSelectBloodSprayKey << std::endl;
        config << "selectBloodBombKey = " << ConfigDefaults::gunSelectBloodBombKey << std::endl;
        config << "selectBloodFlameKey = " << ConfigDefaults::gunSelectBloodFlameKey << std::endl;
        config << "selectBloodHammerKey = " << ConfigDefaults::gunSelectBloodHammerKey << std::endl;
        config << "selectPreviousWeaponKey = " << ConfigDefaults::gunSelectPreviousWeaponKey << std::endl;
        config << "; if true, mousewheel down selects previous weapon mode" << std::endl;
        config << "mouseWheelDownPreviousWeapon = " << (ConfigDefaults::gunMouseWheelDownPreviousWeapon ? "true" : "false") << std::endl;

        config << std::endl;

        config << "[internalSettings]" << std::endl;
        config << "outfitMaxEntries = " << ConfigDefaults::outfitMaxEntries << std::endl;
        config << "outfitModPath = " << ConfigDefaults::outfitModPath << std::endl;

        // write to gunsucked.ini
        config.close();
        return true;
    }

    // just for testing if we really loaded values or not
    void logConfig() {
        DEBUG_LOG(this->enableGunBalance);
        DEBUG_LOG(this->enableGunKeys);
        DEBUG_LOG(this->enablePhotoMode);
        DEBUG_LOG(this->enableNoHud);
        DEBUG_LOG(this->enableOutfitMods);

        DEBUG_LOG(this->cameraDecrementXKey);
        DEBUG_LOG(this->togglePhotoModeKey);
        DEBUG_LOG(this->superSlowModeTimeFactor);
        
        DEBUG_LOG(this->photoModeDisableHudOnEnter);
        DEBUG_LOG(this->gunSelectBloodShotKey);
        DEBUG_LOG(this->outfitMaxEntries);
    }
};

