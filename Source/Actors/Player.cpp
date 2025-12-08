//
// Created by roger on 22/04/2025.
//

#include "Player.h"
#include <cfloat>
#include "Checkpoint.h"
#include "Effect.h"
#include "HookPoint.h"
#include "Mushroom.h"
#include "PillarGround.h"
#include "../Game.h"
#include "../RadialMenu.h"
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

Player::Player(Game* game)
    :Actor(game)
    ,mStartingPosition(Vector2::Zero)
    ,mElementalMode(ElementalMode::Earth)
    ,mWidth(45 * mGame->GetScale())
    ,mHeight(75 * mGame->GetScale())

    ,mIsOnGround(false)
    ,mIsOnSpike(false)
    ,mIsOnMovingGround(false)
    ,mMovingGroundVelocity(Vector2::Zero)
    ,mMoveSpeed(700 * mGame->GetScale())

    ,mMaxTimeOutOfGroundToJump(0.07)
    ,mTimerOutOfGroundToJump(0.0f)
    ,mMaxTimeOutOfWallToJump(0.07)
    ,mTimerOutOfWallToJump(0.0f)

    ,mIsJumping(false)
    ,mJumpTimer(0.0f)
    ,mMaxJumpTime(0.33f)
    ,mJumpForce(-750.0f * mGame->GetScale())
    ,mCanJump(true)
    ,mJumpCountInAir(0)
    ,mMaxJumpsInAir(1)
    ,mLowGravity(50.0f * mGame->GetScale())
    ,mMediumGravity(3300.0f * mGame->GetScale())
    ,mHighGravity(4500.0f * mGame->GetScale())

    ,mCanDash(true)
    ,mDashSpeed(1500)
    ,mDashDuration(0.2f)
    ,mDashCooldown(0.5f)
    ,mIsLightningDashing(false)
    ,mLightningDashSpeed(1800)
    ,mLightningDashDuration(0.2f)
    ,mLightningDashCooldown(0.5f)
    ,mLightningDashDamage(5.0f)
    ,mLightningDashManaCost(10.0f)
    ,mLightningDashIFramesDuration(mLightningDashDuration + 0.1f)

    ,mCanGroundSlam(true)
    ,mPrevGroundSlamPressed(false)
    ,mIsGroundSlamStarting(false)
    ,mIsGroundSlamRecovering(false)
    ,mIsDiving(false)
    ,mGroundSlamStartDuration(0.15f)
    ,mGroundSlamRecoveryDuration(0.45f)
    ,mGroundSlamTimer(0.0f)
    ,mGroundSlamSpeed(2000)
    ,mGroundSlamDamage(15)
    ,mGroundSlamImpactDist(300.0f)
    ,mGroundSlamImpactHeightRange(mHeight * 1.8f)
    ,mGroundSlamImpactDamage(10)
    ,mGroundSlamIFramesDuration(0.3f)
    ,mGroundSlamCameraShakeStrength(80.0f)
    ,mGroundSlamCameraShakeDuration(0.2f)
    ,mGroundSlamManaCost(20.0f)

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

    ,mCanFireBall(true)
    ,mPrevFireBallPressed(false)
    ,mFireBallCooldownDuration(0.1f)
    ,mFireBallCooldownTimer(0.0f)
    ,mIsFireAttacking(false)
    ,mStopInAirFireBallTimer(0.0f)
    ,mStopInAirFireBallMaxDuration(0.0f)
    ,mFireballRecoil(0.0f * mGame->GetScale())
    ,mFireballWidth(45 * mGame->GetScale())
    ,mFireBallHeight(45 * mGame->GetScale())
    ,mFireballSpeed(1500 * mGame->GetScale())
    ,mFireballDamage(10.0f)
    ,mMaxMana(90.0f)
    ,mMana(90.0f)
    ,mManaIncreaseRate(6.0f)
    ,mFireballManaCost(30.0f)
    ,mFireballAnimationDuration(0.2f)
    ,mFireballAnimationTimer(mFireballAnimationDuration)

    ,mCanFreeze(true)
    ,mIsFreezingFront(false)
    ,mIsFreezingUp(false)
    ,mIsFreezingDown(false)
    ,mIntervalBetweenFreezeEmitDuration(0.1f)
    ,mIntervalBetweenFreezeEmitTimer(0.0f)
    ,mFreezeManaCost(2.0f)

    ,mCanCreatePillar(true)
    ,mPrevCreatePillarPressed(false)
    ,mPillarDistanceFromPlayer(100.0f)
    ,mPillarManaCost(20.0f)
    ,mPillarAnimationDuration(0.5f)
    ,mPillarAnimationTimer(mPillarAnimationDuration)
    ,mAlreadyCreatedPillar(false)

    ,mCanWallSlide(true)
    ,mIsWallSliding(false)
    ,mWallSlideSide(WallSlideSide::notSliding)
    ,mWallSlideSpeed(300 * mGame->GetScale())
    ,mTryingLeavingWallSlideLeft(0)
    ,mTryingLeavingWallSlideRight(0)
    ,mTimerToLeaveWallSlidingLeft(0.0f)
    ,mTimerToLeaveWallSlidingRight(0.0f)
    ,mMaxTimerToLiveWallSliding(0.0f)

    ,mIsGoingRight(false)
    ,mIsGoingLeft(false)

    ,mWallJumpMaxTime(0.12f)
    ,mWallJumpTimer(0.0f)

    ,mKnockBackSpeed(1000.0f * mGame->GetScale())
    ,mKnockBackTimer(0.0f)
    ,mKnockBackDuration(0.2f)
    ,mCameraShakeStrength(70.0f * mGame->GetScale())

    ,mMaxHealthPoints(70.0f)
    ,mHealthPoints(mMaxHealthPoints)
    ,mIsInvulnerable(false)
    ,mInvulnerableDuration(0.85f)
    ,mInvulnerableTimer(mInvulnerableDuration)
    ,mMaxHealCount(3)
    ,mHealCount(mMaxHealCount)
    ,mHealAmount(30.0f)
    ,mIsHealing(false)
    ,mHealAnimationDuration(0.8f)
    ,mHealAnimationTimer(0.0f)

    ,mIFramesTimer(0.0f)

    ,mMoney(1000)
    ,mStartMoney(0)

    ,mCanHook(true)
    ,mIsHooking(false)
    ,mPrevHookPressed(false)
    ,mHookDirection(Vector2::Zero)
    ,mHookSpeed(1550.0f * mGame->GetScale())
    ,mHookCooldownDuration(0.4f)
    ,mHookCooldownTimer(0.0f)
    ,mHookingDuration(0.3f)
    ,mHookingTimer(0.0f)
    ,mHookEnd(Vector2::Zero)
    ,mHookAnimProgress(0.0f)
    ,mIsHookAnimating(false)
    ,mHookPoint(nullptr)
    ,mHookAnimationDuration(0.45f)
    ,mHookSegments(20)
    ,mHookAmplitude(14.0f * mGame->GetScale())
    ,mHookSegmentHeight(8.0f * mGame->GetScale())

    ,mIsHookThrowing(false)
    ,mCurrentRopeTip(Vector2::Zero)
    ,mRopeThrowSpeed(3000.0f)

    ,mRadialMenu(nullptr)

    ,mIsRunning(false)
    ,mHurtDuration(0.2f)
    ,mHurtTimer(mHurtDuration)
    ,mBlink(false)
    ,mBlinkDuration(0.03f)
    ,mBlinkTimer(mBlinkDuration)

    ,mRunningSoundIntervalDuration(0.3f)
    ,mRunningSoundIntervalTimer(0.0f)
    ,mWasOnGround(false)
    ,mDeathCounter(0)
    ,mDeathAnimationDuration(1.0f)
    ,mDeathAnimationTimer(0.0f)
    ,mIsDead(false)
    ,mInvertControls(false)

    ,mLight(nullptr)

    ,mIsEnteringLevel(false)
    ,mEnteringLevelDuration(0.35f)
    ,mEnteringLevelTimer(0.0f)

    ,mRectComponent(nullptr)
    ,mDrawComponent(nullptr)
    ,mDrawRopeComponent(nullptr)
{
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
                                           "../Assets/Sprites/Esquilo5/Esquilo.png",
                                           "../Assets/Sprites/Esquilo5/Esquilo.json",
                                           mWidth * 4.93f, mWidth * 4.93f * 1.11f, 1002);

    std::vector idle = {32, 33, 34, 35};
    mDrawComponent->AddAnimation("idle", idle);

    std::vector attackFront = {32, 2, 3};
    mDrawComponent->AddAnimation("attackFront", attackFront);

    std::vector attackUp = {32, 4, 5};
    mDrawComponent->AddAnimation("attackUp", attackUp);

    std::vector attackDown = {32, 0, 1};
    mDrawComponent->AddAnimation("attackDown", attackDown);

    std::vector fireball = {12, 13};
    mDrawComponent->AddAnimation("fireball", fireball);

    std::vector freezeFront = {16, 17};
    mDrawComponent->AddAnimation("freezeFront", freezeFront);

    std::vector freezeDown = {14, 15};
    mDrawComponent->AddAnimation("freezeDown", freezeDown);

    std::vector freezeUp = {18, 19};
    mDrawComponent->AddAnimation("freezeUp", freezeUp);

    std::vector groundSlam = {20, 21, 22, 23, 24, 24, 24};
    mDrawComponent->AddAnimation("groundSlam", groundSlam);

    std::vector pillar = {43, 44, 45, 45, 45, 45, 45, 45, 45, 45};
    mDrawComponent->AddAnimation("pillar", pillar);

    std::vector dash = {6, 7, 7, 7, 8};
    mDrawComponent->AddAnimation("dash", dash);

    std::vector lightningDash = {7, 39, 40, 40, 41, 41, 42, 42, 8};
    mDrawComponent->AddAnimation("lightningDash", lightningDash);

    std::vector run = {46, 47, 48, 49, 50, 51};
    mDrawComponent->AddAnimation("run", run);

    std::vector heal = {25, 26, 27, 28, 29, 29, 28, 27, 26, 25};
    mDrawComponent->AddAnimation("heal", heal);

    std::vector wallSlide = {52};
    mDrawComponent->AddAnimation("wallSlide", wallSlide);

    std::vector hurt = {30, 31};
    mDrawComponent->AddAnimation("hurt", hurt);

    std::vector die = {30, 9, 10, 11, 11, 11};
    mDrawComponent->AddAnimation("die", die);

    std::vector jumpUp = {36};
    mDrawComponent->AddAnimation("jumpUp", jumpUp);

    std::vector jumpApex = {37};
    mDrawComponent->AddAnimation("jumpApex", jumpApex);

    std::vector falling = {38};
    mDrawComponent->AddAnimation("falling", falling);

    mDrawComponent->SetAnimation("idle");
    mDrawComponent->SetAnimFPS(10.0f);

    mDrawRopeComponent = new DrawRopeComponent(this, "../Assets/Sprites/Rope/Rope2.png");
    mDrawRopeComponent->SetNumSegments(mHookSegments);
    mDrawRopeComponent->SetAmplitude(mHookAmplitude);
    mDrawRopeComponent->SetSegmentHeight(mHookSegmentHeight);

    // mRectComponent = new RectComponent(this, mWidth, mHeight, RendererMode::LINES);
    // mRectComponent->SetColor(Vector3(255, 255, 0));

    mRigidBodyComponent = new RigidBodyComponent(this, 1, 40000 * mGame->GetScale(), 1600 * mGame->GetScale());
    mAABBComponent = new AABBComponent(this, v1, v3);
    mDashComponent = new DashComponent(this, mLightningDashSpeed, mLightningDashDuration, mLightningDashCooldown);

    mSword = new Sword(mGame, this, mSwordWidth, mSwordHeight, mSwordDuration, mSwordDamage);

    InitLight();

    SetJumpEffects();
}

void Player::SetJumpEffects() {
    mJumpEffects.clear();
    for (int i = 0; i < 5; i++) {
        auto* jumpEffect = new JumpEffect(mGame, this, 0.3f);
        mJumpEffects.emplace_back(jumpEffect);
    }
}

void Player::InitLight() {
    mLight = new Light(mGame);
    mLight->SetRadius(300.0f);
    mLight->SetMaxIntensity(0.65f);
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

    bool heal = mGame->IsActionPressed(Game::Action::Heal, state, &controller) ||
                SDL_GameControllerGetAxis(&controller, SDL_CONTROLLER_AXIS_TRIGGERRIGHT) > 10000;

    bool hook = mGame->IsActionPressed(Game::Action::Hook, state, &controller);

    bool radialMenu = SDL_GameControllerGetAxis(&controller, SDL_CONTROLLER_AXIS_TRIGGERLEFT) > 10000;


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
                    mRigidBodyComponent->SetVelocity(Vector2(-mMoveSpeed + mMovingGroundVelocity.x,
                                                             mRigidBodyComponent->GetVelocity().y));
                    mTryingLeavingWallSlideLeft = 0;
                    mTimerToLeaveWallSlidingLeft = 0;
                }
            }
            else {
                mIsRunning = true;
                mRigidBodyComponent->SetVelocity(Vector2(-mMoveSpeed + mMovingGroundVelocity.x,
                                                         mRigidBodyComponent->GetVelocity().y));
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
                    mRigidBodyComponent->SetVelocity(Vector2(mMoveSpeed + mMovingGroundVelocity.x,
                                                             mRigidBodyComponent->GetVelocity().y));
                    mTryingLeavingWallSlideRight = 0;
                    mTimerToLeaveWallSlidingRight = 0;
                }
            }
            else {
                mIsRunning = true;
                mRigidBodyComponent->SetVelocity(Vector2(mMoveSpeed + mMovingGroundVelocity.x,
                                                         mRigidBodyComponent->GetVelocity().y));
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
        if (mElementalMode == ElementalMode::Earth) {
            if (down && !mIsOnGround) {
                UseGroundSlam();
            }
        }
    }
    else {
        mIsFreezingDown = false;
        mIsFreezingUp = false;
        mIsFreezingFront = false;
    }
    mPrevFireBallPressed = skill1;
    mPrevGroundSlamPressed = skill1;

    // Skill2
    if (skill2) {
        if (mElementalMode == ElementalMode::Earth) {
            UsePillar();
        }
    }
    mPrevCreatePillarPressed = skill2;

    // Heal
    if (heal && !left && !leftSlow && !right && !rightSlow && !jump && !dash && !sword && !skill1) {
        UseHeal();
    }
    else {
        mIsHealing = false;
        mHealAnimationTimer = 0;
    }

    // Hook
    if (mCanHook) {
        std::vector<HookPoint* > hookPoints = mGame->GetHookPoints();

        HookPoint* nearestHookPoint = nullptr;

        float nearestDistance = FLT_MAX;

        for (HookPoint* hp: hookPoints) {
            float dist = (GetPosition() - hp->GetPosition()).Length();
            if (dist < hp->GetRadius()) {
                float distX = GetPosition().x - hp->GetPosition().x;

                // Verifica se o jogador está olhando para a direção do hookPoint
                bool lookingRight = GetRotation() == 0 && distX < 0;
                bool lookingLeft = GetRotation() == Math::Pi && distX > 0;

                if ((lookingRight || lookingLeft) && dist < nearestDistance) {
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

    if (mSwordCooldownTimer < mSwordCooldownDuration) {
        mSwordCooldownTimer += deltaTime;
    }

    if (mFireBallCooldownTimer < mFireBallCooldownDuration) {
        mFireBallCooldownTimer += deltaTime;
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
        mHookAnimProgress += deltaTime / mHookAnimationDuration;
        if (mHookAnimProgress >= 1.0f) {
            mHookAnimProgress = 1.0f;
        }
        if (mDrawRopeComponent) {
            mDrawRopeComponent->SetEndpoints(GetPosition(), mHookEnd);
            mDrawRopeComponent->SetAnimationProgress(mHookAnimProgress);
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

    if (mMana < mMaxMana) {
        mMana += mManaIncreaseRate * deltaTime;
        if (mMana > mMaxMana) {
            mMana = mMaxMana;
        }
    }

    if (mIsHealing) {
        mHealAnimationTimer += deltaTime;
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

    mIsOnGround = false;
    mIsOnSpike = false;
    mIsOnMovingGround = false;
    mMovingGroundVelocity = Vector2::Zero;
    mIsWallSliding = false;
    if (mTimerOutOfWallToJump > mMaxTimeOutOfWallToJump) {
        mWallSlideSide = WallSlideSide::notSliding;
    }

    if (mIsDiving) {
        mRigidBodyComponent->SetMaxSpeedY(2500);
    }
    else {
        mRigidBodyComponent->SetMaxSpeedY(1600);
    }

    if (mIsLightningDashing) {
        if (!mDashComponent->GetIsDashing()) {
            mIsLightningDashing = false;
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
        if (!mDashComponent->GetIsDashing() && !mIsFireAttacking && !mIsOnMovingGround && !mIsOnGround && !mIsHooking) {
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
    }

    ResolveEnemyCollision();
    ResolveGroundCollision();

    if (mIsOnGround) {
        mIsGoingRight = false;
        mIsGoingLeft = false;
        mTimerOutOfGroundToJump = 0;
    }
    else {
        mTimerOutOfGroundToJump += deltaTime;
    }

    if (mIsWallSliding) {
        mTimerOutOfWallToJump = 0;
        mIsDiving = false;
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
        if (mDrawRopeComponent) {
            // Start no Player (que pode estar caindo), End na ponta viajante
            mDrawRopeComponent->SetEndpoints(GetPosition(), mCurrentRopeTip);
        }
    }
    if (mIsHooking) {
        if (mHookingTimer < mHookingDuration) {
            mHookPoint->SetHookPointState(HookPoint::HookPointState::Hooked);
            mRigidBodyComponent->SetVelocity(mHookDirection * mHookSpeed);
            mHookingTimer += deltaTime;

            // Garante que a corda fique desenhada esticada até o fim
            if (mDrawRopeComponent) {
                mDrawRopeComponent->SetEndpoints(GetPosition(), mHookEnd);
            }
        } else {
            mIsHooking = false;
            mIsHookAnimating = false; // Para de desenhar
            mHookPoint = nullptr;

            if (mDrawRopeComponent) {
                mDrawRopeComponent->SetVisible(false);
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
    }

    if (mWasOnGround == false) {
        if (mIsOnGround) {
            mGame->GetAudio()->PlaySound("FallOnGround.wav");
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
    if (GetPosition().y > 20000 * mGame->GetScale()) {
        // SetPosition(mStartingPosition);
    }

    if (Died()) {
        mGame->SetGamePlayState(Game::GamePlayState::GameOver);
        mAABBComponent->SetActive(false);
        mRigidBodyComponent->SetVelocity(Vector2::Zero);
        mKnockBackTimer = mKnockBackDuration;
        mInvulnerableTimer = mInvulnerableDuration;
        mIsHealing = false;
        if (mRadialMenu) {
            mRadialMenu->Close();
            mRadialMenu = nullptr;
        }
        mIsHooking = false;
        mIsHookThrowing = false;
        mHookAnimProgress = 1.0f;
        mIsHookAnimating = false;
        mHookPoint = nullptr;
        if (mDrawRopeComponent) {
            mDrawRopeComponent->SetVisible(false);
        }
        // mGame->GetAudio()->StopAllSounds();
        mGame->GetAudio()->StopSound(mGame->GetMusicHandle());
        mGame->GetAudio()->StopSound(mGame->GetBossMusicHandle());
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
                        mIsDiving = false;
                        GroundSlamImpact();
                        GroundSlamEffects();
                        mGame->GetCamera()->StartCameraShake(mGroundSlamCameraShakeDuration, mGroundSlamCameraShakeStrength);
                        mIsGroundSlamRecovering = true;
                        mGroundSlamTimer = 0;
                        mRigidBodyComponent->SetVelocity(Vector2::Zero);
                        if (g->GetIsBreakable()) {
                            g->DestroyEffects();
                            delete g;
                            continue;
                        }
                    }

                    // Move o player junto ao ground em movimento
                    if (g->GetIsMoving()) {
                        mIsOnMovingGround = true;
                        mMovingGroundVelocity = g->GetComponent<RigidBodyComponent>()->GetVelocity();
                        if (!mDashComponent->GetIsDashing()) {
                            mRigidBodyComponent->SetVelocity(mMovingGroundVelocity);
                            SetPosition(Vector2(GetPosition().x,
                                                g->GetPosition().y - g->GetHeight() / 2 - mHeight / 2 + 1));
                            // Gambiarra (Ao detectar colisão por cima em moving ground, desce o ‘player’ 1 pixel para baixo para não ficar trepidando
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
                if (mCanWallSlide) {
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
                            mIsWallSliding = true;
                            mWallSlideSide = WallSlideSide::left;
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
                            mIsWallSliding = true;
                            mWallSlideSide = WallSlideSide::right;
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

                if (mSword->GetComponent<ColliderComponent>()->Intersect(*g->GetComponent<ColliderComponent>())) {
                    // Colisão da sword com grounds
                    if (!mSwordHitGround) {
                        collisionNormal = mSword->GetComponent<ColliderComponent>()->CollisionSide(*g->GetComponent<ColliderComponent>());
                        if ((collisionNormal == Vector2::NegUnitY && Math::Abs(mSword->GetForward().y) == 1) ||
                            (collisionNormal == Vector2::UnitY && Math::Abs(mSword->GetForward().y) == 1) ||
                            (collisionNormal == Vector2::NegUnitX && Math::Abs(mSword->GetForward().x) == 1) ||
                            (collisionNormal == Vector2::UnitX && Math::Abs(mSword->GetForward().x) == 1) )
                        {
                            auto* grass = new ParticleSystem(mGame, Particle::ParticleType::SolidParticle, 6, 150.0, 0.30, 0.05f);
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
                            SDL_Color color;
                            switch (mGame->GetGameScene()) {
                                case Game::GameScene::LevelTeste:
                                    color = {80, 148, 45, 255};
                                    break;

                                case Game::GameScene::Coliseu:
                                    color = {102, 114, 145, 255};
                                    break;

                                case Game::GameScene::Level1:
                                    color = {80, 148, 45, 255};
                                    break;

                                case Game::GameScene::Level2:
                                    color = {102, 114, 145, 255};
                                    break;

                                case Game::GameScene::Level3:
                                    color = {80, 148, 45, 255};
                                    break;

                                case Game::GameScene::Level4:
                                    color = {98, 171, 212, 255};
                                    break;

                                case Game::GameScene::Level5:
                                    color = {98, 171, 212, 255};
                                    break;

                                default:
                                    color = {102, 114, 145, 255};
                                    break;
                            }
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
                        mGame->GetCamera()->StartCameraShake(mGroundSlamCameraShakeDuration, mGroundSlamCameraShakeStrength);
                    }
                    mDashComponent->StopDash();

                    ReceiveHit(10, collisionNormal, DamageType::Environment);
                }
                if (mSword->GetComponent<ColliderComponent>()->Intersect(*g->GetComponent<ColliderComponent>())) { // Colisão da sword com spikes
                    if (!mSwordHitSpike) {
                        collisionNormal = mSword->GetComponent<ColliderComponent>()->CollisionSide(*g->GetComponent<ColliderComponent>());
                        if ((collisionNormal == Vector2::NegUnitY && Math::Abs(mSword->GetForward().y) == 1) ||
                            (collisionNormal == Vector2::UnitY && Math::Abs(mSword->GetForward().y) == 1) ||
                            (collisionNormal == Vector2::NegUnitX && Math::Abs(mSword->GetForward().x) == 1) ||
                            (collisionNormal == Vector2::UnitX && Math::Abs(mSword->GetForward().x) == 1) )
                        {
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
            if (mAABBComponent->Intersect(*e->GetComponent<ColliderComponent>())) {
                if (mDashComponent->GetIsDashing()) {
                    if (mIsLightningDashing) {
                        auto it = std::find(mEnemiesHitByCurrentDash.begin(), mEnemiesHitByCurrentDash.end(), e);
                        if (it == mEnemiesHitByCurrentDash.end()) {
                            e->ReceiveHit(mLightningDashDamage, GetForward(), false);
                            mEnemiesHitByCurrentDash.push_back(e);
                        }
                    }
                    else if (!mIsInvulnerable && mIFramesTimer <= 0 && !e->IsFrozen()) {
                        collisionNormal = mAABBComponent->ResolveCollision(*e->GetComponent<ColliderComponent>());
                        mDashComponent->StopDash();
                        ReceiveHit(e->GetContactDamage(), collisionNormal);
                    }
                }
                else {
                    if (mIsDiving) {
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
                    else if (!mIsInvulnerable && mIFramesTimer <= 0 && !e->IsFrozen()) {
                        collisionNormal = mAABBComponent->ResolveCollision(*e->GetComponent<ColliderComponent>());
                        ReceiveHit(e->GetContactDamage(), collisionNormal);
                    }
                }
            }

            if (mSword->GetComponent<ColliderComponent>()->Intersect(*e->GetComponent<ColliderComponent>())) { // Colisão da sword com enemies
                auto it = std::find(mEnemiesHitBySword.begin(), mEnemiesHitBySword.end(), e);
                if (it == mEnemiesHitBySword.end()) {
                    e->ReceiveHit(mSword->GetDamage(), mSword->GetForward());
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
                    mEnemiesHitBySword.push_back(e);
                }
            }
        }
    }
}

void Player::UseDash() {
    if (mCanDash) {
        if (!mIsFireAttacking && !mIsDiving) {
            if (mDashComponent->UseDash(mIsOnGround)) {
                if (mElementalMode == ElementalMode::Lightning && mMana >= mLightningDashManaCost) {
                    mIsLightningDashing = true;
                    mIFramesTimer = mLightningDashIFramesDuration;
                    mEnemiesHitByCurrentDash.clear();
                    mMana -= mLightningDashManaCost;
                }
                if (mIsOnGround) {
                    for (JumpEffect* j: mJumpEffects) {
                        if (j->GetState() == ActorState::Paused) {
                            j->SetState(ActorState::Active);
                            j->StartEffect(JumpEffect::EffectType::TakeOff);
                            break;
                        }
                    }
                }
            }
            mIsHooking = false;
            mIsHookThrowing = false;
            mHookAnimProgress = 1.0f;
            mIsHookAnimating = false;
            mHookPoint = nullptr;
            if (mDrawRopeComponent) {
                mDrawRopeComponent->SetVisible(false);
            }
        }
    }
}

void Player::UseGroundSlam() {
    if (mElementalMode == ElementalMode::Earth) {
        if (!mPrevGroundSlamPressed &&
            !mIsGroundSlamStarting &&
            !mIsGroundSlamRecovering &&
            !mIsDiving &&
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
            mHookAnimProgress = 1.0f;
            mIsHookAnimating = false;
            mHookPoint = nullptr;
            if (mDrawRopeComponent) {
                mDrawRopeComponent->SetVisible(false);
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
    auto* smoke = new ParticleSystem(mGame, Particle::ParticleType::BlurParticle, 80.0f, 25.0f, 0.35f, 0.3f);
    smoke->SetParticleColor(SDL_Color{130, 130, 130, 80});
    smoke->SetConeSpread(360.0f);
    smoke->SetParticleSpeedScale(0.2f);
    smoke->SetParticleGravity(false);
    smoke->SetEmitDirection(Vector2::Zero);
    smoke->SetPosition(GetPosition() + Vector2(0, mHeight * 0.5f));
    smoke->SetGroundCollision(false);

    // auto* miniGrounds = new ParticleSystem(mGame, Particle::ParticleType::SolidParticle, 10, 400.0, 3.0, 0.07f);
    // miniGrounds->SetPosition(GetPosition() + Vector2(0, mHeight * 0.5f));
    // miniGrounds->SetEmitDirection(Vector2::NegUnitY);
    // miniGrounds->SetParticleSpeedScale(0.6);
    // miniGrounds->SetParticleColor(SDL_Color{55, 27, 7, 255});
    // miniGrounds->SetParticleGravity(true);
    // miniGrounds->SetConeSpread(180.0f);
}

void Player::UseJump() {
    //Início do pulo
    if (!mIsFireAttacking) {
        if (!mDashComponent->GetIsDashing() && !mIsDiving) {
            // Pulo do chao
            if ((mTimerOutOfGroundToJump < mMaxTimeOutOfGroundToJump || mIsOnSpike) && !mIsJumping && mCanJump && (mWallJumpTimer >= mWallJumpMaxTime)) {
                mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x, mJumpForce)
                                                 + mMovingGroundVelocity);
                mIsJumping = true;
                mCanJump = false;
                mJumpTimer = 0.0f;
                mTimerOutOfGroundToJump = mMaxTimeOutOfGroundToJump;
                mGame->GetAudio()->PlaySound("Jump/Jump1.wav");
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
            }
            // Pulo no ar
            if (!(mIsOnGround || mIsWallSliding) && mJumpCountInAir < mMaxJumpsInAir && mCanJump
                && (mWallJumpTimer >= mWallJumpMaxTime)) {
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
    if (!mPrevSwordPressed && mSwordCooldownTimer >= mSwordCooldownDuration &&
        !mDashComponent->GetIsDashing() && !mIsDiving)
    {
        mGame->GetAudio()->PlayVariantSound("SwordSlash/SwordSlash.wav", 11);
        // Ativa a espada
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
    if (mCanFireBall) {
        if (!mPrevFireBallPressed &&
            mFireBallCooldownTimer >= mFireBallCooldownDuration &&
            mMana >= mFireballManaCost &&
            !mDashComponent->GetIsDashing() && !mIsDiving)
        {
            std::vector<FireBall* > fireBalls = mGame->GetFireBalls();
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

void Player::UseFreeze(bool up, bool down) {
    if (mCanFreeze) {
        if (mMana >= mFreezeManaCost && !mDashComponent->GetIsDashing() && !mIsDiving) {
            AttachedEffect freezeEffect;
            if (mIntervalBetweenFreezeEmitTimer >= mIntervalBetweenFreezeEmitDuration) {
                mIsFreezingDown = false;
                mIsFreezingUp = false;
                mIsFreezingFront = false;

                auto* snowBalls = new ParticleSystem(mGame, Particle::ParticleType::BlurParticle, 13.0f, 100.0f, 0.45f, 0.15f);
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

                auto* iceCloud = new ParticleSystem(mGame, Particle::ParticleType::BlurParticle, 80.0f, 60.0f, 0.55f, 0.2f);
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
    if (mCanCreatePillar) {
        if (!mPrevCreatePillarPressed &&
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
    if (mHealCount > 0 && mHealthPoints < mMaxHealthPoints && mIsOnGround) {
        mIsHealing = true;
        if (mHealAnimationTimer >= mHealAnimationDuration) {
            mHealAnimationTimer = 0;
            mHealthPoints += mHealAmount;
            mHealCount--;
            if (mHealthPoints > mMaxHealthPoints) {
                mHealthPoints = mMaxHealthPoints;
            }
        }
    }
    else {
        mIsHealing = false;
        mHealAnimationTimer = 0;
    }
}

void Player::UseHook(HookPoint* nearestHookPoint) {
    if (mCanHook) {
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
            mHookAnimProgress = 0.0f; // Reseta a ondulação da corda

            if (mDrawRopeComponent) {
                mDrawRopeComponent->SetVisible(true);
                mDrawRopeComponent->SetEndpoints(GetPosition(), mCurrentRopeTip);
            }

            // Resetar dash no ar
            mDashComponent->SetHasDashedInAir(false);
            // RESET DO CONTADOR DE PULO
            mJumpCountInAir = 0;
        }
    }
}

void Player::SetElementalMode(ElementalMode mode) {
    mElementalMode = mode;
    if (mElementalMode == ElementalMode::Lightning) {
        mDashComponent->SetDashSpeed(mLightningDashSpeed);
        mDashComponent->SetDashDuration(mLightningDashDuration);
        mDashComponent->SetDashCooldown(mLightningDashCooldown);
    }
    else {
        mDashComponent->SetDashSpeed(mDashSpeed);
        mDashComponent->SetDashDuration(mDashDuration);
        mDashComponent->SetDashCooldown(mDashCooldown);
    }
}


void Player::OpenElementalMenu() {
    if (!mRadialMenu) {
        mRadialMenu = new RadialMenu(mGame, "../Assets/Fonts/K2D-Bold.ttf", 250.0f);

        // Adiciona os elementos
        mRadialMenu->AddRadialOption("FOGO", [this]() { SetElementalMode(ElementalMode::Fire); });
        mRadialMenu->AddRadialOption("GELO", [this]() { SetElementalMode(ElementalMode::Ice); });
        mRadialMenu->AddRadialOption("TERRA", [this]() { SetElementalMode(ElementalMode::Earth); });
        mRadialMenu->AddRadialOption("RAIO", [this]() { SetElementalMode(ElementalMode::Lightning); });

        mRadialMenu->SetSelectedOption(static_cast<int>(mElementalMode));
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
        mDrawComponent->SetAnimation("wallSlide");
    }
    else if (mIsRunning && mIsOnGround) {
        mDrawComponent->SetAnimation("run");
    }
    else if (!mIsOnGround) {
        if (mRigidBodyComponent->GetVelocity().y < -200 * mGame->GetScale()) {
            mDrawComponent->SetAnimation("jumpUp");
        }
        if (mRigidBodyComponent->GetVelocity().y > 200 * mGame->GetScale()) {
            mDrawComponent->SetAnimation("falling");
        }
        if (mRigidBodyComponent->GetVelocity().y > -200 * mGame->GetScale() &&
            mRigidBodyComponent->GetVelocity().y < 200 * mGame->GetScale())
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
        mIsInvulnerable = true;
        mHurtTimer = 0;

        Vector2 vel = mRigidBodyComponent->GetVelocity();
        if (vel.Length() > 0) {
            vel.Normalize();
        }

        Vector2 knockBack = knockBackDirection * mKnockBackSpeed + vel * (mKnockBackSpeed / 3);
        knockBack.Normalize();
        mRigidBodyComponent->SetVelocity(knockBack * mKnockBackSpeed);
        // mRigidBodyComponent->SetVelocity(knockBackDirection * mKnockBackSpeed + vel * (mKnockBackSpeed / 3));
        mKnockBackTimer = 0;
        mInvulnerableTimer = 0;
        mGame->ActiveHitStop();
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

void Player::SetIsEnteringLevel(Vector2 velocity) {
    mIsEnteringLevel = true;
    mEnteringLevelTimer = 0;

    mRigidBodyComponent->SetVelocity(velocity);
    if (velocity.x > 0) {
        mIsRunning = true;
        SetRotation(0);
        SetScale(Vector2(1, 1));
    }
    if (velocity.x < 0) {
        mIsRunning = true;
        SetRotation(Math::Pi);
        SetScale(Vector2(-1, 1));
    }
}

void Player::ChangeResolution(float oldScale, float newScale) {
    mWidth = mWidth / oldScale * newScale;
    mHeight = mHeight / oldScale * newScale;
    SetPosition(Vector2(GetPosition().x / oldScale * newScale, GetPosition().y / oldScale * newScale));
    SetStartingPosition(Vector2(mStartingPosition.x / oldScale * newScale, mStartingPosition.y / oldScale * newScale));
    mMoveSpeed = mMoveSpeed / oldScale * newScale;
    mJumpForce = mJumpForce / oldScale * newScale;
    mLowGravity = mLowGravity / oldScale * newScale;
    mMediumGravity = mMediumGravity / oldScale * newScale;
    mHighGravity = mHighGravity / oldScale * newScale;
    mSwordWidth = mSwordWidth / oldScale * newScale;
    mSwordHeight = mSwordHeight / oldScale * newScale;
    mFireballRecoil = mFireballRecoil / oldScale * newScale;
    mFireballWidth = mFireballWidth / oldScale * newScale;
    mFireBallHeight = mFireBallHeight / oldScale * newScale;
    mFireballSpeed = mFireballSpeed / oldScale * newScale;
    mWallSlideSpeed = mWallSlideSpeed / oldScale * newScale;
    mKnockBackSpeed = mKnockBackSpeed / oldScale * newScale;
    mCameraShakeStrength = mCameraShakeStrength / oldScale * newScale;
    mHookSpeed = mHookSpeed / oldScale * newScale;
    mHookAmplitude = mHookAmplitude / oldScale * newScale;
    mHookSegmentHeight = mHookSegmentHeight / oldScale * newScale;

    mHookEnd.x = mHookEnd.x / oldScale * newScale;
    mHookEnd.y = mHookEnd.y / oldScale * newScale;

    // if (mDrawRopeComponent) {
    //     mDrawRopeComponent->SetNumSegments(mHookSegments);
    //     mDrawRopeComponent->SetAmplitude(mHookAmplitude);
    //     mDrawRopeComponent->SetSegmentHeight(mHookSegmentHeight);
    // }

    mRigidBodyComponent->SetMaxSpeedX(mRigidBodyComponent->GetMaxSpeedX() / oldScale * newScale);
    mRigidBodyComponent->SetMaxSpeedY(mRigidBodyComponent->GetMaxSpeedY() / oldScale * newScale);
    mDashComponent->SetDashSpeed(mDashComponent->GetDashSpeed() / oldScale * newScale);

    mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x / oldScale * newScale, mRigidBodyComponent->GetVelocity().y / oldScale * newScale));

    // if (mDrawAnimatedComponent) {
    //     mDrawAnimatedComponent->SetWidth(mWidth * 4.93f);
    //     mDrawAnimatedComponent->SetHeight(mWidth * 4.93f * 1.11f);
    // }

    Vector2 v1(-mWidth / 2, -mHeight / 2);
    Vector2 v2(mWidth / 2, -mHeight / 2);
    Vector2 v3(mWidth / 2, mHeight / 2);
    Vector2 v4(-mWidth / 2, mHeight / 2);

    std::vector<Vector2> vertices;
    vertices.emplace_back(v1);
    vertices.emplace_back(v2);
    vertices.emplace_back(v3);
    vertices.emplace_back(v4);

    if (auto* aabb = dynamic_cast<AABBComponent*>(mAABBComponent)) {
        aabb->SetMin(v1);
        aabb->SetMax(v3);
    }

    // if (mDrawPolygonComponent) {
    //     mDrawPolygonComponent->SetVertices(vertices);
    // }
}
