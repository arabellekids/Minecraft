#include "baseObject.h"
#include "player.h"

BaseObject::BaseObject() {}
BaseObject::~BaseObject() {}

void BaseObject::Update(const Player& player) {}
void BaseObject::RenderSolid(const glm::mat4& vp) {}