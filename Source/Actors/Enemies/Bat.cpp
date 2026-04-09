//
// Created by roger on 02/04/2026.
//

#include "Bat.h"
#include "../Actor.h"
#include "../../Game.h"
#include "../../Random.h"
#include "../../Components/RigidBodyComponent.h"
#include "../../Components/Drawing/AnimatorComponent.h"
#include "../../Components/CombatBoxComponent.h"

Bat::Bat(Game *game)
    :Enemy(game)
    ,mBatState(State::Sleep)

    ,mDistToSpotPlayer(300)

    ,mStopDuration(0.4f)
    ,mStopTimer(0.0f)

    ,mHitDuration(0.07f)

    ,mWakeUpDuration(1.0f)
    ,mWakeUpTimer(0.0f)

    ,mDistToAttack(300)
    ,mAttackDirectionRight(true)

    ,mAttackDuration(0.5f)
    ,mAttackTimer(0.0f)

    ,mBiteDuration(1.2f)
    ,mBiteTimer(0.0f)
    ,mDistToBiteY(120.0f)

    ,mRecoverDuration(1.5f)
    ,mRecoverTimer(0.0f)
    ,mFlightTime(0.0f)
{
    mWidth = 30;
    mHeight = 30;
    mMoveSpeed = 500;
    mHealthPoints = 80;
    mMaxHealthPoints = mHealthPoints;
    mContactDamage = 15;
    mMoneyDrop = 7;
    mKnockBackSpeed = 700.0f;
    mKnockBackDuration = 0.1f;
    mKnockBackTimer = mKnockBackDuration;
    mIdleWidth = mWidth;
    mAttackSpriteWidth = mWidth * 2.9f;
    mEnemyCollision = false;

    SetSize(mWidth, mHeight);

    mDrawComponent = new AnimatorComponent(this, "../Assets/Sprites/Bat/Bat.png",
                                                    "../Assets/Sprites/Bat/Bat.json",
                                                    mWidth * 8.5f, mHeight * 8.5f, 998);
    std::vector attack = {0, 1, 2, 3, 4, 5, 6, 7};
    mDrawComponent->AddAnimation("attack", attack);

    std::vector bite = {8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18};
    mDrawComponent->AddAnimation("bite", bite);

    std::vector hurt = {19, 20, 21, 22, 48};
    mDrawComponent->AddAnimation("hurt", hurt);

    std::vector idle = {49, 23, 24, 25, 26, 27, 28, 29, 30};
    mDrawComponent->AddAnimation("idle", idle);

    std::vector run = {31, 32, 33, 34, 35, 36, 37, 38};
    mDrawComponent->AddAnimation("run", run);

    std::vector sleep = {39};
    mDrawComponent->AddAnimation("sleep", sleep);

    std::vector wakeUp = {55, 40, 41, 42, 43, 44, 45, 46, 47, 50, 51, 52, 53, 54};
    mDrawComponent->AddAnimation("wakeUp", wakeUp);

    mDrawComponent->SetAnimation("sleep");
    mDrawComponent->SetAnimFPS(7.0f);
}

void Bat::OnUpdate(float deltaTime) {
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
        MovementAfterPlayerSpotted(deltaTime);
    }

    if (mCombatBoxComponent) {
        ManageCombatBox();
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

void Bat::MovementAfterPlayerSpotted(float deltaTime) {
    switch (mBatState) {
        case State::Sleep:
            Sleep(deltaTime);
        break;

        case State::WakeUp:
            WakeUp(deltaTime);
        break;

        case State::Stop:
            Stop(deltaTime);
        break;

        case State::Run:
            Run(deltaTime);
        break;

        case State::Attack:
            Attack(deltaTime);
        break;

        case State::Bite:
            Bite(deltaTime);
        break;

        case State::Recover:
            Recover(deltaTime);
        break;
    }
}

void Bat::Sleep(float deltaTime) {
    mRigidBodyComponent->SetVelocity(Vector2::Zero);

    // Testa se spotted player
    Vector2 dist = GetPosition() - GetGame()->GetPlayer()->GetPosition();
    if (dist.Length() < mDistToSpotPlayer) {
        mPlayerSpotted = true;
    }

    if (mPlayerSpotted) {
        mBatState = State::WakeUp;
    }
}

void Bat::WakeUp(float deltaTime) {
    if (mKnockBackTimer >= mKnockBackDuration) {
        mRigidBodyComponent->SetVelocity(Vector2(0, 0));
    }

    mWakeUpTimer += deltaTime;
    if (mWakeUpTimer >= mWakeUpDuration) {
        mWakeUpTimer = 0;
        mBatState = State::Stop;
    }
}

void Bat::Stop(float deltaTime) {
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
        mRigidBodyComponent->SetVelocity(Vector2(0, 0));
    }
    mStopTimer += deltaTime;
    if (mStopTimer >= mStopDuration) {
        mStopTimer = 0;
        mBatState = State::Run;
    }
}

void Bat::Run(float deltaTime) {
    Player *player = GetGame()->GetPlayer();

    Vector2 toPlayer = player->GetPosition() - GetPosition();
    float toPlayerY = toPlayer.y;
    float dist = toPlayer.Length();
    toPlayer.Normalize();

    // Adicionar Ruído
    mFlightTime += deltaTime;
    // Criamos um vetor perpendicular à direção do player. Isso faz ele "derrapar" de um lado pro outro.
    Vector2 perpendicular(-toPlayer.y, toPlayer.x);
    // 8.0f (frequência) e 0.4f (amplitude)
    float noiseFactor = Math::Sin(mFlightTime * 8.0f) * 0.4f;

    Vector2 desiredDirection = toPlayer + (perpendicular * noiseFactor);
    desiredDirection.Normalize();

    float targetAngle = Math::Atan2(desiredDirection.y, desiredDirection.x);
    if (targetAngle < 0) {
        targetAngle += 2 * Math::Pi;
    }
    SetRotation(targetAngle);

    if (mKnockBackTimer >= mKnockBackDuration) {
        Vector2 currentVelocity = mRigidBodyComponent->GetVelocity();
        Vector2 desiredVelocity = desiredDirection * mMoveSpeed;

        // Interpolação linear para a velocidade
        float smoothFactor = 4.0f * deltaTime;
        Vector2 newVelocity = currentVelocity + (desiredVelocity - currentVelocity) * smoothFactor;

        mRigidBodyComponent->SetVelocity(newVelocity);
    }

    // Transições de Ataque
    if (dist <= mDistToAttack) {
        if (Math::Abs(toPlayerY) < mDistToBiteY) {
            if (GetRotation() > Math::PiOver2 && GetRotation() < 3 * Math::PiOver2) {
                mAttackDirectionRight = false;
            } else {
                mAttackDirectionRight = true;
            }
            mBatState = State::Bite;
        }
        else {
            if (GetRotation() > Math::PiOver2 && GetRotation() < 3 * Math::PiOver2) {
                mAttackDirectionRight = false;
            } else {
                mAttackDirectionRight = true;
            }
            mBatState = State::Attack;
        }
    }
}

void Bat::Attack(float deltaTime) {
    mAttackTimer += deltaTime;
    if (mKnockBackTimer >= mKnockBackDuration) {
        mRigidBodyComponent->SetVelocity(GetForward() * mMoveSpeed * 1.4);
    }

    if (mAttackTimer >= mAttackDuration) {
        mAttackTimer = 0;
        mBatState = State::Recover;
    }
}

void Bat::Bite(float deltaTime) {
    mBiteTimer += deltaTime;

    if (mKnockBackTimer >= mKnockBackDuration) {
        if (mBiteTimer < mBiteDuration * 0.4f) {
            Player *player = GetGame()->GetPlayer();
            Vector2 toPlayer = player->GetPosition() - GetPosition();

            float targetAngle = Math::Atan2(toPlayer.y, toPlayer.x);
            if (targetAngle < 0) {
                targetAngle += 2 * Math::Pi;
            }
            SetRotation(targetAngle);

            if (targetAngle > Math::PiOver2 && targetAngle < 3 * Math::PiOver2) {
                mAttackDirectionRight = false;
            } else {
                mAttackDirectionRight = true;
            }

            mRigidBodyComponent->SetVelocity(GetForward() * (mMoveSpeed * -0.5f));
        }
        else if (mBiteTimer < mBiteDuration * 0.8f) {
            mRigidBodyComponent->SetVelocity(GetForward() * (mMoveSpeed * 2.0f));
        }
        else {
            mRigidBodyComponent->SetVelocity(Vector2::Zero);
        }
    }

    if (mBiteTimer >= mBiteDuration) {
        mBiteTimer = 0;
        mBatState = State::Recover;
    }
}

void Bat::Recover(float deltaTime) {
    mRecoverTimer += deltaTime;
    Player *player = GetGame()->GetPlayer();

    // Direção oposta ao jogador
    Vector2 escapeDir = GetPosition() - player->GetPosition();
    escapeDir.Normalize();

    // Adiciona uma leve tendência para voar para cima
    escapeDir = escapeDir + Vector2(0, -0.5f);
    escapeDir.Normalize();

    if (escapeDir.x > 0) {
        SetScale(Vector2(-1, 1));
    } else {
        SetScale(Vector2(1, 1));
    }

    if (mKnockBackTimer >= mKnockBackDuration) {
        Vector2 currentVelocity = mRigidBodyComponent->GetVelocity();
        Vector2 desiredVelocity = escapeDir * (mMoveSpeed * 0.6f);

        Vector2 newVelocity = currentVelocity + (desiredVelocity - currentVelocity) * (deltaTime * 3.0f);
        mRigidBodyComponent->SetVelocity(newVelocity);
    }

    if (mRecoverTimer >= mRecoverDuration) {
        mRecoverTimer = 0;
        mBatState = State::Run;
    }
}

void Bat::ManageAnimations() {
    mDrawComponent->SetAnimFPS(14.0f);
    if (GetRotation() > Math::PiOver2 && GetRotation() < 3 * Math::PiOver2) {
        SetScale(Vector2(-1,1));
    }
    else {
        SetScale(Vector2(1,1));
    }

    if (mBatState == State::Attack) {
        mDrawComponent->SetAnimation("attack");
        mDrawComponent->SetAnimFPS(8.0f / mAttackDuration);
    }
    else if (mBatState == State::Bite) {
        mDrawComponent->SetAnimation("bite");
        mDrawComponent->SetAnimFPS(11.0f / mBiteDuration);
    }
    else if (mBatState == State::Sleep) {
        mDrawComponent->SetAnimation("sleep");
        mDrawComponent->SetAnimFPS(2);
    }
    else if (mBatState == State::WakeUp) {
        mDrawComponent->SetAnimation("wakeUp");
        mDrawComponent->SetAnimFPS(14.0f / mWakeUpDuration);
    }
    else if (mBatState == State::Stop) {
        mDrawComponent->SetAnimation("idle");
    }
    else if (mBatState == State::Run) {
        mDrawComponent->SetAnimation("run");
        mDrawComponent->SetAnimFPS(15.0f);
    }
    else if (mBatState == State::Recover) {
        mDrawComponent->SetAnimation("run");
        mDrawComponent->SetAnimFPS(15.0f);
    }
    if (mIsFlashing) {
        mDrawComponent->SetTextureFactor(0.0f);
    }
    else {
        mDrawComponent->SetTextureFactor(1.0f);
    }
}

void Bat::ManageCombatBox() {
    if (mBatState == State::Run || mBatState == State::Stop || mBatState == State::Recover) {
        mCombatBoxComponent->SetBoxOffset("hitbox", Vector2(0, mHeight * 0.2f));
        mCombatBoxComponent->SetBoxOffset("hurtbox", Vector2(0, mHeight * 0.2f));
    }

    if (mBatState == State::Attack) {
        if (mAttackTimer > 0.2f * mAttackDuration && mAttackTimer < 0.5f * mAttackDuration) {
            if (mAttackDirectionRight) {
                mCombatBoxComponent->SetBoxOffset("hitbox", Vector2(mWidth * 1.5f, -mHeight * 1.5f));
                mCombatBoxComponent->SetBoxOffset("hurtbox", Vector2(mWidth * 1.5f, -mHeight * 1.5f));
            }
            else {
                mCombatBoxComponent->SetBoxOffset("hitbox", Vector2(-mWidth * 1.5f, -mHeight * 1.5f));
                mCombatBoxComponent->SetBoxOffset("hurtbox", Vector2(-mWidth * 1.5f, -mHeight * 1.5f));
            }
        }
        else if (mAttackTimer > 0.5f * mAttackDuration && mAttackTimer < 0.625f * mAttackDuration) {
            if (mAttackDirectionRight) {
                mCombatBoxComponent->SetBoxOffset("hitbox", Vector2(mWidth * 2.9f, 0));
                mCombatBoxComponent->SetBoxOffset("hurtbox", Vector2(mWidth * 2.9f, 0));
            }
            else {
                mCombatBoxComponent->SetBoxOffset("hitbox", Vector2(-mWidth * 2.9f, 0));
                mCombatBoxComponent->SetBoxOffset("hurtbox", Vector2(-mWidth * 2.9f, 0));
            }
        }
        else if (mAttackTimer > 0.625f * mAttackDuration && mAttackTimer < 0.75f * mAttackDuration) {
            mWidth = mAttackSpriteWidth;
            mCombatBoxComponent->SetBoxHalfSize("hitbox", Vector2(mWidth / 2, mHeight / 2));
            mCombatBoxComponent->SetBoxHalfSize("hurtbox", Vector2(mWidth / 2, mHeight / 2));

            if (mAttackDirectionRight) {
                mCombatBoxComponent->SetBoxOffset("hitbox", Vector2(mWidth * 0.4f, mHeight * 1.8f));
                mCombatBoxComponent->SetBoxOffset("hurtbox", Vector2(mWidth * 0.4f, mHeight * 1.8f));
            }
            else {
                mCombatBoxComponent->SetBoxOffset("hitbox", Vector2(-mWidth * 0.4f, mHeight * 1.8f));
                mCombatBoxComponent->SetBoxOffset("hurtbox", Vector2(-mWidth * 0.4f, mHeight * 1.8f));
            }
        }
        else if (mAttackTimer > 0.75f * mAttackDuration && mAttackTimer < 1.0f * mAttackDuration) {
            mWidth = mIdleWidth;
            mCombatBoxComponent->SetBoxHalfSize("hitbox", Vector2(mWidth / 2, mHeight / 2));
            mCombatBoxComponent->SetBoxHalfSize("hurtbox", Vector2(mWidth / 2, mHeight / 2));

            mCombatBoxComponent->SetBoxOffset("hitbox", Vector2(0, mHeight * 1.8f));
            mCombatBoxComponent->SetBoxOffset("hurtbox", Vector2(0, mHeight * 1.8f));
        }
        else {
            mCombatBoxComponent->SetBoxOffset("hitbox", Vector2::Zero);
            mCombatBoxComponent->SetBoxOffset("hurtbox", Vector2::Zero);
        }
    }

    if (mBatState == State::Bite) {
        if (mBiteTimer > 0.12f * mBiteDuration && mBiteTimer < 0.35f * mBiteDuration) {
            if (mAttackDirectionRight) {
                mCombatBoxComponent->SetBoxOffset("hitbox", Vector2(-mWidth * 1.8f, -mHeight * 1.0f));
                mCombatBoxComponent->SetBoxOffset("hurtbox", Vector2(-mWidth * 1.8f, -mHeight * 1.0f));
            }
            else {
                mCombatBoxComponent->SetBoxOffset("hitbox", Vector2(mWidth * 1.8f, -mHeight * 1.0f));
                mCombatBoxComponent->SetBoxOffset("hurtbox", Vector2(mWidth * 1.8f, -mHeight * 1.0f));
            }
        }
        if (mBiteTimer > 0.35f * mBiteDuration && mBiteTimer < 0.61f * mBiteDuration) {
            mCombatBoxComponent->SetBoxOffset("hitbox", Vector2(0, -mHeight * 0.7f));
            mCombatBoxComponent->SetBoxOffset("hurtbox", Vector2(0, -mHeight * 0.7f));
        }
        else if (mBiteTimer > 0.61f * mBiteDuration && mBiteTimer < 0.95f * mBiteDuration) {
            if (mAttackDirectionRight) {
                mCombatBoxComponent->SetBoxOffset("hitbox", Vector2(mWidth * 2.5f, -mHeight * 0.4f));
                mCombatBoxComponent->SetBoxOffset("hurtbox", Vector2(mWidth * 2.5f, -mHeight * 0.4f));
            }
            else {
                mCombatBoxComponent->SetBoxOffset("hitbox", Vector2(-mWidth * 2.5f, -mHeight * 0.4f));
                mCombatBoxComponent->SetBoxOffset("hurtbox", Vector2(-mWidth * 2.5f, -mHeight * 0.4f));
            }
        }
    }
}
