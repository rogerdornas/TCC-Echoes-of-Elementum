//
// Created by roger on 22/04/2025.
//

#include "Player.h"
#include <cfloat>
#include "AirGlideEffect.h"
#include "Checkpoint.h"
#include "DashEffect.h"
#include "GroundSlamImpactEffect.h"
#include "Effect.h"
#include "FireWisp.h"
#include "HookPoint.h"
#include "LightningEffect.h"
#include "LightningSpear.h"
#include "Enemies/Mushroom.h"
#include "PillarGround.h"
#include "VineRope.h"
#include "../Game.h"
#include "../PlayerSkillManager.h"
#include "../RadialMenu.h"
#include "../Random.h"
#include "../Actors/Sword.h"
#include "../Actors/JumpEffect.h"
#include "../Actors/FireBall.h"
#include "../Actors/ParticleSystem.h"
#include "../Actors/Light.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/AABBComponent.h"
#include "../Components/DashComponent.h"
#include "../Components/Drawing/AnimatorComponent.h"
#include "../Components/Drawing/RectComponent.h"
#include "../Components/Drawing/DrawRopeComponent.h"
#include "../Components/CombatBoxComponent.h"
#include "../Components/Drawing/GhostTrailComponent.h"

Player::Player(Game* game)
    :Actor(game)
    ,mStartingPosition(Vector2::Zero)
    ,mElementalMode(ElementalMode::Fire)
    ,mWidth(45)
    ,mHeight(75)
    ,mSkillManager(nullptr)

    ,mIsOnGround(false)
    ,mIsOnSpike(false)
    ,mIsOnMovingGround(false)
    ,mMovingGroundVelocity(Vector2::Zero)
    ,mMoveSpeed(700)
    ,mMaxSpeedYNormal(1600)

    ,mMaxTimeOutOfGroundToJump(0.07)
    ,mTimerOutOfGroundToJump(0.0f)
    ,mMaxTimeOutOfWallToJump(0.07)
    ,mTimerOutOfWallToJump(0.0f)

    ,mIsGliding(false)
    ,mMinGlideDuration(0.25f)
    ,mGlideTimer(0.0f)
    ,mGlideCooldownDuration(0.4f)
    ,mGlideCooldownTimer(0.0f)
    ,mGlideInitialSpeedY(30)
    ,mMaxSpeedYGlide(300)
    ,mGlideGravity(1300)
    ,mGlideManaCost(40)
    ,mIsGlideManaOver(false)
    ,mGlideEffect(nullptr)

    ,mIsJumping(false)
    ,mJumpTimer(0.0f)
    ,mMaxJumpTime(0.33f)
    ,mJumpForce(-750.0f)
    ,mCanJump(true)
    ,mJumpCountInAir(0)
    ,mLowGravity(50.0f)
    ,mMediumGravity(3300.0f)
    ,mHighGravity(4500.0f)

    ,mDashSpeed(1500)
    ,mDashDuration(0.2f)
    ,mDashCooldown(0.5f)
    ,mIsLightningDashing(false)
    ,mLightningDashSpeed(1800)
    ,mLightningDashDuration(0.2f)
    ,mLightningDashCooldown(0.5f)
    ,mLightningDashDamage(5.0f)
    ,mLightningDashManaCost(0.0f)
    ,mLightningDashIFramesDuration(mLightningDashDuration + 0.2f)
    ,mLightningDashEffect(nullptr)

    ,mIsOnFrenzyMode(false)
    ,mFrenzyModeDuration(10.0f)
    ,mFrenzyModeTimer(0.0f)
    ,mNormalSpeed(700.0f)
    ,mFrenzyModeSpeed(850.0f)
    ,mFrenzyAuraEffect(nullptr)
    ,mFrenzyAuraTimer(0.2f)
    ,mFrenzyModeManaCost(60.0f)

    ,mLightningSpearCooldownDuration(1.0f)
    ,mLightningSpearCooldownTimer(mLightningSpearCooldownDuration)
    ,mLightningSpearManaCost(20.0f)
    ,mLightningSpearStartOffsetPosition(15, 0)
    ,mLightningSpear(nullptr)

    ,mPrevSkill1Pressed(false)
    ,mPrevSkill2Pressed(false)

    ,mIsGroundSlamStarting(false)
    ,mIsGroundSlamRecovering(false)
    ,mIsDiving(false)
    ,mGroundSlamStartDuration(0.15f)
    ,mGroundSlamRecoveryDuration(0.45f)
    ,mGroundSlamTimer(0.0f)
    ,mGroundSlamSpeed(2000)
    ,mMaxSpeedYGroundSlam(2500)
    ,mGroundSlamDamage(15)
    ,mGroundSlamImpactDist(300.0f)
    ,mGroundSlamImpactHeightRange(mHeight * 1.8f)
    ,mGroundSlamImpactDamage(15)
    ,mGroundSlamIFramesDuration(0.3f)
    ,mGroundSlamCameraShakeStrength(90.0f)
    ,mGroundSlamCameraShakeDuration(0.2f)
    ,mGroundSlamManaCost(30.0f)
    ,mDiveEffect(nullptr)
    ,mGroundSlamImpactEffect(nullptr)

    ,mPrevSwordPressed(false)
    ,mSwordCooldownTimer(0.0f)
    ,mSwordCooldownDuration(0.4f)
    ,mSwordWidth(mWidth * 3.0f)
    ,mSwordHeight(mHeight * 1.3f)
    ,mSwordDuration(0.15f)
    ,mSwordDamage(10.0f)
    ,mSwordDirection(0)
    ,mSwordHitGround(false)
    ,mSwordHitSpike(false)
    ,mSwordHitKnockBack(750.0f)

    ,mFireBallCooldownDuration(0.7f)
    ,mFireBallCooldownTimer(0.0f)
    ,mIsFireAttacking(false)
    ,mStopInAirFireBallTimer(0.0f)
    ,mStopInAirFireBallMaxDuration(0.0f)
    ,mFireballRecoil(0.0f)
    ,mFireballWidth(45)
    ,mFireBallHeight(45)
    ,mFireballSpeed(1500)
    ,mFireballDamage(10.0f)
    ,mBaseMaxMana(90.0f)
    ,mMana(90.0f)
    ,mManaIncreaseRate(12.0f)
    ,mFireballManaCost(20.0f)
    ,mFireballAnimationDuration(0.2f)
    ,mFireballAnimationTimer(mFireballAnimationDuration)

    ,mFireWispCooldownDuration(10.0f)
    ,mFireWispCooldownTimer(mFireWispCooldownDuration)
    ,mFireWispManaCost(45.0f)

    ,mIsFreezingFront(false)
    ,mIsFreezingUp(false)
    ,mIsFreezingDown(false)
    ,mIntervalBetweenFreezeEmitDuration(0.1f)
    ,mIntervalBetweenFreezeEmitTimer(0.0f)
    ,mFreezeManaCost(2.0f)

    ,mPillarDistanceFromPlayer(100.0f)
    ,mPillarManaCost(20.0f)
    ,mPillarAnimationDuration(0.5f)
    ,mPillarAnimationTimer(mPillarAnimationDuration)
    ,mAlreadyCreatedPillar(false)

    ,mIsWallSliding(false)
    ,mWallSlideSide(WallSlideSide::notSliding)
    ,mWallSlideSpeed(350)
    ,mTryingLeavingWallSlideLeft(0)
    ,mTryingLeavingWallSlideRight(0)
    ,mTimerToLeaveWallSlidingLeft(0.0f)
    ,mTimerToLeaveWallSlidingRight(0.0f)
    ,mMaxTimerToLiveWallSliding(0.0f)

    ,mIsGoingRight(false)
    ,mIsGoingLeft(false)

    ,mWallJumpMaxTime(0.12f)
    ,mWallJumpTimer(0.0f)

    ,mKnockBackSpeed(1000.0f)
    ,mKnockBackTimer(0.0f)
    ,mKnockBackDuration(0.2f)
    ,mCameraShakeStrength(70.0f)

    ,mBaseMaxHealthPoints(70.0f)
    ,mHealthPoints(mBaseMaxHealthPoints)
    ,mIsInvulnerable(false)
    ,mInvulnerableDuration(0.85f)
    ,mInvulnerableTimer(mInvulnerableDuration)
    ,mMaxHealCount(3)
    ,mHealCount(mMaxHealCount)
    ,mHealAmount(30.0f)
    ,mIsHealing(false)
    ,mHealAnimationDuration(0.9f)
    ,mHealAnimationTimer(0.0f)

    ,mIFramesTimer(0.0f)

    ,mMoney(1000)
    ,mStartMoney(0)
    ,mEarthStone(0)
    ,mFireStone(0)
    ,mIceStone(0)
    ,mLightningStone(0)

    ,mVineRope(nullptr)
    ,mVineRopeThickness(90.0f)
    ,mIsHooking(false)
    ,mPrevHookPressed(false)
    ,mHookDirection(Vector2::Zero)
    ,mHookSpeed(1550.0f)
    ,mHookCooldownDuration(0.4f)
    ,mHookCooldownTimer(0.0f)
    ,mHookingDuration(0.3f)
    ,mHookingTimer(0.0f)
    ,mHookEnd(Vector2::Zero)
    ,mIsHookAnimating(false)
    ,mHookPoint(nullptr)

    ,mIsHookThrowing(false)
    ,mCurrentRopeTip(Vector2::Zero)
    ,mRopeThrowSpeed(2500.0f)

    ,mRadialMenu(nullptr)
    ,mRadialMenuSlowMotionDuration(0.25f)
    ,mRadialMenuSlowMotionTimer(mRadialMenuSlowMotionDuration)
    ,mRadialMenuSlowMotionCharging(false)
    ,mRadialMenuSlowMotionChangingRate(0.1f)

    ,mIsRunning(false)
    ,mHurtDuration(0.2f)
    ,mHurtTimer(mHurtDuration)
    ,mBlink(false)
    ,mBlinkDuration(0.03f)
    ,mBlinkTimer(mBlinkDuration)

    ,mRunningGrassParticleIntervalDuration(0.05f)
    ,mRunningGrassParticleIntervalTimer(0.0f)
    ,mIsGrassParticle(false)

    ,mRunningSoundIntervalDuration(0.3f)
    ,mRunningSoundIntervalTimer(0.0f)
    ,mWasOnGround(true)
    ,mDeathCounter(0)
    ,mDeathAnimationDuration(1.0f)
    ,mDeathAnimationTimer(0.0f)
    ,mIsDead(false)
    ,mInvertControls(false)
    ,mConfusionTime(0.0f)

    ,mLight(nullptr)

    ,mIsEnteringLevel(false)
    ,mEnteringLevelDuration(0.35f)
    ,mEnteringLevelTimer(0.0f)

    ,mRectComponent(nullptr)
    ,mDrawComponent(nullptr)
    ,mCombatBoxComponent(nullptr)
    ,mGhostTrailComponent(nullptr)
{
    mSkillManager = new PlayerSkillManager(this);

    Vector2 v1(-mWidth / 2, -mHeight / 2);
    Vector2 v2(mWidth / 2, -mHeight / 2);
    Vector2 v3(mWidth / 2, mHeight / 2);
    Vector2 v4(-mWidth / 2, mHeight / 2);

    std::vector<Vector2> vertices;
    vertices.emplace_back(v1);
    vertices.emplace_back(v2);
    vertices.emplace_back(v3);
    vertices.emplace_back(v4);

    mDrawComponent = new AnimatorComponent(this,
                                           "../Assets/Sprites/EsquiloFire/Esquilo.png",
                                           "../Assets/Sprites/EsquiloFire/Esquilo.json",
                                           mWidth * 4.93f, mWidth * 4.93f * 1.11f, 1002);

    std::vector idle = {33, 34, 35, 36};
    mDrawComponent->AddAnimation("idle", idle);

    std::vector attackFront = {33, 2, 3};
    mDrawComponent->AddAnimation("attackFront", attackFront);

    std::vector attackUp = {33, 4, 5};
    mDrawComponent->AddAnimation("attackUp", attackUp);

    std::vector attackDown = {33, 0, 1};
    mDrawComponent->AddAnimation("attackDown", attackDown);

    std::vector fireball = {13, 14};
    mDrawComponent->AddAnimation("fireball", fireball);

    std::vector freezeFront = {17, 18};
    mDrawComponent->AddAnimation("freezeFront", freezeFront);

    std::vector freezeDown = {15, 16};
    mDrawComponent->AddAnimation("freezeDown", freezeDown);

    std::vector freezeUp = {19, 20};
    mDrawComponent->AddAnimation("freezeUp", freezeUp);

    std::vector dive = {12};
    mDrawComponent->AddAnimation("dive", dive);

    std::vector groundSlam = {21, 22, 23, 24, 25, 25, 25};
    mDrawComponent->AddAnimation("groundSlam", groundSlam);

    std::vector pillar = {44, 45, 46, 46, 46, 46, 46, 46, 46, 46};
    mDrawComponent->AddAnimation("pillar", pillar);

    std::vector dash = {6, 7, 7, 7, 8};
    mDrawComponent->AddAnimation("dash", dash);

    std::vector lightningDash = {7, 40, 41, 41, 42, 42, 43, 43, 8};
    mDrawComponent->AddAnimation("lightningDash", lightningDash);

    std::vector run = {47, 48, 49, 50, 51, 52};
    mDrawComponent->AddAnimation("run", run);

    std::vector heal = {26, 27, 28, 29, 30, 30, 29, 28, 27, 26};
    mDrawComponent->AddAnimation("heal", heal);

    std::vector wallSlide = {53};
    mDrawComponent->AddAnimation("wallSlide", wallSlide);

    std::vector hurt = {31, 32};
    mDrawComponent->AddAnimation("hurt", hurt);

    std::vector die = {31, 9, 10, 11, 11, 11};
    mDrawComponent->AddAnimation("die", die);

    std::vector jumpUp = {37};
    mDrawComponent->AddAnimation("jumpUp", jumpUp);

    std::vector jumpApex = {38};
    mDrawComponent->AddAnimation("jumpApex", jumpApex);

    std::vector falling = {39};
    mDrawComponent->AddAnimation("falling", falling);

    mDrawComponent->SetAnimation("idle");
    mDrawComponent->SetAnimFPS(10.0f);

    mGhostTrailComponent = new GhostTrailComponent(this, mDrawComponent);

    mVineRope = new VineRope(mGame, mVineRopeThickness);

    // mRectComponent = new RectComponent(this, mWidth, mHeight, RendererMode::LINES);
    // mRectComponent->SetColor(Vector3(255, 255, 0));

    mRigidBodyComponent = new RigidBodyComponent(this, 1, 40000, mMaxSpeedYNormal);
    mAABBComponent = new AABBComponent(this, v1, v3);
    mDashComponent = new DashComponent(this, mLightningDashSpeed, mLightningDashDuration, mLightningDashCooldown);

    mCombatBoxComponent = new CombatBoxComponent(this);
    mCombatBoxComponent->AddAABBBox("body", false, v1, v3);
    mCombatBoxComponent->AddAABBBox("lightningDash", true, Vector2(-80, -40), Vector2(80, 40));
    mCombatBoxComponent->AddAABBBox("groundSlam", true, Vector2(-35, -60), Vector2(35, 60));
    // mCombatBoxComponent->SetDebugDraw(true);

    mSword = new Sword(mGame, this, mSwordWidth * mSkillManager->GetSwordRangeMultiplier(), mSwordHeight * mSkillManager->GetSwordRangeMultiplier(), mSwordDuration, mSwordDamage);

    InitLight();

    SetJumpEffects();
}

void Player::SetJumpEffects() {
    mJumpEffects.clear();
    for (int i = 0; i < 5; i++) {
        auto* jumpEffect = new JumpEffect(mGame, this, 0.3f);
        mJumpEffects.emplace_back(jumpEffect);
    }

    // Dive Effect
    mDiveEffect = new DashEffect(mGame, this, 1000);

    // LightningEffect
    mLightningDashEffect = new LightningEffect(mGame, this, mDashDuration);
    mLightningDashEffect->SetNumBolts(7);
    mLightningDashEffect->SetSpeadRadius(30.0f);
    mLightningDashEffect->SetGenerations(3);
    mLightningDashEffect->SetMaxOffset(40.0f);
    mLightningDashEffect->SetGlowThickness(10.0f);
    mLightningDashEffect->SetCoreThickness(2.0f);

    // Frenzy Aura Effect
    mFrenzyAuraEffect = new LightningEffect(mGame, this, 0.06f);
    mFrenzyAuraEffect->SetLightningGenerationIntervalDuration(0.02f);
    mFrenzyAuraEffect->SetNumBolts(1);
    mFrenzyAuraEffect->SetSpeadRadius(0);
    mFrenzyAuraEffect->SetGenerations(3);
    mFrenzyAuraEffect->SetMaxOffset(25.0f);
    mFrenzyAuraEffect->SetDrawOrder(1003);
    mFrenzyAuraEffect->SetGlowThickness(5.0f);
    mFrenzyAuraEffect->SetCoreThickness(1.0f);

    // Ground Slam Impact Effect
    mGroundSlamImpactEffect = new GroundSlamImpactEffect(mGame, mGroundSlamRecoveryDuration * 1.8f);

    // LightningSpear
    mLightningSpear = new LightningSpear(mGame);
    // Glide Effect
    mGlideEffect = new AirGlideEffect(mGame, this);
    mGlideEffect->Deactivate();

    // VineRope
    mVineRope = new VineRope(mGame, mVineRopeThickness);

    // Reset Radial Menu
    mRadialMenu = nullptr;
}

void Player::InitLight() {
    mLight = new Light(mGame);
    mLight->SetRadius(200.0f);
    mLight->SetMaxIntensity(0.45f);
    mLight->Activate();
}

void Player::OnProcessInput(const uint8_t* state, SDL_GameController &controller) {
    mTryingLeavingWallSlideLeft = 0;
    mTryingLeavingWallSlideRight = 0;
    mIsRunning = false;

    if (mIsEnteringLevel) {
        if (mRigidBodyComponent->GetVelocity().x > 0) {
            mIsRunning = true;
            SetRotation(0);
            SetScale(Vector2(1, 1));
        }
        if (mRigidBodyComponent->GetVelocity().x < 0) {
            mIsRunning = true;
            SetRotation(Math::Pi);
            SetScale(Vector2(-1, 1));
        }
        return;
    }

    // Trava enquando Ground Slam
    if (mIsGroundSlamStarting || mIsGroundSlamRecovering || mIsDiving) {
        return;
    }

    // Trava enquanto criando pilar
    if (mPillarAnimationTimer < mPillarAnimationDuration) {
        return;
    }

    bool left = (mGame->IsActionPressed(Game::Action::MoveLeft, state, &controller) &&
                !mGame->IsActionPressed(Game::Action::Look, state, &controller)) ||
                SDL_GameControllerGetAxis(&controller, SDL_CONTROLLER_AXIS_LEFTX) < -10000;

    bool leftSlow = false;
    // bool leftSlow = (mGame->IsActionPressed(Game::Action::MoveLeft, state, &controller) &&
    //                 mGame->IsActionPressed(Game::Action::Look, state, &controller)) ||
    //                 (SDL_GameControllerGetAxis(&controller, SDL_CONTROLLER_AXIS_LEFTX) < -10000 &&
    //                 SDL_GameControllerGetAxis(&controller, SDL_CONTROLLER_AXIS_LEFTX) > -20000);

    bool right = (mGame->IsActionPressed(Game::Action::MoveRight, state, &controller) &&
                 !mGame->IsActionPressed(Game::Action::Look, state, &controller)) ||
                 SDL_GameControllerGetAxis(&controller, SDL_CONTROLLER_AXIS_LEFTX) > 10000;

    bool rightSlow = false;
    // bool rightSlow = (mGame->IsActionPressed(Game::Action::MoveRight, state, &controller) &&
    //                  mGame->IsActionPressed(Game::Action::Look, state, &controller)) ||
    //                  (SDL_GameControllerGetAxis(&controller, SDL_CONTROLLER_AXIS_LEFTX) > 10000 &&
    //                  SDL_GameControllerGetAxis(&controller, SDL_CONTROLLER_AXIS_LEFTX) < 20000);

    bool lookUp = (!right && !rightSlow && !left && !leftSlow && mIsOnGround) &&
                  ((mGame->IsActionPressed(Game::Action::Up, state, &controller) &&
                  mGame->IsActionPressed(Game::Action::Look, state, &controller)) ||
                  SDL_GameControllerGetAxis(&controller, SDL_CONTROLLER_AXIS_RIGHTY) < -28000);

    bool lookDown = (!right && !rightSlow && !left && !leftSlow && mIsOnGround) &&
                   ((mGame->IsActionPressed(Game::Action::Down, state, &controller) &&
                   mGame->IsActionPressed(Game::Action::Look, state, &controller)) ||
                   SDL_GameControllerGetAxis(&controller, SDL_CONTROLLER_AXIS_RIGHTY) > 28000);

    bool up = mGame->IsActionPressed(Game::Action::Up, state, &controller) ||
              SDL_GameControllerGetAxis(&controller, SDL_CONTROLLER_AXIS_LEFTY) < -22000;

    bool down = mGame->IsActionPressed(Game::Action::Down, state, &controller) ||
          SDL_GameControllerGetAxis(&controller, SDL_CONTROLLER_AXIS_LEFTY) > 22000;

    bool jump = mGame->IsActionPressed(Game::Action::Jump, state, &controller);

    bool dash = mGame->IsActionPressed(Game::Action::Dash, state, &controller);

    bool sword = mGame->IsActionPressed(Game::Action::Attack, state, &controller);

    bool skill1 = mGame->IsActionPressed(Game::Action::Skill1, state, &controller);

    bool skill2 = mGame->IsActionPressed(Game::Action::Skill2, state, &controller);

    bool heal = mGame->IsActionPressed(Game::Action::Heal, state, &controller);

    bool hook = mGame->IsActionPressed(Game::Action::Hook, state, &controller);

    bool radialMenu = mGame->IsActionPressed(Game::Action::ChangeMode, state, &controller);


    if (mInvertControls) {
        std::swap(left, right);
        std::swap(leftSlow, rightSlow);
    }

    if (right) {
        mIsGoingRight = true;
    }
    if (left) {
        mIsGoingLeft = true;
    }

    if (!left && !leftSlow && !right && !rightSlow && !mDashComponent->GetIsDashing() && !mIsDiving && !mIsFireAttacking &&
        !mIsOnMovingGround && (mWallJumpTimer >= mWallJumpMaxTime) && (mKnockBackTimer >= mKnockBackDuration) && (!mIsHooking)) {
        mRigidBodyComponent->SetVelocity(Vector2(0.0f, mRigidBodyComponent->GetVelocity().y));
    }
    else {
        if (left && !mDashComponent->GetIsDashing() && !mIsDiving && !mIsFireAttacking && (mWallJumpTimer >= mWallJumpMaxTime) &&
            (mKnockBackTimer >= mKnockBackDuration) && (!mIsHooking)) {
            SetRotation(Math::Pi);
            SetScale(Vector2(-1.0f, 1.0f));
            mSwordDirection = Math::Pi;
            if (mIsWallSliding && !mIsOnGround) {
                mTryingLeavingWallSlideLeft = 1;
                if (mTimerToLeaveWallSlidingLeft >= mMaxTimerToLiveWallSliding) {
                    mIsRunning = true;
                    if (mRigidBodyComponent->GetKnockBackVelocity().x > 180) {
                        mRigidBodyComponent->SetVelocity(Vector2(-mMoveSpeed * 0.0f + mMovingGroundVelocity.x,
                                                             mRigidBodyComponent->GetVelocity().y));
                    }
                    else {
                        mRigidBodyComponent->SetVelocity(Vector2(-mMoveSpeed + mMovingGroundVelocity.x,
                                                                 mRigidBodyComponent->GetVelocity().y));
                    }
                    mTryingLeavingWallSlideLeft = 0;
                    mTimerToLeaveWallSlidingLeft = 0;
                }
            }
            else {
                mIsRunning = true;
                if (mRigidBodyComponent->GetKnockBackVelocity().x > 180) {
                    mRigidBodyComponent->SetVelocity(Vector2(-mMoveSpeed * 0.0f + mMovingGroundVelocity.x,
                                                         mRigidBodyComponent->GetVelocity().y));
                }
                else {
                    mRigidBodyComponent->SetVelocity(Vector2(-mMoveSpeed + mMovingGroundVelocity.x,
                                                             mRigidBodyComponent->GetVelocity().y));
                }
                mTimerToLeaveWallSlidingLeft = 0;
            }
        }

        if (leftSlow && !mDashComponent->GetIsDashing() && !mIsDiving && !mIsFireAttacking && (mWallJumpTimer >= mWallJumpMaxTime) &&
            (mKnockBackTimer >= mKnockBackDuration) && (!mIsHooking)) {
            SetRotation(Math::Pi);
            SetScale(Vector2(-1.0f, 1.0f));
            mSwordDirection = Math::Pi;
            if (mIsWallSliding && !mIsOnGround) {
                mTryingLeavingWallSlideLeft = 1;
                if (mTimerToLeaveWallSlidingLeft >= mMaxTimerToLiveWallSliding) {
                    mIsRunning = true;
                    mRigidBodyComponent->SetVelocity(Vector2(-mMoveSpeed * 0.1f + mMovingGroundVelocity.x,
                                                             mRigidBodyComponent->GetVelocity().y));
                    mTryingLeavingWallSlideLeft = 0;
                    mTimerToLeaveWallSlidingLeft = 0;
                }
            }
            else {
                mIsRunning = true;
                mRigidBodyComponent->SetVelocity(Vector2(-mMoveSpeed * 0.1f + mMovingGroundVelocity.x,
                                                         mRigidBodyComponent->GetVelocity().y));
                mTimerToLeaveWallSlidingLeft = 0;
            }
        }

        if (right && !mDashComponent->GetIsDashing() && !mIsDiving && !mIsFireAttacking && (mWallJumpTimer >= mWallJumpMaxTime)
            && (mKnockBackTimer >= mKnockBackDuration) && (!mIsHooking)) {
            SetRotation(0);
            SetScale(Vector2(1.0f, 1.0f));
            mSwordDirection = 0;
            if (mIsWallSliding && !mIsOnGround) {
                mTryingLeavingWallSlideRight = 1;
                if (mTimerToLeaveWallSlidingRight >= mMaxTimerToLiveWallSliding) {
                    mIsRunning = true;
                    if (mRigidBodyComponent->GetKnockBackVelocity().x < -180) {
                        mRigidBodyComponent->SetVelocity(Vector2(mMoveSpeed * 0.0f + mMovingGroundVelocity.x,
                                                             mRigidBodyComponent->GetVelocity().y));
                    }
                    else {
                        mRigidBodyComponent->SetVelocity(Vector2(mMoveSpeed + mMovingGroundVelocity.x,
                                                                 mRigidBodyComponent->GetVelocity().y));
                    }
                    mTryingLeavingWallSlideRight = 0;
                    mTimerToLeaveWallSlidingRight = 0;
                }
            }
            else {
                mIsRunning = true;
                if (mRigidBodyComponent->GetKnockBackVelocity().x < -180) {
                    mRigidBodyComponent->SetVelocity(Vector2(mMoveSpeed * 0.0f + mMovingGroundVelocity.x,
                                                         mRigidBodyComponent->GetVelocity().y));
                }
                else {
                    mRigidBodyComponent->SetVelocity(Vector2(mMoveSpeed + mMovingGroundVelocity.x,
                                                             mRigidBodyComponent->GetVelocity().y));
                }
                mTimerToLeaveWallSlidingRight = 0;
            }
        }

        if (rightSlow && !mDashComponent->GetIsDashing() && !mIsDiving && !mIsFireAttacking && (mWallJumpTimer >= mWallJumpMaxTime)
            && (mKnockBackTimer >= mKnockBackDuration) && (!mIsHooking)) {
            SetRotation(0);
            SetScale(Vector2(1.0f, 1.0f));
            mSwordDirection = 0;
            if (mIsWallSliding && !mIsOnGround) {
                mTryingLeavingWallSlideRight = 1;
                if (mTimerToLeaveWallSlidingRight >= mMaxTimerToLiveWallSliding) {
                    mIsRunning = true;
                    mRigidBodyComponent->SetVelocity(Vector2(mMoveSpeed * 0.1f + mMovingGroundVelocity.x,
                                                             mRigidBodyComponent->GetVelocity().y));
                    mTryingLeavingWallSlideRight = 0;
                    mTimerToLeaveWallSlidingRight = 0;
                }
            }
            else {
                mIsRunning = true;
                mRigidBodyComponent->SetVelocity(Vector2(mMoveSpeed * 0.1f + mMovingGroundVelocity.x,
                                                         mRigidBodyComponent->GetVelocity().y));
                mTimerToLeaveWallSlidingRight = 0;
            }
        }
    }

    if (lookUp && !radialMenu) {
        mGame->GetCamera()->SetLookUp();
    }

    if (lookDown && !radialMenu) {
        mGame->GetCamera()->SetLookDown();
    }

    if (!down && !up) {
        mSwordDirection = GetRotation();
    }
    else {
        if (down) {
            mSwordDirection = Math::Pi / 2;
        }
        if (up) {
            mSwordDirection = 3 * Math::Pi / 2;
        }
    }

    if (radialMenu) {
        OpenElementalMenu();
    }
    else if (mRadialMenu) {
        mRadialMenu->Close();
        mRadialMenu = nullptr;
    }

    // Dash
    if (dash) {
        UseDash();
    }

    // Jump
    if (jump) {
        UseJump();
    }
    else {
        mIsJumping = false;
        mCanJump = true;
    }

    // Sword
    if (sword) {
        UseSword();
    }
    mPrevSwordPressed = sword;

    // Skill1
    if (skill1) {
        if (mElementalMode == ElementalMode::Fire) {
            UseFireBall();
        }
        if (mElementalMode == ElementalMode::Ice) {
            UseFreeze(up, down);
        }
        if (mElementalMode == ElementalMode::Lightning) {
            UseLightningSpear();
        }
        if (mElementalMode == ElementalMode::Earth) {
            if (!mIsOnGround) {
                UseGroundSlam();
            }
        }
    }
    else {
        mIsFreezingDown = false;
        mIsFreezingUp = false;
        mIsFreezingFront = false;
    }
    mPrevSkill1Pressed = skill1;

    // Skill2
    if (skill2) {
        if (mElementalMode == ElementalMode::Earth) {
            UsePillar();
        }
        if (mElementalMode == ElementalMode::Fire) {
            UseFireWisp();
        }
        if (mElementalMode == ElementalMode::Lightning) {
            UseFrenzyMode();
        }
        if (mElementalMode == ElementalMode::Ice) {
            Glide();
        }
    }
    else {
        if (mPrevSkill2Pressed && mIsGliding) {
            mGlideCooldownTimer = 0;
        }
        if (mGlideTimer >= mMinGlideDuration) {
            mIsGliding = false;
        }
    }
    mPrevSkill2Pressed = skill2;

    // Heal
    if (heal && !left && !leftSlow && !right && !rightSlow && !jump && !dash && !sword && !skill1) {
        UseHeal();
    }
    else {
        mIsHealing = false;
        mHealAnimationTimer = 0;
    }

    // Hook
    if (mSkillManager->CanHook()) {
        std::vector<HookPoint* > hookPoints = mGame->GetHookPoints();
        HookPoint* nearestHookPoint = nullptr;
        float nearestDistance = FLT_MAX;

        // Tolerância horizontal
        const float marginX = 140.0f;

        for (HookPoint* hp: hookPoints) {
            float dist = (GetPosition() - hp->GetPosition()).Length();
            if (dist < hp->GetRadius()) {
                float distX = GetPosition().x - hp->GetPosition().x;

                // Verifica se o jogador está olhando para a direção do hookPoint
                bool lookingRight = GetRotation() == 0 && distX < 0;
                bool lookingLeft = GetRotation() == Math::Pi && distX > 0;

                bool isVerticallyAligned = Math::Abs(distX) <= marginX;

                if ((lookingRight || lookingLeft|| isVerticallyAligned) && dist < nearestDistance) {
                    nearestDistance = dist;
                    nearestHookPoint = hp;
                }
            }
        }
        if (nearestHookPoint && (nearestHookPoint != mHookPoint)) {
            nearestHookPoint->SetHookPointState(HookPoint::HookPointState::InRange);
        }

        if (hook) {
            UseHook(nearestHookPoint);
        }
        mPrevHookPressed = hook;
    }
}

void Player::OnUpdate(float deltaTime) {
    if (mIsEnteringLevel) {
        mEnteringLevelTimer += deltaTime;
        if (mEnteringLevelTimer >= mEnteringLevelDuration) {
            mIsEnteringLevel = false;
        }
    }

    if (mSwordCooldownTimer < mSwordCooldownDuration * mSkillManager->GetSwordAttackSpeedMultiplier()) {
        mSwordCooldownTimer += deltaTime;
    }

    if (mFireBallCooldownTimer < mFireBallCooldownDuration) {
        mFireBallCooldownTimer += deltaTime;
    }

    if (mFireWispCooldownTimer < mFireWispCooldownDuration) {
        mFireWispCooldownTimer += deltaTime;
    }

    if (mLightningSpearCooldownTimer < mLightningSpearCooldownDuration) {
        mLightningSpearCooldownTimer += deltaTime;
    }

    if (mGlideCooldownTimer < mGlideCooldownDuration) {
        mGlideCooldownTimer += deltaTime;
    }

    if (mIntervalBetweenFreezeEmitTimer < mIntervalBetweenFreezeEmitDuration) {
        mIntervalBetweenFreezeEmitTimer += deltaTime;
    }

    if (mFireballAnimationTimer < mFireballAnimationDuration) {
        mFireballAnimationTimer += deltaTime;
    }

    if (mPillarAnimationTimer < mPillarAnimationDuration) {
        mPillarAnimationTimer += deltaTime;
        if (!mAlreadyCreatedPillar && mPillarAnimationTimer >= mPillarAnimationDuration * 0.3f) {
            auto* pillar = new PillarGround(mGame);
            pillar->SetPosition(GetPosition() + Vector2(mPillarDistanceFromPlayer * GetForward().x, mHeight * 0.6f));
            mAlreadyCreatedPillar = true;
        }
    }

    if (mHookCooldownTimer < mHookCooldownDuration) {
        mHookCooldownTimer += deltaTime;
    }

    if (mIsHookAnimating) {
        if (mVineRope) {
            mVineRope->SetEndpoints(GetPosition(), mHookEnd);
        }
    }

    if (mKnockBackTimer < mKnockBackDuration) {
        mKnockBackTimer += deltaTime;
    }

    if (mIsDead) {
        mDeathAnimationTimer += deltaTime;
    }


    if (mStopInAirFireBallTimer < mStopInAirFireBallMaxDuration) {
        mStopInAirFireBallTimer += deltaTime;
    }
    else {
        mIsFireAttacking = false;
    }

    if (mInvulnerableTimer < mInvulnerableDuration) {
        mInvulnerableTimer += deltaTime;
        mIsInvulnerable = true;
    }
    else {
        if (mGame->GetGamePlayState() != Game::GamePlayState::Cutscene) {
            mIsInvulnerable = false;
        }
    }

    if (mIFramesTimer > 0) {
        mIFramesTimer -= deltaTime;
    }

    if (mMana < mBaseMaxMana * mSkillManager->GetMaxManaMultiplier()) {
        mMana += mManaIncreaseRate * deltaTime;
        if (mMana > mBaseMaxMana * mSkillManager->GetMaxManaMultiplier()) {
            mMana = mBaseMaxMana * mSkillManager->GetMaxManaMultiplier();
        }
    }

    if (mIsHealing) {
        mHealAnimationTimer += deltaTime;
    }

    if (mRadialMenu) {
        if (mRadialMenuSlowMotionTimer > 0 && !mRadialMenuSlowMotionCharging) {
            mGame->SetIsSlowMotion(true);
            mRadialMenuSlowMotionTimer -= deltaTime;
        }
        else {
            mGame->SetIsSlowMotion(false);
            mRadialMenuSlowMotionCharging = true;
        }
    }
    else {
        if (mRadialMenuSlowMotionTimer < mRadialMenuSlowMotionDuration) {
            mRadialMenuSlowMotionTimer += deltaTime * mRadialMenuSlowMotionChangingRate;
            if (mRadialMenuSlowMotionTimer > mRadialMenuSlowMotionDuration) {
                mRadialMenuSlowMotionTimer = mRadialMenuSlowMotionDuration;
                // Se estava carregando e acabou de encher, toca um som!
                if (mRadialMenuSlowMotionCharging) {
                    // mGame->GetAudio()->PlaySound("StaminaReady.wav");
                }
                mRadialMenuSlowMotionCharging = false;
            }
        }
    }

    mTimerToLeaveWallSlidingLeft += mTryingLeavingWallSlideLeft * deltaTime;
    mTimerToLeaveWallSlidingRight += mTryingLeavingWallSlideRight * deltaTime;

    mWallJumpTimer += deltaTime;

    // Controla animação de levar dano
    if (mHurtTimer < mHurtDuration) {
        mHurtTimer += deltaTime;
    }
    else {
        if (mIsInvulnerable) {
            mBlinkTimer += deltaTime;
            if (mBlinkTimer >= mBlinkDuration) {
                mBlink = !mBlink;
                mBlinkTimer -= mBlinkDuration;
            }
        }
    }

    if (mInvertControls) {
        mConfusionTime += deltaTime;

        float pulseSpeed = 4.0f;
        float wave = std::sin(mConfusionTime * pulseSpeed) * 0.1f;
        float intensity = 0.15f + wave;

        float rotationSpeed = 4.5f;
        float currentAngle = mConfusionTime * rotationSpeed;

        mGame->GetRenderer()->SetEffectIntensity(PostProcessEffect::ChromaticAberration, intensity);
        mGame->GetRenderer()->SetAberrationAngle(currentAngle);
    }
    else {
        mConfusionTime = 0.0f;
        mGame->GetRenderer()->SetEffectIntensity(PostProcessEffect::ChromaticAberration, 0.0f);
        mGame->GetRenderer()->SetAberrationAngle(0.0f);
    }

    mIsOnGround = false;
    mIsOnSpike = false;
    mIsOnMovingGround = false;
    mMovingGroundVelocity = Vector2::Zero;
    mIsWallSliding = false;
    if (mTimerOutOfWallToJump > mMaxTimeOutOfWallToJump) {
        mWallSlideSide = WallSlideSide::notSliding;
    }

    if (mIsLightningDashing) {
        if (!mDashComponent->GetIsDashing()) {
            mIsLightningDashing = false;
        }
        mLightningDashEffect->SetStartPosition(mStartLightningDashPosition);
        mLightningDashEffect->SetEndPosition(GetPosition());
    }

    if (mIsOnFrenzyMode) {
        mFrenzyModeTimer += deltaTime;

        mDashComponent->SetDashSpeed(mLightningDashSpeed);
        mDashComponent->SetDashDuration(mLightningDashDuration);
        mDashComponent->SetDashCooldown(mLightningDashCooldown);

        mFrenzyAuraTimer -= deltaTime;
        if (mFrenzyAuraTimer <= 0.0f) {
            mFrenzyAuraTimer = Random::GetFloatRange(0.04f, 0.08f);

            // Sorteia dois pontos aleatórios ao redor do jogador
            float angle1 = Random::GetFloatRange(0, Math::TwoPi);
            float angle2 = Random::GetFloatRange(0, Math::TwoPi);
            float radius = 35.0f; // Tamanho da aura

            Vector2 p1 = GetPosition() + Vector2(cosf(angle1) * radius, sinf(angle1) * radius);
            Vector2 p2 = GetPosition() + Vector2(cosf(angle2) * radius, sinf(angle2) * radius);

            mFrenzyAuraEffect->StartEffect(p1, p2);
        }

        if (mFrenzyModeTimer >= mFrenzyModeDuration * mSkillManager->GetFrenzyModeDurationMultiplier()) {
            StopFrenzyMode();
        }
    }

    if (mIsFireAttacking) {
        mRigidBodyComponent->SetVelocity(Vector2(-GetForward().x * mFireballRecoil, 0.0f) + mMovingGroundVelocity);
    }

    if (mIsJumping) {
        mJumpTimer += deltaTime;
        if (mJumpTimer <= mMaxJumpTime) {
            // Gravidade menor
            // So aplica gravidade se nao estiver dashando e nao estiver tacando fireball
            if (!mDashComponent->GetIsDashing() && !mIsFireAttacking && !mIsOnMovingGround && !mIsOnGround && !mIsHooking) {
                mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x,
                                                         mRigidBodyComponent->GetVelocity().y
                                                         + mLowGravity * deltaTime));
            }
        }
        else {
            mIsJumping = false;
        }
    }
    else {
        // So aplica gravidade se nao estiver dashando e nao estiver tacando fireball
        if (!mDashComponent->GetIsDashing() && !mIsFireAttacking && !mIsOnMovingGround && !mIsOnGround && !mIsHooking && !mIsGliding) {
            if (mRigidBodyComponent->GetVelocity().y < 0) {
                mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x,
                                                         mRigidBodyComponent->GetVelocity().y
                                                         + mMediumGravity * deltaTime));
            }
            else {
                mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x,
                                                         mRigidBodyComponent->GetVelocity().y
                                                         + mHighGravity * deltaTime));
            }
        }
        if (mIsGliding) {
            mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x,
                                                         mRigidBodyComponent->GetVelocity().y
                                                         + mGlideGravity * deltaTime));
        }
    }

    ResolveEnemyCollision();
    ResolveGroundCollision();

    if (mRigidBodyComponent->GetVelocity().y <= 0) {
        mIsGliding = false;
    }

    if (mIsDiving) {
        mRigidBodyComponent->SetMaxSpeedY(mMaxSpeedYGroundSlam);
    }
    else if (mIsGliding) {
        mRigidBodyComponent->SetMaxSpeedY(mMaxSpeedYGlide);
    }
    else {
        mRigidBodyComponent->SetMaxSpeedY(mMaxSpeedYNormal);
    }

    if (mIsGliding) {
        mGlideTimer += deltaTime;
        mGlideEffect->SetState(ActorState::Active);
        mGlideEffect->Activate();
        mGlideEffect->SetPosition(GetPosition() + Vector2(-10 * GetForward().x, 0));
        mMana -= mGlideManaCost * deltaTime;
        if (mMana <= 0) {
            mMana = 0;
            mIsGlideManaOver = true;
            mIsGliding = false;
        }
    }
    else {
        mGlideTimer = 0;
        mGlideEffect->Deactivate();
    }

    if (mIsGlideManaOver) {
        if (mMana >= mBaseMaxMana * mSkillManager->GetMaxManaMultiplier() * 0.25f) {
            mIsGlideManaOver = false;
        }
    }

    if (mIsOnGround) {
        mIsGoingRight = false;
        mIsGoingLeft = false;
        mIsGliding = false;
        mTimerOutOfGroundToJump = 0;
    }
    else {
        mTimerOutOfGroundToJump += deltaTime;
    }

    if (mIsWallSliding) {
        mTimerOutOfWallToJump = 0;
        mIsDiving = false;
        mDiveEffect->StopDash();
        mIsGliding = false;
        if (mRigidBodyComponent->GetVelocity().y - mMovingGroundVelocity.y > 0) {
            if (mWallSlideSide == WallSlideSide::left) {
                SetScale(Vector2(-1, 1));
            }
            if (mWallSlideSide == WallSlideSide::right) {
                SetScale(Vector2(1, 1));
            }
        }
    }
    else {
        mTimerOutOfWallToJump += deltaTime;
    }

    // Ground Slam
    if (mIsGroundSlamStarting) {
        mGroundSlamTimer += deltaTime;

        mRigidBodyComponent->SetVelocity(Vector2::Zero);

        if (mGroundSlamTimer >= mGroundSlamStartDuration) {
            mIsGroundSlamStarting = false;
            mIsDiving = true;
            // inicia animação do dive
            mDiveEffect->SetRotation(Math::PiOver2);
            mDiveEffect->SetTransformRotation(Math::PiOver2);
            mDiveEffect->SetPosition(GetPosition());
            mDiveEffect->SetOffsetPosition(-1 * Vector2(0, mHeight * 1.25f));
            mDiveEffect->StartDashEffect();
            mRigidBodyComponent->SetVelocity(Vector2(0, mGroundSlamSpeed));
        }
    }

    if (mIsGroundSlamRecovering) {
        mGroundSlamTimer += deltaTime;

        if (mGroundSlamTimer >= mGroundSlamRecoveryDuration) {
            mIsGroundSlamRecovering = false;
            mIFramesTimer = mGroundSlamIFramesDuration;
        }
    }

    // Hook
    if (mIsHookThrowing) {
        // Calcula a distância até o alvo
        float distanceToTarget = (mHookEnd - mCurrentRopeTip).Length();
        float moveStep = mRopeThrowSpeed * deltaTime;
        mHookPoint->SetHookPointState(HookPoint::HookPointState::InRange);

        if (moveStep >= distanceToTarget) {
            // A CORDA CHEGOU NO ALVO!
            mCurrentRopeTip = mHookEnd;
            mIsHookThrowing = false;

            // AGORA sim começamos a puxar o jogador
            mIsHooking = true;
            mHookingTimer = 0.0f; // Reseta o timer de puxada

            // Toca som de impacto/conectar
            // mGame->GetAudio()->PlaySound("Hook/HookHit.wav"); // Exemplo
        }
        else {
            // A corda ainda está viajando
            // Move a ponta na direção do alvo
            Vector2 travelDir = (mHookEnd - mCurrentRopeTip);
            travelDir.Normalize();
            mCurrentRopeTip += travelDir * moveStep;
        }

        // Atualiza o desenho da corda enquanto ela viaja
        if (mVineRope) {
            mVineRope->SetEndpoints(GetPosition(), mCurrentRopeTip);
            mVineRope->SetIsHooking(false);
        }
    }
    if (mIsHooking) {
        if (mHookingTimer < mHookingDuration) {
            mHookPoint->SetHookPointState(HookPoint::HookPointState::Hooked);
            mRigidBodyComponent->SetVelocity(mHookDirection * mHookSpeed);
            mHookingTimer += deltaTime;

            // Garante que a corda fique desenhada esticada até o fim
            if (mVineRope) {
                mVineRope->SetEndpoints(GetPosition(), mHookEnd);
                mVineRope->SetIsHooking(true);
            }
        } else {
            mIsHooking = false;
            mIsHookAnimating = false; // Para de desenhar
            mHookPoint = nullptr;

            // Resetar dash no ar
            mDashComponent->SetHasDashedInAir(false);
            // RESET DO CONTADOR DE PULO
            mJumpCountInAir = 0;

            if (mVineRope) {
                mVineRope->Deactivate();
            }
        }
    }

    // Altera velocidade dos projéteis de gelo de acordo com a velocidade do player
    for (auto it = mSnowBallsParticleSystems.begin(); it != mSnowBallsParticleSystems.end(); ) {
        // Acessamos os dados da struct usando 'it->'
        ParticleSystem* ps = it->system;
        EffectDir dir = it->direction;

        if (ps && ps->GetLifeTime() > 0.02f) {
            Vector2 offset = Vector2::Zero;
            float speedScale = 1.1f;

            switch (dir) {
                case EffectDir::Down:
                    offset = Vector2(-10 * GetForward().x, mHeight * 0.3f);
                    if (mRigidBodyComponent->GetVelocity().y > 0) {
                        speedScale = 1.1f + 0.0006f * Math::Abs(mRigidBodyComponent->GetVelocity().y);
                    }
                    else if (mRigidBodyComponent->GetVelocity().y < 0) {
                        // speedScale = 1.1f - 0.0006f * Math::Abs(mRigidBodyComponent->GetVelocity().y);
                    }
                    break;

                case EffectDir::Up:
                    offset = -1 * Vector2(10 * GetForward().x, mHeight * 0.3f);
                    if (mRigidBodyComponent->GetVelocity().y < 0) {
                        speedScale = 1.1f + 0.0006f * Math::Abs(mRigidBodyComponent->GetVelocity().y);
                    }
                    else if (mRigidBodyComponent->GetVelocity().y > 0) {
                        // speedScale = 1.1f - 0.0006f * Math::Abs(mRigidBodyComponent->GetVelocity().y);
                    }
                    break;

                case EffectDir::Front:
                    offset = Vector2(mWidth * 0.45f * GetForward().x, 11);
                    speedScale = 1.1f + 0.0006f * Math::Abs(mRigidBodyComponent->GetVelocity().x);
                    break;
            }
            ps->SetPosition(GetPosition() + offset);
            ps->SetParticleSpeedScale(speedScale);
            ++it;
        }
        else {
            it = mSnowBallsParticleSystems.erase(it);
        }
    }
    for (auto it = mIceCloudParticleSystems.begin(); it != mIceCloudParticleSystems.end(); ) {
        ParticleSystem* ps = it->system;
        EffectDir dir = it->direction;

        if (ps && ps->GetLifeTime() > 0.02f) {
            Vector2 offset = Vector2::Zero;
            float speedScale = 1.1f;

            switch (dir) {
                case EffectDir::Down:
                    offset = Vector2(-10 * GetForward().x, mHeight * 0.3f);
                    if (mRigidBodyComponent->GetVelocity().y > 0) {
                        speedScale = 1.1f + 0.0006f * Math::Abs(mRigidBodyComponent->GetVelocity().y);
                    }
                    else if (mRigidBodyComponent->GetVelocity().y < 0) {
                        // speedScale = 1.1f - 0.0006f * Math::Abs(mRigidBodyComponent->GetVelocity().y);
                    }
                    break;

                case EffectDir::Up:
                    offset = -1 * Vector2(10 * GetForward().x, mHeight * 0.3f);
                    if (mRigidBodyComponent->GetVelocity().y < 0) {
                        speedScale = 1.1f + 0.0006f * Math::Abs(mRigidBodyComponent->GetVelocity().y);
                    }
                    else if (mRigidBodyComponent->GetVelocity().y > 0) {
                        // speedScale = 1.1f - 0.0006f * Math::Abs(mRigidBodyComponent->GetVelocity().y);
                    }
                    break;

                case EffectDir::Front:
                    offset = Vector2(mWidth * 0.45f * GetForward().x, 11);
                speedScale = 1.1f + 0.0006f * Math::Abs(mRigidBodyComponent->GetVelocity().x);
                break;
            }
            ps->SetPosition(GetPosition() + offset);
            ps->SetParticleSpeedScale(speedScale);
            ++it;
        }
        else {
            it = mIceCloudParticleSystems.erase(it);
        }
    }

    if (mIsRunning && mIsOnGround) {
        mRunningSoundIntervalTimer += deltaTime;
        if (mRunningSoundIntervalTimer >= mRunningSoundIntervalDuration) {
            mRunningSoundIntervalTimer -= mRunningSoundIntervalDuration;
            mGame->GetAudio()->PlayVariantSound("StepsInGrass/StepsInGrass.wav", 4);
        }

        mRunningGrassParticleIntervalTimer += deltaTime;
        if (mRunningGrassParticleIntervalTimer >= mRunningGrassParticleIntervalDuration) {
            mRunningGrassParticleIntervalTimer -= mRunningGrassParticleIntervalDuration;
            StartGrassEffect(GrassEffectType::Run);
        }
    }

    if (mWasOnGround == false) {
        if (mIsOnGround) {
            mGame->GetAudio()->PlaySound("FallOnGround.wav");
            StartGrassEffect(GrassEffectType::Land);
            for (JumpEffect* j: mJumpEffects) {
                if (j->GetState() == ActorState::Paused) {
                    j->SetState(ActorState::Active);
                    j->StartEffect(JumpEffect::EffectType::Land);
                    break;
                }
            }
            mRunningSoundIntervalTimer = 0;
        }
    }

    mWasOnGround = mTimerOutOfGroundToJump < mMaxTimeOutOfGroundToJump;

    // Se cair, volta para a posição inicial
    if (GetPosition().y > 20000) {
        // SetPosition(mStartingPosition);
    }

    if (Died()) {
        mGame->SetGamePlayState(Game::GamePlayState::GameOver);
        mAABBComponent->SetActive(false);
        mCombatBoxComponent->SetAllBoxesActive(false);
        mRigidBodyComponent->SetVelocity(Vector2::Zero);
        StopFrenzyMode();
        mKnockBackTimer = mKnockBackDuration;
        mInvulnerableTimer = mInvulnerableDuration;
        mIsHealing = false;
        if (mRadialMenu) {
            mRadialMenu->Close();
            mRadialMenu = nullptr;
        }
        mIsHooking = false;
        mIsHookThrowing = false;
        mIsHookAnimating = false;
        mHookPoint = nullptr;
        if (mVineRope) {
            mVineRope->Deactivate();
        }
        // mGame->GetAudio()->StopAllSounds();
        // mGame->GetAudio()->StopSound(mGame->GetMusicHandle());
        // mGame->GetAudio()->StopSound(mGame->GetBossMusicHandle());
        if (mDeathAnimationTimer >= mDeathAnimationDuration) {
            mDeathCounter++;
            mDeathAnimationTimer = 0;
            mGame->SetBackToCheckpoint();
            SetState(ActorState::Paused);
        }
    }

    if (mGame->GetGoingToNextLevel()) {
        mRigidBodyComponent->SetVelocity(Vector2::Zero);
    }

    if (mCombatBoxComponent) {
        ManageCombatBoxes(deltaTime);
    }

    if (mDrawComponent) {
        ManageAnimations();
    }

    if (mLight) {
        mLight->SetPosition(GetPosition());
    }
}

void Player::ResolveGroundCollision() {
    bool isCollidingSides = false;
    Vector2 collisionNormal(Vector2::Zero);
    std::vector<Ground* > grounds = mGame->GetGrounds();
    if (!grounds.empty()) {
        for (Ground* g: grounds) {
            if (!g->GetIsSpike()) { // Colisão com ground
                if (mAABBComponent->Intersect(*g->GetComponent<ColliderComponent>())) {
                    collisionNormal = mAABBComponent->ResolveCollision(*g->GetComponent<ColliderComponent>());
                }
                else {
                    collisionNormal = Vector2::Zero;
                }

                // colidiu top
                if (collisionNormal == Vector2::NegUnitY) {
                    mIsOnGround = true;
                    mIsJumping = false;
                    // Resetar dash no ar
                    mDashComponent->SetHasDashedInAir(false);
                    // RESET DO CONTADOR DE PULO
                    mJumpCountInAir = 0;

                    if (mIsDiving) {
                        mGame->GetCamera()->StartCameraShake(mGroundSlamCameraShakeDuration, mGroundSlamCameraShakeStrength);
                        if (g->GetIsBreakable()) {
                            mRigidBodyComponent->SetVelocity(Vector2(0, mGroundSlamSpeed));
                            g->DestroyGround();
                            mGame->ActiveHitStop(0.01f);
                        }
                        else {
                            mIsDiving = false;
                            mDiveEffect->StopDash();
                            mIsGroundSlamRecovering = true;
                            mGroundSlamTimer = 0;
                            GroundSlamEffects();
                        }
                    }

                    // Move o player junto ao ground em movimento
                    if (g->GetIsMoving()) {
                        mIsOnMovingGround = true;
                        mMovingGroundVelocity = g->GetComponent<RigidBodyComponent>()->GetVelocity();
                        if (!mDashComponent->GetIsDashing()) {
                            mRigidBodyComponent->SetVelocity(mMovingGroundVelocity);

                            // Pega o topo do AABB do chão
                            auto* groundCollider = g->GetComponent<ColliderComponent>();
                            auto* groundAABB = dynamic_cast<AABBComponent*>(groundCollider);

                            if (groundAABB) {
                                float physicalGroundTop = g->GetPosition().y + groundAABB->GetOffset().y + groundAABB->GetMin().y;

                                // Posiciona o player exatamente no topo físico, mais a margem de 1 pixel para não trepidar
                                SetPosition(Vector2(GetPosition().x, physicalGroundTop - mHeight / 2 + 1));
                            }
                        }
                    }
                }

                // colidiu bot
                if (collisionNormal == Vector2::UnitY) {
                    mJumpTimer = mMaxJumpTime;
                    mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x, 1.0f));
                    if (g->GetIsMoving()) {
                        if (g->GetComponent<RigidBodyComponent>()->GetVelocity().y > 0) {
                            mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x, g->GetComponent<RigidBodyComponent>()->GetVelocity().y * 1.5f));
                            // Para não grudar quando pular por baixo de uma plataforma movel
                        }
                    }
                }

                //colidiu pelas laterais
                if (mSkillManager->CanWallSlide()) {
                    if ((collisionNormal == Vector2::NegUnitX || collisionNormal == Vector2::UnitX)) {
                        isCollidingSides = true;
                        // Testa se não está dashando para não bugar quando dar um dash na quina de baixo e inverter a direção do dash
                        if (collisionNormal == Vector2::NegUnitX &&
                            !mDashComponent->GetIsDashing() &&
                            mIsGoingRight)
                        {
                            // Move o player junto ao ground em movimento
                            if (g->GetIsMoving()) {
                                mIsOnMovingGround = true;
                                mMovingGroundVelocity = g->GetComponent<RigidBodyComponent>()->GetVelocity();
                            }
                            if (!mIsOnGround) {
                                mIsWallSliding = true;
                                mWallSlideSide = WallSlideSide::left;
                            }
                            if (!mIsOnGround) {
                                // SetRotation(Math::Pi);
                            }
                            if (mMovingGroundVelocity.x > 0) {
                                mRigidBodyComponent->SetVelocity(Vector2(
                                    mRigidBodyComponent->GetVelocity().x + mMovingGroundVelocity.x + 100,
                                    mRigidBodyComponent->GetVelocity().y));
                            }
                            else if (mMovingGroundVelocity.x < 0) {
                                mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x + 100,
                                                                         mRigidBodyComponent->GetVelocity().y));
                            }
                        }
                        else if (collisionNormal == Vector2::UnitX &&
                            !mDashComponent->GetIsDashing() &&
                            mIsGoingLeft)
                        {
                            // Move o player junto ao ground em movimento
                            if (g->GetIsMoving()) {
                                mIsOnMovingGround = true;
                                mMovingGroundVelocity = g->GetComponent<RigidBodyComponent>()->GetVelocity();
                            }
                            if (!mIsOnGround) {
                                mIsWallSliding = true;
                                mWallSlideSide = WallSlideSide::right;
                            }
                            if (!mIsOnGround) {
                                // SetRotation(0);
                            }
                            if (mMovingGroundVelocity.x < 0) {
                                mRigidBodyComponent->SetVelocity(Vector2(
                                    mRigidBodyComponent->GetVelocity().x + mMovingGroundVelocity.x - 100,
                                    mRigidBodyComponent->GetVelocity().y));
                            }
                            else if (mMovingGroundVelocity.x > 0) {
                                mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x - 100,
                                                                         mRigidBodyComponent->GetVelocity().y));
                            }
                        }

                        if (mIsWallSliding) {
                            // Resetar dash no ar
                            mDashComponent->SetHasDashedInAir(false);
                            // RESET DO CONTADOR DE PULO
                            mJumpCountInAir = 0;
                            if (mIsOnMovingGround && mRigidBodyComponent->GetVelocity().y - mMovingGroundVelocity.y > 0) {
                                mIsJumping = false;
                                mRigidBodyComponent->SetVelocity(Vector2(
                                    mRigidBodyComponent->GetVelocity().x + mMovingGroundVelocity.x,
                                    mWallSlideSpeed + mMovingGroundVelocity.y));
                            }
                            else if (!mIsOnMovingGround && mRigidBodyComponent->GetVelocity().y > 0) {
                                mIsJumping = false;
                                mRigidBodyComponent->SetVelocity(
                                    Vector2(mRigidBodyComponent->GetVelocity().x, mWallSlideSpeed));
                            }
                        }
                    }
                }
                if (mSword->GetComponent<CombatBoxComponent>()->GetBox("ground")->collider->Intersect(*g->GetComponent<ColliderComponent>())) {
                    // Colisão da sword com grounds
                    if (!mSwordHitGround) {
                        collisionNormal = mSword->GetComponent<CombatBoxComponent>()->GetBox("ground")->collider->CollisionSide(*g->GetComponent<ColliderComponent>());
                        if ((collisionNormal == Vector2::NegUnitY && Math::Abs(mSword->GetForward().y) == 1) ||
                            (collisionNormal == Vector2::UnitY && Math::Abs(mSword->GetForward().y) == 1) ||
                            (collisionNormal == Vector2::NegUnitX && Math::Abs(mSword->GetForward().x) == 1) ||
                            (collisionNormal == Vector2::UnitX && Math::Abs(mSword->GetForward().x) == 1) )
                        {
                            Vector2 knockBackForce;
                            knockBackForce.x = -mSword->GetForward().x * mSwordHitKnockBack;
                            mRigidBodyComponent->ApplyKnockBack(knockBackForce);

                            auto* grass = new ParticleSystem(mGame, Particle::ParticleType::SolidParticle, 6.0f * 1.2f, 6.0f, 150.0, 0.30, 0.05f);
                            if (collisionNormal == Vector2::NegUnitY) {
                                grass->SetPosition(Vector2(mSword->GetPosition().x, g->GetPosition().y - g->GetHeight() / 2));
                            }
                            if (collisionNormal == Vector2::UnitY) {
                                grass->SetPosition(Vector2(mSword->GetPosition().x, g->GetPosition().y + g->GetHeight() / 2));
                            }
                            if (collisionNormal == Vector2::NegUnitX) {
                                grass->SetPosition(Vector2(g->GetPosition().x - g->GetWidth() / 2, GetPosition().y));
                            }
                            if (collisionNormal == Vector2::UnitX) {
                                grass->SetPosition(Vector2(g->GetPosition().x + g->GetWidth() / 2, GetPosition().y));
                            }

                            grass->SetEmitDirection(mSword->GetForward() * -1);
                            grass->SetGroundCollision(false);
                            grass->SetParticleSpeedScale(0.4);
                            grass->SetConeSpread(45.0f);
                            SDL_Color color = mGame->GetGroundParticleColor();
                            grass->SetParticleColor(color);
                            grass->SetParticleGravity(true);
                            mSwordHitGround = true;
                        }
                    }
                }
            }
            else if (g->GetIsSpike()) { // Colisão com spikes
                if (mAABBComponent->Intersect(*g->GetComponent<ColliderComponent>())) {
                    collisionNormal = mAABBComponent->ResolveCollision(*g->GetComponent<ColliderComponent>());

                    // colidiu top
                    if (collisionNormal == Vector2::NegUnitY) {
                        mIsOnSpike = true;
                    }
                    if (mIsDiving) {
                        mIsDiving = false;
                        mDiveEffect->StopDash();
                        mGame->GetCamera()->StartCameraShake(mGroundSlamCameraShakeDuration, mGroundSlamCameraShakeStrength);
                    }
                    mDashComponent->StopDash();
                    mLightningDashEffect->StopEffect();

                    ReceiveHit(10, collisionNormal, DamageType::Environment);
                }
                // Colisão da sword com spikes
                if (mSword->GetComponent<ColliderComponent>()->Intersect(*g->GetComponent<ColliderComponent>())) {
                    if (!mSwordHitSpike) {
                        collisionNormal = mSword->GetComponent<ColliderComponent>()->CollisionSide(*g->GetComponent<ColliderComponent>());
                        if ((collisionNormal == Vector2::NegUnitY && Math::Abs(mSword->GetForward().y) == 1) ||
                            (collisionNormal == Vector2::UnitY && Math::Abs(mSword->GetForward().y) == 1) ||
                            (collisionNormal == Vector2::NegUnitX && Math::Abs(mSword->GetForward().x) == 1) ||
                            (collisionNormal == Vector2::UnitX && Math::Abs(mSword->GetForward().x) == 1) )
                        {
                            Vector2 knockBackForce;
                            knockBackForce.x = -mSword->GetForward().x * mSwordHitKnockBack;
                            mRigidBodyComponent->ApplyKnockBack(knockBackForce);
                            mGame->GetAudio()->PlaySound("HitSpike/HitSpike1.wav");
                            for (int i = 0; i < 3; i++) {
                                auto* sparkEffect = new Effect(mGame);
                                sparkEffect->SetDuration(0.1f);

                                // collisionSide = mSword->GetComponent<AABBComponent>()->CollisionSide(*g->GetComponent<AABBComponent>());
                                if (collisionNormal == Vector2::NegUnitY) {
                                    sparkEffect->SetPosition(Vector2(mSword->GetPosition().x, g->GetPosition().y - g->GetHeight() / 2));
                                }

                                if (collisionNormal == Vector2::UnitY) {
                                    sparkEffect->SetPosition(Vector2(mSword->GetPosition().x, g->GetPosition().y + g->GetHeight() / 2));
                                }

                                if (collisionNormal == Vector2::NegUnitX) {
                                    sparkEffect->SetPosition(Vector2(g->GetPosition().x - g->GetWidth() / 2, GetPosition().y));
                                }

                                if (collisionNormal == Vector2::UnitX) {
                                    sparkEffect->SetPosition(Vector2(g->GetPosition().x + g->GetWidth() / 2, GetPosition().y));
                                }

                                sparkEffect->SetEffect(TargetEffect::SwordHit);
                            }
                            mSwordHitSpike = true;
                            mSwordHitGround = true;
                        }
                    }

                    if (mSword->GetRotation() == Math::Pi / 2) {
                        if (!mDashComponent->GetIsDashing()) {
                            mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x, mJumpForce));
                        }
                        // Resetar dash no ar
                        mDashComponent->SetHasDashedInAir(false);
                        // RESET DO CONTADOR DE PULO
                        mJumpCountInAir = 0;
                    }
                }
            }
        }
        if (!isCollidingSides) {
            mIsGoingRight = false;
            mIsGoingLeft = false;
        }
    }
}

void Player::ResolveEnemyCollision() {
    Vector2 collisionNormal(Vector2::Zero);
    std::vector<Enemy* > enemies = mGame->GetEnemies();
    if (!enemies.empty()) {
        for (Enemy* e: enemies) {
            // Receber dano
            HitResult hitResult = mCombatBoxComponent->CheckReceiveAttack(e->GetComponent<CombatBoxComponent>());
            if (hitResult.isValid && !e->IsFrozen()) {
                Vector2 knockBackDirection = GetPosition() - e->GetPosition();
                if (knockBackDirection.Length() > 0) {
                    knockBackDirection.Normalize();
                }
                ReceiveHit(e->GetContactDamage(), knockBackDirection);
            }

            // Aplicar dano
            hitResult = mCombatBoxComponent->CheckAttackAgainst(e->GetComponent<CombatBoxComponent>());
            if (hitResult.isValid) {
                if (hitResult.hitTag == "lightningDash") {
                    auto it = std::find(mEnemiesHitByCurrentDash.begin(), mEnemiesHitByCurrentDash.end(), e);
                    if (it == mEnemiesHitByCurrentDash.end()) {
                        e->ReceiveHit(mLightningDashDamage * mSkillManager->GetLightningDashDamageMultiplier(), GetForward(), false);
                        mEnemiesHitByCurrentDash.push_back(e);
                    }
                }
                if (hitResult.hitTag == "groundSlam") {
                    float dist = GetPosition().x - e->GetPosition().x;
                    auto it = std::find(mEnemiesHitByGroundSlam.begin(), mEnemiesHitByGroundSlam.end(), e);
                    if (it == mEnemiesHitByGroundSlam.end()) {
                        if (dist < 0) {
                            e->ReceiveHit(mGroundSlamDamage, Vector2::UnitX);
                        }
                        else {
                            e->ReceiveHit(mGroundSlamDamage, Vector2::NegUnitX);
                        }
                        mEnemiesHitByGroundSlam.push_back(e);
                    }
                }
            }

            // Colisão da espada com inimigos
            hitResult = mSword->GetComponent<CombatBoxComponent>()->CheckAttackAgainst(e->GetComponent<CombatBoxComponent>());
            if (hitResult.isValid) {
                auto it = std::find(mEnemiesHitBySword.begin(), mEnemiesHitBySword.end(), e);
                if (it == mEnemiesHitBySword.end()) {
                    e->ReceiveHit(mSword->GetDamage() * mSkillManager->GetSwordDamageMultiplier(), mSword->GetForward());
                    if (mSword->GetRotation() == Math::Pi / 2) {
                        if (!mDashComponent->GetIsDashing()) {
                            if (auto* mushroom = dynamic_cast<Mushroom*>(e)) {
                                if (mushroom->GetMushroomState() != Mushroom::State::Attack) {
                                    mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x, mJumpForce * 1.75f));
                                }
                                else {
                                    mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x, mJumpForce));
                                }
                            }
                            else {
                                mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x, mJumpForce));
                            }
                        }
                        // Resetar dash no ar
                        mDashComponent->SetHasDashedInAir(false);
                        // RESET DO CONTADOR DE PULO
                        mJumpCountInAir = 0;
                    }

                    Vector2 knockBackForce;
                    knockBackForce.x = -mSword->GetForward().x * mSwordHitKnockBack;
                    mRigidBodyComponent->ApplyKnockBack(knockBackForce);

                    mEnemiesHitBySword.push_back(e);
                }
            }
        }
    }
}

void Player::Stop() {
    mIsJumping = false;
    mIsHooking = false;
    mIsHookAnimating = false;
    mIsHookThrowing = false;
    mHookPoint = nullptr;
    if (mVineRope) {
        mVineRope->Deactivate();
    }
    mDashComponent->StopDash();
    mLightningDashEffect->StopEffect();
    mRigidBodyComponent->SetVelocity(Vector2::Zero);
}

void Player::ResetCooldown() {
    mGlideCooldownTimer = mGlideCooldownDuration;
    mSwordCooldownTimer = mSwordCooldownDuration * mSkillManager->GetSwordAttackSpeedMultiplier();
    mFireBallCooldownTimer = mFireBallCooldownDuration;
    mFireWispCooldownTimer = mFireWispCooldownDuration;
    mLightningSpearCooldownTimer = mLightningSpearCooldownDuration;
}

void Player::UseDash() {
    if (mSkillManager->CanDash()) {
        if (!mIsFireAttacking && !mIsDiving && !mIsHookThrowing && !mIsHooking) {
            if (mIsWallSliding && mRigidBodyComponent->GetVelocity().y - mMovingGroundVelocity.y > 0) {
                if (mWallSlideSide == WallSlideSide::left) {
                    SetRotation(Math::Pi);
                    SetScale(Vector2(-1, 1));
                }
                if (mWallSlideSide == WallSlideSide::right) {
                    SetRotation(0);
                    SetScale(Vector2(1, 1));
                }
            }
            if (mDashComponent->UseDash(mIsOnGround)) {
                mIsHooking = false;
                mIsHookThrowing = false;
                mIsHookAnimating = false;
                mHookPoint = nullptr;
                if (mVineRope) {
                    mVineRope->Deactivate();
                }

                if (mIsOnFrenzyMode && mMana >= mLightningDashManaCost) {
                    mIsLightningDashing = true;
                    mStartLightningDashPosition = GetPosition();
                    mLightningDashEffect->StartEffect(mStartLightningDashPosition, GetPosition());
                    mIFramesTimer = mLightningDashIFramesDuration;
                    mEnemiesHitByCurrentDash.clear();
                    mMana -= mLightningDashManaCost;
                }
                if (mIsOnGround) {
                    StartGrassEffect(GrassEffectType::Dash);
                    for (JumpEffect* j: mJumpEffects) {
                        if (j->GetState() == ActorState::Paused) {
                            j->SetState(ActorState::Active);
                            j->StartEffect(JumpEffect::EffectType::TakeOff);
                            break;
                        }
                    }
                }
            }
        }
    }
}

void Player::UseFrenzyMode() {
    if (mElementalMode == ElementalMode::Lightning) {
        if (mSkillManager->CanFrenzyMode() &&
            !mIsOnFrenzyMode &&
            !mIsGroundSlamStarting &&
            !mIsGroundSlamRecovering &&
            !mIsDiving &&
            !mIsHookThrowing &&
            !mIsHooking &&
            !mDashComponent->GetIsDashing() &&
            mMana >= mFrenzyModeManaCost)
        {
            mGhostTrailComponent->SetIsEmitting(true);
            mIsOnFrenzyMode = true;
            mMoveSpeed = mFrenzyModeSpeed;
            mFrenzyModeTimer = 0;
            mMana -= mFrenzyModeManaCost;
        }
    }
}

void Player::StopFrenzyMode() {
    mGhostTrailComponent->SetIsEmitting(false);
    mMoveSpeed = mNormalSpeed;
    mDashComponent->SetDashSpeed(mDashSpeed);
    mDashComponent->SetDashDuration(mDashDuration);
    mDashComponent->SetDashCooldown(mDashCooldown);
    mIsOnFrenzyMode = false;
}

void Player::UseLightningSpear() {
    if (mSkillManager->CanLightningSpear() && mElementalMode == ElementalMode::Lightning) {
        if (!mPrevSkill1Pressed &&
            mLightningSpearCooldownTimer >= mLightningSpearCooldownDuration &&
            mMana >= mLightningSpearManaCost &&
            !mDashComponent->GetIsDashing() && !mIsDiving)
        {
            if (mIsWallSliding && mRigidBodyComponent->GetVelocity().y - mMovingGroundVelocity.y > 0) {
                if (mWallSlideSide == WallSlideSide::left) {
                    SetRotation(Math::Pi);
                    SetScale(Vector2(-1, 1));
                }
                if (mWallSlideSide == WallSlideSide::right) {
                    SetRotation(0);
                    SetScale(Vector2(1, 1));
                }
            }
            mLightningSpear->SetRotation(GetRotation());
            mLightningSpear->SetTransformRotation(GetRotation());
            mLightningSpear->SetPosition(GetPosition() + mLightningSpearStartOffsetPosition * mLightningSpear->GetForward());
            mLightningSpear->Activate();
            mMana -= mLightningSpearManaCost;
            mLightningSpearCooldownTimer = 0;
        }
    }
}

void Player::UseGroundSlam() {
    if (mElementalMode == ElementalMode::Earth) {
        if (mSkillManager->CanGroundSlam() &&
            !mPrevSkill1Pressed &&
            !mIsGroundSlamStarting &&
            !mIsGroundSlamRecovering &&
            !mIsDiving &&
            !mIsHookThrowing &&
            !mIsHooking &&
            !mDashComponent->GetIsDashing() &&
            !mIsWallSliding &&
            mMana >= mGroundSlamManaCost)
        {
            mEnemiesHitByGroundSlam.clear();
            mIsGroundSlamStarting = true;
            mRigidBodyComponent->SetVelocity(Vector2::Zero);
            mGroundSlamTimer = 0;
            mMana -= mGroundSlamManaCost;

            mIsHooking = false;
            mIsHookThrowing = false;
            mIsHookAnimating = false;
            mHookPoint = nullptr;
            if (mVineRope) {
                mVineRope->Deactivate();
            }
        }
    }
}

void Player::GroundSlamImpact() {
    std::vector<Enemy* > enemies = mGame->GetEnemies();
    if (enemies.empty()) {
        return;
    }

    float slamMinY = GetPosition().y - mGroundSlamImpactHeightRange / 2.0f;
    float slamMaxY = GetPosition().y + mGroundSlamImpactHeightRange / 2.0f;

    for (Enemy* e: enemies) {
        auto it = std::find(mEnemiesHitByGroundSlam.begin(), mEnemiesHitByGroundSlam.end(), e);
        if (it == mEnemiesHitByGroundSlam.end()) {
            float enemyHeight = e->GetHeight();
            float enemyMinY = e->GetPosition().y - enemyHeight / 2.0f;
            float enemyMaxY = e->GetPosition().y + enemyHeight / 2.0f;

            bool verticalOverlap = (slamMinY <= enemyMaxY) && (slamMaxY >= enemyMinY);

            if (verticalOverlap) {
                float xDist = std::abs(e->GetPosition().x - GetPosition().x);

                if (xDist <= mGroundSlamImpactDist) {

                    // Calcula a direção do empurrão (Knockback)
                    Vector2 direction = e->GetPosition() - GetPosition();
                    if (direction.LengthSq() > 0) {
                        direction.Normalize();
                    } else {
                        // Se estiverem na mesma posição exata, empurra para a direita por padrão
                        direction = Vector2(1.0f, 0.0f);
                    }

                    e->ReceiveHit(mGroundSlamImpactDamage, direction);
                    mEnemiesHitByGroundSlam.push_back(e);
                }
            }
        }
    }
}

void Player::GroundSlamEffects() {
    auto* smoke = new ParticleSystem(mGame, Particle::ParticleType::BlurParticle, 80.0f * 1.2f, 80.0f, 25.0f, 0.35f, 0.3f);
    smoke->SetParticleColor(SDL_Color{130, 130, 130, 80});
    smoke->SetConeSpread(360.0f);
    smoke->SetParticleSpeedScale(0.2f);
    smoke->SetParticleGravity(false);
    smoke->SetEmitDirection(Vector2::Zero);
    smoke->SetPosition(GetPosition() + Vector2(0, mHeight * 0.5f));
    smoke->SetGroundCollision(false);

    mGroundSlamImpactEffect->Start(GetPosition() + Vector2(0, 5));
    mGroundSlamImpactEffect->SetScale(Vector2(GetForward().x, 1));

    // auto* miniGrounds = new ParticleSystem(mGame, Particle::ParticleType::SolidParticle, 10, 400.0, 3.0, 0.07f);
    // miniGrounds->SetPosition(GetPosition() + Vector2(0, mHeight * 0.5f));
    // miniGrounds->SetEmitDirection(Vector2::NegUnitY);
    // miniGrounds->SetParticleSpeedScale(0.6);
    // miniGrounds->SetParticleColor(SDL_Color{55, 27, 7, 255});
    // miniGrounds->SetParticleGravity(true);
    // miniGrounds->SetConeSpread(180.0f);
}

void Player::Glide() {
    if (mSkillManager->CanGlide() &&
        mElementalMode == ElementalMode::Ice &&
        mGlideCooldownTimer >= mGlideCooldownDuration &&
        !mIsOnGround &&
        !mIsWallSliding &&
        !mIsJumping &&
        !mIsHooking &&
        !mIsHookThrowing &&
        mRigidBodyComponent->GetVelocity().y > 0 &&
        mMana > 0 &&
        !mIsGlideManaOver)
    {
        if (!mIsGliding) {
            mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x, mGlideInitialSpeedY));
        }
        mIsGliding = true;
        mGlideEffect->SetState(ActorState::Active);
    }
    else {
        mIsGliding = false;
    }
}

void Player::UseJump() {
    //Início do pulo
    if (!mIsFireAttacking) {
        if (!mDashComponent->GetIsDashing() && !mIsDiving && !mIsHooking) {
            // Pulo do chao
            if ((mTimerOutOfGroundToJump < mMaxTimeOutOfGroundToJump || mIsOnSpike) && !mIsJumping && mCanJump && (mWallJumpTimer >= mWallJumpMaxTime)) {
                mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x, mJumpForce)
                                                 + mMovingGroundVelocity);
                mIsJumping = true;
                mCanJump = false;
                mJumpTimer = 0.0f;
                mTimerOutOfGroundToJump = mMaxTimeOutOfGroundToJump;
                mGame->GetAudio()->PlaySound("Jump/Jump1.wav");
                StartGrassEffect(GrassEffectType::Jump);
                for (JumpEffect* j: mJumpEffects) {
                    if (j->GetState() == ActorState::Paused) {
                        j->SetState(ActorState::Active);
                        j->StartEffect(JumpEffect::EffectType::TakeOff);
                        break;
                    }
                }
            }
            // Wall jumping
            if ((mTimerOutOfWallToJump < mMaxTimeOutOfWallToJump) && !mIsJumping && mCanJump) {
                if (mWallSlideSide == WallSlideSide::left) {
                    mRigidBodyComponent->SetVelocity(Vector2(-mMoveSpeed, mJumpForce) + mMovingGroundVelocity);
                    SetRotation(Math::Pi);
                    SetScale(Vector2(-1, 1));
                }
                else if (mWallSlideSide == WallSlideSide::right) {
                    mRigidBodyComponent->SetVelocity(Vector2(mMoveSpeed, mJumpForce) + mMovingGroundVelocity);
                    SetRotation(0);
                    SetScale(Vector2(1, 1));
                }

                mIsJumping = true;
                mCanJump = false;
                mJumpTimer = 0.0f;
                mWallJumpTimer = 0;
                mTimerOutOfWallToJump = mMaxTimeOutOfWallToJump;
                mGame->GetAudio()->PlaySound("Jump/Jump1.wav");
                StartGrassEffect(GrassEffectType::WallJump);
            }
            // Pulo no ar
            if (!(mIsOnGround || mIsWallSliding) && mJumpCountInAir < mSkillManager->MaxJumpsInAir() && mCanJump
                && (mWallJumpTimer >= mWallJumpMaxTime) && !mIsGliding) {
                mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x, mJumpForce * 0.8f)
                                                 + mMovingGroundVelocity);
                mIsJumping = true;
                mCanJump = false;
                mJumpTimer = mMaxJumpTime * 0.4f;
                mJumpCountInAir++; // Incrementa número de pulos
                mGame->GetAudio()->PlaySound("Jump/Jump1.wav");
                for (JumpEffect* j: mJumpEffects) {
                    if (j->GetState() == ActorState::Paused) {
                        j->SetState(ActorState::Active);
                        j->StartEffect(JumpEffect::EffectType::DoubleJump);
                        break;
                    }
                }
            }
        }
    }
    else {
        mIsJumping = false;
        mCanJump = true;
    }
}

void Player::UseSword() {
    if (!mPrevSwordPressed && mSwordCooldownTimer >= mSwordCooldownDuration * mSkillManager->GetSwordAttackSpeedMultiplier() &&
        !mDashComponent->GetIsDashing() && !mIsDiving)
    {
        mGame->GetAudio()->PlayVariantSound("SwordSlash/SwordSlash.wav", 11);
        // Ativa a espada
        if (mIsWallSliding && mRigidBodyComponent->GetVelocity().y - mMovingGroundVelocity.y > 0) {
            if (mWallSlideSide == WallSlideSide::left) {
                mSwordDirection = Math::Pi;
            }
            if (mWallSlideSide == WallSlideSide::right) {
                mSwordDirection = 0;
            }
        }
        mSword->SetState(ActorState::Active);
        mSword->SetRotation(mSwordDirection);
        if (mSwordDirection == Math::Pi) {
            mSword->SetTransformRotation(0.0f);
            mSword->SetScale(Vector2(-1, 1));
        }
        else {
            mSword->SetTransformRotation(mSwordDirection);
            mSword->SetScale(Vector2(1, 1));
        }
        mSword->SetPosition(GetPosition());
        mEnemiesHitBySword.clear();
        mSwordHitGround = false;
        mSwordHitSpike = false;

        // Inicia cooldown
        mSwordCooldownTimer = 0;
    }
}

void Player::UseFireBall() {
    if (mSkillManager->CanFireBall() && mElementalMode == ElementalMode::Fire) {
        if (!mPrevSkill1Pressed &&
            mFireBallCooldownTimer >= mFireBallCooldownDuration &&
            mMana >= mFireballManaCost &&
            !mDashComponent->GetIsDashing() && !mIsDiving)
        {
            std::vector<FireBall* > fireBalls = mGame->GetFireBalls();
            for (FireBall* f: fireBalls) {
                if (f->GetState() == ActorState::Paused) {
                    f->SetState(ActorState::Active);
                    if (mIsWallSliding && mRigidBodyComponent->GetVelocity().y - mMovingGroundVelocity.y > 0) {
                        if (mWallSlideSide == WallSlideSide::left) {
                            SetRotation(Math::Pi);
                            SetScale(Vector2(-1, 1));
                        }
                        if (mWallSlideSide == WallSlideSide::right) {
                            SetRotation(0);
                            SetScale(Vector2(1, 1));
                        }
                    }
                    f->SetRotation(GetRotation());
                    f->SetTransformRotation(0.0f);
                    f->SetScale(Vector2(GetForward().x, 1));
                    f->SetWidth(mFireballWidth * mSkillManager->GetFireBallSizeMultiplier());
                    f->SetHeight(mFireBallHeight * mSkillManager->GetFireBallSizeMultiplier());
                    f->SetSpeed(mFireballSpeed);
                    f->SetDamage(mFireballDamage * mSkillManager->GetFireBallDamageMultiplier());
                    f->SetPosition(GetPosition() + f->GetForward() * (f->GetWidth() / 2));
                    mIsFireAttacking = true;
                    mStopInAirFireBallTimer = 0;
                    mFireballAnimationTimer = 0;
                    mMana -= mFireballManaCost;
                    break;
                }
            }
            // Inicia cooldown
            mFireBallCooldownTimer = 0;
        }
    }
}

void Player::UseFireWisp() {
    if (mSkillManager->CanFireWisp() && mElementalMode == ElementalMode::Fire) {
        if (!mPrevSkill2Pressed &&
            mFireWispCooldownTimer >= mFireWispCooldownDuration &&
            mMana >= mFireWispManaCost &&
            !mDashComponent->GetIsDashing() && !mIsDiving)
        {
            new FireWisp(mGame);
            mMana -= mFireWispManaCost;
            mFireWispCooldownTimer = 0;
        }
    }
}

void Player::UseFreeze(bool up, bool down) {
    if (mSkillManager->CanFreeze() && mElementalMode == ElementalMode::Ice) {
        if (mMana >= mFreezeManaCost && !mDashComponent->GetIsDashing() && !mIsDiving) {
            AttachedEffect freezeEffect;
            if (mIntervalBetweenFreezeEmitTimer >= mIntervalBetweenFreezeEmitDuration) {
                mIsFreezingDown = false;
                mIsFreezingUp = false;
                mIsFreezingFront = false;

                auto* snowBalls = new ParticleSystem(mGame, Particle::ParticleType::BlurParticle, 13.0f * 1.2f, 13.0f, 100.0f, 0.45f, 0.15f);
                snowBalls->SetParticleColor(SDL_Color{255, 255, 255, 180});
                snowBalls->SetParticleGravity(false);
                if (down) {
                    mIsFreezingDown = true;
                    snowBalls->SetEmitDirection(Vector2::UnitY);
                    snowBalls->SetPosition(GetPosition() + Vector2(-10 * GetForward().x, mHeight * 0.3f));
                    freezeEffect.direction = EffectDir::Down;
                }
                else if (up) {
                    mIsFreezingUp = true;
                    snowBalls->SetEmitDirection(Vector2::NegUnitY);
                    snowBalls->SetPosition(GetPosition() - Vector2(10 * GetForward().x, mHeight * 0.3f));
                    freezeEffect.direction = EffectDir::Up;
                }
                else {
                    if (mIsWallSliding && mRigidBodyComponent->GetVelocity().y - mMovingGroundVelocity.y > 0) {
                        if (mWallSlideSide == WallSlideSide::left) {
                            SetRotation(Math::Pi);
                            SetScale(Vector2(-1, 1));
                        }
                        if (mWallSlideSide == WallSlideSide::right) {
                            SetRotation(0);
                            SetScale(Vector2(1, 1));
                        }
                    }
                    mIsFreezingFront = true;
                    snowBalls->SetEmitDirection(GetForward());
                    snowBalls->SetPosition(GetPosition() + Vector2(mWidth * 0.45f * GetForward().x, 11));
                    freezeEffect.direction = EffectDir::Front;
                }
                snowBalls->SetConeSpread(35.0f);
                snowBalls->SetParticleSpeedScale(1.1f);
                snowBalls->SetEnemyCollision(true);
                snowBalls->SetApplyFreeze(true);
                snowBalls->SetFreezeDamage(0.05f);
                snowBalls->SetFreezeIntensity(2.0f);
                snowBalls->SetParticleDrawOrder(4999);
                freezeEffect.system = snowBalls;
                mSnowBallsParticleSystems.emplace_back(freezeEffect);

                auto* iceCloud = new ParticleSystem(mGame, Particle::ParticleType::BlurParticle, 80.0f * 1.2f, 80.0f, 60.0f, 0.55f, 0.2f);
                iceCloud->SetParticleColor(SDL_Color{100, 200, 255, 50});
                iceCloud->SetConeSpread(40.0f);
                iceCloud->SetParticleSpeedScale(0.9f);
                iceCloud->SetParticleGravity(false);
                if (down) {
                    iceCloud->SetEmitDirection(Vector2::UnitY);
                    iceCloud->SetPosition(GetPosition() + Vector2(-10 * GetForward().x, mHeight * 0.3f));
                    freezeEffect.direction = EffectDir::Down;
                }
                else if (up) {
                    iceCloud->SetEmitDirection(Vector2::NegUnitY);
                    iceCloud->SetPosition(GetPosition() - Vector2(10 * GetForward().x, mHeight * 0.3f));
                    freezeEffect.direction = EffectDir::Up;
                }
                else {
                    if (mIsWallSliding && mRigidBodyComponent->GetVelocity().y - mMovingGroundVelocity.y > 0) {
                        if (mWallSlideSide == WallSlideSide::left) {
                            SetRotation(Math::Pi);
                            SetScale(Vector2(-1, 1));
                        }
                        if (mWallSlideSide == WallSlideSide::right) {
                            SetRotation(0);
                            SetScale(Vector2(1, 1));
                        }
                    }
                    iceCloud->SetEmitDirection(GetForward());
                    iceCloud->SetPosition(GetPosition() + Vector2(mWidth * 0.45f * GetForward().x, 11));
                    freezeEffect.direction = EffectDir::Front;
                }
                iceCloud->SetGroundCollision(false);
                freezeEffect.system = iceCloud;
                mIceCloudParticleSystems.emplace_back(freezeEffect);

                mMana -= mFreezeManaCost;
                mIntervalBetweenFreezeEmitTimer = 0;
            }
        }
        else {
            mIsFreezingDown = false;
            mIsFreezingUp = false;
            mIsFreezingFront = false;
        }
    }
}

void Player::UsePillar() {
    if (mSkillManager->CanCreatePillar() && mElementalMode == ElementalMode::Earth) {
        if (!mPrevSkill2Pressed &&
            mMana >= mPillarManaCost &&
            mPillarAnimationTimer >= mPillarAnimationDuration &&
            mIsOnGround &&
            !mIsOnMovingGround &&
            !mDashComponent->GetIsDashing() && !mIsDiving)
        {
            mRigidBodyComponent->SetVelocity(Vector2::Zero);
            mAlreadyCreatedPillar = false;
            mPillarAnimationTimer = 0;
            mMana -= mPillarManaCost;
        }
    }
}

void Player::UseHeal() {
    if (mHealCount > 0 && mHealthPoints < mBaseMaxHealthPoints * mSkillManager->GetMaxHealthPointsMultiplier() && mIsOnGround) {
        mIsHealing = true;
        if (mHealAnimationTimer >= mHealAnimationDuration) {
            mHealAnimationTimer = 0;
            mHealthPoints += mHealAmount * mSkillManager->GetHealAmountMultiplier();
            mHealCount--;
            if (mHealthPoints > mBaseMaxHealthPoints * mSkillManager->GetMaxHealthPointsMultiplier()) {
                mHealthPoints = mBaseMaxHealthPoints * mSkillManager->GetMaxHealthPointsMultiplier();
            }
        }
    }
    else {
        mIsHealing = false;
        mHealAnimationTimer = 0;
    }
}

void Player::UseHook(HookPoint* nearestHookPoint) {
    if (mSkillManager->CanHook()) {
        if (nearestHookPoint &&
            !mPrevHookPressed &&
            !mDashComponent->GetIsDashing() &&
            !mIsDiving &&
            mHookCooldownTimer >= mHookCooldownDuration)
        {
            mHookPoint = nearestHookPoint;
            Vector2 dir = (nearestHookPoint->GetPosition() - GetPosition());
            if (dir.Length() > 0) {
                dir.Normalize();
            }

            mHookDirection = dir;

            // Configura o alvo final
            mHookEnd = nearestHookPoint->GetPosition();

            // A ponta da corda começa na posição do jogador
            mCurrentRopeTip = GetPosition();

            // Ativa o estado de ARREMESSO (Throwing), mas NÃO o de puxar (Hooking)
            mIsHookThrowing = true;
            mIsHooking = false; // Garante que não puxa ainda

            mHookCooldownTimer = 0.0f;

            // Inicia a animação visual (o componente precisa ficar visível)
            mIsHookAnimating = true;

            if (mVineRope) {
                mVineRope->SetState(ActorState::Active);
                mVineRope->Activate();
                mVineRope->SetEndpoints(GetPosition(), mCurrentRopeTip);
            }
        }
    }
}

void Player::SetElementalMode(ElementalMode mode) {
    if (mElementalMode == mode) {
        return;
    }

    mElementalMode = mode;
    switch (mElementalMode) {
        case ElementalMode::Fire:
            FireTransformationEffect();
            break;

        case ElementalMode::Lightning:
            LightningTransformationEffect();
            break;

        case ElementalMode::Ice:
            IceTransformationEffect();
            break;

        case ElementalMode::Earth:
            EarthTransformationEffect();
            break;
    }
}

void Player::StartGrassEffect(GrassEffectType type) {
    if (mIsGrassParticle) {
        ParticleSystem* grass = nullptr;
        switch (type) {
            case GrassEffectType::Run:
                grass = new ParticleSystem(mGame, Particle::ParticleType::Grass,
                  12.17f,
                  5.5f,
                  20.0f,
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
                grass->SetParticleDrawOrder(mDrawComponent->GetDrawOrder() + 1);
                grass->SetPosition(GetPosition() + Vector2(0, mHeight * 0.5f));
                break;

            case GrassEffectType::Jump:
                grass = new ParticleSystem(mGame, Particle::ParticleType::Grass,
                  12.17f,
                  5.5f,
                  220.0f,
                  0.9f,
                  0.1f);

                grass->SetEmitArea(Vector2(mWidth, mHeight));
                grass->SetParticleGravity(true);
                grass->SetParticleGravityForce(450.0f);
                grass->SetEmitDirection(Vector2::NegUnitY);
                grass->SetGroundCollision(false);
                grass->SetConeSpread(180.0f);
                grass->SetParticleSpeedScale(0.30f);
                grass->SetParticleTextureFactor(1.0f);
                grass->SetParticleAutoRotate(true);
                grass->SetParticleRotationSpeed(1.0f);
                grass->SetParticleDrawOrder(mDrawComponent->GetDrawOrder() + 1);
                grass->SetPosition(GetPosition() + Vector2(0, mHeight * 0.3f));
                break;

            case GrassEffectType::Land:
                grass = new ParticleSystem(mGame, Particle::ParticleType::Grass,
                  12.17f,
                  5.5f,
                  180.0f,
                  0.9f,
                  0.1f);

                grass->SetEmitArea(Vector2(mWidth, mHeight));
                grass->SetParticleGravity(true);
                grass->SetParticleGravityForce(450.0f);
                grass->SetEmitDirection(Vector2::NegUnitY);
                grass->SetGroundCollision(false);
                grass->SetConeSpread(360.0f);
                grass->SetParticleSpeedScale(0.18f);
                grass->SetParticleTextureFactor(1.0f);
                grass->SetParticleAutoRotate(true);
                grass->SetParticleRotationSpeed(1.0f);
                grass->SetParticleDrawOrder(mDrawComponent->GetDrawOrder() + 1);
                grass->SetPosition(GetPosition() + Vector2(0, mHeight * 0.3f));
                break;

            case GrassEffectType::WallJump:
                grass = new ParticleSystem(mGame, Particle::ParticleType::Grass,
                  12.17f,
                  5.5f,
                  180.0f,
                  0.6f,
                  0.1f);

                grass->SetEmitArea(Vector2(mWidth, mHeight));
                grass->SetParticleGravity(true);
                grass->SetParticleGravityForce(450.0f);
                grass->SetGroundCollision(false);
                grass->SetConeSpread(30.0f);
                grass->SetParticleSpeedScale(0.38f);
                grass->SetParticleTextureFactor(1.0f);
                grass->SetParticleAutoRotate(true);
                grass->SetParticleRotationSpeed(1.0f);
                grass->SetParticleDrawOrder(mDrawComponent->GetDrawOrder() + 1);
                if (GetForward().x == 1) {
                    grass->SetEmitDirection(Vector2(0.71f, -0.81f));
                    grass->SetPosition(GetPosition() + Vector2(mWidth * -0.6f, mHeight * 0.3f));
                }
                else {
                    grass->SetEmitDirection(Vector2(-0.71f, -0.81f));
                    grass->SetPosition(GetPosition() + Vector2(mWidth * 0.6f, mHeight * 0.3f));
                }
                break;

            case GrassEffectType::Dash:
                grass = new ParticleSystem(mGame, Particle::ParticleType::Grass,
                  12.17f,
                  5.5f,
                  380.0f,
                  0.9f,
                  0.1f);

                grass->SetEmitArea(Vector2(mWidth * 4.0f, mHeight * 2.0f));
                grass->SetParticleGravity(true);
                grass->SetParticleGravityForce(450.0f);
                grass->SetGroundCollision(false);
                grass->SetConeSpread(30.0f);
                grass->SetParticleSpeedScale(0.58f);
                grass->SetParticleTextureFactor(1.0f);
                grass->SetParticleAutoRotate(true);
                grass->SetParticleRotationSpeed(1.0f);
                grass->SetParticleDrawOrder(mDrawComponent->GetDrawOrder() + 1);
                if (GetForward().x == 1) {
                    grass->SetEmitDirection(Vector2(-0.71f, -0.71f));
                }
                else {
                    grass->SetEmitDirection(Vector2(0.71f, -0.71f));
                }
                grass->SetPosition(GetPosition() + Vector2(0, mHeight * 0.3f));
                break;
        }
    }
    else {
        ParticleSystem* dust = nullptr;
        ParticleSystem* dustAux = nullptr;
        switch (type) {
            case GrassEffectType::Run:
                dust = new ParticleSystem(mGame, Particle::ParticleType::BlurParticle,
                  40.0f * 1.2f,
                  40.0f,
                  80.0f,
                  0.5f,
                  0.1f);

                dust->SetEmitArea(Vector2(mWidth * 0.3f, mHeight * 0.3f));
                dust->SetParticleGravity(false);
                dust->SetEmitDirection(Vector2::NegUnitY);
                dust->SetGroundCollision(false);
                dust->SetConeSpread(20.0f);
                dust->SetParticleSpeedScale(0.07f);
                dust->SetParticleTextureFactor(0.0f);
                dust->SetParticleColor(SDL_Color{97, 80, 73, 40});
                dust->SetParticleAutoRotate(true);
                dust->SetParticleRotationSpeed(1.0f);
                dust->SetParticleDrawOrder(98);
                dust->SetPosition(GetPosition() + Vector2(0, mHeight * 0.5f));
                break;

            case GrassEffectType::Jump:
                dustAux = new ParticleSystem(mGame, Particle::ParticleType::BlurParticle,
                30.0f * 1.2f,
                30.0f,
                200.0f,
                0.5f,
                0.1f);

                dustAux->SetEmitArea(Vector2(mWidth, mHeight));
                dustAux->SetParticleGravity(false);
                dustAux->SetEmitDirection(Vector2::NegUnitY);
                dustAux->SetGroundCollision(false);
                dustAux->SetConeSpread(180.0f);
                dustAux->SetParticleSpeedScale(0.05f);
                dustAux->SetParticleTextureFactor(0.0f);
                dustAux->SetParticleColor(SDL_Color{97, 80, 73, 40});
                dustAux->SetParticleAutoRotate(true);
                dustAux->SetParticleRotationSpeed(1.0f);
                dustAux->SetParticleDrawOrder(98);
                dustAux->SetPosition(GetPosition() + Vector2(0, mHeight * 0.3f));

                dust = new ParticleSystem(mGame, Particle::ParticleType::BlurParticle,
                30.0f * 1.2f,
                30.0f,
                100.0f,
                0.4f,
                0.3f);

                dust->SetEmitArea(Vector2(mWidth * 0.3f, mHeight * 0.3f));
                dust->SetParticleGravity(false);
                dust->SetEmitDirection(Vector2::Normalize(mRigidBodyComponent->GetVelocity()));
                dust->SetGroundCollision(false);
                dust->SetConeSpread(10.0f);
                dust->SetParticleSpeedScale(0.45f);
                dust->SetParticleTextureFactor(0.0f);
                dust->SetParticleColor(SDL_Color{97, 80, 73, 40});
                dust->SetParticleAutoRotate(true);
                dust->SetParticleRotationSpeed(1.0f);
                dust->SetParticleDrawOrder(98);
                dust->SetPosition(GetPosition() + Vector2(0, mHeight * 0.3f));
                break;

            case GrassEffectType::Land:
                dust = new ParticleSystem(mGame, Particle::ParticleType::BlurParticle,
                30.0f * 1.2f,
                30.0f,
                400.0f,
                0.5f,
                0.1f);

                dust->SetEmitArea(Vector2(mWidth, mHeight));
                dust->SetParticleGravity(false);
                dust->SetEmitDirection(Vector2::NegUnitY);
                dust->SetGroundCollision(false);
                dust->SetConeSpread(360.0f);
                dust->SetParticleSpeedScale(0.08f);
                dust->SetParticleTextureFactor(0.0f);
                dust->SetParticleColor(SDL_Color{97, 80, 73, 40});
                dust->SetParticleAutoRotate(true);
                dust->SetParticleRotationSpeed(1.0f);
                dust->SetParticleDrawOrder(98);
                dust->SetPosition(GetPosition() + Vector2(0, mHeight * 0.5f));
                break;

            case GrassEffectType::WallJump:
                dust = new ParticleSystem(mGame, Particle::ParticleType::BlurParticle,
                35.0f * 1.2f,
                35.0f,
                150.0f,
                0.4f,
                0.3f);

                dust->SetEmitArea(Vector2(mWidth * 0.3f, mHeight * 0.3f));
                dust->SetParticleGravity(false);
                dust->SetGroundCollision(false);
                dust->SetConeSpread(20.0f);
                dust->SetParticleSpeedScale(0.45f);
                dust->SetParticleTextureFactor(0.0f);
                dust->SetParticleColor(SDL_Color{97, 80, 73, 40});
                dust->SetParticleAutoRotate(true);
                dust->SetParticleRotationSpeed(1.0f);
                dust->SetParticleDrawOrder(98);
                if (GetForward().x == 1) {
                    dust->SetEmitDirection(Vector2(0.71f, -0.81f));
                    dust->SetPosition(GetPosition() + Vector2(mWidth * -0.6f, mHeight * 0.3f));
                }
                else {
                    dust->SetEmitDirection(Vector2(-0.71f, -0.81f));
                    dust->SetPosition(GetPosition() + Vector2(mWidth * 0.6f, mHeight * 0.3f));
                }
                break;

            case GrassEffectType::Dash:
                dust = new ParticleSystem(mGame, Particle::ParticleType::BlurParticle,
                60.0f * 1.2f,
                60.0f,
                600.0f,
                0.5f,
                0.1f);

                dust->SetEmitArea(Vector2(mWidth * 3.5f, mHeight * 1.6f));
                dust->SetParticleGravity(false);
                dust->SetGroundCollision(false);
                dust->SetConeSpread(30.0f);
                dust->SetParticleSpeedScale(0.58f);
                dust->SetParticleTextureFactor(0.0f);
                dust->SetParticleColor(SDL_Color{97, 80, 73, 40});
                dust->SetParticleAutoRotate(true);
                dust->SetParticleRotationSpeed(1.0f);
                dust->SetParticleDrawOrder(98);
                if (GetForward().x == 1) {
                    dust->SetEmitDirection(Vector2(-0.87f, -0.50f));
                }
                else {
                    dust->SetEmitDirection(Vector2(0.87f, -0.50f));
                }
                dust->SetPosition(GetPosition() + Vector2(0, mHeight * 0.3f));
                break;
        }
    }
}

void Player::OpenElementalMenu() {
    if (!mRadialMenu) {
        mRadialMenu = new RadialMenu(mGame, "../Assets/Fonts/K2D-Bold.ttf", 200.0f);

        // Adiciona os elementos
        mRadialMenu->AddRadialOption("", [this]() { SetElementalMode(ElementalMode::Fire); });
        mRadialMenu->AddRadialOption("", [this]() { SetElementalMode(ElementalMode::Lightning); });
        mRadialMenu->AddRadialOption("", [this]() { SetElementalMode(ElementalMode::Earth); });
        mRadialMenu->AddRadialOption("", [this]() { SetElementalMode(ElementalMode::Ice); });

        mRadialMenu->SetSelectedOption(static_cast<int>(mElementalMode));

        SDL_Window* window = mGame->GetWindow();
        if (window) {
            int width, height;
            SDL_GetWindowSize(window, &width, &height);

            // Teleporta o cursor para o centro da tela
            SDL_WarpMouseInWindow(window, width / 2, height / 2);
        }
    }
}

void Player::FireTransformationEffect() {
    // float transitionDuration = 0.3f;
    //
    // // 1. O "Burst" de Ignição: Uma aura rápida que se expande 360 graus
    // auto* ignition = new ParticleSystem(mGame, Particle::ParticleType::BlurParticle, 20.0f, 50.0f, 0.25f, transitionDuration);
    // ignition->SetPosition(GetPosition());
    // ignition->SetParticleColor(SDL_Color{255, 50, 20, 255}); // Começa mais amarelado/branco (quente)
    // ignition->SetConeSpread(360.0f); // Expande para todos os lados em volta do player
    // ignition->SetEmitDirection(Vector2::NegUnitY);
    // ignition->SetParticleSpeedScale(0.3f); // Um pouco mais rápido para o "impacto" inicial
    // ignition->SetParticleGravity(false);
    // ignition->SetGroundCollision(false);
    // ignition->SetAdditiveBlending(true);
    // ignition->SetParticleDrawOrder(5001);
    // ignition->SetFollowTarget(this);
    //
    // // 2. As Fagulhas: Menos quantidade, subindo suavemente ao redor do player
    // auto* embers = new ParticleSystem(mGame, Particle::ParticleType::SolidParticle, 2.0f, 15.0f, 0.4f, transitionDuration);
    // embers->SetPosition(GetPosition()); // Idealmente, levemente abaixo do centro do player
    // embers->SetParticleColor(SDL_Color{255, 100, 10, 255}); // Laranja clássico
    // embers->SetConeSpread(90.0f); // Cone voltado para cima
    // embers->SetEmitDirection(Vector2::NegUnitY);
    // embers->SetParticleSpeedScale(0.25f); // Sobe bem devagar
    // embers->SetParticleGravity(false);
    // embers->SetGroundCollision(false);
    // embers->SetAdditiveBlending(true);
    // embers->SetParticleDrawOrder(5001);
    // embers->SetFollowTarget(this);
}

void Player::LightningTransformationEffect() {

}

void Player::IceTransformationEffect() {

}

void Player::EarthTransformationEffect() {

}

void Player::IncreaseStone(std::string stoneType, int value) {
    if (stoneType == "earthStone") {
        mEarthStone += value;
    }
    else if (stoneType == "fireStone") {
        mFireStone += value;
    }
    else if (stoneType == "iceStone") {
        mIceStone += value;
    }
    else if (stoneType == "lightningStone") {
        mLightningStone += value;
    }
}

void Player::DecreaseStone(std::string stoneType, int value) {
    if (stoneType == "earthStone") {
        mEarthStone -= value;
    }
    else if (stoneType == "fireStone") {
        mFireStone -= value;
    }
    else if (stoneType == "iceStone") {
        mIceStone -= value;
    }
    else if (stoneType == "lightningStone") {
        mLightningStone -= value;
    }
}

int Player::GetStone(std::string stoneType) {
    if (stoneType == "earthStone") {
        return mEarthStone;
    }
    else if (stoneType == "fireStone") {
        return mFireStone;
    }
    else if (stoneType == "iceStone") {
        return mIceStone;
    }
    else if (stoneType == "lightningStone") {
        return mLightningStone;
    }
    return 0;
}

void Player::SetStone(std::string stoneType, int value) {
    if (stoneType == "earthStone") {
        mEarthStone = value;
    }
    else if (stoneType == "fireStone") {
        mFireStone = value;
    }
    else if (stoneType == "iceStone") {
        mIceStone = value;
    }
    else if (stoneType == "lightningStone") {
        mLightningStone = value;
    }
}

void Player::ManageCombatBoxes(float deltaTime) {
    mCombatBoxComponent->SetBoxActive("body", true);
    if (mIsLightningDashing) {
        mCombatBoxComponent->SetBoxActive("lightningDash", true);
        mCombatBoxComponent->SetBoxActive("body", false);
    }
    else {
        mCombatBoxComponent->SetBoxActive("lightningDash", false);
    }

    if (mIsDiving || mIsGroundSlamRecovering) {
        mCombatBoxComponent->SetBoxActive("groundSlam", true);
        mCombatBoxComponent->SetBoxActive("body", false);
    }
    else {
        mCombatBoxComponent->SetBoxActive("groundSlam", false);
    }
}

void Player::ManageAnimations() {
    mDrawComponent->SetAnimFPS(10.0f);
    if (mIsDead) {
        mDrawComponent->SetAnimation("die");
        mDrawComponent->SetAnimFPS(4.0f / mDeathAnimationDuration);
    }
    else if (mHurtTimer < mHurtDuration) {
        mDrawComponent->SetAnimation("hurt");
    }
    else if (mIsDiving) {
        mDrawComponent->SetAnimation("dive");
        mDrawComponent->SetAnimFPS(2);
    }
    else if (mIsGroundSlamRecovering) {
        mDrawComponent->SetAnimation("groundSlam");
        mDrawComponent->SetAnimFPS(7.0f / mGroundSlamRecoveryDuration);
    }
    else if (mPillarAnimationTimer < mPillarAnimationDuration) {
        mDrawComponent->SetAnimation("pillar");
        mDrawComponent->SetAnimFPS(10.0f / mPillarAnimationDuration);
    }
    else if (mDashComponent->GetIsDashing()) {
        if (mIsLightningDashing) {
            mDrawComponent->SetAnimation("lightningDash");
            mDrawComponent->SetAnimFPS(9.0f / mLightningDashDuration);
        }
        else {
            mDrawComponent->SetAnimation("dash");
        }
    }
    else if (mSword->GetState() == ActorState::Active) {
        if (mSword->GetRotation() == 3 * Math::Pi / 2) {
            mDrawComponent->SetAnimation("attackUp");
        }
        if (mSword->GetRotation() == Math::Pi / 2) {
            mDrawComponent->SetAnimation("attackDown");
        }
        if (mSword->GetRotation() == 0 || mSword->GetRotation() == Math::Pi) {
            SetRotation(mSword->GetRotation());
            SetScale(Vector2(mSword->GetForward().x, 1));
            mDrawComponent->SetAnimation("attackFront");
        }
        mDrawComponent->SetAnimFPS(3.0f / 0.15f);
    }
    else if (mFireballAnimationTimer < mFireballAnimationDuration) {
        mDrawComponent->SetAnimation("fireball");
        mDrawComponent->SetAnimFPS(2.0f / mFireballAnimationDuration);
    }
    else if (mIsFreezingFront) {
        mDrawComponent->SetAnimation("freezeFront");
        mDrawComponent->SetAnimFPS(8.0f);
    }
    else if (mIsFreezingDown) {
        mDrawComponent->SetAnimation("freezeDown");
        mDrawComponent->SetAnimFPS(8.0f);
    }
    else if (mIsFreezingUp) {
        mDrawComponent->SetAnimation("freezeUp");
        mDrawComponent->SetAnimFPS(8.0f);
    }
    else if ((!mIsOnMovingGround && !mIsOnGround && mIsWallSliding &&
               mRigidBodyComponent->GetVelocity().y > 0) ||
              (mIsOnMovingGround && !mIsOnGround && mIsWallSliding &&
               mRigidBodyComponent->GetVelocity().y - mMovingGroundVelocity.y > 0))
    {
        // mDrawAnimatedComponent->UseFlip(true);
        if (mWallSlideSide == WallSlideSide::left) {
            // mDrawAnimatedComponent->SetFlip(SDL_FLIP_NONE);
        }
        else if (mWallSlideSide == WallSlideSide::right) {
            // mDrawAnimatedComponent->SetFlip(SDL_FLIP_HORIZONTAL);
        }
        // SetScale(Vector2(-1, 1));
        mDrawComponent->SetAnimation("wallSlide");
    }
    else if (mIsRunning && mIsOnGround) {
        mDrawComponent->SetAnimation("run");
    }
    else if (!mIsOnGround) {
        if (mIsGliding) {
            mDrawComponent->SetAnimation("jumpUp");
        }
        else if (mRigidBodyComponent->GetVelocity().y < -200) {
            mDrawComponent->SetAnimation("jumpUp");
        }
        else if (mRigidBodyComponent->GetVelocity().y > 200) {
            mDrawComponent->SetAnimation("falling");
        }
        else if (mRigidBodyComponent->GetVelocity().y > -200 &&
            mRigidBodyComponent->GetVelocity().y < 200)
        {
            mDrawComponent->SetAnimation("jumpApex");
        }
    }
    else if (mIsHealing) {
        mDrawComponent->SetAnimation("heal");
        mDrawComponent->SetAnimFPS(10.0f / (mHealAnimationDuration));
    }
    else {
        mDrawComponent->SetAnimation("idle");
        mDrawComponent->SetAnimFPS(6.0f);
    }

    if (mIsInvulnerable && mHurtTimer > mHurtDuration) {
        if (mBlink) {
            // mDrawAnimatedComponent->SetTransparency(100);
            mDrawComponent->SetAlpha(0.45f);
        }
        else {
            // mDrawAnimatedComponent->SetTransparency(255);
            mDrawComponent->SetAlpha(1.0f);
        }
    }
    else {
        // mDrawAnimatedComponent->SetTransparency(255);
        mDrawComponent->SetAlpha(1.0f);
    }
}


void Player::ReceiveHit(float damage, Vector2 knockBackDirection, DamageType damageType) {
    if (mIFramesTimer > 0) {
        if (damageType == DamageType::Projectile || damageType == DamageType::Normal) {
            return; // Ignora o dano completamente
        }
    }

    if (mIsLightningDashing && mDashComponent->GetIsDashing()) {
        if (damageType == DamageType::Projectile || damageType == DamageType::Normal) {
            return; // Ignora o dano completamente
        }
    }

    if (mIsDiving || mIsGroundSlamRecovering) {
        if (damageType == DamageType::Projectile || damageType == DamageType::Normal) {
            return; // Ignora o dano completamente
        }
    }

    if (!mIsInvulnerable && mGame->GetGamePlayState() == Game::GamePlayState::Playing) {
        mHealthPoints -= damage;
        mIsDiving = false;
        mDiveEffect->StopDash();
        mDashComponent->StopDash();
        mLightningDashEffect->StopEffect();
        mIsInvulnerable = true;
        mHurtTimer = 0;

        Vector2 vel = mRigidBodyComponent->GetVelocity();
        if (vel.Length() > 0) {
            vel.Normalize();
        }

        Vector2 knockBack = knockBackDirection * mKnockBackSpeed + vel * (mKnockBackSpeed / 3);
        if (knockBack.Length() > 0) {
            knockBack.Normalize();
        }
        mRigidBodyComponent->SetVelocity(knockBack * mKnockBackSpeed);
        // mRigidBodyComponent->SetVelocity(knockBackDirection * mKnockBackSpeed + vel * (mKnockBackSpeed / 3));
        mKnockBackTimer = 0;
        mInvulnerableTimer = 0;
        mGame->ActiveHitStop();
        mGame->ActiveDamageFlash();
        mHealAnimationTimer = 0;
        mGame->GetCamera()->StartCameraShake(0.5, mCameraShakeStrength);
        mGame->GetAudio()->PlaySound("Damage/Damage.wav");
    }
}

bool Player::Died() {
    if (mHealthPoints <= 0) {
        if (mIsDead == false) {
            mIsDead = true;
        }
        return true;
    }
    return false;
}

void Player::SetIsEnteringLevel(Vector2 velocity, float enteringOffset) {
    mIsEnteringLevel = true;
    mEnteringLevelTimer = 0;
    if (velocity.y < 0) {
        SetPosition(GetPosition() + Vector2(enteringOffset * -GetForward().x, 0));
        velocity *= Vector2(GetForward().x, 1);
    }
    mRigidBodyComponent->SetVelocity(velocity);
}
