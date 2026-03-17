//
// Created by roger on 15/03/2026.
//

#pragma once

#include "Actor.h"

class LightningEffect : public Actor
{
public:
    LightningEffect(Game* game, Actor* owner, float duration, bool destroyAfterEffect = false);

    void OnUpdate(float deltaTime) override;
    void SetStartPosition(Vector2 startPos) { mStartPos = startPos; }
    void SetEndPosition(Vector2 endPos) { mEndPos = endPos; }
    void StartEffect(Vector2 startPos, Vector2 endPos);
    void StopEffect() { mEffectTimer = mEffectDuration; }
    void SetDrawOrder(int drawOrder) const;
    void SetGlowThickness(float thickness) const;
    void SetCoreThickness(float thickness) const;
    void SetLightningGenerationIntervalDuration(float duration) { mLightningGenerationIntervalDuration = duration; }
    void SetNumBolts(int numBolts) { mNumBolts = numBolts; }
    void SetSpeadRadius (float  spreadRadius) { mSpreadRadius = spreadRadius; }
    void SetGenerations (int generations) { mGenerations = generations; }
    void SetMaxOffset(float maxOffset) { mMaxOffset = maxOffset; }

private:
    bool mDestroyAfterEffect;
    Vector2 mStartPos;
    Vector2 mEndPos;
    float mEffectDuration;
    float mEffectTimer;
    int mNumBolts;
    float mSpreadRadius;
    int mGenerations;
    float mMaxOffset;
    float mLightningGenerationIntervalDuration;
    float mLightningGenerationIntervalTimer;
    Actor* mOwner;

    class LightningDrawComponent* mDrawComponent;
};
