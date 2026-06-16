//
// Created by roger on 16/06/2026.
//

#pragma once

#include "../UIElements/UIScreen.h"

class AdvancedModeMenu : public UIScreen
{
public:
    AdvancedModeMenu(class Game* game, const std::string& fontName, bool isClosable = true);
};
