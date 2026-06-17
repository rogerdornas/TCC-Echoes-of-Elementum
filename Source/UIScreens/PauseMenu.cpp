//
// Created by roger on 12/03/2026.
//

#include "PauseMenu.h"
#include "ConfirmBackToMenu.h"
#include "OptionsMenu.h"
#include "../Game.h"

PauseMenu::PauseMenu(class Game *game, const std::string &fontName, bool isClosable)
    :UIScreen(game, fontName, isClosable)
{
    mGame->TogglePause();

    SetSize(Vector2(1920, 1080));
    SetPosition(Vector2::Zero);

    auto* background = AddImage("../Assets/Sprites/Menus/FundoPreto.png", Vector2(1920, 1080) * 0.5f, Vector2(1920, 1080));
    background->SetAlpha(0.6f);

    auto* text = AddText("PAUSA", Vector2::Zero,  34);
    text->SetPosition(Vector2(960, 375));

    AddImage("../Assets/Sprites/Menus/titulo_sublinhado.png", Vector2(960, 407), Vector2(360, 30));

    AddButton("CONTINUAR", Vector2(856, 445), Vector2::Zero, 34, UIButton::TextPos::Center,
    [this]() {
        Close();
    }
    , true);

    AddButton("OPÇÕES", Vector2(888, 519), Vector2::Zero, 34, UIButton::TextPos::Center,
    [this]() {
        new OptionsMenu(mGame, "../Assets/Fonts/K2D-Bold.ttf");
        mIsVisible = false;
    }
    , true);

    AddButton("VOLTAR AO MENU", Vector2(811, 593), Vector2::Zero, 34, UIButton::TextPos::Center,
    [this]() {
        new ConfirmBackToMenu(mGame, "../Assets/Fonts/K2D-Bold.ttf");
        mIsVisible = false;
    }
    , true);
}

void PauseMenu::Close() {
    UIScreen::Close();
    mGame->TogglePause();
}
