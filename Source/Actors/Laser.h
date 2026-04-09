//
// Created by roger on 05/04/2026.
//

#pragma once

#include "Actor.h"

class Laser : public Actor
{
public:
    Laser(Game* game, float height, float duration, float damage);

    void OnUpdate(float deltaTime) override;

    void Initialize();

    void SetStartPosition(const Vector2 startPosition) { mStartPosition = startPosition; }
    void SetStartPositionOffset(float offset) { mStartPositionOffset = offset; }
    void SetPlayerCollision(const bool collision) { mPlayerCollision = collision; }
    void CropFromEnd() { mCropBeginning = false; }
    void ShowLaserExplosion(bool showLaserExplosion);

private:
    void ResolvePlayerCollision();
    void CalculateInstantWidth();

    void UpdateLaserTransform();
    void UpdateLaserExplosion();

    float mWidth;
    float mHeight;
    float mBaseHeight;
    float mMaxWidth;
    float mExtraWidth;
    float mDuration;
    float mTimer;
    float mDamage;
    Vector2 mStartPosition;
    float mStartPositionOffset;
    float mWidthOffset;
    bool mPlayerCollision;
    bool mCropBeginning;
    bool mShowLaserExplosion;
    Vector2 mHitNormal;
    class LaserExplosion* mLaserExplosion;

    class RigidBodyComponent* mRigidBodyComponent;
    class CombatBoxComponent* mCombatBoxComponent;
    class AnimatorComponent* mDrawComponent;
};
