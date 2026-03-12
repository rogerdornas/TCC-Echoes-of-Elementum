//
// Created by roger on 10/03/2026.
//

#pragma once
#include "Actor.h"

class FireWisp : public  Actor
{
public:
    enum class State
    {
        Idle,
        Windup,
        Attacking
    };

    FireWisp(Game* game);

    void OnUpdate(float deltaTime) override;
    void SetCanAttackEnemies(bool canAttackEnemies) { mCanAttackEnemies = canAttackEnemies; }

private:
    void InitParticles();
    void InitLight();
    void UpdateLight(float deltaTime);
    void AttackEnemies(float deltaTime);

    State mFireWispState;

    float mWidth;
    float mHeight;

    class Light* mLight;
    class Light* mCoreLight;
    float mLightTimer;
    float mLightRadius;
    float mLightIntensity;
    float mCoreLightRadius;
    float mCoreLightIntensity;
    float mSpeed;
    float mLerpSpeed;
    Vector2 mOffsetPosition;
    float mDuration;
    float mHoverTimer;

    bool mCanAttackEnemies;
    float mDamage;
    float mAttackRange;
    float mAttackIntervalDuration;
    float mAttackIntervalTimer;
    float mWindupDuration;
    float mWindupTimer;
    Vector2 mAttackVelocity;
    float mAttackSpeed;
    class Enemy* mTargetEnemy;

    float mExplosionDuration;
    float mExplosionTimer;
    float mAttackDuration;
    float mAttackTimer;
    Vector3 mColorNormal;
    Vector3 mColorWindup;
    Vector3 mColorExplosion;

    class ParticleSystem* mSmoke;
    class ParticleSystem* mSpark;
    class ParticleSystem* mFlame;

    class RigidBodyComponent* mRigidBodyComponent;
    class CombatBoxComponent* mCombatBoxComponent;
};

