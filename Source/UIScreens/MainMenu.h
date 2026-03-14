//
// Created by roger on 13/03/2026.
//

#pragma once

#include "../UIElements/UIScreen.h"

class MainMenu : public UIScreen
{
public:
    MainMenu(class Game* game, const std::string& fontName, bool isClosable = false);
};

