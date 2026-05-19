//
// Created by roger on 16/04/2025.
//

#pragma once
#include "Actor.h"
#include "../Actors/Particle.h"

class ParticleSystem : public Actor
{
public:
    ParticleSystem(class Game* game, Particle::ParticleType particleType, float particleWidth, float particleHeight, float emitRate = 20.0f, float particleLifeTime = 0.7f, float lifeTime = 1.0f);

    void OnUpdate(float deltaTime) override;
    void SetParticleLifeTime(float time) { mParticleLifeTime = time; }
    void SetEmitRate(float rate) { mEmitRate = rate; }
    float GetLifeTime() { return mLifeTime; }
    void EndParticleSystem() { mLifeTime = 0; }

    void SetEmitDirection(Vector2 emitDirection) { mEmitDirection = emitDirection; }
    void SetGroundCollision(bool groundCollision) { mGroundCollision = groundCollision; }
    void SetEnemyCollision(bool enemyCollision) { mEnemyCollision = enemyCollision; }
    void SetParticleSpeedScale(float speedScale);
    void SetParticleColor(SDL_Color color) { mColor = color; }
    void SetParticleTextureFactor(float textureFactor) { mParticleTextureFactor = textureFactor; }
    void SetParticleGravity(bool g) { mParticleGravity = g; }
    void SetParticleGravityForce(float gravityForce) { mParticleGravityForce = gravityForce; }

    void SetApplyDamage(bool applyDamage) { mApplyDamage = applyDamage; }
    void SetApplyFreeze(bool applyFreeze) { mApplyFreeze = applyFreeze; }
    void SetFreezeDamage(float freezeDamage) { mFreezeDamage = freezeDamage; }
    void SetFreezeIntensity(float freezeIntensity) { mFreezeIntensity = freezeIntensity; }

    void SetConeSpread(float degrees) { mConeSpread = degrees; }
    void SetParticleDrawOrder(int particleDrawOrder) { mParticleDrawOrder = particleDrawOrder; }
    void SetAdditiveBlending(bool additiveBlending) { mAdditiveBlending = additiveBlending; }
    void SetParallaxFactor(Vector2 parallaxFactor) { mParallaxFactor = parallaxFactor; }
    void SetParticleFadeIn(bool fadeIn) { mParticleFadeIn = fadeIn; }
    void SetParticleAutoRotate(bool autoRotate) { mParticleAutoRotate = autoRotate; }
    void SetParticleRotationSpeed(float rotationSpeed) { mParticleRotationSpeed = rotationSpeed; }
    void SetEmitArea(Vector2 emitArea) { mEmitArea = emitArea; }

    void SetFollowTarget(Actor* target) { mFollowTarget = target; }
private:
    void EmitParticle();

    Particle::ParticleType mParticleType;
    float mEmitRate;  // partículas por segundo
    float mEmitTimer; // timer para controlar emissões
    float mParticleLifeTime;
    float mParticleWidth;
    float mParticleHeight;
    float mLifeTime;
    bool mGroundCollision;
    bool mEnemyCollision;
    SDL_Color mColor;
    float mParticleTextureFactor;
    float mParticleSpeedScale;
    bool mParticleGravity;
    float mParticleGravityForce;
    Vector2 mEmitDirection;
    Vector2 mEmitArea;

    bool mApplyDamage;
    bool mApplyFreeze;
    float mFreezeDamage;
    float mFreezeIntensity;

    float mConeSpread;
    int mParticleDrawOrder;
    bool mAdditiveBlending;
    bool mParticleFadeIn;
    bool mParticleAutoRotate;
    float mParticleRotationSpeed;
    Vector2 mParallaxFactor;

    Actor* mFollowTarget;
};
