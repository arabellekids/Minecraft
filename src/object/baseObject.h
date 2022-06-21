#pragma once

#include <glm/mat4x4.hpp>

class Player;

class BaseObject
{
public:
    BaseObject();
    virtual ~BaseObject();
    
    virtual void Update(const Player& player);
    virtual void RenderSolid(const glm::mat4& vp);
};