//
// Created by roger on 23/04/2026.
//

#pragma once

#include "Actor.h"

class VineRope : public Actor
{
public:
    VineRope(Game* game, float thickness);

    void OnUpdate(float deltaTime) override;

    void SetEndpoints(const Vector2& start, const Vector2& end);

    void SetIsHooking(const bool isHooking);

    void Deactivate();
    void Activate();

private:
    float mThickness;
    float mMaxWidth;

    class AnimatorComponent* mDrawComponent;
};

