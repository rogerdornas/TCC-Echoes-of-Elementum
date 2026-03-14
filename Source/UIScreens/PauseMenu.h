//
// Created by roger on 12/03/2026.
//

#pragma once

#include "../UIElements/UIScreen.h"

class PauseMenu : public UIScreen
{
public:
    PauseMenu(class Game* game, const std::string& fontName, UIScreen* background, bool isClosable = true);
    void Close() override;
private:
    UIScreen* mBackGround;
};

