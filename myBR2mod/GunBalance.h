#pragma once
#include <Windows.h>
#include "Config.h"
#include "MinHook.h"
#pragma comment(lib, "libMinHook.x86.lib")

// function signature for FUN_00585480
// the function that initializes gun properties
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
};

// Total ammunition count is 1000 divided by ammo cost
// so, given a desired total ammo count (desiredPoolSize), calculate the cost
//constexpr float costFromPool(float desiredPoolSize) {
//    return 1000.0f / desiredPoolSize;
//}

// Blood Shot
// should have utility throughout the game
// Most generous ammo pool 180/200/220
// slightly boosted damage and fire rate but not too crazy.
GunProperties bloodShot = {
    .baseOffset = 0x2BC,   // where data starts within CMultigun
    .ammoCostLvl1 = ConfigDefaults::gunBloodShotAmmoCostLvl1,    // default: 25
    .ammoCostLvl2 = ConfigDefaults::gunBloodShotAmmoCostLvl2,    // default: 18
    .ammoCostLvl3 = ConfigDefaults::gunBloodShotAmmoCostLvl3,    // default: 15
    .damageLvl1 = ConfigDefaults::gunBloodShotDamageLvl1,   // default: 50
    .damageLvl2 = ConfigDefaults::gunBloodShotDamageLvl2,   // default: 55
    .damageLvl3 = ConfigDefaults::gunBloodShotDamageLvl3,  // default: 60
    .fireRateLvl1 = ConfigDefaults::gunBloodShotFireRateLvl1,    // default: 4
    .fireRateLvl2 = ConfigDefaults::gunBloodShotFireRateLvl2,    // default: 5
    .fireRateLvl3 = ConfigDefaults::gunBloodShotFireRateLvl3,    // default: 6
    .range = ConfigDefaults::gunBloodShotRange   // default: 80
};

// Blood Stream
// Akimbo SMGs
// Should feel like the MAC-10s from Max Payne
// Very high DPS and limited ammo pool 90/120/150
// maxed SMGs should absolutely shred enemies
GunProperties bloodStream = {
    .baseOffset = 0x330,
    .ammoCostLvl1 = ConfigDefaults::gunBloodStreamAmmoCostLvl1,   // default: 25
    .ammoCostLvl2 = ConfigDefaults::gunBloodStreamAmmoCostLvl2,   // default: 22
    .ammoCostLvl3 = ConfigDefaults::gunBloodStreamAmmoCostLvl3,   // default: 20
    .damageLvl1 = ConfigDefaults::gunBloodStreamDamageLvl1,   // default: 50
    .damageLvl2 = ConfigDefaults::gunBloodStreamDamageLvl2,   // default: 55
    .damageLvl3 = ConfigDefaults::gunBloodStreamDamageLvl3,   // default: 60
    .fireRateLvl1 = ConfigDefaults::gunBloodStreamFireRateLvl1,   // default: 8
    .fireRateLvl2 = ConfigDefaults::gunBloodStreamFireRateLvl2,   // default: 10
    .fireRateLvl3 = ConfigDefaults::gunBloodStreamFireRateLvl3,   // default: 12
    .range = ConfigDefaults::gunBloodStreamRange   // default: 80
};

// Blood Spray
// ammo pool of 30/35/40. should be enough to have fun while not being too plentiful
// should feel like a Super Shotgun
// original damage was 13.3, 8.3, 10, probably a mistake
GunProperties bloodSpray = {
    .baseOffset = 0x3A4,
    .ammoCostLvl1 = ConfigDefaults::gunBloodSprayAmmoCostLvl1,   // default: 100
    .ammoCostLvl2 = ConfigDefaults::gunBloodSprayAmmoCostLvl2,   // default: 50
    .ammoCostLvl3 = ConfigDefaults::gunBloodSprayAmmoCostLvl3,  // default: 50
    .damageLvl1 = ConfigDefaults::gunBloodSprayDamageLvl1,    // default: 13.33 (per pellet?)
    .damageLvl2 = ConfigDefaults::gunBloodSprayDamageLvl2,    // default: 8.33
    .damageLvl3 = ConfigDefaults::gunBloodSprayDamageLvl3,    // default: 10
    .fireRateLvl1 = ConfigDefaults::gunBloodSprayFireRateLvl1,    // default: 1
    .fireRateLvl2 = ConfigDefaults::gunBloodSprayFireRateLvl2,   // default: 1
    .fireRateLvl3 = ConfigDefaults::gunBloodSprayFireRateLvl3,    // default: 1
    .range = ConfigDefaults::gunBloodSprayRange           // default: 80
};

// Blood Bomb
// conservative ammo pool 20/25/30
// Original damage decreased with level (?) probably mistake
// note: no idea how the damage calculations work on this weapon, so damage values are guesses
GunProperties bloodBomb = {
    .baseOffset = 0x418,
    .ammoCostLvl1 = ConfigDefaults::gunBloodBombAmmoCostLvl1,   // default: 200
    .ammoCostLvl2 = ConfigDefaults::gunBloodBombAmmoCostLvl2,   // default: 80
    .ammoCostLvl3 = ConfigDefaults::gunBloodBombAmmoCostLvl3,   // default: 67
    .damageLvl1 = ConfigDefaults::gunBloodBombDamageLvl1,    // default: 100
    .damageLvl2 = ConfigDefaults::gunBloodBombDamageLvl2,    // default: 50
    .damageLvl3 = ConfigDefaults::gunBloodBombDamageLvl3,    // default: 50
    .fireRateLvl1 = ConfigDefaults::gunBloodBombFireRateLvl1,    // default: 1
    .fireRateLvl2 = ConfigDefaults::gunBloodBombFireRateLvl2,   // default: 2
    .fireRateLvl3 = ConfigDefaults::gunBloodBombFireRateLvl3,    // default: 1
    .range = ConfigDefaults::gunBloodBombRange           // default: 50
};

// Blood Flame
// ammo 15/18/22
// another weapon where the damage decreased with level.
// again, don't know how damage is calculated; does it do tick damage or is it just instant + aoe?
GunProperties bloodFlame = {
    .baseOffset = 0x48C,
    .ammoCostLvl1 = ConfigDefaults::gunBloodFlameAmmoCostLvl1,   // default: 200
    .ammoCostLvl2 = ConfigDefaults::gunBloodFlameAmmoCostLvl2,   // default: 80
    .ammoCostLvl3 = ConfigDefaults::gunBloodFlameAmmoCostLvl3,   // default: 67
    .damageLvl1 = ConfigDefaults::gunBloodFlameDamageLvl1,     // default: 100
    .damageLvl2 = ConfigDefaults::gunBloodFlameDamageLvl2,     // default: 50
    .damageLvl3 = ConfigDefaults::gunBloodFlameDamageLvl3,     // default: 50
    .fireRateLvl1 = ConfigDefaults::gunBloodFlameFireRateLvl1,    // default: 1
    .fireRateLvl2 = ConfigDefaults::gunBloodFlameFireRateLvl2,    // default: 1
    .fireRateLvl3 = ConfigDefaults::gunBloodFlameFireRateLvl3,    // default: 1
    .range = ConfigDefaults::gunBloodFlameRange           // default: 10
};

// Blood Hammer
// ammo 12/14/16
GunProperties bloodHammer = {
    .baseOffset = 0x500,
    .ammoCostLvl1 = ConfigDefaults::gunBloodHammerAmmoCostLvl1,  // default: 300
    .ammoCostLvl2 = ConfigDefaults::gunBloodHammerAmmoCostLvl2,  // default: 131
    .ammoCostLvl3 = ConfigDefaults::gunBloodHammerAmmoCostLvl3,  // default: 116
    .damageLvl1 = ConfigDefaults::gunBloodHammerDamageLvl1,    // default: 300
    .damageLvl2 = ConfigDefaults::gunBloodHammerDamageLvl2,    // default: 325
    .damageLvl3 = ConfigDefaults::gunBloodHammerDamageLvl3,    // default: 350
    .fireRateLvl1 = ConfigDefaults::gunBloodHammerFireRateLvl1,   // default: 1
    .fireRateLvl2 = ConfigDefaults::gunBloodHammerFireRateLvl2,   // default: 1
    .fireRateLvl3 = ConfigDefaults::gunBloodHammerFireRateLvl3,    // default: 1
    .range = ConfigDefaults::gunBloodHammerRange          // default: 200
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

    // Load values from g_Config into the gun properties
    // Call after g_Config is loaded and before installing hook
    void loadFromConfig() {
        bloodShot.ammoCostLvl1 = g_Config.gunBloodShotAmmoCostLvl1;
        bloodShot.ammoCostLvl2 = g_Config.gunBloodShotAmmoCostLvl2;
        bloodShot.ammoCostLvl3 = g_Config.gunBloodShotAmmoCostLvl3;
        bloodShot.damageLvl1 = g_Config.gunBloodShotDamageLvl1;
        bloodShot.damageLvl2 = g_Config.gunBloodShotDamageLvl2;
        bloodShot.damageLvl3 = g_Config.gunBloodShotDamageLvl3;
        bloodShot.fireRateLvl1 = g_Config.gunBloodShotFireRateLvl1;
        bloodShot.fireRateLvl2 = g_Config.gunBloodShotFireRateLvl2;
        bloodShot.fireRateLvl3 = g_Config.gunBloodShotFireRateLvl3;
        bloodShot.range = g_Config.gunBloodShotRange;

        bloodStream.ammoCostLvl1 = g_Config.gunBloodStreamAmmoCostLvl1;
        bloodStream.ammoCostLvl2 = g_Config.gunBloodStreamAmmoCostLvl2;
        bloodStream.ammoCostLvl3 = g_Config.gunBloodStreamAmmoCostLvl3;
        bloodStream.damageLvl1 = g_Config.gunBloodStreamDamageLvl1;
        bloodStream.damageLvl2 = g_Config.gunBloodStreamDamageLvl2;
        bloodStream.damageLvl3 = g_Config.gunBloodStreamDamageLvl3;
        bloodStream.fireRateLvl1 = g_Config.gunBloodStreamFireRateLvl1;
        bloodStream.fireRateLvl2 = g_Config.gunBloodStreamFireRateLvl2;
        bloodStream.fireRateLvl3 = g_Config.gunBloodStreamFireRateLvl3;
        bloodStream.range = g_Config.gunBloodStreamRange;

        bloodSpray.ammoCostLvl1 = g_Config.gunBloodSprayAmmoCostLvl1;
        bloodSpray.ammoCostLvl2 = g_Config.gunBloodSprayAmmoCostLvl2;
        bloodSpray.ammoCostLvl3 = g_Config.gunBloodSprayAmmoCostLvl3;
        bloodSpray.damageLvl1 = g_Config.gunBloodSprayDamageLvl1;
        bloodSpray.damageLvl2 = g_Config.gunBloodSprayDamageLvl2;
        bloodSpray.damageLvl3 = g_Config.gunBloodSprayDamageLvl3;
        bloodSpray.fireRateLvl1 = g_Config.gunBloodSprayFireRateLvl1;
        bloodSpray.fireRateLvl2 = g_Config.gunBloodSprayFireRateLvl2;
        bloodSpray.fireRateLvl3 = g_Config.gunBloodSprayFireRateLvl3;
        bloodSpray.range = g_Config.gunBloodSprayRange;

        bloodBomb.ammoCostLvl1 = g_Config.gunBloodBombAmmoCostLvl1;
        bloodBomb.ammoCostLvl2 = g_Config.gunBloodBombAmmoCostLvl2;
        bloodBomb.ammoCostLvl3 = g_Config.gunBloodBombAmmoCostLvl3;
        bloodBomb.damageLvl1 = g_Config.gunBloodBombDamageLvl1;
        bloodBomb.damageLvl2 = g_Config.gunBloodBombDamageLvl2;
        bloodBomb.damageLvl3 = g_Config.gunBloodBombDamageLvl3;
        bloodBomb.fireRateLvl1 = g_Config.gunBloodBombFireRateLvl1;
        bloodBomb.fireRateLvl2 = g_Config.gunBloodBombFireRateLvl2;
        bloodBomb.fireRateLvl3 = g_Config.gunBloodBombFireRateLvl3;
        bloodBomb.range = g_Config.gunBloodBombRange;

        bloodFlame.ammoCostLvl1 = g_Config.gunBloodFlameAmmoCostLvl1;
        bloodFlame.ammoCostLvl2 = g_Config.gunBloodFlameAmmoCostLvl2;
        bloodFlame.ammoCostLvl3 = g_Config.gunBloodFlameAmmoCostLvl3;
        bloodFlame.damageLvl1 = g_Config.gunBloodFlameDamageLvl1;
        bloodFlame.damageLvl2 = g_Config.gunBloodFlameDamageLvl2;
        bloodFlame.damageLvl3 = g_Config.gunBloodFlameDamageLvl3;
        bloodFlame.fireRateLvl1 = g_Config.gunBloodFlameFireRateLvl1;
        bloodFlame.fireRateLvl2 = g_Config.gunBloodFlameFireRateLvl2;
        bloodFlame.fireRateLvl3 = g_Config.gunBloodFlameFireRateLvl3;
        bloodFlame.range = g_Config.gunBloodFlameRange;

        bloodHammer.ammoCostLvl1 = g_Config.gunBloodHammerAmmoCostLvl1;
        bloodHammer.ammoCostLvl2 = g_Config.gunBloodHammerAmmoCostLvl2;
        bloodHammer.ammoCostLvl3 = g_Config.gunBloodHammerAmmoCostLvl3;
        bloodHammer.damageLvl1 = g_Config.gunBloodHammerDamageLvl1;
        bloodHammer.damageLvl2 = g_Config.gunBloodHammerDamageLvl2;
        bloodHammer.damageLvl3 = g_Config.gunBloodHammerDamageLvl3;
        bloodHammer.fireRateLvl1 = g_Config.gunBloodHammerFireRateLvl1;
        bloodHammer.fireRateLvl2 = g_Config.gunBloodHammerFireRateLvl2;
        bloodHammer.fireRateLvl3 = g_Config.gunBloodHammerFireRateLvl3;
        bloodHammer.range = g_Config.gunBloodHammerRange;

        // We have to update the guns vector because they don't mutate in place
        guns = { bloodShot, bloodStream, bloodSpray, bloodBomb, bloodFlame, bloodHammer };
    }

    bool installHook() {
        return this->hook.install();
    }
};