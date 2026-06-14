//
// Created by roger on 09/04/2026.
//

#include "CheckPointMenu.h"

#include "SkillMenu.h"
#include "../Game.h"

CheckPointMenu::CheckPointMenu(class Game *game, const std::string &fontName, bool isClosable)
    :UIScreen(game, fontName, isClosable)
{
    mGame->SaveGame();
    mGame->TogglePause();

    SetSize(Vector2(1920, 1080));
    SetPosition(Vector2::Zero);

    auto* background = AddImage("../Assets/Sprites/Menus/FundoPreto.png", Vector2(1920, 1080) * 0.5f, Vector2(1920, 1080));
    background->SetAlpha(0.6f);

    auto* text = AddText("FLORESTA", Vector2::Zero,  34);
    text->SetPosition(Vector2(960, 421));

    AddImage("../Assets/Sprites/Menus/titulo_sublinhado.png", Vector2(960, 453), Vector2(360, 30));

    AddButton("DESCANSAR", Vector2(849, 492), Vector2::Zero, 30, UIButton::TextPos::Center,
    [this]() {
        auto* player = mGame->GetPlayer();
        player->ResetHealthPoints();
        player->ResetMana();
        player->ResetHealCount();
        Close();
    }
    , true);

    AddButton("HABILIDADES", Vector2(845, 541), Vector2::Zero, 30, UIButton::TextPos::Center,
    [this]() {
        new SkillMenu(mGame, "../Assets/Fonts/K2D-Bold.ttf");
        mIsVisible = false;
    }
    , true);

    AddButton("VOLTAR", Vector2(890, 590), Vector2::Zero, 30, UIButton::TextPos::Center,
    [this]() {
        Close();
    }
    , true);
}

void CheckPointMenu::Close() {
    UIScreen::Close();
    mGame->SaveGame();
    mGame->TogglePause();
}
