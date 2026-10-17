#pragma once
#include <Windows.h>
#include <cmath>
#include "Config.h"
#include "GamepadSupport.h"
#include "GunKeys.h"
#include "Overlay.h"

// Gamepad weapon wheel.
// We hook the right trigger input and respond to:
// - fast tap: cycles to the next weapon like the original
// - hold: game slows down while the weapon wheel opens. right stick to select weapon and release to equip

class WeaponWheel {
private:
	enum class State {
		Idle,           // trigger released
		TriggerDown,    // trigger pressed, but not held long enough to be a hold yet
		Open,           // wheel on screen
		WaitForRelease  // the wheel was cancelled (cutscene, pause...) while the trigger was held; ignore it until released
	};

	GamepadSupport* gamepad;
	Overlay* overlay;

	// our own instance rather than dllmain's: weapon hotkeys can be disabled in the INI, and the wheel still needs these
	GunKeys gunKeys;

	State state;
	DWORD triggerDownTime;
	int selectedWeapon; // weapon mode index, -1 = none
	uint32_t unlockedWeapons; // bitmask, captured when the wheel opens

	// the time factor we set when opening, or 0 if we didn't touch it.
	// we only restore the time factor if it still holds our value, so photo mode / super slow mode keep theirs
	float appliedTimeFactor;

	// presses shorter than this are taps (next weapon) longer open the wheel. set in gunsucked.ini.
	// note GamepadSupport reports releases up to ~64ms late, so a tap has to be released a bit sooner than this
	DWORD holdThresholdMilliseconds;

	// right trigger is read as an analog value, so it counts as pressed after a light pull instead of near the bottom.
	// it's released a little below the press point, so a trigger resting near the threshold can't flicker the wheel
	float triggerPressThreshold;
	float triggerReleaseThreshold;
	static constexpr float triggerHysteresis = 0.1f;
	bool triggerPressed; // last result of updateTriggerHeld(), so the release threshold applies while pressed

	// how far the stick has to be pushed (0-1) before it selects anything
	static constexpr float stickDeadzone = 0.5f;

	static constexpr float twoPi = 6.283185307f;

	// ignore input if rayne2.exe isn't focused.
	bool isGameFocused() {
		HWND foreground = GetForegroundWindow();
		if (foreground == nullptr) return false;

		DWORD foregroundPid;
		GetWindowThreadProcessId(foreground, &foregroundPid);

		return foregroundPid == GetCurrentProcessId();
	}

	// why canOpen() last returned false. for the debug log
	const char* blockedReason = "";

	// the hero pointer is slot 0 of the game's hero list. for the debug log; no C++ objects here because of __try
	static void readHeroSlot(uintptr_t& hero, uintptr_t& vftable) {
		hero = 0;
		vftable = 0;
		__try {
			hero = *reinterpret_cast<uintptr_t*>(Rayne2::RayneBasePtr);
			if (hero != 0) vftable = *reinterpret_cast<uintptr_t*>(hero);
		}
		__except (EXCEPTION_EXECUTE_HANDLER) {}
	}

	// whether the Rayne check failed on the last update while the wheel was open, so we log the change only once
	bool rayneInvalidWhileOpen = false;
	DWORD rayneInvalidSince = 0;

	// the wheel is only allowed during normal gameplay.
	// requireRayne: scripted events (like the car explosion in the Meatpacking District) briefly fail the Rayne check,
	// so an open wheel ignores it. Equipping checks it again anyway
	bool canOpen(bool requireRayne = true) {
		if (!this->isGameFocused()) { this->blockedReason = "window not focused"; return false; }

		this->gunKeys.checkSafe();
		if (requireRayne && !this->gunKeys.isSafe()) { this->blockedReason = "Rayne not valid"; return false; }

		uintptr_t gameTimeBase = *reinterpret_cast<uintptr_t*>(Rayne2::GameTimeBase);
		if (gameTimeBase == 0) { this->blockedReason = "no game state object"; return false; }

		if (*reinterpret_cast<int*>(gameTimeBase + Rayne2::CutsceneActiveOffset)) { this->blockedReason = "cutscene"; return false; }
		if (*reinterpret_cast<int*>(gameTimeBase + Rayne2::AllowHeroControlsOffset)) { this->blockedReason = "hero controls disabled"; return false; }
		if (*reinterpret_cast<int*>(Rayne2::PauseState) != 0) { this->blockedReason = "paused (input mode)"; return false; }

		// photo mode freezes time with a time factor of 0, so this also keeps the wheel out of photo mode
		if (*Rayne2::TimeFactor == 0.0f) { this->blockedReason = "time frozen (photo mode)"; return false; }

		return true;
	}

	uint32_t readUnlockedWeapons() {
		uint32_t unlocked = 0;
		for (unsigned int weapon = 0; weapon < 6; weapon++) {
			if (this->gunKeys.isWeaponUnlocked(weapon)) {
				unlocked |= 1u << weapon;
			}
		}
		return unlocked;
	}

	void open() {
		this->selectedWeapon = -1;
		this->unlockedWeapons = this->readUnlockedWeapons();

		this->overlay->setUnlockedWeapons(this->unlockedWeapons);
		this->overlay->setSelectedWeapon(-1);
		this->overlay->setEquippedWeapon(*Rayne2::currentEquippedWeaponIndex);
		this->overlay->showWheel();
		this->gamepad->setRightStickBlocked(true);

		// leave the time factor alone if something else (super slow mode) already changed it
		if (*Rayne2::TimeFactor == 1.0f) {
			// never 0: canOpen() reads a time factor of 0 as photo mode and would close the wheel straight away
			this->appliedTimeFactor = (g_Config.weaponWheelTimeFactor > 0.01f) ? g_Config.weaponWheelTimeFactor : 0.01f;
			*Rayne2::TimeFactor = this->appliedTimeFactor;
		}

		this->state = State::Open;
		DEBUG_LOG("Weapon wheel opened");
	}

	void close(bool equipSelection) {
		this->overlay->hideWheel();
		this->gamepad->setRightStickBlocked(false);

		if (this->appliedTimeFactor != 0.0f && *Rayne2::TimeFactor == this->appliedTimeFactor) {
			*Rayne2::TimeFactor = 1.0f;
		}
		this->appliedTimeFactor = 0.0f;

		if (equipSelection && this->selectedWeapon >= 0 && this->selectedWeapon != *Rayne2::currentEquippedWeaponIndex) {
			this->gunKeys.switchWeapon(this->selectedWeapon);
		}

		DEBUG_LOG("Weapon wheel closed, selected " << std::dec << this->selectedWeapon);
	}

	// the game only reports RT as a button past its own threshold, and in practice that needs a much deeper pull,
	// so we read the analog trigger with our own threshold instead.
	// The game's button still counts as a fallback... with DirectInput pads, the analog trigger may never be filled in.
	// actually... does Terminal Cut even work with DI pads?
	bool updateTriggerHeld() {
		float trigger = this->gamepad->getAnalogState().rightTrigger;
		float threshold = this->triggerPressed ? this->triggerReleaseThreshold : this->triggerPressThreshold;
		this->triggerPressed = (trigger >= threshold) || this->gamepad->getButtonPressed(GamepadSupport::Button::RT);
		return this->triggerPressed;
	}

	// selects the unlocked weapon whose ring is closest to the direction the stick points
	void updateSelection() {
		AnalogState analog = this->gamepad->getAnalogState();
		float x = analog.rightX;
		float y = analog.rightY;

		if (x * x + y * y < stickDeadzone * stickDeadzone) return;

		// 0 = up, increasing clockwise, matching the wheel's slots. the game's stick Y is negative when pushed up
		float stickAngle = atan2f(x, -y);

		int closestWeapon = -1;
		float closestDistance = twoPi;
		for (int slot = 0; slot < WheelLayout::slotCount; slot++) {
			int weapon = WheelLayout::slotWeapons[slot];
			if (weapon < 0 || !(this->unlockedWeapons & (1u << weapon))) continue;

			// angular distance, wrapped into -pi..pi
			float distance = fabsf(remainderf(stickAngle - slot * WheelLayout::slotAngle, twoPi));
			if (distance < closestDistance) {
				closestDistance = distance;
				closestWeapon = weapon;
			}
		}

		if (closestWeapon >= 0 && closestWeapon != this->selectedWeapon) {
			this->selectedWeapon = closestWeapon;
			this->overlay->setSelectedWeapon(closestWeapon);
		}
	}

public:
	WeaponWheel(GamepadSupport* gamepad, Overlay* overlay)
		: gamepad(gamepad), overlay(overlay), state(State::Idle), triggerDownTime(0),
		selectedWeapon(-1), unlockedWeapons(0), appliedTimeFactor(0.0f),
		holdThresholdMilliseconds(static_cast<DWORD>(g_Config.weaponWheelHoldThresholdMilliseconds > 0 ? g_Config.weaponWheelHoldThresholdMilliseconds : 0)),
		triggerPressThreshold(g_Config.weaponWheelTriggerThreshold), triggerReleaseThreshold(0.0f), triggerPressed(false)
	{
		// keep the release point above 0 so drift alone can't hold the trigger down
		this->triggerReleaseThreshold = this->triggerPressThreshold - triggerHysteresis;
		if (this->triggerReleaseThreshold < 0.05f) {
			this->triggerReleaseThreshold = 0.05f;
		}
		if (this->triggerPressThreshold < this->triggerReleaseThreshold) {
			this->triggerPressThreshold = this->triggerReleaseThreshold;
		}

		// the game never sees right trigger again; tapping it is handled by us
		this->gamepad->registerBlockedButton(GamepadSupport::Button::RT);
	}

	~WeaponWheel() {
		if (this->state == State::Open) {
			this->close(false);
		}
	}

	// call from dllmain's loop, after gamepad->update()
	void update() {
		bool triggerHeld = this->updateTriggerHeld();

		switch (this->state) {
		case State::Idle:
			if (triggerHeld) {
				this->triggerDownTime = GetTickCount();
				this->state = State::TriggerDown;
			}
			break;

		case State::TriggerDown:
			if (!triggerHeld) {
				// a tap: do what the button originally did
				if (this->canOpen()) {
					this->gunKeys.switchToNextWeapon();
				}
				this->state = State::Idle;
			}
			else if (GetTickCount() - this->triggerDownTime >= holdThresholdMilliseconds) {
				if (this->canOpen()) {
					this->open();
				}
				else {
					this->state = State::WaitForRelease;
				}
			}
			break;

		case State::Open:
			if (!this->canOpen(false)) {
				// cutscene started, game paused, window lost focus... back out without changing weapons
				DEBUG_LOG("Weapon wheel cancelled: " << this->blockedReason
					<< " (trigger " << this->gamepad->getAnalogState().rightTrigger << ")");
				this->rayneInvalidWhileOpen = false;
				this->close(false);
				this->state = triggerHeld ? State::WaitForRelease : State::Idle;
				break;
			}

			if (this->gunKeys.isSafe() == this->rayneInvalidWhileOpen) {
				this->rayneInvalidWhileOpen = !this->gunKeys.isSafe();
				uintptr_t hero, vftable;
				readHeroSlot(hero, vftable);
				if (this->rayneInvalidWhileOpen) {
					this->rayneInvalidSince = GetTickCount();
					DEBUG_LOG("Weapon wheel: Rayne check failed, hero slot 0x" << std::hex << hero << " vftable 0x" << vftable << std::dec);
				}
				else {
					DEBUG_LOG("Weapon wheel: Rayne check passes again after " << (GetTickCount() - this->rayneInvalidSince)
						<< " ms, hero slot 0x" << std::hex << hero << std::dec);
				}
			}

			if (!triggerHeld) {
				DEBUG_LOG("Weapon wheel trigger released (trigger " << this->gamepad->getAnalogState().rightTrigger
					<< ", button " << this->gamepad->getButtonPressed(GamepadSupport::Button::RT) << ")");
				this->rayneInvalidWhileOpen = false;
				this->close(true);
				this->state = State::Idle;
			}
			else {
				this->updateSelection();
			}
			break;

		case State::WaitForRelease:
			if (!triggerHeld) {
				this->state = State::Idle;
			}
			break;
		}
	}
};
