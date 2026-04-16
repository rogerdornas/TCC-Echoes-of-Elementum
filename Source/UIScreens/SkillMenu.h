//
// Created by roger on 10/04/2026.
//

#pragma once

#include "../UIElements/UIScreen.h"

class SkillMenu : public UIScreen
{
public:
    SkillMenu(class Game* game, const std::string& fontName, bool isClosable = true);
    void Draw(class Renderer *renderer) override;

    UIButton* FindNeighbor(UIButton *current, const Vector2 &dir) override;

private:
    class UISkillNodeButton* GetNodeButton(const std::string& skillId);
    void SelectSkill(const std::string& skillId);
    void AttemptUnlockSkill();
    void RefreshUI();

    std::string mSelectedSkillId;

    class UIImage* mPlayerGoldImage;
    class UIImage* mPlayerFireStoneImage;
    class UIImage* mPlayerEarthStoneImage;
    class UIImage* mPlayerIceStoneImage;
    class UIImage* mPlayerLightningStoneImage;
    class UIText* mPlayerGoldText;
    class UIText* mPlayerFireStoneText;
    class UIText* mPlayerEarthStoneText;
    class UIText* mPlayerIceStoneText;
    class UIText* mPlayerLightningStoneText;

    class UIText* mTitleText;
    class UIText* mTypeText;
    class UIText* mDescriptionTitle;
    class UIText* mDescriptionText;
    class UIText* mHowToUseTitle;
    class UIText* mHowToUseText;
    class UIText* mUnlockCostTitle;
    class UIImage* mGoldImage;
    class UIImage* mStoneImage;
    class UIText* mUnlockText;

    class UIText* mGoldCostText;
    class UIText* mStoneCostText;

    Vector2 mPlayerGoldTextPosition;
    Vector2 mPlayerFireStoneTextPosition;
    Vector2 mPlayerEarthStoneTextPosition;
    Vector2 mPlayerIceStoneTextPosition;
    Vector2 mPlayerLightningStoneTextPosition;

    Vector2 mTitleTextPosition;
    Vector2 mTypeTextPosition;
    Vector2 mDescriptionTextPosition;
    Vector2 mHowToUseTextPosition;
    Vector2 mGoldCostTextPosition;
    Vector2 mStoneCostTextPosition;
    Vector2 mUnlockTextPosition;
};

