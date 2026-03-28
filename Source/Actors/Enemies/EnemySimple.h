//
// Created by roger on 02/05/2025.
//

#pragma once

#include "../Actor.h"
#include "Enemy.h"

class EnemySimple : public Enemy
{
public:
    EnemySimple(Game* game);
    void OnUpdate(float deltaTime) override;

private:
    void MovementAfterPlayerSpotted();
    void MovementBeforePlayerSpotted();

    void ManageAnimations();

    float mDistToSpotPlayer;
    float mPatrolRadius;
    float mWalkingAroundDuration;
    float mWalkingAroundTimer;
    float mWalkingAroundMoveSpeed;
    float mGravity;
};
