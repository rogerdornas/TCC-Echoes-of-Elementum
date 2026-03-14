//
// Created by roger on 13/03/2026.
//

#include "ControlMenu.h"
#include "../Game.h"

ControlMenu::ControlMenu(class Game *game, const std::string &fontName, bool isClosable)
    :UIScreen(game, fontName, isClosable)
{
    SetSize(Vector2(1920, 1080));
    SetPosition(Vector2::Zero);

    auto* text = AddText("CONTROLE", Vector2::Zero,  34);
    text->SetPosition(Vector2(960, 368));

    AddImage("../Assets/Sprites/Menus/titulo_sublinhado.png", Vector2(960, 400), Vector2(360, 30));






    AddButton("VOLTAR", Vector2(887, 930), Vector2::Zero, 34, UIButton::TextPos::Center,
    [this]() {
        Close();
    }
    , true);


}
