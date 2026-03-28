//
// Created by roger on 18/12/2025.
//

#include "DashEffect.h"
#include "../Game.h"
#include "../Components/Drawing/AnimatorComponent.h"

DashEffect::DashEffect(Game *game, Actor* owner, float duration)
    :Actor(game)
    ,mWidth(195)
    ,mHeight(159)
    ,mEffectDuration(duration)
    ,mEffectTimer(mEffectDuration)
    ,mOffsetPosition(Vector2::Zero)
    ,mOwner(owner)
    ,mDrawComponent(nullptr)
{
    mDrawComponent = new AnimatorComponent(this, "../Assets/Sprites/Dash5/Dash.png",
                                               "../Assets/Sprites/Dash5/Dash.json",
                                               mWidth, mHeight, 1002);

    std::vector idle = {0, 1, 2, 3, 4, 5};
    mDrawComponent->AddAnimation("idle", idle);

    mDrawComponent->SetAnimation("idle");
    mDrawComponent->SetAnimFPS(30);

    mDrawComponent->SetAlpha(0.6f);
}

void DashEffect::OnUpdate(float deltaTime) {
    mEffectTimer += deltaTime;
    if (mEffectTimer >= mEffectDuration) {
        if (mDrawComponent) {
            mDrawComponent->SetVisible(false);
        }
    }
    else if (mOwner->GetState() == ActorState::Active) {
        if (mDrawComponent) {
            mDrawComponent->SetVisible(true);
        }
        SetPosition(mOwner->GetPosition() + mOffsetPosition);
    }
}

void DashEffect::StartDashEffect() {
    mEffectTimer = 0;
}
