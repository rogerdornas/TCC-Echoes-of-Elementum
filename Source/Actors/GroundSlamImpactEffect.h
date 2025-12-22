//
// Created by roger on 19/12/2025.
//

#pragma once

#include "Actor.h"

class GroundSlamImpactEffect : public Actor
{
public:
    GroundSlamImpactEffect(Game* game, float duration);

    void OnUpdate(float deltaTime) override;
    void Start(Vector2 position);

private:
    void ResolveEnemyCollision();

    float mWidth;
    float mHeight;
    float mDuration;
    float mTimer;
    float mMaxWidth;
    float mGrowSpeed;
    float mDamage;
    std::vector<class Enemy*> mEnemiesHit;

    class AnimatorComponent* mDrawComponent;
    class RectComponent* mRectComponent;
    class ColliderComponent* mAABBComponent;
};
