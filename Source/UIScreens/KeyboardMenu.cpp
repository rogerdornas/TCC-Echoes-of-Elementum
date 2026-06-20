//
// Created by roger on 13/03/2026.
//

#include "KeyboardMenu.h"
#include "../UIElements/UIText.h"

KeyboardMenu::KeyboardMenu(class Game *game, const std::string &fontName, bool isClosable)
    :UIScreen(game, fontName, isClosable)
{
    SetSize(Vector2(1920, 1080));
    SetPosition(Vector2::Zero);

    auto* background = AddImage("../Assets/Sprites/Menus/FundoPreto.png", Vector2(1920, 1080) * 0.5f, Vector2(1920, 1080));
    background->SetAlpha(0.6f);

    mButtonSize = Vector2(615, 58);
    mButtonPos = Vector2(321, 380);
    mButtonPos2 = Vector2(1013, 380);
    mButtonTextOffset = Vector2(10, 0);
    mTextPos = Vector2(835, 408);
    mTextPos2 = Vector2(1527, 408);

    mIconFont = mGame->LoadFont("../Assets/Fonts/Buttons.ttf");

    auto* text = AddText("TECLADO", Vector2::Zero,  34);
    text->SetPosition(Vector2(960, 290));

    AddImage("../Assets/Sprites/Menus/titulo_sublinhado.png", Vector2(960, 322), Vector2(360, 30));

    // Função Lambda auxiliar para criar o texto com a fonte de ícones e manter a ordem do vetor
    auto AddIconText = [&](Game::Action action, Vector2 pos) {
        UIText* iconTxt = new UIText(mGame->GetIconStringForAction(action, true, false), mIconFont, 54, 1024, pos, Color::White);
        mTexts.push_back(iconTxt);
    };

    // COLUNA 1
    auto* button = AddButton("CIMA", mButtonPos + Vector2(0, mButtonSize.y * 0), mButtonSize, 34, UIButton::TextPos::AlignLeft,
    [this]() {
        GetTexts()[1]->SetFont(mFont);
        GetTexts()[1]->SetText("...");
        GetTexts()[1]->SetPosition(mTextPos + Vector2(0, mButtonSize.y) * 0);
        mGame->RebindKeyboard(GetTexts()[1], Game::Action::Up);
    }
    , false, mButtonTextOffset);
    button->UseBackGroundColor();
    AddIconText(Game::Action::Up, mTextPos + Vector2(0, mButtonSize.y) * 0);


    button = AddButton("ESQUERDA", mButtonPos + Vector2(0, mButtonSize.y * 1), mButtonSize, 34, UIButton::TextPos::AlignLeft,
    [this]() {
        GetTexts()[2]->SetFont(mFont);
        GetTexts()[2]->SetText("...");
        GetTexts()[2]->SetPosition(mTextPos + Vector2(0, mButtonSize.y) * 1);
        mGame->RebindKeyboard(GetTexts()[2], Game::Action::MoveLeft);
    }
    , false, mButtonTextOffset);
    button->UseBackGroundColor();
    AddIconText(Game::Action::MoveLeft, mTextPos + Vector2(0, mButtonSize.y) * 1);


    button = AddButton("BAIXO", mButtonPos + Vector2(0, mButtonSize.y * 2), mButtonSize, 34, UIButton::TextPos::AlignLeft,
    [this]() {
        GetTexts()[3]->SetFont(mFont);
        GetTexts()[3]->SetText("...");
        GetTexts()[3]->SetPosition(mTextPos + Vector2(0, mButtonSize.y) * 2);
        mGame->RebindKeyboard(GetTexts()[3], Game::Action::Down);
    }
    , false, mButtonTextOffset);
    button->UseBackGroundColor();
    AddIconText(Game::Action::Down, mTextPos + Vector2(0, mButtonSize.y) * 2);


    button = AddButton("DIREITA", mButtonPos + Vector2(0, mButtonSize.y * 3), mButtonSize, 34, UIButton::TextPos::AlignLeft,
    [this]() {
        GetTexts()[4]->SetFont(mFont);
        GetTexts()[4]->SetText("...");
        GetTexts()[4]->SetPosition(mTextPos + Vector2(0, mButtonSize.y) * 3);
        mGame->RebindKeyboard(GetTexts()[4], Game::Action::MoveRight);
    }
    , false, mButtonTextOffset);
    button->UseBackGroundColor();
    AddIconText(Game::Action::MoveRight, mTextPos + Vector2(0, mButtonSize.y) * 3);


    button = AddButton("PULO", mButtonPos + Vector2(0, mButtonSize.y * 4), mButtonSize, 34, UIButton::TextPos::AlignLeft,
    [this]() {
        GetTexts()[5]->SetFont(mFont);
        GetTexts()[5]->SetText("...");
        GetTexts()[5]->SetPosition(mTextPos + Vector2(0, mButtonSize.y) * 4);
        mGame->RebindKeyboard(GetTexts()[5], Game::Action::Jump);
    }
    , false, mButtonTextOffset);
    button->UseBackGroundColor();
    AddIconText(Game::Action::Jump, mTextPos + Vector2(0, mButtonSize.y) * 4);


    button = AddButton("ATAQUE", mButtonPos + Vector2(0, mButtonSize.y * 5), mButtonSize, 34, UIButton::TextPos::AlignLeft,
    [this]() {
        GetTexts()[6]->SetFont(mFont);
        GetTexts()[6]->SetText("...");
        GetTexts()[6]->SetPosition(mTextPos + Vector2(0, mButtonSize.y) * 5);
        mGame->RebindKeyboard(GetTexts()[6], Game::Action::Attack);
    }
    , false, mButtonTextOffset);
    button->UseBackGroundColor();
    AddIconText(Game::Action::Attack, mTextPos + Vector2(0, mButtonSize.y) * 5);


    button = AddButton("CURA", mButtonPos + Vector2(0, mButtonSize.y * 6), mButtonSize, 34, UIButton::TextPos::AlignLeft,
    [this]() {
        GetTexts()[7]->SetFont(mFont);
        GetTexts()[7]->SetText("...");
        GetTexts()[7]->SetPosition(mTextPos + Vector2(0, mButtonSize.y) * 6);
        mGame->RebindKeyboard(GetTexts()[7], Game::Action::Heal);
    }
    , false, mButtonTextOffset);
    button->UseBackGroundColor();
    AddIconText(Game::Action::Heal, mTextPos + Vector2(0, mButtonSize.y) * 6);


    // COLUNA 2
    button = AddButton("AVANÇO", mButtonPos2 + Vector2(0, mButtonSize.y * 0), mButtonSize, 34, UIButton::TextPos::AlignLeft,
    [this]() {
        GetTexts()[8]->SetFont(mFont);
        GetTexts()[8]->SetText("...");
        GetTexts()[8]->SetPosition(mTextPos2 + Vector2(0, mButtonSize.y) * 0);
        mGame->RebindKeyboard(GetTexts()[8], Game::Action::Dash);
    }
    , false, mButtonTextOffset);
    button->UseBackGroundColor();
    AddIconText(Game::Action::Dash, mTextPos2 + Vector2(0, mButtonSize.y) * 0);


    button = AddButton("GANCHO", mButtonPos2 + Vector2(0, mButtonSize.y * 1), mButtonSize, 34, UIButton::TextPos::AlignLeft,
    [this]() {
        GetTexts()[9]->SetFont(mFont);
        GetTexts()[9]->SetText("...");
        GetTexts()[9]->SetPosition(mTextPos2 + Vector2(0, mButtonSize.y) * 1);
        mGame->RebindKeyboard(GetTexts()[9], Game::Action::Hook);
    }
    , false, mButtonTextOffset);
    button->UseBackGroundColor();
    AddIconText(Game::Action::Hook, mTextPos2 + Vector2(0, mButtonSize.y) * 1);


    button = AddButton("HABILIDADE 1", mButtonPos2 + Vector2(0, mButtonSize.y * 2), mButtonSize, 34, UIButton::TextPos::AlignLeft,
    [this]() {
        GetTexts()[10]->SetFont(mFont);
        GetTexts()[10]->SetText("...");
        GetTexts()[10]->SetPosition(mTextPos2 + Vector2(0, mButtonSize.y) * 2);
        mGame->RebindKeyboard(GetTexts()[10], Game::Action::Skill1);
    }
    , false, mButtonTextOffset);
    button->UseBackGroundColor();
    AddIconText(Game::Action::Skill1, mTextPos2 + Vector2(0, mButtonSize.y) * 2);


    button = AddButton("HABILIDADE 2", mButtonPos2 + Vector2(0, mButtonSize.y * 3), mButtonSize, 34, UIButton::TextPos::AlignLeft,
    [this]() {
        GetTexts()[11]->SetFont(mFont);
        GetTexts()[11]->SetText("...");
        GetTexts()[11]->SetPosition(mTextPos2 + Vector2(0, mButtonSize.y) * 3);
        mGame->RebindKeyboard(GetTexts()[11], Game::Action::Skill2);
    }
    , false, mButtonTextOffset);
    button->UseBackGroundColor();
    AddIconText(Game::Action::Skill2, mTextPos2 + Vector2(0, mButtonSize.y) * 3);


    button = AddButton("MAPA", mButtonPos2 + Vector2(0, mButtonSize.y * 4), mButtonSize, 34, UIButton::TextPos::AlignLeft,
    [this]() {
        GetTexts()[12]->SetFont(mFont);
        GetTexts()[12]->SetText("...");
        GetTexts()[12]->SetPosition(mTextPos2 + Vector2(0, mButtonSize.y) * 4);
        mGame->RebindKeyboard(GetTexts()[12], Game::Action::Map);
    }
    , false, mButtonTextOffset);
    button->UseBackGroundColor();
    AddIconText(Game::Action::Map, mTextPos2 + Vector2(0, mButtonSize.y) * 4);


    button = AddButton("DESCANSAR", mButtonPos2 + Vector2(0, mButtonSize.y * 5), mButtonSize, 34, UIButton::TextPos::AlignLeft,
    [this]() {
        GetTexts()[13]->SetFont(mFont);
        GetTexts()[13]->SetText("...");
        GetTexts()[13]->SetPosition(mTextPos2 + Vector2(0, mButtonSize.y) * 5);
        mGame->RebindKeyboard(GetTexts()[13], Game::Action::OpenStore);
    }
    , false, mButtonTextOffset);
    button->UseBackGroundColor();
    AddIconText(Game::Action::OpenStore, mTextPos2 + Vector2(0, mButtonSize.y) * 5);


    button = AddButton("TROCAR MODO", mButtonPos2 + Vector2(0, mButtonSize.y * 6), mButtonSize, 34, UIButton::TextPos::AlignLeft,
    [this]() {
        GetTexts()[14]->SetFont(mFont);
        GetTexts()[14]->SetText("...");
        GetTexts()[14]->SetPosition(mTextPos2 + Vector2(0, mButtonSize.y) * 6);
        mGame->RebindKeyboard(GetTexts()[14], Game::Action::ChangeMode);
    }
    , false, mButtonTextOffset);
    button->UseBackGroundColor();
    AddIconText(Game::Action::ChangeMode, mTextPos2 + Vector2(0, mButtonSize.y) * 6);


    AddButton("VOLTAR AO PADRÃO (TECLADO)", Vector2(725, 815), Vector2::Zero, 30, UIButton::TextPos::Center,
    [this]() {
        mGame->ResetKeyboardToDefault();
        RefreshTexts();
    }
    , true);

    AddButton("VOLTAR AO PADRÃO (TECLADO E MOUSE)", Vector2(655, 872), Vector2::Zero, 30, UIButton::TextPos::Center,
    [this]() {
        mGame->ResetKeyboardAndMouseToDefault();
        RefreshTexts();
    }
    , true);

    AddButton("VOLTAR", Vector2(887, 930), Vector2::Zero, 34, UIButton::TextPos::Center,
    [this]() {
        Close();
    }
    , true);

    text = AddText("Clique nos comandos para alterar", Vector2::Zero,  24);
    text->SetPosition(Vector2(960, 348));
    text->SetColor(Vector3(0.5f, 0.5f, 0.5f));
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
    GetTexts()[1]->SetFont(mIconFont);
    GetTexts()[1]->SetText(mGame->GetIconStringForAction(Game::Action::Up, true, false));

    GetTexts()[2]->SetFont(mIconFont);
    GetTexts()[2]->SetText(mGame->GetIconStringForAction(Game::Action::MoveLeft, true, false));

    GetTexts()[3]->SetFont(mIconFont);
    GetTexts()[3]->SetText(mGame->GetIconStringForAction(Game::Action::Down, true, false));

    GetTexts()[4]->SetFont(mIconFont);
    GetTexts()[4]->SetText(mGame->GetIconStringForAction(Game::Action::MoveRight, true, false));

    GetTexts()[5]->SetFont(mIconFont);
    GetTexts()[5]->SetText(mGame->GetIconStringForAction(Game::Action::Jump, true, false));

    GetTexts()[6]->SetFont(mIconFont);
    GetTexts()[6]->SetText(mGame->GetIconStringForAction(Game::Action::Attack, true, false));

    GetTexts()[7]->SetFont(mIconFont);
    GetTexts()[7]->SetText(mGame->GetIconStringForAction(Game::Action::Heal, true, false));

    GetTexts()[8]->SetFont(mIconFont);
    GetTexts()[8]->SetText(mGame->GetIconStringForAction(Game::Action::Dash, true, false));

    GetTexts()[9]->SetFont(mIconFont);
    GetTexts()[9]->SetText(mGame->GetIconStringForAction(Game::Action::Hook, true, false));

    GetTexts()[10]->SetFont(mIconFont);
    GetTexts()[10]->SetText(mGame->GetIconStringForAction(Game::Action::Skill1, true, false));

    GetTexts()[11]->SetFont(mIconFont);
    GetTexts()[11]->SetText(mGame->GetIconStringForAction(Game::Action::Skill2, true, false));

    GetTexts()[12]->SetFont(mIconFont);
    GetTexts()[12]->SetText(mGame->GetIconStringForAction(Game::Action::Map, true, false));

    GetTexts()[13]->SetFont(mIconFont);
    GetTexts()[13]->SetText(mGame->GetIconStringForAction(Game::Action::OpenStore, true, false));

    GetTexts()[14]->SetFont(mIconFont);
    GetTexts()[14]->SetText(mGame->GetIconStringForAction(Game::Action::ChangeMode, true, false));
}
