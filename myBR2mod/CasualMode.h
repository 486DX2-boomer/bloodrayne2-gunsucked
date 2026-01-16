#pragma once
#include "Config.h"

// without some way to check game state, health regenerates in cutscenes and when the game is paused
// Also, time factor is a value that is independent of Dilated Perception/Slow Mo/Time Freeze,
// so regeneration rate isn't affected by slow mo powers
// Without these problems being fixed this isn't ready to be added to the mod.

class CasualMode {

	float sinceLastDamage;
	float healthRegenCooldown = 1000.0f; // load from config
	float healthRegenRate = 2.0f; // load from config
	bool healthRegenerating;

	float sinceLastRageExpended;
	float rageRegenCooldown;
	float rageRegenRate;
	bool rageRegenerating;

	// check previous vs current values to determine regeneration state
	float lastHealth;
	float lastRage;

	float currentHealth;
	float currentRage;

	// no operations on these values are safe unless player object is instantiated
	// this must be checked every time a value is modified as backing out to the main menu may destroy the object
	// since the same logic is used in other modules it wouldn't hurt to put these in a header such as Rayne.h
	bool checkRayneValid() {
		uintptr_t rayneBase = *reinterpret_cast<uintptr_t*>(Rayne2::RayneBasePtr);
		return (rayneBase != 0) ? true : false;
	}

	uintptr_t getRayneObject() {
		uintptr_t rayneBase = *reinterpret_cast<uintptr_t*>(Rayne2::RayneBasePtr);
		return rayneBase;
	}

public:
	CasualMode() :
		sinceLastDamage(0.0f),
		sinceLastRageExpended(0.0f),
		healthRegenerating(true),
		rageRegenerating(false),
		lastHealth(0.0f),
		lastRage(0.0f) {}
	~CasualMode() {}

	// this should be called from dllmain's loop.
	void update() {

		if (!this->checkRayneValid()) {
			return;
		}

		uintptr_t rayneBase = getRayneObject();

		float* currentHealth = reinterpret_cast<float*>(rayneBase + Rayne2::RayneHealthOffset);
		float* currentRage = reinterpret_cast<float*>(rayneBase + Rayne2::RayneRageOffset);

		// maximum health and rage can change in-game, so we have to check them each update too.
		float* maxHealth = reinterpret_cast<float*>(rayneBase + Rayne2::RayneMaxHealthOffset);
		float* maxRage = reinterpret_cast<float*>(rayneBase + Rayne2::RayneMaxRageOffset);

		float timeFactor = *Rayne2::TimeFactor;

		// In combat
		if (*currentHealth < this->lastHealth) {
			this->healthRegenerating = false;
			this->sinceLastDamage = 0.0f;
		}

		if (!this->healthRegenerating) {
			// multiply by time factor to keep calculations consistent when slow mo enabled
			this->sinceLastDamage += 1.0f * timeFactor;
		}

		if ((this->sinceLastDamage >= this->healthRegenCooldown) && (*currentHealth < *maxHealth)) {
			this->healthRegenerating = true;
			DEBUG_LOG("[Casual Mode] Regenerating health...");
		}

		if (this->healthRegenerating) {
			*currentHealth += (this->healthRegenRate * timeFactor);
			// disable when max health exceeded
			if (*currentHealth >= *maxHealth) {
				*currentHealth = *maxHealth; // clamp
				this->healthRegenerating = false;
				DEBUG_LOG("[Casual Mode] Health regeneration disabled");
			}
		}

		// store for next update
		this->lastHealth = *currentHealth;
		this->lastRage = *currentRage;
	}
};