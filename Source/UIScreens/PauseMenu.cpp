//
// Created by roger on 12/03/2026.
//

#include "PauseMenu.h"

#include "ConfirmBackToMenu.h"
#include "OptionsMenu.h"
#include "../Game.h"

PauseMenu::PauseMenu(class Game *game, const std::string &fontName, UIScreen* background, bool isClosable)
    :UIScreen(game, fontName, isClosable)
    ,mBackGround(background)
{
    SetSize(Vector2(1920, 1080));
    SetPosition(Vector2::Zero);

    AddButton("CONTINUAR", Vector2(856, 445), Vector2::Zero, 34, UIButton::TextPos::Center,
    [this]() {
        mGame->TogglePause();
        Close();
    }
    , true);

    AddButton("OPÇÕES", Vector2(888, 519), Vector2::Zero, 34, UIButton::TextPos::Center,
    [this]() {
        new OptionsMenu(mGame, "../Assets/Fonts/K2D-Bold.ttf");
        mIsVisible = false;
    }
    , true);

    AddButton("SELECIONAR FASE", Vector2(805, 593), Vector2::Zero, 34, UIButton::TextPos::Center,
    [this]() {
        mGame->LoadLevelSelectMenu();
        mIsVisible = false;
    }
    , true);

    AddButton("VOLTAR AO MENU", Vector2(811, 667), Vector2::Zero, 34, UIButton::TextPos::Center,
    [this]() {
        new ConfirmBackToMenu(mGame, "../Assets/Fonts/K2D-Bold.ttf");
        mIsVisible = false;
    }
    , true);
}

void PauseMenu::Close() {
    UIScreen::Close();
    mBackGround->Close();
}
