#include "CharactersManager.h"

#include "Core/Random.h"

#include <iostream>

static void openConsole()
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
    Character::Data charData;

    charData.maxSpeed = 1.5f;
    charData.maxJumpVelocity = 1.0f;

    charData.collisionElasticitySides = 0.2f;
    charData.collisionElasticityRoof = 0.2f;
    charData.collisionElasticityFloor = 0.0f;

    charData.frictionFloor = 0.4f;

    for (int i = 0; i < 1; i++)
    {
        float x = Random::Float(-1.0f, 1.0f);
        float y = Random::Float(-1.0f, 1.0f);
        float vx = Random::Float(-1.0f, 1.0f);
        float vy = Random::Float(-1.0f, 1.0f);

        if (!manager.addCharacter({ x, y }, { vx, vy }, charData))
        {
            return -1;
        }
    }

    // Run the message loop - manager will handle all windows
    int result = manager.runLoop();

    return result;
}

// TODO: MainWindow prevents taskbar from opening.
// TODO: Can drag character out of screen
// TODO: Make fixed updates