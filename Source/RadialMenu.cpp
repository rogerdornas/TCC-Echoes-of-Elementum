//
// Created by roger on 05/12/2025.
//

#include "RadialMenu.h"
#include "Game.h"

RadialMenu::RadialMenu(class Game *game, const std::string &fontName, float radius)
    :UIScreen(game, fontName)
    ,mRadius(radius)
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

    float shiftedAngle = angle + (Math::PiOver2);
    if (shiftedAngle < 0) shiftedAngle += Math::TwoPi;

    // Divide pelo tamanho da fatia
    float sliceSize = (Math::TwoPi) / mButtons.size();

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
    // Lógica do Analógico (Radial)
    // Os valores de Axis do SDL geralmente vão de -32768 a 32767.
    // Vamos normalizar para -1.0 a 1.0
    float normX = 0.0f;
    float normY = 0.0f;

    // Ajuste esses valores constantes conforme sua classe Game ou use direto se já vier normalizado
    const float MAX_AXIS = 32767.0f;

    if (rightControllerAxisX != 0 || rightControllerAxisY != 0) {
        normX = rightControllerAxisX / MAX_AXIS;
        normY = rightControllerAxisY / MAX_AXIS;

        int newIndex = GetIndexFromInput(normX, normY);
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
    // Calcula vetor do centro da tela até o mouse
    Vector2 dir = virtualMousePos - mCenterPos;

    // Só atualiza se o mouse estiver a uma distância mínima do centro (ex: 50 pixels)
    // para evitar "jitter" quando o mouse está exatamente no meio.
    if (dir.LengthSq() > 2500.0f) { // 50^2
        int newIndex = GetIndexFromInput(dir.x, dir.y);
        if (newIndex != -1) {
            mSelectedButtonIndex = newIndex;
        }
    }

    // Atualiza Highlight
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
}

void RadialMenu::Draw(Renderer* renderer) {
    if (!mIsVisible) {
        return;
    }

    // Desenha os botões normalmente (a base já faz isso, mas se quiser ordem específica...)
    UIScreen::Draw(renderer);
}