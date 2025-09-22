#include "Character.h"

#include "Window/WindowsWindow.h"
using WindowClass = WindowsWindow;

#include <iostream>

PlatformInterface* Character::platform = nullptr;
std::vector<WindowData> Character::windowsData;

Vec2 Character::worldSize;   // The logical world size for physics
Vec2 Character::screenSize;


Character::Character()
    : position(), size(), velocity(), alive(false)
{
}

Character::~Character()
{
    destroy();
}

bool Character::create(const Vec2& position, const Vec2& size)
{
    if (window != nullptr)
    {
        std::cout << "Character is already created!" << std::endl;
        return false;
    }

    // Create the window
    InitWindowParams params;
    params.width = 200; // TODO: Update size
    params.height = 300;
    params.title = L"Character";
    params.topMost = true;
    params.frameless = true;

    window = std::make_unique<WindowClass>();
    if (!window->createWindow(params))
    {
        std::cout << "Failed to create character window!" << std::endl;
        window.reset();
        return false;
    }

    // Hook Character’s callback
    window->setCallback([this](const WindowEvent& evt) {
        this->onWindowEvent(evt);
        });

    // Set character properties
    this->position = position;
    this->size = size;
    velocity = {};

    alive = true;
    return true;
}

void Character::onWindowEvent(const WindowEvent& evt)
{
    switch (evt.type)
    {
    case WindowEventType::MouseDown:
        int x, y;
        platform->getGlobalMousePosition(x, y);
        startDrag(Vec2(x, y));
        break;

    case WindowEventType::MouseUp:
        break;

    case WindowEventType::Close:
        destroy();
        break;

    default:
        break;
    }
}

void Character::setPosition(float x, float y)
{
    setPosition(Vec2(x, y));
}

void Character::setPosition(const Vec2& newPosition)
{
    if (!alive) return;
    position = newPosition;
}

void Character::setSize(float width, float height)
{
    setSize(Vec2(width, height));
}

void Character::setSize(const Vec2& newSize)
{
    if (!alive) return;
    size = newSize;
}

void Character::setVelocity(float vx, float vy)
{
    setVelocity(Vec2(vx, vy));
}

void Character::setVelocity(const Vec2& newVelocity)
{
    velocity = newVelocity;
}

void Character::move(float deltaX, float deltaY)
{
    move(Vec2(deltaX, deltaY));
}

void Character::move(const Vec2& delta)
{
    setPosition(position + delta);
}

const Vec2& Character::getPosition()
{
    return position;
}

const Vec2& Character::getSize()
{
    return size;
}

const Vec2& Character::getVelocity()
{
    return velocity;
}

void Character::update(float deltaTime)
{
    if (!alive || !window) return;
    
    // Dragging
    if (dragging)
    {
        if (platform->getMouseButtonPressed(MouseButton::Left))
        {
            velocity = Vec2();

            int mouseX, mouseY;
            platform->getGlobalMousePosition(mouseX, mouseY);

            Vec2 mousePosInWorldSpace = screenToWorld(Vec2(mouseX, mouseY));

            position = mousePosInWorldSpace + dragOffset;

            // Record history
            dragHistory.push_back({ position, deltaTime });
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
        }
        else
        {
            dragging = false;

            if (!dragHistory.empty())
            {
                Vec2 deltaPos = dragHistory.back().position - dragHistory.front().position;
                float totalTime = 0.0f;
                for (auto& s : dragHistory) totalTime += s.time;

                if (totalTime > 0.0f)
                    velocity = deltaPos / totalTime;
            }
            dragHistory.clear();
        }
    }
    
    // Kinematics
    if (!dragging)
    {
        Vec2 gravity(0.0f, -10.0f);

        velocity += gravity * deltaTime;

        position += velocity * deltaTime;

        if (position.x < -(worldSize.x - size.x * 0.5f))
        {
            position.x = -(worldSize.x - size.x * 0.5f);
            velocity.x *= -0.9f;
        }
        else if (position.x > (worldSize.x - size.x * 0.5f))
        {
            position.x = (worldSize.x - size.x * 0.5f);
            velocity.x *= -0.9f;
        }

        if (position.y < -(worldSize.y - size.y * 0.5f))
        {
            position.y = -(worldSize.y - size.y * 0.5f);
            velocity.y *= -0.9f;
        }
        else if (position.y > (worldSize.y - size.y * 0.5f))
        {
            position.y = (worldSize.y - size.y * 0.5f);
            velocity.y *= -0.9f;
        }
    }

    updateWindowTransform();
}

bool Character::isAlive() const
{
    if (!alive || !window) return false;

    // Check if underlying window is still valid
    WindowClass* winWindow = static_cast<WindowClass*>(window.get());
    return winWindow && winWindow->isValid();
}

void Character::destroy()
{
    if (window)
    {
        window.reset();
        std::cout << "Character destroyed" << std::endl;
    }
    alive = false;
}

BaseWindow* Character::getWindow() const
{
    return window.get();
}

void Character::startDrag(const Vec2& mousePos)
{
    if (!alive) return;

    dragging = true;
    dragOffset = position - screenToWorld(mousePos);

    dragHistory.clear();
    dragHistory.push_back({ position, 0.0f });
}

bool Character::isDragging() const
{
    return dragging;
}

void Character::updateWindowTransform() const
{
    if (!alive || !window) return;

    if (window->isValid())
    {
        // Convert world space to screen space
        Vec2 charMin = position - size * 0.5f;
        Vec2 charMax = position + size * 0.5f;

        Vec2 screen1 = worldToScreen(charMin);
        Vec2 screen2 = worldToScreen(charMax);

        int screenX = (int)screen1.x;
        int screenY = (int)screen2.y;
        int screenW = (int)(screen2 - screen1).x;
        int screenH = (int)(screen1 - screen2).y;

        window->setPositionAndSize(screenX, screenY, screenW, screenH);
    }
}

Vec2 Character::screenToWorld(const Vec2& screen) const
{
    Vec2 screenNormalized = screen / screenSize;
    Vec2 normalized = screenNormalized * 2.0f - 1.0f;
    normalized.y = -normalized.y;
    Vec2 world = normalized * worldSize;
    return world;
}

Vec2 Character::worldToScreen(const Vec2& world) const
{
    Vec2 normalized = world / worldSize;
    normalized.y = -normalized.y;
    Vec2 screenNormalized = (normalized + 1.0f) * 0.5f;
    Vec2 screen = screenNormalized * screenSize;
    return screen;
}
