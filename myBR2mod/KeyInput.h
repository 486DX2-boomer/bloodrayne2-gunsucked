#pragma once
#include <Windows.h>
#include <functional>

class KeyInput {
private:
    int virtualKeyCode;
    bool previousState;
    bool isToggleKey;  // true = fires once per press, false = fires while held
    std::function<void()> callback;  // Optional

    // use to ignore input if rayne2.exe isn't active
    // So this fixes keybinds working when the game window isn't focused,
    // but it's still possible to trigger them when the game is paused.
    // I guess that's fine for photo mode but it's weird to be able to switch weapons when paused
    // it doesn't seem like super slow mo toggles when paused so the game's pause state isn't
    // based on time factor; should be possible to find where the game tracks
    // pause state and account for that too- if it's worth the effort
    bool isGameFocused() {
        HWND foreground = GetForegroundWindow();
        if (foreground == nullptr) return false;

        DWORD foregroundPid;
        GetWindowThreadProcessId(foreground, &foregroundPid);

        return foregroundPid == GetCurrentProcessId();
    }

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

        if (!this->isGameFocused()) { // ignore if window not active
            return;
        }

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