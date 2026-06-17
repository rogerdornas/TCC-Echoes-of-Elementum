//
// Created by roger on 17/06/2026.
//

#include "ThankYouScreen.h"
#include "../Game.h"

ThankYouScreen::ThankYouScreen(class Game *game, const std::string &fontName, bool isClosable)
    :UIScreen(game, fontName, isClosable)
{
    SetSize(Vector2(1920, 1080));
    SetPosition(Vector2::Zero);

    AddImage("../Assets/Sprites/Background/Menu6.png", Vector2(1920, 1080) * 0.5f, Vector2(1920, 1080));

    auto* background = AddImage("../Assets/Sprites/Menus/FundoPretoArredondado.png", Vector2(960, 660), Vector2(690, 450));
    background->SetAlpha(0.5f);

    auto* text = AddText("OBRIGADO POR JOGAR!", Vector2::Zero,  40);
    text->SetPosition(Vector2(960, 500));

    text = AddText("AGUARDE PARA MAIS ATUALIZAÇÕES", Vector2::Zero,  34);
    text->SetPosition(Vector2(960, 600));

    text = AddText("DESENVOLVIDO POR ROGER DORNAS", Vector2::Zero,  24);
    text->SetPosition(Vector2(1055, 720));
    text->SetColor(Vector3(0.7f, 0.7f, 0.7f));

    AddButton("VOLTAR AO MENU", Vector2(811, 800), Vector2::Zero, 34, UIButton::TextPos::Center,
    [this]() {
        mGame->BackToMenu();
    }
    , true);
}
