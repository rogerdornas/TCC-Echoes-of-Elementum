//
// Created by roger on 20/05/2026.
//

#pragma once

#include "../Actor.h"
#include "Enemy.h"

class Frogger : public Enemy
{
public:
    Frogger(Game* game);
    void OnUpdate(float deltaTime) override;
    void SetArenaMinPos(Vector2 pos) { mArenaMinPos = pos; }
    void SetArenaMaxPos(Vector2 pos) { mArenaMaxPos = pos; }

private:
    enum class State
    {
        Sleep,
        Stop,
        JumpCombo,
        RunForward,
        Tongue,
        Spit
    };

    enum class WallSide
    {
        Bottom,
        Top,
        Left,
        Right
    };

    enum class GrassEffectType {
        Run,
        Jump,
        Land
    };

    void TriggerBossDefeat();
    void MovementAfterPlayerSpotted(float deltaTime);

    void Sleep(float deltaTime);
    void Stop(float deltaTime);
    void JumpCombo(float deltaTime);
    void RunForward(float deltaTime);
    void Tongue(float delTime);
    void Spit(float delTime);

    void ManageAnimations();
    void ManageCombatBox();

    void ResolveGroundCollision() override;

    void EvaluateNextState();

    void StartGrassEffect(GrassEffectType type);

    State mFroggerState;

    float mDistToSpotPlayer;

    float mStopDuration;
    float mStopTimer;

    float mHitDuration;

    int mMaxJumps;
    int mJumpCount;
    float mJumpForce;
    float mDurationBetweenJumps;
    float mTimerBetweenJumps;
    float mNextJumpDirectionX;
    float mNextJumpAngle;
    bool mWasOnGround;

    WallSide mWallPosition;
    WallSide mDestinyWall;
    float mGravity;

    Vector2 mArenaMinPos;
    Vector2 mArenaMaxPos;
    float mMinDistFromEdge;
    int mAttackJumpInterval;

    float mDistToTongue;
    float mTongueDuration;
    float mTongueTimer;

    float mSpitDuration;
    float mSpitTimer;

    float mJumpComboProbability;

    // Efeitos de grama ao correr
    float mRunningGrassParticleIntervalDuration;
    float mRunningGrassParticleIntervalTimer;
};
