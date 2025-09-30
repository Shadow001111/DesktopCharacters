#include "CharactersManager.h"

#include <iostream>

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

void splitSegment(const Range& a, const Range& b, std::vector<Range>& result)
{
    float left = fmaxf(a.min, b.min);
    float right = fminf(a.max, b.max);

    // If no overlap
    if (left >= right)
    {
        result.push_back({ a.min, a.max });
        return;
    }

    // Left piece
    if (a.min < left)
    {
        result.push_back({ a.min, left });
    }

    // Right piece
    if (right < a.max)
    {
        result.push_back({ right, a.max });
    }
}

void splitObstacleByAABB(Obstacle& obstacle, const AABB& occluder)
{
    std::vector<Range> newSegments;

    Range occluderSegment;
    if (obstacle.type == Obstacle::Type::Horizontal)
    {
        if (obstacle.perpOffset < occluder.minY || obstacle.perpOffset > occluder.maxY)
        {
            return;
        }

        occluderSegment.min = occluder.minX;
        occluderSegment.max = occluder.maxX;
    }
    else
    {
        if (obstacle.perpOffset < occluder.minX || obstacle.perpOffset > occluder.maxX)
        {
            return;
        }

        occluderSegment.min = occluder.minY;
        occluderSegment.max = occluder.maxY;
    }

    for (auto& segment : obstacle.segments)
    {
        splitSegment(segment, occluderSegment, newSegments);
    }

    obstacle.segments = std::move(newSegments);
}


CharactersManager::CharactersManager() : shouldExit(false)
{
}

CharactersManager::~CharactersManager()
{
    characters.clear();
}

bool CharactersManager::initialize()
{
    // Platform
    platformInterface = std::make_unique<PlatformInterfaceClass>();
    platformInterface->start();

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

bool CharactersManager::addCharacter(const Vec2& position, const Vec2& velocity, const Character::Data& charData)
{
    Vec2 size(0.5f, 0.5f);

    auto character = std::make_unique<Character>(position, size, charData);
    character->setVelocity(velocity);
    characters.push_back(std::move(character));

    return true;
}

int CharactersManager::runLoop()
{
    std::cout << "Program is running. Press Ctrl+Shift+Q to exit." << std::endl;
    std::cout << "Click and drag characters to move them around!" << std::endl;

    MSG msg;
    auto lastTime = GetTickCount64();

    float updatesCounter = 0.0f;
    float profilerCounter = 0.0f;

    float updatePeriod = 1.0f / 60.0f;

    while (!shouldExit)
    {
        Profiler::beginFrame();

        // Calculate delta time
        auto currentTime = GetTickCount64();
        float deltaTime = (currentTime - lastTime) * 0.001f; // Convert to seconds
        lastTime = currentTime;

        updatesCounter += deltaTime;
        profilerCounter += deltaTime;

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
        while (updatesCounter > updatePeriod)
        {
            update(updatePeriod);
            updatesCounter -= updatePeriod;
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

        // Profiler
        if (profilerCounter >= 3.0f)
        {
            profilerCounter -= 3.0f;

            Profiler::printProfileReport();
            Profiler::resetAllProfiles();
        }

        Profiler::endFrame();
    }

    return 0;
}


void CharactersManager::collectWindowsData(float deltaTime)
{
    {
        PROFILE_SCOPE("Collect windows data");

        windowsData.clear();
        platformInterface->getWindows(windowsData);
    }
    {
        // TODO: Remove data that is closed window
        PROFILE_SCOPE("Collect in-game windows data");

        std::unordered_map<size_t, size_t> windowMap;
        for (size_t i = 0; i < inGameWindowsData.size(); i++)
        {
            auto key = inGameWindowsData[i].data.id;
            windowMap[key] = i;
        }

        std::vector<InGameWindowData> newInGameWindowsData;
        newInGameWindowsData.reserve(windowsData.size());

        for (auto& newData : windowsData)
        {
            auto key = newData.id;
            auto it = windowMap.find(key);

            InGameWindowData cached;
            cached.data = newData;

            if (it != windowMap.end())
            {
                // Window exists, calculate velocity
                const auto& oldCached = inGameWindowsData[it->second];
                Vec2 oldPos(oldCached.data.x, oldCached.data.y);
                Vec2 newPos(newData.x, newData.y);

                cached.velocity = (newPos - oldPos) / deltaTime;
                cached.lastPosition = newPos;
            }
            else
            {
                // New window
                cached.velocity = Vec2(0.0f, 0.0f);
                cached.lastPosition = Vec2(newData.x, newData.y);
            }

            newInGameWindowsData.push_back(cached);
        }

        inGameWindowsData = std::move(newInGameWindowsData);
    }
}

void CharactersManager::removeContainedWindows()
{
    PROFILE_FUNCTION();

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


void CharactersManager::update(float deltaTime)
{
    // Collect windows data
    collectWindowsData(deltaTime);
    removeContainedWindows();

    // Update obstacles
    updateObstacles();

    // Update dragging
    updateDragging(deltaTime);

    // Update characters
    {
        int mX, mY;
        platformInterface->getGlobalMousePosition(mX, mY);

        Vec2 mouseWorldPosition = screenToWorld(Vec2(mX, mY));

        Character::FollowTarget target;
        target.exist = true;
        target.position = mouseWorldPosition;

        PROFILE_SCOPE("Update characters");
        for (auto& character : characters)
        {
            character->setFollowTarget(target);
            character->update(deltaTime);
        }
    }
}

void CharactersManager::updateObstacles()
{
    PROFILE_FUNCTION();
    auto& obstacles = Character::obstacles;
    obstacles.clear();

    // World
    {
        // Top
        obstacles.emplace_back(Obstacle::Type::Horizontal, Character::worldSize.y, -Character::worldSize.x, Character::worldSize.x);

        // Bottom
        obstacles.emplace_back(Obstacle::Type::Horizontal, -Character::worldSize.y, -Character::worldSize.x, Character::worldSize.x);

        // Left
        obstacles.emplace_back(Obstacle::Type::Vertical, -Character::worldSize.x, -Character::worldSize.y, Character::worldSize.y);

        // Right
        obstacles.emplace_back(Obstacle::Type::Vertical, Character::worldSize.x, -Character::worldSize.y, Character::worldSize.y);
    }

    // Windows
    // TODO: Add constructor and move semantics for Obstacle
    size_t windowsCount = windowsData.size();

    std::vector<AABB> occluders;

    for (size_t i = 0; i < windowsCount; i++)
    {
        const auto& data = windowsData[i];

        Vec2 leftTop_screen = { data.x, data.y };
        Vec2 rectSize_screen = { data.w, data.h };
        Vec2 rightBottom_screen = leftTop_screen + rectSize_screen;

        Vec2 leftTop = screenToWorld(leftTop_screen);
        Vec2 rightBottom = screenToWorld(rightBottom_screen);

        AABB windowAABB = { leftTop.x, rightBottom.y, rightBottom.x, leftTop.y };

        // Create obstacles
        Obstacle top(Obstacle::Type::Horizontal, leftTop.y, leftTop.x, rightBottom.x);
        Obstacle bottom(Obstacle::Type::Horizontal, rightBottom.y, leftTop.x, rightBottom.x);
        Obstacle left(Obstacle::Type::Vertical, leftTop.x, rightBottom.y, leftTop.y);
        Obstacle right(Obstacle::Type::Vertical, rightBottom.x, rightBottom.y, leftTop.y);

        // Split
        for (const auto& occluder : occluders)
        {
            if (occluder.isIntersecting(windowAABB))
            {
                splitObstacleByAABB(top, occluder);
                splitObstacleByAABB(bottom, occluder);
                splitObstacleByAABB(left, occluder);
                splitObstacleByAABB(right, occluder);
            }
        }
        if (!top.segments.empty()) obstacles.push_back(std::move(top));
        if (!bottom.segments.empty()) obstacles.push_back(std::move(bottom));
        if (!left.segments.empty()) obstacles.push_back(std::move(left));
        if (!right.segments.empty()) obstacles.push_back(std::move(right));

        // Add occluder
        occluders.emplace_back(windowAABB);
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



// Check for exit key combination (Ctrl + Shift + Q)
bool CharactersManager::checkExitKeys()
{
    return (GetAsyncKeyState(VK_CONTROL) & 0x8000) &&
        (GetAsyncKeyState(VK_SHIFT) & 0x8000) &&
        (GetAsyncKeyState('Q') & 0x8000);
}

void CharactersManager::onWindowEvent(const WindowEvent& evt)
{
    if (evt.type == WindowEvent::Type::LeftMouseDown)
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

    // Obstacles
    for (const auto& obst : Character::obstacles)
    {
        for (const auto& segment : obst.segments)
        {
            Vec2 p1, p2;

            if (obst.type == Obstacle::Type::Horizontal)
            {
                float obst_min = map(segment.min, -Character::worldSize.x, Character::worldSize.x, 0.0f, screenSize.x);
                float obst_max = map(segment.max, -Character::worldSize.x, Character::worldSize.x, 0.0f, screenSize.x);
                float obst_perp = map(-obst.perpOffset, -Character::worldSize.y, Character::worldSize.y, 0.0f, screenSize.y);

                p1.x = obst_min;
                p1.y = obst_perp;

                p2.x = obst_max;
                p2.y = obst_perp;
            }
            else
            {
                float obst_min = map(-segment.min, -Character::worldSize.y, Character::worldSize.y, 0.0f, screenSize.y);
                float obst_max = map(-segment.max, -Character::worldSize.y, Character::worldSize.y, 0.0f, screenSize.y);
                float obst_perp = map(obst.perpOffset, -Character::worldSize.x, Character::worldSize.x, 0.0f, screenSize.x);

                p1.x = obst_perp;
                p1.y = obst_min;

                p2.x = obst_perp;
                p2.y = obst_max;
            }

            Color color = { 0.0f, 0.0f, 1.0f, 1.0f };

            mainWindow->getRenderer()->drawLine(p1, p2, color, 5.0f);
        }
    }
}


float CharactersManager::map(float value, float min1, float max1, float min2, float max2) const
{
    float normalized = (value - min1) / (max1 - min1);
    return min2 + normalized * (max2 - min2);
}

Vec2 CharactersManager::map(const Vec2& value, const Vec2& min1, const Vec2& max1, const Vec2& min2, const Vec2& max2) const
{
    Vec2 normalized = (value - min1) / (max1 - min1);
    return min2 + normalized * (max2 - min2);
}

Vec2 CharactersManager::screenToWorld(const Vec2& screen) const
{
    Vec2 coord = map(screen, Vec2(), screenSize, -Character::worldSize, Character::worldSize);
    coord.y = -coord.y;
    return coord;
}

Vec2 CharactersManager::worldToScreen(const Vec2& world) const
{
    Vec2 coord = map(world, -Character::worldSize, Character::worldSize, Vec2(), screenSize);
    coord.y = screenSize.y - coord.y;
    return coord;
}
