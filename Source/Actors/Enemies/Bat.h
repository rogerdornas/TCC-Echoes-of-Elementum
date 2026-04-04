//
// Created by roger on 02/04/2026.
//

#pragma once

#include "Enemy.h"

class Bat : public Enemy
{
public:
    Bat(Game* game);
    void OnUpdate(float deltaTime) override;

private:
    enum class State
    {
        Sleep,
        WakeUp,
        Stop,
        Run,
        Attack,
        Bite,
        Recover
    };

    void MovementAfterPlayerSpotted(float deltaTime);

    void Sleep(float deltaTime);
    void WakeUp(float deltaTime);
    void Stop(float deltaTime);
    void Run(float deltaTime);
    void Attack(float deltaTime);
    void Bite(float deltaTime);
    void Recover(float deltaTime);

    void ManageAnimations();
    void ManageCombatBox();

    State mBatState;

    float mDistToSpotPlayer;

    float mStopDuration;
    float mStopTimer;

    float mHitDuration;
    float mHitTimer;

    float mWakeUpDuration;
    float mWakeUpTimer;

    float mDistToAttack;
    bool mAttackDirectionRight;

    float mAttackDuration;
    float mAttackTimer;
    float mIdleWidth;
    float mAttackSpriteWidth;

    float mBiteDuration;
    float mBiteTimer;
    float mDistToBiteY;

    float mRecoverDuration;
    float mRecoverTimer;
    float mFlightTime;
};

