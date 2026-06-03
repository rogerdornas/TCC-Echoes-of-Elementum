//
// Created by Lucas N. Ferreira on 08/12/23.
//

#pragma once

#include <string>
#include "UIElements/UIScreen.h"
#include "Game.h"

class HUD : public UIScreen
{
public:
    struct RectF {
        float x, y, w, h;
    };

    const int POINT_SIZE = 30;
    const int WORD_HEIGHT = 30.0f;
    const int WORD_OFFSET = 25.0f;
    const int CHAR_WIDTH = 30.0f;
    const int HUD_POS_Y = 10.0f;

    HUD(class Game* game, const std::string& fontName);
    ~HUD();

    void Update(float deltaTime) override;

    void StartBossFight(class Enemy* boss);
    void EndBossFight(class Enemy* boss);

    void IncreaseHPBar();
    void IncreaseManaBar();

    void ShowTutorial(const std::string& message);
    void HideTutorial();

    void Draw(class Renderer *renderer) override;

private:
    enum class TutorialState {
        Hidden,
        FadingIn,
        Visible,
        FadingOut
    };

    TutorialState mTutorialState;
    float mTutorialAlpha;
    float mTutorialFadeSpeed;

    // SISTEMA DE TUTORIAL
    static const int MAX_TUTORIAL_PARTS = 5; // Suporta até 5 recortes (ex: [Texto] [Ícone] [Texto] [Ícone] [Texto])

    std::vector<UIText*> mTutorialTexts;
    std::vector<UIText*> mTutorialActionTexts;

    class UIFont* mIconFont;

    std::string mTutorialTemplate;
    Game::InputPlayerMode mLastInputMode;
    Game::InputController mLastController;
    bool mTutorialNeedsRebuild;

    void RebuildTutorialLayout();

    struct BossLifeBar {
        RectF bossHPBar;
        RectF bossDamageTakenBar;
        RectF bossHPRemainingBar;
        RectF bossHPGrowingBar;
        Enemy* boss;
        float waitToDecreaseTimer;
    };

    // HUD elements
    UIText* mPlayerHealCount;
    UIText* mPlayerMoney;
    UIImage* mPotion;
    UIImage* mElementalMode;

    float mSpeedHPDecrease;
    float mSpeedHPIncrease;

    RectF mHPBar;
    RectF mDamageTakenBar;
    RectF mHPRemainingBar;
    RectF mHPGrowingBar;

    RectF mManaBar;
    RectF mManaUsedBar;
    RectF mManaRemainingBar;
    int mNumOfSubManaBars;

    RectF mBossHPBar;
    RectF mBossDamageTakenBar;
    RectF mBossHPRemainingBar;
    RectF mBossHPGrowingBar;

    Vector2 mSlowMotionBarPos;
    float mSlowMotionBarRadius;
    float mSlowMotionBarThickness;
    Vector3 mFireColor;
    Vector3 mLightningColor;
    Vector3 mIceColor;
    Vector3 mEarthColor;

    Vector2 mFireIconSize;
    Vector2 mLightningIconSize;
    Vector2 mIceIconSize;
    Vector2 mEarthIconSize;

    std::vector<BossLifeBar> mBossLifeBars;

    float mWaitToDecreaseDuration;
    float mWaitToDecreaseTimer;
    float mWaitToDecreaseManaDuration;
    float mWaitToDecreaseManaTimer;
    bool mPlayerDie;

    void DrawLifeBar(class Renderer *renderer);
    void DrawManaBar(class Renderer * renderer);
    void DrawBossLifeBar(class Renderer * renderer);
    void DrawSlowMotionBar(class Renderer * renderer);
};
