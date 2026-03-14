//
// Created by roger on 13/03/2026.
//

#include "AudioMenu.h"
#include "../Game.h"

AudioMenu::AudioMenu(class Game *game, const std::string &fontName, bool isClosable)
    :UIScreen(game, fontName, isClosable)
{
    SetSize(Vector2(1920, 1080));
    SetPosition(Vector2::Zero);

    Vector2 buttonPos(700, 438);
    Vector2 buttonSize(520, 42);
    Vector2 sliderOffset(200, buttonSize.y / 2);
    Vector2 sliderSize(270, 3);

    auto* text = AddText("ÁUDIO", Vector2::Zero,  34);
    text->SetPosition(Vector2(960, 368));

    AddImage("../Assets/Sprites/Menus/titulo_sublinhado.png", Vector2(960, 400), Vector2(360, 30));

    AddSlider("GERAL", buttonPos, buttonSize, sliderOffset, sliderSize, 0.0f, 1.0f, mGame->GetAudio()->GetCategoryVolume(SoundCategory::Master), 34, 20,
        [this](float valor) {
            mGame->GetAudio()->SetCategoryVolume(SoundCategory::Master, valor);
        }
    );

    AddSlider("MÚSICA", Vector2(700, 511), buttonSize, sliderOffset, sliderSize, 0.0f, 1.0f, mGame->GetAudio()->GetCategoryVolume(SoundCategory::Master), 34, 20,
        [this](float valor) {
            mGame->GetAudio()->SetCategoryVolume(SoundCategory::Music, valor);
        }
    );

    AddSlider("EFEITOS", Vector2(700, 584), buttonSize, sliderOffset, sliderSize, 0.0f, 1.0f, mGame->GetAudio()->GetCategoryVolume(SoundCategory::SFX), 34, 20,
        [this](float valor) {
            mGame->GetAudio()->SetCategoryVolume(SoundCategory::SFX, valor);
        }
    );

    AddButton("VOLTAR", Vector2(887, 930), Vector2::Zero, 34, UIButton::TextPos::Center,
    [this]() {
        Close();
    }
    , true);

}
