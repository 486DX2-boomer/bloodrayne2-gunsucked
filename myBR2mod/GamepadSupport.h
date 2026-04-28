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

// GetActionState
// every update, checks button press from data previously retrieved from XInputGetState
// button presses are translated to action state. Buttons can be rebound by the player,
// so actions are not necessarily bound to the same button

// TO DO:
// figure out how to wire control to photo mode.
// we could grab a direct reference to photo mode and call its methods (bad practice)
// or we could broadcast action state via public method
// We already have InputBase/KeyInput class
// We could define ControllerInput that grabs action state same as KeyInput
// and then photo mode (and in the future weapon wheel) control works just like how our key inputs already work

// from Ghidra:
// void __thiscall FUN_00467600(int param_1, int param_2, int param_3)
// first arg should be this, second arg should be action/button, 3rd should be pressed state

// __thiscall uses ECX for 'this', so we use __fastcall (ECX, EDX, then stack params)
typedef void(__fastcall* FN_GetActionState)(void* thisPointer, void* edx, int actionId, int pressed);

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
	static FN_GetActionState originalFunction;

	// the game rapidly emits pressed=0 between pressed=1 events, even while a button is held.
	// So for example, if you log out hookedGetActionState's pressed arg while playing, it looks like this
	// pressed = 1
	// pressed = 0
	// pressed = 0
	// pressed = 0
	// pressed = 1... and so on, even when the button is fully held.
	// so this makes it impossible to accurately read a press from the hook without processing.
	// we treat the action as pressed if pressed=1 was seen within the hold window.
	// in other words, the hold window "filters" the noisy action state.

	// the hold window needs to be tuned just right; if it's too short, our thread will see "phantom" presses when
	// pressed = 0 and pressed = 1 are read in succession.
	static constexpr DWORD holdWindowMilliseconds = 64; // just a little bit more than the 16 ms our thread runs on.

	// actions uses GetTickCount to store a timestamp corresponding to the last time an action
	// was emitted. Treat actions as "which actions are pressed right now", ignoring the additional 
	// complication of adding a grace period in order to reliably capture the actions.
	// array index maps to actionId.
	// We want it atomic because we are writing from the game's thread across to our thread.
	static std::array<std::atomic<DWORD>, 32> actions;

	// blocked action = intercept the controller input and nullify it; we "rebind" to our desired code.
	// ie, if Back button should toggle photo mode, then Back shouldn't open the Objectives menu.
	// we disable the Objectives menu action by zeroing it out in the hook.
	// just like actions, array index maps to actionId.
	// also cross-thread written and read, so the elements must be atomic.
	static std::array<std::atomic<bool>, 32> blockedActions;

	bool hookInstalled = false;
	bool hookEnabled = false;

	static void __fastcall hookedGetActionState(void* thisPointer, void* edx, int actionId, int pressed) {

		// to intercept and block actions, overwrite actionId/pressed here.
		if (blockedActions[actionId] == true) {
			originalFunction(thisPointer, edx, actionId, 0);
		}
		else {
			originalFunction(thisPointer, edx, actionId, pressed);
		}

		//originalFunction(thisPointer, edx, actionId, pressed);

		if (pressed && actionId >= 0 && actionId < 32) {
			actions[actionId].store(GetTickCount());
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
			(LPVOID)&hookedGetActionState,
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

	static bool getActionPressed(int actionId) {
		if (actionId < 0 || actionId >= 32) return false;
		DWORD last = actions[actionId].load();
		if (last == 0) return false;
		return (GetTickCount() - last) < holdWindowMilliseconds;
	}

	void registerBlockedAction(int actionId) {
		// don't register invalid actions
		if (actionId < 0 || actionId >= 32) {
			DEBUG_LOG("GamepadSupport: tried to register invalid blocked action: " << actionId);
			return;
		};
		blockedActions[actionId].store(true);
	}

	// I'm not sure there's anywhere in the mod that actually needs unregistering blocked actions.
	// can probably safely remove this.
	void unregisterBlockedAction(int actionId) {
		// don't unregister invalid actions
		if (actionId < 0 || actionId >= 32) {
			DEBUG_LOG("GamepadSupport: tried to unregister invalid blocked action: " << actionId);
			return;
		};
		// there's no check for registration state, meaning we can unregister or reregister
		// even if the action is already registered/unregistered.
		// But, I don't think that matters at all.
		blockedActions[actionId].store(false);
	}
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

	bool installHook() {
		return this->hook.install();
	}

	void update() {
		// capture locals that shadow the gamepad state
		// I did it the same way in photo mode; it's easier to work with local copies IMO.
		this->leftX = *Rayne2::GamepadThumbLeftX;
		this->leftY = *Rayne2::GamepadThumbLeftY;
		this->rightX = *Rayne2::GamepadThumbRightX;
		this->rightY = *Rayne2::GamepadThumbRightY;
		this->leftTrigger = *Rayne2::GamepadTriggerLeft;
		this->rightTrigger = *Rayne2::GamepadTriggerRight;

		// I can shadow getActionPressed state here locally as well, if I want.

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
	bool getActionPressed(int actionId) {
		// get action from hook and parse action id state
		return GamepadSupportHook::getActionPressed(actionId);
	}

	void registerBlockedAction(int actionId) {
		this->hook.registerBlockedAction(actionId);
	}

	void logAnalogState() {
		DEBUG_LOG(" leftX " << this->leftX << " left y " << this->leftY);
		DEBUG_LOG(" rightX " << this->rightX << " right y " << this->rightY);
		DEBUG_LOG(" left trigger" << this->leftTrigger);
		DEBUG_LOG(" right trigger" << this->rightTrigger);
	};
};