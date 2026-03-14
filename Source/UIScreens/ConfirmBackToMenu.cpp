//
// Created by roger on 13/03/2026.
//

#include "ConfirmBackToMenu.h"
#include "../Game.h"

ConfirmBackToMenu::ConfirmBackToMenu(class Game *game, const std::string &fontName, bool isClosable)
    :UIScreen(game, fontName, isClosable)
{
    SetSize(Vector2(1920, 1080));
    SetPosition(Vector2::Zero);

    auto* text = AddText("VOLTAR AO MENU?", Vector2::Zero,  34);
    text->SetPosition(Vector2(960, 463));

    AddImage("../Assets/Sprites/Menus/titulo_sublinhado.png", Vector2(960, 495), Vector2(360, 30));

    AddButton("SIM", Vector2(920, 531), Vector2::Zero, 34, UIButton::TextPos::Center,
    [this]() {
        Close();
        mGame->BackToMenu();
    }
    , true);

    AddButton("NÃO", Vector2(917, 596), Vector2::Zero, 34, UIButton::TextPos::Center,
    [this]() {
        Close();
    }
    , true);
}
