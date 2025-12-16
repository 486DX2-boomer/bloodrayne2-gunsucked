#pragma once
#include <Windows.h>
#include "Config.h"
#include "Minhook.h"

// function signature for FUN_00592050
// which is the HUD update logic. we hook it and overwrite our value after it has completed
typedef void(__thiscall* FN_UpdateHud)(void* thisPointer, float deltaTime);

enum HudState {
	Off = 0,
	On = 1
};

class NoHudHook {
private:
	uintptr_t targetFunctionAddress = 0x00592050;

	static inline FN_UpdateHud originalFunction = nullptr;

	static inline bool overrideActive = false;

	bool hookInstalled = false;
	bool hookEnabled = false;

	static void __fastcall hookedNoHud(void* thisPointer, void* edx, float deltaTime) {
		originalFunction(thisPointer, deltaTime);
		if (overrideActive) {
			*Rayne2::DrawHud = HudState::Off;
		}
	}

public:
	bool install() {
		if (this->hookInstalled) {
			DEBUG_LOG("NoHud hook already installed");
			return true;
		}

		MH_STATUS status = MH_Initialize();
		if (status != MH_OK && status != MH_ERROR_ALREADY_INITIALIZED) {
			DEBUG_LOG("NoHud hook failed: " << status);
			return false;
		}

		status = MH_CreateHook(
			(LPVOID)this->targetFunctionAddress,
			(LPVOID)&hookedNoHud,
			(LPVOID*)&originalFunction
		);

		if (status != MH_OK) {
			DEBUG_LOG("NoHud CreateHook failed: " << status);
			return false;
		}

		status = MH_EnableHook((LPVOID)this->targetFunctionAddress);
		if (status != MH_OK) {
			DEBUG_LOG("NoHud EnableHook failed: " << status);
			return false;
		}

		this->hookInstalled = true;
		this->hookEnabled = true;
		DEBUG_LOG("NoHud hook installed at 0x" << std::hex << this->targetFunctionAddress);
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
		DEBUG_LOG("NoHud hook uninstalled");
	}

	void enableOverride() {
		this->overrideActive = true;
	}

	void disableOverride() {
		this->overrideActive = false;
	}
};

class NoHud {
private:
	NoHudHook hook;
	HudState hudState;

	// we must check to see if Rayne's vftable is loaded before we attempt to hook.
	bool safeToHook;

public:
	NoHud()
		: hudState(On), safeToHook(false)
	{
		
	}

	~NoHud() {
		if (this->hudState == HudState::Off) {
			this->hudState = HudState::On;
		}
		this->hook.uninstall();
	}

	void hudOn() {
		this->hudState = On;
		this->hook.disableOverride();
	}

	void hudOff() {
		this->hudState = Off;
		this->hook.enableOverride();
	}

	void toggle() {
		if (this->hudState == HudState::Off) {
			this->hudOn();
			this->hudState = HudState::On;
		} else {
			this->hudOff();
			this->hudState = HudState::Off;
		}
	}

	void checkSafeToHook() {
		__try {
			// Rayne's obj is dynamically allocated, so we have to follow the pointer.
			uintptr_t* rayneBasePtr = reinterpret_cast<uintptr_t*>(0x061276EC);
			uintptr_t rayneBase = *rayneBasePtr;
			DEBUG_LOG("[NoHud] rayneBasePtr value: 0x" << std::hex << rayneBase);


			if (rayneBase == 0) {
				this->safeToHook = false;
				DEBUG_LOG("Raynebase null");
				return;
			}

			// vftable should be present
			uintptr_t vftable = *reinterpret_cast<uintptr_t*>(rayneBase);
			DEBUG_LOG("[NoHud] vftable value: 0x" << std::hex << vftable << " (expected: 0x007034E4)");


			if (vftable == 0x007034E4) {
				this->safeToHook = true;
			}
			else {
				this->safeToHook = false;
			}
		}
		__except (EXCEPTION_EXECUTE_HANDLER) {
			DEBUG_LOG("[NoHud] Exception during safety check");
			this->safeToHook = false;
		}
	}

	bool isSafeToHook() {
		return this->safeToHook;
	}

	bool installHook() {
		if (!this->safeToHook) {
			DEBUG_LOG("NoHud: not safe to hook");
			return false;
		}
		return this->hook.install();
	}
};