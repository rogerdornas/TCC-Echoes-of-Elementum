//
// Created by roger on 23/04/2026.
//

#include "AmbientParticleArea.h"
#include "ParticleSystem.h"
#include "../Game.h"

AmbientParticleArea::AmbientParticleArea(Game *game, float width, float height, float emitRate, Vector3 color)
    :Actor(game)
    ,mWidth(width)
    ,mHeight(height)
    ,mBackgroundParticles(nullptr)
    ,mForegroundParticles(nullptr)
    ,mBackgroundParallaxFactor(0.8f, 0.8f)
    ,mForegroundParallaxFactor(1.3f, 1.3f)
{
    mBackgroundParticles = new ParticleSystem(game, Particle::ParticleType::BlurParticle,
                                              6.0f * 1.2f,
                                              6.0f,
                                              emitRate,
                                              6.0f,
                                              -1.0f);

    mBackgroundParticles->SetEmitArea(Vector2(mWidth, mHeight));
    mBackgroundParticles->SetParticleGravity(false);
    mBackgroundParticles->SetGroundCollision(false);
    mBackgroundParticles->SetConeSpread(360.0f);
    mBackgroundParticles->SetParticleSpeedScale(0.01f);
    mBackgroundParticles->SetParticleColor(SDL_Color{static_cast<Uint8>(color.x), static_cast<Uint8>(color.y), static_cast<Uint8>(color.z), 150});
    mBackgroundParticles->SetParticleDrawOrder(150);
    mBackgroundParticles->SetParallaxFactor(mBackgroundParallaxFactor);
    mBackgroundParticles->SetAdditiveBlending(true);
    mBackgroundParticles->SetParticleFadeIn(true);


    mForegroundParticles = new ParticleSystem(game, Particle::ParticleType::BlurParticle,
                                              17.0f * 1.2f,
                                              17.0f,
                                              emitRate * 0.7f,
                                              6.0f,
                                              -1.0f);

    mForegroundParticles->SetEmitArea(Vector2(mWidth, mHeight));
    mForegroundParticles->SetParticleGravity(false);
    mForegroundParticles->SetGroundCollision(false);
    mForegroundParticles->SetConeSpread(360.0f);
    mForegroundParticles->SetParticleSpeedScale(0.01f);
    mForegroundParticles->SetParticleColor(SDL_Color{static_cast<Uint8>(color.x), static_cast<Uint8>(color.y), static_cast<Uint8>(color.z), 220});
    mForegroundParticles->SetParticleDrawOrder(8000);
    mForegroundParticles->SetParallaxFactor(mForegroundParallaxFactor);
    mForegroundParticles->SetAdditiveBlending(true);
    mForegroundParticles->SetParticleFadeIn(true);
}

void AmbientParticleArea::OnUpdate(float deltaTime) {
    Vector2 cameraPos = mGame->GetCamera()->GetPosCamera();

    Vector2 basePos = GetPosition();

    if (mBackgroundParticles) {
        // Calcula o offset para compensar a ilusão do paralaxe
        Vector2 offset;
        offset.x = cameraPos.x * (mBackgroundParallaxFactor.x - 1.0f);
        offset.y = cameraPos.y * (mBackgroundParallaxFactor.y - 1.0f);

        // Move o emissor para a posição compensada
        mBackgroundParticles->SetPosition(basePos + offset);
    }

    if (mForegroundParticles) {
        // Calcula o offset
        Vector2 offset;
        offset.x = cameraPos.x * (mForegroundParallaxFactor.x - 1.0f);
        offset.y = cameraPos.y * (mForegroundParallaxFactor.y - 1.0f);

        // Move o emissor da frente para a posição compensada
        mForegroundParticles->SetPosition(basePos + offset);
    }
}
