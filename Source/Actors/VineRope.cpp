//
// Created by roger on 23/04/2026.
//

#include "VineRope.h"
#include "../Game.h"
#include "../Components/Drawing/AnimatorComponent.h"

VineRope::VineRope(Game *game, float thickness)
    :Actor(game)
    ,mThickness(thickness)
    ,mMaxWidth(mThickness * 9.96f)
    ,mDrawComponent(nullptr)
{
    mDrawComponent = new AnimatorComponent(this, "../Assets/Sprites/VineRope/VineRope.png",
                                                "../Assets/Sprites/VineRope/VineRope.json",
                                                mMaxWidth, mThickness * 1.7f, 102);

    std::vector<int> throwing = {0};
    mDrawComponent->AddAnimation("throwing", throwing);

    std::vector<int> hooking = {1};
    mDrawComponent->AddAnimation("hooking", hooking);

    mDrawComponent->SetAnimation("throwing");
    mDrawComponent->SetAnimFPS(1);

    mDrawComponent->SetFrameCrop(0.0f, 0.0f, 0.0f, 1.0f);
    Deactivate();
}

void VineRope::OnUpdate(float deltaTime) {

}

void VineRope::SetIsHooking(const bool isHooking) {
    if (isHooking) {
        mDrawComponent->SetAnimation("hooking");
    }
    else {
        mDrawComponent->SetAnimation("throwing");
    }
}

void VineRope::Activate() {
    if (mDrawComponent) {
        mDrawComponent->SetVisible(true);
    }
}

void VineRope::Deactivate() {
    SetState(ActorState::Paused);
    if (mDrawComponent) {
        mDrawComponent->SetVisible(false);
        mDrawComponent->SetFrameCrop(0.0f, 0.0f, 0.0f, 1.0f);
    }
}

void VineRope::SetEndpoints(const Vector2& start, const Vector2& end) {
    Vector2 diff = end - start;
    float length = diff.Length();

    Vector2 centerPos = start + (diff * 0.5f);
    SetPosition(centerPos);

    if (length > 0.001f) {
        float angle = std::atan2(diff.y, diff.x);
        SetRotation(angle);
        SetTransformRotation(angle);
    }

    float cropW = length / mMaxWidth;

    cropW = std::min(cropW, 1.0f);

    if (mDrawComponent) {
        mDrawComponent->SetFrameCrop(0.0f, 0.0f, cropW, 1.0f);
    }
}
