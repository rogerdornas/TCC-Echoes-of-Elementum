//
// Created by roger on 16/04/2025.
//

#pragma once

#include <string>
#include "Actor.h"
#include "../Components/Drawing/AnimatorComponent.h"

class Particle : public Actor
{
public:
    enum class ParticleType {
        SolidParticle,
        BlurParticle,
        Grass
    };

    Particle(class Game* game, ParticleType particleType);
    ~Particle() override;

    void OnUpdate(float deltaTime) override;
    ParticleType GetParticleType() { return mParticleType; }
    void SetSize(float width, float height);
    void SetLifeDuration(float life) { mLifeTDuration = life; }
    void SetGroundCollision(bool groundCollision) { mGroundCollision = groundCollision; }
    void SetEnemyCollision(bool enemyCollision) { mEnemyCollision = enemyCollision; }
    void SetParticleColor(SDL_Color color) { mColor = color; }
    void SetTextureFactor(float textureFactor) { mDrawComponent->SetTextureFactor(textureFactor); }
    SDL_Color GetColor() { return mColor; }
    void SetGravity(bool g) { mGravity = g; }
    void SetGravityForce(float gravityForce) { mGravityForce = gravityForce; }
    void SetSpeedScale(float speedScale) { mSpeedScale = speedScale; }
    void SetDirection(Vector2 direction);

    void SetApplyDamage(bool applyDamage) { mApplyDamage = applyDamage; }
    void SetApplyFreeze(bool applyFreeze) { mApplyFreeze = applyFreeze; }
    bool GetApplyFreeze() { return mApplyFreeze; }
    void SetFreezeDamage(float freezeDamage) { mFreezeDamage = freezeDamage; }
    void SetFreezeIntensity(float freezeIntensity) { mFreezeIntensity = freezeIntensity; }
    float GetFreezeIntensity() { return mFreezeIntensity; }
    void SetVelocity(Vector2 velocity);
    void UpdateFade();
    void SetFadeIn(bool fadeIn) { mFadeIn = fadeIn; }
    void SetAutoRotate(bool autoRotate) { mAutoRotate = autoRotate; }
    void SetRotationSpeed(float rotationSpeed);

    void Activate();
    void Deactivate();

private:
    void ApplyEnemyDamage();
    void ApplyEnemyFreeze();

    ParticleType mParticleType;
    float mWidth;
    float mHeight;
    float mLifeTDuration;
    float mLifeTimer;
    bool mGroundCollision;
    bool mEnemyCollision;
    std::string mTexturePath;
    SDL_Color mColor;
    bool mGravity;
    float mGravityForce;
    Vector2 mDirection;
    float mSpeedScale;
    bool mFadeIn;
    bool mAutoRotate;
    float mRotationSpeed;

    bool mApplyDamage;
    bool mApplyFreeze;
    float mFreezeDamage;
    float mFreezeIntensity;

    class RectComponent* mRectComponent;
    class AnimatorComponent* mDrawComponent;

    class RigidBodyComponent* mRigidBodyComponent;
    class ColliderComponent* mAABBComponent;
};
