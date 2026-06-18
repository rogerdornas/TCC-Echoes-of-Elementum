//
// Created by roger on 16/06/2026.
//

#include "AdvancedModeMenu.h"

#include "ScenarioSelectorMenu.h"
#include "../Game.h"

AdvancedModeMenu::AdvancedModeMenu(class Game *game, const std::string &fontName, bool isClosable)
    :UIScreen(game, fontName, isClosable)
{
    SetSize(Vector2(1920, 1080));
    SetPosition(Vector2::Zero);

    auto* background = AddImage("../Assets/Sprites/Menus/FundoPreto.png", Vector2(1920, 1080) * 0.5f, Vector2(1920, 1080));
    background->SetAlpha(0.6f);

    background = AddImage("../Assets/Sprites/Menus/FundoPretoArredondado.png", Vector2(960, 600), Vector2(1300, 650));
    background->SetAlpha(0.4f);

    std::string textMessage = "ESTE MODO DE JOGO É PARA TESTES DE CENÁRIOS DO FINAL DO JOGO, ONDE TODAS AS HABILIDADES ESTÃO LIBERADAS E SÃO NECESSÁRIAS PARA DERROTAR OS INIMIGOS.";

    auto* text = AddText(textMessage, Vector2::Zero,  34, Color::White, 1080);
    text->SetPosition(Vector2(960, 390));

    textMessage = "ALERTA: AS ANIMAÇÕES E HABILIDADES DOS INIMIGOS PODEM ESTAR INCOMPLETAS E EM DESENVOLVIMENTO. É RECOMENDADO QUE SE JOGUE O INÍCIO DO JOGO PRINCIPAL ANTES.";

    text = AddText(textMessage, Vector2::Zero,  34, Color::White, 1080);
    text->SetPosition(Vector2(960, 546));

    textMessage = "CASO QUEIRA JOGAR ESSE MODO, SUGIRO QUE COMECE PELO \"TUTORIAL DE HABILIDADES\" PARA NÃO FICAR PERDIDO.";

    text = AddText(textMessage, Vector2::Zero,  34, Color::White, 1080);
    text->SetPosition(Vector2(960, 685));

    textMessage = "DESEJA JOGAR ESSE MODO?";

    text = AddText(textMessage, Vector2::Zero,  34, Color::White, 1080);
    text->SetPosition(Vector2(960, 785));

    AddButton("SIM", Vector2(820, 840), Vector2::Zero, 34, UIButton::TextPos::Center,
    [this]() {
        new ScenarioSelectorMenu(mGame, "../Assets/Fonts/K2D-Bold.ttf");
        mIsVisible = false;
    }
    , true);

    AddButton("NÃO", Vector2(1020, 840), Vector2::Zero, 34, UIButton::TextPos::Center,
    [this]() {
        Close();
    }
    , true);
}
