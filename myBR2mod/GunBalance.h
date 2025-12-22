#pragma once
#include <Windows.h>
#include "Config.h"
#include "MinHook.h"
#pragma comment(lib, "libMinHook.x86.lib")

// function signature for FUN_00585480
// which (I think) is the function that initializes gun properties
// we hook it and overwrite it with our rebalanced values.
typedef void(__fastcall* FN_SetGunProperties)(void* thisPointer);

// so if ZPunks (the mansion) have about 300 HP and Fpunks (meatpacking) have about 450...
// we'd want a basic pistol to blast an FPunk in like 6 shots. 
// default level 1 pistol would take 9! will have to buff dmg more than I thought

// Offsets within each weapon's data block (relative to weapon base)
namespace GunOffsets {
    constexpr uintptr_t AmmoCostLvl1 = 0x20;
    constexpr uintptr_t AmmoCostLvl2 = 0x24;
    constexpr uintptr_t AmmoCostLvl3 = 0x28;
    constexpr uintptr_t DamageLvl1 = 0x2C;
    constexpr uintptr_t DamageLvl2 = 0x30;
    constexpr uintptr_t DamageLvl3 = 0x34;
    constexpr uintptr_t FireRateLvl1 = 0x38;
    constexpr uintptr_t FireRateLvl2 = 0x3C;
    constexpr uintptr_t FireRateLvl3 = 0x40;
    constexpr uintptr_t Range = 0x50;
}

struct GunProperties {
    uintptr_t baseOffset;
    float ammoCostLvl1;
    float ammoCostLvl2;
    float ammoCostLvl3;
    float damageLvl1;
    float damageLvl2;
    float damageLvl3;
    float fireRateLvl1;
    float fireRateLvl2;
    float fireRateLvl3;
    float range;

    // Load values from config file/ this doesn't do anything yet
    void loadFromConfig(const char* section, const char* iniPath) {
        // load ini
        (void)section;
        (void)iniPath;
    }
};

// Total ammunition count is 1000 divided by ammo cost
// so, given a desired total ammo count (desiredPoolSize), calculate the cost
constexpr float costFromPool(float desiredPoolSize) {
    return 1000.0f / desiredPoolSize;
}

// Gun balance properties // // //

// Blood Shot
// should have utility throughout the game
// Most generous ammo pool 180/200/220
// slightly boosted damage and fire rate but not too crazy.
GunProperties bloodShot = {
    .baseOffset = 0x2BC,   // where data starts within CMultigun
    .ammoCostLvl1 = (costFromPool(180.0f)),    // default: 25
    .ammoCostLvl2 = (costFromPool(200.0f)),    // default: 18
    .ammoCostLvl3 = (costFromPool(220.0f)),    // default: 15
    .damageLvl1 = 75.0f,   // default: 50
    .damageLvl2 = 90.0f,   // default: 55
    .damageLvl3 = 112.5f,  // default: 60
    .fireRateLvl1 = 5.0f,    // default: 4
    .fireRateLvl2 = 5.5f,    // default: 5
    .fireRateLvl3 = 6.5f,    // default: 6
    .range = 120.0f   // default: 80
};

// Blood Stream
// Akimbo SMGs
// Should feel like the MAC-10s from Max Payne
// Very high DPS and limited ammo pool 90/120/150
// maxed SMGs should absolutely shred enemies
GunProperties bloodStream = {
    .baseOffset = 0x330,
    .ammoCostLvl1 = (costFromPool(90.0f)),   // default: 25
    .ammoCostLvl2 = (costFromPool(120.0f)),   // default: 22
    .ammoCostLvl3 = (costFromPool(150.0f)),   // default: 20
    .damageLvl1 = 75.0f,   // default: 50
    .damageLvl2 = 80.0f,   // default: 55
    .damageLvl3 = 85.0f,   // default: 60
    .fireRateLvl1 = 12.0f,   // default: 8
    .fireRateLvl2 = 16.0f,   // default: 10
    .fireRateLvl3 = 20.0f,   // default: 12
    .range = 100.0f   // default: 80
};

// Blood Spray
// ammo pool of 30/35/40. should be enough to have fun while not being too plentiful
// should feel like a Super Shotgun
// original damage was 13.3, 8.3, 10, probably a mistake
GunProperties bloodSpray = {
    .baseOffset = 0x3A4,
    .ammoCostLvl1 = (costFromPool(30.0f)),   // default: 100
    .ammoCostLvl2 = (costFromPool(35.0f)),   // default: 50
    .ammoCostLvl3 = (costFromPool(40.0f)),  // default: 50
    .damageLvl1 = 20.0f,    // default: 13.33 (per pellet?)
    .damageLvl2 = 33.0f,    // default: 8.33
    .damageLvl3 = 45.0f,    // default: 10
    .fireRateLvl1 = 1.5f,    // default: 1
    .fireRateLvl2 = 1.66f,   // default: 1
    .fireRateLvl3 = 1.75f,    // default: 1
    .range = 80.0f           // default: 80
};

// Blood Bomb
// conservative ammo pool 20/25/30
// Original damage decreased with level (?) probably mistake
// note: no idea how the damage calculations work on this weapon, so damage values are guesses
GunProperties bloodBomb = {
    .baseOffset = 0x418,
    .ammoCostLvl1 = (costFromPool(20.0f)),   // default: 200
    .ammoCostLvl2 = (costFromPool(25.0f)),   // default: 80
    .ammoCostLvl3 = (costFromPool(30.0f)),   // default: 67
    .damageLvl1 = 200.0f,    // default: 100
    .damageLvl2 = 275.0f,    // default: 50
    .damageLvl3 = 350.0f,    // default: 50
    .fireRateLvl1 = 1.0f,    // default: 1
    .fireRateLvl2 = 1.25f,   // default: 2
    .fireRateLvl3 = 1.5f,    // default: 1
    .range = 80.0f           // default: 50
};

// Blood Flame
// ammo 15/18/22
// another weapon where the damage decreased with level.
// again, don't know how damage is calculated; does it do tick damage or is it just instant + aoe?
GunProperties bloodFlame = {
    .baseOffset = 0x48C,
    .ammoCostLvl1 = (costFromPool(15.0f)),   // default: 200
    .ammoCostLvl2 = (costFromPool(18.0f)),   // default: 80
    .ammoCostLvl3 = (costFromPool(22.0f)),   // default: 67
    .damageLvl1 = 100.0f,     // default: 100
    .damageLvl2 = 120.0f,     // default: 50
    .damageLvl3 = 140.0f,     // default: 50
    .fireRateLvl1 = 1.0f,    // default: 1
    .fireRateLvl2 = 1.2f,    // default: 1
    .fireRateLvl3 = 1.4f,    // default: 1
    .range = 25.0f           // default: 10
};

// Blood Hammer
// ammo 12/14/16
GunProperties bloodHammer = {
    .baseOffset = 0x500,
    .ammoCostLvl1 = (costFromPool(12.0f)),  // default: 300
    .ammoCostLvl2 = (costFromPool(14.0f)),  // default: 131
    .ammoCostLvl3 = (costFromPool(16.0f)),  // default: 116
    .damageLvl1 = 400.0f,    // default: 300
    .damageLvl2 = 500.0f,    // default: 325
    .damageLvl3 = 600.0f,    // default: 350
    .fireRateLvl1 = 1.0f,   // default: 1
    .fireRateLvl2 = 1.1f,   // default: 1
    .fireRateLvl3 = 1.2f,    // default: 1
    .range = 250.0f          // default: 200
};

// collection to iterate over in writeGunProperties
std::vector<GunProperties> guns = { bloodShot, bloodStream, bloodSpray, bloodBomb, bloodFlame, bloodHammer };

// // // //

class GunBalanceHook {
private:
    uintptr_t targetFunctionAddress = 0x00585480;

    static inline FN_SetGunProperties originalFunction = nullptr;

    bool hookInstalled = false;
    bool hookEnabled = false;

    // must be static
    static void __fastcall hookedSetGunProperties(void* thisPointer) {

        originalFunction(thisPointer);

        for (auto& gun : guns) {
            writeGunProperties(thisPointer, gun);
        }
    }

    static void writeGunProperties(void* thisPointer, GunProperties& g) {

        uintptr_t weaponBaseAddress = (uintptr_t)thisPointer + g.baseOffset;

        float* target;

        target = (float*)(weaponBaseAddress + GunOffsets::AmmoCostLvl1);
        *target = g.ammoCostLvl1;
        target = (float*)(weaponBaseAddress + GunOffsets::AmmoCostLvl2);
        *target = g.ammoCostLvl2;
        target = (float*)(weaponBaseAddress + GunOffsets::AmmoCostLvl3);
        *target = g.ammoCostLvl3;

        target = (float*)(weaponBaseAddress + GunOffsets::DamageLvl1);
        *target = g.damageLvl1;
        target = (float*)(weaponBaseAddress + GunOffsets::DamageLvl2);
        *target = g.damageLvl2;
        target = (float*)(weaponBaseAddress + GunOffsets::DamageLvl3);
        *target = g.damageLvl3;

        target = (float*)(weaponBaseAddress + GunOffsets::FireRateLvl1);
        *target = g.fireRateLvl1;
        target = (float*)(weaponBaseAddress + GunOffsets::FireRateLvl2);
        *target = g.fireRateLvl2;
        target = (float*)(weaponBaseAddress + GunOffsets::FireRateLvl3);
        *target = g.fireRateLvl3;

        target = (float*)(weaponBaseAddress + GunOffsets::Range);
        *target = g.range;
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