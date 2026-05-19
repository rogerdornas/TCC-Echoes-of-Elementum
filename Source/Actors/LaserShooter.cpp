//
// Created by roger on 07/04/2026.
//

#include "LaserShooter.h"

#include "Laser.h"
#include "ParticleSystem.h"
#include "../Game.h"
#include "../Components/Drawing/AnimatorComponent.h"

LaserShooter::LaserShooter(Game *game)
    :Actor(game)
    ,mLaserShootState(State::Idle)
    ,mWidth(110.0f)
    ,mHeight(110.0f)

    ,mContinuousShooting(true)
    ,mShotsRemaining(0)

    ,mIdleDuration(0.5f)
    ,mIdleTimer(0.0f)

    ,mLaserCastDuration(1.5f)
    ,mLaserCastTimer(0.0f)
    ,mLaserTargetTime(0.5f)
    ,mLaserTargetAngle(0.0f)
    ,mAlreadySetLaserAngle(false)

    ,mLaserShootDuration(1.0f)
    ,mLaserShootTimer(0.0f)
    ,mLaserHeight(mHeight * 0.5f)
    ,mLaserDamage(15.0f)
    ,mLaserOffset(mWidth * 0.04f, mHeight * -0.04f)
    ,mAlreadyLaserShoot(false)

    ,mDrawComponent(nullptr)
{
    mDrawComponent = new AnimatorComponent(this, "../Assets/Sprites/LaserShooter/LaserShooter.png",
                                            "../Assets/Sprites/LaserShooter/LaserShooter.json",
                                            mWidth, mHeight, 95);

    std::vector<int> idle = {0};
    mDrawComponent->AddAnimation("idle", idle);

    std::vector<int> laserCast = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
    mDrawComponent->AddAnimation("laserCast", laserCast);

    std::vector<int> laserShoot = {13, 0};
    mDrawComponent->AddAnimation("laserShoot", laserShoot);

    mDrawComponent->SetAnimation("idle");
    mDrawComponent->SetAnimFPS(17.0f);

    mGame->AddLaserShooter(this);
}

LaserShooter::~LaserShooter() {
    mGame->RemoveLaserShooter(this);
}

void LaserShooter::OnUpdate(float deltaTime) {
    switch (mLaserShootState) {
        case State::Idle:
            Idle(deltaTime);
        break;

        case State::LaserCast:
            LaserCast(deltaTime);
        break;

        case State::LaserShoot:
            LaserShoot(deltaTime);
        break;
    }
    ManageAnimations();
}

void LaserShooter::Destroy() {
    SetState(ActorState::Destroy);

    auto* smoke = new ParticleSystem(mGame, Particle::ParticleType::BlurParticle, 80.0f * 1.2f, 80.0f, 25.0f, 0.35f, 0.3f);
    smoke->SetParticleColor(SDL_Color{12, 101, 169, 50});
    smoke->SetConeSpread(360.0f);
    smoke->SetParticleSpeedScale(0.2f);
    smoke->SetParticleGravity(false);
    smoke->SetEmitDirection(Vector2::Zero);
    smoke->SetPosition(GetPosition());
    smoke->SetGroundCollision(false);
}

void LaserShooter::Idle(float deltaTime) {
    if (!mContinuousShooting && mShotsRemaining <= 0) {
        return;
    }

    mIdleTimer += deltaTime;
    if (mIdleTimer >= mIdleDuration) {
        mIdleTimer = 0.0f;
        mLaserShootState = State::LaserCast;
        return;
    }
}

void LaserShooter::LaserCast(float deltaTime) {
    mLaserCastTimer += deltaTime;
    if (mLaserCastTimer >= mLaserCastDuration) {
        mLaserCastTimer = 0.0f;
        mAlreadySetLaserAngle = false;
        mLaserShootState = State::LaserShoot;
        return;
    }

    if (!mAlreadySetLaserAngle && mLaserCastTimer >= mLaserCastDuration * mLaserTargetTime) {
        Player* player = GetGame()->GetPlayer();
        Vector2 headPosition = GetPosition() + Vector2(mLaserOffset.x * GetForward().x, mLaserOffset.y);

        float dx = player->GetPosition().x - headPosition.x;
        float dy = player->GetPosition().y - headPosition.y;

        float angle = Math::Atan2(dy, dx);
        // Ajustar para intervalo [0, 2*pi)
        if (angle < 0) {
            angle += 2 * Math::Pi;
        }

        mLaserTargetAngle = angle;

        auto* laser = new Laser(mGame, mLaserHeight * 0.25f, mLaserCastDuration * 1.05f - (mLaserTargetTime * mLaserCastDuration), mLaserDamage);
        laser->SetStartPosition(headPosition);
        laser->SetStartPositionOffset(mHeight * 0.36f);
        laser->SetPlayerCollision(false);
        laser->ShowLaserExplosion(false);
        laser->SetRotation(GetRotation());
        laser->SetTransformRotation(GetRotation());
        laser->CropFromEnd();
        laser->Initialize();

        mAlreadySetLaserAngle = true;
    }
}

void LaserShooter::LaserShoot(float deltaTime) {
    mLaserShootTimer += deltaTime;
    if (mLaserShootTimer >= mLaserShootDuration) {
        if (!mContinuousShooting) {
            mShotsRemaining--;
        }
        mLaserShootTimer = 0;
        mAlreadyLaserShoot = false;
        mLaserShootState = State::Idle;
        return;
    }

    if (!mAlreadyLaserShoot) {
        Vector2 headPosition = GetPosition() + Vector2(mLaserOffset.x * GetForward().x, mLaserOffset.y);

        auto* laser = new Laser(mGame, mLaserHeight, mLaserShootDuration, mLaserDamage);
        laser->SetStartPosition(headPosition);
        laser->ShowLaserExplosion(true);
        laser->SetRotation(GetRotation());
        laser->SetTransformRotation(GetRotation());
        laser->Initialize();

        mAlreadyLaserShoot = true;
    }
}

void LaserShooter::ManageAnimations() {
    if (mLaserShootState == State::Idle) {
        mDrawComponent->SetAnimation("idle");
        mDrawComponent->SetAnimFPS(1.0f);
    }
    else if (mLaserShootState == State::LaserCast) {
        mDrawComponent->SetAnimation("laserCast");
        mDrawComponent->SetAnimFPS(12.0f / mLaserCastDuration);
    }
    else if (mLaserShootState == State::LaserShoot) {
        mDrawComponent->SetAnimation("laserShoot");
        mDrawComponent->SetAnimFPS(2.0f / mLaserShootDuration);
    }
}