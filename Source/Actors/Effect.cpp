//
// Created by roger on 30/05/2025.
//

#include "Effect.h"
#include "../Random.h"
#include "../Components/Drawing/AnimatorComponent.h"
#include "../Components/Drawing/RectComponent.h"


Effect::Effect(class Game* game)
    :Actor(game)
    ,mDuration(0.1f)
    ,mDurationTimer(0.0f)
    ,mSize(50)
    ,mActor(nullptr)
    ,mColor(SDL_Color{200, 200, 200, 255})
    ,mFadeIn(false)
    ,mRectComponent(nullptr)
    ,mDrawComponent(nullptr)
{
}

void Effect::SetEffect(TargetEffect targetEffect) {
    mTargetEffect = targetEffect;
    float size;
    switch (mTargetEffect) {
        case TargetEffect::SwordHit:
            SetTransformRotation(Random::GetFloatRange(-Math::Pi/4, Math::Pi/4));
            size = Random::GetFloatRange(1.2 * mSize, 1.5 * mSize);
            mDrawComponent = new AnimatorComponent(this, "../Assets/Sprites/Effects/Spark.png", "",
                                                                size * 2, size / 2, 5000);
        break;
        case TargetEffect::Circle:
            SetPosition(mActor->GetPosition());
            mDrawComponent = new AnimatorComponent(this, "../Assets/Sprites/Effects/ImperfectCircleBlur.png", "",
                                                        mSize, mSize, 5000);
        break;
    }

    if (mDrawComponent) {
        mDrawComponent->SetColor(Vector3(mColor.r / 255.0f, mColor.g / 255.0f, mColor.b / 255.0f));
        mDrawComponent->SetTextureFactor(0.0f);

        float startAlpha = mFadeIn ? 0.0f : (mColor.a / 255.0f);
        mDrawComponent->SetAlpha(startAlpha);
    }
}


void Effect::OnUpdate(float deltaTime) {
    switch (mTargetEffect) {
        case TargetEffect::SwordHit:
            SwordHitEffect(deltaTime);
            break;
        case TargetEffect::Circle:
            CircleEffect(deltaTime);
            break;
    }
}

void Effect::CircleEffect(float deltaTime) {
    if (!mActor) {
        SetState(ActorState::Destroy);
        return;
    }

    mDurationTimer += deltaTime;
    if (mDurationTimer >= mDuration) {
        SetState(ActorState::Destroy);
        return;
    }

    if (mDrawComponent) {
        float progress = mDurationTimer / mDuration;
        float alphaMultiplier = 0.0f;

        if (mFadeIn) {
            // Ponto de virada: aos 25% da vida, ele atinge o brilho máximo
            const float fadeInThreshold = 0.25f;

            if (progress <= fadeInThreshold) {
                // RETA DE SUBIDA (0.0 até 1.0)
                alphaMultiplier = progress / fadeInThreshold;
            } else {
                // RETA DE DESCIDA (1.0 até 0.0)
                alphaMultiplier = (1.0f - progress) / (1.0f - fadeInThreshold);
            }
        } else {
            // Comportamento original: já nasce no 1.0 e vem caindo até 0.0
            alphaMultiplier = 1.0f - progress;
        }

        float currentAlpha = alphaMultiplier * mColor.a;
        mDrawComponent->SetAlpha(currentAlpha / 255.0f);
    }

    SetPosition(mActor->GetPosition());
}

void Effect::SwordHitEffect(float deltaTime) {
    mDurationTimer += deltaTime;
    if (mDurationTimer >= mDuration) {
        SetState(ActorState::Destroy);
        return;
    }
}
