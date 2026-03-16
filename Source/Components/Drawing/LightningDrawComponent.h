//
// Created by roger on 15/03/2026.
//

#pragma once

#include "DrawComponent.h"

class LightningDrawComponent : public DrawComponent {
public:
    struct LightningSegment {
        Vector2 start;
        Vector2 end;
    };

    LightningDrawComponent(Actor* owner, int drawOrder = 1000);

    void Draw(Renderer* renderer) override;

    void SetPositions(Vector2 startPos, Vector2 endPos);
    void SetGlowThickness(float thickness) { mGlowThickness = thickness; }
    void SetCoreThickness(float thickness) { mCoreThickness = thickness; }
    void ClearLightningBolts();
    void GenerateLightningBolts(Vector2 startPos, Vector2 endPos, int numBolts, float spreadRadius, int generations, float maxOffset);

private:
    std::vector<Vector2> GenerateSingleBolt(Vector2 startPos, Vector2 endPos, int generations, float maxOffset);

    Vector2 mStartPos;
    Vector2 mEndPos;
    float mGlowThickness;
    float mCoreThickness;

    std::vector<std::vector<Vector2>> mLightningBolts;

    bool mAdditiveBlending;

};
