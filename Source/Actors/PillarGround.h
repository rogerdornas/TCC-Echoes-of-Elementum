//
// Created by roger on 05/12/2025.
//

#pragma once

#include "Actor.h"
#include "DynamicGround.h"

class PillarGround  : public DynamicGround
{
public:
    PillarGround(Game* game, float width = 100, float height = 0);
    void OnUpdate(float deltaTime) override;

    void ResolveGroundCollision();

    void ManageAnimations();

private:
    float mLifeDuration;
    float mLifeTimer;
};
