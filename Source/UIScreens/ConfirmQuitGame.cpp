//
// Created by roger on 13/03/2026.
//

#include "ConfirmQuitGame.h"
#include "../Game.h"

ConfirmQuitGame::ConfirmQuitGame(class Game *game, const std::string &fontName, bool isClosable)
    :UIScreen(game, fontName, isClosable)
{
    SetSize(Vector2(1920, 1080));
    SetPosition(Vector2::Zero);

    // AddImage("../Assets/Sprites/Background/Menu6.png", GetSize() / 2, GetSize());

    auto* text = AddText("SAIR DO JOGO?", Vector2::Zero,  34);
    text->SetPosition(Vector2(960, 582));

    AddImage("../Assets/Sprites/Menus/titulo_sublinhado.png", Vector2(960, 614), Vector2(360, 30));

    AddButton("SIM", Vector2(920, 670), Vector2::Zero, 34, UIButton::TextPos::Center,
    [this]() {
        mGame->Quit();
    }
    , true);

    AddButton("NÃO", Vector2(917, 735), Vector2::Zero, 34, UIButton::TextPos::Center,
    [this]() {
        Close();
    }
    , true);
}
