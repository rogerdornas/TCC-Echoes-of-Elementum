//
// Created by roger on 13/03/2026.
//

#pragma once

#include "../UIElements/UIScreen.h"

class ConfirmQuitGame : public UIScreen
{
public:
    ConfirmQuitGame(class Game* game, const std::string& fontName, bool isClosable = true);
};

