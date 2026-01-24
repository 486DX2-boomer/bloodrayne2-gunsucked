// The launcher should load rayne2.exe and inject the mod DLL after the process has started.
#include <iostream>
#include <Windows.h>

#define DEBUG_TESTING_DLL_LOCATION false // if true, launch from absolute path below instead of working directory
#define DEBUG_TESTING_GAME_PATH L"G:\\GOG\\BloodRayne 2 Terminal Cut\\rayne2.exe"
#define DEBUG_LAUNCHER_CONSOLE false // if true, show console window with debug output

// for auto-dismissing message box
extern "C" int WINAPI MessageBoxTimeoutW(HWND hWnd, LPCWSTR lpText, LPCWSTR lpCaption, UINT uType, WORD wLanguageId, DWORD dwMilliseconds);

int main()
{
#if !DEBUG_LAUNCHER_CONSOLE
    FreeConsole();
#endif

#if DEBUG_LAUNCHER_CONSOLE
    std::cout << "starting rayne2.exe\n";
#endif
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    // find the path to rayne2.exe
    // lpcstr exepath = etc etc

    if (!CreateProcess(
#if DEBUG_TESTING_DLL_LOCATION
        DEBUG_TESTING_GAME_PATH, // debug: launch from absolute path
#else
        L"rayne2.exe", // release: relative path, launcher is in game directory
#endif
        NULL, // cmd line args
        NULL, // process attributes
        NULL, // thread attributes
        FALSE, // handle inheritance
        0, // dw creation flags
        NULL, // use parent's environment block (???)
        NULL, // use parent's starting directory (maybe? what is parent?)
        &si, // STARTUPINFO struct
        &pi) // PROCESS_INFORMATION struct
    ) {
        MessageBox(NULL,
            L"Couldn't find rayne2.exe.\n\nMake sure the launcher is in the BloodRayne 2: Terminal Cut game directory.",
            L"GunSucked Mod",
            MB_OK | MB_ICONERROR);
        return 1;
    }
    
#if DEBUG_LAUNCHER_CONSOLE
    std::cout << "process id: " << pi.hProcess << std::endl;
#endif
    auto rayne2Handle = pi.hProcess;

    LPCSTR dllPath = "GunSucked.dll";

    // Check if DLL exists before attempting injection
    if (GetFileAttributesA(dllPath) == INVALID_FILE_ATTRIBUTES) {
        MessageBox(NULL,
            L"Couldn't find GunSucked.dll.\n\nMake sure the DLL is in the BloodRayne 2: Terminal Cut game directory.",
            L"GunSucked Mod",
            MB_OK | MB_ICONERROR);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        return 1;
    }

    void* location = VirtualAllocEx(rayne2Handle, 0, MAX_PATH, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

    if (!location) {
        MessageBox(NULL,
            L"Failed to allocate memory in game process.",
            L"GunSucked Mod",
            MB_OK | MB_ICONERROR);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        return 1;
    }

    if (!WriteProcessMemory(rayne2Handle, location, dllPath, strlen(dllPath) + 1, 0)) {
        MessageBox(NULL,
            L"Failed to write to game process memory.",
            L"GunSucked Mod",
            MB_OK | MB_ICONERROR);
        VirtualFreeEx(rayne2Handle, location, 0, MEM_RELEASE);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        return 1;
    }

    HANDLE hThread = CreateRemoteThread(rayne2Handle, 0, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, location, 0, 0);

    if (!hThread) {
        MessageBox(NULL,
            L"Failed to create remote thread in game process.",
            L"GunSucked Mod",
            MB_OK | MB_ICONERROR);
        VirtualFreeEx(rayne2Handle, location, 0, MEM_RELEASE);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        return 1;
    }

    // Wait for LoadLibraryA to complete and check if it succeeded
    WaitForSingleObject(hThread, INFINITE);

    DWORD exitCode = 0;
    GetExitCodeThread(hThread, &exitCode);

    CloseHandle(hThread);
    VirtualFreeEx(rayne2Handle, location, 0, MEM_RELEASE);

    if (exitCode == 0) {
        MessageBox(NULL,
            L"Failed to load GunSucked.dll into game process.",
            L"GunSucked Mod",
            MB_OK | MB_ICONERROR);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        return 1;
    }

#if DEBUG_LAUNCHER_CONSOLE
    std::cout << "DLL injected successfully" << std::endl;
#endif

    // Show timed success message (auto-dismisses after 3 seconds)
    MessageBoxTimeoutW(NULL,
        L"GunSucked Mod loaded successfully!",
        L"GunSucked Mod",
        MB_OK | MB_ICONINFORMATION,
        0,
        3000);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
