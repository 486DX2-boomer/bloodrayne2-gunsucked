// The launcher should load rayne2.exe and inject the mod DLL after the process has started.
#include <iostream>
#include <Windows.h>

int main()
{
    std::cout << "this should start rayne2.exe\n";
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    // find the path to rayne2.exe
    // lpcstr exepath = etc etc

    if (!CreateProcess(
        L"G:\\GOG\\BloodRayne 2 Terminal Cut\\rayne2.exe", // will a relative path work here?
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
        std::cout << GetLastError();
    }
    
    std::cout << "process id: " << pi.hProcess << std::endl;
    auto rayne2Handle = pi.hProcess;

    LPCSTR dllPath = "G:\\BR2 Modding\\myBR2mod\\Debug\\myBR2mod.dll";

    if (rayne2Handle && rayne2Handle != INVALID_HANDLE_VALUE) {

        void* location = VirtualAllocEx(rayne2Handle, 0, MAX_PATH, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

        if (location) { // ensure nonzero
            WriteProcessMemory(rayne2Handle, location, dllPath, strlen(dllPath) + 1, 0);
        }

        HANDLE hThread = CreateRemoteThread(rayne2Handle, 0, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, location, 0, 0);

        if (hThread) {
            CloseHandle(hThread);
        }

        if (rayne2Handle) {
            CloseHandle(rayne2Handle);
        }
    }
    else {
        std::cout << "Something went wrong." << std::endl;
    }

    // I don't know what these do.
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
