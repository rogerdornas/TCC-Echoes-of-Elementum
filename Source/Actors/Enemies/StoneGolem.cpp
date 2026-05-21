//
// Created by roger on 05/04/2026.
//

#include "StoneGolem.h"
#include <algorithm>
#include "../Actor.h"
#include "../Effect.h"
#include "../Laser.h"
#include "../GolemProjectile.h"
#include "../LaserShooter.h"
#include "../../Game.h"
#include "../../HUD.h"
#include "../../Random.h"
#include "../../Components/RigidBodyComponent.h"
#include "../../Components/Drawing/AnimatorComponent.h"
#include "../../Components/CombatBoxComponent.h"

StoneGolem::StoneGolem(Game *game)
    :Enemy(game)
    ,mEnemyState(State::Sleep)
    ,mCurrentPhase(1)
    ,mPhaseTransitionPending(false)

    ,mDistToSpotPlayer(400.0f)

    ,mTargetHeight(0.0f)

    ,mStopDuration(0.8f)
    ,mStopTimer(0.0f)

    ,mRunDuration(1.5f)
    ,mRunTimer(0.0f)
    ,mIsAerialReposition(false)
    ,mRepositionStartX(0.0f)

    ,mHitDuration(0.07f)

    ,mWakeUpDuration(1.0f)
    ,mWakeUpTimer(0.0f)

    ,mDistToMeleeAttack(250.0f)
    ,mMeleeAttackDuration(0.9f)
    ,mMeleeAttackTimer(0.0f)

    ,mDistToRangeAttack(700.0f)
    ,mProjectileAttackDuration(1.0f)
    ,mProjectileAttackTimer(0.0f)
    ,mProjectileShootTime(0.88f)
    ,mAlreadyProjectileAttack(false)

    ,mLaserCastDuration(1.5f)
    ,mLaserCastTimer(0.0f)
    ,mLaserTargetTime(0.6f)
    ,mLaserTargetAngle(0.0f)
    ,mAlreadySetLaserAngle(false)

    ,mLaserShootDuration(1.0f)
    ,mLaserShootTimer(0.0f)
    ,mLaserDamage(15.0f)
    ,mAlreadyLaserShoot(false)

    ,mImmuneCastDuration(0.5f)
    ,mImmuneCastTimer(0.0f)

    ,mIsImmune(false)
    ,mImmuneDuration(4.0f)
    ,mImmuneTimer(0.0f)

    ,mImmuneRecoverDuration(0.5f)
    ,mImmuneRecoverTimer(0.0f)

    ,mAlreadyActivateLaserShooters(false)
    ,mActivateLaserShootersTime(0.2f)
    ,mNumDeactivateTopLaserShooters(2)
    ,mLaserShootCountMax(3)
    ,mLaserShootCount(0)
{
    mWidth = 110;
    mHeight = 110;
    mMoveSpeed = 300;
    mHealthPoints = 1300;
    mMaxHealthPoints = mHealthPoints;
    mContactDamage = 15;
    mMoneyDrop = 100;
    mKnockBackSpeed = 0.0f;
    mKnockBackDuration = 0.0f;
    mKnockBackTimer = mKnockBackDuration;
    mLaserHeight = mHeight * 0.68f;
    mLaserOffset = Vector2(mWidth * 0.11f, mHeight * -0.59f);
    mFreezeMax = 1000;
    mFrozenDecayRate = mFreezeMax / 3.0f;
    mBloodColor = {148, 133, 134, 255};
    mEnemyCollision = false;

    SetSize(mWidth, mHeight);

    mDrawComponent = new AnimatorComponent(this, "../Assets/Sprites/StoneGolem2/StoneGolem.png",
                                                    "../Assets/Sprites/StoneGolem2/StoneGolem.json",
                                                    mWidth * 3.5f, mHeight * 3.5f, 95);
    std::vector armorBuff = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    mDrawComponent->AddAnimation("armorBuff", armorBuff);

    std::vector sleep = {10};
    mDrawComponent->AddAnimation("sleep", sleep);

    std::vector wakeUp = {10, 85, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22};
    mDrawComponent->AddAnimation("wakeUp", wakeUp);

    std::vector glowing = {23, 24, 25, 26, 27, 28, 29, 30};
    mDrawComponent->AddAnimation("glowing", glowing);

    std::vector idle = {79, 84, 31, 32};
    mDrawComponent->AddAnimation("idle", idle);

    std::vector immuneCast = {80, 33, 34, 35, 36, 37, 38};
    mDrawComponent->AddAnimation("immuneCast", immuneCast);

    std::vector immune = {39, 39, 40, 40};
    mDrawComponent->AddAnimation("immune", immune);

    std::vector immuneRecover = {38, 37, 36, 35, 34, 33, 80};
    mDrawComponent->AddAnimation("immuneRecover", immuneRecover);

    std::vector laserCast = {81, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54};
    mDrawComponent->AddAnimation("laserCast", laserCast);

    std::vector laserShoot = {55, 56, 57, 58, 59, 60};
    mDrawComponent->AddAnimation("laserShoot", laserShoot);

    std::vector meleeAttack = {82, 82, 61, 61, 62, 62, 63, 63, 64, 64, 65, 65, 66, 67, 68, 69, 70};
    mDrawComponent->AddAnimation("meleeAttack", meleeAttack);

    std::vector rangeAttack = {83, 71, 72, 73, 74, 75, 76, 77, 78};
    mDrawComponent->AddAnimation("rangeAttack", rangeAttack);

    mDrawComponent->SetAnimation("sleep");
    mDrawComponent->SetAnimFPS(8.0f);

    mCombatBoxComponent->AddAABBBox("punch", true, Vector2(-20, -60), Vector2(20, 60));
    mCombatBoxComponent->SetBoxActive("punch", false);
    // mCombatBoxComponent->SetDebugDraw(true);
}

void StoneGolem::OnUpdate(float deltaTime) {
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
            mGame->GetHUD()->StartBossFight(this);
            if (!mGame->GetBossMusicHandle().IsValid()) {
                mGame->StartBossMusic(mGame->GetAudio()->PlaySound("MantisLords.wav", true, SoundCategory::Music));
            }
        }
        MovementAfterPlayerSpotted(deltaTime);
    }

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
}

void StoneGolem::TriggerBossDefeat() {
    mGame->StopBossMusic();

    // Destroy Laser Shooters
    for (int topLaserShooter : mTopLaserShooters) {
        if (auto* ls = mGame->GetLaserShooterById(topLaserShooter)) {
            ls->Destroy();
        }
    }
    for (int leftLaserShooter : mLeftLaserShooters) {
        if (auto* ls = mGame->GetLaserShooterById(leftLaserShooter)) {
            ls->Destroy();
        }
    }
    for (int rightLaserShooter : mRightLaserShooters) {
        if (auto* ls = mGame->GetLaserShooterById(rightLaserShooter)) {
            ls->Destroy();
        }
    }
}

void StoneGolem::MovementAfterPlayerSpotted(float deltaTime) {
    switch (mEnemyState) {
        case State::Sleep:
            Sleep(deltaTime);
        break;

        case State::WakeUp:
            WakeUp(deltaTime);
        break;

        case State::Stop:
            Stop(deltaTime);
        break;

        case State::Chase:
            Chase(deltaTime);
        break;

        case State::Reposition:
            Reposition(deltaTime);
        break;

        case State::MoveToCenter:
            MoveToCenter(deltaTime);
        break;

        case State::MeleeAttack:
            MeleeAttack(deltaTime);
        break;

        case State::ProjectileAttack:
            ProjectileAttack(deltaTime);
        break;

        case State::LaserCast:
            LaserCast(deltaTime);
        break;

        case State::LaserShoot:
            LaserShoot(deltaTime);
        break;

        case State::ImmuneCast:
            ImmuneCast(deltaTime);
        break;

        case State::Immune:
            Immune(deltaTime);
        break;

        case State::ImmuneRecover:
            ImmuneRecover(deltaTime);
        break;
    }
}

void StoneGolem::Sleep(float deltaTime) {
    mRigidBodyComponent->SetVelocity(Vector2(0, mRigidBodyComponent->GetVelocity().y));

    // Testa se spotted player
    Vector2 dist = GetPosition() - GetGame()->GetPlayer()->GetPosition();
    if (dist.Length() < mDistToSpotPlayer) {
        mPlayerSpotted = true;
    }

    if (mPlayerSpotted) {
        mTargetHeight = GetPosition().y + mHeight * 0.4f;
        mEnemyState = State::WakeUp;
    }
}

void StoneGolem::WakeUp(float deltaTime) {
    mWakeUpTimer += deltaTime;
    if (mWakeUpTimer >= mWakeUpDuration) {
        mWakeUpTimer = 0.0f;
        mEnemyState = State::Stop;
        return;
    }

    if (mKnockBackTimer >= mKnockBackDuration) {
        mRigidBodyComponent->SetVelocity(Vector2(0, mRigidBodyComponent->GetVelocity().y));
    }
}

void StoneGolem::Stop(float deltaTime) {
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
        SetScale(Vector2(1,1));
    }
    else {
        SetRotation(Math::Pi);
        SetScale(Vector2(-1,1));
    }

    // LÓGICA DE MANUTENÇÃO DE ALTURA
    float distY = mTargetHeight - GetPosition().y;

    float pullStrength = 3.0f;

    // Velocidade base para retornar ao alvo
    float baseVelY = distY * pullStrength;

    float hoverFrequency = 6.0f;
    float hoverAmplitude = 80.0f;
    float oscillationY = Math::Cos(mRunTimer * hoverFrequency) * hoverAmplitude;

    float velY = baseVelY + oscillationY;

    if (mKnockBackTimer >= mKnockBackDuration) {
        mRigidBodyComponent->SetVelocity(Vector2(0, velY));
    }
}

void StoneGolem::Chase(float deltaTime) {
    Player* player = GetGame()->GetPlayer();
    float distToPlayerX = player->GetPosition().x - GetPosition().x;

    mRunTimer += deltaTime;
    if (mRunTimer >= mRunDuration || Math::Abs(distToPlayerX) <= mDistToRangeAttack) {
        mRigidBodyComponent->SetVelocity(Vector2::Zero);
        mRunTimer = 0.0f;
        mEnemyState = State::Stop;
        return;
    }

    if (distToPlayerX > 0) {
        SetRotation(0.0);
        SetScale(Vector2(1,1));
    } else {
        SetRotation(Math::Pi);
        SetScale(Vector2(-1,1));
    }

    float directionX = (distToPlayerX > 0) ? 1.0f : -1.0f;
    float velX = directionX * mMoveSpeed;

    // LÓGICA DE MANUTENÇÃO DE ALTURA
    float distY = mTargetHeight - GetPosition().y;

    float pullStrength = 3.0f;

    // Velocidade base para retornar ao alvo
    float baseVelY = distY * pullStrength;

    float hoverFrequency = 6.0f;
    float hoverAmplitude = 80.0f;
    float oscillationY = Math::Cos(mRunTimer * hoverFrequency) * hoverAmplitude;

    float velY = baseVelY + oscillationY;

    if (mKnockBackTimer >= mKnockBackDuration) {
        mRigidBodyComponent->SetVelocity(Vector2(velX, velY));
    }
}

void StoneGolem::Reposition(float deltaTime) {
    float distToTargetX = mRepositionTarget.x - GetPosition().x;

    mRunTimer += deltaTime;
    if (mRunTimer >= mRunDuration || std::abs(distToTargetX) < 10.0f) {
        mRigidBodyComponent->SetVelocity(Vector2::Zero);
        mRunTimer = 0.0f;
        mEnemyState = State::Stop;
        return;
    }

    // Direção para onde o Golem quer ir (1 = Direita, -1 = Esquerda)
    float directionX = (distToTargetX > 0) ? 1.0f : -1.0f;

    float currentTargetHeight = mTargetHeight;
    float speedMultiplier = 1.8f;
    float pullStrength = 3.0f;

    // Se a decisão inicial foi fazer o arco por cima do player:
    if (mIsAerialReposition) {
        float totalDist = mRepositionTarget.x - mRepositionStartX;
        float currentDist = GetPosition().x - mRepositionStartX;
        float progress = 0.0f;

        if (std::abs(totalDist) > 0.01f) {
            progress = currentDist / totalDist;
        }

        // Trava o progresso entre 0 e 1 por segurança
        if (progress < 0.0f) progress = 0.0f;
        if (progress > 1.0f) progress = 1.0f;

        // Subida quase vertical, voo plano, descida vertical
        float rawSine = Math::Sin(progress * Math::Pi);

        // "steepness" controla a agressividade da subida.
        float steepness = 4.0f;

        // Multiplicamos e cortamos em 1.0 para criar o teto plano
        float curveShape = Math::Min(1.0f, rawSine * steepness);

        float arcHeight = curveShape * (mHeight * 2.5f);

        currentTargetHeight = mTargetHeight - arcHeight;
        speedMultiplier = 2.3f;
        pullStrength = 5.0f;
    }

    float velX = directionX * mMoveSpeed * speedMultiplier;

    // LÓGICA DE MANUTENÇÃO DE ALTURA
    float distY = currentTargetHeight - GetPosition().y;

    // Velocidade base para retornar ao alvo
    float baseVelY = distY * pullStrength;

    float hoverFrequency = 6.0f;
    float hoverAmplitude = 80.0f;
    float oscillationY = Math::Cos(mRunTimer * hoverFrequency) * hoverAmplitude;

    float velY = baseVelY + oscillationY;

    if (mKnockBackTimer >= mKnockBackDuration) {
        mRigidBodyComponent->SetVelocity(Vector2(velX, velY));
    }
}

void StoneGolem::MoveToCenter(float deltaTime) {
    float distToTargetX = mRepositionTarget.x - GetPosition().x;

    mRunTimer += deltaTime;
    if (mRunTimer >= mRunDuration || std::abs(distToTargetX) < 10.0f) {
        mRigidBodyComponent->SetVelocity(Vector2::Zero);
        mRunTimer = 0.0f;
        mEnemyState = State::ImmuneCast;
        return;
    }

    float directionX = (distToTargetX > 0) ? 1.0f : -1.0f;
    float velX = directionX * mMoveSpeed * 1.8f;

    // LÓGICA DE MANUTENÇÃO DE ALTURA
    float distY = mTargetHeight - GetPosition().y;

    float pullStrength = 3.0f;

    // Velocidade base para retornar ao alvo
    float baseVelY = distY * pullStrength;

    float hoverFrequency = 6.0f;
    float hoverAmplitude = 80.0f;
    float oscillationY = Math::Cos(mRunTimer * hoverFrequency) * hoverAmplitude;

    float velY = baseVelY + oscillationY;

    if (mKnockBackTimer >= mKnockBackDuration) {
        mRigidBodyComponent->SetVelocity(Vector2(velX, velY));
    }
}

void StoneGolem::MeleeAttack(float deltaTime) {
    mMeleeAttackTimer += deltaTime;
    if (mMeleeAttackTimer >= mMeleeAttackDuration) {
        mMeleeAttackTimer = 0.0f;
        mEnemyState = State::Stop;
        return;
    }

    if (mKnockBackTimer >= mKnockBackDuration) {
        mRigidBodyComponent->SetVelocity(Vector2::Zero);
    }
}

void StoneGolem::ProjectileAttack(float deltaTime) {
    mProjectileAttackTimer += deltaTime;
    if (mProjectileAttackTimer >= mProjectileAttackDuration) {
        mProjectileAttackTimer = 0.0f;
        mAlreadyProjectileAttack = false;
        mEnemyState = State::Stop;
        return;
    }

    if (mKnockBackTimer >= mKnockBackDuration) {
        mRigidBodyComponent->SetVelocity(Vector2::Zero);
    }

    Player* player = GetGame()->GetPlayer();

    if (!mAlreadyProjectileAttack) {
        float dist = GetPosition().x - player->GetPosition().x;
        if (dist < 0) {
            SetRotation(0.0);
            SetScale(Vector2(1,1));
        }
        else {
            SetRotation(Math::Pi);
            SetScale(Vector2(-1,1));
        }
    }

    if (!mAlreadyProjectileAttack && mProjectileAttackTimer >= mProjectileAttackDuration * mProjectileShootTime) {
        auto* projectile = new GolemProjectile(mGame);
        Vector2 toPlayer = player->GetPosition() - GetPosition();
        if (toPlayer.x > 0) {
            projectile->SetRotation(0.0f);
            projectile->SetTransformRotation(0.0f);
        }
        else {
            projectile->SetRotation(Math::Pi);
            projectile->SetTransformRotation(Math::Pi);
        }
        projectile->SetPosition(GetPosition() + Vector2(mWidth * 1.1f * GetForward().x, mHeight * (-0.5f)));
        mAlreadyProjectileAttack = true;
    }
}

void StoneGolem::LaserCast(float deltaTime) {
    mLaserCastTimer += deltaTime;
    if (mLaserCastTimer >= mLaserCastDuration) {
        mLaserCastTimer = 0.0f;
        mAlreadySetLaserAngle = false;
        mEnemyState = State::LaserShoot;
        return;
    }

    if (mKnockBackTimer >= mKnockBackDuration) {
        mRigidBodyComponent->SetVelocity(Vector2::Zero);
    }

    Player* player = GetGame()->GetPlayer();

    if (!mAlreadySetLaserAngle) {
        float dist = GetPosition().x - player->GetPosition().x;
        if (dist < 0) {
            SetRotation(0.0);
            SetScale(Vector2(1,1));
        }
        else {
            SetRotation(Math::Pi);
            SetScale(Vector2(-1,1));
        }
    }

    if (!mAlreadySetLaserAngle && mLaserCastTimer >= mLaserCastDuration * mLaserTargetTime) {
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
        laser->SetStartPositionOffset(mHeight * 0.53f);
        laser->SetPlayerCollision(false);
        laser->ShowLaserExplosion(false);
        laser->SetRotation(mLaserTargetAngle);
        laser->SetTransformRotation(mLaserTargetAngle);
        laser->CropFromEnd();
        laser->Initialize();

        mAlreadySetLaserAngle = true;
    }

    if (mKnockBackTimer >= mKnockBackDuration) {
        mRigidBodyComponent->SetVelocity(Vector2(0, mRigidBodyComponent->GetVelocity().y));
    }
}

void StoneGolem::LaserShoot(float deltaTime) {
    mLaserShootTimer += deltaTime;
    if (mLaserShootTimer >= mLaserShootDuration) {
        mLaserShootTimer = 0.0f;
        mAlreadyLaserShoot = false;
        mEnemyState = State::Stop;
        return;
    }

    if (!mAlreadyLaserShoot) {
        Vector2 headPosition = GetPosition() + Vector2(mLaserOffset.x * GetForward().x, mLaserOffset.y);

        auto* laser = new Laser(mGame, mLaserHeight, mLaserShootDuration, mLaserDamage);
        laser->SetStartPosition(headPosition);
        laser->ShowLaserExplosion(true);
        laser->SetRotation(mLaserTargetAngle);
        laser->SetTransformRotation(mLaserTargetAngle);
        laser->Initialize();

        mAlreadyLaserShoot = true;
    }
}

void StoneGolem::ImmuneCast(float deltaTime) {
    mImmuneCastTimer += deltaTime;
    if (mImmuneCastTimer >= mImmuneCastDuration) {
        mImmuneCastTimer = 0;
        mEnemyState = State::Immune;
        return;
    }
}

void StoneGolem::Immune(float deltaTime) {
    mImmuneTimer += deltaTime;
    if (mImmuneTimer >= mImmuneDuration) {
        mIsImmune = false;
        mAlreadyActivateLaserShooters = false;
        mImmuneTimer = 0.0f;
        if (mLaserShootCount >= mLaserShootCountMax) {
            mLaserShootCount = 0;
            mEnemyState = State::ImmuneRecover;
        }
        else {
            mEnemyState = State::Immune;
        }
        return;
    }

    mIsImmune = true;

    if (!mAlreadyActivateLaserShooters && mImmuneTimer >= mImmuneDuration * mActivateLaserShootersTime) {
        if (Random::GetFloat() > 0.5f) {
            ActiveSideLaserShooters();
        }
        else {
            ActiveTopLaserShooters();
        }
        mAlreadyActivateLaserShooters = true;
        mLaserShootCount++;
    }
}

void StoneGolem::ImmuneRecover(float deltaTime) {
    mImmuneRecoverTimer += deltaTime;
    if (mImmuneRecoverTimer >= mImmuneRecoverDuration) {
        mImmuneRecoverTimer = 0.0f;
        mEnemyState = State::Stop;
        return;
    }
}

void StoneGolem::EvaluateNextState() {
    // Prioridade Máxima: Mudança de Fase
    if (mPhaseTransitionPending) {
        mPhaseTransitionPending = false;
        mRepositionTarget = Vector2(mArenaMinPos.x + (mArenaMaxPos.x - mArenaMinPos.x) * 0.5f, mTargetHeight);
        mEnemyState = State::MoveToCenter;
        return;
    }

    Player* player = GetGame()->GetPlayer();
    float distToPlayerX = std::abs(player->GetPosition().x - GetPosition().x);
    float distToPlayerY = player->GetPosition().y - GetPosition().y;

    // O player está perto do golem
    if (distToPlayerX <= mDistToMeleeAttack) {
        if (distToPlayerY > -10 && Random::GetFloat() < 0.6f) {
            mEnemyState = State::MeleeAttack;
        } else {
            if (Random::GetFloat() < 0.6f) {
                SetRepositionTarget();
                mEnemyState = State::Reposition;
            }
            else {
                if (Random::GetFloat() < 0.5f) {
                    mEnemyState = State::ProjectileAttack;
                }
                else {
                    mEnemyState = State::LaserCast;
                }
            }
        }
        return;
    }

    // O player está longe do golem
    if (distToPlayerX <= mDistToRangeAttack) {
        if (Random::GetFloat() < 0.5f) {
            mEnemyState = State::ProjectileAttack;
        } else {
            mEnemyState = State::LaserCast;
        }
        return;
    }

    // Player está muito longe
    mEnemyState = State::Chase;
}

void StoneGolem::SetRepositionTarget() {
    // Encontra o centro do eixo X da arena
    float arenaCenter = mArenaMinPos.x + (mArenaMaxPos.x - mArenaMinPos.x) * 0.5f;

    // Se o Golem está na metade esquerda, o alvo é a direita
    if (GetPosition().x < arenaCenter) {
        mRepositionTarget = Vector2(mArenaMaxPos.x - 200.0f, mTargetHeight);
    }
    // Se está na metade direita, o alvo é a esquerda
    else {
        mRepositionTarget = Vector2(mArenaMinPos.x + 200.0f, mTargetHeight);
    }

    mRepositionStartX = GetPosition().x;

    // Decide a rota: Chão ou Ar
    Player* player = GetGame()->GetPlayer();
    float distToTargetX = mRepositionTarget.x - GetPosition().x;
    float directionX = (distToTargetX > 0) ? 1.0f : -1.0f;
    float distToPlayerX = player->GetPosition().x - GetPosition().x;
    float playerDirectionX = (distToPlayerX > 0) ? 1.0f : -1.0f;
    float distToPlayerY = player->GetPosition().y - GetPosition().y;

    mIsAerialReposition = (directionX == playerDirectionX) &&
                          (std::abs(distToPlayerX) < std::abs(distToTargetX)) &&
                          (distToPlayerY > -50.0f);
}

void StoneGolem::PhaseChange() {
    float healthPercent = mHealthPoints / mMaxHealthPoints;

    if (mCurrentPhase == 1 && healthPercent <= 0.75f) {
        mCurrentPhase = 2;
        mLaserShootCountMax = 3;
        mStopDuration = 0.4f;
        mPhaseTransitionPending = true;
    }
    else if (mCurrentPhase == 2 && healthPercent <= 0.50f) {
        mCurrentPhase = 3;
        mLaserShootCountMax = 4;
        mStopDuration = 0.3f;
        mPhaseTransitionPending = true;
    }
    else if (mCurrentPhase == 3 && healthPercent <= 0.25f) {
        mCurrentPhase = 4;
        mLaserShootCountMax = 5;
        mStopDuration = 0.1f;
        mPhaseTransitionPending = true;
    }
}

void StoneGolem::ActiveSideLaserShooters() {
    for (int i = 0; i < mLeftLaserShooters.size(); i++) {
        if (Random::GetFloat() < 0.5f) {
            if (auto* ls = mGame->GetLaserShooterById(mLeftLaserShooters[i])) {
                ls->IncreaseShotsRemaining(1);
            }
        }
        else {
            if (auto* ls = mGame->GetLaserShooterById(mRightLaserShooters[i])) {
                ls->IncreaseShotsRemaining(1);
            }
        }
    }
}

void StoneGolem::ActiveTopLaserShooters() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(mTopLaserShooters.begin(), mTopLaserShooters.end(), gen);

    if (mTopLaserShooters.size() > mNumDeactivateTopLaserShooters) {
        size_t activeCount = mTopLaserShooters.size() - mNumDeactivateTopLaserShooters;
        for (int i = 0; i < activeCount; i++) {
            if (auto* ls = mGame->GetLaserShooterById(mTopLaserShooters[i])) {
                ls->IncreaseShotsRemaining(1);
            }
        }
    }
}

void StoneGolem::ReceiveHit(float damage, Vector2 knockBackDirection, bool knockBack) {
    if (mIsImmune) {
        if (knockBackDirection.y == 1) {
            if (IsOnScreen()) {
                mGame->GetAudio()->PlaySound("HitSpike/HitSpike1.wav");
            }
            for (int i = 0; i < 3; i++) {
                auto* sparkEffect = new Effect(mGame);
                sparkEffect->SetDuration(0.1f);
                sparkEffect->SetPosition(Vector2(mGame->GetPlayer()->GetPosition().x, GetPosition().y - mHeight * 0.3f));
                sparkEffect->SetEffect(TargetEffect::SwordHit);
            }
            return;
        }
        if (knockBackDirection.y == -1) {
            if (IsOnScreen()) {
                mGame->GetAudio()->PlaySound("HitSpike/HitSpike1.wav");
            }
            for (int i = 0; i < 3; i++) {
                auto* sparkEffect = new Effect(mGame);
                sparkEffect->SetDuration(0.1f);
                sparkEffect->SetPosition(Vector2(mGame->GetPlayer()->GetPosition().x, GetPosition().y + mHeight * 0.3f));
                sparkEffect->SetEffect(TargetEffect::SwordHit);
            }
            return;
        }
        if (knockBackDirection.x == 1) {
            if (IsOnScreen()) {
                mGame->GetAudio()->PlaySound("HitSpike/HitSpike1.wav");
            }
            for (int i = 0; i < 3; i++) {
                auto* sparkEffect = new Effect(mGame);
                sparkEffect->SetDuration(0.1f);
                float positionY = mGame->GetPlayer()->GetPosition().y;
                if (positionY < GetPosition().y - mHeight * 0.3f) {
                    positionY = GetPosition().y - mHeight * 0.3f;
                }
                sparkEffect->SetPosition(Vector2(GetPosition().x - mWidth * 0.3f, positionY));
                sparkEffect->SetEffect(TargetEffect::SwordHit);
            }
            return;
        }
        if (knockBackDirection.x == -1) {
            if (IsOnScreen()) {
                mGame->GetAudio()->PlaySound("HitSpike/HitSpike1.wav");
            }
            for (int i = 0; i < 3; i++) {
                auto* sparkEffect = new Effect(mGame);
                sparkEffect->SetDuration(0.1f);
                float positionY = mGame->GetPlayer()->GetPosition().y;
                if (positionY < GetPosition().y - mHeight * 0.3f) {
                    positionY = GetPosition().y - mHeight * 0.3f;
                }
                sparkEffect->SetPosition(Vector2(GetPosition().x + mWidth * 0.3f, positionY));
                sparkEffect->SetEffect(TargetEffect::SwordHit);
            }
            return;
        }
        if (IsOnScreen()) {
            mGame->GetAudio()->PlaySound("HitSpike/HitSpike1.wav");
        }
        for (int i = 0; i < 3; i++) {
            auto* sparkEffect = new Effect(mGame);
            sparkEffect->SetDuration(0.1f);
            sparkEffect->SetPosition(Vector2(GetPosition()));
            sparkEffect->SetEffect(TargetEffect::SwordHit);
        }
        return;
    }

    Enemy::ReceiveHit(damage, knockBackDirection, knockBack);

    PhaseChange();
}

void StoneGolem::ManageAnimations() {
    mDrawComponent->SetAnimFPS(7.0f);

    if (mEnemyState == State::Sleep) {
        mDrawComponent->SetAnimation("sleep");
    }
    else if (mEnemyState == State::WakeUp) {
        mDrawComponent->SetAnimation("wakeUp");
        mDrawComponent->SetAnimFPS(14.0f / mWakeUpDuration);
    }
    else if (mEnemyState == State::Stop) {
        mDrawComponent->SetAnimation("glowing");
    }
    else if (mEnemyState == State::Chase ||
             mEnemyState == State::Reposition ||
             mEnemyState == State::MoveToCenter)
    {
        mDrawComponent->SetAnimation("idle");
    }
    else if (mEnemyState == State::MeleeAttack) {
        mDrawComponent->SetAnimation("meleeAttack");
        mDrawComponent->SetAnimFPS(17.0f / mMeleeAttackDuration);
    }
    else if (mEnemyState == State::ProjectileAttack) {
        mDrawComponent->SetAnimation("rangeAttack");
        mDrawComponent->SetAnimFPS(9.0f / mProjectileAttackDuration);
    }
    else if (mEnemyState == State::LaserCast) {
        mDrawComponent->SetAnimation("laserCast");
        mDrawComponent->SetAnimFPS(15.0f / mLaserCastDuration);
    }
    else if (mEnemyState == State::LaserShoot) {
        mDrawComponent->SetAnimation("laserShoot");
        mDrawComponent->SetAnimFPS(6.0f / mLaserShootDuration);
    }
    else if (mEnemyState == State::ImmuneCast) {
        mDrawComponent->SetAnimation("immuneCast");
        mDrawComponent->SetAnimFPS(7.0f / mImmuneCastDuration);
    }
    else if (mEnemyState == State::Immune) {
        mDrawComponent->SetAnimation("immune");
        mDrawComponent->SetAnimFPS(12.0f);
    }
    else if (mEnemyState == State::ImmuneRecover) {
        mDrawComponent->SetAnimation("immuneRecover");
        mDrawComponent->SetAnimFPS(7.0f / mImmuneRecoverDuration);
    }
    if (mIsFlashing) {
        mDrawComponent->SetTextureFactor(0.0f);
    }
    else {
        mDrawComponent->SetTextureFactor(1.0f);
    }
}

void StoneGolem::ManageCombatBox() {
    if (mEnemyState == State::Sleep) {
        mCombatBoxComponent->SetBoxOffset("hitbox", Vector2(0, mHeight * 1.0f));
        mCombatBoxComponent->SetBoxOffset("hurtbox", Vector2(0, mHeight * 1.0f));
    }
    else if (mEnemyState == State::WakeUp) {
        if (mWakeUpTimer > 0.4f * mWakeUpDuration) {
            mCombatBoxComponent->SetBoxOffset("hitbox", Vector2::Zero);
            mCombatBoxComponent->SetBoxOffset("hurtbox", Vector2::Zero);
        }
    }
    else if (mEnemyState == State::MeleeAttack) {
        if (mMeleeAttackTimer > 0.8f * mMeleeAttackDuration) {
            mCombatBoxComponent->SetBoxActive("punch", true);
            mCombatBoxComponent->SetBoxOffset("punch", Vector2(mWidth * 1.0f, mHeight * 0.5f) * Vector2(GetForward().x, 1));
        }
        else {
            mCombatBoxComponent->SetBoxActive("punch", false);
            mCombatBoxComponent->SetBoxOffset("punch", Vector2::Zero);
        }
    }
    else {
        mCombatBoxComponent->SetBoxOffset("hitbox", Vector2::Zero);
        mCombatBoxComponent->SetBoxOffset("hurtbox", Vector2::Zero);
        mCombatBoxComponent->SetBoxActive("punch", false);
    }
}
