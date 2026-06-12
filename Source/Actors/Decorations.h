//
// Created by roger on 03/10/2025.
//

#pragma once
#include <string>
#include "Actor.h"

class Decorations : public  Actor
{
public:
    Decorations(Game* game, float width, float height, std::string imagePath, std::string decorationName,
                float fps, int numFrames, bool animated, int gid, float rotation,
                int drawOrder, Vector2 parallaxFactor = Vector2(1.0f, 1.0f),
                Vector3 textureColor = Color::White, float textureFactor = 1.0f,
                bool isDestructible = false);

    void OnUpdate(float deltaTime) override;

    void SetWindBalance(bool windBalance) { mHasWindBalance = windBalance; mTimeActive = GetPosition().x * 0.05f; }

    void SetMovement(bool isMoving, Vector2 velocity, float duration);

private:
    float mWidth;
    float mHeight;
    std::string mImagePath;
    float mFPS;
    int mNumFrames;
    bool mAnimated;

    float mBaseRotation;
    float mAngularVelocity;
    float mCooldownDuration;
    float mCooldownTimer;

    // Variáveis para o balanço contínuo
    bool mHasWindBalance;
    float mTimeActive;       // Cronômetro para o Seno
    float mWindAmplitude;    // Quantos graus a planta balança
    float mWindSpeed;        // Quão rápido ela balança

    Vector2 mBasePosition;
    bool mPositionInitialized;

    // Variáveis para o movimento
    bool mIsMoving;
    float mMovingDuration;
    float mMovingTimer;
    Vector2 mMoveVelocity;

    class AnimatorComponent* mDrawComponent;
    class ColliderComponent* mColliderComponent;
};

