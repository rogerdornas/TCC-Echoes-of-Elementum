//
// Created by roger on 06/04/2026.
//

#pragma once

#include "Actor.h"

class GolemProjectile : public Actor
{
public:
    GolemProjectile(Game* game);

    void OnUpdate(float deltaTime) override;

private:
    void ResolveGroundCollision();
    void ResolvePlayerCollision();

    void Destroy();

    float mWidth;
    float mHeight;
    float mSpeed;
    float mDuration;
    float mTimer;
    float mDamage;
    float mTurnSpeed;

    class AnimatorComponent* mDrawComponent;
    class RigidBodyComponent* mRigidBodyComponent;
    class ColliderComponent* mOBBComponent;
    class CombatBoxComponent* mCombatBoxComponent;
};

