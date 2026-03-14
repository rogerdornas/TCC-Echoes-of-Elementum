//
// Created by roger on 13/03/2026.
//

#pragma once

#include "../UIElements/UIScreen.h"

class ConfirmBackToMenu : public UIScreen
{
public:
    ConfirmBackToMenu(class Game* game, const std::string& fontName, bool isClosable = true);
};

