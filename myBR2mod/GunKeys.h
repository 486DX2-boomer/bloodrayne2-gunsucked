#pragma once
#include <Windows.h>
#include "Config.h"

// equipped weapon index is stored at Rayne's object with offset of +1139C
// don't overwrite this value. bad things happen
// unsigned int* equippedWeaponIndex = (unsigned int*)(Rayne2::RayneBase + 0x1139C);

// function to call for weapon switch
typedef int(__thiscall* switchWeaponFn)(void* CMultigun, int weaponIndex, char playSound);

constexpr uintptr_t switchWeaponFunctionAddress = 0x005B5070;
constexpr uintptr_t multiGunOffset = 0x11380;

class GunKeys {
private:
	// unsigned int* currentWeapon = (unsigned int*)(rayneBase + 0x1139C);
	unsigned int* currentWeapon = 0;

	bool safe = false;

public:
	GunKeys() {};
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

	//bool setRayneBase() {
	//	return false;
	//}

	// the address gets lost every time a level or savegame loads;
	// easiest workaround is to run checkSafe again every time we switch weapons
	void resetCurrentWeaponAddress() {
		this->checkSafe();
	}

	// don't allow use if Rayne pointer isn't valid
	bool isSafe() {
		return this->safe;
	}

	void checkSafe() {
		__try {
			// find Rayne obj
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

	bool isWeaponUnlocked(unsigned int weaponIndex) {
		if (weaponIndex >= 6) return false;

		uintptr_t* unlockedWeaponObjPtr = reinterpret_cast<uintptr_t*>(Rayne2::UnlockedWeaponObjPtr);
		if (unlockedWeaponObjPtr == nullptr || *unlockedWeaponObjPtr == 0) return false;

		uintptr_t unlockedWeaponObj = *unlockedWeaponObjPtr;
		uint32_t bitmask = *reinterpret_cast<uint32_t*>(unlockedWeaponObj + Rayne2::UnlockedWeaponBitmaskOffset);

		// the game stores weapon unlock state as an 8 bit bitmask.
		// eg 00000000 = no weapons unlocked, 00000011 = bloodshot and bloodstream unlocked
		return (bitmask & (1 << weaponIndex)) != 0;
	}

	void switchWeapon(unsigned int weaponMode) {

		// ensure that the pointer is always valid
		this->resetCurrentWeaponAddress();

		if (this->isSafe()) {
		//DEBUG_LOG("Switching weapon to " << weaponMode << "at address " << this->currentWeapon);
		//DEBUG_LOG(currentWeapon);
		//*this->currentWeapon = weaponMode;

		uintptr_t rayneBase = *reinterpret_cast<uintptr_t*>(Rayne2::RayneBasePtr);
		if (rayneBase == 0) return;

		// make sure that the weapon is unlocked before equipping it.
		// otherwise, the function will allow us to switch to weapons we shouldn't have yet.
		if (!this->isWeaponUnlocked(weaponMode)) {
			// we don't have this weapon, so cancel
			DEBUG_LOG("Gunkeys: weapon mode " << weaponMode << " is not unlocked");
			return;
		}

		void* multiGun = reinterpret_cast<void*>(rayneBase + multiGunOffset);
		auto weaponSwitchCall = reinterpret_cast<switchWeaponFn>(switchWeaponFunctionAddress);
		int result = weaponSwitchCall(multiGun, weaponMode, 1);
		DEBUG_LOG("switch weapon to " << weaponMode);
		}
	}

	void switchToPreviousWeapon() {

		this->resetCurrentWeaponAddress();

		if (this->isSafe()) {
			// this is the same as switchWeapon, should extract this to a new function
			uintptr_t rayneBase = *reinterpret_cast<uintptr_t*>(Rayne2::RayneBasePtr);
			if (rayneBase == 0) return;

			void* multiGun = reinterpret_cast<void*>(rayneBase + multiGunOffset);
			auto weaponSwitchCall = reinterpret_cast<switchWeaponFn>(switchWeaponFunctionAddress);

			// reads previous weapon index from the game
			int result = weaponSwitchCall(multiGun, *Rayne2::previousWeaponIndex, 1);
		}
	}
};