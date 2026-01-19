#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <filesystem>

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

class Config {
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

        // if unsuccesful, fall back to defaults (should have already been set in constructor via setInternalDefaultConfig)
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

    // fallback for if loading .ini fails
    // so I kind of don't like this because there's no canonical default config source
    // any changes to config must be reflected in loadDefaultConfig and writeDefaultConfig
    // so maybe think about not using this and instead having a centralized place where the canon "Default config" comes from
    // maybe writeDefaultConfig should both write the .ini file and configure the object at the same time
    // actually I don't like that solution either
    // I'll think about this later
    void setInternalDefaultConfig() {
        // enable features
        this->enableGunBalance = true;
        this->enableGunKeys = true;
        this->enablePhotoMode = true;
        this->enableSuperSlowMo = true;
        this->enableNoHud = true;
        this->enableOutfitMods = true;

        // camera keybinds
        this->cameraDecrementXKey = VK_OEM_4;
        this->cameraIncrementXKey = VK_OEM_6;

        this->cameraDecrementZKey = VK_OEM_1;
        this->cameraIncrementZKey = VK_OEM_7;

        this->cameraDecrementYKey = VK_OEM_COMMA;
        this->cameraIncrementYKey = VK_OEM_PERIOD;

        this->cameraDecrementAnglePitchKey = VK_UP;
        this->cameraIncrementAnglePitchkey = VK_DOWN;

        this->cameraDecrementAngleYawKey = VK_LEFT;
        this->cameraIncrementAngleYawKey = VK_RIGHT;

        this->cameraDecrementAngleRollKey = VK_DELETE;
        this->cameraIncrementAngleRollKey = VK_INSERT;

        this->cameraDecrementFovKey = VK_NEXT;
        this->cameraIncrementFovKey = VK_PRIOR;

        // toggle keys
        this->togglePhotoModeKey = VK_F7;
        this->toggleSuperSlowModeKey = VK_F8;
        this->toggleHudKey = VK_F9;

        // tuning values
        this->cameraPosIncDecValue = 0.066f;
        this->cameraAngleIncDecValue = 0.016f;
        this->cameraFovIncDecValue = 1.0f;

        this->superSlowModeTimeFactor = 0.05f;

        // behaviors

        this->photoModeDisableHudOnEnter = true;
        this->photoModeRestoreTimeFactorOnExit = true;

        // gun hotkeys
        this->gunSelectBloodShotKey = 0x35;
        this->gunSelectBloodStreamKey = 0x36;
        this->gunSelectBloodSprayKey = 0x37;
        this->gunSelectBloodBombKey = 0x38;
        this->gunSelectBloodFlameKey = 0x39;
        this->gunSelectBloodHammerKey = 0x30;
        this->gunSelectPreviousWeaponKey = VK_OEM_MINUS;
        this->gunMouseWheelDownPreviousWeapon = true;

        // internal settings
        this->outfitMaxEntries = 1024;
        this->outfitModPath = "mods\\outfits";

    }

    bool writeDefaultConfig() {
        std::ofstream config("gunsucked.ini");

        if (!config.is_open()) {
            return false;  // failed to create
        }

        config << "; key bindings use Win32 virtual key codes" << std::endl;
        config << "; refer to https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes for valid keys" << std::endl;

        config << std::endl;

        config << "[features]" << std::endl;
        config << "enableGunBalance = true" << std::endl;
        config << "enableGunKeys = true" << std::endl;
        config << "enablePhotoMode = true" << std::endl;
        config << "enableSuperSlowMo = true" << std::endl;
        config << "enableNoHud = true" << std::endl;
        config << "enableOutfitMods = true" << std::endl;

        config << std::endl;

        config << "[photoMode]" << std::endl;

        config << "decrementXKey = VK_OEM_4" << std::endl;
        config << "incrementXKey = VK_OEM_6" << std::endl;

        config << "decrementZKey = VK_OEM_1" << std::endl;
        config << "incrementZKey = VK_OEM_7" << std::endl;

        config << "decrementYKey = VK_OEM_COMMA" << std::endl;
        config << "incrementYKey = VK_OEM_PERIOD" << std::endl;

        config << "decrementAnglePitchKey = VK_UP" << std::endl;
        config << "incrementAnglePitchKey = VK_DOWN" << std::endl;

        config << "decrementAngleYawKey = VK_LEFT" << std::endl;
        config << "incrementAngleYawKey = VK_RIGHT" << std::endl;

        config << "decrementAngleRollKey = VK_DELETE" << std::endl;
        config << "IncrementAngleRollKey = VK_INSERT" << std::endl;

        config << "decrementFovKey = VK_NEXT" << std::endl;
        config << "incrementFovKey = VK_PRIOR" << std::endl;

        config << std::endl;

        config << "[toggleKeys]" << std::endl;
        config << "togglePhotoModeKey = VK_F7" << std::endl;
        config << "toggleSuperSlowModeKey = VK_F8" << std::endl;
        config << "toggleHudKey = VK_F9" << std::endl;

        config << std::endl;

        config << "[tuningValues]" << std::endl;
        config << "; increment/decrement values, ie, camera sensitivity" << std::endl;
        config << "cameraPosIncDecValue = 0.066" << std::endl;
        config << "cameraAngleIncDecValue = 0.016" << std::endl;
        config << "cameraFovIncDecValue = 1.0" << std::endl;

        config << std::endl;

        config << "; super slow mo time factor. in-game cheat menu uses 0.1-1.0" << std::endl;
        config << "; default is 0.05. values above 1.0 make the game run faster" << std::endl;
        config << "; don't use negative values. weird stuff happens" << std::endl;
        config << "superSlowModeTimefactor = 0.05" << std::endl;

        config << std::endl;

        config << "[behaviors]" << std::endl;
        config << "photoModeDisableHudOnEnter = true" << std::endl;
        config << "photoModeRestoreTimeFactorOnExit = true" << std::endl;

        config << std::endl;

        config << "[gunHotkeys]" << std::endl;
        config << "selectBloodShotKey = 0x35" << std::endl;
        config << "selectBloodStreamKey = 0x36" << std::endl;
        config << "selectBloodSprayKey = 0x37" << std::endl;
        config << "selectBloodBombKey = 0x38" << std::endl;
        config << "selectBloodFlameKey = 0x39" << std::endl;
        config << "selectBloodHammerKey = 0x30" << std::endl;
        config << "selectPreviousWeaponKey = VK_OEM_MINUS" << std::endl;
        config << "; if true, mousewheel down selects previous weapon mode" << std::endl;
        config << "mouseWheelDownPreviousWeapon = true" << std::endl;

        config << std::endl;

        config << "[internalSettings]" << std::endl;
        config << "outfitMaxEntries = 1024" << std::endl;
        config << "outfitModPath = mods\\outfits" << std::endl;

        // write to gunsucked.ini
        config.close();
        return true;
    };
};

