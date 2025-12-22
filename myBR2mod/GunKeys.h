#pragma once
#include <Windows.h>
#include "Config.h"

// equipped weapon index is stored at Rayne's object with offset of +1139C
// unsigned int* equippedWeaponIndex = (unsigned int*)(Rayne2::RayneBase + 0x1139C);

class GunKeys {
private:
	// unsigned int* currentWeapon = (unsigned int*)(rayneBase + 0x1139C);
	// we have to wait to assign this until after safety check
	unsigned int* currentWeapon;

	bool safe = false;

public:
	GunKeys(): 
		safe(false) {};
	~GunKeys() {};

	// 0-6
	enum WeaponModes {
		BloodShot,
		BloodStream,
		BloodSpray,
		BloodBomb,
		BloodFlame,
		BloodHammer
	};

	bool setRayneBase() {
		return false;
	}

	// don't allow use if Rayne pointer isn't valid
	bool isSafe() {
		return this->safe;
	}

	void checkSafe() {
		__try {
			// fine Rayne obj
			uintptr_t* rayneBasePtr = reinterpret_cast<uintptr_t*>(0x061276EC);
			uintptr_t rayneBase = *rayneBasePtr;

			if (rayneBase == 0) {
				this->safe = false;
				return;
			}

			// vftable check
			uintptr_t vftable = *reinterpret_cast<uintptr_t*>(rayneBase);

			if (vftable == 0x007034E4) {
				this->safe = true;
			}
			else {
				this->safe = false;
			}

			// set pointer
			if (this->safe) {
				this->currentWeapon = (unsigned int*)(rayneBase + 0x1139C);
			}
		}
		__except (EXCEPTION_EXECUTE_HANDLER) {
			DEBUG_LOG("Gunkeys: Exception during safety check");
			this->safe = false;
		}
	}

	void switchWeapon(unsigned int weaponMode) {
		if (this->isSafe()) {
		DEBUG_LOG("Switching weapon to " << weaponMode << " ");
		DEBUG_LOG(currentWeapon);
		*this->currentWeapon = weaponMode;
		}
	}
};