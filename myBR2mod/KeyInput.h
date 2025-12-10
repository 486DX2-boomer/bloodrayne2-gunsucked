#pragma once
#include <Windows.h>

// input.h
#pragma once
#include <Windows.h>

class KeyInput {
private:
    int virtualKeyCode;
    bool previousState;
    bool isToggleKey;  // true = fires once per press, false = fires while held

public:
    KeyInput(int vkCode, bool toggle = true)
        : virtualKeyCode(vkCode), previousState(false), isToggleKey(toggle) {
    }

    // Call this once per frame to update state and check if key should trigger
    bool IsActivated() {
        bool currentState = (GetAsyncKeyState(virtualKeyCode) & 0x8000) != 0;
        bool shouldTrigger = false;

        if (isToggleKey) {
            // Toggle key: only trigger on transition from not-pressed to pressed
            if (currentState && !previousState) {
                shouldTrigger = true;
            }
        }
        else {
            // Continuous key: trigger whenever held down
            shouldTrigger = currentState;
        }

        previousState = currentState;
        return shouldTrigger;
    }

    // Reset state (useful if game loses focus)
    void Reset() {
        previousState = false;
    }
};