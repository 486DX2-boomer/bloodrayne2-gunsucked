#pragma once

#include "Config.h"

// Super slow mode interacts unintuitively with Photo Mode. Because they both overwrite and also restore time factor, it's hard to mentally track
// the values that each will store and restore.
// Some additional thought will be needed to determine how these features should modify time factor and if they should be allowed to coexist

class SuperSlowMode {
private:
	float* timeFactor;

	// we don't really want to restore original time factor - it interacts in unpredictable ways with photo mode
	//float originalTimeFactor;

	bool enabled;

public:
	SuperSlowMode()
		: enabled(false)
	{
		this->timeFactor = (float*)Rayne2::TimeFactor;
	}

	bool isEnabled() const {
		return this->enabled;
	}

	void enable() {
		if (this->enabled) return;

		//this->originalTimeFactor = *this->timeFactor;

		*this->timeFactor = SUPER_SLOW_MODE_TIME_FACTOR;

		this->enabled = true;

		DEBUG_LOG("Super slow mode enabled");
	}

	void disable() {
		if (!this->enabled) return;

		//*this->timeFactor = this->originalTimeFactor;
		*this->timeFactor = 1.0f;

		this->enabled = false;

		DEBUG_LOG("Super slow mode disabled");
	}

	void toggle() {
		if (this->enabled) {
			this->disable();
		}
		else {
			this->enable();
		}
	}
};