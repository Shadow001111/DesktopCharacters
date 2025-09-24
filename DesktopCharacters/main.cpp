#include "CharactersManager.h"

#include <iostream>
#include <fstream>
#include <memory>

#include <locale>
#include <codecvt>

void openConsole()
{
    // Create a new console
    AllocConsole();

    // Redirect stdout, stdin, stderr to the new console
    FILE* fp;
    freopen_s(&fp, "CONOUT$", "w", stdout);
    freopen_s(&fp, "CONOUT$", "w", stderr);
    freopen_s(&fp, "CONIN$", "r", stdin);

    std::cout << "Debug console is ready!" << std::endl;
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
#ifdef _DEBUG
    openConsole();
    SetConsoleOutputCP(CP_UTF8);
#endif

    // Create the characters manager
    CharactersManager manager;
    if (!manager.initialize())
    {
        return -1;
    }

    // Create characters
    if (!manager.addCharacter({-2, 0}, {1, 0}))
    {
        return -1;
    }

    // Run the message loop - manager will handle all windows
    int result = manager.runLoop();

    return result;
}

// TODO: MainWindow prevents taskbar from opening. After a Windows key gets pressed, everything works.