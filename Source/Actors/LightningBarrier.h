//
// Created by roger on 18/12/2025.
//

#pragma once

#include "Actor.h"

class LightningBarrier : public Actor
{
public:
    LightningBarrier(Game* game, float width, float height, bool isMoving, float movingDuration, Vector2 velocity);

    void OnUpdate(float deltaTime) override;

private:
    void ResolveEnemyCollision();
    void ResolvePlayerCollision();

    float mHeight;
    float mWidth;
    float mDamage;
    bool mIsMoving;
    float mMovingDuration;
    float mMovingTimer;
    Vector2 mVelocity;
    bool mAlreadyHitPlayer;

    class RectComponent* mRectComponent;
    class AnimatorComponent* mDrawComponent;
    class RigidBodyComponent *mRigidBodyComponent;
    class ColliderComponent *mAABBComponent;
};

