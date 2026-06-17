//
// Created by roger on 14/06/2026.
//

#include "NewSkillMenu.h"

NewSkillMenu::NewSkillMenu(class Game *game, const std::string &fontName, bool isClosable, Skill::SkillType skillType)
    :UIScreen(game, fontName, isClosable)
{
    mGame->SaveGame();
    mGame->TogglePause();

    SetSize(Vector2(1920, 1080));
    SetPosition(Vector2::Zero);

    mIconFont = mGame->LoadFont("../Assets/Fonts/Buttons.ttf");

    auto* background = AddImage("../Assets/Sprites/Menus/FundoPretoArredondado.png", Vector2(960, 495), Vector2(730, 330));
    background->SetAlpha(0.3f);

    auto* text = AddText("NOVA HABILIDADE ADQUIRIDA!", Vector2::Zero,  34);
    text->SetPosition(Vector2(960, 375));

    AddImage("../Assets/Sprites/Menus/titulo_sublinhado.png", Vector2(960, 407), Vector2(360, 30));

    AddButton("CONFIRMAR", Vector2(856, 600), Vector2::Zero, 34, UIButton::TextPos::Center,
    [this]() {
        Close();
    }
    , true);

    mMessageTemplate = "";
    mMessageNeedsRebuild = true;
    mLastInputMode = mGame->GetInputPlayerMode();
    mLastController = mGame->GetInputController();

    mIconFont = mGame->LoadFont("../Assets/Fonts/Buttons.ttf");

    for (int i = 0; i < MAX_MESSAGE_PARTS; i++) {
        UIText* t = AddText("", Vector2::Zero, 34);
        t->SetIsVisible(false);
        mMessageTexts.push_back(t);

        UIText* iconTxt = new UIText("", mIconFont, 34 + 10, 1024, Vector2::Zero, Color::White);
        iconTxt->SetIsVisible(false);

        mTexts.push_back(iconTxt);
        mMessageActionTexts.push_back(iconTxt);
    }

    switch (skillType) {
        case Skill::SkillType::Dash:
            mMessageTemplate = "PRESSIONE <Dash> PARA USAR AVANÇO RÁPIDO";
            break;

        case Skill::SkillType::Hook:
            mMessageTemplate = "PRESSIONE <Hook> PARA USAR GANCHO";
            break;

        case Skill::SkillType::WallSlide:
            mMessageTemplate = "AGORA VOCÊ PODE SE AGARRAR NA PAREDE";
            break;

        case Skill::SkillType::DoubleJump:
            mMessageTemplate = "GANHOU UM PULO EXTRA NO AR";
            break;

        case Skill::SkillType::TimeControl:
            break;
    }

}

void NewSkillMenu::Update(float deltaTime) {
    Game::InputPlayerMode currentMode = mGame->GetInputPlayerMode();
    Game::InputController currentController = mGame->GetInputController();

    // Reconstrói as imagens se o player trocou de controle para teclado, se trocou de controles, ou se for um tutorial novo
    if (mLastInputMode != currentMode || mLastController != currentController || mMessageNeedsRebuild) {
        mLastInputMode = currentMode;
        mLastController = currentController;
        RebuildMessage();
        mMessageNeedsRebuild = false;
    }
}

void NewSkillMenu::RebuildMessage() {
    for (auto* t : mMessageTexts) t->SetIsVisible(false);
    for (auto* i : mMessageActionTexts) i->SetIsVisible(false);

    if (mMessageTemplate.empty()) return;

    int activeTexts = 0;
    int activeIcons = 0;
    std::string str = mMessageTemplate;
    size_t pos = 0;

    // Vetor temporário para guardar a ordem dos elementos que vamos desenhar
    std::vector<UIElement*> layoutElements;

    // Faz o Parse da string
    while (pos < str.length()) {
        size_t tagStart = str.find("<", pos);

        // Se não tem mais tags, pega o resto do texto
        if (tagStart == std::string::npos) {
            std::string textPart = str.substr(pos);
            if (!textPart.empty() && activeTexts < MAX_MESSAGE_PARTS) {
                UIText* t = mMessageTexts[activeTexts++];
                t->SetText(textPart);
                t->SetIsVisible(true);
                layoutElements.push_back(t);
            }
            break;
        }

        // Adiciona o texto ANTES da tag
        if (tagStart > pos) {
            std::string textPart = str.substr(pos, tagStart - pos);
            if (activeTexts < MAX_MESSAGE_PARTS) {
                UIText* t = mMessageTexts[activeTexts++];
                t->SetText(textPart);
                t->SetIsVisible(true);
                layoutElements.push_back(t);
            }
        }

        // Processa a TAG <Acao>
        size_t tagEnd = str.find(">", tagStart);

        // Tag mal formatada
        if (tagEnd == std::string::npos) {
            break;
        }

        std::string actionStr = str.substr(tagStart + 1, tagEnd - tagStart - 1);
        Game::Action action = mGame->StringToAction(actionStr);

        if (action != Game::Action::Invalid && activeIcons < MAX_MESSAGE_PARTS) {
            std::string iconString = mGame->GetIconStringForAction(action);
            UIText* iconTxt = mMessageActionTexts[activeIcons++];
            iconTxt->SetText(iconString);
            iconTxt->SetIsVisible(true);

            layoutElements.push_back(iconTxt);
        }

        pos = tagEnd + 1;
    }

    // Posiciona os elementos lado a lado
    float padding = 0.0f; // Espaço entre textos e imagens
    float totalWidth = 0.0f;

    // Calcula a largura total
    for (auto* elem : layoutElements) {
        totalWidth += elem->GetSize().x + padding;
    }
    totalWidth -= padding; // Remove o padding extra do final

    // Calcula a posição inicial (X) e a altura (Y)
    float startX = (mGame->GetRenderer()->GetVirtualWidth() - totalWidth) / 2.0f;
    float centerY = mGame->GetRenderer()->GetVirtualHeight() * 0.45f;

    float currentX = startX;
    for (auto* elem : layoutElements) {
        float halfWidth = elem->GetSize().x / 2.0f;
        elem->SetPosition(Vector2(currentX + halfWidth, centerY));

        currentX += elem->GetSize().x + padding;
    }
}

void NewSkillMenu::Close() {
    UIScreen::Close();
    mGame->SaveGame();
    mGame->TogglePause();
}
