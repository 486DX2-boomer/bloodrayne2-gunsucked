#include "pch.h"
#include "GamepadSupport.h"

std::array<std::atomic<DWORD>, 32> GamepadSupportHook::buttons{};
FN_GetButtonState GamepadSupportHook::originalFunction = nullptr;
std::array<std::atomic<bool>, 32> GamepadSupportHook::blockedButtons = {};
std::atomic<float> GamepadSupportHook::rightStickX = 0.0f;
std::atomic<float> GamepadSupportHook::rightStickY = 0.0f;
std::atomic<bool> GamepadSupportHook::rightStickBlocked = false;