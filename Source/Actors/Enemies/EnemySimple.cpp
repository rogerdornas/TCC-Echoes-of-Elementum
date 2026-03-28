//
// Created by roger on 02/05/2025.
//

#include "EnemySimple.h"
#include "../Actor.h"
#include "../Effect.h"
#include "../ParticleSystem.h"
#include "../../Game.h"
#include "../../Components/RigidBodyComponent.h"
#include "../../Components/AABBComponent.h"
#include "../../Components/Drawing/AnimatorComponent.h"
#include "../../Components/Drawing/RectComponent.h"


EnemySimple::EnemySimple(Game* game)
    :Enemy(game)
    ,mDistToSpotPlayer(400)
    ,mPatrolRadius(50)
    ,mWalkingAroundDuration(2.0f)
    ,mWalkingAroundTimer(0.0f)
    ,mWalkingAroundMoveSpeed(50.0f)
    ,mGravity(3000)
{
    mWidth = 53;
    mHeight = 45;
    mMoveSpeed = 200;
    mHealthPoints = 30;
    mMaxHealthPoints = mHealthPoints;
    mContactDamage = 10;
    mMoneyDrop = 2;
    mKnockBackSpeed = 800.0f;
    mKnockBackDuration = 0.15f;
    mKnockBackTimer = mKnockBackDuration;

    SetSize(mWidth, mHeight);

    mDrawComponent = new AnimatorComponent(this, "../Assets/Sprites/Slime/Slime.png",
                                            "../Assets/Sprites/Slime/Slime.json",
                                            mWidth * 1.5f, mHeight * 1.5f, 998);
    std::vector walk = {0, 1, 2, 3, 4, 6};
    mDrawComponent->AddAnimation("walk", walk);

    std::vector hit = {5};
    mDrawComponent->AddAnimation("hit", hit);

    mDrawComponent->SetAnimation("walk");
    mDrawComponent->SetAnimFPS(8.0f);
}

void EnemySimple::OnUpdate(float deltaTime) {
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
                mDrawComponent->SetAnimFPS(15.0f);
            }
            MovementAfterPlayerSpotted();
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
}

void EnemySimple::MovementAfterPlayerSpotted() {
    Player* player = GetGame()->GetPlayer();
    float playerX = player->GetPosition().x;
    float enemyX = GetPosition().x;

    // Verifica se o inimigo passou dos limites e deve inverter a direção
    if (enemyX < playerX - mPatrolRadius) {
        SetRotation(0.0); // anda para direita
        SetScale(Vector2(1, 1));
    }
    else if (enemyX > playerX + mPatrolRadius) {
        SetRotation(Math::Pi); // anda para esquerda
        SetScale(Vector2(-1, 1));
    }

    if (mKnockBackTimer >= mKnockBackDuration) {
        mRigidBodyComponent->SetVelocity(Vector2(GetForward().x * mMoveSpeed, mRigidBodyComponent->GetVelocity().y));
    }
}

void EnemySimple::MovementBeforePlayerSpotted() {
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

void EnemySimple::ManageAnimations() {
    if (mIsFlashing) {
        mDrawComponent->SetAnimation("hit");
    }
    else {
        mDrawComponent->SetAnimation("walk");
    }
}
