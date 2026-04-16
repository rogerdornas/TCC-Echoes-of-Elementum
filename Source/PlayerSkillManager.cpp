//
// Created by roger on 10/04/2026.
//

#include "PlayerSkillManager.h"

#include "Game.h"
#include "HUD.h"
#include "Actors/Player.h"

PlayerSkillManager::PlayerSkillManager(class Player *player)
    :mPlayer(player)
    ,mHasEarthMode(false)
    ,mHasFireMode(false)
    ,mHasIceMode(false)
    ,mHasLightningMode(false)
    ,mMaxJumpsInAir(0)
    ,mCanDash(false)
    ,mCanWallSlide(false)
    ,mCanHook(false)
    ,mCanGroundSlam(false)
    ,mCanCreatePillar(false)
    ,mCanFireBall(false)
    ,mCanFireWisp(false)
    ,mCanFireWispAttack(false)
    ,mCanFreeze(false)
    ,mCanGlide(false)
    ,mCanLightningSpear(false)
    ,mCanLightningSpearChainShock(false)
    ,mCanFrenzyMode(false)

    ,mSwordDamageMultiplier(1.0f)
    ,mSwordRangeMultiplier(1.0f)
    ,mSwordAttackSpeedMultiplier(1.0f)
    ,mMaxHealthPointsMultiplier(1.0f)
    ,mMaxManaMultiplier(1.0f)
    ,mHealCountIncrease(0)
    ,mHealAmountMultiplier(1.0f)
    ,mFireBallDamageMultiplier(1.0f)
    ,mFireBallSizeMultiplier(1.0f)
    ,mLightningSpearDamageMultiplier(1.0f)
    ,mFrenzyModeDurationMultiplier(1.0f)
    ,mLightningDashDamageMultiplier(1.0f)
    ,mFreezeDurationMultiplier(1.0f)
{
}

void PlayerSkillManager::ApplySkillEffect(const std::string &effectType, const std::string &effectValue) {
    if (effectType == "unlock_mechanic") {
        UnlockMechanic(effectValue);
    }
    else if (effectType == "unlock_element") {
        UnlockElement(effectValue);
    }
    else if (effectType == "stat_boost") {
        ApplyStatModifier(effectValue);
    }
}

void PlayerSkillManager::UnlockMechanic(const std::string &mechanicId) {
    if (mechanicId == "double_jump") {
        mMaxJumpsInAir = 1;
    }
    else if (mechanicId == "dash") {
        mCanDash = true;
    }
    else if (mechanicId == "wall_slide") {
        mCanWallSlide = true;
    }
    else if (mechanicId == "hook") {
        mCanHook = true;
    }
    else if (mechanicId == "ground_slam") {
        mCanGroundSlam = true;
    }
    else if (mechanicId == "create_pillar") {
        mCanCreatePillar = true;
    }
    else if (mechanicId == "fire_ball") {
        mCanFireBall = true;
    }
    else if (mechanicId == "fire_wisp") {
        mCanFireWisp = true;
    }
    else if (mechanicId == "fire_wisp_attack") {
        mCanFireWispAttack = true;
    }
    else if (mechanicId == "freeze") {
        mCanFreeze = true;
    }
    else if (mechanicId == "glide") {
        mCanGlide = true;
    }
    else if (mechanicId == "lightning_spear") {
        mCanLightningSpear = true;
    }
    else if (mechanicId == "lightning_spear_chain_shock") {
        mCanLightningSpearChainShock = true;
    }
    else if (mechanicId == "frenzy_mode") {
        mCanFrenzyMode = true;
    }
}

void PlayerSkillManager::UnlockElement(const std::string &elementId) {
    if (elementId == "earth") {
        mHasEarthMode = true;
    }
    else if (elementId == "fire") {
        mHasFireMode = true;
    }
    else if (elementId == "ice") {
        mHasIceMode = true;
    }
    else if (elementId == "lightning") {
        mHasLightningMode = true;
    }
}

void PlayerSkillManager::ApplyStatModifier(const std::string &statData) {
    size_t delimiterPos = statData.find(':');
    if (delimiterPos == std::string::npos) return;

    std::string statType = statData.substr(0, delimiterPos);
    float value = std::stof(statData.substr(delimiterPos + 1));

    if (statType == "sword_damage") {
        mSwordDamageMultiplier += value;
    }
    if (statType == "sword_attack_speed") {
        mSwordAttackSpeedMultiplier -= value;
    }
    if (statType == "sword_range") {
        mSwordRangeMultiplier += value;
        mPlayer->GetSword()->SetWidth(mPlayer->GetSwordWidth() * mSwordRangeMultiplier);
        mPlayer->GetSword()->SetHeight(mPlayer->GetSwordHeight() * mSwordRangeMultiplier);
    }
    if (statType == "fire_ball_damage") {
        mFireBallDamageMultiplier += value;
        mFireBallSizeMultiplier += value;
    }
    if (statType == "lightning_spear_damage") {
        mLightningSpearDamageMultiplier += value;
    }
    if (statType == "frenzy_mode_duration") {
        mFrenzyModeDurationMultiplier += value;
    }
    if (statType == "lightning_dash_damage") {
        mLightningDashDamageMultiplier += value;
    }
    if (statType == "freeze_duration") {
        mFreezeDurationMultiplier += value;
    }
    else if (statType == "max_health_points") {
        mMaxHealthPointsMultiplier += value;
        mPlayer->ResetHealthPoints();
        if (auto* hud = mPlayer->GetGame()->GetHUD()) {
            hud->IncreaseHPBar();
        }
    }
    else if (statType == "mana_max") {
        mMaxManaMultiplier += value;
        mPlayer->ResetMana();
        if (auto* hud = mPlayer->GetGame()->GetHUD()) {
            hud->IncreaseManaBar();
        }
    }
    else if (statType == "heal_count") {
        mHealCountIncrease += static_cast<int>(value);
        mPlayer->IncreaseHealCount();
    }
    else if (statType == "heal_amount") {
        mHealAmountMultiplier += value;
    }
}
