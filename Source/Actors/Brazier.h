//
// Created by roger on 20/11/2025.
//

#pragma once
#include "Actor.h"

class Brazier : public  Actor
{
public:
    enum class BrazierState {
        LightOn,
        LightOff
    };

    Brazier(Game* game, BrazierState state = BrazierState::LightOff);

    void OnUpdate(float deltaTime) override;

private:
    void InitLight();
    void ResolveFreezeParticleCollision();
    void ManageAnimations();

    float mWidth;
    float mHeight;
    class Light* mLight;
    class Light* mRedLight;
    float mLightTimer;
    float mLightRadius;
    float mLightIntensity;
    float mRedLightRadius;
    float mRedLightIntensity;
    BrazierState mBrazierState;
    float mFreezeMax;
    float mFreezeCount;
    float mIntervalBetweenSmokeEmitDuration;
    float mIntervalBetweenSmokeEmitTimer;

    class ColliderComponent* mAABBComponent;
    class RectComponent* mRectComponent;
    class AnimatorComponent* mDrawComponent;
};

