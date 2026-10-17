#pragma once
#include "Config.h"
#include "MinHook.h"
#pragma comment (lib, "libMinHook.x86.lib")
#include <utility>
#include <array>
#include <atomic>

// action, default button, ID
// feed (Y) 0
// kick (B) 1
// jump (A) 2
// blades (X) 3
// switch target (LT) 4
// switch gun mode (RT) 5
// lock on (LB) 6
// shoot (RB) 7
// pause menu (Start) 8
// objectives (Back) 9
// aura vision (D-Left) 1c / 28
// dilated perception (D-Right) 1d / 29
// blood rage (D-Up) 1e / 30
// powers off (D-Down) 1f / 31

// TO DO:
// figure out how to wire control to photo mode.
// we could grab a direct reference to photo mode and call its methods (bad practice)
// or we could broadcast action state via public method
// We already have InputBase/KeyInput class
// We could define ControllerInput that grabs action state same as KeyInput
// and then photo mode (and in the future weapon wheel) control works just like how our key inputs already work

// from Ghidra:
// void __thiscall FUN_00467600(int param_1, int param_2, int param_3)
// first arg should be this, second arg should be button id, 3rd should be pressed state

// every update, checks button press from data retrieved from XInputGetState
// __thiscall uses ECX for 'this', so we use __fastcall (ECX, EDX, then stack params)
typedef void(__fastcall* FN_GetButtonState)(void* thisPointer, void* edx, int buttonId, int pressed);

typedef struct AnalogState {
	float leftX;
	float leftY;
	float rightX;
	float rightY;
	float leftTrigger;
	float rightTrigger;
};

class GamepadSupportHook {
private:

	uintptr_t targetFunctionAddress = 0x00467600;
	static FN_GetButtonState originalFunction;

	// the game rapidly emits pressed=0 between pressed=1 events, even while a button is held.
	// So for example, if you log out hookedGetButtonState's pressed arg while playing, it looks like this
	// pressed = 1
	// pressed = 0
	// pressed = 0
	// pressed = 0
	// pressed = 1... and so on, even when the button is fully held.
	// so this makes it impossible to accurately read a press from the hook without processing.
	// we treat the button as pressed if pressed=1 was seen within the hold window.
	// in other words, the hold window "filters" the noisy button state.

	// the hold window needs to be tuned just right; if it's too short, our thread will see "phantom" presses when
	// pressed = 0 and pressed = 1 are read in succession.
	static constexpr DWORD holdWindowMilliseconds = 64; // just a little bit more than the 16 ms our thread runs on.

	// buttons uses GetTickCount to store a timestamp corresponding to the last time a button
	// was emitted. Treat buttons as "which buttons are pressed right now", ignoring the additional 
	// complication of adding a grace period in order to reliably capture the inputs.
	// array index maps to buttonId.
	// We want it atomic because we are writing from the game's thread across to our thread.
	static std::array<std::atomic<DWORD>, 32> buttons;

	// blocked button = intercept the controller input and nullify it; we "rebind" to our desired code.
	// ie, if Back button should toggle photo mode, then Back shouldn't open the Objectives menu.
	// we disable the Objectives menu button by zeroing it out in the hook.
	// just like buttons, array index maps to buttonId.
	// also cross-thread written and read, so the elements must be atomic.
	static std::array<std::atomic<bool>, 32> blockedButtons;

	// thisPointer is the game's per-pad input state, 0x007E10A0 + pad * 0x1E4
	// (right stick X at +0x30, Y at +0x3C), so pad 0's are the Rayne2::GamepadThumbRight globals
	static constexpr uintptr_t padZeroState = 0x007E10A0;

	// we capture the right stick state this frame captured before we possibly zero it.
	// our thread reads these instead of the globals so blocking the stick doesn't block the mod too
	static std::atomic<float> rightStickX;
	static std::atomic<float> rightStickY;

	// blocked right stick = the game sees it centered (ie camera doesn't turn while the weapon wheel is open)
	static std::atomic<bool> rightStickBlocked;

	bool hookInstalled = false;
	bool hookEnabled = false;

	static void __fastcall hookedGetButtonState(void* thisPointer, void* edx, int buttonId, int pressed) {

		// both poll functions (DirectInput FUN_0068c550, XInput FUN_0068ca20) write every analog value
		// before they report any buttons, and report button 0 exactly once per poll (XInput reports it 4th, not 1st).
		// nothing reads the analog values until the poll returns, so this is our chance to see and zero the stick first.
		if (buttonId == 0 && reinterpret_cast<uintptr_t>(thisPointer) == padZeroState) {
			rightStickX.store(*Rayne2::GamepadThumbRightX);
			rightStickY.store(*Rayne2::GamepadThumbRightY);
			if (rightStickBlocked.load()) {
				*Rayne2::GamepadThumbRightX = 0.0f;
				*Rayne2::GamepadThumbRightY = 0.0f;
			}
		}

		// to intercept and block actions, overwrite buttonId/pressed here.
		if (blockedButtons[buttonId].load()) {
			originalFunction(thisPointer, edx, buttonId, 0);
		}
		else {
			originalFunction(thisPointer, edx, buttonId, pressed);
		}

		if (pressed && buttonId >= 0 && buttonId < 32) {
			buttons[buttonId].store(GetTickCount());
		}
	};

public:
	bool install() {
		if (this->hookInstalled) {
			DEBUG_LOG("Gamepad hook already installed");
			return true;
		}

		MH_STATUS status = MH_Initialize();
		if (status != MH_OK && status != MH_ERROR_ALREADY_INITIALIZED) {
			DEBUG_LOG("Gamepad hook failed: " << status);
			return false;
		}

		status = MH_CreateHook(
			(LPVOID)this->targetFunctionAddress,
			(LPVOID)&hookedGetButtonState,
			(LPVOID*)&originalFunction
		);

		if (status != MH_OK) {
			DEBUG_LOG("Gamepad support CreateHook failed: " << status);
			return false;
		}

		status = MH_EnableHook((LPVOID)this->targetFunctionAddress);
		if (status != MH_OK) {
			DEBUG_LOG("Gamepad support EnableHook failed: " << status);
			return false;
		}

		this->hookInstalled = true;
		this->hookEnabled = true;
		DEBUG_LOG("Gamepad support hook installed");
		return true;
	}

	void uninstall() {
		if (!this->hookInstalled) return;

		MH_DisableHook((LPVOID)this->targetFunctionAddress);
		MH_RemoveHook((LPVOID)this->targetFunctionAddress);
		MH_Uninitialize();

		this->hookInstalled = false;
		this->hookEnabled = false;
		originalFunction = nullptr;
		DEBUG_LOG("Gamepad support hook uninstalled");
	}

	static bool getButtonPressed(int buttonId) {
		if (buttonId < 0 || buttonId >= 32) return false;
		DWORD last = buttons[buttonId].load();
		if (last == 0) return false;
		return (GetTickCount() - last) < holdWindowMilliseconds;
	}

	void registerBlockedButton(int buttonId) {
		// don't register invalid buttons
		if (buttonId < 0 || buttonId >= 32) {
			DEBUG_LOG("GamepadSupport: tried to register invalid blocked button: " << buttonId);
			return;
		};
		blockedButtons[buttonId].store(true);
	}

	// unlike buttons the right stick is blocked and unblocked at runtime (only while the weapon wheel is open)
	void setRightStickBlocked(bool blocked) {
		rightStickBlocked.store(blocked);
	}

	static float getRightStickX() {
		return rightStickX.load();
	}

	static float getRightStickY() {
		return rightStickY.load();
	}

	// I'm not sure there's anywhere in the mod that actually needs unregistering blocked buttons.
	// can probably safely remove this.
	//void unregisterBlockedButton(int buttonId) {
	//	// don't unregister invalid buttons
	//	if (buttonId < 0 || buttonId >= 32) {
	//		DEBUG_LOG("GamepadSupport: tried to unregister invalid blocked button: " << buttonId);
	//		return;
	//	};
	//	// there's no check for registration state, meaning we can unregister or reregister
	//	// even if the action is already registered/unregistered.
	//	// But, I don't think that matters at all.
	//	blockedButtons[buttonId].store(false);
	//}
};

class GamepadSupport {
private:
	GamepadSupportHook hook;

	float leftX;
	float leftY;
	float rightX;
	float rightY;
	float leftTrigger;
	float rightTrigger;

public:
	GamepadSupport() : leftX(0.0), leftY(0.0), rightX(0.0), rightY(0.), leftTrigger(0.0), rightTrigger(0.0) {};

	~GamepadSupport() {
		this->hook.uninstall();
	}

	enum Button {
		Y = 0,
		B = 1,
		A = 2,
		X = 3,
		LT = 4,
		RT = 5,
		LB = 6,
		RB = 7,
		START = 8,
		BACK = 9,
		D_LEFT = 28,
		D_RIGHT = 29,
		D_UP = 30,
		D_DOWN = 31
	};

	enum Thumbstick {
		LEFT,
		RIGHT
	};

	// thumbstick axis
	enum Axis {
		AXIS_X, // named differently to not conflict with X/Y Button. Honestly, maybe these should be in a namespace not this class
		AXIS_Y
	};

	bool installHook() {
		return this->hook.install();
	}

	void update() {
		// capture locals that shadow the gamepad state
		// I did it the same way in photo mode; it's easier to work with local copies IMO.
		this->leftX = *Rayne2::GamepadThumbLeftX;
		this->leftY = *Rayne2::GamepadThumbLeftY;
		// the right stick comes from the hook, since the globals are zeroed while the stick is blocked
		this->rightX = GamepadSupportHook::getRightStickX();
		this->rightY = GamepadSupportHook::getRightStickY();
		this->leftTrigger = *Rayne2::GamepadTriggerLeft;
		this->rightTrigger = *Rayne2::GamepadTriggerRight;

		// I can shadow getButtonPressed state here locally as well, if I want.

		return;
	}

	AnalogState getAnalogState() {
		return {
			this->leftX,
			this->leftY,
			this->rightX,
			this->rightY,
			this->leftTrigger,
			this->rightTrigger
		};
	}

	// 0 if not pressed, 1 if pressed
	bool getButtonPressed(int buttonId) {
		return GamepadSupportHook::getButtonPressed(buttonId);
	}

	void registerBlockedButton(int buttonId) {
		this->hook.registerBlockedButton(buttonId);
	}

	// while blocked, the game sees the right stick as centered, but getAnalogState() still reports it
	void setRightStickBlocked(bool blocked) {
		this->hook.setRightStickBlocked(blocked);
	}

	void logAnalogState() {
		DEBUG_LOG(" leftX " << this->leftX << " left y " << this->leftY);
		DEBUG_LOG(" rightX " << this->rightX << " right y " << this->rightY);
		DEBUG_LOG(" left trigger" << this->leftTrigger);
		DEBUG_LOG(" right trigger" << this->rightTrigger);
	};
};