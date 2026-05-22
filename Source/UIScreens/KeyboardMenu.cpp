//
// Created by roger on 13/03/2026.
//

#include "KeyboardMenu.h"

KeyboardMenu::KeyboardMenu(class Game *game, const std::string &fontName, bool isClosable)
    :UIScreen(game, fontName, isClosable)
{
    SetSize(Vector2(1920, 1080));
    SetPosition(Vector2::Zero);

    auto* background = AddImage("../Assets/Sprites/Menus/FundoPreto.png", Vector2(1920, 1080) * 0.5f, Vector2(1920, 1080));
    background->SetAlpha(0.6f);

    mButtonSize = Vector2(615, 58);
    mButtonPos = Vector2(321, 385);
    mButtonPos2 = Vector2(1013, 385);
    mButtonTextOffset = Vector2(10, 0);
    mTextPos = Vector2(835, 413);
    mTextPos2 = Vector2(1527, 413);

    auto* text = AddText("TECLADO", Vector2::Zero,  34);
    text->SetPosition(Vector2(960, 290));

    AddImage("../Assets/Sprites/Menus/titulo_sublinhado.png", Vector2(960, 322), Vector2(360, 30));

    auto* button = AddButton("CIMA", mButtonPos + Vector2(0, mButtonSize.y * 0), mButtonSize, 34, UIButton::TextPos::AlignLeft,
    [this]() {
        GetTexts()[1]->SetText("...");
        GetTexts()[1]->SetPosition(mTextPos + Vector2(0, mButtonSize.y) * 0);
        mGame->RebindKeyboard(GetTexts()[1], Game::Action::Up);
    }
    , false, mButtonTextOffset);
    button->UseBackGroundColor();

    text = AddText(GetBindingText(Game::Action::Up), Vector2::Zero, 34);
    text->SetPosition(mTextPos + Vector2(0, mButtonSize.y) * 0);


    button = AddButton("BAIXO", mButtonPos + Vector2(0, mButtonSize.y * 1), mButtonSize, 34, UIButton::TextPos::AlignLeft,
    [this]() {
        GetTexts()[2]->SetText("...");
        GetTexts()[2]->SetPosition(mTextPos + Vector2(0, mButtonSize.y) * 1);
        mGame->RebindKeyboard(GetTexts()[2], Game::Action::Down);
    }
    , false, mButtonTextOffset);
    button->UseBackGroundColor();

    text = AddText(GetBindingText(Game::Action::Down), Vector2::Zero, 34);
    text->SetPosition(mTextPos + Vector2(0, mButtonSize.y) * 1);


    button = AddButton("PULO", mButtonPos + Vector2(0, mButtonSize.y * 2), mButtonSize, 34, UIButton::TextPos::AlignLeft,
    [this]() {
        GetTexts()[3]->SetText("...");
        GetTexts()[3]->SetPosition(mTextPos + Vector2(0, mButtonSize.y) * 2);
        mGame->RebindKeyboard(GetTexts()[3], Game::Action::Jump);
    }
    , false, mButtonTextOffset);
    button->UseBackGroundColor();

    text = AddText(GetBindingText(Game::Action::Jump), Vector2::Zero, 34);
    text->SetPosition(mTextPos + Vector2(0, mButtonSize.y) * 2);


    button = AddButton("ATAQUE", mButtonPos + Vector2(0, mButtonSize.y * 3), mButtonSize, 34, UIButton::TextPos::AlignLeft,
    [this]() {
        GetTexts()[4]->SetText("...");
        GetTexts()[4]->SetPosition(mTextPos + Vector2(0, mButtonSize.y) * 3);
        mGame->RebindKeyboard(GetTexts()[4], Game::Action::Attack);
    }
    , false, mButtonTextOffset);
    button->UseBackGroundColor();

    text = AddText(GetBindingText(Game::Action::Attack), Vector2::Zero, 34);
    text->SetPosition(mTextPos + Vector2(0, mButtonSize.y) * 3);


    button = AddButton("AVANÇO", mButtonPos + Vector2(0, mButtonSize.y * 4), mButtonSize, 34, UIButton::TextPos::AlignLeft,
    [this]() {
        GetTexts()[5]->SetText("...");
        GetTexts()[5]->SetPosition(mTextPos + Vector2(0, mButtonSize.y) * 4);
        mGame->RebindKeyboard(GetTexts()[5], Game::Action::Dash);
    }
    , false, mButtonTextOffset);
    button->UseBackGroundColor();

    text = AddText(GetBindingText(Game::Action::Dash), Vector2::Zero, 34);
    text->SetPosition(mTextPos + Vector2(0, mButtonSize.y) * 4);


    button = AddButton("HABILIDADE 1", mButtonPos + Vector2(0, mButtonSize.y * 5), mButtonSize, 34, UIButton::TextPos::AlignLeft,
    [this]() {
        GetTexts()[6]->SetText("...");
        GetTexts()[6]->SetPosition(mTextPos + Vector2(0, mButtonSize.y) * 5);
        mGame->RebindKeyboard(GetTexts()[6], Game::Action::Skill1);
    }
    , false, mButtonTextOffset);
    button->UseBackGroundColor();

    text = AddText(GetBindingText(Game::Action::Skill1), Vector2::Zero, 34);
    text->SetPosition(mTextPos + Vector2(0, mButtonSize.y) * 5);


    button = AddButton("HABILIDADE 2", mButtonPos + Vector2(0, mButtonSize.y * 6), mButtonSize, 34, UIButton::TextPos::AlignLeft,
    [this]() {
        GetTexts()[7]->SetText("...");
        GetTexts()[7]->SetPosition(mTextPos + Vector2(0, mButtonSize.y) * 6);
        mGame->RebindKeyboard(GetTexts()[7], Game::Action::Skill2);
    }
    , false, mButtonTextOffset);
    button->UseBackGroundColor();

    text = AddText(GetBindingText(Game::Action::Skill2), Vector2::Zero, 34);
    text->SetPosition(mTextPos + Vector2(0, mButtonSize.y) * 6);


    button = AddButton("ESQUERDA", mButtonPos2 + Vector2(0, mButtonSize.y * 0), mButtonSize, 34, UIButton::TextPos::AlignLeft,
    [this]() {
        GetTexts()[8]->SetText("...");
        GetTexts()[8]->SetPosition(mTextPos2 + Vector2(0, mButtonSize.y) * 0);
        mGame->RebindKeyboard(GetTexts()[8], Game::Action::MoveLeft);
    }
    , false, mButtonTextOffset);
    button->UseBackGroundColor();

    text = AddText(GetBindingText(Game::Action::MoveLeft), Vector2::Zero, 34);
    text->SetPosition(mTextPos2 + Vector2(0, mButtonSize.y) * 0);


    button = AddButton("DIREITA", mButtonPos2 + Vector2(0, mButtonSize.y * 1), mButtonSize, 34, UIButton::TextPos::AlignLeft,
    [this]() {
        GetTexts()[9]->SetText("...");
        GetTexts()[9]->SetPosition(mTextPos2 + Vector2(0, mButtonSize.y) * 1);
        mGame->RebindKeyboard(GetTexts()[9], Game::Action::MoveRight);
    }
    , false, mButtonTextOffset);
    button->UseBackGroundColor();

    text = AddText(GetBindingText(Game::Action::MoveRight), Vector2::Zero, 34);
    text->SetPosition(mTextPos2 + Vector2(0, mButtonSize.y) * 1);


    button = AddButton("CURA", mButtonPos2 + Vector2(0, mButtonSize.y * 2), mButtonSize, 34, UIButton::TextPos::AlignLeft,
    [this]() {
        GetTexts()[10]->SetText("...");
        GetTexts()[10]->SetPosition(mTextPos2 + Vector2(0, mButtonSize.y) * 2);
        mGame->RebindKeyboard(GetTexts()[10], Game::Action::Heal);
    }
    , false, mButtonTextOffset);
    button->UseBackGroundColor();

    text = AddText(GetBindingText(Game::Action::Heal), Vector2::Zero, 34);
    text->SetPosition(mTextPos2 + Vector2(0, mButtonSize.y) * 2);


    button = AddButton("GANCHO", mButtonPos2 + Vector2(0, mButtonSize.y * 3), mButtonSize, 34, UIButton::TextPos::AlignLeft,
    [this]() {
        GetTexts()[11]->SetText("...");
        GetTexts()[11]->SetPosition(mTextPos2 + Vector2(0, mButtonSize.y) * 3);
        mGame->RebindKeyboard(GetTexts()[11], Game::Action::Hook);
    }
    , false, mButtonTextOffset);
    button->UseBackGroundColor();

    text = AddText(GetBindingText(Game::Action::Hook), Vector2::Zero, 34);
    text->SetPosition(mTextPos2 + Vector2(0, mButtonSize.y) * 3);


    button = AddButton("ABRIR LOJA", mButtonPos2 + Vector2(0, mButtonSize.y * 4), mButtonSize, 34, UIButton::TextPos::AlignLeft,
    [this]() {
        GetTexts()[12]->SetText("...");
        GetTexts()[12]->SetPosition(mTextPos2 + Vector2(0, mButtonSize.y) * 4);
        mGame->RebindKeyboard(GetTexts()[12], Game::Action::OpenStore);
    }
    , false, mButtonTextOffset);
    button->UseBackGroundColor();

    text = AddText(GetBindingText(Game::Action::OpenStore), Vector2::Zero, 34);
    text->SetPosition(mTextPos2 + Vector2(0, mButtonSize.y) * 4);


    button = AddButton("MAPA", mButtonPos2 + Vector2(0, mButtonSize.y * 5), mButtonSize, 34, UIButton::TextPos::AlignLeft,
    [this]() {
        GetTexts()[13]->SetText("...");
        GetTexts()[13]->SetPosition(mTextPos2 + Vector2(0, mButtonSize.y) * 5);
        mGame->RebindKeyboard(GetTexts()[13], Game::Action::Map);
    }
    , false, mButtonTextOffset);
    button->UseBackGroundColor();

    text = AddText(GetBindingText(Game::Action::Map), Vector2::Zero, 34);
    text->SetPosition(mTextPos2 + Vector2(0, mButtonSize.y) * 5);


    button = AddButton("TROCAR MODO", mButtonPos2 + Vector2(0, mButtonSize.y * 6), mButtonSize, 34, UIButton::TextPos::AlignLeft,
    [this]() {
        GetTexts()[14]->SetText("...");
        GetTexts()[14]->SetPosition(mTextPos2 + Vector2(0, mButtonSize.y) * 6);
        mGame->RebindKeyboard(GetTexts()[14], Game::Action::ChangeMode);
    }
    , false, mButtonTextOffset);
    button->UseBackGroundColor();

    text = AddText(GetBindingText(Game::Action::ChangeMode), Vector2::Zero, 34);
    text->SetPosition(mTextPos2 + Vector2(0, mButtonSize.y) * 6);


    AddButton("VOLTAR AO PADRÃO", Vector2(788, 842), Vector2::Zero, 34, UIButton::TextPos::Center,
    [this]() {
        mGame->ResetKeyboardToDefault();
        RefreshTexts();
    }
    , true);

    AddButton("VOLTAR", Vector2(887, 930), Vector2::Zero, 34, UIButton::TextPos::Center,
    [this]() {
        Close();
    }
    , true);
}

std::string KeyboardMenu::GetBindingText(Game::Action action) const {
    auto binding = mGame->GetInputBinding()[action];

    if (binding.key != SDL_SCANCODE_UNKNOWN) {
        return SDL_GetScancodeName(binding.key);
    }
    else if (binding.mouseButton != 0) {
        switch(binding.mouseButton) {
            case SDL_BUTTON_LEFT: return "Mouse Esq";
            case SDL_BUTTON_RIGHT: return "Mouse Dir";
            case SDL_BUTTON_MIDDLE: return "Mouse Meio";
            case SDL_BUTTON_X1: return "Mouse X1";
            case SDL_BUTTON_X2: return "Mouse X2";
            default: return "Mouse " + std::to_string(binding.mouseButton);
        }
    }
    return "N/A";
}

void KeyboardMenu::Draw(class Renderer *renderer) {
    if (!mIsVisible) {
        return;
    }
    UIScreen::Draw(renderer);

    mGame->GetRenderer()->DrawLine(mButtonPos + Vector2(0, mButtonSize.y) * 0, mButtonPos + Vector2(mButtonSize.x, 0) + Vector2(0, mButtonSize.y) * 0, Vector3(0.5f, 0.5f, 0.5f), 2, Vector2::Zero, 1.0f);
    mGame->GetRenderer()->DrawLine(mButtonPos + Vector2(0, mButtonSize.y) * 1, mButtonPos + Vector2(mButtonSize.x, 0) + Vector2(0, mButtonSize.y) * 1, Vector3(0.5f, 0.5f, 0.5f), 2, Vector2::Zero, 1.0f);
    mGame->GetRenderer()->DrawLine(mButtonPos + Vector2(0, mButtonSize.y) * 2, mButtonPos + Vector2(mButtonSize.x, 0) + Vector2(0, mButtonSize.y) * 2, Vector3(0.5f, 0.5f, 0.5f), 2, Vector2::Zero, 1.0f);
    mGame->GetRenderer()->DrawLine(mButtonPos + Vector2(0, mButtonSize.y) * 3, mButtonPos + Vector2(mButtonSize.x, 0) + Vector2(0, mButtonSize.y) * 3, Vector3(0.5f, 0.5f, 0.5f), 2, Vector2::Zero, 1.0f);
    mGame->GetRenderer()->DrawLine(mButtonPos + Vector2(0, mButtonSize.y) * 4, mButtonPos + Vector2(mButtonSize.x, 0) + Vector2(0, mButtonSize.y) * 4, Vector3(0.5f, 0.5f, 0.5f), 2, Vector2::Zero, 1.0f);
    mGame->GetRenderer()->DrawLine(mButtonPos + Vector2(0, mButtonSize.y) * 5, mButtonPos + Vector2(mButtonSize.x, 0) + Vector2(0, mButtonSize.y) * 5, Vector3(0.5f, 0.5f, 0.5f), 2, Vector2::Zero, 1.0f);
    mGame->GetRenderer()->DrawLine(mButtonPos + Vector2(0, mButtonSize.y) * 6, mButtonPos + Vector2(mButtonSize.x, 0) + Vector2(0, mButtonSize.y) * 6, Vector3(0.5f, 0.5f, 0.5f), 2, Vector2::Zero, 1.0f);
    mGame->GetRenderer()->DrawLine(mButtonPos + Vector2(0, mButtonSize.y) * 7, mButtonPos + Vector2(mButtonSize.x, 0) + Vector2(0, mButtonSize.y) * 7, Vector3(0.5f, 0.5f, 0.5f), 2, Vector2::Zero, 1.0f);

    mGame->GetRenderer()->DrawLine(mButtonPos2 + Vector2(0, mButtonSize.y) * 0, mButtonPos2 + Vector2(mButtonSize.x, 0) + Vector2(0, mButtonSize.y) * 0, Vector3(0.5f, 0.5f, 0.5f), 2, Vector2::Zero, 1.0f);
    mGame->GetRenderer()->DrawLine(mButtonPos2 + Vector2(0, mButtonSize.y) * 1, mButtonPos2 + Vector2(mButtonSize.x, 0) + Vector2(0, mButtonSize.y) * 1, Vector3(0.5f, 0.5f, 0.5f), 2, Vector2::Zero, 1.0f);
    mGame->GetRenderer()->DrawLine(mButtonPos2 + Vector2(0, mButtonSize.y) * 2, mButtonPos2 + Vector2(mButtonSize.x, 0) + Vector2(0, mButtonSize.y) * 2, Vector3(0.5f, 0.5f, 0.5f), 2, Vector2::Zero, 1.0f);
    mGame->GetRenderer()->DrawLine(mButtonPos2 + Vector2(0, mButtonSize.y) * 3, mButtonPos2 + Vector2(mButtonSize.x, 0) + Vector2(0, mButtonSize.y) * 3, Vector3(0.5f, 0.5f, 0.5f), 2, Vector2::Zero, 1.0f);
    mGame->GetRenderer()->DrawLine(mButtonPos2 + Vector2(0, mButtonSize.y) * 4, mButtonPos2 + Vector2(mButtonSize.x, 0) + Vector2(0, mButtonSize.y) * 4, Vector3(0.5f, 0.5f, 0.5f), 2, Vector2::Zero, 1.0f);
    mGame->GetRenderer()->DrawLine(mButtonPos2 + Vector2(0, mButtonSize.y) * 5, mButtonPos2 + Vector2(mButtonSize.x, 0) + Vector2(0, mButtonSize.y) * 5, Vector3(0.5f, 0.5f, 0.5f), 2, Vector2::Zero, 1.0f);
    mGame->GetRenderer()->DrawLine(mButtonPos2 + Vector2(0, mButtonSize.y) * 6, mButtonPos2 + Vector2(mButtonSize.x, 0) + Vector2(0, mButtonSize.y) * 6, Vector3(0.5f, 0.5f, 0.5f), 2, Vector2::Zero, 1.0f);
    mGame->GetRenderer()->DrawLine(mButtonPos2 + Vector2(0, mButtonSize.y) * 7, mButtonPos2 + Vector2(mButtonSize.x, 0) + Vector2(0, mButtonSize.y) * 7, Vector3(0.5f, 0.5f, 0.5f), 2, Vector2::Zero, 1.0f);
}

void KeyboardMenu::RefreshTexts() {
    GetTexts()[1]->SetText(GetBindingText(Game::Action::Up));
    GetTexts()[2]->SetText(GetBindingText(Game::Action::Down));
    GetTexts()[3]->SetText(GetBindingText(Game::Action::Jump));
    GetTexts()[4]->SetText(GetBindingText(Game::Action::Attack));
    GetTexts()[5]->SetText(GetBindingText(Game::Action::Dash));
    GetTexts()[6]->SetText(GetBindingText(Game::Action::Skill1));
    GetTexts()[7]->SetText(GetBindingText(Game::Action::Skill2));
    GetTexts()[8]->SetText(GetBindingText(Game::Action::MoveLeft));
    GetTexts()[9]->SetText(GetBindingText(Game::Action::MoveRight));
    GetTexts()[10]->SetText(GetBindingText(Game::Action::Heal));
    GetTexts()[11]->SetText(GetBindingText(Game::Action::Hook));
    GetTexts()[12]->SetText(GetBindingText(Game::Action::OpenStore));
    GetTexts()[13]->SetText(GetBindingText(Game::Action::Map));
    GetTexts()[14]->SetText(GetBindingText(Game::Action::ChangeMode));
}
