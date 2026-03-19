//
// Created by roger on 17/03/2026.
//

#include "RemapControllerMenu.h"

#include "ControlMenu.h"

RemapControllerMenu::RemapControllerMenu(class Game *game, const std::string &fontName, bool isClosable)
    :UIScreen(game, fontName, isClosable)
{
    SetSize(Vector2(1920, 1080));
    SetPosition(Vector2::Zero);

    auto* background = AddImage("../Assets/Sprites/Menus/FundoPreto.png", Vector2(1920, 1080) * 0.5f, Vector2(1920, 1080));
    background->SetAlpha(0.6f);

    mButtonSize = Vector2(615, 58);
    mButtonPos = Vector2(321, 348);
    mButtonPos2 = Vector2(1013, 348);
    mButtonTextOffset = Vector2(10, 0);
    mTextPos = Vector2(835, 376);
    mTextPos2 = Vector2(1527, 376);

    auto* text = AddText("CONTROLE", Vector2::Zero,  34);
    text->SetPosition(Vector2(960, 290));

    AddImage("../Assets/Sprites/Menus/titulo_sublinhado.png", Vector2(960, 322), Vector2(360, 30));

    auto* button = AddButton("PULO", mButtonPos + Vector2(0, mButtonSize.y * 0), mButtonSize, 34, UIButton::TextPos::AlignLeft,
    [this]() {
        GetTexts()[1]->SetText("...");
        GetTexts()[1]->SetPosition(mTextPos + Vector2(0, mButtonSize.y) * 0);
        mGame->RebindController(GetTexts()[1], Game::Action::Jump);
    }
    , false, mButtonTextOffset);
    button->UseBackGroundColor();

    text = AddText(GetBindingText(Game::Action::Jump), Vector2::Zero, 34);
    text->SetPosition(mTextPos + Vector2(0, mButtonSize.y) * 0);


    button = AddButton("ATAQUE", mButtonPos + Vector2(0, mButtonSize.y * 1), mButtonSize, 34, UIButton::TextPos::AlignLeft,
    [this]() {
        GetTexts()[2]->SetText("...");
        GetTexts()[2]->SetPosition(mTextPos + Vector2(0, mButtonSize.y) * 1);
        mGame->RebindController(GetTexts()[2], Game::Action::Attack);
    }
    , false, mButtonTextOffset);
    button->UseBackGroundColor();

    text = AddText(GetBindingText(Game::Action::Attack), Vector2::Zero, 34);
    text->SetPosition(mTextPos + Vector2(0, mButtonSize.y) * 1);


    button = AddButton("AVANÇO", mButtonPos + Vector2(0, mButtonSize.y * 2), mButtonSize, 34, UIButton::TextPos::AlignLeft,
    [this]() {
        GetTexts()[3]->SetText("...");
        GetTexts()[3]->SetPosition(mTextPos + Vector2(0, mButtonSize.y) * 2);
        mGame->RebindController(GetTexts()[3], Game::Action::Dash);
    }
    , false, mButtonTextOffset);
    button->UseBackGroundColor();

    text = AddText(GetBindingText(Game::Action::Dash), Vector2::Zero, 34);
    text->SetPosition(mTextPos + Vector2(0, mButtonSize.y) * 2);


    button = AddButton("CURA", mButtonPos + Vector2(0, mButtonSize.y * 3), mButtonSize, 34, UIButton::TextPos::AlignLeft,
    [this]() {
        GetTexts()[4]->SetText("...");
        GetTexts()[4]->SetPosition(mTextPos + Vector2(0, mButtonSize.y) * 3);
        mGame->RebindController(GetTexts()[4], Game::Action::Heal);
    }
    , false, mButtonTextOffset);
    button->UseBackGroundColor();

    text = AddText(GetBindingText(Game::Action::Heal), Vector2::Zero, 34);
    text->SetPosition(mTextPos + Vector2(0, mButtonSize.y) * 3);


    button = AddButton("GANCHO", mButtonPos2 + Vector2(0, mButtonSize.y * 0), mButtonSize, 34, UIButton::TextPos::AlignLeft,
    [this]() {
        GetTexts()[5]->SetText("...");
        GetTexts()[5]->SetPosition(mTextPos2 + Vector2(0, mButtonSize.y) * 0);
        mGame->RebindController(GetTexts()[5], Game::Action::Hook);
    }
    , false, mButtonTextOffset);
    button->UseBackGroundColor();

    text = AddText(GetBindingText(Game::Action::Hook), Vector2::Zero, 34);
    text->SetPosition(mTextPos2 + Vector2(0, mButtonSize.y) * 0);


    button = AddButton("HABILIDADE 1", mButtonPos2 + Vector2(0, mButtonSize.y * 1), mButtonSize, 34, UIButton::TextPos::AlignLeft,
    [this]() {
        GetTexts()[6]->SetText("...");
        GetTexts()[6]->SetPosition(mTextPos2 + Vector2(0, mButtonSize.y) * 1);
        mGame->RebindController(GetTexts()[6], Game::Action::Skill1);
    }
    , false, mButtonTextOffset);
    button->UseBackGroundColor();

    text = AddText(GetBindingText(Game::Action::Skill1), Vector2::Zero, 34);
    text->SetPosition(mTextPos2 + Vector2(0, mButtonSize.y) * 1);


    button = AddButton("HABILIDADE 2", mButtonPos2 + Vector2(0, mButtonSize.y * 2), mButtonSize, 34, UIButton::TextPos::AlignLeft,
    [this]() {
        GetTexts()[7]->SetText("...");
        GetTexts()[7]->SetPosition(mTextPos2 + Vector2(0, mButtonSize.y) * 2);
        mGame->RebindController(GetTexts()[7], Game::Action::Skill2);
    }
    , false, mButtonTextOffset);
    button->UseBackGroundColor();

    text = AddText(GetBindingText(Game::Action::Skill2), Vector2::Zero, 34);
    text->SetPosition(mTextPos2 + Vector2(0, mButtonSize.y) * 2);


    button = AddButton("MAPA", mButtonPos2 + Vector2(0, mButtonSize.y * 3), mButtonSize, 34, UIButton::TextPos::AlignLeft,
    [this]() {
        GetTexts()[8]->SetText("...");
        GetTexts()[8]->SetPosition(mTextPos2 + Vector2(0, mButtonSize.y) * 3);
        mGame->RebindController(GetTexts()[8], Game::Action::Map);
    }
    , false, mButtonTextOffset);
    button->UseBackGroundColor();

    text = AddText(GetBindingText(Game::Action::Map), Vector2::Zero, 34);
    text->SetPosition(mTextPos2 + Vector2(0, mButtonSize.y) * 3);


    AddButton("VOLTAR AO PADRÃO", Vector2(788, 842), Vector2::Zero, 34, UIButton::TextPos::Center,
    [this]() {
        mGame->ResetControllerToDefault();
        RefreshTexts();
    }
    , true);

    AddButton("VOLTAR", Vector2(887, 930), Vector2::Zero, 34, UIButton::TextPos::Center,
    [this]() {
        mGame->CancelRebind();
        Close();
    }
    , true);
}

void RemapControllerMenu::Close() {
    UIScreen::Close();
    new ControlMenu(mGame, "../Assets/Fonts/K2D-Bold.ttf");
}

std::string RemapControllerMenu::GetBindingText(Game::Action action) const {
    auto binding = mGame->GetInputBinding()[action];

    if (binding.btn != SDL_CONTROLLER_BUTTON_INVALID) {
        const char* name = SDL_GameControllerGetStringForButton(binding.btn);
        return name ? std::string(name) : "N/A";
    }
    else if (binding.axis != SDL_CONTROLLER_AXIS_INVALID) {
        const char* name = SDL_GameControllerGetStringForAxis(binding.axis);
        return name ? std::string(name) : "N/A";
    }

    return "N/A";
}

void RemapControllerMenu::Draw(class Renderer *renderer) {
    if (!mIsVisible) {
        return;
    }
    UIScreen::Draw(renderer);

    mGame->GetRenderer()->DrawLine(mButtonPos + Vector2(0, mButtonSize.y) * 0, mButtonPos + Vector2(mButtonSize.x, 0) + Vector2(0, mButtonSize.y) * 0, Vector3(0.5f, 0.5f, 0.5f), 1, Vector2::Zero, 1.0f);
    mGame->GetRenderer()->DrawLine(mButtonPos + Vector2(0, mButtonSize.y) * 1, mButtonPos + Vector2(mButtonSize.x, 0) + Vector2(0, mButtonSize.y) * 1, Vector3(0.5f, 0.5f, 0.5f), 1, Vector2::Zero, 1.0f);
    mGame->GetRenderer()->DrawLine(mButtonPos + Vector2(0, mButtonSize.y) * 2, mButtonPos + Vector2(mButtonSize.x, 0) + Vector2(0, mButtonSize.y) * 2, Vector3(0.5f, 0.5f, 0.5f), 1, Vector2::Zero, 1.0f);
    mGame->GetRenderer()->DrawLine(mButtonPos + Vector2(0, mButtonSize.y) * 3, mButtonPos + Vector2(mButtonSize.x, 0) + Vector2(0, mButtonSize.y) * 3, Vector3(0.5f, 0.5f, 0.5f), 1, Vector2::Zero, 1.0f);
    mGame->GetRenderer()->DrawLine(mButtonPos + Vector2(0, mButtonSize.y) * 4, mButtonPos + Vector2(mButtonSize.x, 0) + Vector2(0, mButtonSize.y) * 4, Vector3(0.5f, 0.5f, 0.5f), 1, Vector2::Zero, 1.0f);

    mGame->GetRenderer()->DrawLine(mButtonPos2 + Vector2(0, mButtonSize.y) * 0, mButtonPos2 + Vector2(mButtonSize.x, 0) + Vector2(0, mButtonSize.y) * 0, Vector3(0.5f, 0.5f, 0.5f), 1, Vector2::Zero, 1.0f);
    mGame->GetRenderer()->DrawLine(mButtonPos2 + Vector2(0, mButtonSize.y) * 1, mButtonPos2 + Vector2(mButtonSize.x, 0) + Vector2(0, mButtonSize.y) * 1, Vector3(0.5f, 0.5f, 0.5f), 1, Vector2::Zero, 1.0f);
    mGame->GetRenderer()->DrawLine(mButtonPos2 + Vector2(0, mButtonSize.y) * 2, mButtonPos2 + Vector2(mButtonSize.x, 0) + Vector2(0, mButtonSize.y) * 2, Vector3(0.5f, 0.5f, 0.5f), 1, Vector2::Zero, 1.0f);
    mGame->GetRenderer()->DrawLine(mButtonPos2 + Vector2(0, mButtonSize.y) * 3, mButtonPos2 + Vector2(mButtonSize.x, 0) + Vector2(0, mButtonSize.y) * 3, Vector3(0.5f, 0.5f, 0.5f), 1, Vector2::Zero, 1.0f);
    mGame->GetRenderer()->DrawLine(mButtonPos2 + Vector2(0, mButtonSize.y) * 4, mButtonPos2 + Vector2(mButtonSize.x, 0) + Vector2(0, mButtonSize.y) * 4, Vector3(0.5f, 0.5f, 0.5f), 1, Vector2::Zero, 1.0f);
}

void RemapControllerMenu::RefreshTexts() {
    GetTexts()[1]->SetText(GetBindingText(Game::Action::Jump));
    GetTexts()[2]->SetText(GetBindingText(Game::Action::Attack));
    GetTexts()[3]->SetText(GetBindingText(Game::Action::Dash));
    GetTexts()[4]->SetText(GetBindingText(Game::Action::Heal));
    GetTexts()[5]->SetText(GetBindingText(Game::Action::Hook));
    GetTexts()[6]->SetText(GetBindingText(Game::Action::Skill1));
    GetTexts()[7]->SetText(GetBindingText(Game::Action::Skill2));
    GetTexts()[8]->SetText(GetBindingText(Game::Action::Map));
}
