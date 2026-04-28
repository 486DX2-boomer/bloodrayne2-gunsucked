#pragma once

#include "InputBase.h"
#include "GamepadSupport.h"
#include <Windows.h>
#include <functional>

// now, in KeyInput, we bypassed the game's input reading and grabbed key state directly from the Win32 API
// that makes sense to create new keybindings that didn't correspond to any existing game controls

// for gamepad, there are no free buttons to use, so we want to be rebind and override the game's button handling.
// so we have to pass a pointer to the GamepadSupport object to get state (from the button state hook) 
// instead of getting raw XInput from this object.

// to do: gotta rename to GamepadButtonInput to differentiate this from GamepadThumbstickInput
class GamepadInput : public InputBase {
private:

    GamepadSupport* gamepad;
	bool previousState;
	bool isToggleButton; // true = fires once per press, false = fires while held

    int buttonId; // which button this input is bound to
    bool shouldBlock; // true = our input overrides the button read in the game's input handler
    // in other words, if we want the Back button to NOT bring up the objectives menu, set true.
    // if we want to bind some other logic to buttons without interrupting them, false. (for example, logging kicks, blades...)

    std::function<void()> callback; // callbacks were made optional on KeyInput for debugging or testing purposes but we pretty much always want them on a gamepad button

    // ignore input if rayne2.exe isn't focused.
    bool isGameFocused() {
        HWND foreground = GetForegroundWindow();
        if (foreground == nullptr) return false;

        DWORD foregroundPid;
        GetWindowThreadProcessId(foreground, &foregroundPid);

        return foregroundPid == GetCurrentProcessId();
    }

public:
    GamepadInput(GamepadSupport* gamepad, int buttonId, bool toggle, bool shouldBlock, std::function<void()> cb)
        : gamepad(gamepad), buttonId(buttonId), previousState(false), isToggleButton(toggle), shouldBlock(shouldBlock), callback(cb) 
    {
        if (shouldBlock) {
            gamepad->registerBlockedButton(buttonId);
            DEBUG_LOG("Gamepad: blocking button: " << buttonId);
        }
    }

    // call once per frame
    bool isActivated() {
        bool currentState = gamepad->getButtonPressed(this->buttonId);
        bool shouldTrigger = false;

        if (this->isToggleButton) {
            if (currentState && !this->previousState) {
                shouldTrigger = true;
            }
        }
        else {
            // continuous
            shouldTrigger = currentState;
        }

        this->previousState = currentState;

        //DEBUG_LOG("previousState: " << previousState);
        //DEBUG_LOG("currentState: " << currentState);
        //DEBUG_LOG("should trigger: " << shouldTrigger);
        return shouldTrigger;
    }

    void checkAndExecute() override {
        if (!this->isGameFocused()) {
            return;
        }

        if (this->isActivated() && this->callback != nullptr) {
            this->callback();
        }
    }
};