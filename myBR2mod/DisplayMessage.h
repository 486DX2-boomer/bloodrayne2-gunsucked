#pragma once
#include "Config.h"

// Display any text as subtitles or a message box
// used in game for subtitles, tutorial messages
// This appears to be safe to call and will not interfere with menus, main menu, or cutscenes

typedef void(__thiscall* fn_DisplayText)(void* thisPtr, const char* text, float duration, float vertPos, int align, int style1, int style2);

struct DisplayMessage {
    fn_DisplayText displayTextFn = (fn_DisplayText)0x004f4560;
    void* uiManager = (void*)Rayne2::UIManager;

    // Displays a message box
    // duration: seconds to display (-1.0f might auto-calculate from string length)
    // vertPos: vertical position (1.0f should be default)
    // align: -1, 0, or 1 (??? not sure)
    // style1: 1 for tutorial-style messages (I think)
    // style2: 0 = timed display, 1 = boxed display (unsure the difference)
    void display(const char* text, float duration, float vertPos, int align, int style1, int style2) {
        displayTextFn(uiManager, text, duration, vertPos, align, style1, style2);
    }

    void boxedMessage(const char* text, float duration = 5.0f) {
        display(text, duration, 1.0f, 0, 1, 1);
    }
    
    void timedMessage(const char* text, float duration = -1.0f) {
        display(text, duration, 1.0f, 0, 1, 0);
    }

    // not tested to see if this works or not or is needed
    //void clear() {
    //    this->display(nullptr, -1.0f, 1.0f, 0, 0, 0);
    //}
};