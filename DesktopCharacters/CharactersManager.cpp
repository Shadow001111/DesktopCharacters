#include "CharactersManager.h"
#include <iostream>
#include <windows.h>
#include <algorithm>

// Constructor
CharactersManager::CharactersManager() : shouldExit(false)
{
    
}

// Destructor - automatically closes all characters
CharactersManager::~CharactersManager()
{
    closeAllCharacters();
}

// Add a new character (creates it internally)
bool CharactersManager::addCharacter()
{
    auto character = std::make_unique<Character>();

    WindowParams params;
    params.width = 200;
    params.height = 300;
    params.title = L"Character";
    params.topMost = true;
    //params.frameless = true;

    if (!character->create(params))
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
    characters.clear(); // This will call destructors of all characters
}

// Remove dead characters
void CharactersManager::removeDeadCharacters()
{
    auto it = std::remove_if(characters.begin(), characters.end(),
        [](const std::unique_ptr<Character>& character) {
            return !character->isAlive();
        });

    if (it != characters.end())
    {
        size_t removedCount = std::distance(it, characters.end());
        characters.erase(it, characters.end());
        std::cout << "Removed " << removedCount << " dead character(s). Remaining: " << characters.size() << std::endl;
        if (characters.empty())
        {
            shouldExit = true;
        }
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

        // Remove any dead characters
        removeDeadCharacters();

        // Small delay to prevent excessive CPU usage
        Sleep(10);
    }

    return 0;
}