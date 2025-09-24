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

    int scrW = 0, scrH = 0;
    platformInterface->getScreenResolution(scrW, scrH);
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
    params.fullscreen = true;
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

// Add a new character
bool CharactersManager::addCharacter(const Vec2& position, const Vec2& velocity)
{
    Vec2 size(1.0f, 1.0f);

    auto character = std::make_unique<Character>(position, size);
    character->setVelocity(velocity);
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
        const AABB& aabb = character->getAABB();

        Vec2 topLeft = worldToScreen({ aabb.minX, aabb.maxY });
        Vec2 bottomRight = worldToScreen({ aabb.maxX, aabb.minY });

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
    if (evt.type == WindowEventType::LeftMouseDown)
    {
        Vec2 mousePos = screenToWorld({ evt.localMouseX, evt.localMouseY });
        interactLeftMouse(mousePos);
    }
}

void CharactersManager::interactLeftMouse(const Vec2& mousePos)
{
    for (const auto& character : characters)
    {
        if (!character->getAABB().isContaining(mousePos))
        {
            continue;
        }

        character->isBeingDragged = true;
        const Vec2& position = character->getPosition();

        draggedCharacter = character.get();
        dragOffset = position - mousePos;

        dragHistory.clear();
        dragHistory.push_back({ position, 0.0f });

        break;
    }
}

void CharactersManager::updateDragging(float deltaTime)
{
    if (draggedCharacter == nullptr)
    {
        return;
    }

    if (platformInterface->getMouseButtonPressed(MouseButton::Left))
    {
        int mouseX, mouseY;
        platformInterface->getGlobalMousePosition(mouseX, mouseY);
        Vec2 mousePosition = screenToWorld({ mouseX, mouseY });

        dragHistory.push_back({ mousePosition, deltaTime });
        float totalTime = 0.0f;
        for (int i = (int)dragHistory.size() - 1; i >= 0; --i)
        {
            totalTime += dragHistory[i].time;
            if (totalTime > dragHistoryDuration)
            {
                dragHistory.erase(dragHistory.begin(), dragHistory.begin() + i);
                break;
            }
        }

        draggedCharacter->setPosition(mousePosition + dragOffset);
    }
    else
    {
        if (!dragHistory.empty())
        {
            Vec2 deltaPos = dragHistory.back().position - dragHistory.front().position;
            float totalTime = 0.0f;
            for (auto& s : dragHistory) totalTime += s.time;

            if (totalTime > 0.0f)
            {
                draggedCharacter->setVelocity(deltaPos / totalTime);
            }
        }
        dragHistory.clear();

        draggedCharacter->isBeingDragged = false;
        draggedCharacter = nullptr;
    }
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

        // Updates
        updateDragging(deltaTime);
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