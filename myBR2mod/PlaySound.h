#pragma once
#include "Config.h"

// (best guesses)
// FUN_0061ba10 - pushSfxOptions: saves sound options state
// FUN_0061ba60 - popSfxOptions: restores sound options state
// FUN_0061b8e0 - setNextSfxChannel: sets audio channel (0-31)
// FUN_00621f80 - playUISound: wrapper that handles 2D/non-spatialized sound setup

typedef void(__cdecl* fn_PushSfxOptions)();
typedef void(__cdecl* fn_PopSfxOptions)();
typedef void(__cdecl* fn_SetNextSfxChannel)(int channel);
typedef int(__cdecl* fn_PlayUISound)(const char* soundName);

struct PlaySound {
    fn_PushSfxOptions pushSfxOptions = (fn_PushSfxOptions)0x0061ba10;
    fn_PopSfxOptions popSfxOptions = (fn_PopSfxOptions)0x0061ba60;
    fn_SetNextSfxChannel setNextSfxChannel = (fn_SetNextSfxChannel)0x0061b8e0;
    fn_PlayUISound playUISound = (fn_PlayUISound)0x00621f80;

    // Channel 3 for UI sounds
    // we don't know what the other channels are
    // but we know that actors and emitters have spatialized sounds, which we don't need.
    static constexpr int CHANNEL_UI = 3;

    // soundName: filename such as "ui-slider.wav"
    // wav filenames should all be in W32SOUND.POD
    void play(const char* soundName) {
        this->pushSfxOptions();
        this->setNextSfxChannel(CHANNEL_UI);
        this->playUISound(soundName);
        this->popSfxOptions();
    }

    void confirm() {
        this->play("ui-confirm.wav");
    }

    void cancel() {
        this->play("ui-cancel.wav");
    }

    void slider() {
        this->play("ui-slider.wav");
    }
};