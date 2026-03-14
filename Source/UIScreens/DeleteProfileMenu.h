//
// Created by roger on 14/03/2026.
//

#pragma once

#include "../UIElements/UIScreen.h"
#include "../UIScreens/SelectProfileMenu.h"

class DeleteProfileMenu : public UIScreen
{
public:
    DeleteProfileMenu(class Game* game, const std::string& fontName, const SelectProfileMenu::ProfileData& profileData, int slot, bool isClosable = true);
    void Draw(class Renderer *renderer) override;
    void Close() override;
private:
    int mSlot;
    static std::string SecondsToFormatedTime(float seconds);
};
