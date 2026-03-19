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

    auto* background = AddImage("../Assets/Sprites/Menus/FundoPreto.png", Vector2(1920, 1080) * 0.5f, Vector2(1920, 1080));
    background->SetAlpha(0.6f);

    Vector2 buttonPos(700, 360);
    Vector2 buttonSize(520, 42);
    Vector2 sliderOffset(200, buttonSize.y / 2);
    Vector2 sliderSize(270, 3);

    auto* text = AddText("ÁUDIO", Vector2::Zero,  34);
    text->SetPosition(Vector2(960, 290));

    AddImage("../Assets/Sprites/Menus/titulo_sublinhado.png", Vector2(960, 322), Vector2(360, 30));

    AddSlider("GERAL", buttonPos, buttonSize, sliderOffset, sliderSize, 0.0f, 1.0f, mGame->GetAudio()->GetCategoryVolume(SoundCategory::Master), 34, 20,
        [this](float valor) {
            mGame->GetAudio()->SetCategoryVolume(SoundCategory::Master, valor);
        }
    );

    AddSlider("MÚSICA", Vector2(700, 433), buttonSize, sliderOffset, sliderSize, 0.0f, 1.0f, mGame->GetAudio()->GetCategoryVolume(SoundCategory::Master), 34, 20,
        [this](float valor) {
            mGame->GetAudio()->SetCategoryVolume(SoundCategory::Music, valor);
        }
    );

    AddSlider("EFEITOS", Vector2(700, 506), buttonSize, sliderOffset, sliderSize, 0.0f, 1.0f, mGame->GetAudio()->GetCategoryVolume(SoundCategory::SFX), 34, 20,
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
