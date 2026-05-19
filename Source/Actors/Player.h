//
// Created by roger on 22/04/2025.
//

#pragma once

#include "Actor.h"
#include "HookPoint.h"
#include "../Actors/JumpEffect.h"
#include "../Actors/Sword.h"
#include "../AudioSystem.h"
#include "../PlayerSkillManager.h"

class ParticleSystem;

class Player : public Actor
{
public:
    enum class ElementalMode {
        Fire,
        Lightning,
        Earth,
        Ice
    };
    enum class WallSlideSide { notSliding, left, right };
    enum class EffectDir { Front, Up, Down };

    struct AttachedEffect {
        ParticleSystem* system;
        EffectDir direction;
    };

    Player(Game* game);

    void OnProcessInput(const Uint8* keyState, SDL_GameController &controller) override;
    void OnUpdate(float deltaTime) override;

    float GetWidth() override { return mWidth; }
    float GetHeight() override { return mHeight; }

    void SetJumpEffects();

    void SetElementalMode(ElementalMode mode);
    ElementalMode GetElementalMode() { return mElementalMode; }

    class PlayerSkillManager* GetSkillManager() const { return mSkillManager; }

    bool GetIsOnGround() const { return mIsOnGround; }

    void SetStartingPosition(Vector2 pos) { mStartingPosition = pos; }
    Vector2 GetStartingPosition() const { return mStartingPosition; }

    void ReceiveHit(float damage, Vector2 knockBackDirection, DamageType damageType = DamageType::Normal);

    class Sword* GetSword() const { return mSword; }
    void SetSword() { mSword = new Sword(mGame, this, mSwordWidth, mSwordHeight, mSwordDuration, mSwordDamage); }
    void SetSwordWidth(float width) { mSwordWidth = width; }
    float GetSwordWidth() const { return mSwordWidth; }
    void SetSwordHeight(float height) { mSwordHeight = height; }
    float GetSwordHeight() const { return mSwordHeight; }
    void SetSwordDamage(float damage) { mSwordDamage = damage; }

    bool IsLightningDashing() { return mIsLightningDashing; }

    void Stop();

    void ResetCooldown();


    void ResetHealthPoints() { mHealthPoints = mBaseMaxHealthPoints * mSkillManager->GetMaxHealthPointsMultiplier(); }
    float GetHealthPoints() const { return mHealthPoints; }
    void SetHealthPoints(float HP) { mHealthPoints = HP; }
    float GetMaxHealthPoints() const { return mBaseMaxHealthPoints * mSkillManager->GetMaxHealthPointsMultiplier(); }
    void SetMaxHealthPoints(float maxHealthPoints) { mBaseMaxHealthPoints = maxHealthPoints; }
    void SetIsInvulnerable(bool isInvulnerable) { mIsInvulnerable = isInvulnerable; }
    void SetInvulnerableTimer(float invulnerableTimer) { mInvulnerableTimer = invulnerableTimer; }

    void ResetMana() { mMana = mBaseMaxMana * mSkillManager->GetMaxManaMultiplier(); }
    float GetMana() const { return mMana; }
    float GetMaxMana() const { return mBaseMaxMana * mSkillManager->GetMaxManaMultiplier(); }
    void SetMana(float mana) { mMana = mana; }
    void SetMaxMana(float maxMana) { mBaseMaxMana = maxMana; }
    float GetFireballManaCost() const { return mFireballManaCost; }

    void ResetHealCount() { mHealCount = mMaxHealCount + mSkillManager->GetHealCountIncrease(); }
    int GetHealCount() const { return mHealCount; }
    void IncreaseHealCount() { mHealCount++; }

    void IncreaseMoney(int value) { mMoney += value; }
    void DecreaseMoney(int value) { mMoney -= value; }
    int GetMoney() const { return mMoney; }
    void SetMoney(int money) { mMoney = money; }
    int GetSartingMoney() { return mStartMoney; }

    void IncreaseStone(std::string stoneType, int value);
    void DecreaseStone(std::string stoneType, int value);
    int GetStone(std::string stoneType);
    void SetStone(std::string stoneType, int value);

    float GetRadialMenuSlowMotionDuration() const { return mRadialMenuSlowMotionDuration; }
    float GetRadialMenuSlowMotionTimer() const { return mRadialMenuSlowMotionTimer; }
    bool IsSlowMotionCharging() const { return mRadialMenuSlowMotionCharging; }

    void AdjustSwordAttackSpeed(float factor) { mSwordCooldownDuration /= factor; }

    void SetFireballWidth(float width) { mFireballWidth = width; }
    float GetFireballWidth() const { return mFireballWidth; }
    void SetFireballHeight(float height) { mFireBallHeight = height; }
    float GetFireballHeight() const { return mFireBallHeight; }
    void SetFireballDamage(float damage) { mFireballDamage = damage; }
    float GetFireballDamage() const { return mFireballDamage; }

    void SetDeathCounter(int count) { mDeathCounter = count; }
    int GetDeathCounter() const { return mDeathCounter; }
    void SetIsDead(bool isDead) { mIsDead = isDead; }

    // funções para gerenciar a animação em cutscenes
    void SetIsRunning(bool isRunning) {mIsRunning = isRunning; }

    // Funções para sair da pausa com o controle sem pular ou atirar bola de fogo
    void SetCanJump(bool j) { mCanJump = j; }
    void SetPrevSkill1Pressed(bool f) { mPrevSkill1Pressed = f; }
    void SetPrevSwordPressed(bool s) { mPrevSwordPressed = s; }

    void SetIsEnteringLevel(Vector2 velocity);

    void SetInvertControls(bool invert) { mInvertControls = invert; }
    bool GetIsInvertControls() { return mInvertControls; }

    bool Died();

    void InitLight();

private:
    enum class GrassEffectType {
        Run,
        Jump,
        Land,
        WallJump,
        Dash
    };

    void ResolveGroundCollision();
    void ResolveEnemyCollision();

    void UseDash();
    void UseFrenzyMode();
    void StopFrenzyMode();
    void UseLightningSpear();
    void UseGroundSlam();
    void GroundSlamImpact();
    void GroundSlamEffects();
    void Glide();
    void UseJump();
    void UseSword();
    void UseFireBall();
    void UseFireWisp();
    void UseFreeze(bool up, bool down);
    void UsePillar();
    void UseHeal();
    void UseHook(HookPoint* nearestHookPoint);

    void StartGrassEffect(GrassEffectType type);

    void OpenElementalMenu();

    void FireTransformationEffect();
    void LightningTransformationEffect();
    void IceTransformationEffect();
    void EarthTransformationEffect();

    void ManageCombatBoxes(float deltaTime);
    void ManageAnimations();

    Vector2 mStartingPosition;

    ElementalMode mElementalMode;
    float mWidth;
    float mHeight;

    class PlayerSkillManager* mSkillManager;

    bool mIsOnGround;
    bool mIsOnSpike;
    bool mIsOnMovingGround;
    Vector2 mMovingGroundVelocity;
    float mMoveSpeed;    // Velocidade de movimento
    float mMaxSpeedYNormal;

    float mMaxTimeOutOfGroundToJump;  // Variáveis para dar um intervalo para pular quando sair do chão ou parede
    float mTimerOutOfGroundToJump;
    float mMaxTimeOutOfWallToJump;
    float mTimerOutOfWallToJump;

    bool mIsGliding;
    float mMinGlideDuration;
    float mGlideTimer;
    float mGlideCooldownDuration;
    float mGlideCooldownTimer;
    float mGlideInitialSpeedY;
    float mMaxSpeedYGlide;
    float mGlideGravity;
    float mGlideManaCost;
    bool mIsGlideManaOver;
    class AirGlideEffect* mGlideEffect;

    bool mIsJumping;     // Está no meio de um pulo sustentado?
    float mJumpTimer;    // Quanto tempo já pulou
    float mMaxJumpTime;  // Tempo máximo de pulo sustentado
    float mJumpForce;    // Força contínua durante o pulo
    bool mCanJump;       // Usado para não continuar pulando ao segurar botão de pular
    int mJumpCountInAir; // Numero de pulos realizados no ar
    float mLowGravity;
    float mMediumGravity;
    float mHighGravity;
    std::vector<JumpEffect*> mJumpEffects;

    float mDashSpeed;
    float mDashDuration;
    float mDashCooldown;

    bool mIsLightningDashing;
    float mLightningDashSpeed;
    float mLightningDashDuration;
    float mLightningDashCooldown;
    float mLightningDashDamage;
    float mLightningDashManaCost;
    float mLightningDashIFramesDuration;
    std::vector<class Enemy*> mEnemiesHitByCurrentDash;
    class LightningEffect* mLightningDashEffect;
    Vector2 mStartLightningDashPosition;

    bool mIsOnFrenzyMode;
    float mFrenzyModeDuration;
    float mFrenzyModeTimer;
    float mNormalSpeed;
    float mFrenzyModeSpeed;
    class LightningEffect* mFrenzyAuraEffect;
    float mFrenzyAuraTimer;
    float mFrenzyModeManaCost;

    float mLightningSpearCooldownDuration;
    float mLightningSpearCooldownTimer;
    float mLightningSpearManaCost;
    Vector2 mLightningSpearStartOffsetPosition;
    class LightningSpear* mLightningSpear;

    bool mPrevSkill1Pressed;
    bool mPrevSkill2Pressed;

    bool mIsGroundSlamStarting;
    bool mIsGroundSlamRecovering;
    bool mIsDiving;
    float mGroundSlamStartDuration;
    float mGroundSlamRecoveryDuration;
    float mGroundSlamTimer;
    float mGroundSlamSpeed;
    float mMaxSpeedYGroundSlam;
    float mGroundSlamDamage;
    float mGroundSlamImpactDist;
    float mGroundSlamImpactHeightRange;
    float mGroundSlamImpactDamage;
    float mGroundSlamIFramesDuration;
    float mGroundSlamCameraShakeStrength;
    float mGroundSlamCameraShakeDuration;
    float mGroundSlamManaCost;
    class DashEffect* mDiveEffect;
    class GroundSlamImpactEffect* mGroundSlamImpactEffect;
    std::vector<class Enemy*> mEnemiesHitByGroundSlam;

    Sword* mSword;
    bool mPrevSwordPressed;             // Se apertou botão de espada no último frame
    float mSwordCooldownTimer;          // Timer de cooldown da espada
    float mSwordCooldownDuration;       // Cooldown da espada
    float mSwordWidth;
    float mSwordHeight;
    float mSwordDuration;
    float mSwordDamage;
    float mSwordDirection;              // Director da espada(esquerda, direita, cima, baixo)
    bool mSwordHitGround;
    bool mSwordHitSpike;
    float mSwordHitKnockBack;
    std::vector<class Enemy*> mEnemiesHitBySword;

    float mFireBallCooldownTimer; // Timer de cooldown da fireball
    const float mFireBallCooldownDuration; // Cooldown da fireball
    bool mIsFireAttacking; // As seguintes variáveis são para a feature de ficar parado ao atirar e dar um recoil
    float mStopInAirFireBallTimer; // Timer que fica parado no ar
    const float mStopInAirFireBallMaxDuration; // Cooldown do tempo no ar
    float mFireballRecoil; // Recoil do tiro
    float mFireballWidth;
    float mFireBallHeight;
    float mFireballSpeed;
    float mFireballDamage;
    float mBaseMaxMana;
    float mMana;
    float mManaIncreaseRate;
    float mFireballManaCost;
    float mFireballAnimationDuration;
    float mFireballAnimationTimer;

    float mFireWispCooldownDuration;
    float mFireWispCooldownTimer;
    float mFireWispManaCost;

    bool mIsFreezingFront;
    bool mIsFreezingUp;
    bool mIsFreezingDown;
    std::vector<AttachedEffect> mSnowBallsParticleSystems;
    std::vector<AttachedEffect> mIceCloudParticleSystems;
    float mIntervalBetweenFreezeEmitDuration;
    float mIntervalBetweenFreezeEmitTimer;
    float mFreezeManaCost;

    float mPillarDistanceFromPlayer;
    float mPillarManaCost;
    float mPillarAnimationDuration;
    float mPillarAnimationTimer;
    bool mAlreadyCreatedPillar;

    bool mIsWallSliding;                 // Se esta deslizando
    WallSlideSide mWallSlideSide;        // Lado que esta deslizando
    float mWallSlideSpeed;               // Velocidade que desce deslizando
    int mTryingLeavingWallSlideLeft;     // Variáveis para quando tentar sair do wall sliding,
    int mTryingLeavingWallSlideRight;    //
    float mTimerToLeaveWallSlidingLeft;  //
    float mTimerToLeaveWallSlidingRight; //
    float mMaxTimerToLiveWallSliding;    // ter um tempo para segurar a setinha para se desgrudar da parede

    bool mIsGoingRight;
    bool mIsGoingLeft;

    float mWallJumpTimer;   // Timer enquanto está pulando de uma parede
    float mWallJumpMaxTime; // Tempo maximo que fica pulando de uma parede

    float mKnockBackSpeed;
    float mKnockBackTimer;
    float mKnockBackDuration;
    float mCameraShakeStrength;

    float mBaseMaxHealthPoints;
    float mHealthPoints;
    bool mIsInvulnerable;
    float mInvulnerableDuration;
    float mInvulnerableTimer;
    int mMaxHealCount;
    int mHealCount;
    float mHealAmount;
    bool mIsHealing;
    float mHealAnimationDuration;
    float mHealAnimationTimer;

    float mIFramesTimer;

    int mMoney;
    int mStartMoney;
    int mEarthStone;
    int mFireStone;
    int mIceStone;
    int mLightningStone;

    class VineRope* mVineRope;
    float mVineRopeThickness;
    bool mIsHooking;
    bool mPrevHookPressed;
    Vector2 mHookDirection;
    float mHookSpeed;
    float mHookCooldownDuration;
    float mHookCooldownTimer;
    float mHookingDuration;
    float mHookingTimer;
    Vector2 mHookEnd;
    bool mIsHookAnimating;
    HookPoint* mHookPoint;

    bool mIsHookThrowing;      // Se a corda está viajando pelo ar
    Vector2 mCurrentRopeTip;   // Onde está a ponta da corda agora
    float mRopeThrowSpeed;     // Velocidade visual da corda (deve ser rápida)

    class RadialMenu* mRadialMenu;
    float mRadialMenuSlowMotionDuration;
    float mRadialMenuSlowMotionTimer;
    bool mRadialMenuSlowMotionCharging;
    float mRadialMenuSlowMotionChangingRate;

    // Atributos para animar sprites
    bool mIsRunning;
    float mHurtDuration;
    float mHurtTimer;
    bool mBlink;
    float mBlinkDuration;
    float mBlinkTimer;

    // Efeitos de grama ao correr
    float mRunningGrassParticleIntervalDuration;
    float mRunningGrassParticleIntervalTimer;

    // Atributos para sounds
    float mRunningSoundIntervalDuration;
    float mRunningSoundIntervalTimer;

    int mDeathCounter;

    bool mIsEnteringLevel;
    float mEnteringLevelDuration;
    float mEnteringLevelTimer;

    float mDeathAnimationDuration;
    float mDeathAnimationTimer;
    bool mIsDead;

    bool mWasOnGround;

    bool mInvertControls;
    float mConfusionTime;

    class Light* mLight;

    class RectComponent* mRectComponent;
    class AnimatorComponent* mDrawComponent;
    class RigidBodyComponent* mRigidBodyComponent;
    class ColliderComponent* mAABBComponent;
    class CombatBoxComponent* mCombatBoxComponent;
    class DashComponent* mDashComponent;
    class GhostTrailComponent* mGhostTrailComponent;
};
