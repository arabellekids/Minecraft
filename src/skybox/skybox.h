#pragma once

#include "../shader/shader.h"
#include "../texture/texture.h"

#include "../ibo/ibo.h"
#include "../vao/vao.h"
#include "../vbo/vbo.h"

class Skybox
{
private:
    Shader m_shader;
    Texture m_texture;

    Ibo m_ib;
    Vao m_va;
    Vbo m_vb;

    void MakeMesh();

    float m_skyTime;
    float m_skyInc;

public:
    Skybox();
    Skybox(const std::string& tex);
    ~Skybox();

    void Draw(const glm::mat4& vp, const glm::vec3& camPos);
    void Update();
};