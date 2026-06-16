//
// Created by roger on 16/06/2026.
//

#pragma once

#include "../UIElements/UIScreen.h"

class PauseAdvancedMenu : public UIScreen
{
public:
    PauseAdvancedMenu(class Game* game, const std::string& fontName, bool isClosable = true);
    void Close() override;
};
