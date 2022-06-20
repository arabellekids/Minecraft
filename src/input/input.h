#pragma once

#include <glm/vec2.hpp>
#include <SDL2/SDL.h>

enum MouseButtons
{
    MOUSE_LEFT = SDL_BUTTON(1),
    MOUSE_MIDDLE = SDL_BUTTON(2),
    MOUSE_RIGHT = SDL_BUTTON(3)
};

class Input
{
public:
    static Input& Instance()
    {
        if(s_pInstance == NULL)
        {
            s_pInstance = new Input();
        }
        return *s_pInstance;
    }

    // Mouse input ////////////////

    // Returns the mouse button state
    bool GetMouseButton(int buttonNum) const;
    
    // Returns the mouse position
    const glm::vec2& GetMouse() const;
    // Returns the mouse delta position ( or its movement since this was last called )
    const glm::vec2& GetMouseDelta() const;
    
    // Keyboard input ////////////////

    // Returns if the given key is down
    bool GetKey(const SDL_Scancode key) const;

private:
    Input();
    ~Input() {}

    mutable glm::vec2 m_mousePos;
    mutable glm::vec2 m_mouseDelta;

    static Input* s_pInstance;
};
