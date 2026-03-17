//
// Created by roger on 16/03/2026.
//

#pragma once

#include "Actor.h"

class LightningSpear : public Actor
{
public:
    LightningSpear(Game* game);

    void OnUpdate(float deltaTime) override;

    void Activate();
    void Deactivate();

private:
    void ResolveGroundCollision();
    void ResolveEnemyCollision();

    class Enemy* FindClosestEnemy(const Vector2& fromPos);
    void ProcessChainLightning(class Enemy* firstTarget);
    void SpawnChainVisual(Vector2 startPos, Vector2 endPos);

    float mWidth;
    float mHeight;
    float mSpeed;
    float mDuration;
    float mTimer;
    float mDamage;
    std::vector<class Enemy*> mEnemiesHit;
    int mNumEnemiesHit;
    int mMaxNumEnemiesHit;
    float mShockChainRange;

    class LightningEffect* mLightningEffect;
    float mLightningEffectWidth;

    class AnimatorComponent *mDrawComponent;
    class RigidBodyComponent *mRigidBodyComponent;
    class ColliderComponent *mAABBComponent;
    class CombatBoxComponent* mCombatBoxComponent;
    class GhostTrailComponent* mGhostTrailComponent;
};

