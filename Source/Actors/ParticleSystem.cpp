//
// Created by roger on 16/04/2025.
//

#include "ParticleSystem.h"
#include "../Actors/Particle.h"
#include "../Game.h"
#include "../Random.h"
#include "../Components/Drawing/AnimatorComponent.h"

ParticleSystem::ParticleSystem(Game* game, Particle::ParticleType particleType, float particleSize, float emitRate, float particleLifeTime, float lifeTime)
    :Actor(game)
    ,mParticleType(particleType)
    ,mEmitRate(emitRate)
    ,mEmitTimer(0.0f)
    ,mParticleLifeTime(particleLifeTime)
    ,mParticleSize(particleSize)
    ,mLifeTime(lifeTime)
    ,mGroundCollision(true)
    ,mEnemyCollision(false)
    ,mColor(SDL_Color{255, 255, 255, 255})
    ,mParticleSpeedScale(1.0f)
    ,mParticleGravity(true)
    ,mEmitDirection(Vector2::Zero)
    ,mEmitArea(Vector2::Zero)
    ,mConeSpread(0.0f)
    ,mApplyDamage(false)
    ,mApplyFreeze(false)
    ,mFreezeDamage(1.0f)
    ,mFreezeIntensity(1.0f)
    ,mParticleDrawOrder(5000)
    ,mAdditiveBlending(false)
    ,mParticleFadeIn(false)
    ,mParallaxFactor(1.0f, 1.0f)
    ,mFollowTarget(nullptr)
{
}

void ParticleSystem::OnUpdate(float deltaTime) {
    if (mLifeTime >= 0.0f) {
        mLifeTime -= deltaTime;
        if (mLifeTime <= 0.0f) {
            SetState(ActorState::Destroy);
            return; // evita criar partículas depois de destruído
        }
    }

    if (mFollowTarget && mFollowTarget->GetState() != ActorState::Destroy) {
        SetPosition(mFollowTarget->GetPosition());
    }

    // Atualiza o timer de emissão
    mEmitTimer += deltaTime;

    // Emitir partículas de acordo com a taxa
    float interval = 1.0f / mEmitRate;
    while (mEmitTimer >= interval) {
        EmitParticle();
        mEmitTimer -= interval;
    }
}

void ParticleSystem::SetParticleSpeedScale(float speedScale) {
    mParticleSpeedScale = speedScale;
}

void ParticleSystem::EmitParticle() {
    std::vector<Particle* > particles = mGame->GetParticles();
    for (Particle* p: particles) {
        if (p->GetState() == ActorState::Paused && p->GetParticleType() == mParticleType) {
            p->SetSize(mParticleSize);
            p->SetApplyDamage(mApplyDamage);
            p->SetApplyFreeze(mApplyFreeze);
            p->SetFreezeDamage(mFreezeDamage);
            p->SetFreezeIntensity(mFreezeIntensity);
            p->SetFadeIn(mParticleFadeIn);
            p->SetLifeDuration(mParticleLifeTime);
            p->SetGroundCollision(mGroundCollision);
            p->SetEnemyCollision(mEnemyCollision);
            p->SetParticleColor(mColor);
            p->SetGravity(mParticleGravity);
            p->SetSpeedScale(mParticleSpeedScale);
            p->GetComponent<AnimatorComponent>()->SetDrawOrder(mParticleDrawOrder);
            p->GetComponent<AnimatorComponent>()->SetAdditiveBlending(mAdditiveBlending);
            p->GetComponent<AnimatorComponent>()->SetParallaxFactor(mParallaxFactor);

            // Pegar o ângulo base da direção
            float baseAngle = Math::Atan2(mEmitDirection.y, mEmitDirection.x);

            // Gerar um desvio aleatório entre -spread/2 e +spread/2
            float halfSpread = mConeSpread / 2.0f;
            float randomAngleOffset = Random::GetFloatRange(-halfSpread, halfSpread);

            float finalAngle = baseAngle + (randomAngleOffset * Math::Pi / 180.0f);

            // Criar o novo vetor de velocidade
            Vector2 coneVelocity;
            coneVelocity.x = Math::Cos(finalAngle);
            coneVelocity.y = Math::Sin(finalAngle);

            // Definir a velocidade com uma variação de "força"
            float speedVar = Random::GetFloatRange(700.0f, 1000.0f);
            p->SetVelocity(coneVelocity * speedVar * mParticleSpeedScale);

            Vector2 spawnPos = GetPosition();
            if (mEmitArea.x > 0.0f || mEmitArea.y > 0.0f) {
                float halfWidth = mEmitArea.x / 2.0f;
                float halfHeight = mEmitArea.y / 2.0f;
                spawnPos.x += Random::GetFloatRange(-halfWidth, halfWidth);
                spawnPos.y += Random::GetFloatRange(-halfHeight, halfHeight);
            }
            p->SetPosition(spawnPos);

            p->SetState(ActorState::Active);

            break;
        }
    }
}
