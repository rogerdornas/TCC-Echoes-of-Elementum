//
// Created by roger on 15/03/2026.
//

#pragma once

#include "DrawComponent.h"
#include <vector>

struct GhostSnapshot {
    Vector2 position;
    Vector2 scale;
    float rotation;
    Vector4 texRect;
    float lifeTimer;    // Quanto tempo este fantasma já viveu
    float maxLifetime;  // Quanto tempo ele deve viver no total
};

class GhostTrailComponent: public DrawComponent {
public:
    GhostTrailComponent(class Actor* owner, class AnimatorComponent* animator, int drawOrder = 90);

    void Update(float deltaTime) override;
    void Draw(class Renderer* renderer) override;

    void SetIsEmitting(bool emitting) { mIsEmitting = emitting; }

private:
    class AnimatorComponent* mTargetAnimator;
    std::vector<GhostSnapshot> mSnapshots;

    bool mIsEmitting;
    float mSpawnTimer;
    float mSpawnInterval; // Tempo entre gerar um fantasma e outro

    Vector3 mGhostColor;

    bool mAdditiveBlending;
};

