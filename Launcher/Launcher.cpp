// Launcher.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <Windows.h>
#include <string>
#include <Psapi.h> // for EnumProcessModules
#include <VersionHelpers.h>
#include <atlstr.h> // for Cstring

bool inject(DWORD ProcessID, LPCSTR dllPath) {
    return false;
}

int main()
{
    std::cout << "this should start rayne2.exe\n";
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    // find the path to rayne2.exe

    if (!CreateProcess(
        L"G:\\GOG\\BloodRayne 2 Terminal Cut\\rayne2.exe",
        NULL, // cmd line args
        NULL, // process handle
        NULL, // thread handle
        FALSE, // handle inheritance
        0, // creation flags
        NULL, // use parent's environment block (???)
        NULL, // use parent's starting directory (maybe? what is parent?)
        &si, // STARTUPINFO struct
        &pi) // PROCESS_INFORMATION struct
    ) {
        return GetLastError();
    }

    // close process and thread handles - what is this for?
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
