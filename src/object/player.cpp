#include <iostream>

#include "../app/app.h"
#include "../input/input.h"
#include "player.h"
#include "../ray/ray.h"
#include "../settings/settings.h"

void Player::OnKeyDown(SDL_Scancode key) {}

void Player::OnMouseButtonUp(Uint8 button)
{
    // break blocks
    if(button == MOUSE_LEFT)
    {
        glm::vec3 pos(m_pos.x + (Settings::viewDist + 0.5f)*CHUNK_SIZE_X, m_pos.y, m_pos.z + (Settings::viewDist + 0.5f)*CHUNK_SIZE_Z);
        glm::vec3 dir(m_model[2][0], m_model[2][1], m_model[2][2]);

        Ray ray( pos, dir, 10 );
        BlockHitInfo info;
        if(App::Instance().GetWorld().Raycast(ray, info, false))
        {
            App::Instance().GetWorld().SetBlock(info.pos.x, info.pos.y, info.pos.z, BLOCK_AIR);
        }
    }

    // build blocks
    else if(button == MOUSE_RIGHT - 1) // 3 = mouse right, but input needs a button mask so its MOUSE_RIGHT = 4
    {
        glm::vec3 pos(m_pos.x + (Settings::viewDist + 0.5f)*CHUNK_SIZE_X, m_pos.y, m_pos.z + (Settings::viewDist + 0.5f)*CHUNK_SIZE_Z);
        glm::vec3 dir(m_model[2][0], m_model[2][1], m_model[2][2]);

        Ray ray( pos, dir, 10 );
        BlockHitInfo info;
        if(App::Instance().GetWorld().Raycast(ray, info))
        {
            glm::ivec3 neighbors[6] = {
                { 0,  0,  1},
                { 0,  0, -1},
                {-1,  0,  0},
                { 1,  0,  0},
                { 0,  1,  0},
                { 0, -1,  0}
            };
            App::Instance().GetWorld().SetBlock(info.pos.x + neighbors[info.side].x,
            info.pos.y + neighbors[info.side].y,
            info.pos.z + neighbors[info.side].z,
            BLOCK_STONE);
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