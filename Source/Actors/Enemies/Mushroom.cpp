//
// Created by roger on 07/10/2025.
//

#include "Mushroom.h"
#include "../Actor.h"
#include "../Effect.h"
#include "../ParticleSystem.h"
#include "../../Game.h"
#include "../../Components/RigidBodyComponent.h"
#include "../../Components/AABBComponent.h"
#include "../../Components/Drawing/AnimatorComponent.h"
#include "../../Components/Drawing/RectComponent.h"
#include "../../Components/CombatBoxComponent.h"

Mushroom::Mushroom(Game *game)
    :Enemy(game)
    ,mMushroomState(State::Stop)

    ,mStopDuration(0.7f)
    ,mStopTimer(0.0f)

    ,mHitDuration(0.3f)

    ,mDistToSpotPlayer(400)
    ,mLookingAroundDuration(2.5f)
    ,mLookingAroundTimer(0.0f)

    ,mWalkAwayDuration(0.4f)
    ,mWalkAwayTimer(0.0f)

    ,mGravity(3000)

    ,mDistToAttack(300.0f)
    ,mJumpForce(-600.0f)
    ,mAttackDuration(0.7f)
    ,mAttackTimer(0.0f)

    ,mStumDuration(1.2f)
    ,mStumTimer(0.0f)
{
    mWidth = 80;
    mHeight = 110;
    mMoveSpeed = 300;
    mHealthPoints = 45;
    mMaxHealthPoints = mHealthPoints;
    mContactDamage = 15;
    mMoneyDrop = 20;
    mKnockBackSpeed = 700.0f;
    mKnockBackDuration = 0.1f;
    mKnockBackTimer = mKnockBackDuration;
    mIdleWidth = mWidth;
    mIdleHeight = mHeight;
    mAttackWidth = 110;
    mAttackHeight = 80;
    mAttackOffset = Vector2(60, 15);

    SetSize(mWidth, mHeight);

    mDrawComponent = new AnimatorComponent(this, "../Assets/Sprites/Mushroom/Mushroom.png",
                                                    "../Assets/Sprites/Mushroom/Mushroom.json",
                                                    mWidth * 1.375f * 2.69f, mHeight * 2.69f, 998);

    std::vector run = {18, 19, 20, 21, 22, 23, 24, 25, };
    mDrawComponent->AddAnimation("run", run);

    std::vector idle = {13, 14, 15, 16, 47, 17, 46};
    mDrawComponent->AddAnimation("idle", idle);

    std::vector attack = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    mDrawComponent->AddAnimation("attack", attack);

    std::vector stun = {26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43};
    mDrawComponent->AddAnimation("stun", stun);

    std::vector hit = {44, 10, 11, 12, 45};
    mDrawComponent->AddAnimation("hit", hit);

    mDrawComponent->SetAnimation("idle");
    mDrawComponent->SetAnimFPS(10.0f);
}

void Mushroom::OnUpdate(float deltaTime) {
    mKnockBackTimer += deltaTime;

    if (mFlashTimer < mHitDuration) {
        if (mFlashTimer == 0 && mMushroomState != State::Attack) {
            // if (mDrawAnimatedComponent) {
            //     mDrawAnimatedComponent->ResetAnimationTimer();
            // }
        }
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
            MovementAfterPlayerSpotted(deltaTime);
        }
        else {
            MovementBeforePlayerSpotted(deltaTime);
        }
    }

    // Gravidade
    mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x,
                                             mRigidBodyComponent->GetVelocity().y
                                             + mGravity * deltaTime));

    if (mCombatBoxComponent) {
        ManageCombatBox();
    }

    if (Died()) {
    }

    if (!mIsFrozen && !mIsStunned) {
        if (mDrawComponent) {
            ManageAnimations();
        }
    }
}

void Mushroom::ReceiveHit(float damage, Vector2 knockBackDirection, bool knockBack) {
    if (knockBackDirection.y == 1 && mMushroomState != State::Attack) {
        if (IsOnScreen()) {
            mGame->GetAudio()->PlaySound("Boing/Boing.wav");
        }
        return;
    }

    mHealthPoints -= damage;
    if (knockBack) {
        mRigidBodyComponent->SetVelocity(mRigidBodyComponent->GetVelocity() + knockBackDirection * mKnockBackSpeed);
        mKnockBackTimer = 0;
    }
    mIsFlashing = true;
    mFlashTimer = 0;
    mPlayerSpotted = true;

    auto* blood = new ParticleSystem(mGame, Particle::ParticleType::SolidParticle, 10.0f * 1.2f, 10.0f, 170.0, 3.0, 0.07f);
    blood->SetPosition(GetPosition());
    blood->SetEmitDirection(knockBackDirection);
    blood->SetParticleSpeedScale(1);
    blood->SetParticleColor(SDL_Color{226, 90, 70, 255});
    blood->SetParticleGravity(true);
    blood->SetConeSpread(65.0f);

    auto* circleBlur = new Effect(mGame);
    circleBlur->SetDuration(0.3);
    circleBlur->SetSize((GetWidth() + GetHeight()) / 2 * 3.5f);
    circleBlur->SetEnemy(*this);
    circleBlur->SetColor(SDL_Color{226, 90, 70, 150});
    circleBlur->SetEffect(TargetEffect::Circle);

    if (IsOnScreen()) {
        mGame->GetAudio()->PlayVariantSound("HitEnemy/HitEnemy.wav", 4);
    }
}


void Mushroom::MovementBeforePlayerSpotted(float deltaTime) {
    mLookingAroundTimer += deltaTime;
    if (mLookingAroundTimer >= mLookingAroundDuration) {
        SetRotation(Math::Abs(GetRotation() - Math::Pi)); // Comuta rotação entre 0 e Pi
        SetScale(Vector2(GetScale().x * -1, 1));
        mLookingAroundTimer = 0;
    }

    // Testa se spottou player
    Player* player = GetGame()->GetPlayer();
    if (Math::Abs(GetPosition().y - player->GetPosition().y) < 40) { // Se está no mesmo nível verticalmente
        if (player->GetPosition().x < GetPosition().x && GetForward().x < 0 && Math::Abs(player->GetPosition().x - GetPosition().x) < mDistToSpotPlayer) {
            mPlayerSpotted = true;
        }
        else if (player->GetPosition().x > GetPosition().x && GetForward().x > 0 && Math::Abs(player->GetPosition().x - GetPosition().x) < mDistToSpotPlayer) {
            mPlayerSpotted = true;
        }
    }
}

void Mushroom::MovementAfterPlayerSpotted(float deltaTime) {
    switch (mMushroomState) {
        case State::Stop:
            Stop(deltaTime);
        break;

        case State::WalkForward:
            WalkForward(deltaTime);
        break;

        case State::WalkAway:
            WalkAway(deltaTime);
        break;

        case State::Attack:
            Attack(deltaTime);
        break;

        case State::StunState:
            StunState(deltaTime);
        break;
    }
}

void Mushroom::Stop(float deltaTime) {
    Player* player = GetGame()->GetPlayer();
    float dist = GetPosition().x - player->GetPosition().x;
    if (dist < 0) {
        SetRotation(0.0);
        SetScale(Vector2(1,1));
    }
    else {
        SetRotation(Math::Pi);
        SetScale(Vector2(-1,1));
    }

    if (mKnockBackTimer >= mKnockBackDuration) {
        mRigidBodyComponent->SetVelocity(Vector2(0, mRigidBodyComponent->GetVelocity().y));
    }
    mStopTimer += deltaTime;
    if (mStopTimer >= mStopDuration) {
        mStopTimer = 0;
        mMushroomState = State::WalkForward;
    }
}

void Mushroom::WalkForward(float deltaTime) {
    Player* player = GetGame()->GetPlayer();
    float dist = GetPosition().x - player->GetPosition().x;

    if (dist < 0) {
        SetRotation(0.0);
        SetScale(Vector2(1,1));
    }
    else {
        SetRotation(Math::Pi);
        SetScale(Vector2(-1,1));
    }
    if (mKnockBackTimer >= mKnockBackDuration) {
        mRigidBodyComponent->SetVelocity(Vector2(GetForward().x * mMoveSpeed * 2, mRigidBodyComponent->GetVelocity().y));
    }

    if (Math::Abs(dist) < mDistToAttack) {
        mRigidBodyComponent->SetVelocity(Vector2(GetForward().x * mMoveSpeed * 2.5f, mJumpForce));
        mMushroomState = State::Attack;
    }
}

void Mushroom::WalkAway(float deltaTime) {
    mWalkAwayTimer += deltaTime;
    if (mWalkAwayTimer >= mWalkAwayDuration) {
        mWalkAwayTimer = 0;
        mMushroomState = State::Stop;
        return;
    }

    Player* player = GetGame()->GetPlayer();
    float dist = GetPosition().x - player->GetPosition().x;

    if (dist > 0) {
        SetRotation(0.0);
        SetScale(Vector2(1,1));
    }
    else {
        SetRotation(Math::Pi);
        SetScale(Vector2(-1,1));
    }
    if (mKnockBackTimer >= mKnockBackDuration) {
        mRigidBodyComponent->SetVelocity(Vector2(GetForward().x * mMoveSpeed, mRigidBodyComponent->GetVelocity().y));
    }
}

void Mushroom::Attack(float deltaTime) {
    mAttackTimer += deltaTime;
    if (mAttackTimer >= mAttackDuration) {
        mAttackTimer = 0;
        mMushroomState = State::StunState;
        return;
    }

    if (mKnockBackTimer >= mKnockBackDuration) {
        if (mAttackTimer < 0.6f * mAttackDuration) {
            mRigidBodyComponent->SetVelocity(Vector2(GetForward().x * mMoveSpeed * 2.5f, mRigidBodyComponent->GetVelocity().y));
        }
        else {
            mRigidBodyComponent->SetVelocity(Vector2(0, mRigidBodyComponent->GetVelocity().y));
        }
    }
}

void Mushroom::StunState(float deltaTime) {
    mStumTimer += deltaTime;
    if (mStumTimer >= mStumDuration) {
        mStumTimer = 0;
        mMushroomState = State::WalkAway;
        return;
    }

    if (mKnockBackTimer >= mKnockBackDuration) {
        mRigidBodyComponent->SetVelocity(Vector2(0, mRigidBodyComponent->GetVelocity().y));
    }
}

void Mushroom::ManageAnimations() {
    if (mMushroomState == State::Attack) {
        mDrawComponent->SetAnimation("attack");
        mDrawComponent->SetAnimFPS(10.0f / mAttackDuration);
    }
    else if (mIsFlashing) {
        mDrawComponent->SetAnimation("hit");
        mDrawComponent->SetAnimFPS(5.0f / mHitDuration);
    }
    else if (mMushroomState == State::WalkForward) {
        mDrawComponent->SetAnimation("run");
        mDrawComponent->SetAnimFPS(12);
    }
    else if (mMushroomState == State::WalkAway) {
        mDrawComponent->SetAnimation("run");
        mDrawComponent->SetAnimFPS(8);
    }
    else if (mMushroomState == State::StunState) {
        mDrawComponent->SetAnimation("stun");
        mDrawComponent->SetAnimFPS(12);
    }
    else if (mMushroomState == State::Stop) {
        mDrawComponent->SetAnimation("idle");
        mDrawComponent->SetAnimFPS(12);
    }
}

void Mushroom::ManageCombatBox() {
    if (mMushroomState == State::Attack) {
        if (mAttackTimer < 0.2f * mAttackDuration) {
            mWidth = mIdleWidth;
            mHeight = mIdleHeight;
            mCombatBoxComponent->SetBoxHalfSize("hitbox", Vector2(mWidth / 2, mHeight / 2));
            mCombatBoxComponent->SetBoxHalfSize("hurtbox", Vector2(mWidth / 2, mHeight / 2));
            mCombatBoxComponent->SetBoxOffset("hitbox", Vector2::Zero);
            mCombatBoxComponent->SetBoxOffset("hurtbox", Vector2::Zero);
        }
        if (mAttackTimer > 0.2f * mAttackDuration && mAttackTimer < 0.4f * mAttackDuration) {
            mWidth = mAttackWidth;
            mHeight = mAttackHeight;
            mCombatBoxComponent->SetBoxHalfSize("hitbox", Vector2(mWidth / 2, mHeight / 2));
            mCombatBoxComponent->SetBoxHalfSize("hurtbox", Vector2(mWidth / 2, mHeight / 2));
            mCombatBoxComponent->SetBoxOffset("hitbox", mAttackOffset * Vector2(-GetForward().x, 1));
            mCombatBoxComponent->SetBoxOffset("hurtbox", mAttackOffset * Vector2(-GetForward().x, 1));
        }
        if (mAttackTimer > 0.4f * mAttackDuration && mAttackTimer < 0.5f * mAttackDuration) {
            mWidth = mIdleWidth;
            mHeight = mIdleHeight;
            mCombatBoxComponent->SetBoxHalfSize("hitbox", Vector2(mWidth / 2, mHeight / 2));
            mCombatBoxComponent->SetBoxHalfSize("hurtbox", Vector2(mWidth / 2, mHeight / 2));
            mCombatBoxComponent->SetBoxOffset("hitbox", Vector2::Zero);
            mCombatBoxComponent->SetBoxOffset("hurtbox", Vector2::Zero);
        }
        if (mAttackTimer > 0.5f * mAttackDuration && mAttackTimer < 0.85f * mAttackDuration) {
            mWidth = mAttackWidth;
            mHeight = mAttackHeight;
            mCombatBoxComponent->SetBoxHalfSize("hitbox", Vector2(mWidth / 2, mHeight / 2));
            mCombatBoxComponent->SetBoxHalfSize("hurtbox", Vector2(mWidth / 2, mHeight / 2));
            mCombatBoxComponent->SetBoxOffset("hitbox", mAttackOffset * Vector2(GetForward().x, 1));
            mCombatBoxComponent->SetBoxOffset("hurtbox", mAttackOffset * Vector2(GetForward().x, 1));
        }
        if (mAttackTimer > 0.85f * mAttackDuration) {
            mWidth = mIdleWidth;
            mHeight = mIdleHeight;
            mCombatBoxComponent->SetBoxHalfSize("hitbox", Vector2(mWidth / 2, mHeight / 2));
            mCombatBoxComponent->SetBoxHalfSize("hurtbox", Vector2(mWidth / 2, mHeight / 2));
            mCombatBoxComponent->SetBoxOffset("hitbox", Vector2::Zero);
            mCombatBoxComponent->SetBoxOffset("hurtbox", Vector2::Zero);
        }
    }
    else {
        mCombatBoxComponent->SetBoxOffset("hitbox", Vector2(0, mHeight * 0.12f));
        mCombatBoxComponent->SetBoxOffset("hurtbox", Vector2(0, mHeight * 0.12f));
    }
}
