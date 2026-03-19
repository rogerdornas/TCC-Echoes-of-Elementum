//
// Created by roger on 13/03/2026.
//

#pragma once

#include "../UIElements/UIScreen.h"
#include "../Game.h"

class ControlMenu : public UIScreen
{
public:
    ControlMenu(class Game* game, const std::string& fontName, bool isClosable = true);

private:
    std::string GetActionTextForButton(SDL_GameControllerButton btn);
    std::string GetActionTextForAxis(SDL_GameControllerAxis axis);
    std::string TranslateAction(Game::Action action);
};
