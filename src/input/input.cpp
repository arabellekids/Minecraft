#include "input.h"

Input* Input::s_pInstance = NULL;

Input::Input()
{
    m_mousePos.x = 0;
    m_mousePos.y = 0;
    
    m_mouseDelta.x = 0;
    m_mouseDelta.y = 0;
}

bool Input::GetKey(const SDL_Scancode key) const
{
    return SDL_GetKeyboardState(nullptr)[key];
}

const glm::vec2& Input::GetMouse() const
{
    int x, y;
    SDL_GetMouseState(&x, &y);
    
    m_mousePos.x = x;
    m_mousePos.y = y;

    return m_mousePos;
}

const glm::vec2& Input::GetMouseDelta() const
{
    int x, y;
    SDL_GetRelativeMouseState(&x, &y);
    
    m_mouseDelta.x = x;
    m_mouseDelta.y = y;

    return m_mouseDelta;
}

bool Input::GetMouseButton(int buttonNum) const
{
    return (SDL_GetMouseState(nullptr, nullptr) & buttonNum) > 0;
}
