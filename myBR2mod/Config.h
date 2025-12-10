#pragma once

#define DEBUG_CONSOLE_ENABLED true

#if DEBUG_CONSOLE_ENABLED
#include <iostream>
#define DEBUG_LOG(msg) std::cout << msg << std::endl
#else
#define DEBUG_LOG(msg) ((void)0)
#endif

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

// KEY BINDINGS -----------
// Camera position controls
#define DECREMENT_X_KEY VK_OEM_4      // [
#define INCREMENT_X_KEY VK_OEM_6      // ]
#define DECREMENT_Z_KEY VK_OEM_1      // ;
#define INCREMENT_Z_KEY VK_OEM_7      // '
#define DECREMENT_Y_KEY VK_OEM_COMMA  // ,
#define INCREMENT_Y_KEY VK_OEM_PERIOD // .

// FOV controls
#define DECREMENT_FOV_KEY VK_NEXT     // page down
#define INCREMENT_FOV_KEY VK_PRIOR    // page up

// Mode controls
#define CAMERA_CHANGE_MODE_KEY VK_OEM_PLUS // =+

// Photo mode toggle
#define TOGGLE_PHOTO_MODE_KEY VK_F7

// Debug
#define DEBUG_CHECK_KEY VK_F6
// --------------------------

// tuning values -------------
#define CAMERA_POS_INCREMENT_DECREMENT_VALUE 1.0f
#define FOV_INCREMENT_DECREMENT_VALUE 5.0f