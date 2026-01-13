#pragma once

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