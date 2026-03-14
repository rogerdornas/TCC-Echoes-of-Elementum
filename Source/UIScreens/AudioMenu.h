//
// Created by roger on 13/03/2026.
//

#pragma once

#include "../UIElements/UIScreen.h"

class AudioMenu : public UIScreen
{
public:
    AudioMenu(class Game* game, const std::string& fontName, bool isClosable = true);
};
