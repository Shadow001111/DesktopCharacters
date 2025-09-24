#include "CharactersManager.h"

#include <iostream>
#include <algorithm>
#include <sstream>

// Constructor
CharactersManager::CharactersManager() : shouldExit(false)
{
}

// Destructor - automatically closes all characters
CharactersManager::~CharactersManager()
{
    closeAllCharacters();
}

bool CharactersManager::initialize()
{
    // Platform
    platformInterface = std::make_unique<PlatformInterfaceClass>();

    int scrW = 300, scrH = 300;
    //platformInterface->getScreenResolution(scrW, scrH);
    screenSize = Vec2(scrW, scrH);

    Character::worldSize = Vec2((float)scrW / (float)scrH, 1.0f) * 5.0f;

    // Main window
    InitWindowParams params;
    params.width = scrW;
    params.height = scrH;
    params.title = L"DesktopCharacters";
    params.className = L"DesktopCharacters_Shadow001111";
    params.topMost = true;
    params.frameless = true;
    //params.fullscreen = true;
    //params.ignoreMouse = true;
    params.layered = true;

    mainWindow = std::make_unique<WindowClass>();
    if (!mainWindow->createWindow(params))
    {
        std::cout << "Failed to create character window!" << std::endl;
        return false;
    }

    mainWindow->setCallback([this](const WindowEvent& evt) {
        this->onWindowEvent(evt);
        });

    return true;
}

// Add a new character (creates it internally)
bool CharactersManager::addCharacter()
{
    Vec2 position;
    Vec2 size(1.0f, 1.0f);

    auto character = std::make_unique<Character>(position, size);
    characters.push_back(std::move(character));

    return true;
}

// Close all characters
void CharactersManager::closeAllCharacters()
{
    characters.clear(); // This will call destructors of all characters
}

void CharactersManager::updateCharacters(float deltaTime)
{
    windowsData.clear();
    platformInterface->getWindowsDataForCharacters(windowsData);

    for (auto& character : characters)
    {
        character->update(deltaTime);
    }
}

void CharactersManager::renderCharacters()
{
    for (const auto& character : characters)
    {
        const Vec2 position = character->getPosition();
        const Vec2 size = character->getSize();

        Vec2 charMin = position - size * 0.5f;
        Vec2 charMax = position + size * 0.5f;

        Vec2 p1 = worldToScreen(charMin);
        Vec2 p2 = worldToScreen(charMax);

        Vec2 topLeft = Vec2(p1.x, p2.y);
        Vec2 bottomRight = Vec2(p2.x, p1.y);
        Vec2 rectSize = bottomRight - topLeft;

        Color color = { 1.0f, 0.0f, 0.0f, 1.0f };

        mainWindow->getRenderer()->drawRectangle(topLeft, rectSize, color);
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

void CharactersManager::onWindowEvent(const WindowEvent& evt)
{
    
}

// Run the main message loop for all characters
int CharactersManager::runLoop()
{
    std::cout << "Program is running. Press Ctrl+Shift+Q to exit." << std::endl;
    std::cout << "Click and drag characters to move them around!" << std::endl;

    MSG msg;
    DWORD lastTime = GetTickCount64();

    while (!shouldExit)
    {
        // Calculate delta time
        DWORD currentTime = GetTickCount64();
        float deltaTime = (currentTime - lastTime) / 1000.0f; // Convert to seconds
        lastTime = currentTime;

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

        // Update all characters
        updateCharacters(deltaTime);

        // Render all characters
        renderCharacters();

        // Small delay to prevent excessive CPU usage
        Sleep(16); // ~60 FPS
    }

    return 0;
}

Vec2 CharactersManager::screenToWorld(const Vec2& screen) const
{
    Vec2 screenNormalized = screen / screenSize;
    Vec2 normalized = screenNormalized * 2.0f - 1.0f;
    normalized.y = -normalized.y;
    Vec2 world = normalized * Character::worldSize;
    return world;
}

Vec2 CharactersManager::worldToScreen(const Vec2& world) const
{
    Vec2 normalized = world / Character::worldSize;
    normalized.y = -normalized.y;
    Vec2 screenNormalized = (normalized + 1.0f) * 0.5f;
    Vec2 screen = screenNormalized * screenSize;
    return screen;
}