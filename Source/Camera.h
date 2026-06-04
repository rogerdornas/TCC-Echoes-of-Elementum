//
// Created by roger on 23/04/2025.
//

#pragma once

#include "Math.h"
#include "Game.h"

enum class CameraMode {
    FollowPlayerLimited,
    PanoramicCamera,
};

class Camera
{
public:
    Camera(class Game *game, Vector2 startPosition);

    void SetPosition(Vector2 pos) { mPos = pos; }
    Vector2 GetPosCamera() const { return mPos; }

    void SetLimitMinCameraPosition(Vector2 pos);
    void SetLimitMaxCameraPosition(Vector2 pos);
    void SetCameraMinBound(Vector2 minBound) { mCameraMinBound = minBound; }
    void SetCameraMaxBound(Vector2 maxBound) { mCameraMaxBound = maxBound; }

    void Update(float deltaTime);

    void SetLookUp() { mLookUp = true; }
    void SetLookDown() { mLookDown = true; }

    void SetCameraVelocity(Vector2 velocity) { mCameraVelocity = velocity; }

    void SetZoom(float zoom);
    float GetZoom() const { return mZoom; }

    void StartCameraShake(float duration = 1.0f, float strength = 5.0f);
    void ChangeCameraMode(CameraMode cameraMode);

private:
    Vector2 PanoramicCamera(float deltaTime);

    Vector2 mPos;
    class Game *mGame;
    float mCameraLerpSpeed;
    float mNormalSpeed;
    float mSlowTransitionSpeed;
    float mInitPositionSpeed;
    float mTransitionDuration;
    float mTransitionTimer;
    CameraMode mCameraMode;
    Vector2 mLimitMinCameraPosition;
    Vector2 mLimitMaxCameraPosition;
    Vector2 mCurrentLimitMinPosition;
    Vector2 mCurrentLimitMaxPosition;
    Vector2 mCameraMinBound;
    Vector2 mCameraMaxBound;
    float mCurrentOffsetX;
    float mInitPositionTimer;
    bool mIsAdjustingInitialPosition;

    float mDistMove;

    bool mIsShaking;
    float mShakeDuration;
    float mShakeTimer;
    float mShakeStrength;

    Vector2 mCameraVelocity;

    bool mLookUp;
    bool mLookDown;

    float mZoom;
};
