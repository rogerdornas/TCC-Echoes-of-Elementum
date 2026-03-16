//
// Created by roger on 15/03/2026.
//

#include "LightningDrawComponent.h"
#include "../../Game.h"
#include "../../Random.h"

LightningDrawComponent::LightningDrawComponent(Actor *owner, int drawOrder)
    :DrawComponent(owner, drawOrder)
    ,mAdditiveBlending(true)
{
}

void LightningDrawComponent::Draw(Renderer *renderer) {
    if (!mIsVisible|| mLightningBolts.empty()) {
        return;
    }

    // Cor da eletricidade (Ciano vibrante)
    Vector3 glowColor(0.0f, 0.8f, 1.0f);
    Vector3 coreColor(1.0f, 1.0f, 1.0f);
    Vector2 cameraPos = GetGame()->GetCamera()->GetPosCamera();

    for (const auto& boltPoints : mLightningBolts) {
        for (size_t i = 0; i < boltPoints.size() - 1; ++i) {
            Vector2 p1 = boltPoints[i];
            Vector2 p2 = boltPoints[i+1];

            // O Glow (Mais grosso, translúcido, ciano)
            renderer->DrawLine(p1, p2, glowColor, mGlowThickness, cameraPos, 0.5f, mAdditiveBlending);

            // O Core (Fino, opaco, branco)
            renderer->DrawLine(p1, p2, coreColor, mCoreThickness, cameraPos, 1.0f, mAdditiveBlending);
        }
    }
}

void LightningDrawComponent::SetPositions(Vector2 startPos, Vector2 endPos) {
    mStartPos = startPos;
    mEndPos = endPos;
}

void LightningDrawComponent::ClearLightningBolts() {
    mLightningBolts.clear();
}

void LightningDrawComponent::GenerateLightningBolts(Vector2 startPos, Vector2 endPos, int numBolts, float spreadRadius, int generations, float maxOffset) {
    mLightningBolts.clear();

    for (int i = 0; i < numBolts; ++i) {
        // Calcula um offset caótico para a posição inicial e final deste raio específico
        Vector2 randomStartOffset(Random::GetFloatRange(-spreadRadius, spreadRadius),
                                  Random::GetFloatRange(-spreadRadius, spreadRadius));

        Vector2 randomEndOffset(Random::GetFloatRange(-spreadRadius, spreadRadius),
                                Random::GetFloatRange(-spreadRadius, spreadRadius));

        // Aplica o offset
        Vector2 boltStart = startPos + randomStartOffset;
        Vector2 boltEnd = endPos + randomEndOffset;

        // O primeiro raio (i == 0) pode ser o "raio principal" e não ter offset
        if (i == 0) {
            boltStart = startPos;
            boltEnd = endPos;
        }

        mLightningBolts.push_back(GenerateSingleBolt(boltStart, boltEnd, generations, maxOffset));
    }
}

std::vector<Vector2> LightningDrawComponent::GenerateSingleBolt(Vector2 startPos, Vector2 endPos, int generations, float maxOffset) {
    std::vector<LightningSegment> segments;
    segments.push_back({startPos, endPos});

    for (int i = 0; i < generations; ++i) {
        std::vector<LightningSegment> newSegments;
        for (const auto& segment : segments) {
            Vector2 midPoint = (segment.start + segment.end) / 2.0f;
            Vector2 direction = segment.end - segment.start;
            Vector2 normal(-direction.y, direction.x);

            if (normal.Length() > 0) normal.Normalize();

            float offset = Random::GetFloatRange(-1.0f, 1.0f) * maxOffset;
            midPoint = midPoint + (normal * offset);

            newSegments.push_back({segment.start, midPoint});
            newSegments.push_back({midPoint, segment.end});
        }
        segments = newSegments;
        maxOffset /= 2.0f;
    }

    std::vector<Vector2> boltPoints;
    boltPoints.push_back(startPos);
    for (const auto& segment : segments) {
        boltPoints.push_back(segment.end);
    }

    return boltPoints;
}