#pragma once
#include "Window/WindowsWindow.h"

#include <vector>
#include <memory>

class CharactersManager
{
public:
    CharactersManager();
    ~CharactersManager();

    // Set default parameters for all new characters
    void setDefaultParams(const WindowParams& params);

    // Add a new character window (creates it internally)
    bool addCharacter();

    // Close all characters
    void closeAllCharacters();

    // Remove closed/invalid characters
    void removeClosedCharacters();

    // Get the number of characters
    size_t getCharacterCount() const;

    // Run the main message loop for all characters
    int runLoop();

private:
    std::vector<std::unique_ptr<BaseWindow>> characters;
    WindowParams defaultParams;
    bool shouldExit;

    // Check for exit key combination
    bool checkExitKeys();
};