#pragma once

#include <SDL2/SDL_scancode.h>
#include "gameObject.h"

class App;

class Player : public GameObject
{
public:
    void Update();
    void OnKeyDown(SDL_Scancode key);
    void OnKeyUp(SDL_Scancode key);
};