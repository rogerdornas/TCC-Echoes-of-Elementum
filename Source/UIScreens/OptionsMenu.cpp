//
// Created by roger on 13/03/2026.
//

#include "OptionsMenu.h"
#include "AudioMenu.h"
#include "ControlMenu.h"
#include "KeyboardMenu.h"
#include "VideoMenu.h"
#include "../Game.h"

OptionsMenu::OptionsMenu(class Game *game, const std::string &fontName, bool isClosable)
    :UIScreen(game, fontName, isClosable)
{
    SetSize(Vector2(1920, 1080));
    SetPosition(Vector2::Zero);

    auto* text = AddText("OPÇÕES", Vector2::Zero,  34);
    text->SetPosition(Vector2(960, 368));

    AddImage("../Assets/Sprites/Menus/titulo_sublinhado.png", Vector2(960, 400), Vector2(360, 30));

    AddButton("VÍDEO", Vector2(901, 438), Vector2::Zero, 34, UIButton::TextPos::Center,
    [this]() {
        new VideoMenu(mGame, "../Assets/Fonts/K2D-Bold.ttf");
        mIsVisible = false;
    }
    , true);

    AddButton("TECLADO", Vector2(869, 510), Vector2::Zero, 34, UIButton::TextPos::Center,
    [this]() {
        new KeyboardMenu(mGame, "../Assets/Fonts/K2D-Bold.ttf");
        mIsVisible = false;
    }
    , true);

    AddButton("CONTROLE", Vector2(860, 577), Vector2::Zero, 34, UIButton::TextPos::Center,
    [this]() {
        new ControlMenu(mGame, "../Assets/Fonts/K2D-Bold.ttf");
        mIsVisible = false;
    }
    , true);

    AddButton("ÁUDIO", Vector2(898, 654), Vector2::Zero, 34, UIButton::TextPos::Center,
    [this]() {
        new AudioMenu(mGame, "../Assets/Fonts/K2D-Bold.ttf");
        mIsVisible = false;
    }
    , true);

    AddButton("VOLTAR", Vector2(887, 930), Vector2::Zero, 34, UIButton::TextPos::Center,
    [this]() {
        Close();
    }
    , true);
}


