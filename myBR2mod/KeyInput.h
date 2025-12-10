#pragma once
#include <Windows.h>
#include <functional>

class KeyInput {
private:
    int virtualKeyCode;
    bool previousState;
    bool isToggleKey;  // true = fires once per press, false = fires while held
    std::function<void()> callback;  // Optional callback function

public:
    // Constructor without callback (original behavior)
    KeyInput(int vkCode, bool toggle = true)
        : virtualKeyCode(vkCode), previousState(false), isToggleKey(toggle), callback(nullptr) {
    }

    // Constructor with callback
    KeyInput(int vkCode, bool toggle, std::function<void()> cb)
        : virtualKeyCode(vkCode), previousState(false), isToggleKey(toggle), callback(cb) {
    }

    // Call this once per frame to update state and check if key should trigger
    bool IsActivated() {
        bool currentState = (GetAsyncKeyState(this->virtualKeyCode) & 0x8000) != 0;
        bool shouldTrigger = false;

        if (this->isToggleKey) {
            // Toggle key: only trigger on transition from not-pressed to pressed
            if (currentState && !this->previousState) {
                shouldTrigger = true;
            }
        }
        else {
            // Continuous key: trigger whenever held down
            shouldTrigger = currentState;
        }

        this->previousState = currentState;
        return shouldTrigger;
    }

    // Check activation and execute callback if bound
    void CheckAndExecute() {
        if (this->IsActivated() && this->callback != nullptr) {
            this->callback();
        }
    }

    // Set or change callback after construction
    void SetCallback(std::function<void()> cb) {
        this->callback = cb;
    }

    // Check if this input has a callback bound
    bool HasCallback() const {
        return this->callback != nullptr;
    }

    // Reset state (useful if game loses focus)
    void Reset() {
        this->previousState = false;
    }
};