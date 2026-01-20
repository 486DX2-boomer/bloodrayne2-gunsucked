#pragma once
#include <Windows.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <filesystem>
#include "Config.h"
#include "MinHook.h"

// Outfit System for BloodRayne 2: Terminal Cut
// Enables loading custom outfits by hooking the game's outfit accessor
// functions and redirecting file loads via CreateFileA hook.

// Game Function Addresses
namespace OutfitStuff {
    // Accessor function addresses
    constexpr uintptr_t FN_GET_DFM_PATH = 0x004f69b0;
    constexpr uintptr_t FN_GET_GUN_PATH = 0x004f69e0;
    constexpr uintptr_t FN_GET_HARPOON_TEX = 0x004f6a30;
    constexpr uintptr_t FN_GET_BLADE_PATH = 0x004f6a70;
    constexpr uintptr_t FN_GET_BLADE_PATH_BY_ID = 0x004f6a80;
    constexpr uintptr_t FN_GET_INTERNAL_NAME = 0x004f6ad0;
    constexpr uintptr_t FN_GET_DISPLAY_NAME = 0x004f6b10;
    constexpr uintptr_t FN_GET_OUTFIT_INDEX = 0x004f6930;
    constexpr uintptr_t FN_GET_JUG_PATH = 0x004f69c0;

    // Player outfit ID location
    constexpr uintptr_t PTR_PLAYER_STATE = 0x007c07f8;
    constexpr uintptr_t OFFSET_OUTFIT_ID = 0x3cc;

    // Outfit index storage - this is what actually determines which outfit loads
    constexpr uintptr_t OUTFIT_INDEX_STORAGE = 0x05e339B4;

    // Asset handlers
    constexpr uintptr_t HANDLER_ARRAY = 0x05dcc0a0;
    constexpr uintptr_t HANDLER_COUNT = 0x05dcc0b4;
    constexpr uintptr_t POD_HANDLER_FUNCTION = 0x005A3FD0;
    constexpr uintptr_t LOOSE_FILE_HANDLER_FUNCTION = 0x004937B0;

    // Original outfit count
    constexpr int BASE_OUTFIT_COUNT = 10;

    // Menu system addresses
    constexpr uintptr_t FN_ADD_MENU_ITEM = 0x0055a760;
    constexpr uintptr_t FN_ALLOC_MENU_ITEM = 0x0055a6d0;
    constexpr uintptr_t MENU_STATE = 0x05ef4bf8;
    constexpr uintptr_t MENU_ITEM_COUNT = 0x05ef4c28;
    constexpr uintptr_t MENU_SELECTION_INDEX = 0x05EF4AE8;

    // Menu state for outfit selection
    constexpr int MENU_STATE_OUTFITS = 0x2e;

    // Action code formula: Action Code = Outfit ID + ACTION_CODE_BASE
    constexpr int ACTION_CODE_BASE = 0xFA;
    constexpr int ACTION_CODE_STANDARD_OUTFIT = 0xFB;

    // SetOutfitIndex function
    constexpr uintptr_t FN_SET_OUTFIT_INDEX = 0x004f6990;
}

// Function Pointer Types
typedef char* (__stdcall* GetPathByID_t)(int outfitID);
typedef char* (__cdecl* GetPathForCurrent_t)(void);
typedef unsigned int(__cdecl* GetOutfitIndex_t)(void);
typedef void(__stdcall* SetOutfitIndex_t)(int outfitID);

typedef HANDLE(WINAPI* CreateFileA_t)(
    LPCSTR lpFileName,
    DWORD dwDesiredAccess,
    DWORD dwShareMode,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    DWORD dwCreationDisposition,
    DWORD dwFlagsAndAttributes,
    HANDLE hTemplateFile
    );

typedef HANDLE(WINAPI* FindFirstFileA_t)(LPCSTR lpFileName, LPWIN32_FIND_DATAA lpFindFileData);
typedef void(__cdecl* AddMenuItem_t)(void);
typedef void* (__cdecl* AllocMenuItem_t)(void);

// Base Outfit Definitions
struct BaseOutfitDefinition {
    const char* internalName;
    const char* dfmPath;
    const char* gunPath;
    const char* harpoonTexture;
    const char* bladePath;
    const char* jugPath;
};

static const BaseOutfitDefinition baseOutfits[] = {
    { "rayne",             "rayne.dfm",             "WEAPONS_RAYNES_GUN.SMF",             "chainlink.tif", "weapons_rayne_blade_01.smf",      "rayne.jug" },
    { "rayne_dress",       "rayne_dress.dfm",       "WEAPONS_RAYNES_GUN.SMF",             "chainlink.tif", "weapons_rayne_blade_01.smf",      "rayne_dress.jug" },
    { "rayne_dress_blue",  "rayne_dress_blue.dfm",  "WEAPONS_RAYNES_GUN.SMF",             "chainlink.tif", "weapons_rayne_blade_01.smf",      "rayne_dress.jug" },
    { "rayne_dress_red",   "rayne_dress_red.dfm",   "WEAPONS_RAYNES_GUN.SMF",             "chainlink.tif", "weapons_rayne_blade_01.smf",      "rayne_dress.jug" },
    { "rayne_dress_dragon","rayne_dress_dragon.dfm","WEAPONS_RAYNES_GUN.SMF",             "chainlink.tif", "weapons_rayne_blade_01.smf",      "rayne_dress.jug" },
    { "rayne_dress_green", "rayne_dress_green.dfm", "WEAPONS_RAYNES_GUN.SMF",             "chainlink.tif", "weapons_rayne_blade_01.smf",      "rayne_dress.jug" },
    { "rayne_dark",        "rayne_dark.dfm",        "weapons_dark_raynes_gun.smf",        "chainlink.tif", "weapons_rayne_blade_dark.smf",    "rayne_dark.jug" },
    { "rayne_cowgirl",     "rayne_cowgirl.dfm",     "weapons_cowgirl_raynes_gun.smf",     "bullwhip.tif",  "weapons_rayne_blade_cowgirl.smf", "rayne_cowgirl.jug" },
    { "rayne_armor",       "rayne_armor.dfm",       "weapons_armor_raynes_gun.smf",       "chainlink.tif", "weapons_rayne_blade_armor.smf",   "rayne_armor.jug" },
    { "rayne_schoolgirl",  "rayne_schoolgirl.dfm",  "weapons_rayne_desert eagle_gun.smf", "chainlink.tif", "weapons_rayne_katana.smf",        "rayne_schoolgirl.jug" }
};

// OutfitEntry
struct OutfitEntry {
    char internalName[64];
    char displayName[64];
    char dfmPath[256];
    char gunPath[256];
    char harpoonTexture[256];
    char bladePath[256];
    char jugPath[256];
    int outfitID;
    int baseOutfitIndex;
    char modFolderPath[512];

    OutfitEntry() : outfitID(-1), baseOutfitIndex(0) {
        memset(this->internalName, 0, sizeof(this->internalName));
        memset(this->displayName, 0, sizeof(this->displayName));
        memset(this->dfmPath, 0, sizeof(this->dfmPath));
        memset(this->gunPath, 0, sizeof(this->gunPath));
        memset(this->harpoonTexture, 0, sizeof(this->harpoonTexture));
        memset(this->bladePath, 0, sizeof(this->bladePath));
        memset(this->jugPath, 0, sizeof(this->jugPath));
        memset(this->modFolderPath, 0, sizeof(this->modFolderPath));
    }

    void setInternalName(const char* name) {
        strncpy_s(this->internalName, sizeof(this->internalName), name, _TRUNCATE);
    }

    void setDisplayName(const char* name) {
        strncpy_s(this->displayName, sizeof(this->displayName), name, _TRUNCATE);
    }

    void setDfmPath(const char* path) {
        strncpy_s(this->dfmPath, sizeof(this->dfmPath), path, _TRUNCATE);
    }

    void setGunPath(const char* path) {
        strncpy_s(this->gunPath, sizeof(this->gunPath), path, _TRUNCATE);
    }

    void setHarpoonTexture(const char* path) {
        strncpy_s(this->harpoonTexture, sizeof(this->harpoonTexture), path, _TRUNCATE);
    }

    void setBladePath(const char* path) {
        strncpy_s(this->bladePath, sizeof(this->bladePath), path, _TRUNCATE);
    }

    void setJugPath(const char* path) {
        strncpy_s(this->jugPath, sizeof(this->jugPath), path, _TRUNCATE);
    }

    void setModFolderPath(const char* path) {
        strncpy_s(this->modFolderPath, sizeof(this->modFolderPath), path, _TRUNCATE);
    }

    void inheritFromBase() {
        if (this->baseOutfitIndex < 0 || this->baseOutfitIndex >= 10) {
            this->baseOutfitIndex = 0;
        }

        const BaseOutfitDefinition& base = baseOutfits[this->baseOutfitIndex];

        if (this->dfmPath[0] == '\0') {
            this->setDfmPath(base.dfmPath);
        }
        if (this->gunPath[0] == '\0') {
            this->setGunPath(base.gunPath);
        }
        if (this->harpoonTexture[0] == '\0') {
            this->setHarpoonTexture(base.harpoonTexture);
        }
        if (this->bladePath[0] == '\0') {
            this->setBladePath(base.bladePath);
        }
        if (this->jugPath[0] == '\0') {
            this->setJugPath(base.jugPath);
        }
    }
};

// OutfitMenuHook - Injects custom outfits into the outfit selection menu
// Forward declarations
class Outfit;
class OutfitMenuHook;
static OutfitMenuHook* g_menuHookInstance = nullptr;

// Trampoline pointers for naked hooks
static void* g_originalAddMenuItemTrampoline = nullptr;
static void* g_originalSetOutfitIndexTrampoline = nullptr;
static DWORD g_setOutfitIndex_ECX = 0;
static DWORD g_setOutfitIndex_outfitID = 0;

// Forward declarations of hook implementations
static void __cdecl OutfitMenuHook_Impl();
static void __cdecl SetOutfitIndexHook_Impl();

// Naked hook for AddMenuItem - preserves registers across the hook
static void __declspec(naked) OutfitMenuHook_NakedHook() {
    __asm {
        pushad
        pushfd

        popfd
        popad

        call g_originalAddMenuItemTrampoline

        pushad
        pushfd
        call OutfitMenuHook_Impl
        popfd
        popad

        ret
    }
}

// Naked hook for SetOutfitIndex - intercepts outfit selection
static void __declspec(naked) SetOutfitIndexHook_Naked() {
    __asm {
        mov g_setOutfitIndex_ECX, ecx
        mov eax, [esp + 4]
        mov g_setOutfitIndex_outfitID, eax

        pushad
        pushfd
        call SetOutfitIndexHook_Impl
        popfd
        popad

        mov eax, g_setOutfitIndex_outfitID
        mov[esp + 4], eax

        mov ecx, g_setOutfitIndex_ECX
        jmp g_originalSetOutfitIndexTrampoline
    }
}

class OutfitMenuHook {
private:
    bool initialized = false;
    bool hookInstalled = false;
    int outfitMenuCallCount = 0;

    AddMenuItem_t originalAddMenuItem = nullptr;
    void* originalSetOutfitIndex = nullptr;

    Outfit* outfitSystem = nullptr;

    friend void __cdecl OutfitMenuHook_Impl();
    friend void __cdecl SetOutfitIndexHook_Impl();

    void injectCustomOutfits();

    void addOutfitMenuItem(int outfitID, const char* displayName) {
        int actionCode = OutfitStuff::ACTION_CODE_STANDARD_OUTFIT;

        DEBUG_LOG("[MenuHook] Adding menu item: ID=" << outfitID
            << ", ActionCode=0x" << std::hex << actionCode << std::dec
            << " (using Standard action code)"
            << ", Name=" << displayName);

        uintptr_t addMenuItemAddr = OutfitStuff::FN_ADD_MENU_ITEM;

        __asm {
            push eax
            push esi
            push ecx

            mov ecx, displayName
            xor esi, esi
            mov eax, actionCode

            call addMenuItemAddr

            pop ecx
            pop esi
            pop eax
        }
    }

public:
    OutfitMenuHook() = default;
    ~OutfitMenuHook() = default;

    bool initialize(Outfit* outfit) {
        if (this->initialized) return true;

        this->outfitSystem = outfit;
        g_menuHookInstance = this;
        this->initialized = true;

        DEBUG_LOG("[MenuHook] Initialized");
        return true;
    }

    bool installHook() {
        if (this->hookInstalled) return true;
        if (!this->initialized) {
            DEBUG_LOG("[MenuHook] Cannot install hook - not initialized");
            return false;
        }

        MH_STATUS status = MH_CreateHook(
            (LPVOID)OutfitStuff::FN_ADD_MENU_ITEM,
            (LPVOID)&OutfitMenuHook_NakedHook,
            (LPVOID*)&this->originalAddMenuItem
        );

        if (status != MH_OK) {
            DEBUG_LOG("[MenuHook] Failed to create AddMenuItem hook: " << status);
            return false;
        }

        g_originalAddMenuItemTrampoline = (void*)this->originalAddMenuItem;

        status = MH_EnableHook((LPVOID)OutfitStuff::FN_ADD_MENU_ITEM);
        if (status != MH_OK) {
            DEBUG_LOG("[MenuHook] Failed to enable AddMenuItem hook: " << status);
            MH_RemoveHook((LPVOID)OutfitStuff::FN_ADD_MENU_ITEM);
            return false;
        }

        this->hookInstalled = true;
        DEBUG_LOG("[MenuHook] AddMenuItem hook installed successfully");

        status = MH_CreateHook(
            (LPVOID)OutfitStuff::FN_SET_OUTFIT_INDEX,
            (LPVOID)&SetOutfitIndexHook_Naked,
            (LPVOID*)&this->originalSetOutfitIndex
        );

        if (status != MH_OK) {
            DEBUG_LOG("[MenuHook] Failed to create SetOutfitIndex hook: " << status);
        }
        else {
            g_originalSetOutfitIndexTrampoline = this->originalSetOutfitIndex;

            status = MH_EnableHook((LPVOID)OutfitStuff::FN_SET_OUTFIT_INDEX);
            if (status != MH_OK) {
                DEBUG_LOG("[MenuHook] Failed to enable SetOutfitIndex hook: " << status);
                MH_RemoveHook((LPVOID)OutfitStuff::FN_SET_OUTFIT_INDEX);
            }
            else {
                DEBUG_LOG("[MenuHook] SetOutfitIndex hook installed successfully");
            }
        }

        return true;
    }
};

// Outfit Class - Main Interface
class Outfit;
static Outfit* g_outfitInstance = nullptr;

class Outfit {
private:
    bool initialized = false;
    bool hooksInstalled = false;
    bool handlersSwapped = false;
    bool handlersReady = false;

    std::vector<OutfitEntry> customOutfits;
    int nextOutfitID = OutfitStuff::BASE_OUTFIT_COUNT + 1;

    OutfitMenuHook menuHook;

    char gameDirectory[MAX_PATH] = { 0 };
    size_t gameDirectoryLength = 0;

    // Original function pointers
    GetPathByID_t originalGetGunPath = nullptr;
    GetPathByID_t originalGetHarpoonTex = nullptr;
    GetPathByID_t originalGetBladePathByID = nullptr;
    GetPathByID_t originalGetInternalName = nullptr;
    GetPathByID_t originalGetDisplayName = nullptr;
    GetPathForCurrent_t originalGetJugPath = nullptr;
    GetPathForCurrent_t originalGetDfmPath = nullptr;
    GetPathForCurrent_t originalGetBladePath = nullptr;
    GetOutfitIndex_t originalGetOutfitIndex = nullptr;
    CreateFileA_t originalCreateFileA = nullptr;
    FindFirstFileA_t originalFindFirstFileA = nullptr;

    // Helper: Initialize game directory path from executable location
    bool initGamePath() {
        if (this->gameDirectoryLength > 0) return true;

        char exePath[MAX_PATH];
        DWORD len = GetModuleFileNameA(NULL, exePath, MAX_PATH);
        if (len == 0 || len >= MAX_PATH) {
            DEBUG_LOG("[Outfit] Failed to get module filename");
            return false;
        }

        char* lastSlash = strrchr(exePath, '\\');
        if (lastSlash == nullptr) {
            DEBUG_LOG("[Outfit] Invalid executable path format");
            return false;
        }

        size_t dirLen = (lastSlash - exePath) + 1;
        memcpy(this->gameDirectory, exePath, dirLen);
        this->gameDirectory[dirLen] = '\0';
        this->gameDirectoryLength = dirLen;

        DEBUG_LOG("[Outfit] Game directory: " << this->gameDirectory);
        return true;
    }

    // Helper: Check if path is a Rayne-related asset
    bool isRayneAsset(const char* relativePath) const {
        if (relativePath == nullptr) return false;

        char lowerPath[MAX_PATH];
        size_t len = strlen(relativePath);
        if (len >= MAX_PATH) return false;

        for (size_t i = 0; i <= len; i++) {
            lowerPath[i] = (char)tolower((unsigned char)relativePath[i]);
        }

        // DATA folder support for mods that include .FX effect files
        if (strstr(lowerPath, "\\data\\") != nullptr || strncmp(lowerPath, "data\\", 5) == 0) {
            return true;
        }

        const char* filename = strrchr(lowerPath, '\\');
        if (filename == nullptr) {
            filename = lowerPath;
        }
        else {
            filename++;
        }

        // Rayne character assets
        if (strncmp(filename, "rayne", 5) == 0) return true;
        if (strncmp(filename, "hrayne", 6) == 0) return true;

        // Rayne's weapon assets
        if ((strncmp(filename, "weapon_", 7) == 0 || strncmp(filename, "weapons_", 8) == 0) &&
            strstr(filename, "rayne") != nullptr) {
            return true;
        }

        // Armor outfit specific
        if (strncmp(filename, "mans_metal_armor", 16) == 0) return true;

        // Cowgirl outfit specific
        if (strncmp(filename, "cowgirl", 7) == 0) return true;

        // Chainlink/bullwhip (harpoon textures)
        if (strncmp(filename, "chainlink", 9) == 0) return true;
        if (strncmp(filename, "bullwhip", 8) == 0) return true;

        return false;
    }

    // Helper: Get current player's outfit ID
    int getCurrentOutfitID() {
        uintptr_t playerStatePtr = *(uintptr_t*)OutfitStuff::PTR_PLAYER_STATE;
        if (playerStatePtr == 0) return 1;
        return *(int*)(playerStatePtr + OutfitStuff::OFFSET_OUTFIT_ID);
    }

    // Helper: Find custom outfit by ID
    OutfitEntry* findCustomOutfit(int outfitID) {
        for (auto& entry : this->customOutfits) {
            if (entry.outfitID == outfitID) {
                return &entry;
            }
        }
        return nullptr;
    }

    // Helper: Get redirected mod path for file operations
    // Returns true and fills modPath if redirection should occur, false otherwise
    bool getRedirectedModPath(LPCSTR lpFileName, char* modPath, size_t modPathSize) {
        if (lpFileName == nullptr) return false;

        int outfitID = this->getCurrentOutfitID();
        if (outfitID <= OutfitStuff::BASE_OUTFIT_COUNT) return false;

        OutfitEntry* entry = this->findCustomOutfit(outfitID);
        if (!entry || entry->modFolderPath[0] == '\0') return false;

        // Determine relative path
        const char* relativePath = nullptr;

        if (this->gameDirectoryLength > 0 &&
            _strnicmp(lpFileName, this->gameDirectory, this->gameDirectoryLength) == 0) {
            relativePath = lpFileName + this->gameDirectoryLength;
        }
        else if (lpFileName[0] != '\\' && (strlen(lpFileName) < 2 || lpFileName[1] != ':')) {
            relativePath = lpFileName;
        }

        if (relativePath == nullptr) return false;
        if (!this->isRayneAsset(relativePath)) return false;

        snprintf(modPath, modPathSize, "%s\\%s", entry->modFolderPath, relativePath);
        return true;
    }

    // Hook Callbacks
    static char* __stdcall HookedGetGunPath(int outfitID) {
        if (g_outfitInstance && outfitID > OutfitStuff::BASE_OUTFIT_COUNT) {
            OutfitEntry* entry = g_outfitInstance->findCustomOutfit(outfitID);
            if (entry) {
                DEBUG_LOG("[Outfit] GetGunPath(" << outfitID << ") -> " << entry->gunPath);
                return entry->gunPath;
            }
        }
        return g_outfitInstance->originalGetGunPath(outfitID);
    }

    static char* __stdcall HookedGetHarpoonTex(int outfitID) {
        if (g_outfitInstance && outfitID > OutfitStuff::BASE_OUTFIT_COUNT) {
            OutfitEntry* entry = g_outfitInstance->findCustomOutfit(outfitID);
            if (entry) {
                DEBUG_LOG("[Outfit] GetHarpoonTex(" << outfitID << ") -> " << entry->harpoonTexture);
                return entry->harpoonTexture;
            }
        }
        return g_outfitInstance->originalGetHarpoonTex(outfitID);
    }

    static char* __stdcall HookedGetBladePathByID(int outfitID) {
        if (g_outfitInstance && outfitID > OutfitStuff::BASE_OUTFIT_COUNT) {
            OutfitEntry* entry = g_outfitInstance->findCustomOutfit(outfitID);
            if (entry) {
                DEBUG_LOG("[Outfit] GetBladePathByID(" << outfitID << ") -> " << entry->bladePath);
                return entry->bladePath;
            }
        }
        return g_outfitInstance->originalGetBladePathByID(outfitID);
    }

    static char* __stdcall HookedGetInternalName(int outfitID) {
        if (g_outfitInstance && outfitID > OutfitStuff::BASE_OUTFIT_COUNT) {
            OutfitEntry* entry = g_outfitInstance->findCustomOutfit(outfitID);
            if (entry) {
                DEBUG_LOG("[Outfit] GetInternalName(" << outfitID << ") -> " << entry->internalName);
                return entry->internalName;
            }
        }
        return g_outfitInstance->originalGetInternalName(outfitID);
    }

    static char* __stdcall HookedGetDisplayName(int outfitID) {
        if (g_outfitInstance && outfitID > OutfitStuff::BASE_OUTFIT_COUNT) {
            OutfitEntry* entry = g_outfitInstance->findCustomOutfit(outfitID);
            if (entry) {
                DEBUG_LOG("[Outfit] GetDisplayName(" << outfitID << ") -> " << entry->displayName);
                return entry->displayName;
            }
        }
        return g_outfitInstance->originalGetDisplayName(outfitID);
    }

    static char* __cdecl HookedGetJugPath() {
        if (g_outfitInstance) {
            int outfitID = g_outfitInstance->getCurrentOutfitID();
            if (outfitID > OutfitStuff::BASE_OUTFIT_COUNT) {
                OutfitEntry* entry = g_outfitInstance->findCustomOutfit(outfitID);
                if (entry) {
                    DEBUG_LOG("[Outfit] GetJugPath() [ID=" << outfitID << "] -> " << entry->jugPath);
                    return entry->jugPath;
                }
            }
        }
        return g_outfitInstance->originalGetJugPath();
    }

    static char* __cdecl HookedGetDfmPath() {
        if (g_outfitInstance) {
            int outfitID = g_outfitInstance->getCurrentOutfitID();
            if (outfitID > OutfitStuff::BASE_OUTFIT_COUNT) {
                OutfitEntry* entry = g_outfitInstance->findCustomOutfit(outfitID);
                if (entry) {
                    DEBUG_LOG("[Outfit] GetDfmPath() [ID=" << outfitID << "] -> " << entry->dfmPath);
                    return entry->dfmPath;
                }
            }
        }
        return g_outfitInstance->originalGetDfmPath();
    }

    static char* __cdecl HookedGetBladePath() {
        if (g_outfitInstance) {
            int outfitID = g_outfitInstance->getCurrentOutfitID();
            if (outfitID > OutfitStuff::BASE_OUTFIT_COUNT) {
                OutfitEntry* entry = g_outfitInstance->findCustomOutfit(outfitID);
                if (entry) {
                    DEBUG_LOG("[Outfit] GetBladePath() [ID=" << outfitID << "] -> " << entry->bladePath);
                    return entry->bladePath;
                }
            }
        }
        return g_outfitInstance->originalGetBladePath();
    }

    static unsigned int __cdecl HookedGetOutfitIndex() {
        if (g_outfitInstance) {
            int outfitID = g_outfitInstance->getCurrentOutfitID();
            if (outfitID > OutfitStuff::BASE_OUTFIT_COUNT) {
                OutfitEntry* entry = g_outfitInstance->findCustomOutfit(outfitID);
                if (entry) {
                    return (unsigned int)entry->baseOutfitIndex;
                }
            }
        }
        return g_outfitInstance->originalGetOutfitIndex();
    }

    // FindFirstFileA Hook - Redirect file existence checks to mod folders
    static HANDLE WINAPI HookedFindFirstFileA(LPCSTR lpFileName, LPWIN32_FIND_DATAA lpFindFileData) {
        if (!g_outfitInstance || !g_outfitInstance->originalFindFirstFileA) {
            return INVALID_HANDLE_VALUE;
        }

        char modPath[MAX_PATH];
        if (g_outfitInstance->getRedirectedModPath(lpFileName, modPath, sizeof(modPath))) {
            HANDLE hFind = g_outfitInstance->originalFindFirstFileA(modPath, lpFindFileData);
            if (hFind != INVALID_HANDLE_VALUE) {
                DEBUG_LOG("[FindFirstFileA] Redirecting: " << lpFileName << " -> " << modPath);
                return hFind;
            }
        }

        return g_outfitInstance->originalFindFirstFileA(lpFileName, lpFindFileData);
    }

    // CreateFileA Hook - Intercept file loads and redirect to mod folders
    static HANDLE WINAPI HookedCreateFileA(
        LPCSTR lpFileName,
        DWORD dwDesiredAccess,
        DWORD dwShareMode,
        LPSECURITY_ATTRIBUTES lpSecurityAttributes,
        DWORD dwCreationDisposition,
        DWORD dwFlagsAndAttributes,
        HANDLE hTemplateFile
    ) {
        if (!g_outfitInstance || !g_outfitInstance->originalCreateFileA) {
            return INVALID_HANDLE_VALUE;
        }

        char modPath[MAX_PATH];
        if (g_outfitInstance->getRedirectedModPath(lpFileName, modPath, sizeof(modPath))) {
            WIN32_FIND_DATAA findData;
            HANDLE hFind = g_outfitInstance->originalFindFirstFileA(modPath, &findData);
            if (hFind != INVALID_HANDLE_VALUE) {
                FindClose(hFind);
                DEBUG_LOG("[CreateFileA] Redirecting: " << lpFileName << " -> " << modPath);
                return g_outfitInstance->originalCreateFileA(
                    modPath, dwDesiredAccess, dwShareMode,
                    lpSecurityAttributes, dwCreationDisposition,
                    dwFlagsAndAttributes, hTemplateFile
                );
            }
        }

        return g_outfitInstance->originalCreateFileA(
            lpFileName, dwDesiredAccess, dwShareMode,
            lpSecurityAttributes, dwCreationDisposition,
            dwFlagsAndAttributes, hTemplateFile
        );
    }

    // Asset Handler Priority
    bool enableLooseFilePriority() {
        if (this->handlersSwapped) return true;
        if (!this->handlersReady) {
            DEBUG_LOG("[Outfit] Cannot swap handlers - not ready yet");
            return false;
        }

        uintptr_t* handlers = (uintptr_t*)OutfitStuff::HANDLER_ARRAY;

        uintptr_t originalFirst = handlers[0];
        uintptr_t originalSecond = handlers[1];

        DEBUG_LOG("[Outfit] Original handler order: [0]=0x" << std::hex << originalFirst
            << ", [1]=0x" << originalSecond);

        DWORD oldProtect;
        if (!VirtualProtect((LPVOID)OutfitStuff::HANDLER_ARRAY, 8, PAGE_READWRITE, &oldProtect)) {
            DEBUG_LOG("[Outfit] Failed to change handler array protection, error: " << GetLastError());
            return false;
        }

        handlers[0] = originalSecond;
        handlers[1] = originalFirst;

        VirtualProtect((LPVOID)OutfitStuff::HANDLER_ARRAY, 8, oldProtect, &oldProtect);

        DEBUG_LOG("[Outfit] New handler order: [0]=0x" << std::hex << handlers[0]
            << ", [1]=0x" << handlers[1]);

        this->handlersSwapped = true;
        DEBUG_LOG("[Outfit] Loose file priority enabled (handlers swapped)");
        return true;
    }

    // Folder Parsing Helpers
    int detectBaseOutfit(const std::string& folderPath) {
        auto hasFile = [&](const std::string& subPath, const std::string& filename) {
            return std::filesystem::exists(folderPath + "\\" + subPath + "\\" + filename);
            };

        // Check MODELS folder for character model files (.BFM)
        if (hasFile("MODELS", "RAYNE_SCHOOLGIRL.BFM")) return 9;
        if (hasFile("MODELS", "RAYNE_ARMOR.BFM")) return 8;
        if (hasFile("MODELS", "RAYNE_COWGIRL.BFM")) return 7;
        if (hasFile("MODELS", "RAYNE_DARK.BFM")) return 6;
        if (hasFile("MODELS", "RAYNE_DRESS.BFM")) return 1;

        // Check ART folder for texture files
        // Evening Gown / Dress (base 1)
        if (hasFile("ART", "RAYNE_DRESS.TEX")) return 1;
        if (hasFile("ART", "HRAYNEDRESS.TIF")) return 1;
        if (hasFile("ART", "RAYNE_DRESS_BUMPMAP.TEX")) return 1;
        if (hasFile("ART", "RAYNE_DRESS_GLOSSMAP.TEX")) return 1;

        // Schoolgirl (base 9)
        if (hasFile("ART", "RAYNE_SCHOOLGIRL.TEX")) return 9;
        if (hasFile("ART", "RAYNE_SCHOOLGIRL_BUMPMAP.TEX")) return 9;
        if (hasFile("ART", "RAYNE_SCHOOLGIRL_GLOSSMAP.TEX")) return 9;

        // Armor (base 8)
        if (hasFile("ART", "MANS_METAL_ARMOR.TEX")) return 8;
        if (hasFile("ART", "RAYNE_ARMOR.TEX")) return 8;

        // Cowgirl (base 7)
        if (hasFile("ART", "COWGIRL.TEX")) return 7;

        // Dark Rayne (base 6)
        if (hasFile("ART", "RAYNE_DARK.TEX")) return 6;
        if (hasFile("ART", "RAYNE_DARK_BUMPMAP.TEX")) return 6;
        if (hasFile("ART", "RAYNE_DARK_GLOSSMAP.TEX")) return 6;

        // Standard outfit (base 0)
        if (hasFile("MODELS", "RAYNE.BFM")) return 0;
        if (hasFile("ART", "RAYNE.TEX")) return 0;
        if (hasFile("ART", "RAYNE_BUMPMAP.TEX")) return 0;
        if (hasFile("ART", "RAYNE_GLOSSMAP.TEX")) return 0;

        return 0;
    }

    std::string parseDisplayName(const std::string& folderName) {
        std::string name = folderName;

        size_t prefixEnd = name.find(']');
        if (prefixEnd != std::string::npos && prefixEnd < name.length() - 1) {
            name = name.substr(prefixEnd + 1);
        }

        size_t outfitPos = name.find("Outfit");
        if (outfitPos != std::string::npos) {
            size_t dashPos = name.find(" - ", outfitPos);
            if (dashPos != std::string::npos) {
                name = name.substr(dashPos + 3);
            }
        }

        size_t start = name.find_first_not_of(" \t");
        size_t end = name.find_last_not_of(" \t");
        if (start != std::string::npos && end != std::string::npos) {
            name = name.substr(start, end - start + 1);
        }

        if (name.length() > 60) {
            name = name.substr(0, 60);
        }

        return name;
    }

    std::string generateInternalName(int outfitID) {
        return "custom_" + std::to_string(outfitID);
    }

public:
    Outfit() = default;
    ~Outfit() = default;

    // Public Interface
    bool initialize() {
        if (this->initialized) {
            DEBUG_LOG("[Outfit] Already initialized");
            return true;
        }

        g_outfitInstance = this;

        if (!this->initGamePath()) {
            DEBUG_LOG("[Outfit] Warning: Could not detect game directory");
        }

        this->customOutfits.reserve(OUTFIT_MAX_ENTRIES);

        if (!this->menuHook.initialize(this)) {
            DEBUG_LOG("[Outfit] Warning: Could not initialize menu hook");
        }

        this->initialized = true;
        DEBUG_LOG("[Outfit] Initialized successfully");
        return true;
    }

    bool installHooks() {
        if (this->hooksInstalled) return true;

        if (!this->initialized) {
            DEBUG_LOG("[Outfit] Cannot install hooks - not initialized");
            return false;
        }

        MH_STATUS status = MH_Initialize();
        if (status != MH_OK && status != MH_ERROR_ALREADY_INITIALIZED) {
            DEBUG_LOG("[Outfit] MinHook init failed: " << status);
            return false;
        }

        status = MH_CreateHook(
            (LPVOID)OutfitStuff::FN_GET_GUN_PATH,
            (LPVOID)&HookedGetGunPath,
            (LPVOID*)&this->originalGetGunPath
        );
        if (status != MH_OK) {
            DEBUG_LOG("[Outfit] Failed to create GetGunPath hook: " << status);
            return false;
        }

        status = MH_CreateHook(
            (LPVOID)OutfitStuff::FN_GET_HARPOON_TEX,
            (LPVOID)&HookedGetHarpoonTex,
            (LPVOID*)&this->originalGetHarpoonTex
        );
        if (status != MH_OK) {
            DEBUG_LOG("[Outfit] Failed to create GetHarpoonTex hook: " << status);
            return false;
        }

        status = MH_CreateHook(
            (LPVOID)OutfitStuff::FN_GET_BLADE_PATH_BY_ID,
            (LPVOID)&HookedGetBladePathByID,
            (LPVOID*)&this->originalGetBladePathByID
        );
        if (status != MH_OK) {
            DEBUG_LOG("[Outfit] Failed to create GetBladePathByID hook: " << status);
            return false;
        }

        status = MH_CreateHook(
            (LPVOID)OutfitStuff::FN_GET_INTERNAL_NAME,
            (LPVOID)&HookedGetInternalName,
            (LPVOID*)&this->originalGetInternalName
        );
        if (status != MH_OK) {
            DEBUG_LOG("[Outfit] Failed to create GetInternalName hook: " << status);
            return false;
        }

        status = MH_CreateHook(
            (LPVOID)OutfitStuff::FN_GET_DISPLAY_NAME,
            (LPVOID)&HookedGetDisplayName,
            (LPVOID*)&this->originalGetDisplayName
        );
        if (status != MH_OK) {
            DEBUG_LOG("[Outfit] Failed to create GetDisplayName hook: " << status);
            return false;
        }

        status = MH_CreateHook(
            (LPVOID)OutfitStuff::FN_GET_DFM_PATH,
            (LPVOID)&HookedGetDfmPath,
            (LPVOID*)&this->originalGetDfmPath
        );
        if (status != MH_OK) {
            DEBUG_LOG("[Outfit] Failed to create GetDfmPath hook: " << status);
            return false;
        }

        status = MH_CreateHook(
            (LPVOID)OutfitStuff::FN_GET_BLADE_PATH,
            (LPVOID)&HookedGetBladePath,
            (LPVOID*)&this->originalGetBladePath
        );
        if (status != MH_OK) {
            DEBUG_LOG("[Outfit] Failed to create GetBladePath hook: " << status);
            return false;
        }

        status = MH_CreateHook(
            (LPVOID)OutfitStuff::FN_GET_OUTFIT_INDEX,
            (LPVOID)&HookedGetOutfitIndex,
            (LPVOID*)&this->originalGetOutfitIndex
        );
        if (status != MH_OK) {
            DEBUG_LOG("[Outfit] Failed to create GetOutfitIndex hook: " << status);
            return false;
        }

        status = MH_CreateHook(
            (LPVOID)OutfitStuff::FN_GET_JUG_PATH,
            (LPVOID)&HookedGetJugPath,
            (LPVOID*)&this->originalGetJugPath
        );
        if (status != MH_OK) {
            DEBUG_LOG("[Outfit] Failed to create GetJugPath hook: " << status);
            return false;
        }

        HMODULE kernel32 = GetModuleHandleA("kernel32.dll");
        if (kernel32 == nullptr) {
            DEBUG_LOG("[Outfit] Failed to get kernel32.dll handle");
            return false;
        }

        LPVOID findFirstFileAAddr = GetProcAddress(kernel32, "FindFirstFileA");
        if (findFirstFileAAddr == nullptr) {
            DEBUG_LOG("[Outfit] Failed to get FindFirstFileA address");
            return false;
        }

        DEBUG_LOG("[Outfit] FindFirstFileA address: " << findFirstFileAAddr);

        status = MH_CreateHook(
            findFirstFileAAddr,
            (LPVOID)&HookedFindFirstFileA,
            (LPVOID*)&this->originalFindFirstFileA
        );
        if (status != MH_OK) {
            DEBUG_LOG("[Outfit] Failed to create FindFirstFileA hook: " << status);
            return false;
        }

        DEBUG_LOG("[Outfit] FindFirstFileA hook created, original at: " << (void*)this->originalFindFirstFileA);

        LPVOID createFileAAddr = GetProcAddress(kernel32, "CreateFileA");
        if (createFileAAddr == nullptr) {
            DEBUG_LOG("[Outfit] Failed to get CreateFileA address");
            return false;
        }

        DEBUG_LOG("[Outfit] CreateFileA address: " << createFileAAddr);

        status = MH_CreateHook(
            createFileAAddr,
            (LPVOID)&HookedCreateFileA,
            (LPVOID*)&this->originalCreateFileA
        );
        if (status != MH_OK) {
            DEBUG_LOG("[Outfit] Failed to create CreateFileA hook: " << status);
            return false;
        }

        DEBUG_LOG("[Outfit] CreateFileA hook created, original at: " << (void*)this->originalCreateFileA);

        status = MH_EnableHook(MH_ALL_HOOKS);
        if (status != MH_OK) {
            DEBUG_LOG("[Outfit] Failed to enable hooks: " << status);
            return false;
        }

        if (!this->menuHook.installHook()) {
            DEBUG_LOG("[Outfit] Warning: Failed to install menu hook - custom outfits won't appear in menu");
        }

        this->hooksInstalled = true;
        DEBUG_LOG("[Outfit] All hooks installed successfully");
        return true;
    }

    // Check if handler array is ready and enable loose file priority
    // Call this from WaitForGameReady polling loop
    bool checkAndEnableLooseFiles() {
        if (this->handlersSwapped) return true;

        if (!this->handlersReady) {
            uintptr_t* handlers = (uintptr_t*)OutfitStuff::HANDLER_ARRAY;
            if (handlers[0] != 0 && handlers[1] != 0) {
                this->handlersReady = true;
                DEBUG_LOG("[Outfit] Handler array is now populated: [0]=0x" << std::hex << handlers[0]
                    << ", [1]=0x" << handlers[1]);
            }
            else {
                return false;
            }
        }

        return this->enableLooseFilePriority();
    }

    bool addOutfitFromFolder(const std::string& folderPath) {
        if (!this->initialized) {
            DEBUG_LOG("[Outfit] Cannot add outfit - not initialized");
            return false;
        }

        if (!std::filesystem::exists(folderPath) || !std::filesystem::is_directory(folderPath)) {
            DEBUG_LOG("[Outfit] Invalid folder path: " << folderPath);
            return false;
        }

        if (this->customOutfits.size() >= OUTFIT_MAX_ENTRIES) {
            DEBUG_LOG("[Outfit] Maximum custom outfit count reached (" << OUTFIT_MAX_ENTRIES << ")");
            return false;
        }

        std::string folderName = std::filesystem::path(folderPath).filename().string();

        OutfitEntry entry;
        entry.outfitID = this->nextOutfitID++;
        entry.baseOutfitIndex = this->detectBaseOutfit(folderPath);
        entry.setModFolderPath(folderPath.c_str());

        std::string displayName = this->parseDisplayName(folderName);
        entry.setDisplayName(displayName.c_str());

        std::string internalName = this->generateInternalName(entry.outfitID);
        entry.setInternalName(internalName.c_str());

        entry.inheritFromBase();

        this->customOutfits.push_back(entry);

        DEBUG_LOG("[Outfit] Added outfit '" << displayName << "' (ID: " << entry.outfitID
            << ", base: " << entry.baseOutfitIndex << ") from: " << folderPath);

        return true;
    }

    int scanDirectory(const std::string& directoryPath) {
        if (!std::filesystem::exists(directoryPath) || !std::filesystem::is_directory(directoryPath)) {
            DEBUG_LOG("[Outfit] Invalid directory path: " << directoryPath);
            return 0;
        }

        int count = 0;
        for (const auto& entry : std::filesystem::directory_iterator(directoryPath)) {
            if (entry.is_directory()) {
                std::string subPath = entry.path().string();
                if (std::filesystem::exists(subPath + "\\ART") || std::filesystem::exists(subPath + "\\MODELS")) {
                    if (this->addOutfitFromFolder(subPath)) {
                        count++;
                    }
                }
            }
        }

        DEBUG_LOG("[Outfit] Scanned directory, found " << count << " outfit mods");
        return count;
    }

    // Getters
    int getTotalOutfitCount() const { return OutfitStuff::BASE_OUTFIT_COUNT + (int)this->customOutfits.size(); }
    const std::vector<OutfitEntry>& getCustomOutfits() const { return this->customOutfits; }

    void printStatus() const {
        DEBUG_LOG("[Outfit] Status:");
        DEBUG_LOG("  Initialized: " << (this->initialized ? "yes" : "no"));
        DEBUG_LOG("  Hooks installed: " << (this->hooksInstalled ? "yes" : "no"));
        DEBUG_LOG("  Loose file priority: " << (this->handlersSwapped ? "yes" : "no"));
        DEBUG_LOG("  Game directory: " << this->gameDirectory);
        DEBUG_LOG("  Custom outfits: " << this->customOutfits.size());
        DEBUG_LOG("  Total outfit count: " << this->getTotalOutfitCount());

        for (size_t i = 0; i < this->customOutfits.size(); i++) {
            const OutfitEntry& entry = this->customOutfits[i];
            DEBUG_LOG("  [" << i << "] " << entry.displayName << " (ID: " << entry.outfitID
                << ", base: " << entry.baseOutfitIndex << ")");
        }
    }
};

// OutfitMenuHook - Deferred implementations
static void __cdecl OutfitMenuHook_Impl() {
    if (!g_menuHookInstance) {
        return;
    }

    int menuState = *(int*)OutfitStuff::MENU_STATE;
    if (menuState != OutfitStuff::MENU_STATE_OUTFITS) {
        g_menuHookInstance->outfitMenuCallCount = 0;
        return;
    }

    g_menuHookInstance->outfitMenuCallCount++;

    if (g_menuHookInstance->outfitMenuCallCount == OutfitStuff::BASE_OUTFIT_COUNT) {
        g_menuHookInstance->injectCustomOutfits();
        g_menuHookInstance->outfitMenuCallCount = 0;
    }
}

inline void OutfitMenuHook::injectCustomOutfits() {
    if (!this->outfitSystem) {
        DEBUG_LOG("[MenuHook] No outfit system reference");
        return;
    }

    const std::vector<OutfitEntry>& customOutfits = this->outfitSystem->getCustomOutfits();
    if (customOutfits.empty()) {
        DEBUG_LOG("[MenuHook] No custom outfits to inject");
        return;
    }

    DEBUG_LOG("[MenuHook] Injecting " << customOutfits.size() << " custom outfit(s) into menu");

    for (size_t i = 0; i < customOutfits.size(); i++) {
        const OutfitEntry& outfit = customOutfits[i];
        this->addOutfitMenuItem(outfit.outfitID, outfit.displayName);
    }
}

// SetOutfitIndex Hook Implementation
static void __cdecl SetOutfitIndexHook_Impl() {
    if (!g_menuHookInstance || !g_menuHookInstance->outfitSystem) {
        return;
    }

    int menuSelectionIndex = *(int*)OutfitStuff::MENU_SELECTION_INDEX;
    int menuState = *(int*)OutfitStuff::MENU_STATE;
    int requestedOutfitID = (int)g_setOutfitIndex_outfitID;

    DEBUG_LOG("[SetOutfitHook] Called: requestedID=" << requestedOutfitID
        << ", menuIndex=" << menuSelectionIndex
        << ", menuState=0x" << std::hex << menuState << std::dec);

    if (menuState != OutfitStuff::MENU_STATE_OUTFITS) {
        DEBUG_LOG("[SetOutfitHook] Not in outfit menu, passing through");
        return;
    }

    if (menuSelectionIndex >= OutfitStuff::BASE_OUTFIT_COUNT) {
        int customOutfitIndex = menuSelectionIndex - OutfitStuff::BASE_OUTFIT_COUNT;

        const std::vector<OutfitEntry>& customOutfits = g_menuHookInstance->outfitSystem->getCustomOutfits();

        DEBUG_LOG("[SetOutfitHook] Custom selection detected: menuIndex=" << menuSelectionIndex
            << ", customOutfitIndex=" << customOutfitIndex
            << ", totalCustomOutfits=" << customOutfits.size());

        if (customOutfitIndex >= 0 && customOutfitIndex < (int)customOutfits.size()) {
            int customOutfitID = customOutfits[customOutfitIndex].outfitID;

            DEBUG_LOG("[SetOutfitHook] Setting custom outfit: "
                << customOutfits[customOutfitIndex].displayName
                << " (ID=" << customOutfitID << ")");

            *(int*)OutfitStuff::OUTFIT_INDEX_STORAGE = customOutfitID;
            g_setOutfitIndex_outfitID = customOutfitID;
        }
        else {
            DEBUG_LOG("[SetOutfitHook] ERROR: Invalid custom outfit index: " << customOutfitIndex
                << " (valid range: 0-" << (customOutfits.size() - 1) << ")");
        }
    }
    else {
        DEBUG_LOG("[SetOutfitHook] Base outfit selected, passing through");
    }
}