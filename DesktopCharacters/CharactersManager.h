#pragma once

#include "Character.h"
#include <vector>
#include <memory>

class CharactersManager
{
public:
    CharactersManager();
    ~CharactersManager();

    bool addCharacter();

    void closeAllCharacters();

    void removeDeadCharacters();

    void updateCharacters(float deltaTime);

    size_t getCharacterCount() const;

    int runLoop();
private:
    std::vector<std::unique_ptr<Character>> characters;
    bool shouldExit;

    // Check for exit key combination
    bool checkExitKeys();
};