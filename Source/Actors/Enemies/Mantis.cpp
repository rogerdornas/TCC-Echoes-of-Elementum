//
// Created by roger on 20/06/2025.
//

#include "Mantis.h"
#include "../Actor.h"
#include "../Effect.h"
#include "../ParticleSystem.h"
#include "../../Game.h"
#include "../../Components/RigidBodyComponent.h"
#include "../../Components/AABBComponent.h"
#include "../../Components/Drawing/AnimatorComponent.h"
#include "../../Components/Drawing/RectComponent.h"
#include "../../Components/CombatBoxComponent.h"

Mantis::Mantis(Game *game)
    :Enemy(game)
    ,mMantisState(State::WalkForward)
    ,mDistToSpotPlayer(400)
    ,mWalkingAroundDuration(3.0f)
    ,mWalkingAroundTimer(0.0f)
    ,mWalkingAroundMoveSpeed(70)
    ,mGravity(3000)
    ,mDistToAttack(450.0f)
    ,mJumpForce(-700.0f)
    ,mAttackDuration(2.0f)
    ,mAttackTimer(0.0f)
    ,mWalkBackDuration(0.5f)
    ,mWalkBackTimer(0.0f)
    ,mWaitToAttackDuration(0.3f)
    ,mWaitToAttackTimer(0.0f)
{
    mWidth = 60;
    mHeight = 120;
    mMoveSpeed = 250;
    mHealthPoints = 50;
    mMaxHealthPoints = mHealthPoints;
    mContactDamage = 25;
    mMoneyDrop = 10;
    mKnockBackSpeed = 700.0f;
    mKnockBackDuration = 0.1f;
    mKnockBackTimer = mKnockBackDuration;

    SetSize(mWidth, mHeight);

    mDrawComponent = new AnimatorComponent(this, "../Assets/Sprites/Mantis/Mantis.png",
                                                    "../Assets/Sprites/Mantis/Mantis.json",
                                                    mWidth * 2.0f * 1.35f, mHeight * 1.35f, 998);
    std::vector walk = {8, 9, 10, 11};
    mDrawComponent->AddAnimation("walk", walk);

    std::vector attack = {1, 2, 3, 4, 5, 6, 7};
    mDrawComponent->AddAnimation("attack", attack);

    std::vector hit = {0};
    mDrawComponent->AddAnimation("hit", hit);

    mDrawComponent->SetAnimation("walk");
    mDrawComponent->SetAnimFPS(7.0f);

    mCombatBoxComponent->AddAABBBox("claw", true, Vector2(mWidth * -0.4f, mHeight * -0.3f), Vector2(mWidth * 0.4f, mHeight * 0.3f));
    mCombatBoxComponent->SetBoxActive("claw", false);
    mCombatBoxComponent->SetBoxOffset("hitbox", Vector2(0, mHeight * 0.1f));
    mCombatBoxComponent->SetBoxOffset("hurtbox", Vector2(0, mHeight * 0.1f));
}

void Mantis::OnUpdate(float deltaTime) {
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
    ManageStun(deltaTime);

    if (!mIsFrozen && !mIsStunned) {
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

    // Se morreu
    if (Died()) {
    }

    if (!mIsFrozen && !mIsStunned) {
        if (mDrawComponent) {
            ManageAnimations();
        }
    }

    if (mCombatBoxComponent) {
        ManageCombatBox();
    }
}

void Mantis::MovementBeforePlayerSpotted() {
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
    if (Math::Abs(GetPosition().y - player->GetPosition().y) < 40) { // Se está no mesmo nível verticalmente
        if (player->GetPosition().x < GetPosition().x && GetForward().x < 0 && Math::Abs(player->GetPosition().x - GetPosition().x) < mDistToSpotPlayer) {
            mPlayerSpotted = true;
        }
        else if (player->GetPosition().x > GetPosition().x && GetForward().x > 0 && Math::Abs(player->GetPosition().x - GetPosition().x) < mDistToSpotPlayer) {
            mPlayerSpotted = true;
        }
    }
}

void Mantis::MovementAfterPlayerSpotted(float deltaTime) {
    switch (mMantisState) {
        case State::WalkForward:
            WalkForward(deltaTime);
            break;

        case State::WalkBack:
            WalkBack(deltaTime);
            break;

        case State::Attack:
            Attack(deltaTime);
            break;
    }
}

void Mantis::WalkForward(float deltaTime) {
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
        mWaitToAttackTimer += deltaTime;
        if (mKnockBackTimer >= mKnockBackDuration) {
            mRigidBodyComponent->SetVelocity(Vector2(0, mRigidBodyComponent->GetVelocity().y));
        }
        if (mWaitToAttackTimer >= mWaitToAttackDuration) {
            mWaitToAttackTimer = 0;
            mRigidBodyComponent->SetVelocity(Vector2(GetForward().x * mMoveSpeed * 4, mJumpForce));
            mMantisState = State::Attack;
            // if (mDrawAnimatedComponent) {
            //     mDrawAnimatedComponent->ResetAnimationTimer();
            // }
            mGame->GetAudio()->PlaySound("Jump/Jump1.wav");
        }
    }
    else {
        mWaitToAttackTimer = 0;
    }
}

void Mantis::WalkBack(float deltaTime) {
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
        mRigidBodyComponent->SetVelocity(Vector2(-GetForward().x * mMoveSpeed * 2, mRigidBodyComponent->GetVelocity().y));
    }

    mWalkBackTimer += deltaTime;
    if (mWalkBackTimer >= mWalkBackDuration) {
        mWalkBackTimer = 0;
        mMantisState = State::WalkForward;
    }
}

void Mantis::Attack(float deltaTime) {
    mAttackTimer += deltaTime;
    if (mAttackTimer >= mAttackDuration) {
        mAttackTimer = 0;
        mMantisState = State::WalkBack;
    }
    if (mKnockBackTimer >= mKnockBackDuration) {
        mRigidBodyComponent->SetVelocity(Vector2(GetForward().x * mMoveSpeed * 4, mRigidBodyComponent->GetVelocity().y));
    }
    if (mRigidBodyComponent->GetVelocity().y == 0) {
        mAttackTimer = 0;
        mMantisState = State::WalkBack;
    }
}

void Mantis::ManageAnimations() {
    if (mIsFlashing) {
        mDrawComponent->SetAnimation("hit");
    }
    else if (mMantisState == State::Attack) {
        mDrawComponent->SetAnimFPS(10.0f);
        mDrawComponent->SetAnimation("attack");
    }
    else {
        mDrawComponent->SetAnimation("walk");
    }
}

void Mantis::ManageCombatBox() {
    if (mMantisState == State::Attack) {
        if (mAttackTimer > 0.15f * mAttackDuration) {
            mCombatBoxComponent->SetBoxActive("claw", true);
            mCombatBoxComponent->SetBoxOffset("claw", Vector2(mWidth * 0.9f, mHeight * 0.2f) * Vector2(GetForward().x, 1));
        }
        else {
            mCombatBoxComponent->SetBoxActive("claw", false);
        }
    }
    else {
        mCombatBoxComponent->SetBoxActive("claw", false);
    }
}
