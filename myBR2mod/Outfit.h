#pragma once
#include <Windows.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <filesystem>
#include <algorithm>
#include "Config.h"
#include "MinHook.h"

// ============================================================================
// Outfit System for BloodRayne 2: Terminal Cut
// ============================================================================
// Enables loading custom outfits by hooking the game's outfit accessor
// functions and redirecting file loads via CreateFileA hook.
//
// When a custom outfit is active, file requests for Rayne assets are 
// intercepted and redirected to the mod's folder structure.
// ============================================================================

// ============================================================================
// Game Function Addresses
// ============================================================================

namespace Rayne2Outfit {
    // Accessor function addresses
    constexpr uintptr_t FN_GET_DFM_PATH = 0x004f69b0;          // Returns dfmPath for current outfit
    constexpr uintptr_t FN_GET_GUN_PATH = 0x004f69e0;          // Returns gunPath by outfit ID
    constexpr uintptr_t FN_GET_HARPOON_TEX = 0x004f6a30;       // Returns harpoonTexture by outfit ID
    constexpr uintptr_t FN_GET_BLADE_PATH = 0x004f6a70;        // Returns bladePath for current outfit
    constexpr uintptr_t FN_GET_BLADE_PATH_BY_ID = 0x004f6a80;  // Returns bladePath by outfit ID
    constexpr uintptr_t FN_GET_INTERNAL_NAME = 0x004f6ad0;     // Returns internalName by outfit ID
    constexpr uintptr_t FN_GET_DISPLAY_NAME = 0x004f6b10;      // Returns displayName by outfit ID
    constexpr uintptr_t FN_GET_OUTFIT_INDEX = 0x004f6930;      // Returns current outfit table index
    constexpr uintptr_t FN_GET_JUG_PATH = 0x004f69c0;          // Returns jugPath for current outfit (no param)

    // Player outfit ID location
    constexpr uintptr_t PTR_PLAYER_STATE = 0x007c07f8;         // Pointer to player state object
    constexpr uintptr_t OFFSET_OUTFIT_ID = 0x3cc;              // Offset to outfit ID within player state

    // Asset handler system (for loose file priority)
    constexpr uintptr_t HANDLER_ARRAY = 0x05dcc0a0;
    constexpr uintptr_t HANDLER_COUNT = 0x05dcc0b4;

    // Original outfit count
    constexpr int BASE_OUTFIT_COUNT = 10;
}

// ============================================================================
// Function Pointer Types
// ============================================================================

// Functions that take an outfit ID parameter use __stdcall (callee cleans stack, RET 0x4)
typedef char* (__stdcall* GetPathByID_t)(int outfitID);

// Functions with no parameters use __cdecl (plain RET)
typedef char* (__cdecl* GetPathForCurrent_t)(void);
typedef unsigned int(__cdecl* GetOutfitIndex_t)(void);

// CreateFileA signature
typedef HANDLE(WINAPI* CreateFileA_t)(
    LPCSTR lpFileName,
    DWORD dwDesiredAccess,
    DWORD dwShareMode,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    DWORD dwCreationDisposition,
    DWORD dwFlagsAndAttributes,
    HANDLE hTemplateFile
    );

// FindFirstFileA signature
typedef HANDLE(WINAPI* FindFirstFileA_t)(LPCSTR lpFileName, LPWIN32_FIND_DATAA lpFindFileData);

// ============================================================================
// Base Outfit Definitions
// ============================================================================

struct BaseOutfitDefinition {
    int index;
    const char* internalName;
    const char* dfmPath;
    const char* gunPath;
    const char* harpoonTexture;
    const char* bladePath;
    const char* jugPath;
};

static const BaseOutfitDefinition baseOutfits[] = {
    { 0, "rayne",             "rayne.dfm",             "WEAPONS_RAYNES_GUN.SMF",             "chainlink.tif", "weapons_rayne_blade_01.smf",      "rayne.jug" },
    { 1, "rayne_dress",       "rayne_dress.dfm",       "WEAPONS_RAYNES_GUN.SMF",             "chainlink.tif", "weapons_rayne_blade_01.smf",      "rayne_dress.jug" },
    { 2, "rayne_dress_blue",  "rayne_dress_blue.dfm",  "WEAPONS_RAYNES_GUN.SMF",             "chainlink.tif", "weapons_rayne_blade_01.smf",      "rayne_dress.jug" },
    { 3, "rayne_dress_red",   "rayne_dress_red.dfm",   "WEAPONS_RAYNES_GUN.SMF",             "chainlink.tif", "weapons_rayne_blade_01.smf",      "rayne_dress.jug" },
    { 4, "rayne_dress_dragon","rayne_dress_dragon.dfm","WEAPONS_RAYNES_GUN.SMF",             "chainlink.tif", "weapons_rayne_blade_01.smf",      "rayne_dress.jug" },
    { 5, "rayne_dress_green", "rayne_dress_green.dfm", "WEAPONS_RAYNES_GUN.SMF",             "chainlink.tif", "weapons_rayne_blade_01.smf",      "rayne_dress.jug" },
    { 6, "rayne_dark",        "rayne_dark.dfm",        "weapons_dark_raynes_gun.smf",        "chainlink.tif", "weapons_rayne_blade_dark.smf",    "rayne_dark.jug" },
    { 7, "rayne_cowgirl",     "rayne_cowgirl.dfm",     "weapons_cowgirl_raynes_gun.smf",     "bullwhip.tif",  "weapons_rayne_blade_cowgirl.smf", "rayne_cowgirl.jug" },
    { 8, "rayne_armor",       "rayne_armor.dfm",       "weapons_armor_raynes_gun.smf",       "chainlink.tif", "weapons_rayne_blade_armor.smf",   "rayne_armor.jug" },
    { 9, "rayne_schoolgirl",  "rayne_schoolgirl.dfm",  "weapons_rayne_desert eagle_gun.smf", "chainlink.tif", "weapons_rayne_katana.smf",        "rayne_schoolgirl.jug" }
};

// ============================================================================
// OutfitEntry
// ============================================================================

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

// ============================================================================
// Outfit Class - Main Interface
// ============================================================================
// Forward declaration for the static hook callbacks
class Outfit;
static Outfit* g_outfitInstance = nullptr;

class Outfit {
private:
    bool initialized = false;
    bool hooksInstalled = false;
    bool handlersSwapped = false;
    bool handlersReady = false;

    std::vector<OutfitEntry> customOutfits;
    int nextOutfitID = Rayne2Outfit::BASE_OUTFIT_COUNT + 1;  // Start at 11

    // Game directory path (detected at runtime)
    char gameDirectory[MAX_PATH] = { 0 };
    size_t gameDirectoryLength = 0;

    // Original function pointers (for calling the original implementations)
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

    // Original handler order for restoration
    uintptr_t originalHandlerOrder[2] = { 0, 0 };

    // ========================================================================
    // Helper: Detect game directory from executable path
    // ========================================================================
    bool detectGameDirectory() {
        if (this->gameDirectoryLength > 0) return true;  // Already detected

        char exePath[MAX_PATH];
        DWORD len = GetModuleFileNameA(NULL, exePath, MAX_PATH);
        if (len == 0 || len >= MAX_PATH) {
            DEBUG_LOG("[Outfit] Failed to get module filename");
            return false;
        }

        // Find last backslash to strip executable name
        char* lastSlash = strrchr(exePath, '\\');
        if (lastSlash == nullptr) {
            DEBUG_LOG("[Outfit] Invalid executable path format");
            return false;
        }

        // Include the trailing backslash
        size_t dirLen = (lastSlash - exePath) + 1;
        memcpy(this->gameDirectory, exePath, dirLen);
        this->gameDirectory[dirLen] = '\0';
        this->gameDirectoryLength = dirLen;

        DEBUG_LOG("[Outfit] Game directory: " << this->gameDirectory);
        return true;
    }

    // ========================================================================
    // Helper: Case-insensitive string comparison
    // ========================================================================
    static bool strEqualsIgnoreCase(const char* a, const char* b) {
        return _stricmp(a, b) == 0;
    }

    // ========================================================================
    // Helper: Check if path is a Rayne-related asset
    // ========================================================================
    bool isRayneAsset(const char* relativePath) const {
        if (relativePath == nullptr) return false;

        // Convert to lowercase for comparison
        char lowerPath[MAX_PATH];
        size_t len = strlen(relativePath);
        if (len >= MAX_PATH) return false;

        for (size_t i = 0; i <= len; i++) {
            lowerPath[i] = (char)tolower((unsigned char)relativePath[i]);
        }

        // Check for Rayne-related patterns
        // Models: rayne.bfm, rayne_*.bfm, weapons_rayne*.smf
        // Textures: rayne.tex, rayne_*.tex, hrayne*.tif
        // Other: rayne.dfm, rayne_*.dfm, rayne.jug, rayne_*.jug

        const char* filename = strrchr(lowerPath, '\\');
        if (filename == nullptr) {
            filename = lowerPath;
        }
        else {
            filename++;  // Skip the backslash
        }

        // Check filename patterns
        if (strncmp(filename, "rayne", 5) == 0) return true;
        if (strncmp(filename, "hrayne", 6) == 0) return true;
        if (strncmp(filename, "weapons_rayne", 13) == 0) return true;

        return false;
    }

    // ========================================================================
    // Helper: Get current player's outfit ID
    // ========================================================================
    int getCurrentOutfitID() {
        uintptr_t playerStatePtr = *(uintptr_t*)Rayne2Outfit::PTR_PLAYER_STATE;
        if (playerStatePtr == 0) return 1;  // Default to standard outfit
        return *(int*)(playerStatePtr + Rayne2Outfit::OFFSET_OUTFIT_ID);
    }

    // ========================================================================
    // Helper: Find custom outfit by ID
    // ========================================================================
    OutfitEntry* findCustomOutfit(int outfitID) {
        for (auto& entry : this->customOutfits) {
            if (entry.outfitID == outfitID) {
                return &entry;
            }
        }
        return nullptr;
    }

    // ========================================================================
    // Hook Callbacks (static, forwarding to instance methods)
    // ========================================================================

    static char* __stdcall HookedGetGunPath(int outfitID) {
        if (g_outfitInstance && outfitID > Rayne2Outfit::BASE_OUTFIT_COUNT) {
            OutfitEntry* entry = g_outfitInstance->findCustomOutfit(outfitID);
            if (entry) {
                DEBUG_LOG("[Outfit] GetGunPath(" << outfitID << ") -> " << entry->gunPath);
                return entry->gunPath;
            }
        }
        return g_outfitInstance->originalGetGunPath(outfitID);
    }

    static char* __stdcall HookedGetHarpoonTex(int outfitID) {
        if (g_outfitInstance && outfitID > Rayne2Outfit::BASE_OUTFIT_COUNT) {
            OutfitEntry* entry = g_outfitInstance->findCustomOutfit(outfitID);
            if (entry) {
                DEBUG_LOG("[Outfit] GetHarpoonTex(" << outfitID << ") -> " << entry->harpoonTexture);
                return entry->harpoonTexture;
            }
        }
        return g_outfitInstance->originalGetHarpoonTex(outfitID);
    }

    static char* __stdcall HookedGetBladePathByID(int outfitID) {
        if (g_outfitInstance && outfitID > Rayne2Outfit::BASE_OUTFIT_COUNT) {
            OutfitEntry* entry = g_outfitInstance->findCustomOutfit(outfitID);
            if (entry) {
                DEBUG_LOG("[Outfit] GetBladePathByID(" << outfitID << ") -> " << entry->bladePath);
                return entry->bladePath;
            }
        }
        return g_outfitInstance->originalGetBladePathByID(outfitID);
    }

    static char* __stdcall HookedGetInternalName(int outfitID) {
        if (g_outfitInstance && outfitID > Rayne2Outfit::BASE_OUTFIT_COUNT) {
            OutfitEntry* entry = g_outfitInstance->findCustomOutfit(outfitID);
            if (entry) {
                DEBUG_LOG("[Outfit] GetInternalName(" << outfitID << ") -> " << entry->internalName);
                return entry->internalName;
            }
        }
        return g_outfitInstance->originalGetInternalName(outfitID);
    }

    static char* __stdcall HookedGetDisplayName(int outfitID) {
        if (g_outfitInstance && outfitID > Rayne2Outfit::BASE_OUTFIT_COUNT) {
            OutfitEntry* entry = g_outfitInstance->findCustomOutfit(outfitID);
            if (entry) {
                DEBUG_LOG("[Outfit] GetDisplayName(" << outfitID << ") -> " << entry->displayName);
                return entry->displayName;
            }
        }
        return g_outfitInstance->originalGetDisplayName(outfitID);
    }

    // JugPath uses current outfit (no parameter)
    static char* __cdecl HookedGetJugPath() {
        if (g_outfitInstance) {
            int outfitID = g_outfitInstance->getCurrentOutfitID();
            if (outfitID > Rayne2Outfit::BASE_OUTFIT_COUNT) {
                OutfitEntry* entry = g_outfitInstance->findCustomOutfit(outfitID);
                if (entry) {
                    DEBUG_LOG("[Outfit] GetJugPath() [ID=" << outfitID << "] -> " << entry->jugPath);
                    return entry->jugPath;
                }
            }
        }
        return g_outfitInstance->originalGetJugPath();
    }

    // These use the current outfit ID rather than taking a parameter
    static char* __cdecl HookedGetDfmPath() {
        if (g_outfitInstance) {
            int outfitID = g_outfitInstance->getCurrentOutfitID();
            if (outfitID > Rayne2Outfit::BASE_OUTFIT_COUNT) {
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
            if (outfitID > Rayne2Outfit::BASE_OUTFIT_COUNT) {
                OutfitEntry* entry = g_outfitInstance->findCustomOutfit(outfitID);
                if (entry) {
                    DEBUG_LOG("[Outfit] GetBladePath() [ID=" << outfitID << "] -> " << entry->bladePath);
                    return entry->bladePath;
                }
            }
        }
        return g_outfitInstance->originalGetBladePath();
    }

    // GetOutfitIndex - returns table index (0-9 for base outfits)
    // For custom outfits, we return the index of the base outfit they extend
    // This is needed because some game code uses the index to access the table directly
    static unsigned int __cdecl HookedGetOutfitIndex() {
        if (g_outfitInstance) {
            int outfitID = g_outfitInstance->getCurrentOutfitID();
            if (outfitID > Rayne2Outfit::BASE_OUTFIT_COUNT) {
                OutfitEntry* entry = g_outfitInstance->findCustomOutfit(outfitID);
                if (entry) {
                    // Return the base outfit's index so table lookups work
                    // DEBUG_LOG("[Outfit] GetOutfitIndex() [ID=" << outfitID << "] -> base index " << entry->baseOutfitIndex);
                    return (unsigned int)entry->baseOutfitIndex;
                }
            }
        }
        return g_outfitInstance->originalGetOutfitIndex();
    }

    // ========================================================================
    // FindFirstFileA Hook - Redirect file existence checks to mod folders
    // ========================================================================
    static HANDLE WINAPI HookedFindFirstFileA(LPCSTR lpFileName, LPWIN32_FIND_DATAA lpFindFileData) {
        // Safety: must have valid instance and original function
        if (!g_outfitInstance || !g_outfitInstance->originalFindFirstFileA) {
            return INVALID_HANDLE_VALUE;
        }

        // Early exit for null filename
        if (lpFileName == nullptr) {
            return g_outfitInstance->originalFindFirstFileA(lpFileName, lpFindFileData);
        }

        // Check if we have a custom outfit active
        int outfitID = g_outfitInstance->getCurrentOutfitID();
        if (outfitID <= Rayne2Outfit::BASE_OUTFIT_COUNT) {
            return g_outfitInstance->originalFindFirstFileA(lpFileName, lpFindFileData);
        }

        OutfitEntry* entry = g_outfitInstance->findCustomOutfit(outfitID);
        if (!entry || entry->modFolderPath[0] == '\0') {
            return g_outfitInstance->originalFindFirstFileA(lpFileName, lpFindFileData);
        }

        // Determine relative path - handle both absolute and relative paths
        const char* relativePath = nullptr;

        // Check if it's an absolute path starting with game directory
        if (g_outfitInstance->gameDirectoryLength > 0 &&
            _strnicmp(lpFileName, g_outfitInstance->gameDirectory,
                g_outfitInstance->gameDirectoryLength) == 0) {
            relativePath = lpFileName + g_outfitInstance->gameDirectoryLength;
        }
        // Check if it's already a relative path (no drive letter or leading backslash)
        else if (lpFileName[0] != '\\' && (strlen(lpFileName) < 2 || lpFileName[1] != ':')) {
            relativePath = lpFileName;
        }

        if (relativePath == nullptr) {
            return g_outfitInstance->originalFindFirstFileA(lpFileName, lpFindFileData);
        }

        // Check if this is a Rayne-related asset
        if (!g_outfitInstance->isRayneAsset(relativePath)) {
            return g_outfitInstance->originalFindFirstFileA(lpFileName, lpFindFileData);
        }

        // Build mod path
        char modPath[MAX_PATH];
        snprintf(modPath, sizeof(modPath), "%s\\%s", entry->modFolderPath, relativePath);

        // Check if file exists in mod folder
        HANDLE hFind = g_outfitInstance->originalFindFirstFileA(modPath, lpFindFileData);
        if (hFind != INVALID_HANDLE_VALUE) {
            DEBUG_LOG("[FindFirstFileA] Redirecting: " << relativePath << " -> " << modPath);
            // Return the handle from mod path check - game thinks original path exists
            return hFind;
        }

        // Fall through to original path
        return g_outfitInstance->originalFindFirstFileA(lpFileName, lpFindFileData);
    }

    // ========================================================================
    // CreateFileA Hook - Intercept file loads and redirect to mod folders
    // ========================================================================
    static HANDLE WINAPI HookedCreateFileA(
        LPCSTR lpFileName,
        DWORD dwDesiredAccess,
        DWORD dwShareMode,
        LPSECURITY_ATTRIBUTES lpSecurityAttributes,
        DWORD dwCreationDisposition,
        DWORD dwFlagsAndAttributes,
        HANDLE hTemplateFile
    ) {
        // Safety: must have valid instance and original function
        if (!g_outfitInstance || !g_outfitInstance->originalCreateFileA) {
            return INVALID_HANDLE_VALUE;
        }

        // Debug: log first 10 calls to verify hook is working
        static int createFileCallCount = 0;
        if (createFileCallCount < 10) {
            DEBUG_LOG("[CreateFileA] Call #" << createFileCallCount << ": " << (lpFileName ? lpFileName : "NULL"));
            createFileCallCount++;
        }

        // Early exit for null filename
        if (lpFileName == nullptr) {
            return g_outfitInstance->originalCreateFileA(
                lpFileName, dwDesiredAccess, dwShareMode,
                lpSecurityAttributes, dwCreationDisposition,
                dwFlagsAndAttributes, hTemplateFile
            );
        }

        // Debug: log any Rayne-related asset
        if (strstr(lpFileName, "rayne") != nullptr || strstr(lpFileName, "RAYNE") != nullptr) {
            DEBUG_LOG("[CreateFileA] Rayne asset detected: " << lpFileName);
            DEBUG_LOG("[CreateFileA] Outfit ID: " << g_outfitInstance->getCurrentOutfitID());
        }

        // Check if we have a custom outfit active
        int outfitID = g_outfitInstance->getCurrentOutfitID();
        if (outfitID <= Rayne2Outfit::BASE_OUTFIT_COUNT) {
            return g_outfitInstance->originalCreateFileA(
                lpFileName, dwDesiredAccess, dwShareMode,
                lpSecurityAttributes, dwCreationDisposition,
                dwFlagsAndAttributes, hTemplateFile
            );
        }

        OutfitEntry* entry = g_outfitInstance->findCustomOutfit(outfitID);
        if (!entry || entry->modFolderPath[0] == '\0') {
            return g_outfitInstance->originalCreateFileA(
                lpFileName, dwDesiredAccess, dwShareMode,
                lpSecurityAttributes, dwCreationDisposition,
                dwFlagsAndAttributes, hTemplateFile
            );
        }

        // Determine relative path - handle both absolute and relative paths
        const char* relativePath = nullptr;

        // Check if it's an absolute path starting with game directory
        if (g_outfitInstance->gameDirectoryLength > 0 &&
            _strnicmp(lpFileName, g_outfitInstance->gameDirectory,
                g_outfitInstance->gameDirectoryLength) == 0) {
            relativePath = lpFileName + g_outfitInstance->gameDirectoryLength;
        }
        // Check if it's already a relative path (no drive letter or leading backslash)
        else if (lpFileName[0] != '\\' && (strlen(lpFileName) < 2 || lpFileName[1] != ':')) {
            relativePath = lpFileName;
        }

        if (relativePath == nullptr) {
            return g_outfitInstance->originalCreateFileA(
                lpFileName, dwDesiredAccess, dwShareMode,
                lpSecurityAttributes, dwCreationDisposition,
                dwFlagsAndAttributes, hTemplateFile
            );
        }

        // Check if this is a Rayne-related asset
        if (!g_outfitInstance->isRayneAsset(relativePath)) {
            return g_outfitInstance->originalCreateFileA(
                lpFileName, dwDesiredAccess, dwShareMode,
                lpSecurityAttributes, dwCreationDisposition,
                dwFlagsAndAttributes, hTemplateFile
            );
        }

        // Build mod path
        char modPath[MAX_PATH];
        snprintf(modPath, sizeof(modPath), "%s\\%s", entry->modFolderPath, relativePath);

        // Check if file exists in mod folder using FindFirstFileA
        WIN32_FIND_DATAA findData;
        HANDLE hFind = g_outfitInstance->originalFindFirstFileA(modPath, &findData);
        if (hFind != INVALID_HANDLE_VALUE) {
            FindClose(hFind);
            DEBUG_LOG("[CreateFileA] Redirecting: " << relativePath << " -> " << modPath);
            return g_outfitInstance->originalCreateFileA(
                modPath, dwDesiredAccess, dwShareMode,
                lpSecurityAttributes, dwCreationDisposition,
                dwFlagsAndAttributes, hTemplateFile
            );
        }

        // Fall through to original path
        return g_outfitInstance->originalCreateFileA(
            lpFileName, dwDesiredAccess, dwShareMode,
            lpSecurityAttributes, dwCreationDisposition,
            dwFlagsAndAttributes, hTemplateFile
        );
    }

    // ========================================================================
    // Install/Uninstall Hooks
    // ========================================================================

    bool installHooks() {
        if (this->hooksInstalled) return true;

        if (!this->initialized) {
            DEBUG_LOG("[Outfit] Cannot install hooks - not initialized");
            return false;
        }

        MH_STATUS status;

        // Hook GetGunPath
        status = MH_CreateHook(
            (LPVOID)Rayne2Outfit::FN_GET_GUN_PATH,
            (LPVOID)&HookedGetGunPath,
            (LPVOID*)&this->originalGetGunPath
        );
        if (status != MH_OK) {
            DEBUG_LOG("[Outfit] Failed to create GetGunPath hook: " << status);
            return false;
        }

        // Hook GetHarpoonTex
        status = MH_CreateHook(
            (LPVOID)Rayne2Outfit::FN_GET_HARPOON_TEX,
            (LPVOID)&HookedGetHarpoonTex,
            (LPVOID*)&this->originalGetHarpoonTex
        );
        if (status != MH_OK) {
            DEBUG_LOG("[Outfit] Failed to create GetHarpoonTex hook: " << status);
            return false;
        }

        // Hook GetBladePathByID
        status = MH_CreateHook(
            (LPVOID)Rayne2Outfit::FN_GET_BLADE_PATH_BY_ID,
            (LPVOID)&HookedGetBladePathByID,
            (LPVOID*)&this->originalGetBladePathByID
        );
        if (status != MH_OK) {
            DEBUG_LOG("[Outfit] Failed to create GetBladePathByID hook: " << status);
            return false;
        }

        // Hook GetInternalName
        status = MH_CreateHook(
            (LPVOID)Rayne2Outfit::FN_GET_INTERNAL_NAME,
            (LPVOID)&HookedGetInternalName,
            (LPVOID*)&this->originalGetInternalName
        );
        if (status != MH_OK) {
            DEBUG_LOG("[Outfit] Failed to create GetInternalName hook: " << status);
            return false;
        }

        // Hook GetDisplayName
        status = MH_CreateHook(
            (LPVOID)Rayne2Outfit::FN_GET_DISPLAY_NAME,
            (LPVOID)&HookedGetDisplayName,
            (LPVOID*)&this->originalGetDisplayName
        );
        if (status != MH_OK) {
            DEBUG_LOG("[Outfit] Failed to create GetDisplayName hook: " << status);
            return false;
        }

        // Hook GetDfmPath
        status = MH_CreateHook(
            (LPVOID)Rayne2Outfit::FN_GET_DFM_PATH,
            (LPVOID)&HookedGetDfmPath,
            (LPVOID*)&this->originalGetDfmPath
        );
        if (status != MH_OK) {
            DEBUG_LOG("[Outfit] Failed to create GetDfmPath hook: " << status);
            return false;
        }

        // Hook GetBladePath (no ID parameter version)
        status = MH_CreateHook(
            (LPVOID)Rayne2Outfit::FN_GET_BLADE_PATH,
            (LPVOID)&HookedGetBladePath,
            (LPVOID*)&this->originalGetBladePath
        );
        if (status != MH_OK) {
            DEBUG_LOG("[Outfit] Failed to create GetBladePath hook: " << status);
            return false;
        }

        // Hook GetOutfitIndex
        status = MH_CreateHook(
            (LPVOID)Rayne2Outfit::FN_GET_OUTFIT_INDEX,
            (LPVOID)&HookedGetOutfitIndex,
            (LPVOID*)&this->originalGetOutfitIndex
        );
        if (status != MH_OK) {
            DEBUG_LOG("[Outfit] Failed to create GetOutfitIndex hook: " << status);
            return false;
        }

        // Hook GetJugPath
        status = MH_CreateHook(
            (LPVOID)Rayne2Outfit::FN_GET_JUG_PATH,
            (LPVOID)&HookedGetJugPath,
            (LPVOID*)&this->originalGetJugPath
        );
        if (status != MH_OK) {
            DEBUG_LOG("[Outfit] Failed to create GetJugPath hook: " << status);
            return false;
        }

        // Get kernel32 handle for Windows API hooks
        HMODULE kernel32 = GetModuleHandleA("kernel32.dll");
        if (kernel32 == nullptr) {
            DEBUG_LOG("[Outfit] Failed to get kernel32.dll handle");
            return false;
        }

        // Hook FindFirstFileA - for redirecting file existence checks to mod folders
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

        // Hook CreateFileA - for redirecting file loads to mod folders
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

        // Enable all hooks
        status = MH_EnableHook(MH_ALL_HOOKS);
        if (status != MH_OK) {
            DEBUG_LOG("[Outfit] Failed to enable hooks: " << status);
            return false;
        }

        this->hooksInstalled = true;
        DEBUG_LOG("[Outfit] All hooks installed successfully");
        return true;
    }

    void uninstallHooks() {
        if (!this->hooksInstalled) return;

        MH_DisableHook((LPVOID)Rayne2Outfit::FN_GET_GUN_PATH);
        MH_DisableHook((LPVOID)Rayne2Outfit::FN_GET_HARPOON_TEX);
        MH_DisableHook((LPVOID)Rayne2Outfit::FN_GET_BLADE_PATH_BY_ID);
        MH_DisableHook((LPVOID)Rayne2Outfit::FN_GET_INTERNAL_NAME);
        MH_DisableHook((LPVOID)Rayne2Outfit::FN_GET_DISPLAY_NAME);
        MH_DisableHook((LPVOID)Rayne2Outfit::FN_GET_DFM_PATH);
        MH_DisableHook((LPVOID)Rayne2Outfit::FN_GET_BLADE_PATH);
        MH_DisableHook((LPVOID)Rayne2Outfit::FN_GET_OUTFIT_INDEX);
        MH_DisableHook((LPVOID)Rayne2Outfit::FN_GET_JUG_PATH);

        HMODULE kernel32 = GetModuleHandleA("kernel32.dll");
        if (kernel32) {
            LPVOID findFirstFileAAddr = GetProcAddress(kernel32, "FindFirstFileA");
            if (findFirstFileAAddr) {
                MH_DisableHook(findFirstFileAAddr);
                MH_RemoveHook(findFirstFileAAddr);
            }

            LPVOID createFileAAddr = GetProcAddress(kernel32, "CreateFileA");
            if (createFileAAddr) {
                MH_DisableHook(createFileAAddr);
                MH_RemoveHook(createFileAAddr);
            }
        }

        MH_RemoveHook((LPVOID)Rayne2Outfit::FN_GET_GUN_PATH);
        MH_RemoveHook((LPVOID)Rayne2Outfit::FN_GET_HARPOON_TEX);
        MH_RemoveHook((LPVOID)Rayne2Outfit::FN_GET_BLADE_PATH_BY_ID);
        MH_RemoveHook((LPVOID)Rayne2Outfit::FN_GET_INTERNAL_NAME);
        MH_RemoveHook((LPVOID)Rayne2Outfit::FN_GET_DISPLAY_NAME);
        MH_RemoveHook((LPVOID)Rayne2Outfit::FN_GET_DFM_PATH);
        MH_RemoveHook((LPVOID)Rayne2Outfit::FN_GET_BLADE_PATH);
        MH_RemoveHook((LPVOID)Rayne2Outfit::FN_GET_OUTFIT_INDEX);
        MH_RemoveHook((LPVOID)Rayne2Outfit::FN_GET_JUG_PATH);

        this->hooksInstalled = false;
        DEBUG_LOG("[Outfit] All hooks uninstalled");
    }

    // ========================================================================
    // Asset Handler Priority
    // ========================================================================

    // Check if the handler array is populated (non-zero values)
    bool checkHandlersReady() {
        if (this->handlersReady) return true;

        uintptr_t* handlers = (uintptr_t*)Rayne2Outfit::HANDLER_ARRAY;

        // Check if handlers are populated (non-zero)
        if (handlers[0] != 0 && handlers[1] != 0) {
            this->handlersReady = true;
            DEBUG_LOG("[Outfit] Handler array is now populated: [0]=0x" << std::hex << handlers[0]
                << ", [1]=0x" << handlers[1]);
            return true;
        }
        return false;
    }

    bool enableLooseFilePriority() {
        if (this->handlersSwapped) return true;

        if (!this->handlersReady) {
            DEBUG_LOG("[Outfit] Cannot swap handlers - not ready yet");
            return false;
        }

        uintptr_t* handlers = (uintptr_t*)Rayne2Outfit::HANDLER_ARRAY;

        // Store original order
        this->originalHandlerOrder[0] = handlers[0];
        this->originalHandlerOrder[1] = handlers[1];

        DEBUG_LOG("[Outfit] Original handler order: [0]=0x" << std::hex << this->originalHandlerOrder[0]
            << ", [1]=0x" << this->originalHandlerOrder[1]);

        // Change memory protection to allow writing
        DWORD oldProtect;
        if (!VirtualProtect((LPVOID)Rayne2Outfit::HANDLER_ARRAY, 8, PAGE_READWRITE, &oldProtect)) {
            DEBUG_LOG("[Outfit] Failed to change handler array protection, error: " << GetLastError());
            return false;
        }

        // Swap so loose file handler (index 1) comes before POD handler (index 0)
        handlers[0] = this->originalHandlerOrder[1];  // Loose file handler first
        handlers[1] = this->originalHandlerOrder[0];  // POD handler second

        // Restore original protection
        VirtualProtect((LPVOID)Rayne2Outfit::HANDLER_ARRAY, 8, oldProtect, &oldProtect);

        // Verify the swap worked
        DEBUG_LOG("[Outfit] New handler order: [0]=0x" << std::hex << handlers[0]
            << ", [1]=0x" << handlers[1]);

        this->handlersSwapped = true;
        DEBUG_LOG("[Outfit] Loose file priority enabled (handlers swapped)");
        return true;
    }

    void disableLooseFilePriority() {
        if (!this->handlersSwapped) return;

        uintptr_t* handlers = (uintptr_t*)Rayne2Outfit::HANDLER_ARRAY;

        DWORD oldProtect;
        if (VirtualProtect((LPVOID)Rayne2Outfit::HANDLER_ARRAY, 8, PAGE_READWRITE, &oldProtect)) {
            handlers[0] = this->originalHandlerOrder[0];
            handlers[1] = this->originalHandlerOrder[1];
            VirtualProtect((LPVOID)Rayne2Outfit::HANDLER_ARRAY, 8, oldProtect, &oldProtect);
        }

        this->handlersSwapped = false;
        DEBUG_LOG("[Outfit] Loose file priority disabled (handlers restored)");
    }

    // ========================================================================
    // Folder Parsing Helpers
    // ========================================================================

    int detectBaseOutfit(const std::string& folderPath) {
        namespace fs = std::filesystem;

        // Check for specific files that indicate which base outfit to extend
        // These patterns match common mod naming conventions

        auto hasFile = [&](const std::string& subPath, const std::string& filename) {
            return fs::exists(folderPath + "\\" + subPath + "\\" + filename);
            };

        // Check MODELS folder for character model
        if (hasFile("MODELS", "RAYNE_SCHOOLGIRL.BFM") || hasFile("MODELS", "rayne_schoolgirl.bfm")) return 9;
        if (hasFile("MODELS", "RAYNE_ARMOR.BFM") || hasFile("MODELS", "rayne_armor.bfm")) return 8;
        if (hasFile("MODELS", "RAYNE_COWGIRL.BFM") || hasFile("MODELS", "rayne_cowgirl.bfm")) return 7;
        if (hasFile("MODELS", "RAYNE_DARK.BFM") || hasFile("MODELS", "rayne_dark.bfm")) return 6;
        if (hasFile("MODELS", "RAYNE_DRESS.BFM") || hasFile("MODELS", "rayne_dress.bfm")) return 1;

        // Check ART folder for textures
        if (hasFile("ART", "RAYNE_SCHOOLGIRL.TEX") || hasFile("ART", "rayne_schoolgirl.tex")) return 9;
        if (hasFile("ART", "MANS_METAL_ARMOR.TEX") || hasFile("ART", "mans_metal_armor.tex")) return 8;
        if (hasFile("ART", "RAYNE_COWGIRL.TEX") || hasFile("ART", "rayne_cowgirl.tex")) return 7;
        if (hasFile("ART", "RAYNE_DARK.TEX") || hasFile("ART", "rayne_dark.tex")) return 6;
        if (hasFile("ART", "HRAYNEDRESS.TIF") || hasFile("ART", "hraynedress.tif")) return 1;

        // Default to standard outfit (rayne)
        if (hasFile("MODELS", "RAYNE.BFM") || hasFile("MODELS", "rayne.bfm")) return 0;
        if (hasFile("ART", "RAYNE.TEX") || hasFile("ART", "rayne.tex")) return 0;

        return 0;  // Default to standard outfit
    }

    std::string parseDisplayName(const std::string& folderName) {
        std::string name = folderName;

        // Strip common prefixes like "[BR2L]Outfit Standard - "
        size_t prefixEnd = name.find(']');
        if (prefixEnd != std::string::npos && prefixEnd < name.length() - 1) {
            name = name.substr(prefixEnd + 1);
        }

        // Strip "Outfit X - " pattern
        size_t outfitPos = name.find("Outfit");
        if (outfitPos != std::string::npos) {
            size_t dashPos = name.find(" - ", outfitPos);
            if (dashPos != std::string::npos) {
                name = name.substr(dashPos + 3);
            }
        }

        // Trim whitespace
        size_t start = name.find_first_not_of(" \t");
        size_t end = name.find_last_not_of(" \t");
        if (start != std::string::npos && end != std::string::npos) {
            name = name.substr(start, end - start + 1);
        }

        // Truncate to reasonable length for menu display
        if (name.length() > 60) {
            name = name.substr(0, 60);
        }

        return name;
    }

    std::string generateInternalName(const std::string& displayName, int outfitID) {
        return "custom_" + std::to_string(outfitID);
    }

public:
    Outfit() = default;

    ~Outfit() {
        this->shutdown();
    }

    // ========================================================================
    // Public Interface
    // ========================================================================

    bool initialize() {
        if (this->initialized) {
            DEBUG_LOG("[Outfit] Already initialized");
            return true;
        }

        // Set global instance for static callbacks
        g_outfitInstance = this;

        // Detect game directory
        if (!this->detectGameDirectory()) {
            DEBUG_LOG("[Outfit] Warning: Could not detect game directory");
            // Continue anyway - CreateFileA hook just won't work
        }

        // Reserve space in vector to prevent reallocation
        this->customOutfits.reserve(32);

        this->initialized = true;
        DEBUG_LOG("[Outfit] Initialized successfully");
        return true;
    }

    // Install function hooks - call after WaitForGameReady
    bool installHook() {
        return this->installHooks();
    }

    // Check if handler array is ready - call in WaitForGameReady loop
    void checkSafeToSwapHandlers() {
        this->checkHandlersReady();
    }

    bool isSafeToSwapHandlers() const {
        return this->handlersReady;
    }

    // Enable loose file priority - call after handlers are ready
    bool enableLooseFiles() {
        return this->enableLooseFilePriority();
    }

    void shutdown() {
        if (!this->initialized) return;

        this->uninstallHooks();
        this->disableLooseFilePriority();
        this->customOutfits.clear();
        this->initialized = false;
        this->nextOutfitID = Rayne2Outfit::BASE_OUTFIT_COUNT + 1;
        g_outfitInstance = nullptr;

        DEBUG_LOG("[Outfit] Shutdown complete");
    }

    bool addOutfitFromFolder(const std::string& folderPath) {
        if (!this->initialized) {
            DEBUG_LOG("[Outfit] Cannot add outfit - not initialized");
            return false;
        }

        namespace fs = std::filesystem;

        if (!fs::exists(folderPath) || !fs::is_directory(folderPath)) {
            DEBUG_LOG("[Outfit] Invalid folder path: " << folderPath);
            return false;
        }

        if (this->customOutfits.size() >= 32) {
            DEBUG_LOG("[Outfit] Maximum custom outfit count reached (32)");
            return false;
        }

        std::string folderName = fs::path(folderPath).filename().string();

        OutfitEntry entry;
        entry.outfitID = this->nextOutfitID++;
        entry.baseOutfitIndex = this->detectBaseOutfit(folderPath);
        entry.setModFolderPath(folderPath.c_str());

        std::string displayName = this->parseDisplayName(folderName);
        entry.setDisplayName(displayName.c_str());

        std::string internalName = this->generateInternalName(displayName, entry.outfitID);
        entry.setInternalName(internalName.c_str());

        entry.inheritFromBase();

        this->customOutfits.push_back(entry);

        DEBUG_LOG("[Outfit] Added outfit '" << displayName << "' (ID: " << entry.outfitID
            << ", base: " << entry.baseOutfitIndex << ") from: " << folderPath);

        return true;
    }

    int scanDirectory(const std::string& directoryPath) {
        namespace fs = std::filesystem;

        if (!fs::exists(directoryPath) || !fs::is_directory(directoryPath)) {
            DEBUG_LOG("[Outfit] Invalid directory path: " << directoryPath);
            return 0;
        }

        int count = 0;
        for (const auto& entry : fs::directory_iterator(directoryPath)) {
            if (entry.is_directory()) {
                std::string subPath = entry.path().string();
                if (fs::exists(subPath + "\\ART") || fs::exists(subPath + "\\MODELS")) {
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
    bool isInitialized() const { return this->initialized; }
    bool areHooksInstalled() const { return this->hooksInstalled; }
    bool isLooseFilePriorityEnabled() const { return this->handlersSwapped; }
    bool areHandlersReady() const { return this->handlersReady; }
    size_t getCustomOutfitCount() const { return this->customOutfits.size(); }
    int getTotalOutfitCount() const { return Rayne2Outfit::BASE_OUTFIT_COUNT + (int)this->customOutfits.size(); }
    const std::vector<OutfitEntry>& getCustomOutfits() const { return this->customOutfits; }

    // Check if handlers are ready and enable loose file priority
    // Call this from WaitForGameReady polling loop
    bool checkAndEnableLooseFiles() {
        if (this->handlersSwapped) return true;
        if (!this->checkHandlersReady()) return false;
        return this->enableLooseFilePriority();
    }

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