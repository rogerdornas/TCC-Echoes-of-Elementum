//
// Created by roger on 13/03/2026.
//

#include "ControlMenu.h"
#include "RemapControllerMenu.h"

ControlMenu::ControlMenu(class Game *game, const std::string &fontName, bool isClosable)
    :UIScreen(game, fontName, isClosable)
{
    SetSize(Vector2(1920, 1080));
    SetPosition(Vector2::Zero);

    auto* background = AddImage("../Assets/Sprites/Menus/FundoPreto.png", Vector2(1920, 1080) * 0.5f, Vector2(1920, 1080));
    background->SetAlpha(0.6f);

    float textPositionX1 = 641.0f;
    float textPositionX2 = 1280.0f;
    int textPointSize = 20;

    auto* text = AddText("CONTROLE", Vector2::Zero,  34);
    text->SetPosition(Vector2(960, 290));

    AddImage("../Assets/Sprites/Menus/titulo_sublinhado.png", Vector2(960, 322), Vector2(360, 30));

    AddImage("../Assets/Sprites/Menus/ControllerXboxArrows.png", Vector2(960, 590), Vector2(659, 460));

    text = AddText("LT", Vector2::Zero, textPointSize);
    text->SetPosition(Vector2(668, 360));

    text = AddText("LB", Vector2::Zero, textPointSize);
    text->SetPosition(Vector2(668, 400));

    text = AddText(GetActionTextForAxis(SDL_CONTROLLER_AXIS_TRIGGERLEFT), Vector2::Zero, textPointSize);
    text->SetPosition(Vector2(textPositionX1, 375) - Vector2(text->GetSize().x * 0.5f, 0));

    text = AddText(GetActionTextForButton(SDL_CONTROLLER_BUTTON_LEFTSHOULDER), Vector2::Zero, textPointSize);
    text->SetPosition(Vector2(textPositionX1, 415) - Vector2(text->GetSize().x * 0.5f, 0));

    text = AddText(GetActionTextForButton(SDL_CONTROLLER_BUTTON_BACK), Vector2::Zero, textPointSize);
    text->SetPosition(Vector2(textPositionX1, 460) - Vector2(text->GetSize().x * 0.5f, 0));

    text = AddText("MOVER", Vector2::Zero, textPointSize);
    text->SetPosition(Vector2(textPositionX1, 535) - Vector2(text->GetSize().x * 0.5f, 0));

    text = AddText("MOVER", Vector2::Zero, textPointSize);
    text->SetPosition(Vector2(textPositionX1, 620) - Vector2(text->GetSize().x * 0.5f, 0));

    text = AddText("MOVER", Vector2::Zero, textPointSize);
    text->SetPosition(Vector2(textPositionX1, 620) - Vector2(text->GetSize().x * 0.5f, 0));

    text = AddText("RT", Vector2::Zero, textPointSize);
    text->SetPosition(Vector2(1254, 360));

    text = AddText("RB", Vector2::Zero, textPointSize);
    text->SetPosition(Vector2(1254, 400));

    text = AddText(GetActionTextForAxis(SDL_CONTROLLER_AXIS_TRIGGERRIGHT), Vector2::Zero, textPointSize);
    text->SetPosition(Vector2(textPositionX2, 375) + Vector2(text->GetSize().x * 0.5f, 0));

    text = AddText(GetActionTextForButton(SDL_CONTROLLER_BUTTON_RIGHTSHOULDER), Vector2::Zero, textPointSize);
    text->SetPosition(Vector2(textPositionX2, 415) + Vector2(text->GetSize().x * 0.5f, 0));

    text = AddText("PAUSA", Vector2::Zero, textPointSize);
    text->SetPosition(Vector2(textPositionX2, 460) + Vector2(text->GetSize().x * 0.5f, 0));

    text = AddText(GetActionTextForButton(SDL_CONTROLLER_BUTTON_Y), Vector2::Zero, textPointSize);
    text->SetPosition(Vector2(textPositionX2, 500) + Vector2(text->GetSize().x * 0.5f, 0));

    text = AddText(GetActionTextForButton(SDL_CONTROLLER_BUTTON_B), Vector2::Zero, textPointSize);
    text->SetPosition(Vector2(textPositionX2, 537) + Vector2(text->GetSize().x * 0.5f, 0));

    text = AddText(GetActionTextForButton(SDL_CONTROLLER_BUTTON_A), Vector2::Zero, textPointSize);
    text->SetPosition(Vector2(textPositionX2, 573) + Vector2(text->GetSize().x * 0.5f, 0));

    text = AddText(GetActionTextForButton(SDL_CONTROLLER_BUTTON_X), Vector2::Zero, textPointSize);
    text->SetPosition(Vector2(textPositionX2, 620) + Vector2(text->GetSize().x * 0.5f, 0));

    text = AddText("OLHAR", Vector2::Zero, textPointSize);
    text->SetPosition(Vector2(textPositionX2, 663) + Vector2(text->GetSize().x * 0.5f, 0));


    AddButton("REMAPEAR BOTÕES", Vector2(794, 842), Vector2::Zero, 34, UIButton::TextPos::Center,
    [this]() {
        new RemapControllerMenu(mGame, "../Assets/Fonts/K2D-Bold.ttf");
        Close();
    }
    , true);

    AddButton("VOLTAR", Vector2(887, 930), Vector2::Zero, 34, UIButton::TextPos::Center,
    [this]() {
        Close();
    }
    , true);
}

std::string ControlMenu::GetActionTextForButton(SDL_GameControllerButton btn) {
    auto bindings = mGame->GetInputBinding();

    for (const auto& pair : bindings) {
        if (pair.second.btn == btn) {
            return TranslateAction(pair.first);
        }
    }

    return "---";
}

std::string ControlMenu::GetActionTextForAxis(SDL_GameControllerAxis axis) {
    auto bindings = mGame->GetInputBinding();

    for (const auto& pair : bindings) {
        if (pair.second.axis == axis) {
            return TranslateAction(pair.first);
        }
    }

    return "---";
}

std::string ControlMenu::TranslateAction(Game::Action action) {
    switch (action) {
        case Game::Action::MoveLeft:  return "ESQUERDA";
        case Game::Action::MoveRight: return "DIREITA";
        case Game::Action::Up:        return "CIMA";
        case Game::Action::Down:      return "BAIXO";
        case Game::Action::Look:      return "OLHAR";
        case Game::Action::Jump:      return "PULO";
        case Game::Action::Dash:      return "AVANCO";
        case Game::Action::Attack:    return "ATAQUE";
        case Game::Action::Skill1:    return "HABILIDADE 1";
        case Game::Action::Skill2:    return "HABILIDADE 2";
        case Game::Action::Heal:      return "CURA";
        case Game::Action::Hook:      return "GANCHO";
        case Game::Action::Pause:     return "PAUSAR";
        case Game::Action::OpenStore: return "LOJA";
        case Game::Action::Map:       return "MAPA";
        default:                      return "---";
    }
}