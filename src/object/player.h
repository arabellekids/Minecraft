#pragma once

#include <glm/mat4x4.hpp>
#include <SDL2/SDL_scancode.h>

#include "gameObject.h"

#include "../vbo/vbo.h"
#include "../vao/vao.h"
#include "../ibo/ibo.h"
#include "../shader/shader.h"
#include "../texture/texture.h"

class Player : public GameObject
{
private:
    Shader m_outlineShader;
    Texture m_outlineTex;

    Vbo m_outlineCubeVb;
    Vao m_outlineCubeVa;
    Ibo m_outlineCubeIb;
    
    int m_curBlock;
public:
    Player();
    ~Player();

    void Update();
    void Draw(const glm::mat4& vp);

    void OnKeyDown(SDL_Scancode key);
    void OnMouseButtonUp(Uint8 button);
};