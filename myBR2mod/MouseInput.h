#pragma once
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")
#include "Config.h"
#include "InputBase.h"

class MouseInput : public InputBase {
private:
    // we have pretty much HAVE to use DirectInput here
    // Win32 mouse events can't be captured without hooking a window
    // GameInput isn't compatible with anything before Win10 19H1
    // 
    IDirectInput8* directInput = nullptr;
    IDirectInputDevice8* mouse = nullptr;

    bool initialized = false;
    bool wheelDownTriggered = false;

    std::function<void()> callback;

public:
    MouseInput(std::function<void()> cb) : callback(cb) {}

    ~MouseInput() {
        if (this->mouse) {
            this->mouse->Unacquire();
            this->mouse->Release();
        }
        if (this->directInput) {
            this->directInput->Release();
        }
    }

    // call manually after instantiation
    bool initialize() {
        if (this->initialized) return true;

        HRESULT result = DirectInput8Create(
            GetModuleHandle(nullptr),
            DIRECTINPUT_VERSION,
            IID_IDirectInput8,
            (void**)&this->directInput,
            nullptr
        );

        if (FAILED(result)) {
            DEBUG_LOG("MouseHandler DirectInput8Create failed");
            return false;
        }

        result = this->directInput->CreateDevice(GUID_SysMouse, &this->mouse, nullptr);
        if (FAILED(result)) {
            DEBUG_LOG("MouseHandler CreateDevice failed");
            return false;
        }

        result = this->mouse->SetDataFormat(&c_dfDIMouse2);
        if (FAILED(result)) {
            DEBUG_LOG("MouseHandler SetDataFormat failed");
            return false;
        }

        result = this->mouse->Acquire();
        if (FAILED(result)) {
            DEBUG_LOG("MouseHandler Acquire failed");
            return false;
        }

        this->initialized = true;
        DEBUG_LOG("MouseHandler initialized");
        return true;
    }

    void poll() {
        if (!this->initialized) return;

        DIMOUSESTATE2 state;
        HRESULT result = this->mouse->GetDeviceState(sizeof(state), &state);

        if (result == DIERR_INPUTLOST || result == DIERR_NOTACQUIRED) {
            this->mouse->Acquire();
            return;
        }

        if (FAILED(result)) return;

        if (state.lZ < 0) {
            this->wheelDownTriggered = true;
        }
    }

    bool wasMouseWheelDown() {
        bool result = this->wheelDownTriggered;
        this->wheelDownTriggered = false;
        return result;
    }

    void checkAndExecute() override {
        if (!this->initialized) {
            return;
        }

        this->poll();
        if (this->wasMouseWheelDown()) {
            this->callback();
        }
    }
};