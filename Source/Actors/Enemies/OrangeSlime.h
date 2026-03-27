//
// Created by roger on 19/02/2026.
//

#pragma once

#include "../Actor.h"
#include "Enemy.h"

class OrangeSlime : public Enemy
{
public:
    enum class State
    {
        Stop,
        IdleJump,
        Run,
        RunJump,
        Attack,
    };

    OrangeSlime(Game* game);
    void OnUpdate(float deltaTime) override;

private:
    void MovementBeforePlayerSpotted(float deltaTime);
    void MovementAfterPlayerSpotted(float deltaTime);

    void Stop(float deltaTime);
    void IdleJump(float deltaTime);
    void Run(float deltaTime);
    void RunJump(float deltaTime);
    void Attack(float deltaTime);

    void ManageAnimations();
    void ManageCombatBox();

    State mEnemyState;

    float mHitDuration;

    float mDistToSpotPlayer;

    float mBeforeSpotPlayerStopDuration;
    float mBeforeSpotPlayerStopTimer;

    bool mAttackNextMove;
    float mAttackProbability;

    float mStopDuration;
    float mStopTimer;

    bool mAlreadyJump;
    float mJumpForce;
    float mJumpStartDuration;
    float mJumpStartTimer;
    float mGravity;

    float mDistToRunJump;

    float mDistToAttack;
    float mAttackDuration;
    float mAttackTimer;
    float mIdleWidth;
    float mIdleHeight;
    float mHorizontalAttackSpriteWidth;
    float mHorizontalAttackSpriteHeight;
    float mVerticalAttackSpriteWidth;
    float mVerticalAttackSpriteHeight;
    Vector2 mHorizontalAttackOffsetHitBox;
    Vector2 mVerticalAttackOffsetHitBox;
};
