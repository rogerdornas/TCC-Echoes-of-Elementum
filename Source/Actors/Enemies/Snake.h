//
// Created by roger on 07/10/2025.
//

#pragma once

#include "Enemy.h"

class Snake : public Enemy
{
public:
    Snake(Game* game);
    void OnUpdate(float deltaTime) override;

private:
    enum class State
    {
        WalkForward,
        Stop,
        Attack,
        Hurt
    };

    void ReceiveHit(float damage, Vector2 knockBackDirection, bool knockBack = true) override;

    void MovementAfterPlayerSpotted(float deltaTime);
    void MovementBeforePlayerSpotted();

    void WalkForward(float deltaTime);
    void Stop(float deltaTime);
    void Attack(float deltaTime);
    void Hurt(float deltaTime);

    void ManageAnimations();
    void ManageCombatBox();

    State mSnakeState;

    float mStopDuration;
    float mStopTimer;

    float mDistToSpotPlayer;
    float mWalkingAroundTimer;
    float mWalkingAroundDuration;
    float mWalkingAroundMoveSpeed;

    float mGravity;

    float mDistToAttack;
    float mAttackDuration;
    float mAttackTimer;
    float mIdleWidth;
    float mIdleHeight;
    float mAttackSpriteWidth;
    float mAttackSpriteHeight;
    Vector2 mAttackOffsetHitBox;
};

