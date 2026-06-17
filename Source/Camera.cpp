//
// Created by roger on 23/04/2025.
//

#include "Camera.h"
#include "Random.h"
#include "Components/RigidBodyComponent.h"

Camera::Camera(class Game* game, Vector2 startPosition)
    :mPos(startPosition)
    ,mGame(game)
    ,mCameraLerpSpeed(6.0f)
    ,mNormalSpeed(6.0f)
    ,mSlowTransitionSpeed(2.0f)
    ,mInitPositionSpeed(20.0f)
    ,mTransitionDuration(0.0f)
    ,mTransitionTimer(mTransitionDuration)
    ,mCameraMode(CameraMode::FollowPlayerLimited)
    ,mLimitMinCameraPosition(Vector2(0, 0))
    ,mLimitMaxCameraPosition(Vector2(70000, 70000))
    ,mCurrentOffsetX(0.0f)
    ,mInitPositionTimer(0.1f)
    ,mIsAdjustingInitialPosition(true)
    ,mDistMove(200)
    ,mIsShaking(false)
    ,mShakeDuration(1.0f)
    ,mShakeTimer(0.0f)
    ,mShakeStrength(5.0f)
    ,mCameraVelocity(Vector2::Zero)
    ,mLookUp(false)
    ,mLookDown(false)
    ,mZoom(1.0f)
{
}

void Camera::StartCameraShake(float duration, float strength) {
    mIsShaking = true;
    mShakeTimer = 0.0f;
    mShakeDuration = duration;
    mShakeStrength = strength;
}

void Camera::ChangeCameraMode(CameraMode cameraMode) {
    if (mCameraMode != cameraMode) {
        mTransitionTimer = 0;
    }
    mCameraMode = cameraMode;
}

void Camera::SetLimitMinCameraPosition(Vector2 pos) {
    if (mLimitMinCameraPosition.x != pos.x ||
        mLimitMinCameraPosition.y != pos.y)
    {
        mLimitMinCameraPosition = pos;
        mCurrentLimitMinPosition = mPos;
    }
}

void Camera::SetLimitMaxCameraPosition(Vector2 pos) {
    if (mLimitMaxCameraPosition.x != pos.x ||
        mLimitMaxCameraPosition.y != pos.y)
    {
        mLimitMaxCameraPosition = pos;
        mCurrentLimitMaxPosition = mPos + Vector2(mGame->GetRenderer()->GetZoomedWidth(), mGame->GetRenderer()->GetZoomedHeight());
    }
}

void Camera::Update(float deltaTime) {
    // Timer de início de cena
    if (mInitPositionTimer > 0) {
        mInitPositionTimer -= deltaTime;
        mIsAdjustingInitialPosition = true;
    }
    else {
        mIsAdjustingInitialPosition = false;
    }

    if (mCameraMode == CameraMode::FollowPlayerLimited) {
        float screenW = mGame->GetRenderer()->GetZoomedWidth();
        float screenH = mGame->GetRenderer()->GetZoomedHeight();

        // Calcula o offset alvo
        float desiredOffsetX = mGame->GetPlayer()->GetWidth() * 2.5f * mGame->GetPlayer()->GetForward().x;

        // Limites e Offsets: Snap vs Suavidade
        if (mIsAdjustingInitialPosition) {
            // SNAP (TELEPORTE): Aplica tudo instantaneamente no começo da cena
            mCurrentLimitMinPosition = mLimitMinCameraPosition;
            mCurrentLimitMaxPosition = mLimitMaxCameraPosition;
            mCurrentOffsetX = desiredOffsetX;
        } else {
            // LERP: Comportamento suave normal do jogo
            mCameraLerpSpeed = mSlowTransitionSpeed;

            mCurrentLimitMinPosition = Vector2::Lerp(mCurrentLimitMinPosition, mLimitMinCameraPosition, mCameraLerpSpeed * deltaTime);
            mCurrentLimitMaxPosition = Vector2::Lerp(mCurrentLimitMaxPosition, mLimitMaxCameraPosition, mCameraLerpSpeed * deltaTime);
            mCurrentOffsetX = Math::Lerp(mCurrentOffsetX, desiredOffsetX, mCameraLerpSpeed * deltaTime);
        }

        // Calcula posição alvo base baseada no jogador + offset
        Vector2 playerPos = mGame->GetPlayer()->GetPosition();
        Vector2 playerPosOffset(
            playerPos.x - screenW * 0.5f + mCurrentOffsetX,
            playerPos.y - screenH * 0.5f
        );

        // Aplica limites (agora com valores corretos dependendo se foi snap ou lerp)
        playerPosOffset.x = Math::Clamp(playerPosOffset.x, mCurrentLimitMinPosition.x, mCurrentLimitMaxPosition.x - screenW);
        playerPosOffset.y = Math::Clamp(playerPosOffset.y, mCurrentLimitMinPosition.y, mCurrentLimitMaxPosition.y - screenH);

        // Aplica deslocamento vertical (LookUp/LookDown)
        if (mLookUp) {
            playerPosOffset.y -= mDistMove;
        }
        else if (mLookDown) {
            playerPosOffset.y += mDistMove;
        }

        // Hard Bounds do Mapa
        playerPosOffset.x = Math::Clamp(playerPosOffset.x, mCameraMinBound.x, mCameraMaxBound.x - screenW);
        playerPosOffset.y = Math::Clamp(playerPosOffset.y, mCameraMinBound.y, mCameraMaxBound.y - screenH);

        // Posição da Câmera: Snap vs Suavidade
        if (mIsAdjustingInitialPosition) {
            mPos = playerPosOffset; // A câmera é teleportada para o alvo exato.
        } else {
            mCameraLerpSpeed = mNormalSpeed;
            mPos = Vector2::Lerp(mPos, playerPosOffset, mCameraLerpSpeed * deltaTime);
        }

        // Camera Shake
        if (mShakeTimer < mShakeDuration) {
            mShakeTimer += deltaTime;
        }

        float shakeOffsetX = 0;
        float shakeOffsetY = 0;

        if (mIsShaking) {
            shakeOffsetX = Random::GetFloat() * (2 * mShakeStrength + 1) - mShakeStrength;
            shakeOffsetY = Random::GetFloat() * (2 * mShakeStrength + 1) - mShakeStrength;

            if (mShakeTimer >= mShakeDuration) {
                mIsShaking = false;
                shakeOffsetX = 0;
                shakeOffsetY = 0;
            }
        }

        mPos.x += shakeOffsetX * mCameraLerpSpeed * deltaTime;
        mPos.y += shakeOffsetY * mCameraLerpSpeed * deltaTime;

        // Reset flags
        mLookUp = false;
        mLookDown = false;

        return;
    }

    if (mTransitionTimer < mTransitionDuration) {
        mTransitionTimer += deltaTime;
        mCameraLerpSpeed = mSlowTransitionSpeed;
    }
    else {
        mCameraLerpSpeed = mNormalSpeed;
    }

    Vector2 targetPosition = Vector2::Zero;
    switch (mCameraMode) {
        case CameraMode::PanoramicCamera:
            targetPosition = PanoramicCamera(deltaTime);
            break;

        default:
            targetPosition = Vector2::Zero;
            break;
    }

    // Aplica deslocamento vertical se estiver olhando para cima ou para baixo
    if (mLookUp) {
        targetPosition.y -= mDistMove;
    }
    else if (mLookDown) {
        targetPosition.y += mDistMove;
    }

    if (mCameraMode == CameraMode::PanoramicCamera) {
        mPos = targetPosition;
    }

    // Camera Shake
    if (mShakeTimer < mShakeDuration) {
        mShakeTimer += deltaTime;
    }

    float shakeOffsetX = 0;
    float shakeOffsetY = 0;

    if (mIsShaking) {
        shakeOffsetX = Random::GetFloat() * (2 * mShakeStrength + 1) - mShakeStrength;
        shakeOffsetY = Random::GetFloat() * (2 * mShakeStrength + 1) - mShakeStrength;

        if (mShakeTimer >= mShakeDuration) {
            mIsShaking = false;
            shakeOffsetX = 0;
            shakeOffsetY = 0;
        }
    }
    mPos.x += shakeOffsetX * mCameraLerpSpeed * deltaTime;
    mPos.y += shakeOffsetY * mCameraLerpSpeed * deltaTime;

    // Reset flags para o próximo frame
    mLookUp = false;
    mLookDown = false;
}

Vector2 Camera::PanoramicCamera(float deltaTime) {
    Vector2 targetPos = GetPosCamera();
    targetPos.x = mPos.x + mCameraVelocity.x * deltaTime;
    targetPos.y = mPos.y + mCameraVelocity.y * deltaTime;
    return targetPos;
}

void Camera::SetZoom(float zoom)
{
    // Limita o zoom para evitar valores inválidos
    if (zoom < 0.01f) {
        zoom = 0.01f;
    }

    if (mZoom == zoom) {
        return;
    }

    float oldWidth = mGame->GetRenderer()->GetZoomedWidth();
    float oldHeight = mGame->GetRenderer()->GetZoomedHeight();

    mZoom = zoom;
    mGame->GetRenderer()->SetZoom(mZoom);

    float newWidth = mGame->GetRenderer()->GetZoomedWidth();
    float newHeight = mGame->GetRenderer()->GetZoomedHeight();

    mPos.x += (oldWidth - newWidth) * 0.5f;
    mPos.y += (oldHeight - newHeight) * 0.5f;
}
