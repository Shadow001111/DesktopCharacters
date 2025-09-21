#include "CharactersManager.h"

#include <iostream>
#include <windows.h>
#include <algorithm>

// Constructor
CharactersManager::CharactersManager() : shouldExit(false)
{
    // Set default parameters
    defaultParams.width = 400;
    defaultParams.height = 300;
    defaultParams.title = L"Character";
    defaultParams.topMost = true;
    defaultParams.frameless = true;
    defaultParams.fullscreen = false;
    defaultParams.ignoreMouse = false;
}

// Destructor - automatically closes all characters
CharactersManager::~CharactersManager()
{
    closeAllCharacters();
}

// Set default parameters for all new characters
void CharactersManager::setDefaultParams(const WindowParams& params)
{
    defaultParams = params;
}

// Add a new character window (creates it internally)
bool CharactersManager::addCharacter()
{
    auto character = std::make_unique<WindowsWindow>();

    if (!character->createWindow(defaultParams))
    {
        std::cout << "Failed to create character!" << std::endl;
        return false;
    }

    characters.push_back(std::move(character));
    std::cout << "Created character #" << characters.size() << std::endl;
    return true;
}

// Close all characters
void CharactersManager::closeAllCharacters()
{
    characters.clear(); // This will call destructors of all windows
}

// Remove closed/invalid characters
void CharactersManager::removeClosedCharacters()
{
    auto it = std::remove_if(characters.begin(), characters.end(),
        [](const std::unique_ptr<BaseWindow>& character) {
            // Cast to WindowsWindow to check validity
            WindowsWindow* winChar = static_cast<WindowsWindow*>(character.get());
            return !winChar->isValid();
        });

    if (it != characters.end())
    {
        size_t removedCount = std::distance(it, characters.end());
        characters.erase(it, characters.end());
        std::cout << "Removed " << removedCount << " closed character(s). Remaining: " << characters.size() << std::endl;
    }
}

// Get the number of characters
size_t CharactersManager::getCharacterCount() const
{
    return characters.size();
}

// Check for exit key combination (Ctrl + Shift + Q)
bool CharactersManager::checkExitKeys()
{
    return (GetAsyncKeyState(VK_CONTROL) & 0x8000) &&
        (GetAsyncKeyState(VK_SHIFT) & 0x8000) &&
        (GetAsyncKeyState('Q') & 0x8000);
}

// Run the main message loop for all characters
int CharactersManager::runLoop()
{
    if (characters.empty())
    {
        std::cout << "No characters to run!" << std::endl;
        return 0;
    }

    std::cout << "Program is running. Press Ctrl+Shift+Q to exit." << std::endl;

    MSG msg;
    while (!shouldExit)
    {
        // Check for messages
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        // Check for exit key combination
        if (checkExitKeys())
        {
            std::cout << "Exit key combination detected. Closing..." << std::endl;
            shouldExit = true;
            break;
        }

        // Remove any closed characters
        removeClosedCharacters();

        // Small delay to prevent excessive CPU usage
        Sleep(10);
    }

    return 0;
}