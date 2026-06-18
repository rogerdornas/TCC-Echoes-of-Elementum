//
// Created by roger on 10/04/2026.
//

#pragma once

#include <string>

class PlayerSkillManager
{
public:
    PlayerSkillManager(class Player* player);

    void ApplySkillEffect(const std::string& effectType, const std::string& effectValue);

    void UnlockMechanic(const std::string& mechanicId);

    void UnlockElement(const std::string& elementId);

    // Getters
    bool HasEarthMode() const { return mHasEarthMode; }
    bool HasFireMode() const { return mHasFireMode; }
    bool HasIceMode() const { return mHasIceMode; }
    bool HasLightningMode() const { return mHasLightningMode; }

    int MaxJumpsInAir() const { return mMaxJumpsInAir; }
    bool CanDash() const { return mCanDash; }
    bool CanWallSlide() const { return mCanWallSlide; }
    bool CanHook() const { return mCanHook; }
    bool CanGroundSlam() const { return mCanGroundSlam; }
    bool CanCreatePillar() const { return mCanCreatePillar; }
    bool CanFireBall() const { return mCanFireBall; }
    bool CanFireWisp() const { return mCanFireWisp; }
    bool CanFireWispAttack() const { return mCanFireWispAttack; }
    bool CanFreeze() const { return mCanFreeze; }
    bool CanGlide() const { return mCanGlide; }
    bool CanLightningSpear() const { return mCanLightningSpear; }
    bool CanLightningSpearChainShock() const { return mCanLightningSpearChainShock; }
    bool CanFrenzyMode() const { return mCanFrenzyMode; }

    float GetSwordDamageMultiplier() const { return mSwordDamageMultiplier; }
    float GetSwordRangeMultiplier() const { return mSwordRangeMultiplier; }
    float GetSwordAttackSpeedMultiplier() const { return mSwordAttackSpeedMultiplier; }
    float GetMaxHealthPointsMultiplier() const { return mMaxHealthPointsMultiplier; }
    float GetMaxManaMultiplier() const { return mMaxManaMultiplier; }
    int GetHealCountIncrease() const { return mHealCountIncrease; }
    float GetHealAmountMultiplier() const { return mHealAmountMultiplier; }
    float GetFireBallDamageMultiplier() const { return mFireBallDamageMultiplier; }
    float GetFireBallSizeMultiplier() const { return mFireBallSizeMultiplier; }
    float GetLightningSpearDamageMultiplier() const { return mLightningSpearDamageMultiplier; }
    float GetFrenzyModeDurationMultiplier() const { return mFrenzyModeDurationMultiplier; }
    float GetLightningDashDamageMultiplier() const { return mLightningDashDamageMultiplier; }
    float GetFreezeDurationMultiplier() const { return mFreezeDurationMultiplier; }

private:
    void ApplyStatModifier(const std::string& statData);

    class Player* mPlayer;

    // Modos Elementais
    bool mHasEarthMode;
    bool mHasFireMode;
    bool mHasIceMode;
    bool mHasLightningMode;

    // Movimentação
    int mMaxJumpsInAir;
    bool mCanDash;
    bool mCanWallSlide;
    bool mCanHook;

    // Habilidades Elementais
    bool mCanGroundSlam;
    bool mCanCreatePillar;

    bool mCanFireBall;
    bool mCanFireWisp;
    bool mCanFireWispAttack;

    bool mCanFreeze;
    bool mCanGlide;

    bool mCanLightningSpear;
    bool mCanLightningSpearChainShock;
    bool mCanFrenzyMode;

    // Upgrades de Status
    float mSwordDamageMultiplier;
    float mSwordRangeMultiplier;
    float mSwordAttackSpeedMultiplier;
    float mMaxHealthPointsMultiplier;
    float mMaxManaMultiplier;
    int mHealCountIncrease;
    float mHealAmountMultiplier;

    float mFireBallDamageMultiplier;
    float mFireBallSizeMultiplier;
    float mLightningSpearDamageMultiplier;
    float mFrenzyModeDurationMultiplier;
    float mLightningDashDamageMultiplier;
    float mFreezeDurationMultiplier;
};
