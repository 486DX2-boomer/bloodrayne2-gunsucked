#pragma once
#include <Windows.h>
#include "Config.h"
#include "MinHook.h"
#pragma comment(lib, "libMinHook.x86.lib")

// function signature for FUN_00585480
// which (I think) is the function that initializes gun properties
// we hook it and overwrite it with our rebalanced values.
typedef void(__fastcall* FN_SetGunProperties)(void* thisPointer);

// This is really not an elegant solution for this. We'll change this to a class
// so if ZPunks (the mansion) have about 300 HP and Fpunks (meatpacking) have about 450...
// we'd want a basic pistol to blast an FPunk in like 6 shots. 
// default level 1 pistol would take 9! will have to buff dmg more than I thought
namespace Guns {
    namespace BloodShot {
        // Bloodshot
        // should have utility throughout the game
        // Most generous ammo pool 180/200/220
        // slightly boosted damage and fire rate but not too crazy.
        uintptr_t BaseOffset = 0x2BC; // where data starts within CMultigun
        float AmmoCostLvl1 = 5.5f; // default 25
        float AmmoCostLvl2 = 5.0f; // default 18
        float AmmoCostLvl3 = 4.5f; // default 15
        float DamageLvl1 = 75.0; // default 50
        float DamageLvl2 = 90.0f; // default 55
        float DamageLvl3 = 112.5f; // default 60
        float FireRateLvl1 = 5.0f; // default 4
        float FireRateLvl2 = 6.0f; // default 5
        float FireRateLvl3 = 7.0f; // default 6
        float Range = 120.0f; // default 80
    }

    namespace BloodStream {
        // Akimbo SMGs
        // Should feel like the MAC-10s from Max Payne
        // Very high DPS and limited ammo pool 90/120/150
        // maxed SMGs should absolutely shred enemies
        uintptr_t BaseOffset = 0x330;
        float AmmoCostLvl1 = 11.0f; // default 25
        float AmmoCostLvl2 = 8.33f; // default 22
        float AmmoCostLvl3 = 6.67f; // default 20
        float DamageLvl1 = 75.0f; // default 50
        float DamageLvl2 = 80.0f; // default 55
        float DamageLvl3 = 85.0f; // default 60
        float FireRateLvl1 = 12.0f; // default 8
        float FireRateLvl2 = 16.0f; // default 10
        float FireRateLvl3 = 20.0f; // default 12
        float Range = 100.0f; // default 80
    }

    namespace Offsets {
        uintptr_t AmmoLvl1 = 0x20;
        uintptr_t AmmoLvl2 = 0x24;
        uintptr_t AmmoLvl3 = 0x28;
        uintptr_t DamageLvl1 = 0x2C;
        uintptr_t DamageLvl2 = 0x30;
        uintptr_t DamageLvl3 = 0x34;
        uintptr_t FireRateLvl1 = 0x38;
        uintptr_t FireRateLvl2 = 0x3C;
        uintptr_t FireRateLvl3 = 0x40;
        uintptr_t Range = 0x50;
    }
}

class GunBalanceHook {
private:
    uintptr_t targetFunctionAddress = 0x00585480;

    static inline FN_SetGunProperties originalFunction = nullptr;

    bool hookInstalled = false;
    bool hookEnabled = false;

    static void overrideFloat(void* base, uintptr_t offset, float value) {
        float* target = (float*)((uintptr_t)base + offset);
        *target = value;
    }

    // must be static
    static void __fastcall hookedSetGunProperties(void* thisPointer) {

        originalFunction(thisPointer);

        uintptr_t bloodShotBase = (uintptr_t)thisPointer + Guns::BloodShot::BaseOffset;
        overrideFloat((void*)bloodShotBase, Guns::Offsets::AmmoLvl1, Guns::BloodShot::AmmoCostLvl1);
        overrideFloat((void*)bloodShotBase, Guns::Offsets::AmmoLvl2, Guns::BloodShot::AmmoCostLvl2);
        overrideFloat((void*)bloodShotBase, Guns::Offsets::AmmoLvl3, Guns::BloodShot::AmmoCostLvl3);

        overrideFloat((void*)bloodShotBase, Guns::Offsets::DamageLvl1, Guns::BloodShot::DamageLvl1);
        overrideFloat((void*)bloodShotBase, Guns::Offsets::DamageLvl2, Guns::BloodShot::DamageLvl2);
        overrideFloat((void*)bloodShotBase, Guns::Offsets::DamageLvl3, Guns::BloodShot::DamageLvl3);

        overrideFloat((void*)bloodShotBase, Guns::Offsets::FireRateLvl1, Guns::BloodShot::FireRateLvl1);
        overrideFloat((void*)bloodShotBase, Guns::Offsets::FireRateLvl2, Guns::BloodShot::FireRateLvl2);
        overrideFloat((void*)bloodShotBase, Guns::Offsets::FireRateLvl3, Guns::BloodShot::FireRateLvl3);

        uintptr_t bloodStreamBase = (uintptr_t)thisPointer + Guns::BloodStream::BaseOffset;
        overrideFloat((void*)bloodStreamBase, Guns::Offsets::AmmoLvl1, Guns::BloodStream::AmmoCostLvl1);
        overrideFloat((void*)bloodStreamBase, Guns::Offsets::AmmoLvl2, Guns::BloodStream::AmmoCostLvl2);
        overrideFloat((void*)bloodStreamBase, Guns::Offsets::AmmoLvl3, Guns::BloodStream::AmmoCostLvl3);

        overrideFloat((void*)bloodStreamBase, Guns::Offsets::DamageLvl1, Guns::BloodStream::DamageLvl1);
        overrideFloat((void*)bloodStreamBase, Guns::Offsets::DamageLvl2, Guns::BloodStream::DamageLvl2);
        overrideFloat((void*)bloodStreamBase, Guns::Offsets::DamageLvl3, Guns::BloodStream::DamageLvl3);

        overrideFloat((void*)bloodStreamBase, Guns::Offsets::FireRateLvl1, Guns::BloodStream::FireRateLvl1);
        overrideFloat((void*)bloodStreamBase, Guns::Offsets::FireRateLvl2, Guns::BloodStream::FireRateLvl2);
        overrideFloat((void*)bloodStreamBase, Guns::Offsets::FireRateLvl3, Guns::BloodStream::FireRateLvl3);
    }

public:
    bool install() {
        if (this->hookInstalled) {
            DEBUG_LOG("Gunbalance hook already installed");
            return true;
        }

        MH_STATUS status = MH_Initialize();
        if (status != MH_OK && status != MH_ERROR_ALREADY_INITIALIZED) {
            DEBUG_LOG("Gunbalance hook failed: " << status);
            return false;
        }

        status = MH_CreateHook(
            (LPVOID)this->targetFunctionAddress,
            (LPVOID)&hookedSetGunProperties,
            (LPVOID*)&originalFunction
        );

        if (status != MH_OK) {
            DEBUG_LOG("Gunbalance CreateHook failed: " << status);
            return false;
        }

        status = MH_EnableHook((LPVOID)this->targetFunctionAddress);
        if (status != MH_OK) {
            DEBUG_LOG("Gunbalance EnableHook failed: " << status);
            return false;
        }

        this->hookInstalled = true;
        this->hookEnabled = true;
        DEBUG_LOG("Gunbalance hook installed at 0x" << std::hex << this->targetFunctionAddress);
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
        DEBUG_LOG("Gunbalance hook uninstalled");
    }
};

class GunBalance {
private:
    GunBalanceHook hook;
public:
    GunBalance() {};

    ~GunBalance() {
        this->hook.uninstall();
    }
    bool installHook() {
        return this->hook.install();
    }
};