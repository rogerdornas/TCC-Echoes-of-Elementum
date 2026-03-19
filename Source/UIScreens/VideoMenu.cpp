//
// Created by roger on 13/03/2026.
//

#include "VideoMenu.h"
#include "../Game.h"

VideoMenu::VideoMenu(class Game *game, const std::string &fontName, bool isClosable)
    :UIScreen(game, fontName, isClosable)
{
    SetSize(Vector2(1920, 1080));
    SetPosition(Vector2::Zero);

    auto* background = AddImage("../Assets/Sprites/Menus/FundoPreto.png", Vector2(1920, 1080) * 0.5f, Vector2(1920, 1080));
    background->SetAlpha(0.6f);

    auto* text = AddText("VÍDEO", Vector2::Zero,  34);
    text->SetPosition(Vector2(960, 290));

    AddImage("../Assets/Sprites/Menus/titulo_sublinhado.png", Vector2(960, 322), Vector2(360, 30));

    if (SDL_GetWindowFlags(mGame->GetWindow()) & SDL_WINDOW_FULLSCREEN_DESKTOP) {
        AddButton("TELA CHEIA", Vector2(760, 360), Vector2(390, 42), 34, UIButton::TextPos::AlignLeft,
        [this]() {
            SDL_SetWindowFullscreen(mGame->GetWindow(), 0);
            Close();
            new VideoMenu(mGame, "../Assets/Fonts/K2D-Bold.ttf");
        }, false);

        text = AddText("ON", Vector2::Zero, 34);
        text->SetPosition(Vector2(1120, 380));
    }
    else {
        AddButton("TELA CHEIA", Vector2(760, 360), Vector2(390, 42), 34, UIButton::TextPos::AlignLeft,
        [this]() {
            Uint32 flags = SDL_GetWindowFlags(mGame->GetWindow());

            // Alterna o bit de fullscreen
            if (flags & SDL_WINDOW_FULLSCREEN_DESKTOP)
            {
                SDL_SetWindowFullscreen(mGame->GetWindow(), 0);
            }
            else
            {
                SDL_SetWindowFullscreen(mGame->GetWindow(), SDL_WINDOW_FULLSCREEN_DESKTOP);
            }

            Close();
            new VideoMenu(mGame, "../Assets/Fonts/K2D-Bold.ttf");
        }, false);

        text = AddText("OFF", Vector2::Zero, 34);
        text->SetPosition(Vector2(1120, 380));
    }

    AddButton("VOLTAR", Vector2(887, 930), Vector2::Zero, 34, UIButton::TextPos::Center,
    [this]() {
        Close();
    }
    , true);
}
