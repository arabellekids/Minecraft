#pragma once

class BaseObject
{
public:
    BaseObject();
    virtual ~BaseObject();
    
    virtual void Update();
    virtual void RenderSolid();
};