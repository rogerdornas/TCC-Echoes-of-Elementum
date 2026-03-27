//
// Created by roger on 19/02/2026.
//

#include "OrangeSlime.h"
#include "../Actor.h"
#include "../../Game.h"
#include "../../Random.h"
#include "../../Components/RigidBodyComponent.h"
#include "../../Components/Drawing/AnimatorComponent.h"
#include "../../Components/CombatBoxComponent.h"

OrangeSlime::OrangeSlime(Game *game)
    :Enemy(game)
    ,mEnemyState(State::Stop)

    ,mHitDuration(0.07f)

    ,mDistToSpotPlayer(400)

    ,mBeforeSpotPlayerStopDuration(1.0f)
    ,mBeforeSpotPlayerStopTimer(0.0f)

    ,mAttackNextMove(false)
    ,mAttackProbability(0.5f)

    ,mStopDuration(0.5f)
    ,mStopTimer(0.0f)

    ,mAlreadyJump(false)
    ,mJumpForce(-1000.0f)
    ,mJumpStartDuration(0.35f)
    ,mJumpStartTimer(0.0f)
    ,mGravity(3000.0f)

    ,mDistToRunJump(700.0f)

    ,mDistToAttack(90.0f)
    ,mAttackDuration(0.9f)
    ,mAttackTimer(0.0f)
{
    mWidth = 50;
    mHeight = 50;
    mMoveSpeed = 550;
    mHealthPoints = 70;
    mMaxHealthPoints = mHealthPoints;
    mContactDamage = 15;
    mMoneyDrop = 28;
    mKnockBackSpeed = 700.0f;
    mKnockBackDuration = 0.1f;
    mKnockBackTimer = mKnockBackDuration;
    mIdleWidth = mWidth;
    mIdleHeight = mHeight;
    mHorizontalAttackSpriteWidth = mWidth * 1.9f;
    mHorizontalAttackSpriteHeight = mHeight * 0.7f;
    mVerticalAttackSpriteWidth = mWidth * 0.8f;
    mVerticalAttackSpriteHeight = mHeight * 2.0f;
    mHorizontalAttackOffsetHitBox = Vector2(mWidth * 0.8f, mHeight * 0.2f);
    mVerticalAttackOffsetHitBox = Vector2(mWidth * 0.0f, mHeight * -0.2f);

    SetSize(mWidth, mHeight);

    mDrawComponent = new AnimatorComponent(this, "../Assets/Sprites/OrangeSlime3/OrangeSlime.png",
                                                    "../Assets/Sprites/OrangeSlime3/OrangeSlime.json",
                                                    mWidth * 4.0f, mWidth * 4.0f * 0.9f, 998);

    std::vector run = {27, 28, 29, 30, 31, 32};
    mDrawComponent->AddAnimation("run", run);

    std::vector idle = {47, 13, 14, 15, 48, 16, 50};
    mDrawComponent->AddAnimation("idle", idle);

    std::vector attack = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 44};
    mDrawComponent->AddAnimation("attack", attack);

    std::vector runJump = {33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43};
    mDrawComponent->AddAnimation("runJump", runJump);

    std::vector idleJump = {49, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26};
    mDrawComponent->AddAnimation("idleJump", idleJump);

    std::vector hit = {45, 11, 12, 46};
    mDrawComponent->AddAnimation("hit", hit);

    mDrawComponent->SetAnimation("idle");
    mDrawComponent->SetAnimFPS(10.0f);
}

void OrangeSlime::OnUpdate(float deltaTime) {
    mKnockBackTimer += deltaTime;

    if (mFlashTimer < mHitDuration) {
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

void OrangeSlime::MovementBeforePlayerSpotted(float deltaTime) {
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

    float currentYVel = mRigidBodyComponent->GetVelocity().y;

    if (mIsOnGround && mAlreadyJump) {
        mEnemyState = State::Stop;
        mBeforeSpotPlayerStopTimer += deltaTime;
        if (mBeforeSpotPlayerStopTimer >= mBeforeSpotPlayerStopDuration) {
            mBeforeSpotPlayerStopTimer = Random::GetFloatRange(0, mBeforeSpotPlayerStopDuration * 0.7f);
            mAlreadyJump = false;
            mJumpStartTimer = 0;
            SetRotation(Math::Abs(GetRotation() - Math::Pi)); // Comuta rotação entre 0 e Pi
            SetScale(Vector2(GetScale().x * -1, 1));
        }
    }

    if (mIsOnGround && !mAlreadyJump) {
        mEnemyState = State::IdleJump;
        mJumpStartTimer += deltaTime;
        if (mJumpStartTimer >= mJumpStartDuration) {
            mRigidBodyComponent->SetVelocity(Vector2(0, mJumpForce));
            mAlreadyJump = true;
        }
    }

    // Lógica de "Game Feel" para o pulo
    if (mAlreadyJump) {
        if (currentYVel < 0) {
            // Se estiver quase parando (ápice), diminui a gravidade para flutuar
            if (std::abs(currentYVel) < 130.0f) {
                mGravity = 700.0f;
            } else {
                mGravity = 3000.0f;
            }
        }
        else if (currentYVel > 0) { // Caindo
            // Aumenta a gravidade para uma queda rápida e pesada
            mGravity = 3400.0f;
        }
    }

    if (mKnockBackTimer >= mKnockBackDuration) {
        mRigidBodyComponent->SetVelocity(Vector2(0, mRigidBodyComponent->GetVelocity().y));
    }
}

void OrangeSlime::MovementAfterPlayerSpotted(float deltaTime) {
    switch (mEnemyState) {
        case State::Stop:
            Stop(deltaTime);
        break;

        case State::IdleJump:
            IdleJump(deltaTime);
        break;

        case State::Run:
            Run(deltaTime);
        break;

        case State::RunJump:
            RunJump(deltaTime);
        break;

        case State::Attack:
            Attack(deltaTime);
        break;
    }
}

void OrangeSlime::Stop(float deltaTime) {
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
    if (mStopTimer >= mStopDuration && mIsOnGround) {
        mStopTimer = 0;
        mEnemyState = State::Run;
    }
}

void OrangeSlime::IdleJump(float deltaTime) {
    mKnockBackYActive = false;
    float currentYVel = mRigidBodyComponent->GetVelocity().y;

    if (mIsOnGround && mAlreadyJump) {
        if (Random::GetFloat() < mAttackProbability) {
            mAttackNextMove = true;
        }
        else {
            mAttackNextMove = false;
        }
        mEnemyState = State::Stop;
        mAlreadyJump = false;
        mJumpStartTimer = 0.0f;
        mGravity = 3000.0f;
        mKnockBackYActive = true;
        return;
    }

    if (mIsOnGround) {
        mJumpStartTimer += deltaTime;
        if (mJumpStartTimer >= mJumpStartDuration) {
            mRigidBodyComponent->SetVelocity(Vector2(0, mJumpForce));
            mAlreadyJump = true;
        }
    }

    // Lógica de "Game Feel" para o pulo
    if (mAlreadyJump) {
        if (currentYVel < 0) {
            // Se estiver quase parando (ápice), diminui a gravidade para flutuar
            if (std::abs(currentYVel) < 130.0f) {
                mGravity = 700.0f;
            } else {
                mGravity = 3000.0f;
            }
        }
        else if (currentYVel > 0) { // Caindo
            // Aumenta a gravidade para uma queda rápida e pesada
            mGravity = 3400.0f;
        }
    }

    if (mKnockBackTimer >= mKnockBackDuration) {
        mRigidBodyComponent->SetVelocity(Vector2(0, mRigidBodyComponent->GetVelocity().y));
    }
}

void OrangeSlime::Run(float deltaTime) {
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

    if (mAttackNextMove) {
        if (Math::Abs(dist) < mDistToAttack) {
            mEnemyState = State::Attack;
        }
    }
    else {
        if (Math::Abs(dist) < mDistToRunJump) {
            mEnemyState = State::RunJump;
        }
    }
}

void OrangeSlime::RunJump(float deltaTime) {
    mKnockBackYActive = false;
    float currentYVel = mRigidBodyComponent->GetVelocity().y;

    if (mIsOnGround && mAlreadyJump) {
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
        mEnemyState = State::IdleJump;
        mAlreadyJump = false;
        mJumpStartTimer = 0.0f;
        mGravity = 3000.0f;
        mKnockBackYActive = true;
        return;
    }

    if (mIsOnGround) {
        mJumpStartTimer += deltaTime;
        if (mJumpStartTimer >= mJumpStartDuration) {
            mRigidBodyComponent->SetVelocity(Vector2(GetForward().x * mMoveSpeed, mJumpForce));
            mAlreadyJump = true;
        }
    }

    // Lógica de "Game Feel" para o pulo
    if (mAlreadyJump) {
        if (currentYVel < 0) {
            // Se estiver quase parando (ápice), diminui a gravidade para flutuar
            if (std::abs(currentYVel) < 130.0f) {
                mGravity = 700.0f;
            } else {
                mGravity = 3000.0f;
            }
        }
        else if (currentYVel > 0) { // Caindo
            // Aumenta a gravidade para uma queda rápida e pesada
            mGravity = 3400.0f;
        }
    }

    if (mKnockBackTimer >= mKnockBackDuration) {
        mRigidBodyComponent->SetVelocity(Vector2(GetForward().x * mMoveSpeed, mRigidBodyComponent->GetVelocity().y));
    }
}

void OrangeSlime::Attack(float deltaTime) {
    mAttackTimer += deltaTime;

    if (mAttackTimer >= mAttackDuration) {
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
        mAttackTimer = 0;
        mEnemyState = State::IdleJump;
        return;
    }

    if (mKnockBackTimer >= mKnockBackDuration) {
        mRigidBodyComponent->SetVelocity(Vector2(0, mRigidBodyComponent->GetVelocity().y));
    }
}

void OrangeSlime::ManageAnimations() {
    if (mEnemyState == State::Attack) {
        mDrawComponent->SetAnimation("attack");
        mDrawComponent->SetAnimFPS(12.0f / mAttackDuration);
    }
    else if (mEnemyState == State::Stop) {
        mDrawComponent->SetAnimation("idle");
        mDrawComponent->SetAnimFPS(10);
    }
    else if (mEnemyState == State::IdleJump) {
        mDrawComponent->SetAnimation("idleJump");
        mDrawComponent->SetAnimFPS(9);
    }
    else if (mEnemyState == State::RunJump) {
        mDrawComponent->SetAnimation("runJump");
        mDrawComponent->SetAnimFPS(9);
    }
    else if (mEnemyState == State::Run) {
        mDrawComponent->SetAnimation("run");
        mDrawComponent->SetAnimFPS(10);
    }
    if (mIsFlashing) {
        mDrawComponent->SetTextureFactor(0.0f);
    }
    else {
        mDrawComponent->SetTextureFactor(1.0f);
    }
}

void OrangeSlime::ManageCombatBox() {
    if (mEnemyState == State::Attack) {
        if (mAttackTimer > 0.15f * mAttackDuration && mAttackTimer <= 0.3f * mAttackDuration) {
            mWidth = mHorizontalAttackSpriteWidth;
            mHeight = mHorizontalAttackSpriteHeight;
            mCombatBoxComponent->SetBoxHalfSize("hitbox", Vector2(mWidth / 2, mHeight / 2));
            mCombatBoxComponent->SetBoxHalfSize("hurtbox", Vector2(mWidth / 2, mHeight / 2));
            mCombatBoxComponent->SetBoxOffset("hitbox", mHorizontalAttackOffsetHitBox * Vector2(-GetForward().x, 1));
            mCombatBoxComponent->SetBoxOffset("hurtbox", mHorizontalAttackOffsetHitBox * Vector2(-GetForward().x, 1));
        }
        else if (mAttackTimer > 0.3f * mAttackDuration && mAttackTimer <= 0.4f * mAttackDuration) {
            mWidth = mVerticalAttackSpriteWidth;
            mHeight = mVerticalAttackSpriteHeight;
            mCombatBoxComponent->SetBoxHalfSize("hitbox", Vector2(mWidth / 2, mHeight / 2));
            mCombatBoxComponent->SetBoxHalfSize("hurtbox", Vector2(mWidth / 2, mHeight / 2));
            mCombatBoxComponent->SetBoxOffset("hitbox", mVerticalAttackOffsetHitBox);
            mCombatBoxComponent->SetBoxOffset("hurtbox", mVerticalAttackOffsetHitBox);
        }
        else if (mAttackTimer > 0.4f * mAttackDuration && mAttackTimer <= 0.8f * mAttackDuration) {
            mWidth = mHorizontalAttackSpriteWidth;
            mHeight = mHorizontalAttackSpriteHeight;
            mCombatBoxComponent->SetBoxHalfSize("hitbox", Vector2(mWidth / 2, mHeight / 2));
            mCombatBoxComponent->SetBoxHalfSize("hurtbox", Vector2(mWidth / 2, mHeight / 2));
            mCombatBoxComponent->SetBoxOffset("hitbox", mHorizontalAttackOffsetHitBox * Vector2(GetForward().x, 1));
            mCombatBoxComponent->SetBoxOffset("hurtbox", mHorizontalAttackOffsetHitBox * Vector2(GetForward().x, 1));
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
