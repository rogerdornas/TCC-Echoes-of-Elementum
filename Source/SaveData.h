//
// Created by roger on 28/09/2025.
//

#pragma once
#include "Game.h"
#include <unordered_map>

class SaveData
{
public:
    SaveData(class Game* game);

    void Save(const std::string& filename);
    bool Load(const std::string& filename);

    float GetTotalPlayTime() const { return mTotalPlayTime; }

    // injeta dados do save no jogo
    void ApplyToGame();
    void ApplyToPlayer();
    void ApplyWorldState();
    void ApplyConfigs();

    // captura dados atuais do jogo para o save
    void CaptureFromGame();

private:
    std::string ElementalModeToString(Player::ElementalMode elementalMode);
    Player::ElementalMode StringToElementalMode(const std::string& str);

    std::unordered_map<std::string, bool> mWorldState;

    class Game* mGame;
    std::string mGameScenePath;
    float mTotalPlayTime;

    float mMasterAudio;
    float mMusicAudio;
    float mSFXAudio;

    Vector2 mLastCheckpointPosition;

    bool mHasEarthMode;
    bool mHasFireMode;
    bool mHasIceMode;
    bool mHasLightningMode;
    Player::ElementalMode mCurrentElementalMode;
    int mMoney;
    int mDeathCounter;
    int mMaxJumpsInAir;
    bool mCanDash;
    bool mCanWallSlide;
    bool mCanHook;

    std::vector<std::string> mUnlockedSkillNodes;
};

