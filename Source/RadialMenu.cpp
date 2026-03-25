//
// Created by roger on 05/12/2025.
//

#include "RadialMenu.h"
#include "Game.h"

RadialMenu::RadialMenu(class Game *game, const std::string &fontName, float radius)
    :UIScreen(game, fontName)
    ,mRadius(radius)
    ,mLastMousePos(Vector2::Zero)
    ,mMouseVirtualStick(Vector2::Zero)
    ,mFirstMouseUpdate(true)
{
    float virtualWidth = mGame->GetRenderer()->GetVirtualWidth();
    float virtualHeight = mGame->GetRenderer()->GetVirtualHeight();
    mCenterPos = Vector2(virtualWidth / 2.0f, virtualHeight / 2.0f);
    SetSize(Vector2(virtualWidth, virtualHeight));
    SetPosition(Vector2::Zero);
}

RadialMenu::~RadialMenu() {

}

void RadialMenu::AddRadialOption(const std::string& name, std::function<void()> onClick, const std::string& iconPath) {
    AddButton(name, Vector2::Zero, Vector2(100, 50), 30, UIButton::TextPos::Center, onClick);

    RearrangeButtons();
}

void RadialMenu::RearrangeButtons() {
    if (mButtons.empty()) return;

    float angleStep = (Math::TwoPi) / mButtons.size();
    // Começa em -PI/2 (Topo / 12 horas)
    float currentAngle = -Math::PiOver2;

    for (UIButton* btn : mButtons) {
        // Calcula posição polar
        float x = mCenterPos.x + std::cos(currentAngle) * mRadius;
        float y = mCenterPos.y + std::sin(currentAngle) * mRadius;

        // Centraliza o botão na coordenada calculada (subtrai metade do tamanho)
        Vector2 btnSize = btn->GetSize();
        btn->SetPosition(Vector2(x - btnSize.x / 2.0f, y - btnSize.y / 2.0f));

        currentAngle += angleStep;
    }
}

int RadialMenu::GetIndexFromInput(float x, float y) {
    if (mButtons.empty()) return -1;

    // Zona morta para evitar seleção acidental no centro
    const float deadzone = 0.2f;

    if (std::abs(x) < deadzone && std::abs(y) < deadzone) {
        return mSelectedButtonIndex; // Mantém o anterior se estiver no meio
    }

    // Calcula o ângulo do input: atan2 retorna entre -PI e +PI
    float angle = std::atan2(y, x);

    float shiftedAngle = angle + Math::PiOver2;
    if (shiftedAngle < 0) shiftedAngle += Math::TwoPi;

    // Divide pelo tamanho da fatia
    float sliceSize = Math::TwoPi / mButtons.size();

    // Opcional: Rotacionar meia fatia para trás para que o ícone fique no CENTRO da fatia de seleção
    // em vez de a fatia começar no ícone.
    shiftedAngle += sliceSize / 2.0f;
    if (shiftedAngle > Math::TwoPi) shiftedAngle -= Math::TwoPi;

    int index = static_cast<int>(shiftedAngle / sliceSize);

    // Garante bounds
    if (index >= mButtons.size()) index = 0;

    return index;
}

void RadialMenu::HandleKeyPress(int key, int controllerButton, int leftControllerAxisY, int leftControllerAxisX, int rightControllerAxisY, int rightControllerAxisX) {
    if (mButtons.empty()) {
        return;
    }

    auto inputBinding = mGame->GetInputBinding();
    float inputX = 0.0f;
    float inputY = 0.0f;
    bool hasDirectionalInput = false;

    // Lógica do Analógico
    const float MAX_AXIS = 32767.0f;
    if (rightControllerAxisX != 0 || rightControllerAxisY != 0) {
        inputX = rightControllerAxisX / MAX_AXIS;
        inputY = rightControllerAxisY / MAX_AXIS;
        hasDirectionalInput = true;
    }
    else {
        if (key == SDLK_UP ||
            key == SDL_GetKeyFromScancode(inputBinding[Game::Action::Up].key))
        {
            inputY = -1.0f;
            hasDirectionalInput = true;
        }
        else if (key == SDLK_DOWN ||
                 key == SDL_GetKeyFromScancode(inputBinding[Game::Action::Down].key))
        {
            inputY = 1.0f;
            hasDirectionalInput = true;
        }
        else if (key == SDLK_LEFT ||
                 key == SDL_GetKeyFromScancode(inputBinding[Game::Action::MoveLeft].key))
        {
            inputX = -1.0f;
            hasDirectionalInput = true;
        }
        else if (key == SDLK_RIGHT ||
         key == SDL_GetKeyFromScancode(inputBinding[Game::Action::MoveRight].key))
        {
            inputX = 1.0f;
            hasDirectionalInput = true;
        }
    }
    if (hasDirectionalInput) {
        int newIndex = GetIndexFromInput(inputX, inputY);
        if (newIndex != -1) {
            mSelectedButtonIndex = newIndex;
        }
    }

    // Atualiza Highlight visual
    for (size_t i = 0; i < mButtons.size(); ++i) {
        mButtons[i]->SetHighlighted(static_cast<int>(i) == mSelectedButtonIndex);
    }
}

void RadialMenu::HandleMouseMotion(const Vector2& virtualMousePos) {
    if (mFirstMouseUpdate) {
        mLastMousePos = virtualMousePos;
        mFirstMouseUpdate = false;
        return;
    }

    // Calcula o quanto o mouse se moveu (Delta)
    Vector2 delta = virtualMousePos - mLastMousePos;
    mLastMousePos = virtualMousePos;

    // Fator de sensibilidade
    float sensitivity = 1.0f;

    // Empurra o nosso "analógico virtual" na direção do movimento
    mMouseVirtualStick += delta * sensitivity;

    // Limita o tamanho desse vetor imaginário.
    float maxStickRadius = 150.0f;
    if (mMouseVirtualStick.LengthSq() > (maxStickRadius * maxStickRadius)) {
        mMouseVirtualStick.Normalize();
        mMouseVirtualStick *= maxStickRadius;
    }

    // Verifica se saiu da zona morta para atualizar a seleção
    if (mMouseVirtualStick.LengthSq() > 2500.0f) {
        int newIndex = GetIndexFromInput(mMouseVirtualStick.x, mMouseVirtualStick.y);
        if (newIndex != -1) {
            mSelectedButtonIndex = newIndex;
        }
    }

    // Atualiza Highlight visual
    for (size_t i = 0; i < mButtons.size(); ++i) {
        mButtons[i]->SetHighlighted(static_cast<int>(i) == mSelectedButtonIndex);
    }
}

void RadialMenu::SetSelectedOption(int index) {
    // Validação básica para não quebrar o jogo
    if (index >= 0 && index < static_cast<int>(mButtons.size())) {
        mSelectedButtonIndex = index;

        // Atualiza o visual de todos os botões
        for (size_t i = 0; i < mButtons.size(); ++i) {
            // true apenas para o botão que bate com o índice
            mButtons[i]->SetHighlighted(static_cast<int>(i) == mSelectedButtonIndex);
        }
    }
}

void RadialMenu::Close() {
    mButtons[mSelectedButtonIndex]->OnClick();
    mState = UIState::Closing;
    mGame->SetIsSlowMotion(false);
}

void RadialMenu::Draw(Renderer* renderer) {
    if (!mIsVisible) {
        return;
    }

    // Desenha os botões normalmente (a base já faz isso, mas se quiser ordem específica...)
    UIScreen::Draw(renderer);
}