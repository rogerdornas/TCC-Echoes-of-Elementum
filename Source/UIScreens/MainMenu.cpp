//
// Created by roger on 13/03/2026.
//

#include "MainMenu.h"

#include "AdvancedModeMenu.h"
#include "ConfirmQuitGame.h"
#include "OptionsMenu.h"
#include "SelectProfileMenu.h"
#include "../Game.h"

MainMenu::MainMenu(class Game *game, const std::string &fontName, bool isClosable)
    :UIScreen(game, fontName, isClosable)
{
    SetSize(Vector2(1920, 1080));
    SetPosition(Vector2::Zero);

    AddButton("INICIAR JOGO", Vector2(846, 639), Vector2::Zero, 34, UIButton::TextPos::Center,
        [this]() {
            new SelectProfileMenu(mGame, "../Assets/Fonts/K2D-Bold.ttf");
            mIsVisible = false;
        }
    , true);

    AddButton("MODO AVANÇADO", Vector2(806, 713), Vector2::Zero, 34, UIButton::TextPos::Center,
        [this]() {
            new AdvancedModeMenu(mGame, "../Assets/Fonts/K2D-Bold.ttf");
            mIsVisible = false;
        }
    , true);

    AddButton("OPÇÕES", Vector2(888, 787), Vector2::Zero, 34, UIButton::TextPos::Center,
        [this]() {
            new OptionsMenu(mGame, "../Assets/Fonts/K2D-Bold.ttf");
            mIsVisible = false;
        }
    , true);

    AddButton("SAIR", Vector2(920, 861), Vector2::Zero, 34, UIButton::TextPos::Center,
        [this]() {
            new ConfirmQuitGame(mGame, "../Assets/Fonts/K2D-Bold.ttf");
            mIsVisible = false;
        }
    , true);
}
