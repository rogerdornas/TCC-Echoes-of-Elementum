//
// Created by roger on 06/04/2026.
//

#pragma once

#include "Actor.h"

class LaserExplosion : public Actor
{
public:
    LaserExplosion(Game* game, float width, float damage, float duration);

    void OnUpdate(float deltaTime) override;

private:
    void ResolvePlayerCollision();
    void ParticleEffect();

    float mWidth;
    float mHeight;
    float mDuration;
    float mTimer;
    float mDamage;

    class ParticleSystem* mSplash;

    class CombatBoxComponent* mCombatBoxComponent;
    class AnimatorComponent* mDrawComponent;
};

