#pragma once

// ============================================
// Debug Configuration
// ============================================

#define DEBUG_CONSOLE_ENABLED true

#if DEBUG_CONSOLE_ENABLED
#include <iostream>
#define DEBUG_LOG(msg) std::cout << msg << std::endl
#else
#define DEBUG_LOG(msg) ((void)0)
#endif

// ============================================
// Game Addresses
// ============================================

namespace GameAddresses {
    // CGameView base address
    constexpr uintptr_t CameraBase = 0x05E34EE0;

    // CGameView pointers (XZY order)
    float* const CameraX = (float*)(CameraBase + 0x004);
    float* const CameraZ = (float*)(CameraBase + 0x008);
    float* const CameraY = (float*)(CameraBase + 0x00C);
    float* const CameraPitch = (float*)(CameraBase + 0x010);
    float* const CameraYaw = (float*)(CameraBase + 0x014);
    float* const CameraFOV = (float*)(CameraBase + 0x01C);
    int* const CameraMode = (int*)(CameraBase + 0x214);

    // Other game pointers
    bool* const PushCamera = (bool*)(0x05E3473D);
    float* const TimeFactor = (float*)(0x0619FB68);
}

// ============================================
// Key Bindings
// ============================================

// Camera position controls (XZY order)
#define DECREMENT_X_KEY VK_OEM_4      // [
#define INCREMENT_X_KEY VK_OEM_6      // ]
#define DECREMENT_Z_KEY VK_OEM_1      // ;
#define INCREMENT_Z_KEY VK_OEM_7      // '
#define DECREMENT_Y_KEY VK_OEM_COMMA  // ,
#define INCREMENT_Y_KEY VK_OEM_PERIOD // .

// Target position controls (XZY order)
#define DECREMENT_TARGET_X_KEY VK_LEFT
#define INCREMENT_TARGET_X_KEY VK_RIGHT
#define DECREMENT_TARGET_Z_KEY VK_DOWN  
#define INCREMENT_TARGET_Z_KEY VK_UP
#define DECREMENT_TARGET_Y_KEY VK_DELETE
#define INCREMENT_TARGET_Y_KEY VK_INSERT

// FOV controls
#define DECREMENT_FOV_KEY VK_NEXT     // page down
#define INCREMENT_FOV_KEY VK_PRIOR    // page up

// Photo mode toggle
#define TOGGLE_PHOTO_MODE_KEY VK_F7

// super slow mode
#define TOGGLE_SUPER_SLOW_MODE_KEY VK_F8

// Debug
#define DEBUG_CHECK_KEY VK_F6

// ============================================
// Tuning Values
// ============================================

#define CAMERA_POS_INCREMENT_DECREMENT_VALUE 0.1f
#define TARGET_POS_INCREMENT_DECREMENT_VALUE 0.1f
#define FOV_INCREMENT_DECREMENT_VALUE 1.0f

#define SUPER_SLOW_MODE_TIME_FACTOR 0.05f