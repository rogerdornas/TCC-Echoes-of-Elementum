//
// Created by roger on 15/03/2026.
//

#include "LightningEffect.h"
#include "../Game.h"
#include "../Components/Drawing/LightningDrawComponent.h"

LightningEffect::LightningEffect(Game *game, Actor *owner, float duration, bool destroyAfterEffect)
    :Actor(game)
    ,mDestroyAfterEffect(destroyAfterEffect)
    ,mEffectDuration(duration)
    ,mEffectTimer(mEffectDuration)
    ,mNumBolts(1)
    ,mSpreadRadius(30.0f)
    ,mGenerations(5)
    ,mMaxOffset(40.0f)
    ,mLightningGenerationIntervalDuration(0.02f)
    ,mLightningGenerationIntervalTimer(mLightningGenerationIntervalDuration)
    ,mOwner(owner)
    ,mDrawComponent(nullptr)
{
    mDrawComponent = new LightningDrawComponent(this);
}

void LightningEffect::OnUpdate(float deltaTime) {
    mEffectTimer += deltaTime;
    if (mEffectTimer >= mEffectDuration) {
        if (mDestroyAfterEffect) {
            SetState(ActorState::Destroy);
        }
        else if (mDrawComponent) {
            mDrawComponent->SetVisible(false);
        }
        return;
    }
    if (mDrawComponent) {
        mDrawComponent->SetVisible(true);
        mLightningGenerationIntervalTimer += deltaTime;
        if (mLightningGenerationIntervalTimer >= mLightningGenerationIntervalDuration) {
            mDrawComponent->SetPositions(mStartPos, mEndPos);
            mDrawComponent->GenerateLightningBolts(mStartPos, mEndPos, mNumBolts, mSpreadRadius, mGenerations, mMaxOffset);
            mLightningGenerationIntervalTimer -= mLightningGenerationIntervalDuration;
        }
    }
}

void LightningEffect::StartEffect(Vector2 startPos, Vector2 endPos) {
    mEffectTimer = 0.0f;

    mStartPos = startPos;
    mEndPos = endPos;

    // Força o timer a estourar no próximo OnUpdate para gerar o raio na hora
    mLightningGenerationIntervalTimer = mLightningGenerationIntervalDuration;

    // Limpa os raios da memória
    if (mDrawComponent) {
        mDrawComponent->ClearLightningBolts();
        mDrawComponent->SetPositions(mStartPos, mEndPos);
    }
}

void LightningEffect::SetDrawOrder(int drawOrder) const {
    mDrawComponent->SetDrawOrder(drawOrder);
}

void LightningEffect::SetGlowThickness(float thickness) const {
    mDrawComponent->SetGlowThickness(thickness);
}

void LightningEffect::SetCoreThickness(float thickness) const {
    mDrawComponent->SetCoreThickness(thickness);
}
