//
// Created by roger on 23/04/2026.
//

#pragma once

#include "Actor.h"

class AmbientParticleArea: public Actor
{
public:
    AmbientParticleArea(Game* game, float width, float height, float emitRate, Vector3 color);

    void OnUpdate(float deltaTime) override;

private:
    float mWidth;
    float mHeight;

    class ParticleSystem* mBackgroundParticles;
    class ParticleSystem* mForegroundParticles;
    Vector2 mBackgroundParallaxFactor;
    Vector2 mForegroundParallaxFactor;
};

