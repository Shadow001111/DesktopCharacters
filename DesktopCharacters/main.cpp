#include "Window/WindowsWindow.h"
#include "CharactersManager.h"

#include <iostream>
#include <fstream>
#include <memory>

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
#endif

    // Create the characters manager
    CharactersManager manager;

    // Create multiple characters with the same parameters
    if (!manager.addCharacter())
    {
        return -1;
    }

    if (!manager.addCharacter())
    {
        return -1;
    }

    if (!manager.addCharacter())
    {
        return -1;
    }

    // Run the message loop - manager will handle all windows
    int result = manager.runLoop();

    return result;
}