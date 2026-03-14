//
// Created by roger on 13/03/2026.
//

#pragma once

#include "../UIElements/UIScreen.h"

class SelectProfileMenu : public UIScreen
{
public:
    struct ProfileData {
        float totalPlayTime;
        int money;
        std::string location;
    };

    SelectProfileMenu(class Game* game, const std::string& fontName, bool isClosable = true);
    void Draw(class Renderer *renderer) override;
private:
    void GetProfileData();
    static std::string SecondsToFormatedTime(float seconds);
    std::string mSaveFolder;
    std::vector<ProfileData> mProfilesData;
};

