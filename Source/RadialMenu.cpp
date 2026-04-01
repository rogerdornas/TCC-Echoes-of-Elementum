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
    ,mCurrentSelectorAngle(0.0f)
{
    float virtualWidth = mGame->GetRenderer()->GetVirtualWidth();
    float virtualHeight = mGame->GetRenderer()->GetVirtualHeight();
    mCenterPos = Vector2(virtualWidth / 2.0f, virtualHeight / 2.0f);
    SetSize(Vector2(virtualWidth, virtualHeight));
    SetPosition(Vector2::Zero);

    mRing = AddImage("../Assets/Sprites/HUD/Ring4.png", mCenterPos, Vector2(600, 600));
    mRing->SetAlpha(0.3f);

    mRingArc = AddImage("../Assets/Sprites/HUD/RingArc4.png", mCenterPos, Vector2(600, 600));
    mRingArc->SetAlpha(0.3f);

    mInsiderCircle = AddImage("../Assets/Sprites/HUD/InsiderCircle3.png", mCenterPos, Vector2(600, 600));

    mTriangleSelector = AddImage("../Assets/Sprites/HUD/Triangle3.png", mCenterPos, Vector2(600, 600));

    mSlowMotionBarPos = mCenterPos;
    mSlowMotionBarRadius = 120.0f;
    mSlowMotionBarThickness = 25.0f;

    AddImage("../Assets/Sprites/HUD/Fire.png", Vector2(960, 310), Vector2(73, 92));

    AddImage("../Assets/Sprites/HUD/Lightning.png", Vector2(1185, 540), Vector2(77, 99));

    AddImage("../Assets/Sprites/HUD/Ice.png", Vector2(732, 540), Vector2(81, 103));

    AddImage("../Assets/Sprites/HUD/Earth.png", Vector2(960, 765), Vector2(87, 110));
}

RadialMenu::~RadialMenu() {

}

void RadialMenu::Update(float deltaTime) {
    if (!mIsVisible || mButtons.empty()) return;

    float inputX = mGame->GetRightAxisX() / 32767.0f;
    float inputY = mGame->GetRightAxisY() / 32767.0f;

    float magnitudeSq = (inputX * inputX) + (inputY * inputY);
    const float DEADZONE_SQ = 0.2f * 0.2f;

    if (magnitudeSq > DEADZONE_SQ) {
        float targetAngle = std::atan2(inputY, inputX);
        int newIndex = GetIndexFromInput(inputX, inputY);

        // Atualiza a opção logicamente sem forçar o visual (snapVisuals = false)
        if (newIndex != -1 && newIndex != mSelectedButtonIndex) {
            SetSelectedOption(newIndex, false);
        }

        // 1. Calcula a diferença de ângulo primeiro
        float diff = targetAngle - mCurrentSelectorAngle;
        while (diff <= -Math::Pi) diff += Math::TwoPi;
        while (diff > Math::Pi) diff -= Math::TwoPi;

        // 2. LERP DINÂMICO BASEADO EM DISTÂNCIA
        // Math::PiOver4 é 45 graus (metade de uma fatia).
        // Se a diferença for maior que isso, o analógico foi jogado para longe. Acelera!
        // Se for menor, você está só fazendo micro-ajustes dentro da opção. Fica suave.
        float lerpSpeed = (std::abs(diff) > Math::PiOver2 * 0.5f) ? 80.0f : 10.0f;

        // Tolerância para não tremer
        if (std::abs(diff) > 0.01f) {
            if (mGame->IsSlowMotion()) {
                mCurrentSelectorAngle += diff * lerpSpeed * deltaTime * 3.33f;
            }
            else {
                mCurrentSelectorAngle += diff * lerpSpeed * deltaTime;
            }
            mTriangleSelector->SetRotation(mCurrentSelectorAngle);
        }
    }
}

void RadialMenu::AddRadialOption(const std::string& name, std::function<void()> onClick, const std::string& iconPath) {
    auto* button = AddButton(name, Vector2::Zero, Vector2(100, 50), 30, UIButton::TextPos::Center, onClick);
    button->RemoveImageSelector();

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

    if (hasDirectionalInput) {
        int newIndex = GetIndexFromInput(inputX, inputY);
        if (newIndex != -1 && newIndex != mSelectedButtonIndex) {
            SetSelectedOption(newIndex, true); // true = teleporta o triângulo pro centro da fatia!
        }

        float rawAngle = std::atan2(inputY, inputX);
        mCurrentSelectorAngle = rawAngle;
        mTriangleSelector->SetRotation(rawAngle);
    }

    float sliceSize = Math::TwoPi / mButtons.size(); // Para 4 botões, isso é Pi/2 (90 graus)
    float arcAngle = (mSelectedButtonIndex * sliceSize) - Math::PiOver2;
    mRingArc->SetRotation(arcAngle);

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

        float rawAngle = std::atan2(mMouseVirtualStick.y, mMouseVirtualStick.x);
        mCurrentSelectorAngle = rawAngle;
        mTriangleSelector->SetRotation(rawAngle);
    }

    float sliceSize = Math::TwoPi / mButtons.size();
    float arcAngle = (mSelectedButtonIndex * sliceSize) - Math::PiOver2;
    mRingArc->SetRotation(arcAngle);

    // Atualiza Highlight visual
    for (size_t i = 0; i < mButtons.size(); ++i) {
        mButtons[i]->SetHighlighted(static_cast<int>(i) == mSelectedButtonIndex);
    }
}

void RadialMenu::SetSelectedOption(int index, bool snapVisuals) {
    // Validação básica para não quebrar o jogo
    if (index >= 0 && index < static_cast<int>(mButtons.size())) {
        mSelectedButtonIndex = index;

        // Atualiza o visual de todos os botões
        for (size_t i = 0; i < mButtons.size(); ++i) {
            // true apenas para o botão que bate com o índice
            mButtons[i]->SetHighlighted(static_cast<int>(i) == mSelectedButtonIndex);
        }

        float sliceSize = Math::TwoPi / mButtons.size();
        float arcAngle = (mSelectedButtonIndex * sliceSize) - Math::PiOver2;

        mRingArc->SetRotation(arcAngle);
        // Como não há direcional ativo, centralizamos o seletor no meio da fatia também
        if (snapVisuals) {
            mCurrentSelectorAngle = arcAngle; // Sincroniza o estado interno!
            mTriangleSelector->SetRotation(mCurrentSelectorAngle);
        }
    }
}

void RadialMenu::Close() {
    mButtons[mSelectedButtonIndex]->OnClick();
    mState = UIState::Closing;
    mGame->SetIsSlowMotion(false);
}

void RadialMenu::Draw(Renderer* renderer) {
    UIScreen::Draw(renderer);

    // DrawSlowMotionBar(renderer);
}

void RadialMenu::DrawSlowMotionBar(Renderer* renderer) {
    auto* player = mGame->GetPlayer();
    float timer = player->GetRadialMenuSlowMotionTimer();
    float duration = player->GetRadialMenuSlowMotionDuration();
    bool isCharging = player->IsSlowMotionCharging();

    if (duration <= 0.0f) return;

    // Calcula a porcentagem de preenchimento (0.0f a 1.0f)
    float ratio = timer / duration;
    ratio = std::clamp(ratio, 0.0f, 1.0f);

    // Configurações visuais do círculo
    int numSegments = 128;
    float angleStep = Math::TwoPi / numSegments;

    // Cores
    Vector3 bgColor = Vector3(40 / 255.0f, 40 / 255.0f, 40 / 255.0f); // Fundo escuro

    // LÓGICA DE CORES E OPACIDADE
    Vector3 fillColor;
    float alpha = 1.0f;

    if (isCharging) {
        // ESTADO BLOQUEADO: Fica vermelho
        fillColor = Vector3(242 / 255.0f, 90 / 255.0f, 70 / 255.0f);

        // Faz a opacidade pulsar usando o tempo do jogo ou do sistema
        float time = SDL_GetTicks() / 60.0f;
        alpha = 0.5f + (std::sin(time) * 0.5f); // Varia suavemente entre 0.0 e 1.0

        // Evita que fique 100% invisível
        if (alpha < 0.2f) {
            alpha = 0.2f;
        }
    }
    else {
        // ESTADO PRONTO/NORMAL: Ciano
        fillColor = Vector3(65 / 255.0f, 217 / 255.0f, 188 / 255.0f);
        alpha = 1.0f; // Opacidade máxima
    }

    renderer->DrawCircularBar(mSlowMotionBarPos, mSlowMotionBarRadius, mSlowMotionBarThickness, ratio, fillColor, bgColor, alpha);
}
