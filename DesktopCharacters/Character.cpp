#include "Character.h"

#include "Window/WindowsWindow.h"

#include <iostream>

using WindowClass = WindowsWindow;

// Constructor
Character::Character()
    : position(), size(), velocity(), alive(false)
{
}

// Destructor
Character::~Character()
{
    destroy();
}

// Create the character with a window
bool Character::create(const WindowParams& params)
{
    if (window != nullptr)
    {
        std::cout << "Character is already created!" << std::endl;
        return false;
    }

    // Create the window with provided parameters
    window = std::make_unique<WindowClass>();
    if (!window->createWindow(params))
    {
        std::cout << "Failed to create character window!" << std::endl;
        window.reset();
        return false;
    }

    // Set character properties
    position = {};
    size = { params.width, params.height };
    velocity = {};

    alive = true;
    return true;
}

void Character::setPosition(float x, float y)
{
    setPosition(Vec2(x, y));
}

void Character::setPosition(const Vec2& newPosition)
{
    if (!alive) return;
    position = newPosition;
    updateWindowTransform();
}

void Character::setSize(float width, float height)
{
    setSize(Vec2(width, height));
}

void Character::setSize(const Vec2& newSize)
{
    if (!alive) return;
    size = newSize;
    updateWindowTransform();
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
    if (!alive || !window || dragging) return;

    position += velocity * deltaTime;

    updateWindowTransform();
}

// Check if character is alive
bool Character::isAlive() const
{
    if (!alive || !window) return false;

    // Check if underlying window is still valid
    WindowClass* winWindow = static_cast<WindowClass*>(window.get());
    return winWindow && winWindow->isValid();
}

// Destroy the character
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
    dragOffset = position - mousePos;
    std::cout << "Started dragging character" << std::endl;
}

void Character::updateDrag(const Vec2& mousePos)
{
    if (!alive || !dragging) return;

    setPosition(mousePos + dragOffset);
}

void Character::endDrag()
{
    if (!alive) return;

    dragging = false;
    std::cout << "Stopped dragging character" << std::endl;
}

bool Character::isDragging() const
{
    return dragging;
}

void Character::updateWindowTransform()
{
    if (!alive || !window) return;

    // TODO: Move code below to WindowClass

    WindowClass* winWindow = static_cast<WindowClass*>(window.get());
    if (winWindow && winWindow->isValid())
    {
        HWND hwnd = winWindow->getHWND();
        if (hwnd)
        {
            // Convert world space to screen space (for now, 1:1 mapping)
            int screenX = position.intX();
            int screenY = position.intY();
            int screenW = size.intX();
            int screenH = size.intY();

            SetWindowPos(hwnd, nullptr, screenX, screenY, screenW, screenH,
                SWP_NOZORDER | SWP_NOACTIVATE);
        }
    }
}
