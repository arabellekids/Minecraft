#include <GLES3/gl32.h>
#include <glm/ext/matrix_transform.hpp>
#include <iostream>

#include "../app/app.h"
#include "../input/input.h"
#include "player.h"
#include "../ray/ray.h"
#include "../settings/settings.h"

#include "../block/block.h"

Player::Player() : m_outlineShader("assets/shaders/outline/vert.glsl", "assets/shaders/outline/frag.glsl"), m_outlineTex("assets/textures/outline.png", GL_NEAREST)
{
    m_curBlock = 1;

    m_outlineShader.SetUniform1f("u_tex", 0);

    auto verts = GetCubeVertices();
    auto& indices = GetCubeIndices();

    for(int i = 0; i < verts.size(); ++i)
    {
        verts[i].x *= 1.01;
        verts[i].y *= 1.01;
        verts[i].z *= 1.01;
    }

    m_outlineCubeVb.StaticBufferData(verts);
    m_outlineCubeIb.StaticBufferData(indices);

    BufferLayout layout;
    layout.Push<float>(3); // x,y,z
    layout.Push<float>(2); // u,v
    layout.Push<float>(1); // light
    
    m_outlineCubeVa.AddBuffer(m_outlineCubeVb, layout);
}

Player::~Player() {}

void Player::OnKeyDown(SDL_Scancode key)
{
    if(key == SDL_SCANCODE_M)
    {
        SDL_ShowCursor(SDL_DISABLE);
        SDL_SetRelativeMouseMode(SDL_TRUE);
    }
    else if (key == SDL_SCANCODE_N)
    {
        SDL_ShowCursor(SDL_ENABLE);
        SDL_SetRelativeMouseMode(SDL_FALSE);
    }
    else if(key == SDL_SCANCODE_UP)
    {
        m_curBlock = (m_curBlock + 1) % GetBlockPalette().size();
    }
    else if(key == SDL_SCANCODE_DOWN)
    {
        m_curBlock = (m_curBlock - 1) % GetBlockPalette().size();
    }
}

void Player::OnMouseButtonUp(Uint8 button)
{
    // break blocks
    if(button == MOUSE_LEFT)
    {
        glm::vec3 dir(m_model[2][0], m_model[2][1], m_model[2][2]);

        Ray ray( m_pos, dir, 10 );
        BlockHitInfo info;
        if(App::Instance().GetWorld().Raycast(ray, info))
        {
            App::Instance().GetWorld().SetBlockFromPos(info.pos.x, info.pos.y, info.pos.z, BLOCK_AIR);
        }
    }

    // build blocks
    else if(button == MOUSE_RIGHT - 1) // 3 = mouse right, but input needs a button mask so its MOUSE_RIGHT = 4
    {
        glm::vec3 dir(m_model[2][0], m_model[2][1], m_model[2][2]);

        Ray ray( m_pos, dir, 10 );
        BlockHitInfo info;
        if(App::Instance().GetWorld().Raycast(ray, info))
        {
            App::Instance().GetWorld().SetBlockFromPos(info.neighbor.x, info.neighbor.y, info.neighbor.z, m_curBlock);
        }
    }
}

void Player::Update()
{
    if(Input::Instance().GetKey(SDL_SCANCODE_A))
    {
        glm::vec3 right = {
            m_model[0][0],
            0.0f,
            m_model[0][2],
        };
        m_pos -= right * 0.2f;
    }
    if(Input::Instance().GetKey(SDL_SCANCODE_D))
    {
        glm::vec3 right = {
            m_model[0][0],
            0.0f,
            m_model[0][2],
        };
        m_pos += right * 0.2f;
    }
    if(Input::Instance().GetKey(SDL_SCANCODE_W))
    {
        glm::vec3 forward = {
            m_model[2][0],
            0.0f,
            m_model[2][2],
        };
        m_pos += glm::normalize(forward) * 0.2f;
    }
    if(Input::Instance().GetKey(SDL_SCANCODE_S))
    {
        glm::vec3 forward = {
            m_model[2][0],
            0.0f,
            m_model[2][2],
        };
        m_pos -= glm::normalize(forward) * 0.2f;
    }
    if(Input::Instance().GetKey(SDL_SCANCODE_SPACE))
    {
        m_pos.y += 0.2f;
    }
    if(Input::Instance().GetKey(SDL_SCANCODE_LSHIFT))
    {
        m_pos.y -= 0.2f;
    }

    auto& delta = Input::Instance().GetMouseDelta();

    m_rot.x += delta.y * 0.2f;
    if(m_rot.x < -89.0f) { m_rot.x = -89.0f; }
    if(m_rot.x >  89.0f) { m_rot.x =  89.0f; }
    
    m_rot.y += delta.x * 0.2f;
}

void Player::Draw(const glm::mat4& vp)
{
    glm::vec3 pos(m_pos.x + (Settings::viewDist + 0.5f)*CHUNK_SIZE_X, m_pos.y, m_pos.z + (Settings::viewDist + 0.5f)*CHUNK_SIZE_Z);
    glm::vec3 dir(m_model[2][0], m_model[2][1], m_model[2][2]);

    Ray ray( m_pos, dir, 10 );
    BlockHitInfo info;
    if(App::Instance().GetWorld().Raycast(ray, info))
    {
        glm::mat4 model = glm::translate(glm::identity<glm::mat4>(), {info.pos.x + 0.5f, info.pos.y + 0.5f, info.pos.z + 0.5f});

        m_outlineShader.Bind();
        m_outlineTex.Bind();
        m_outlineShader.SetUniformMat4("u_mvp", vp * model);

        m_outlineCubeVa.Bind();
        m_outlineCubeIb.Bind();
        
        glDrawElements(GL_TRIANGLES, m_outlineCubeIb.GetData().size(), GL_UNSIGNED_SHORT, nullptr);
    }
}