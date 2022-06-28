#include "../input/input.h"
#include "player.h"

void Player::OnKeyUp(SDL_Scancode key)
{

}
void Player::OnKeyDown(SDL_Scancode key)
{
    
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