//
// Created by roger on 09/04/2026.
//

#pragma once

#include "../UIElements/UIScreen.h"

class CheckPointMenu : public UIScreen
{
public:
    CheckPointMenu(class Game* game, const std::string& fontName, bool isClosable = true);
    void Close() override;
};

