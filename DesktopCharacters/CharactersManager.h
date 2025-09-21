#pragma once

#include "Character.h"
#include <vector>
#include <memory>

class CharactersManager
{
public:
    CharactersManager();
    ~CharactersManager();

    // Add a new character (creates it internally)
    bool addCharacter();

    // Close all characters
    void closeAllCharacters();

    // Remove dead characters
    void removeDeadCharacters();

    // Get the number of characters
    size_t getCharacterCount() const;

    // Run the main message loop for all characters
    int runLoop();

private:
    std::vector<std::unique_ptr<Character>> characters;
    bool shouldExit;

    // Check for exit key combination
    bool checkExitKeys();
};