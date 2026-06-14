//
// Created by roger on 20/05/2026.
//

#include "Frogger.h"

#include <pstl/algorithm_fwd.h>

#include "../Actor.h"
#include "../../Game.h"
#include "../../HUD.h"
#include "../../Random.h"
#include "../ParticleSystem.h"
#include "../../Components/RigidBodyComponent.h"
#include "../../Components/Drawing/AnimatorComponent.h"
#include "../../Components/CombatBoxComponent.h"
#include "../../Components/OBBComponent.h"

Frogger::Frogger(Game *game)
    :Enemy(game)
    ,mFroggerState(State::Sleep)

    ,mDistToSpotPlayer(400.0f)

    ,mStopDuration(1.2f)
    ,mStopTimer(0.0f)

    ,mHitDuration(0.07f)

    ,mMaxJumps(6)
    ,mJumpCount(0)
    ,mJumpForce(1500.0f)
    ,mDurationBetweenJumps(0.3f)
    ,mTimerBetweenJumps(0.0f)
    ,mNextJumpDirectionX(0.0f)
    ,mNextJumpAngle(0.0f)
    ,mWasOnGround(false)

    ,mWallPosition(WallSide::Bottom)
    ,mDestinyWall(WallSide::Bottom)
    ,mGravity(3000)

    ,mMinDistFromEdge(256)
    ,mAttackJumpInterval(2)

    ,mDistToTongue(400.0f)
    ,mTongueDuration(0.6f)
    ,mTongueTimer(0.0f)

    ,mSpitDuration(0.8f)
    ,mSpitTimer(0.0f)

    ,mJumpComboProbability(0.6f)

    ,mRunningGrassParticleIntervalDuration(0.05f)
    ,mRunningGrassParticleIntervalTimer(0.0f)
{
    mWidth = 145;
    mHeight = 145;
    mMoveSpeed = 500;
    mHealthPoints = 200;
    mMaxHealthPoints = mHealthPoints;
    mContactDamage = 18;
    mMoneyDrop = 100;
    mKnockBackSpeed = 0.0f;
    mKnockBackDuration = 0.0f;
    mKnockBackTimer = mKnockBackDuration;
    mFreezeMax = 1000;
    mFrozenDecayRate = mFreezeMax / 3.0f;
    mEnemyCollision = false;

    SetRotation(Math::Pi);
    SetTransformRotation(0.0f);
    SetScale(Vector2(-1,1));

    SetSize(mWidth, mHeight);

    mDrawComponent = new AnimatorComponent(this, "../Assets/Sprites/Frogger2/Frogger.png",
                                                "../Assets/Sprites/Frogger2/Frogger.json",
                                                mWidth * 2.5f * 3.0f, mHeight * 2.5f, 998);

    std::vector hurt = {0, 1, 2, 3};
    mDrawComponent->AddAnimation("hurt", hurt);

    std::vector idleSide = {4, 5, 6, 7, 8};
    mDrawComponent->AddAnimation("idleSide", idleSide);

    std::vector idleFront = {9, 10, 11, 12, 13};
    mDrawComponent->AddAnimation("idleFront", idleFront);

    std::vector run = {14, 15, 16, 17, 18, 19, 20, 21};
    mDrawComponent->AddAnimation("run", run);

    std::vector spit = {22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32};
    mDrawComponent->AddAnimation("spit", spit);

    std::vector tongue = {33, 34, 35, 36, 37, 38, 39, 40};
    mDrawComponent->AddAnimation("tongue", tongue);

    std::vector jump = {18};
    mDrawComponent->AddAnimation("jump", jump);

    std::vector jumpPreparation = {21, 20};
    mDrawComponent->AddAnimation("jumpPreparation", jumpPreparation);

    mDrawComponent->SetAnimation("idleSide");
    mDrawComponent->SetAnimFPS(10.0f);

    RemoveComponent(mColliderComponent);
    delete mColliderComponent;
    mColliderComponent = nullptr;

    mColliderComponent = new OBBComponent(this, Vector2(mWidth / 2, mHeight / 2));
    // mColliderComponent->SetDebugDraw(true);

    RemoveComponent(mCombatBoxComponent);
    delete mCombatBoxComponent;
    mCombatBoxComponent = nullptr;

    mCombatBoxComponent = new CombatBoxComponent(this);
    mCombatBoxComponent->AddOBBBox("hitbox", true, Vector2(mWidth / 2, mHeight / 2));
    mCombatBoxComponent->AddOBBBox("hurtbox", false, Vector2(mWidth / 2, mHeight / 2));

    mCombatBoxComponent->AddAABBBox("tongue", true, Vector2(-80, -40), Vector2(80, 40));
    mCombatBoxComponent->SetBoxActive("tongue", false);

    mCombatBoxComponent->AddOBBBox("spit", true, Vector2(-80, -40), Vector2(80, 40));
    mCombatBoxComponent->SetBoxActive("spit", false);

    // mCombatBoxComponent->SetDebugDraw(true);
}

void Frogger::OnUpdate(float deltaTime) {
    mIsOnGround = false;
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

    // Gravidade
    if (!mIsOnGround && mFroggerState != State::JumpCombo) {
        mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x,
                                                 mRigidBodyComponent->GetVelocity().y
                                                 + mGravity * deltaTime));
    }

    if (!mIsFrozen && !mIsStunned) {
        if (mPlayerSpotted) {
            mGame->GetHUD()->StartBossFight(this);
            if (!mGame->GetBossMusicHandle().IsValid()) {
                mGame->StartBossMusic(mGame->GetAudio()->PlaySound("MantisLords.wav", true, SoundCategory::Music));
            }
        }
        MovementAfterPlayerSpotted(deltaTime);
    }

    mWasOnGround = mIsOnGround;

    if (mCombatBoxComponent) {
        ManageCombatBox();
    }

    // Se morreu
    if (Died()) {
        TriggerBossDefeat();
    }

    if (!mIsFrozen && !mIsStunned) {
        if (mDrawComponent) {
            ManageAnimations();
        }
    }

    if (mIsOnGround && mFroggerState == State::RunForward) {
        mRunningGrassParticleIntervalTimer += deltaTime;
        if (mRunningGrassParticleIntervalTimer >= mRunningGrassParticleIntervalDuration) {
            mRunningGrassParticleIntervalTimer -= mRunningGrassParticleIntervalDuration;
            StartGrassEffect(GrassEffectType::Run);
        }
    }

    if (mHealthPoints <= mMaxHealthPoints / 2) {
        mStopDuration = 0.8f;
        mMaxJumps = 9;
        mJumpForce = 1850.0f;
        mDurationBetweenJumps = 0.15f;
        mAttackJumpInterval = 3;
        mMoveSpeed = 700;
    }
}

void Frogger::TriggerBossDefeat() {
    mGame->StopBossMusic();
}

void Frogger::MovementAfterPlayerSpotted(float deltaTime) {
    switch (mFroggerState) {
        case State::Sleep:
            Sleep(deltaTime);
            break;

        case State::Stop:
            Stop(deltaTime);
            break;

        case State::JumpCombo:
            JumpCombo(deltaTime);
            break;

        case State::RunForward:
            RunForward(deltaTime);
            break;

        case State::Tongue:
            Tongue(deltaTime);
            break;

        case State::Spit:
            Spit(deltaTime);
            break;
    }
}

void Frogger::Sleep(float deltaTime) {
    mRigidBodyComponent->SetVelocity(Vector2(0, mRigidBodyComponent->GetVelocity().y));

    // Testa se spotted player
    Vector2 dist = GetPosition() - GetGame()->GetPlayer()->GetPosition();
    if (mPlayerSpotted || dist.Length() < mDistToSpotPlayer) {
        mPlayerSpotted = true;
        mFroggerState = State::Stop;
    }
}

void Frogger::Stop(float deltaTime) {
    mStopTimer += deltaTime;
    if (mStopTimer >= mStopDuration) {
        mStopTimer = 0;
        EvaluateNextState();
        return;
    }

    Player* player = GetGame()->GetPlayer();
    float dist = GetPosition().x - player->GetPosition().x;
    if (dist < 0) {
        SetRotation(0.0);
        SetTransformRotation(0.0f);
        SetScale(Vector2(1,1));
    }
    else {
        SetRotation(Math::Pi);
        SetTransformRotation(0.0f);
        SetScale(Vector2(-1,1));
    }

    if (mKnockBackTimer >= mKnockBackDuration) {
        mRigidBodyComponent->SetVelocity(Vector2(0, mRigidBodyComponent->GetVelocity().y));
    }
}

void Frogger::JumpCombo(float deltaTime) {
    if (mIsOnGround) {

        // CHECAGEM DE POUSO: Acabou de bater na parede neste frame?
        if (mIsOnGround && !mWasOnGround) {
            mRigidBodyComponent->SetVelocity(Vector2::Zero);
            StartGrassEffect(GrassEffectType::Land);

            // Força o OBB a ficar reto para não sobrar espaço da quina
            if (mWallPosition == WallSide::Bottom || mWallPosition == WallSide::Top) {
                SetRotation(0.0f);
            } else {
                SetRotation(Math::PiOver2);
            }

            // Empurra o sapo para dentro da parede e pede para a física expulsar ele perfeitamente alinhado
            Vector2 snapPos = GetPosition();
            float snapDist = 40.0f; // Maior que o safeOffset para garantir sobreposição

            switch (mWallPosition) {
                case WallSide::Bottom: snapPos.y += snapDist; break;
                case WallSide::Top: snapPos.y -= snapDist; break;
                case WallSide::Right: snapPos.x += snapDist; break;
                case WallSide::Left: snapPos.x -= snapDist; break;
            }
            SetPosition(snapPos);
        }

        if (mJumpCount >= mMaxJumps) {
            mJumpCount = 0;
            mFroggerState = State::Stop;
            return;
        }

        // Se acabou de pousar , calculamos o próximo alvo
        if (mTimerBetweenJumps == 0.0f) {
            Vector2 targetPos;
            float dx = 0;
            float dy = 0;
            float angle = 0;

            int nextJumpCount = mJumpCount + 1;

            if (nextJumpCount == mMaxJumps) {
                mDestinyWall = WallSide::Bottom;
                targetPos.x = Random::GetFloatRange(mArenaMinPos.x + mMinDistFromEdge, mArenaMaxPos.x - mMinDistFromEdge);
                targetPos.y = mArenaMaxPos.y;
                dx = targetPos.x - GetPosition().x;
                dy = targetPos.y - GetPosition().y;
            }
            else if ((nextJumpCount % mAttackJumpInterval) == 0) {
                dx = mGame->GetPlayer()->GetPosition().x - GetPosition().x;
                dy = mGame->GetPlayer()->GetPosition().y - GetPosition().y;
            }
            else {
                mDestinyWall = mWallPosition;
                std::vector<WallSide> validSides = {WallSide::Top, WallSide::Left, WallSide::Right};
                while (mDestinyWall == mWallPosition) {
                    int index = Random::GetIntRange(0, 2);
                    mDestinyWall = validSides[index];
                }
                switch (mDestinyWall) {
                    case WallSide::Bottom:
                        targetPos.x = Random::GetFloatRange(mArenaMinPos.x + mMinDistFromEdge, mArenaMaxPos.x - mMinDistFromEdge);
                        targetPos.y = mArenaMaxPos.y;
                        break;
                    case WallSide::Top:
                        targetPos.x = Random::GetFloatRange(mArenaMinPos.x + mMinDistFromEdge, mArenaMaxPos.x - mMinDistFromEdge);
                        targetPos.y = mArenaMinPos.y;
                        break;
                    case WallSide::Right:
                        targetPos.x = mArenaMaxPos.x;
                        targetPos.y = Random::GetFloatRange(mArenaMinPos.y + mMinDistFromEdge, mArenaMaxPos.y - mMinDistFromEdge);
                        break;
                    case WallSide::Left:
                        targetPos.x = mArenaMinPos.x;
                        targetPos.y = Random::GetFloatRange(mArenaMinPos.y + mMinDistFromEdge, mArenaMaxPos.y - mMinDistFromEdge);
                        break;
                }
                dx = targetPos.x - GetPosition().x;
                dy = targetPos.y - GetPosition().y;
            }

            angle = Math::Atan2(dy, dx);
            if (angle < 0) {
                angle += 2 * Math::Pi;
            }

            // Guardamos os resultados do cálculo para usar durante a preparação
            mNextJumpDirectionX = dx;
            mNextJumpAngle = angle;
        }

        // Ajustar o olhar do sapo enquanto ele estiver no chão
        if (mWallPosition == WallSide::Bottom) {
            if (mNextJumpDirectionX >= 0) {
                SetRotation(0.0f);
                SetScale(Vector2(1, 1));
            } else {
                SetRotation(Math::Pi);
                SetScale(Vector2(-1, 1));
            }
        }

        mTimerBetweenJumps += deltaTime;

        if (mTimerBetweenJumps >= mDurationBetweenJumps) {
            mTimerBetweenJumps = 0.0f;
            mJumpCount++;

            // Como a quina do OBB é maior que o lado plano, afastamos o sapo da parede
            // em 35 pixels (margem segura) para ele não enganchar no chão ao rotacionar.
            if (mWasOnGround) {
                Vector2 safePos = GetPosition();
                float safeOffset = 35.0f;

                switch (mWallPosition) {
                    case WallSide::Bottom: safePos.y -= safeOffset; break;
                    case WallSide::Top:    safePos.y += safeOffset; break;
                    case WallSide::Right:  safePos.x -= safeOffset; break;
                    case WallSide::Left:   safePos.x += safeOffset; break;
                }
                SetPosition(safePos);
            }

            SetRotation(mNextJumpAngle);

            if (mJumpCount != mMaxJumps) {
                SetTransformRotation(mNextJumpAngle);
            }

            mRigidBodyComponent->SetVelocity(Vector2(GetForward() * mJumpForce));
            StartGrassEffect(GrassEffectType::Jump);
        }
        else {
            Vector2 stickVel = Vector2::Zero;
            float stickForce = 80.0f;

            switch (mWallPosition) {
                case WallSide::Bottom: stickVel.y = stickForce; break;
                case WallSide::Top:    stickVel.y = -stickForce; break;
                case WallSide::Right:  stickVel.x = stickForce; break;
                case WallSide::Left:   stickVel.x = -stickForce; break;
            }
            mRigidBodyComponent->SetVelocity(stickVel);
        }
    }
    else {
        mRigidBodyComponent->SetVelocity(Vector2(GetForward() * mJumpForce));
    }
}

void Frogger::RunForward(float deltaTime) {
    Player* player = GetGame()->GetPlayer();
    float dist = GetPosition().x - player->GetPosition().x;
    if (dist < 0) {
        SetRotation(0.0);
        SetTransformRotation(0.0f);
        SetScale(Vector2(1,1));
    }
    else {
        SetRotation(Math::Pi);
        SetTransformRotation(0.0f);
        SetScale(Vector2(-1,1));
    }

    mRigidBodyComponent->SetVelocity(Vector2(GetForward().x * mMoveSpeed, mRigidBodyComponent->GetVelocity().y));

    if (Math::Abs(dist) <= mDistToTongue) {
        EvaluateNextState();
        return;
    }
}

void Frogger::Tongue(float delTime) {
    mTongueTimer += delTime;
    if (mTongueTimer >= mTongueDuration) {
        mTongueTimer = 0.0f;
        mFroggerState = State::Stop;
        return;
    }

    if (mKnockBackTimer >= mKnockBackDuration) {
        mRigidBodyComponent->SetVelocity(Vector2::Zero);
    }
}

void Frogger::Spit(float delTime) {
    mSpitTimer += delTime;
    if (mSpitTimer >= mSpitDuration) {
        mSpitTimer = 0.0f;
        mFroggerState = State::Stop;
        return;
    }

    if (mKnockBackTimer >= mKnockBackDuration) {
        mRigidBodyComponent->SetVelocity(Vector2::Zero);
    }
}

void Frogger::ManageAnimations() {
    mDrawComponent->SetAnimFPS(10.0f);

    if (mFroggerState == State::Sleep) {
        mDrawComponent->SetAnimation("idleSide");
    }
    else if (mFroggerState == State::RunForward) {
        mDrawComponent->SetAnimation("run");
        if (mHealthPoints <= mMaxHealthPoints / 2) {
            mDrawComponent->SetAnimFPS(18.0f);
        }
        else {
            mDrawComponent->SetAnimFPS(14.0f);
        }
    }
    else if (mFroggerState == State::JumpCombo) {
        if (mIsOnGround) {
            switch (mWallPosition) {
                case WallSide::Bottom:
                    SetTransformRotation(0.0f);
                    break;

                case WallSide::Top:
                    SetTransformRotation(Math::Pi);
                    break;

                case WallSide::Left:
                    SetTransformRotation(Math::PiOver2);
                    break;

                case WallSide::Right:
                    SetTransformRotation(3 * Math::PiOver2);
                    break;
            }
            if (mWallPosition == WallSide::Bottom) {
                mDrawComponent->SetAnimation("jumpPreparation");
                mDrawComponent->SetAnimFPS(2.0f / mDurationBetweenJumps);
            }
            else {
                mDrawComponent->SetAnimation("idleFront");
            }
        }
        else {
            mDrawComponent->SetAnimation("jump");

            Vector2 vel = mRigidBodyComponent->GetVelocity();

            // Só ajusta a rotação se o boss realmente estiver se movendo
            if (vel.LengthSq() > 0.001f) {
                if (vel.x >= 0) {
                    // Indo para a Direita: Escala normal, ângulo normal
                    SetScale(Vector2(1, 1));
                    SetTransformRotation(Math::Atan2(vel.y, vel.x));
                }
                else {
                    // Indo para a Esquerda: Flipa a escala em X, inverte os valores no Atan2
                    SetScale(Vector2(-1, 1));
                    SetTransformRotation(Math::Atan2(-vel.y, -vel.x));
                }
            }
        }
    }
    else if (mFroggerState == State::Stop) {
        mDrawComponent->SetAnimation("idleSide");
    }
    else if (mFroggerState == State::Tongue) {
        mDrawComponent->SetAnimation("tongue");
        mDrawComponent->SetAnimFPS(8.0f / mTongueDuration);
    }
    else if (mFroggerState == State::Spit) {
        mDrawComponent->SetAnimation("spit");
        mDrawComponent->SetAnimFPS(11.0f / mSpitDuration);
    }
    if (mIsFlashing) {
        mDrawComponent->SetTextureFactor(0.0f);
    }
    else {
        mDrawComponent->SetTextureFactor(1.0f);
    }
}

void Frogger::ManageCombatBox() {
    if (mFroggerState == State::JumpCombo) {
        if (!mIsOnGround) {
            mCombatBoxComponent->SetBoxHalfSize("hitbox", Vector2((mWidth * 0.5f) * 1.5f, (mHeight * 0.5f) * 0.7f));
            mCombatBoxComponent->SetBoxHalfSize("hurtbox", Vector2((mWidth * 0.5f) * 1.5f, (mHeight * 0.5f) * 0.7f));

            float baseOffsetX = mWidth * 0.2f;
            float baseOffsetY = mHeight * -0.2f;

            float currentOffsetX = baseOffsetX * GetScale().x;
            float currentOffsetY = baseOffsetY;

            float angle = GetTransformRotation();
            float cosA = Math::Cos(angle);
            float sinA = Math::Sin(angle);

            // matriz de Rotação 2D
            Vector2 finalOffset;
            finalOffset.x = (currentOffsetX * cosA) - (currentOffsetY * sinA);
            finalOffset.y = (currentOffsetX * sinA) + (currentOffsetY * cosA);

            mCombatBoxComponent->SetBoxOffset("hitbox", finalOffset);
            mCombatBoxComponent->SetBoxOffset("hurtbox", finalOffset);
        }
        else {
            mCombatBoxComponent->SetBoxHalfSize("hitbox", Vector2((mWidth * 0.5f), (mHeight * 0.5f)));
            mCombatBoxComponent->SetBoxHalfSize("hurtbox", Vector2((mWidth * 0.5f), (mHeight * 0.5f)));
            mCombatBoxComponent->SetBoxOffset("hitbox", Vector2::Zero);
            mCombatBoxComponent->SetBoxOffset("hurtbox", Vector2::Zero);
        }
    }
    else if (mFroggerState == State::Tongue) {
        if (mTongueTimer > 0.25f * mTongueDuration && mTongueTimer < 0.39f * mTongueDuration) {
            mCombatBoxComponent->SetBoxActive("tongue", true);
            mCombatBoxComponent->SetBoxHalfSize("tongue", Vector2(mWidth * 0.7f, mHeight * 0.08f));
            mCombatBoxComponent->SetBoxOffset("tongue", Vector2(mWidth * 0.8f, mHeight * 0.08f) * Vector2(GetForward().x, -1));
        }
        else if (mTongueTimer > 0.39f * mTongueDuration && mTongueTimer < 0.62f * mTongueDuration) {
            mCombatBoxComponent->SetBoxActive("tongue", true);
            mCombatBoxComponent->SetBoxHalfSize("tongue", Vector2(mWidth * 1.4f, mHeight * 0.08f));
            mCombatBoxComponent->SetBoxOffset("tongue", Vector2(mWidth * 1.4f, mHeight * 0.08f) * Vector2(GetForward().x, -1));
        }
        else if (mTongueTimer > 0.62f * mTongueDuration && mTongueTimer < 0.75f * mTongueDuration) {
            mCombatBoxComponent->SetBoxActive("tongue", true);
            mCombatBoxComponent->SetBoxHalfSize("tongue", Vector2(mWidth * 0.85f, mHeight * 0.08f));
            mCombatBoxComponent->SetBoxOffset("tongue", Vector2(mWidth * 1.2f, mHeight * 0.08f) * Vector2(GetForward().x, -1));
        }
        else if (mTongueTimer > 0.75f * mTongueDuration && mTongueTimer < 0.88f * mTongueDuration) {
            mCombatBoxComponent->SetBoxActive("tongue", true);
            mCombatBoxComponent->SetBoxHalfSize("tongue", Vector2(mWidth * 0.5f, mHeight * 0.08f));
            mCombatBoxComponent->SetBoxOffset("tongue", Vector2(mWidth * 0.7f, mHeight * 0.08f) * Vector2(GetForward().x, -1));
        }
        else {
            mCombatBoxComponent->SetBoxActive("tongue", false);
        }
    }
    else if (mFroggerState == State::Spit) {
        if (mSpitTimer > 0.615f * mSpitDuration && mSpitTimer < 0.73f * mSpitDuration) {
            mCombatBoxComponent->SetBoxActive("spit", true);
            mCombatBoxComponent->SetBoxHalfSize("spit", Vector2(mWidth * 1.0f, mHeight * 0.18f));
            mCombatBoxComponent->SetBoxOffset("spit", Vector2(mWidth * 1.7f, mHeight * 0.15f) * Vector2(GetForward().x, 1));
            if (CombatBox* box = mCombatBoxComponent->GetBox("spit")) {
                if (auto* obb = dynamic_cast<OBBComponent*>(box->collider)) {
                    int rotation = 0;
                    if (GetForward().x == 1) {
                        rotation = 13;
                    }
                    else if (GetForward().x == -1) {
                        rotation = 180 - 13;
                    }
                    obb->SetRotation(Math::ToRadians(rotation));
                }
            }
        }
        else if (mSpitTimer > 0.73f * mSpitDuration && mSpitTimer < 0.82f * mSpitDuration) {
            mCombatBoxComponent->SetBoxActive("spit", true);
            mCombatBoxComponent->SetBoxHalfSize("spit", Vector2(mWidth * 0.6f, mHeight * 0.18f));
            mCombatBoxComponent->SetBoxOffset("spit", Vector2(mWidth * 2.5f, mHeight * 0.28f) * Vector2(GetForward().x, 1));
            if (CombatBox* box = mCombatBoxComponent->GetBox("spit")) {
                if (auto* obb = dynamic_cast<OBBComponent*>(box->collider)) {
                    int rotation = 0;
                    if (GetForward().x == 1) {
                        rotation = 18;
                    }
                    else if (GetForward().x == -1) {
                        rotation = 180 - 18;
                    }
                    obb->SetRotation(Math::ToRadians(rotation));
                }
            }
        }
        else if (mSpitTimer > 0.82f * mSpitDuration && mSpitTimer < 0.9f * mSpitDuration) {
            mCombatBoxComponent->SetBoxActive("spit", true);
            mCombatBoxComponent->SetBoxHalfSize("spit", Vector2(mWidth * 0.3f, mHeight * 0.18f));
            mCombatBoxComponent->SetBoxOffset("spit", Vector2(mWidth * 3.1f, mHeight * 0.45f) * Vector2(GetForward().x, 1));
            if (CombatBox* box = mCombatBoxComponent->GetBox("spit")) {
                if (auto* obb = dynamic_cast<OBBComponent*>(box->collider)) {
                    int rotation = 0;
                    if (GetForward().x == 1) {
                        rotation = 18;
                    }
                    else if (GetForward().x == -1) {
                        rotation = 180 - 18;
                    }
                    obb->SetRotation(Math::ToRadians(rotation));
                }
            }
        }
        else {
            mCombatBoxComponent->SetBoxActive("spit", false);
        }
    }
    else {
        mCombatBoxComponent->SetBoxHalfSize("hitbox", Vector2((mWidth * 0.5f), (mHeight * 0.5f)));
        mCombatBoxComponent->SetBoxHalfSize("hurtbox", Vector2((mWidth * 0.5f), (mHeight * 0.5f)));
        mCombatBoxComponent->SetBoxOffset("hitbox", Vector2::Zero);
        mCombatBoxComponent->SetBoxOffset("hurtbox", Vector2::Zero);
    }
}

void Frogger::ResolveGroundCollision() {
    Vector2 collisionNormal(Vector2::Zero);
    std::vector<Ground* > grounds = GetGame()->GetGrounds();
    if (!grounds.empty()) {
        for (Ground* g: grounds) {
            if (!g->GetIsSpike()) { // Colisão com ground
                if (mColliderComponent->Intersect(*g->GetComponent<ColliderComponent>())) {
                    collisionNormal = mColliderComponent->ResolveCollision(*g->GetComponent<ColliderComponent>());
                    mIsOnGround = true;
                    if (collisionNormal == Vector2::NegUnitY) {
                        mWallPosition = WallSide::Bottom;
                    }
                    if (collisionNormal == Vector2::UnitY) {
                        mWallPosition = WallSide::Top;
                    }
                    if (collisionNormal == Vector2::NegUnitX) {
                        mWallPosition = WallSide::Right;
                    }
                    if (collisionNormal == Vector2::UnitX) {
                        mWallPosition = WallSide::Left;
                    }
                }
            }
            else if (g->GetIsSpike()) { // Colisão com spikes
                if (mColliderComponent->Intersect(*g->GetComponent<ColliderComponent>())) {
                    collisionNormal = mColliderComponent->ResolveCollision(*g->GetComponent<ColliderComponent>());
                    // Colidiu top
                    if (collisionNormal == Vector2::NegUnitY) {
                        ReceiveHit(10, Vector2::NegUnitY);
                    }
                    // Colidiu bot
                    if (collisionNormal == Vector2::UnitY) {
                        ReceiveHit(10, Vector2::UnitY);
                    }
                    //Colidiu left
                    if (collisionNormal == Vector2::NegUnitX) {
                        ReceiveHit(10, Vector2::NegUnitX);
                    }
                    //Colidiu right
                    if (collisionNormal == Vector2::UnitX) {
                        ReceiveHit(10, Vector2::UnitX);
                    }
                    mKnockBackTimer = 0;
                }
            }
        }
    }
}

void Frogger::EvaluateNextState() {
    Player* player = GetGame()->GetPlayer();
    float distToPlayerX = std::abs(player->GetPosition().x - GetPosition().x);
    float distToPlayerY = player->GetPosition().y - GetPosition().y;

    if (distToPlayerX <= mDistToTongue) {
        if (distToPlayerY < 0) {
            if (Random::GetFloat() < 0.85f) {
                mFroggerState = State::JumpCombo;
                return;
            }
        }

        if (Random::GetFloat() < 0.25f) {
            mFroggerState = State::JumpCombo;
        }
        else {
            if (Random::GetFloat() < 0.5f) {
                mFroggerState = State::Tongue;
            }
            else {
                mFroggerState = State::Spit;
            }
        }
    }
    else {
        if (Random::GetFloat() < 0.2f) {
            mFroggerState = State::JumpCombo;
        }
        else {
            mFroggerState = State::RunForward;
        }
    }
}

void Frogger::StartGrassEffect(GrassEffectType type) {
    ParticleSystem* grass = nullptr;
    float directionAngle = 0;
    switch (type) {
        case GrassEffectType::Run:
            grass = new ParticleSystem(mGame, Particle::ParticleType::Grass,
              12.17f,
              5.5f,
              30.0f,
              0.9f,
              0.1f);

            grass->SetEmitArea(Vector2(mWidth * 0.8f, mHeight * 0.8f));
            grass->SetParticleGravity(true);
            grass->SetParticleGravityForce(450.0f);
            grass->SetEmitDirection(Vector2::NegUnitY);
            grass->SetGroundCollision(false);
            grass->SetConeSpread(20.0f);
            grass->SetParticleSpeedScale(0.16f);
            grass->SetParticleTextureFactor(1.0f);
            grass->SetParticleAutoRotate(true);
            grass->SetParticleRotationSpeed(1.0f);
            grass->SetParticleDrawOrder(1003);
            grass->SetPosition(GetPosition() + Vector2(0, mHeight * 0.5f));
            break;

        case GrassEffectType::Jump:
            grass = new ParticleSystem(mGame, Particle::ParticleType::Grass,
              12.17f,
              5.5f,
              280.0f,
              0.75f,
              0.1f);

            grass->SetEmitArea(Vector2(mWidth * 1.0f, mHeight * 1.0f));
            grass->SetParticleGravity(true);
            grass->SetParticleGravityForce(450.0f);
            grass->SetGroundCollision(false);
            grass->SetConeSpread(30.0f);
            grass->SetParticleSpeedScale(0.50f);
            grass->SetParticleTextureFactor(1.0f);
            grass->SetParticleAutoRotate(true);
            grass->SetParticleRotationSpeed(1.0f);
            grass->SetParticleDrawOrder(1003);
            switch (mWallPosition) {
                case WallSide::Top:
                    directionAngle = Math::Pi - GetRotation();
                    grass->SetEmitDirection(Vector2(Math::Cos(directionAngle), Math::Sin(directionAngle)));
                    grass->SetPosition(GetPosition() - Vector2(0, mHeight * 0.3f));
                    break;

                case WallSide::Bottom:
                    directionAngle = Math::Pi - GetRotation();
                    grass->SetEmitDirection(Vector2(Math::Cos(directionAngle), Math::Sin(directionAngle)));
                    grass->SetPosition(GetPosition() + Vector2(0, mHeight * 0.3f));
                    break;

                case WallSide::Left:
                    // Espelha no eixo X
                    directionAngle = Math::TwoPi - GetRotation(); // ou apenas -GetRotation()
                    grass->SetEmitDirection(Vector2(Math::Cos(directionAngle), Math::Sin(directionAngle)));
                    grass->SetPosition(GetPosition() - Vector2(mWidth * 0.3f, 0));
                    break;

                case WallSide::Right:
                    // Espelha no eixo X (mesma lógica do Left)
                    directionAngle = Math::TwoPi - GetRotation();
                    grass->SetEmitDirection(Vector2(Math::Cos(directionAngle), Math::Sin(directionAngle)));
                    grass->SetPosition(GetPosition() + Vector2(mWidth * 0.3f, 0));
                    break;
            }
            break;

        case GrassEffectType::Land:
            grass = new ParticleSystem(mGame, Particle::ParticleType::Grass,
              12.17f,
              5.5f,
              180.0f,
              0.7f,
              0.1f);

            grass->SetEmitArea(Vector2(mWidth, mHeight));
            grass->SetParticleGravity(true);
            grass->SetParticleGravityForce(450.0f);
            grass->SetGroundCollision(false);
            grass->SetConeSpread(300.0f);
            grass->SetParticleSpeedScale(0.18f);
            grass->SetParticleTextureFactor(1.0f);
            grass->SetParticleAutoRotate(true);
            grass->SetParticleRotationSpeed(1.0f);
            grass->SetParticleDrawOrder(1003);
            switch (mWallPosition) {
                case WallSide::Top:
                    grass->SetEmitDirection(Vector2::UnitY);
                    grass->SetPosition(GetPosition() - Vector2(0, mHeight * 0.3f));
                    break;

                case WallSide::Bottom:
                    grass->SetEmitDirection(Vector2::NegUnitY);
                    grass->SetPosition(GetPosition() + Vector2(0, mHeight * 0.3f));
                    break;

                case WallSide::Left:
                    grass->SetEmitDirection(Vector2::UnitX);
                    grass->SetPosition(GetPosition() - Vector2(mWidth * 0.3f, 0));
                    break;

                case WallSide::Right:
                    grass->SetEmitDirection(Vector2::NegUnitX);
                    grass->SetPosition(GetPosition() + Vector2(mWidth * 0.3f, 0));
                    break;
            }
            break;
    }
}
