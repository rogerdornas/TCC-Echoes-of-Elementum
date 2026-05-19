//
// Created by roger on 21/09/2025.
//

#include "LittleBat.h"
#include "../Actor.h"
#include "../Effect.h"
#include "../ParticleSystem.h"
#include "../../Game.h"
#include "../../Components/RigidBodyComponent.h"
#include "../../Components/AABBComponent.h"
#include "../../Components/Drawing/AnimatorComponent.h"
#include "../../Components/Drawing/RectComponent.h"

LittleBat::LittleBat(Game *game)
    :Enemy(game)
    ,mDistToSpotPlayer(400)
    ,mFlyingAroundDuration(1.0f)
    ,mFlyingAroundTimer(0.0f)
    ,mFlyingAroundMoveSpeed(100.0f)
{
    mWidth = 24;
    mHeight = 24;
    mMoveSpeed = 500;
    mHealthPoints = 0.05f;
    mMaxHealthPoints = mHealthPoints;
    mContactDamage = 10;
    mMoneyDrop = 0;
    mKnockBackSpeed = 1200.0f;
    mKnockBackDuration = 0.2f;
    mKnockBackTimer = mKnockBackDuration;
    mEnemyCollision = false;

    SetSize(mWidth, mHeight);

    mDrawComponent = new AnimatorComponent(this, "../Assets/Sprites/SpawnFly/SpawnFly.png",
                                                    "../Assets/Sprites/SpawnFly/SpawnFly.json",
                                                    mWidth * 6.0f, mHeight * 6.0f, 999);
    std::vector fly = {0, 1, 2, 3, 4};
    mDrawComponent->AddAnimation("fly", fly);

    std::vector hit = {5};
    mDrawComponent->AddAnimation("hit", hit);

    mDrawComponent->SetAnimation("fly");
    mDrawComponent->SetAnimFPS(10.0f);
}

void LittleBat::OnUpdate(float deltaTime) {
    mKnockBackTimer += deltaTime;
    mFlyingAroundTimer += deltaTime;

    if (mFlashTimer < mFlashDuration) {
        mFlashTimer += deltaTime;
    }
    else {
        mIsFlashing = false;
    }

    ResolveGroundCollision();
    ResolveEnemyCollision();
    ManageFreezing(deltaTime);
    ManageStun(deltaTime);

    if (!mIsFrozen && !mIsStunned) {
        if (mPlayerSpotted) {
            if (mDrawComponent) {
                mDrawComponent->SetAnimFPS(14.0f);
            }
            MovementAfterPlayerSpotted(deltaTime);
        }
        else {
            MovementBeforePlayerSpotted();
        }
    }

    // Se morreu
    if (Died()) {
    }

    if (!mIsFrozen && !mIsStunned) {
        if (mDrawComponent) {
            ManageAnimations();
        }
    }
}

void LittleBat::MovementBeforePlayerSpotted() {
    Player *player = GetGame()->GetPlayer();
    if (mFlyingAroundTimer > mFlyingAroundDuration) {
        SetRotation(Math::Abs(GetRotation() - Math::Pi)); // Comuta rotação entre 0 e Pi
        SetScale(Vector2(GetScale().x * -1, 1));
        mFlyingAroundTimer = 0;
    }
    if (mKnockBackTimer >= mKnockBackDuration) {
        mRigidBodyComponent->SetVelocity(Vector2(GetForward() * mFlyingAroundMoveSpeed));
    }

    // Testa se spotted player
    Vector2 dist = GetPosition() - player->GetPosition();
    if (dist.Length() < mDistToSpotPlayer) {
        mPlayerSpotted = true;
    }
}

void LittleBat::MovementAfterPlayerSpotted(float deltaTime) {
    Player *player = GetGame()->GetPlayer();

    float dx = player->GetPosition().x - GetPosition().x;
    float dy = player->GetPosition().y - GetPosition().y;

    float angle = Math::Atan2(dy, dx);
    // Ajustar para intervalo [0, 2*pi)
    if (angle < 0) {
        angle += 2 * Math::Pi;
    }

    SetRotation(angle);

    if (mKnockBackTimer >= mKnockBackDuration) {
        mRigidBodyComponent->SetVelocity(GetForward() * mMoveSpeed);
    }
}

void LittleBat::ManageAnimations() {
    if (GetRotation() > Math::PiOver2 && GetRotation() < 3 * Math::PiOver2) {
        SetScale(Vector2(-1, 1));
        // mDrawAnimatedComponent->UseFlip(true);
        // mDrawAnimatedComponent->SetFlip(SDL_FLIP_HORIZONTAL);
    }
    else {
        SetScale(Vector2(1, 1));
        // mDrawAnimatedComponent->UseFlip(false);
    }

    if (mIsFlashing) {
        mDrawComponent->SetAnimation("hit");
    }
    else {
        mDrawComponent->SetAnimation("fly");
    }
}
