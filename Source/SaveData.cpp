//
// Created by roger on 28/09/2025.
//

#include "SaveData.h"
#include "Json.h"
#include <fstream>
#include <iostream>

#include "PlayerSkillManager.h"

SaveData::SaveData(class Game *game)
    :mGame(game)
    ,mGameScenePath("Room2/Room2")
    ,mTotalPlayTime(0.0f)

    ,mMasterAudio(1.0f)
    ,mMusicAudio(1.0f)
    ,mSFXAudio(1.0f)

    ,mLastCheckpointPosition(Vector2(742, 1780))

    ,mHasEarthMode(false)
    ,mHasFireMode(false)
    ,mHasIceMode(false)
    ,mHasLightningMode(false)
    ,mCurrentElementalMode(Player::ElementalMode::Earth)

    ,mMoney(0)
    ,mDeathCounter(0)
    ,mMaxJumpsInAir(0)
    ,mCanDash(false)
    ,mCanWallSlide(false)
    ,mCanHook(false)
{
}

void SaveData::Save(const std::string &filename) {
    nlohmann::json j;

    j["game"]["scene"] = mGameScenePath;
    j["game"]["last_checkpoint"] = { {"x", mLastCheckpointPosition.x}, {"y", mLastCheckpointPosition.y} };
    j["game"]["total_play_time"] = mTotalPlayTime;

    j["configs"]["master_audio"] = mMasterAudio;
    j["configs"]["music_audio"] = mMusicAudio;
    j["configs"]["sfx_audio"] = mSFXAudio;

    j["world_state"] = mWorldState;

    j["player"] = {
        {"money", mMoney},
        {"death_counter", mDeathCounter},
        {"elemental_mode", ElementalModeToString(mCurrentElementalMode)},
        {"max_jumps_in_air", mMaxJumpsInAir},
        {"can_dash", mCanDash},
        {"can_wallslide", mCanWallSlide},
        {"can_hook", mCanHook}
    };
    j["player"]["elemental_modes"] = {
        {"has_earth", mHasEarthMode},
        {"has_fire", mHasFireMode},
        {"has_ice", mHasIceMode},
        {"has_lightning", mHasLightningMode}
    };

    j["skill_tree"]["unlocked_nodes"] = mUnlockedSkillNodes;

    std::string tempFilename = filename + ".tmp";
    std::ofstream file(tempFilename);

    if (!file.is_open()) {
        // não foi possível criar o arquivo temporário
        return;
    }

    // Escreve os dados
    file << j.dump(4); // Identação

    file.flush();
    if (file.good()) {
        file.close();
        std::remove(filename.c_str());
        std::rename(tempFilename.c_str(), filename.c_str());
    } else {
        file.close();
        std::remove(tempFilename.c_str());
    }
}

bool SaveData::Load(const std::string &filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    nlohmann::json j;
    file >> j;

    mGameScenePath = j["game"]["scene"];
    mLastCheckpointPosition.x = j["game"]["last_checkpoint"]["x"];
    mLastCheckpointPosition.y = j["game"]["last_checkpoint"]["y"];
    mTotalPlayTime = j["game"]["total_play_time"];

    mMasterAudio = j["configs"]["master_audio"];
    mMusicAudio = j["configs"]["music_audio"];
    mSFXAudio = j["configs"]["sfx_audio"];

    mWorldState = j["world_state"];

    mCurrentElementalMode = StringToElementalMode(j["player"]["elemental_mode"]);
    mMoney = j["player"]["money"];
    mDeathCounter = j["player"]["death_counter"];

    mMaxJumpsInAir = j["player"]["max_jumps_in_air"];
    mCanDash = j["player"]["can_dash"];
    mCanWallSlide = j["player"]["can_wallslide"];
    mCanHook = j["player"]["can_hook"];

    mHasEarthMode = j["player"]["elemental_modes"]["has_earth"];
    mHasFireMode = j["player"]["elemental_modes"]["has_fire"];
    mHasIceMode = j["player"]["elemental_modes"]["has_ice"];
    mHasLightningMode = j["player"]["elemental_modes"]["has_lightning"];

    // Carregando a árvore
    mUnlockedSkillNodes.clear();
    if (j.contains("skill_tree") && j["skill_tree"].contains("unlocked_nodes")) {
        for (const auto& node_id : j["skill_tree"]["unlocked_nodes"]) {
            mUnlockedSkillNodes.push_back(node_id);
        }
    }

    return true;
}

std::string SaveData::ElementalModeToString(Player::ElementalMode elementalMode) {
    switch (elementalMode) {
        case Player::ElementalMode::Fire: return "Fire";
        case Player::ElementalMode::Ice: return "Ice";
        case Player::ElementalMode::Lightning: return "Lightning";
        case Player::ElementalMode::Earth: return "Earth";
        default: return "Unknown";
    }
}

Player::ElementalMode SaveData::StringToElementalMode(const std::string &str) {
    if (str == "Fire") return Player::ElementalMode::Fire;
    if (str == "Ice") return Player::ElementalMode::Ice;
    if (str == "Lightning") return Player::ElementalMode::Lightning;
    if (str == "Earth") return Player::ElementalMode::Earth;
    return Player::ElementalMode::Fire; // fallback
}

void SaveData::ApplyToGame() {
    mGame->SetCheckPointPosition(mLastCheckpointPosition);
    mGame->SetCheckpointGameScenePath(mGameScenePath);
    mGame->LoadNextLevel(mGameScenePath, 0.5f);
    mGame->SetTotalPlayTime(mTotalPlayTime);
}

void SaveData::ApplyToPlayer() {
    Player* player = mGame->GetPlayer();
    PlayerSkillManager* psm = player->GetSkillManager();

    player->SetPosition(mLastCheckpointPosition);
    player->SetStartingPosition(mLastCheckpointPosition);
    player->SetElementalMode(mCurrentElementalMode);
    player->SetMoney(mMoney);
    player->SetDeathCounter(mDeathCounter);

    if (mMaxJumpsInAir == 1) psm->UnlockMechanic("double_jump");
    if (mCanDash) psm->UnlockMechanic("dash");
    if (mCanWallSlide) psm->UnlockMechanic("wall_slide");
    if (mCanHook) psm->UnlockMechanic("hook");

    mGame->GetSkillTreeManager()->LoadUnlockedNodes(mUnlockedSkillNodes, psm);
}

void SaveData::ApplyWorldState() {
    mGame->SetWorldState(mWorldState);
}

void SaveData::ApplyConfigs() {
    // Audio
    mGame->GetAudio()->SetCategoryVolume(SoundCategory::Master, mMasterAudio);
    mGame->GetAudio()->SetCategoryVolume(SoundCategory::Music, mMusicAudio);
    mGame->GetAudio()->SetCategoryVolume(SoundCategory::SFX, mSFXAudio);
}

void SaveData::CaptureFromGame() {
    mGameScenePath = mGame->GetCheckpointGameScenePath();
    mLastCheckpointPosition.x = mGame->GetCheckPointPosition().x;
    mLastCheckpointPosition.y = mGame->GetCheckPointPosition().y;
    mTotalPlayTime = mGame->GetTotalPlayTime();

    mMasterAudio = mGame->GetAudio()->GetCategoryVolume(SoundCategory::Master);
    mMusicAudio = mGame->GetAudio()->GetCategoryVolume(SoundCategory::Music);
    mSFXAudio = mGame->GetAudio()->GetCategoryVolume(SoundCategory::SFX);

    mWorldState = mGame->GetWorldState();

    Player* player = mGame->GetPlayer();
    PlayerSkillManager* psm = player->GetSkillManager();

    mMaxJumpsInAir = psm->MaxJumpsInAir();
    mCanDash = psm->CanDash();
    mCanWallSlide = psm->CanWallSlide();
    mCanHook = psm->CanHook();

    mHasEarthMode = psm->HasEarthMode();
    mHasFireMode = psm->HasFireMode();
    mHasIceMode = psm->HasIceMode();
    mHasLightningMode = psm->HasLightningMode();
    mCurrentElementalMode = player->GetElementalMode();

    mMoney = player->GetMoney();
    mDeathCounter = player->GetDeathCounter();

    // Captura a lista da árvore
    mUnlockedSkillNodes = mGame->GetSkillTreeManager()->GetUnlockedNodesIDs();
}
