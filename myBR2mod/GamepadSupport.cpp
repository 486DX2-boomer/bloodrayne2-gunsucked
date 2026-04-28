#include "pch.h"
#include "GamepadSupport.h"

std::array<std::atomic<DWORD>, 32> GamepadSupportHook::lastPressedTick{};
FN_GetActionState GamepadSupportHook::originalFunction = nullptr;