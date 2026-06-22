//
// Created by roger on 30/05/2025.
//

#pragma once

#include "Actor.h"
#include "../Game.h"

enum class TargetEffect {
    SwordHit,
    Circle
};

class Effect : public Actor
{
public:
    Effect(class Game* game);

    void SetEffect(TargetEffect targetEffect);
    void SetSize(float size) { mSize = size; }
    void SetColor(SDL_Color color) { mColor = color; }
    void SetActor(Actor &actor) { mActor = &actor; }
    void SetDuration(float duration) { mDuration = duration; }
    void ActorDestroyed() { mActor = nullptr; }
    void SetFadeIn(bool fadeIn) { mFadeIn = fadeIn; }

    void OnUpdate(float deltaTime) override;

private:
    void SwordHitEffect(float deltaTime);
    void CircleEffect(float deltaTime);

    TargetEffect mTargetEffect;

    float mDuration;
    float mDurationTimer;
    float mSize;
    Actor* mActor;

    SDL_Color mColor;
    bool mFadeIn;

    class RectComponent* mRectComponent;
    class AnimatorComponent* mDrawComponent;
};
