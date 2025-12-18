//
// Created by roger on 18/12/2025.
//

#pragma once

#include "Actor.h"

class DashEffect : public Actor
{
public:
    DashEffect(Game* game, Actor* owner, float duration);

    void OnUpdate(float deltaTime) override;
    void SetOffsetPosition(Vector2 offsetPosition) { mOffsetPosition = offsetPosition; }
    void StartDashEffect();
    void StopDash() { mEffectTimer = mEffectDuration; }

private:
    float mWidth;
    float mHeight;
    float mEffectDuration;
    float mEffectTimer;
    Vector2 mOffsetPosition;
    Actor* mOwner;

    class AnimatorComponent* mDrawComponent;
};
