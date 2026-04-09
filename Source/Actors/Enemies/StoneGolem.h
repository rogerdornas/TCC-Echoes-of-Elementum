//
// Created by roger on 05/04/2026.
//

#pragma once

#include "Enemy.h"

class StoneGolem : public Enemy
{
public:
    StoneGolem(Game* game);
    void OnUpdate(float deltaTime) override;

    void SetArenaMinPos(Vector2 pos) { mArenaMinPos = pos; }
    void SetArenaMaxPos(Vector2 pos) { mArenaMaxPos = pos; }
    void SetLeftLaserShooters(const std::vector<int> &laserShooters) { mLeftLaserShooters = laserShooters; }
    void SetRightLaserShooters(const std::vector<int> &laserShooters) { mRightLaserShooters = laserShooters; }
    void SetTopLaserShooters(const std::vector<int> &laserShooters) { mTopLaserShooters = laserShooters; }

    void ReceiveHit(float damage, Vector2 knockBackDirection, bool knockBack) override;

private:
    enum class State
    {
        Sleep, WakeUp, Stop,
        Chase, Reposition, MoveToCenter,
        MeleeAttack,
        ProjectileAttack,
        LaserCast, LaserShoot,
        ImmuneCast, Immune, ImmuneRecover,
    };

    void TriggerBossDefeat();
    void MovementAfterPlayerSpotted(float deltaTime);

    void Sleep(float deltaTime);
    void WakeUp(float deltaTime);
    void Stop(float deltaTime);
    void Chase(float deltaTime);
    void Reposition(float deltaTime);
    void MoveToCenter(float deltaTime);
    void MeleeAttack(float deltaTime);
    void ProjectileAttack(float deltaTime);
    void LaserCast(float deltaTime);
    void LaserShoot(float deltaTime);
    void ImmuneCast(float deltaTime);
    void Immune(float deltaTime);
    void ImmuneRecover(float deltaTime);

    void EvaluateNextState();
    void SetRepositionTarget();
    void PhaseChange();

    void ActiveSideLaserShooters();
    void ActiveTopLaserShooters();

    void ManageAnimations();
    void ManageCombatBox();

    State mEnemyState;
    int mCurrentPhase;
    bool mPhaseTransitionPending;

    float mDistToSpotPlayer;

    Vector2 mArenaMinPos;
    Vector2 mArenaMaxPos;

    float mTargetHeight;

    float mStopDuration;
    float mStopTimer;

    float mRunDuration;
    float mRunTimer;
    Vector2 mRepositionTarget;
    bool mIsAerialReposition;
    float mRepositionStartX;

    float mHitDuration;

    float mWakeUpDuration;
    float mWakeUpTimer;

    float mDistToMeleeAttack;
    float mMeleeAttackDuration;
    float mMeleeAttackTimer;
    Vector2 mMeleeAttackOffset;

    float mDistToRangeAttack;
    float mProjectileAttackDuration;
    float mProjectileAttackTimer;
    float mProjectileShootTime;
    bool mAlreadyProjectileAttack;

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
    bool mAlreadyLaserShoot;

    float mImmuneCastDuration;
    float mImmuneCastTimer;

    bool mIsImmune;
    float mImmuneDuration;
    float mImmuneTimer;

    float mImmuneRecoverDuration;
    float mImmuneRecoverTimer;

    std::vector<int> mLeftLaserShooters;
    std::vector<int> mRightLaserShooters;
    std::vector<int> mTopLaserShooters;
    bool mAlreadyActivateLaserShooters;
    float mActivateLaserShootersTime;
    int mNumDeactivateTopLaserShooters;
    int mLaserShootCountMax;
    int mLaserShootCount;
};

