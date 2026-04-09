//
// Created by roger on 07/04/2026.
//

#pragma once

#include "Actor.h"

class LaserShooter : public Actor
{
public:
    LaserShooter(Game* game);
    ~LaserShooter();

    void OnUpdate(float deltaTime) override;

    void SetId(int id) { mId = id; }
    int GetId() const { return mId; }
    void SetContinuousShooting(bool continuousShooting) { mContinuousShooting = continuousShooting; }
    void SetIdleDuration(float duration) { mIdleDuration = duration; }
    void SetCastDuration(float duration) { mLaserCastDuration = duration; }
    void SetShootDuration(float duration) { mLaserShootDuration = duration; }

    void Destroy();

    void IncreaseShotsRemaining(int shots) { mShotsRemaining += shots; }

private:
    enum class State
    {
        Idle,
        LaserCast,
        LaserShoot
    };

    void Idle(float deltaTime);
    void LaserCast(float deltaTime);
    void LaserShoot(float deltaTime);

    void ManageAnimations();

    State mLaserShootState;

    int mId;

    float mWidth;
    float mHeight;

    bool mContinuousShooting;
    int mShotsRemaining;

    float mIdleDuration;
    float mIdleTimer;

    float mLaserCastDuration;
    float mLaserCastTimer;
    float mLaserTargetTime;
    float mLaserTargetAngle;
    bool mAlreadySetLaserAngle;

    float mLaserShootDuration;
    float mLaserShootTimer;
    float mLaserHeight;
    float mLaserDamage;
    Vector2 mLaserOffset;
    float mAlreadyLaserShoot;

    class AnimatorComponent* mDrawComponent;
};

