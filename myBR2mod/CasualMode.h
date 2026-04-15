#pragma once
#include "Config.h"
#include "PlaySound.h"

// still to do:
// optional sound when regen begins and ends
// update reverse engineering notes with pause/time factor findings

class CasualMode {
	float sinceLastDamage;
	float healthRegenCooldown;
	float healthRegenRate;
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

	bool playSoundOnRegen;
	PlaySound sound; // we could make this optional, but it's lightweight so shouldn't hurt to always allocate it

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
		lastRage(0.0f),
		healthRegenCooldown(g_Config.casualModeHealthRegenCooldown),
		healthRegenRate(g_Config.casualModeHealthRegenRate),
		playSoundOnRegen(g_Config.casualModePlaySoundOnRegen) {}
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

		uintptr_t gameTimeBase = *reinterpret_cast<uintptr_t*>(Rayne2::GameTimeBase);
		float timeFactor = *reinterpret_cast<float*>(gameTimeBase + Rayne2::WorldTimeFactorOffset);
		int inCutscene = *reinterpret_cast<int*>(gameTimeBase + Rayne2::CutsceneActiveOffset);
		int controlDisabled = *reinterpret_cast<int*>(gameTimeBase + Rayne2::AllowHeroControlsOffset);
		// game unpaused = 0, game paused = 256
		// so we dereference then determine if it is nonzero.
		int pauseState = *reinterpret_cast<int*>(Rayne2::PauseState);
		bool isPaused = (pauseState != 0);

		//DEBUG_LOG("CM: cutscene status: " << inCutscene);
		//DEBUG_LOG("CM: hero control status: " << controlDisabled);

		// do not proceed if we're in a cutscene or non-interactive state (like pause menu)
		if (inCutscene || controlDisabled || isPaused) {
			return;
		}

		// In combat
		if (*currentHealth < this->lastHealth) { // in other words, Rayne took damage last tick.
			this->healthRegenerating = false;
			this->sinceLastDamage = 0.0f;
		}

		if (!this->healthRegenerating) {
			// multiply by time factor to keep calculations consistent when slow mo enabled
			this->sinceLastDamage += 1.0f * timeFactor;
		}

		if ((this->sinceLastDamage >= this->healthRegenCooldown) && (*currentHealth < *maxHealth)) {
			
			// play a sound when regen begins
			if (!this->healthRegenerating) {
				if (this->playSoundOnRegen) {
					this->sound.confirm();
				}
			}

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

				// play a sound when regen ends
				if (this->playSoundOnRegen) {
					this->sound.cancel();

				}
			}
		}

		// store for next update
		this->lastHealth = *currentHealth;
		this->lastRage = *currentRage;
	}
};