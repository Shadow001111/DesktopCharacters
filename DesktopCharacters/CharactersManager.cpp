#include "CharactersManager.h"

#include <iostream>
#include <algorithm>
#include <sstream>

#include "Core/Profiler.h"

std::wstring getSafeString(const std::wstring& original)
{
    size_t length = original.size();
    std::wstring safe = original;

    for (size_t i = 0; i < length; i++)
    {
        wchar_t ch = original[i];
        if (ch < 32 || ch > 255)
        {
            safe[i] = L'?';
        }
    }
    return safe;
}

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

    Character::worldSize = Vec2((float)scrW / (float)scrH, 1.0f) * 2.5f;

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
    Vec2 size(0.5f, 0.5f);

    auto character = std::make_unique<Character>(position, size);
    character->setVelocity(velocity);
    characters.push_back(std::move(character));

    return true;
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

int CharactersManager::runLoop()
{
    std::cout << "Program is running. Press Ctrl+Shift+Q to exit." << std::endl;
    std::cout << "Click and drag characters to move them around!" << std::endl;

    MSG msg;
    DWORD lastTime = GetTickCount64();

    float elapsed = 0.0f;
    int frames = 0;

    while (!shouldExit)
    {
        Profiler::beginFrame();

        // Calculate delta time
        DWORD currentTime = GetTickCount64();
        float deltaTime = (currentTime - lastTime) * 0.001f; // Convert to seconds
        lastTime = currentTime;

        elapsed += deltaTime;

        // Check for messages
        {
            PROFILE_SCOPE("Poll events");
            while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }

        // Check for exit key combination
        if (checkExitKeys())
        {
            std::cout << "Exit key combination detected. Closing..." << std::endl;
            shouldExit = true;
            break;
        }

        // Updates
        {
            PROFILE_SCOPE("Updates");
            updateDragging(deltaTime);
            update(deltaTime);
        }

        {
            PROFILE_SCOPE("Before render");
            mainWindow->getRenderer()->beforeRender();
        }
        {
            PROFILE_SCOPE("Render");
            render();
        }
        {
            PROFILE_SCOPE("After render");
            mainWindow->getRenderer()->afterRender();
        }
        frames++;

        // Profiler
        if (elapsed >= 3.0f)
        {
            elapsed -= 3.0f;

            //Profiler::printProfileReport();
            Profiler::resetAllProfiles();
        }

        Profiler::endFrame();
    }

    return 0;
}

void CharactersManager::closeAllCharacters()
{
    characters.clear(); // This will call destructors of all characters
}

void CharactersManager::update(float deltaTime)
{
    // Collect windows data
    collectWindowsData();
    removeContainedWindows();

    // Update obstacles
    updateObstacles();

    // Characters
    for (auto& character : characters)
    {
        character->update(deltaTime);
    }

    /*static float elapsed = 9999.0f;
    elapsed += deltaTime;
    if (elapsed >= 2.0f)
    {
        elapsed = 0.0f;

        std::cout << "-----------------------------\n";
        for (const auto& data : windowsData)
        {
            auto safeTitle = getSafeString(data.title);
            auto safeClassName = getSafeString(data.className);
            std::wcout << safeTitle << " | " << safeClassName << L"\n";
        }
        std::cout << "-----------------------------" << std::endl;
    }*/
}

void CharactersManager::collectWindowsData()
{
    windowsData.clear();
    platformInterface->getWindowsDataForCharacters(windowsData);
}

void CharactersManager::removeContainedWindows()
{
    if (windowsData.empty())
    {
        return;
    }

    for (size_t i = 0; i + 1 < windowsData.size(); i++)
    {
        const auto& outer = windowsData[i];
        const auto& inner = windowsData[i + 1];

        bool contains = inner.x >= outer.x && (inner.x + inner.w) <= (outer.x + outer.w) &&
            inner.y >= outer.y && (inner.y + inner.h) <= (outer.y + outer.h);

        if (contains)
        {
            windowsData.erase(windowsData.begin() + i + 1);
        }
    }
}

void CharactersManager::updateObstacles()
{
    auto& obstacles = Character::obstacles;
    obstacles.clear();

    // World
    Obstacle obst;
    obst.cantPassThrough = true;

    {
        // Top
        obst.type = ObstacleType::Horizontal;
        obst.min = -Character::worldSize.x;
        obst.max = Character::worldSize.x;
        obst.perpOffset = Character::worldSize.y;
        obstacles.push_back(obst);

        // Bottom
        obst.perpOffset = -Character::worldSize.y;
        obstacles.push_back(obst);

        // Left
        obst.type = ObstacleType::Vertical;
        obst.min = -Character::worldSize.y;
        obst.max = Character::worldSize.y;
        obst.perpOffset = -Character::worldSize.x;
        obstacles.push_back(obst);

        // Right
        obst.perpOffset = Character::worldSize.x;
        obstacles.push_back(obst);
    }

    // Windows
    obst.cantPassThrough = false;
    for (const auto& data : windowsData)
    {
        Vec2 leftTop_screen = { data.x, data.y };
        Vec2 rectSize_screen = { data.w, data.h };
        Vec2 rightBottom_screen = leftTop_screen + rectSize_screen;

        Vec2 leftTop = screenToWorld(leftTop_screen);
        Vec2 rightBottom = screenToWorld(rightBottom_screen);

        // Top
        obst.type = ObstacleType::Horizontal;
        obst.min = leftTop.x;
        obst.max = rightBottom.x;
        obst.perpOffset = leftTop.y;
        obstacles.push_back(obst);

        // Bottom
        obst.perpOffset = rightBottom.y;
        obstacles.push_back(obst);

        // Left
        obst.type = ObstacleType::Vertical;
        obst.min = rightBottom.y;
        obst.max = leftTop.y;
        obst.perpOffset = leftTop.x;
        obstacles.push_back(obst);

        // Right
        obst.perpOffset = rightBottom.x;
        obstacles.push_back(obst);
    }
}

void CharactersManager::render()
{
    // Characters
    for (const auto& character : characters)
    {
        const AABB& aabb = character->getAABB();

        Vec2 topLeft = worldToScreen({ aabb.minX, aabb.maxY });
        Vec2 bottomRight = worldToScreen({ aabb.maxX, aabb.minY });

        Vec2 rectSize = bottomRight - topLeft;

        Color color = { 1.0f, 0.0f, 0.0f, 1.0f };

        mainWindow->getRenderer()->drawRectangle(topLeft, rectSize, color);
    }

    // Windows
   /* for (const auto& data : windowsData)
    {
        Vec2 topLeft = { data.x, data.y };
        Vec2 rectSize = { data.w, data.h };

        Color color = { 1.0f, 1.0f, 0.0f, 1.0f };

        mainWindow->getRenderer()->drawRectangle(topLeft, rectSize, color, 2.0f);
    }*/

    // Obstacles
    // Get color by obstacle ID
    for (const auto& obst : Character::obstacles)
    {
        Vec2 p1, p2;

        if (obst.type == ObstacleType::Horizontal)
        {
            float obst_min = map(obst.min, -Character::worldSize.x, Character::worldSize.x, 0.0f, screenSize.x);
            float obst_max = map(obst.max, -Character::worldSize.x, Character::worldSize.x, 0.0f, screenSize.x);
            float obst_perp = map(-obst.perpOffset, -Character::worldSize.y, Character::worldSize.y, 0.0f, screenSize.y);

            p1.x = obst_min;
            p1.y = obst_perp;

            p2.x = obst_max;
            p2.y = obst_perp;
        }
        else
        {
            float obst_min = map(-obst.min, -Character::worldSize.y, Character::worldSize.y, 0.0f, screenSize.y);
            float obst_max = map(-obst.max, -Character::worldSize.y, Character::worldSize.y, 0.0f, screenSize.y);
            float obst_perp = map(obst.perpOffset, -Character::worldSize.x, Character::worldSize.x, 0.0f, screenSize.x);

            p1.x = obst_perp;
            p1.y = obst_min;

            p2.x = obst_perp;
            p2.y = obst_max;
        }

        Color color;
        if (obst.cantPassThrough)
        {
            color = { 1.0f, 0.0f, 0.0f, 1.0f };
        }
        else
        {
            color = { 0.0f, 0.0f, 1.0f, 1.0f };
        }

        mainWindow->getRenderer()->drawLine(p1, p2, color, 5.0f);
    }
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

float CharactersManager::map(float value, float min1, float max1, float min2, float max2) const
{
    float normalized = (value - min1) / (max1 - min1);
    return min2 + normalized * (max2 - min2);
}
