#pragma once
#include "Config.h"
#include "MinHook.h"
#pragma comment (lib, "libMinHook.x86.lib")

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
// aura vision (D-Left) 1c
// dilated perception (D-Right) 1d
// blood rage (D-Up) 1e
// powers oof (D-Down) 1f

// GetActionState
// every update, checks button press from data previously retrieved from XInputGetState
// button presses are translated to action state. Buttons can be rebound by the player,
// so actions are not necessarily bound to the same button

// TO DO:
// we are already able to log button presses, but there is no thumbstick data here
// we need to both find out how to get thumbstick data, and then figure out how to wire
// control to photo mode.
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

class GamepadSupportHook {
private:
	uintptr_t targetFunctionAddress = 0x00467600;

	static inline FN_GetActionState originalFunction = nullptr;

	bool hookInstalled = false;
	bool hookEnabled = false;

	static void __fastcall hookedGetButtonState(void* thisPointer, void* edx, int actionId, int pressed) {
		originalFunction(thisPointer, edx, actionId, pressed);

		if (pressed != 0) {
			DEBUG_LOG(actionId << " PRESSED");
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
};

class GamepadSupport {
private:
	GamepadSupportHook hook;
public:
	GamepadSupport() {};

	~GamepadSupport() {
		this->hook.uninstall();
	}

	bool installHook() {
		return this->hook.install();
	}
};