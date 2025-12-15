#pragma once
#include <Windows.h>
#include <functional>

class KeyInput {
private:
    int virtualKeyCode;
    bool previousState;
    bool isToggleKey;  // true = fires once per press, false = fires while held
    std::function<void()> callback;  // Optional

public:
    // no callback
    KeyInput(int vkCode, bool toggle = true)
        : virtualKeyCode(vkCode), previousState(false), isToggleKey(toggle), callback(nullptr) {
    }

    // with callback
    KeyInput(int vkCode, bool toggle, std::function<void()> cb)
        : virtualKeyCode(vkCode), previousState(false), isToggleKey(toggle), callback(cb) {
    }

    // call once per frame
    bool isActivated() {
        bool currentState = (GetAsyncKeyState(this->virtualKeyCode) & 0x8000) != 0;
        bool shouldTrigger = false;

        if (this->isToggleKey) {
            // Toggle check
            if (currentState && !this->previousState) {
                shouldTrigger = true;
            }
        }
        else {
            // Continuous
            shouldTrigger = currentState;
        }

        this->previousState = currentState;
        return shouldTrigger;
    }

    void checkAndExecute() {
        if (this->isActivated() && this->callback != nullptr) {
            this->callback();
        }
    }

    // change callback after construction
    void setCallback(std::function<void()> cb) {
        this->callback = cb;
    }

    bool hasCallback() const {
        return this->callback != nullptr;
    }

    // Reset state (for if game loses focus)
    void reset() {
        this->previousState = false;
    }
};