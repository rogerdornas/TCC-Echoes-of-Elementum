//
// Created by roger on 28/06/2025.
//

#include "Golem.h"
#include "../Effect.h"
#include "../Lever.h"
#include "../ParticleSystem.h"
#include "../Skill.h"
#include "../../Game.h"
#include "../../HUD.h"
#include "../../Components/RigidBodyComponent.h"
#include "../../Components/AABBComponent.h"
#include "../../Components/Drawing/AnimatorComponent.h"
#include "../../Components/Drawing/RectComponent.h"
#include "../../Components/CombatBoxComponent.h"
#include "../FireBall.h"
#include "../../Random.h"

Golem::Golem(Game *game)
    :Enemy(game)
    ,mGolemState(State::Stop)

    ,mIsRunning(false)
    ,mGravity(3000)

    ,mIsInvulnerable(false)
    ,mAlreadySpawnedCrystal(false)
    ,mCrystalWidth(108)
    ,mCrystalHeight(72)

    ,mStopDuration(1.2f)
    ,mStopTimer(0.0f)

    ,mHitDuration(0.3f)
    ,mHitTimer(0.0f)

    ,mRunAwayDuration(0.8f)
    ,mRunAwayTimer(0.0f)
    ,mMoveSpeedIncrease(1.0f)

    ,mPunchProbability(0.5f)
    ,mPunchDuration(0.4f)
    ,mPunchTimer(0.0f)
    ,mDistToPunch(200)
    ,mPunchDirectionRight(true)

    ,mFireballDuration(1.0f)
    ,mFireballTimer(0.0f)
    ,mAlreadyFireBalled(false)
    ,mFireballWidth(100)
    ,mFireBallHeight(100)
    ,mFireballSpeed(1400)
    ,mFireballDamage(15)

    ,mFireballRainIntervalDuration(0.6f)
    ,mFireballRainIntervalTimer(0.0f)
    ,mFireballRainWidth(70)
    ,mFireballRainHeight(70)
    ,mFireballRainSpeed(850)
    ,mFireballRainDamage(15)
    ,mFireballRainPositionYOffset(30)

    ,mRunningSoundIntervalDuration(0.3f)
    ,mRunningSoundIntervalTimer(0.0f)
{
    mWidth = 144;
    mHeight = 190;
    mMoveSpeed = 600;
    mHealthPoints = 400;
    mMaxHealthPoints = mHealthPoints;
    mContactDamage = 10;
    mMoneyDrop = 50;
    mKnockBackSpeed = 0.0f;
    mKnockBackDuration = 0.0f;
    mKnockBackTimer = mKnockBackDuration;
    mIdleWidth = mWidth;
    mPunchSpriteWidth = mWidth * 1.5f;
    mPunchOffsetHitBox = mWidth * 0.8f;
    mPunchOffset = Vector2(mWidth * 0.8f, 0);
    mFreezeMax = 1000;
    mFrozenDecayRate = mFreezeMax / 3.0f;

    SetSize(mWidth, mHeight);

    mDrawComponent = new AnimatorComponent(this, "../Assets/Sprites/Golem2/Golem.png",
                                                    "../Assets/Sprites/Golem2/Golem.json",
                                                    mWidth * 1.8f * 1.73f, mWidth * 1.8f, 998);
    std::vector idle = {54, 22, 23, 24, 55, 25, 58, 26};
    mDrawComponent->AddAnimation("idle", idle);

    std::vector idleInvulnerable = {56, 27, 28, 29, 57, 30, 59, 31};
    mDrawComponent->AddAnimation("idleInvulnerable", idleInvulnerable);

    std::vector walk = {32, 33, 34, 35, 36, 37, 38, 39, 40, 41};
    mDrawComponent->AddAnimation("walk", walk);

    std::vector walkInvulnerable = {42, 43, 44, 45, 46, 47, 48, 49, 50, 51};
    mDrawComponent->AddAnimation("walkInvulnerable", walkInvulnerable);

    std::vector punch = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    mDrawComponent->AddAnimation("punch", punch);

    std::vector punchInvulnerable = {10, 11, 12, 13, 14, 15, 16, 17, 18, 19};
    mDrawComponent->AddAnimation("punchInvulnerable", punchInvulnerable);

    std::vector hit = {52, 20, 21, 53};
    mDrawComponent->AddAnimation("hit", hit);

    mDrawComponent->SetAnimation("idle");
    mDrawComponent->SetAnimFPS(10.0f);

    mCombatBoxComponent->AddAABBBox("punch", true, Vector2(-80, -40), Vector2(80, 40));
    mCombatBoxComponent->SetBoxActive("punch", false);
    // mCombatBoxComponent->SetDebugDraw(true);
}

void Golem::OnUpdate(float deltaTime) {
    if (mFlashTimer < mHitDuration) {
        mFlashTimer += deltaTime;
    }
    else {
        mIsFlashing = false;
    }

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

    ResolveGroundCollision();
    ResolveEnemyCollision();
    ManageFreezing(deltaTime);
    ManageStun(deltaTime);

    // Gravidade
    mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x,
                                             mRigidBodyComponent->GetVelocity().y
                                             + mGravity * deltaTime));

    if (!mIsFrozen && !mIsStunned) {
        if (mPlayerSpotted) {
            mGame->GetHUD()->StartBossFight(this);
            if (!mGame->GetBossMusicHandle().IsValid()) {
                mGame->StartBossMusic(mGame->GetAudio()->PlaySound("MantisLords.wav", true, SoundCategory::Music));
            }
            MovementAfterPlayerSpotted(deltaTime);
        }
        else {
            MovementBeforePlayerSpotted();
        }
    }

    // Se morreu
    if (Died()) {
        TriggerBossDefeat();
    }

    ControlSpawCrystal();

    // Controla sons de passos
    if (mGolemState == State::RunForward) {
        mRunningSoundIntervalTimer += deltaTime;
        if (mRunningSoundIntervalTimer >= mRunningSoundIntervalDuration) {
            mRunningSoundIntervalTimer -= mRunningSoundIntervalDuration;
            if (IsOnScreen()) {
                mGame->GetAudio()->PlaySound("GolemSteps/GolemSteps.wav");
            }
        }
    }

    if (!mIsFrozen && !mIsStunned) {
        if (mDrawComponent) {
            ManageAnimations();
        }
    }

    if (mCombatBoxComponent) {
        ManageCombatBox();
    }

    if (mHealthPoints <= 0.65f * mMaxHealthPoints) {
        mStopDuration = 1.0f;
        mFireballDuration = 0.7f;
        mMoveSpeedIncrease = 1.6f;
        mRunningSoundIntervalDuration = 0.2;
        FireballRain(deltaTime);
    }
}

void Golem::MovementBeforePlayerSpotted() {

}

void Golem::MovementAfterPlayerSpotted(float deltaTime) {
    switch (mGolemState) {
        case State::Stop:
            Stop(deltaTime);
            break;

        case State::RunAway:
            RunAway(deltaTime);
            break;

        case State::RunForward:
            RunForward(deltaTime);
            break;

        case State::Punch:
            Punch(deltaTime);
            break;

        case State::Fireball:
            Fireball(deltaTime);
            break;
    }
}

void Golem::Stop(float deltaTime) {
    mRigidBodyComponent->SetVelocity(Vector2(0, mRigidBodyComponent->GetVelocity().y));
    mStopTimer += deltaTime;
    if (mStopTimer >= mStopDuration) {
        mStopTimer = 0;
        if (Random::GetFloat() < mPunchProbability) {
            mGolemState = State::RunForward;
        }
        else {
            mGolemState = State::Fireball;
        }

        // Controla probabilidade de soco para não ficar spamando
        if (mGolemState == State::RunForward) {
            mPunchProbability -= 0.1;
        }
        else {
            mPunchProbability += 0.1;
        }
    }
}

void Golem::RunAway(float deltaTime) {
    mIsRunning = true;
    mRunAwayTimer += deltaTime;
    if (mRunAwayTimer >= mRunAwayDuration) {
        mRunAwayTimer = 0;
        mGolemState = State::Stop;
        return;
    }

    Player* player = GetGame()->GetPlayer();
    float dist = GetPosition().x - player->GetPosition().x;

    if (dist > 0) {
        SetRotation(0.0);
        SetScale(Vector2(1, 1));
    }
    else {
        SetRotation(Math::Pi);
        SetScale(Vector2(-1, 1));
    }
    mRigidBodyComponent->SetVelocity(Vector2(GetForward().x * mMoveSpeed, mRigidBodyComponent->GetVelocity().y));
}

void Golem::RunForward(float deltaTime) {
    mIsRunning = true;
    mRigidBodyComponent->SetVelocity(Vector2(GetForward().x * mMoveSpeed * mMoveSpeedIncrease, mRigidBodyComponent->GetVelocity().y));

    Player* player = GetGame()->GetPlayer();
    float dist = GetPosition().x - player->GetPosition().x;

    if (Math::Abs(dist) < mDistToPunch) {
        // if (mDrawAnimatedComponent) {
        //     mDrawAnimatedComponent->ResetAnimationTimer();
        // }
        if (GetRotation() == 0) {
            mPunchDirectionRight = true;
        }
        else {
            mPunchDirectionRight = false;
        }
        mGolemState = State::Punch;
        return;
    }
}

void Golem::Punch(float deltaTime) {
    mPunchTimer += deltaTime;
    mRigidBodyComponent->SetVelocity(Vector2(0, 0));
    if (mPunchDirectionRight) {
        SetRotation(0);
        SetScale(Vector2(1, 1));
    }
    else {
        SetRotation(Math::Pi);
        SetScale(Vector2(-1, 1));
    }

    if (mPunchTimer >= mPunchDuration) {
        mPunchTimer = 0;
        mGolemState = State::Stop;
    }
}


void Golem::Fireball(float deltaTime) {
    mFireballTimer += deltaTime;
    if (mFireballTimer >= mFireballDuration) {
        mFireballTimer = 0;
        mAlreadyFireBalled = false;
        if (Random::GetFloat() < 0.0) {
            mGolemState = State::Fireball;
        }
        else {
            mGolemState = State::Stop;
        }
    }

    if (!mAlreadyFireBalled) {
        std::vector<FireBall* > fireBalls = GetGame()->GetFireBalls();
        for (FireBall* f: fireBalls) {
            if (f->GetState() == ActorState::Paused) {
                f->SetState(ActorState::Active);
                f->SetRotation(GetRotation());
                f->SetTransformRotation(0.0f);
                f->SetScale(Vector2(GetForward().x, 1));
                f->SetWidth(mFireballWidth);
                f->SetHeight(mFireBallHeight);
                f->SetSpeed(mFireballSpeed);
                f->SetDamage(mFireballDamage);
                f->SetIsFromEnemy();
                f->SetPosition(GetPosition() + f->GetForward() * (f->GetWidth() / 2));
                break;
            }
        }
        mAlreadyFireBalled = true;
    }
}

void Golem::FireballRain(float deltaTime) {
    mFireballRainIntervalTimer += deltaTime;
    if (mFireballRainIntervalTimer >= mFireballRainIntervalDuration) {
        mFireballRainIntervalTimer = 0;
        float positionX = Random::GetFloatRange(mArenaMinPos.x, mArenaMaxPos.x);
        float positionY = mArenaMinPos.y + mFireballRainPositionYOffset;
        Vector2 position(positionX, positionY);

        std::vector<FireBall* > fireBalls = GetGame()->GetFireBalls();
        for (FireBall* f: fireBalls) {
            if (f->GetState() == ActorState::Paused) {
                f->SetState(ActorState::Active);
                f->SetRotation(Math::PiOver2);
                f->SetTransformRotation(Math::PiOver2);
                f->SetScale(Vector2(1, 1));
                f->SetWidth(mFireballRainWidth);
                f->SetHeight(mFireballRainHeight);
                f->SetSpeed(mFireballRainSpeed);
                f->SetDamage(mFireballRainDamage);
                f->SetIsFromEnemy();
                f->SetPosition(position);
                break;
            }
        }
    }
}


void Golem::ControlSpawCrystal() {
    if (!mAlreadySpawnedCrystal) {
        if (mHealthPoints > 0.7f * mMaxHealthPoints && mHealthPoints <= 0.8f * mMaxHealthPoints) {
            Vector2 position(Random::GetFloatRange(mArenaMinPos.x, mArenaMaxPos.x),
                            mArenaMaxPos.y);
            SpawCrystal(position);
            mAlreadySpawnedCrystal = true;
        }
    }

    if (mHealthPoints > 0.6f * mMaxHealthPoints && mHealthPoints <= 0.7f * mMaxHealthPoints) {
        mAlreadySpawnedCrystal = false;
    }

    if (!mAlreadySpawnedCrystal) {
        if (mHealthPoints > 0.5f * mMaxHealthPoints && mHealthPoints <= 0.6f * mMaxHealthPoints) {
            Vector2 position(Random::GetFloatRange(mArenaMinPos.x, mArenaMaxPos.x),
                            mArenaMaxPos.y);
            SpawCrystal(position);
            mAlreadySpawnedCrystal = true;
        }
    }

    if (mHealthPoints > 0.4f * mMaxHealthPoints && mHealthPoints <= 0.5f * mMaxHealthPoints) {
        mAlreadySpawnedCrystal = false;
    }

    if (!mAlreadySpawnedCrystal) {
        if (mHealthPoints > 0.3f * mMaxHealthPoints && mHealthPoints <= 0.4f * mMaxHealthPoints) {
            Vector2 position(Random::GetFloatRange(mArenaMinPos.x, mArenaMaxPos.x),
                            mArenaMaxPos.y);
            SpawCrystal(position);
            mAlreadySpawnedCrystal = true;
        }
    }

    if (mHealthPoints > 0.2f * mMaxHealthPoints && mHealthPoints <= 0.3f * mMaxHealthPoints) {
        mAlreadySpawnedCrystal = false;
    }
}


void Golem::SpawCrystal(Vector2 position) {
    mIsInvulnerable = true;
    auto* crystal = new Lever(mGame, mCrystalWidth, mCrystalHeight, Lever::LeverType::Crystal);
    crystal->SetPosition(position);
    crystal->SetTarget("Enemy");
    crystal->SetEvent("GolemVulnerable");
    crystal->SetEnemiesIds(std::vector<int>{mId});
}


void Golem::TriggerBossDefeat() {
    // Player ganha bola de fogo
    if (!mGame->GetPlayer()->GetCanFireBall()) {
        auto* skill = new Skill(mGame, Skill::SkillType::FireBall);
        skill->SetPosition(GetPosition());
    }

    mGame->SetWorldFlag("GolemDefeated", true);
    mGame->StopBossMusic();
}

void Golem::ReceiveHit(float damage, Vector2 knockBackDirection, bool knockBack) {
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

    if (mIsInvulnerable) {
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

    auto* blood = new ParticleSystem(mGame, Particle::ParticleType::SolidParticle, 10, 170.0, 3.0, 0.07f);
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


void Golem::ManageAnimations() {
    mDrawComponent->SetAnimFPS(10.0f);
    if (mGolemState == State::Punch) {
        if (mIsInvulnerable) {
            mDrawComponent->SetAnimation("punchInvulnerable");
        }
        else {
            mDrawComponent->SetAnimation("punch");
        }
        mDrawComponent->SetAnimFPS(10.0f / mPunchDuration);
    }
    else if (mGolemState == State::RunForward ||
             mGolemState == State::RunAway) {
        if (mIsInvulnerable) {
            mDrawComponent->SetAnimation("walkInvulnerable");
        }
        else {
            mDrawComponent->SetAnimation("walk");
        }
        mDrawComponent->SetAnimFPS(mMoveSpeed / 40);
    }
    else if (mIsFlashing) {
        mDrawComponent->SetAnimation("hit");
        mDrawComponent->SetAnimFPS(4.0f / mHitDuration);
    }
    else {
        if (mIsInvulnerable) {
            mDrawComponent->SetAnimation("idleInvulnerable");
        }
        else {
            mDrawComponent->SetAnimation("idle");
        }
        mDrawComponent->SetAnimFPS(10.0f);
    }
}

void Golem::ManageCombatBox() {
    if (mGolemState == State::Punch) {
        if (mPunchTimer > 0.41f * mPunchDuration && mPunchTimer < 0.78f * mPunchDuration) {
            mCombatBoxComponent->SetBoxActive("punch", true);
            mCombatBoxComponent->SetBoxOffset("punch", mPunchOffset * Vector2(GetForward().x, 1));
            mCombatBoxComponent->SetBoxOffset("hitbox", (mPunchOffset * 0.6f) * Vector2(GetForward().x, 1));
            mCombatBoxComponent->SetBoxOffset("hurtbox", (mPunchOffset * 0.6f) * Vector2(GetForward().x, 1));
        }
        else {
            mCombatBoxComponent->SetBoxActive("punch", false);
            mCombatBoxComponent->SetBoxOffset("punch", Vector2::Zero);
            mCombatBoxComponent->SetBoxOffset("hitbox", Vector2::Zero);
            mCombatBoxComponent->SetBoxOffset("hurtbox", Vector2::Zero);
        }
    }
}
