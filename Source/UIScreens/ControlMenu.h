//
// Created by roger on 13/03/2026.
//

#pragma once

#include "../UIElements/UIScreen.h"

class ControlMenu : public UIScreen
{
public:
    ControlMenu(class Game* game, const std::string& fontName, bool isClosable = true);
};
