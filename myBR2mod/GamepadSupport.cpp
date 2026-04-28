#include "pch.h"
#include "GamepadSupport.h"

std::array<std::atomic<DWORD>, 32> GamepadSupportHook::buttons{};
FN_GetButtonState GamepadSupportHook::originalFunction = nullptr;
std::array<std::atomic<bool>, 32> GamepadSupportHook::blockedButtons = {};