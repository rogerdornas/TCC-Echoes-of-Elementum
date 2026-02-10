//
// Created by roger on 07/10/2025.
//

#include "Snake.h"
#include "Actor.h"
#include "../Game.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/AABBComponent.h"
#include "../Components/Drawing/AnimatorComponent.h"
#include "../Components/Drawing/RectComponent.h"
#include "../Components/CombatBoxComponent.h"

Snake::Snake(Game *game)
    :Enemy(game)
    ,mSnakeState(State::Stop)
    ,mStopDuration(0.7f)
    ,mStopTimer(0.0f)
    ,mDistToSpotPlayer(400 * mGame->GetScale())
    ,mWalkingAroundDuration(3.0f)
    ,mWalkingAroundTimer(0.0f)
    ,mWalkingAroundMoveSpeed(70 * mGame->GetScale())
    ,mGravity(3000 * mGame->GetScale())
    ,mDistToAttack(300.0f * mGame->GetScale())
    ,mAttackDuration(0.7f)
    ,mAttackTimer(0.0f)
{
    mWidth = 100 * mGame->GetScale();
    mHeight = 25 * mGame->GetScale();
    mMoveSpeed = 300 * mGame->GetScale();
    mHealthPoints = 40;
    mMaxHealthPoints = mHealthPoints;
    mContactDamage = 15;
    mMoneyDrop = 7;
    mKnockBackSpeed = 700.0f * mGame->GetScale();
    mKnockBackDuration = 0.1f;
    mKnockBackTimer = mKnockBackDuration;
    mIdleWidth = mWidth;
    mIdleHeight = mHeight;
    mAttackSpriteWidth = mWidth * 1.6f;
    mAttackSpriteHeight = mHeight * 0.5f;
    mAttackOffsetHitBox = Vector2(mWidth * 0.4f, mHeight * 0.4f);
    mGrowthSpeed = 700.0f;
    mEnemyCollision = false;

    SetSize(mWidth, mHeight);

    mDrawComponent = new AnimatorComponent(this, "../Assets/Sprites/Snake/Snake.png",
                                                        "../Assets/Sprites/Snake/Snake.json",
                                                        mWidth * 2.5f, mWidth * 2.5f * 0.36f, 998);
    std::vector walk = {14, 10, 11, 12};
    mDrawComponent->AddAnimation("walk", walk);

    std::vector idle = {13, 7, 8, 9};
    mDrawComponent->AddAnimation("idle", idle);

    std::vector attack = {0, 1, 2, 3, 4, 5};
    mDrawComponent->AddAnimation("attack", attack);

    std::vector hit = {6};
    mDrawComponent->AddAnimation("hit", hit);

    mDrawComponent->SetAnimation("walk");
    mDrawComponent->SetAnimFPS(7.0f);
}

void Snake::OnUpdate(float deltaTime) {
    mKnockBackTimer += deltaTime;
    mWalkingAroundTimer += deltaTime;

    if (mFlashTimer < mFlashDuration) {
        mFlashTimer += deltaTime;
    }
    else {
        mIsFlashing = false;
    }

    ResolveGroundCollision();
    ResolveEnemyCollision();
    ManageFreezing(deltaTime);

    if (!mIsFrozen) {
        if (mPlayerSpotted) {
            if (mDrawComponent) {
                mDrawComponent->SetAnimFPS(10.0f);
            }
            MovementAfterPlayerSpotted(deltaTime);
        }
        else {
            MovementBeforePlayerSpotted();
        }
    }

    // Gravidade
    mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x,
                                             mRigidBodyComponent->GetVelocity().y
                                             + mGravity * deltaTime));

    if (mCombatBoxComponent) {
        ManageCombatBox();
    }

    // Se morreu
    if (Died()) {
    }

    if (!mIsFrozen) {
        if (mDrawComponent) {
            ManageAnimations();
        }
    }
}

void Snake::MovementBeforePlayerSpotted() {
    Player* player = GetGame()->GetPlayer();
    if (mWalkingAroundTimer > mWalkingAroundDuration) {
        SetRotation(Math::Abs(GetRotation() - Math::Pi)); // Comuta rotação entre 0 e Pi
        SetScale(Vector2(GetScale().x * -1, 1));
        mWalkingAroundTimer = 0;
    }
    if (mKnockBackTimer >= mKnockBackDuration) {
        mRigidBodyComponent->SetVelocity(Vector2(GetForward().x * mWalkingAroundMoveSpeed, mRigidBodyComponent->GetVelocity().y));
    }

    // Testa se spottou player
    if (Math::Abs(GetPosition().y - player->GetPosition().y) < 40 * mGame->GetScale()) { // Se está no mesmo nível verticalmente
        if (player->GetPosition().x < GetPosition().x && GetForward().x < 0 && Math::Abs(player->GetPosition().x - GetPosition().x) < mDistToSpotPlayer) {
            mPlayerSpotted = true;
        }
        else if (player->GetPosition().x > GetPosition().x && GetForward().x > 0 && Math::Abs(player->GetPosition().x - GetPosition().x) < mDistToSpotPlayer) {
            mPlayerSpotted = true;
        }
    }
}

void Snake::MovementAfterPlayerSpotted(float deltaTime) {
    switch (mSnakeState) {
        case State::WalkForward:
            WalkForward(deltaTime);
        break;

        case State::Stop:
            Stop(deltaTime);
        break;

        case State::Attack:
            Attack(deltaTime);
        break;
    }
}

void Snake::WalkForward(float deltaTime) {
    Player* player = GetGame()->GetPlayer();
    float dist = GetPosition().x - player->GetPosition().x;

    if (dist < 0) {
        SetRotation(0.0);
        SetScale(Vector2(1, 1));
    }
    else {
        SetRotation(Math::Pi);
        SetScale(Vector2(-1, 1));
    }
    if (mKnockBackTimer >= mKnockBackDuration) {
        mRigidBodyComponent->SetVelocity(Vector2(GetForward().x * mMoveSpeed, mRigidBodyComponent->GetVelocity().y));
    }

    if (Math::Abs(dist) < mDistToAttack) {
        mSnakeState = State::Attack;
        // if (mDrawAnimatedComponent) {
        //     mDrawAnimatedComponent->ResetAnimationTimer();
        // }
    }
}

void Snake::Stop(float deltaTime) {
    Player* player = GetGame()->GetPlayer();
    float dist = GetPosition().x - player->GetPosition().x;
    if (dist < 0) {
        SetRotation(0.0);
        SetScale(Vector2(1, 1));
    }
    else {
        SetRotation(Math::Pi);
        SetScale(Vector2(-1, 1));
    }

    if (mKnockBackTimer >= mKnockBackDuration) {
        mRigidBodyComponent->SetVelocity(Vector2(0, mRigidBodyComponent->GetVelocity().y));
    }
    mStopTimer += deltaTime;
    if (mStopTimer >= mStopDuration) {
        mStopTimer = 0;
        mSnakeState = State::WalkForward;
    }
}


void Snake::Attack(float deltaTime) {
    mAttackTimer += deltaTime;

    if (mAttackTimer >= mAttackDuration) {
        mAttackTimer = 0;
        mSnakeState = State::Stop;
        return;
    }

    if (mKnockBackTimer >= mKnockBackDuration) {
        mRigidBodyComponent->SetVelocity(Vector2(GetForward().x * mMoveSpeed * 2.0f, mRigidBodyComponent->GetVelocity().y));
    }
}

void Snake::ManageAnimations() {
    if (mSnakeState == State::Attack) {
        mDrawComponent->SetAnimation("attack");
    }
    else if (mIsFlashing) {
        mDrawComponent->SetAnimation("hit");
    }
    else if (mSnakeState == State::Stop) {
        mDrawComponent->SetAnimation("idle");
    }
    else if (mSnakeState == State::WalkForward) {
        mDrawComponent->SetAnimation("walk");
    }
}

void Snake::ManageCombatBox() {
    if (mSnakeState == State::Attack) {
        if (mAttackTimer > 0.45f * mAttackDuration && mAttackTimer < 0.8f * mAttackDuration) {
            mWidth = mAttackSpriteWidth;
            mHeight = mAttackSpriteHeight;
            mCombatBoxComponent->SetBoxHalfSize("hitbox", Vector2(mWidth / 2, mHeight / 2));
            mCombatBoxComponent->SetBoxHalfSize("hurtbox", Vector2(mWidth / 2, mHeight / 2));
            mCombatBoxComponent->SetBoxOffset("hitbox", mAttackOffsetHitBox * Vector2(GetForward().x, 1));
            mCombatBoxComponent->SetBoxOffset("hurtbox", mAttackOffsetHitBox * Vector2(GetForward().x, 1));
        }
        else {
            mWidth = mIdleWidth;
            mHeight = mIdleHeight;
            mCombatBoxComponent->SetBoxHalfSize("hitbox", Vector2(mWidth / 2, mHeight / 2));
            mCombatBoxComponent->SetBoxHalfSize("hurtbox", Vector2(mWidth / 2, mHeight / 2));
            mCombatBoxComponent->SetBoxOffset("hitbox", Vector2::Zero);
            mCombatBoxComponent->SetBoxOffset("hurtbox", Vector2::Zero);
        }
    }
}

void Snake::ChangeResolution(float oldScale, float newScale) {

}
