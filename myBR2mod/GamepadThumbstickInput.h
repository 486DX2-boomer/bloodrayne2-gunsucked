#pragma once

#include "InputBase.h"
#include "GamepadSupport.h"
#include <Windows.h>
#include <functional>

// since thumbsticks are a straight read from getAnalogState, this class is pretty thin
// but we're still mirroring GamepadButtonInput for consistency's sake.
class GamepadThumbstickInput : public InputBase {
private:
	GamepadSupport* gamepad;

	GamepadSupport::Thumbstick thumbstick;
	GamepadSupport::Axis axis; // x or y

	std::function<void(float)> callback;


	bool isGameFocused() {
		HWND foreground = GetForegroundWindow();
		if (foreground == nullptr) return false;

		DWORD foregroundPid;
		GetWindowThreadProcessId(foreground, &foregroundPid);

		return foregroundPid == GetCurrentProcessId();	
	}

public:
	float magnitude;

	GamepadThumbstickInput(GamepadSupport* gamepad, GamepadSupport::Thumbstick thumbstick,
		GamepadSupport::Axis axis, std::function<void(float)> cb)
		: gamepad(gamepad), thumbstick(thumbstick), axis(axis), callback(cb), magnitude(0.0) {};

	void checkAndExecute() override {
		if (!this->isGameFocused()) {
			return;
		}

		AnalogState state = gamepad->getAnalogState();

		// update magnitude to reference in the callback.
		if (this->thumbstick == GamepadSupport::Thumbstick::LEFT) {
			if (this->axis == GamepadSupport::Axis::AXIS_X) {
				this->magnitude = state.leftX;
			}
			else {
				this->magnitude = state.leftY;
			}
		}

		if (this->thumbstick == GamepadSupport::Thumbstick::RIGHT) {
			if (this->axis == GamepadSupport::Axis::AXIS_X) {
				this->magnitude = state.rightX;
			}
			else {
				this->magnitude = state.rightY;
			}
		}

		this->callback(magnitude);
		//DEBUG_LOG("THUMBSTICK: " << std::dec << this->magnitude);
	}
};