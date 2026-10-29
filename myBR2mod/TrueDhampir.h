#pragma once
#include <Windows.h>
#include "Config.h"
#include "MinHook.h"
#pragma comment(lib, "libMinHook.x86.lib")

// True Dhampir mode: makes Rayne play more like BR1 Rayne.
// - immune to knockdown (hits, explosions and feed counters stagger her instead)
// - faster run
// - higher jump with a longer hang time
// - faster attacks (animations play faster, so combo windows open sooner too)
// - configurable airborne gravity (physics air states like the double jump roll. single jump ignores gravity)
// BR1 reference values are in reverse engineering notes.md

// FUN_00669A40: sets the animation playback rate (+0x48B0) for this frame. __thiscall, no args.
// it resets the rate to 1.0 and only lowers it to blend walk and run, but FUN_00672A30 calls it right before
// it multiplies the delta time for all of Rayne's animations by +0x48B0, so we can scale any animation here
typedef void(__fastcall* FN_SetAnimationRate)(void* thisPointer, void* edx);

// FUN_00672A30: Rayne's animation advance. __thiscall(float deltaTime).
// it accumulates this frame's root motion into +0x15C/160/164 and scales it right before returning
typedef void(__fastcall* FN_AdvanceAnimation)(void* thisPointer, void* edx, float deltaTime);

namespace TrueDhampirOffsets {
	// CMainCharacter
	constexpr uintptr_t AnimationIndex = 0x17C; // current animation, the "state" (CAnimationController + 0x24)
	constexpr uintptr_t RootMotionY = 0x160; // vertical root motion this frame
	constexpr uintptr_t AnimationRate = 0x48B0;

	// animation indices
	constexpr int Walk = 0x0A;
	constexpr int Run = 0x0B;
	constexpr int Jump = 0xFF; // single jump, driven entirely by the animation's root motion

	// attack animations, from startMove's move to animation table (FUN_006660F0).
	// each range includes the follow-up animations between move starts (recoveries, chained through animation data).
	// the variant ranges are the ones FUN_00666880 maps back onto the base ranges
	struct AnimationRange { int first; int last; };
	constexpr AnimationRange Attacks[] = {
		{ 0x1A, 0x2D },   // moves 0x09-0x14
		{ 0x43, 0x56 },   // variants of 0x1A-0x2D
		{ 0x57, 0x77 },   // moves 0x29-0x3C
		{ 0x7B, 0x98 },   // variants of 0x57-0x74
		{ 0x18D, 0x19F }, // moves 0x18-0x28
		{ 0x1B1, 0x1BC }, // variants of 0x18D-0x19A
		{ 0x1C5, 0x1C6 }, // Blood Rage blade and kick (moves 0x43, 0x44)
	};

	constexpr bool isAttack(int animation) {
		for (const auto& range : Attacks) {
			if (animation >= range.first && animation <= range.last) return true;
		}
		return false;
	}
}

// a code patch we verify before writing, so a different game build doesn't get garbage written into it
struct CodePatch {
	uintptr_t address;
	BYTE original[4];
	BYTE patched[4];
	size_t size;
	const char* name;
};

class TrueDhampirHook {
private:
	uintptr_t setAnimationRateAddress = 0x00669A40;
	uintptr_t advanceAnimationAddress = 0x00672A30;

	static inline FN_SetAnimationRate originalSetAnimationRate = nullptr;
	static inline FN_AdvanceAnimation originalAdvanceAnimation = nullptr;

	// read by the hooks on the game thread, so they have to be static
	static inline float runSpeedMultiplier = 1.0f;
	static inline float jumpHeightMultiplier = 1.0f;
	static inline float jumpAnimationRate = 1.0f;
	static inline float attackSpeedMultiplier = 1.0f;

	// Rayne's airborne gravity is world gravity times this. the game's own constant is 3.0 (double at 0x006DE838),
	// which is probably shared, so instead of changing it we point the one instruction that uses it for Rayne at ours.
	// FUN_00674230 at 0x006749E8: FMUL double ptr [0x006DE838] (DC 0D + address). the address starts at 0x006749EA
	static inline double gravityMultiplier = 3.0;
	CodePatch gravityPatch = { 0x006749EA, { 0x38, 0xE8, 0x6D, 0x00 }, {}, 4, "airborne gravity" };

	bool hooksInstalled = false;
	bool patchesApplied = false;
	bool gravityPatched = false;

	// the hit handler FUN_00551160 picks a reaction from a jump table on hit severity (1-5).
	// severity 4 (heavy stagger 0xA3-0xA6) chains into the down state through animation data, and 5 is the knockdown.
	// point both at the severity 3 code (medium stagger 0x9F-0xA2), which returns to idle.
	// covers melee, explosions and physics hits.
	// the feed counter (an enemy repelling a feed attempt) doesn't go through the hit handler.
	// it sets animation 0xF1 directly in two places (feed grab FUN_00663CC0 and anim event 0x645 in FUN_00672A30).
	// we push 0xA2 (medium stagger) instead. side effect: ninjas die instantly on a feed attempt. that's intended, they're annoying
	CodePatch patches[4] = {
		{ 0x00551B68, { 0xBD, 0x17, 0x55, 0x00 }, { 0x72, 0x17, 0x55, 0x00 }, 4, "severity 4 reaction" },
		{ 0x00551B6C, { 0x07, 0x18, 0x55, 0x00 }, { 0x72, 0x17, 0x55, 0x00 }, 4, "severity 5 reaction" },
		{ 0x00663EDA, { 0xF1 }, { 0xA2 }, 1, "feed grab counter" },
		{ 0x006738CF, { 0xF1 }, { 0xA2 }, 1, "feed event counter" },
	};

	static bool isRayne(void* thisPointer) {
		uintptr_t rayneBase = *reinterpret_cast<uintptr_t*>(Rayne2::RayneBasePtr);
		return rayneBase != 0 && reinterpret_cast<uintptr_t>(thisPointer) == rayneBase;
	}

	// speeds up the run cycle instead of scaling the distance, so her feet don't slide.
	// slowing the jump animation lengthens the hang time without changing the height,
	// since the animation still covers the same root motion in total.
	// speeding up attacks also moves their animation events earlier, including the combo window (event 0x4B1)
	static void __fastcall hookedSetAnimationRate(void* thisPointer, void* edx) {
		originalSetAnimationRate(thisPointer, edx);

		if (!isRayne(thisPointer)) return;

		uintptr_t rayne = reinterpret_cast<uintptr_t>(thisPointer);
		int animation = *reinterpret_cast<int*>(rayne + TrueDhampirOffsets::AnimationIndex);
		float* rate = reinterpret_cast<float*>(rayne + TrueDhampirOffsets::AnimationRate);

		if (animation == TrueDhampirOffsets::Walk || animation == TrueDhampirOffsets::Run) {
			*rate *= runSpeedMultiplier;
		}
		else if (animation == TrueDhampirOffsets::Jump) {
			*rate *= jumpAnimationRate;
		}
		else if (TrueDhampirOffsets::isAttack(animation)) {
			*rate *= attackSpeedMultiplier;
		}
	}

	// scales the jump's vertical root motion. timing is unchanged, she just goes higher
	static void __fastcall hookedAdvanceAnimation(void* thisPointer, void* edx, float deltaTime) {
		originalAdvanceAnimation(thisPointer, edx, deltaTime);

		if (!isRayne(thisPointer)) return;

		uintptr_t rayne = reinterpret_cast<uintptr_t>(thisPointer);
		if (*reinterpret_cast<int*>(rayne + TrueDhampirOffsets::AnimationIndex) == TrueDhampirOffsets::Jump) {
			*reinterpret_cast<float*>(rayne + TrueDhampirOffsets::RootMotionY) *= jumpHeightMultiplier;
		}
	}

	bool writeCode(uintptr_t address, const BYTE* bytes, size_t size) {
		DWORD oldProtect;
		if (!VirtualProtect((LPVOID)address, size, PAGE_EXECUTE_READWRITE, &oldProtect)) {
			DEBUG_LOG("[TrueDhampir] VirtualProtect failed at 0x" << std::hex << address << ", error: " << GetLastError());
			return false;
		}
		memcpy((void*)address, bytes, size);
		VirtualProtect((LPVOID)address, size, oldProtect, &oldProtect);
		FlushInstructionCache(GetCurrentProcess(), (LPCVOID)address, size);
		return true;
	}

	bool createHook(uintptr_t target, LPVOID detour, LPVOID* original, const char* name) {
		MH_STATUS status = MH_CreateHook((LPVOID)target, detour, original);
		if (status != MH_OK) {
			DEBUG_LOG("[TrueDhampir] " << name << " CreateHook failed: " << status);
			return false;
		}

		status = MH_EnableHook((LPVOID)target);
		if (status != MH_OK) {
			DEBUG_LOG("[TrueDhampir] " << name << " EnableHook failed: " << status);
			return false;
		}

		DEBUG_LOG("[TrueDhampir] " << name << " hook installed at 0x" << std::hex << target);
		return true;
	}

public:
	void setMultipliers(float runSpeed, float jumpHeight, float jumpRate, float gravity, float attackSpeed) {
		runSpeedMultiplier = runSpeed;
		jumpHeightMultiplier = jumpHeight;
		jumpAnimationRate = jumpRate;
		gravityMultiplier = gravity;
		attackSpeedMultiplier = attackSpeed;
	}

	bool applyGravityPatch() {
		if (this->gravityPatched) return true;

		if (memcmp((void*)this->gravityPatch.address, this->gravityPatch.original, this->gravityPatch.size) != 0) {
			DEBUG_LOG("[TrueDhampir] unexpected bytes at 0x" << std::hex << this->gravityPatch.address << " (" << this->gravityPatch.name << "), gravity patch skipped");
			return false;
		}

		uintptr_t multiplierAddress = reinterpret_cast<uintptr_t>(&gravityMultiplier);
		memcpy(this->gravityPatch.patched, &multiplierAddress, sizeof(multiplierAddress));

		if (!this->writeCode(this->gravityPatch.address, this->gravityPatch.patched, this->gravityPatch.size)) {
			return false;
		}

		this->gravityPatched = true;
		DEBUG_LOG("[TrueDhampir] airborne gravity multiplier set to " << gravityMultiplier);
		return true;
	}

	void restoreGravityPatch() {
		if (!this->gravityPatched) return;

		this->writeCode(this->gravityPatch.address, this->gravityPatch.original, this->gravityPatch.size);

		this->gravityPatched = false;
		DEBUG_LOG("[TrueDhampir] airborne gravity patch restored");
	}

	// all or nothing: if any original bytes don't match, nothing is written
	bool applyPatches() {
		if (this->patchesApplied) return true;

		for (auto& patch : this->patches) {
			if (memcmp((void*)patch.address, patch.original, patch.size) != 0) {
				DEBUG_LOG("[TrueDhampir] unexpected bytes at 0x" << std::hex << patch.address << " (" << patch.name << "), knockdown patches skipped");
				return false;
			}
		}

		for (auto& patch : this->patches) {
			if (!this->writeCode(patch.address, patch.patched, patch.size)) {
				return false;
			}
		}

		this->patchesApplied = true;
		DEBUG_LOG("[TrueDhampir] knockdown patches applied");
		return true;
	}

	void restorePatches() {
		if (!this->patchesApplied) return;

		for (auto& patch : this->patches) {
			this->writeCode(patch.address, patch.original, patch.size);
		}

		this->patchesApplied = false;
		DEBUG_LOG("[TrueDhampir] knockdown patches restored");
	}

	bool install() {
		if (this->hooksInstalled) {
			DEBUG_LOG("TrueDhampir hooks already installed");
			return true;
		}

		MH_STATUS status = MH_Initialize();
		if (status != MH_OK && status != MH_ERROR_ALREADY_INITIALIZED) {
			DEBUG_LOG("TrueDhampir hook failed: " << status);
			return false;
		}

		if (!this->createHook(this->setAnimationRateAddress, (LPVOID)&hookedSetAnimationRate, (LPVOID*)&originalSetAnimationRate, "animation rate")) {
			return false;
		}

		if (!this->createHook(this->advanceAnimationAddress, (LPVOID)&hookedAdvanceAnimation, (LPVOID*)&originalAdvanceAnimation, "animation advance")) {
			return false;
		}

		this->hooksInstalled = true;
		return true;
	}

	void uninstall() {
		this->restorePatches();
		this->restoreGravityPatch();

		if (!this->hooksInstalled) return;

		MH_DisableHook((LPVOID)this->setAnimationRateAddress);
		MH_RemoveHook((LPVOID)this->setAnimationRateAddress);
		MH_DisableHook((LPVOID)this->advanceAnimationAddress);
		MH_RemoveHook((LPVOID)this->advanceAnimationAddress);
		MH_Uninitialize();

		this->hooksInstalled = false;
		originalSetAnimationRate = nullptr;
		originalAdvanceAnimation = nullptr;
		DEBUG_LOG("TrueDhampir hooks uninstalled");
	}
};

class TrueDhampir {
private:
	TrueDhampirHook hook;
	bool knockdownImmunity;

public:
	TrueDhampir() : knockdownImmunity(true) {};

	~TrueDhampir() {
		this->hook.uninstall();
	}

	// Call after g_Config is loaded and before installing hooks
	void loadFromConfig() {
		this->knockdownImmunity = g_Config.trueDhampirKnockdownImmunity;
		this->hook.setMultipliers(
			g_Config.trueDhampirRunSpeedMultiplier,
			g_Config.trueDhampirJumpHeightMultiplier,
			g_Config.trueDhampirJumpAnimationRate,
			g_Config.trueDhampirGravityMultiplier,
			g_Config.trueDhampirAttackSpeedMultiplier);
	}

	// everything we patch or hook is code, so this is safe to call at startup, before Rayne exists
	bool installHooks() {
		if (this->knockdownImmunity && !this->hook.applyPatches()) {
			DEBUG_LOG("TrueDhampir: knockdown immunity unavailable");
		}
		if (!this->hook.applyGravityPatch()) {
			DEBUG_LOG("TrueDhampir: gravity multiplier unavailable");
		}
		return this->hook.install();
	}
};
