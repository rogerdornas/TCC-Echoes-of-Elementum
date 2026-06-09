#include "UIScreen.h"
#include "../Game.h"
#include "UIFont.h"
#include <cfloat>

UIScreen::UIScreen(Game* game, const std::string& fontName, bool isClosable)
	:mGame(game)
	,mPos(0.f, 0.f)
	,mSize(0.f, 0.f)
	,mState(UIState::Active)
    ,mIsVisible(true)
    ,mIsClosable(isClosable)
    ,mDraggedButton(nullptr)
    ,mSelectedButtonIndex(-1)
    ,mHeldDirection(NavDirection::None)
    ,mHoldTimer(0.0f)
    ,mRepeatDelay(0.3f)
    ,mRepeatRate(0.09f)
{
    mGame->PushUI(this);

    mFont = mGame->LoadFont(fontName);
}

UIScreen::~UIScreen()
{
    for (UIText* text : mTexts) {
        delete text;
    }
    mTexts.clear();

    for (UIButton* button : mButtons) {
        delete button;
    }
    mButtons.clear();

    for (UIImage* image : mImages) {
        delete image;
    }
    mImages.clear();
}

void UIScreen::Update(float deltaTime) {
    for (UIButton* button : mButtons) {
        button->Update(deltaTime);
    }

    if (mHeldDirection != NavDirection::None) {
        mHoldTimer += deltaTime;

        if (mHoldTimer >= mRepeatDelay) {
            MoveSelection(mHeldDirection);
            mHoldTimer -= mRepeatRate;
        }
    }
}

void UIScreen::Draw(Renderer *renderer)
{
    if (!mIsVisible) {
        return;
    }

    for (UIImage* image : mImages) {
        image->Draw(renderer, mPos);
    }

    for (UIButton* button : mButtons) {
        button->Draw(renderer, mPos);
    }

    for (UIText* text : mTexts) {
        text->Draw(renderer, mPos);
    }
}

void UIScreen::ProcessInput(const uint8_t* keys) {

}

void UIScreen::MoveSelection(NavDirection dir) {
    if (mButtons.empty()) return;

    UIButton* current = mButtons[mSelectedButtonIndex];
    UIButton* next = nullptr;
    UISlider* slider = dynamic_cast<UISlider*>(current);

    if (dir == NavDirection::Up) {
        next = FindNeighbor(current, Vector2(0, -1));
    }
    else if (dir == NavDirection::Down) {
        next = FindNeighbor(current, Vector2(0, 1));
    }
    else if (dir == NavDirection::Left) {
        if (slider) { slider->Decrease(); return; }
        next = FindNeighbor(current, Vector2(-1, 0));
    }
    else if (dir == NavDirection::Right) {
        if (slider) { slider->Increase(); return; }
        next = FindNeighbor(current, Vector2(1, 0));
    }

    // Troca seleção se houver vizinho
    if (next) {
        if (mSelectedButtonIndex >= 0) {
            mButtons[mSelectedButtonIndex]->SetBeingHeld(false);
        }

        mSelectedButtonIndex = static_cast<int>(
            std::distance(mButtons.begin(),
                          std::find(mButtons.begin(), mButtons.end(), next))
        );

        // Atualiza destaque
        for (size_t i = 0; i < mButtons.size(); ++i) {
            mButtons[i]->SetHighlighted(static_cast<int>(i) == mSelectedButtonIndex);
        }
    }
}

void UIScreen::HandleKeyPress(int key, int controllerButton, int leftControllerAxisY, int leftControllerAxisX, int rightControllerAxisY, int rightControllerAxisX) {
    if (mButtons.empty()) {
        return;
    }

    auto inputBinding = mGame->GetInputBinding();

    if (key == SDLK_UP ||
        key == SDL_GetKeyFromScancode(inputBinding[Game::Action::Up].key) ||
        controllerButton == SDL_CONTROLLER_BUTTON_DPAD_UP ||
        leftControllerAxisY < 0 || rightControllerAxisY < 0)
    {
        if (mHeldDirection != NavDirection::Up) {
            mHeldDirection = NavDirection::Up;
            mHoldTimer = 0.0f;
            MoveSelection(NavDirection::Up);
        }
    }
    else if (key == SDLK_DOWN ||
             key == SDL_GetKeyFromScancode(inputBinding[Game::Action::Down].key) ||
             controllerButton == SDL_CONTROLLER_BUTTON_DPAD_DOWN ||
             leftControllerAxisY > 0 || rightControllerAxisY > 0)
    {
        if (mHeldDirection != NavDirection::Down) {
            mHeldDirection = NavDirection::Down;
            mHoldTimer = 0.0f;
            MoveSelection(NavDirection::Down);
        }
    }
    else if (key == SDLK_LEFT ||
         key == SDL_GetKeyFromScancode(inputBinding[Game::Action::MoveLeft].key) ||
         controllerButton == SDL_CONTROLLER_BUTTON_DPAD_LEFT ||
         leftControllerAxisX < 0 || rightControllerAxisX < 0)
    {
        if (mHeldDirection != NavDirection::Left) {
            mHeldDirection = NavDirection::Left;
            mHoldTimer = 0.0f;
            MoveSelection(NavDirection::Left);
        }
    }
    else if (key == SDLK_RIGHT ||
             key == SDL_GetKeyFromScancode(inputBinding[Game::Action::MoveRight].key) ||
             controllerButton == SDL_CONTROLLER_BUTTON_DPAD_RIGHT ||
             leftControllerAxisX > 0 || rightControllerAxisX > 0)
    {
        if (mHeldDirection != NavDirection::Right) {
            mHeldDirection = NavDirection::Right;
            mHoldTimer = 0.0f;
            MoveSelection(NavDirection::Right);
        }
    }

    // Ativa botão selecionado
    if (key == SDLK_RETURN ||
        controllerButton == SDL_CONTROLLER_BUTTON_A||
        key == SDL_GetKeyFromScancode(inputBinding[Game::Action::Jump].key) ||
        key == SDL_GetKeyFromScancode(inputBinding[Game::Action::Attack].key))
    {
        if (mSelectedButtonIndex >= 0 && mSelectedButtonIndex < static_cast<int>(mButtons.size())) {
            UIButton* btn = mButtons[mSelectedButtonIndex];

            // SE FOR HOLDABLE, COMEÇA A SEGURAR EM VEZ DE CLICAR
            if (btn->IsHoldable()) {
                btn->SetBeingHeld(true);
            } else {
                btn->OnClick();
            }

            if (mGame->GetPlayer()) {
                mGame->GetPlayer()->SetCanJump(false);
                mGame->GetPlayer()->SetPrevSwordPressed(true);
            }
        }
    }
}

void UIScreen::HandleMousePress(const Vector2& virtualMousePos) {
    Vector2 uiScreenRelativePos = virtualMousePos - GetPosition();

    for (UIButton* button : mButtons) {
        if (button->ContainsPoint(uiScreenRelativePos)) {
            if (button->IsHoldable()) {
                button->SetBeingHeld(true);
            } else {
                button->OnMouseClick(uiScreenRelativePos);
            }

            if (button->IsDraggable()) {
                mDraggedButton = button;
            }
            return;
        }
    }

    bool isInsideWidth = (virtualMousePos.x >= mPos.x) && (virtualMousePos.x <= (mPos.x + mSize.x));
    bool isInsideHeight = (virtualMousePos.y >= mPos.y) && (virtualMousePos.y <= (mPos.y + mSize.y));

    if (!(isInsideWidth && isInsideHeight)) {
        if (mIsClosable) {
            Close();
        }
    }
}

void UIScreen::HandleKeyRelease(int key, int controllerButton) {
    auto inputBinding = mGame->GetInputBinding();

    if (!mButtons.empty()) {
        if (key == SDLK_RETURN ||
            controllerButton == SDL_CONTROLLER_BUTTON_A||
            key == SDL_GetKeyFromScancode(inputBinding[Game::Action::Jump].key) ||
            key == SDL_GetKeyFromScancode(inputBinding[Game::Action::Attack].key))
        {
            if (mSelectedButtonIndex >= 0 && mSelectedButtonIndex < static_cast<int>(mButtons.size())) {
                mButtons[mSelectedButtonIndex]->SetBeingHeld(false);
            }
        }
    }

    // Lógica Direcional
    bool isUp = (key == SDLK_UP ||
                 key == SDL_GetKeyFromScancode(inputBinding[Game::Action::Up].key) ||
                 controllerButton == SDL_CONTROLLER_BUTTON_DPAD_UP);
    bool isDown = (key == SDLK_DOWN ||
                   key == SDL_GetKeyFromScancode(inputBinding[Game::Action::Down].key) ||
                   controllerButton == SDL_CONTROLLER_BUTTON_DPAD_DOWN);
    bool isLeft = (key == SDLK_LEFT ||
                   key == SDL_GetKeyFromScancode(inputBinding[Game::Action::MoveLeft].key) ||
                   controllerButton == SDL_CONTROLLER_BUTTON_DPAD_LEFT);
    bool isRight = (key == SDLK_RIGHT ||
                    key == SDL_GetKeyFromScancode(inputBinding[Game::Action::MoveRight].key) ||
                    controllerButton == SDL_CONTROLLER_BUTTON_DPAD_RIGHT);

    if (isUp) CancelDirectionalHold(NavDirection::Up);
    if (isDown) CancelDirectionalHold(NavDirection::Down);
    if (isLeft) CancelDirectionalHold(NavDirection::Left);
    if (isRight) CancelDirectionalHold(NavDirection::Right);
}

void UIScreen::HandleMouseRelease(const Vector2& virtualMousePos) {
    mDraggedButton = nullptr;
    for (UIButton* button : mButtons) {
        button->SetBeingHeld(false);
    }
}

void UIScreen::HandleMouseMotion(const Vector2& virtualMousePos) {
    Vector2 uiScreenRelativePos = virtualMousePos - GetPosition();

    if (mDraggedButton != nullptr) {
        mDraggedButton->OnMouseClick(uiScreenRelativePos);
        return;
    }

    int index = -1;

    for (size_t i = 0; i < mButtons.size(); ++i) {
        if (mButtons[i]->ContainsPoint(uiScreenRelativePos)) {
            index = i;
            mSelectedButtonIndex = i;
        }
    }

    // Atualiza destaque
    if (index != -1) {
        for (size_t i = 0; i < mButtons.size(); ++i) {
            mButtons[i]->SetHighlighted(static_cast<int>(i) == index);
        }
    }
}

void UIScreen::CancelDirectionalHold(NavDirection dirToCancel) {
    if (dirToCancel == NavDirection::None || mHeldDirection == dirToCancel) {
        mHeldDirection = NavDirection::None;
    }
}

void UIScreen::Close() {
	mState = UIState::Closing;
}

UIText* UIScreen::AddText(const std::string &name, const Vector2 &pos, const int pointSize, Vector3 color, const int unsigned wrapLength) {
    UIText* t = new UIText(name, mFont, pointSize, wrapLength, pos, color);

    mTexts.emplace_back(t);

    return t;
}

UIButton* UIScreen::AddButton(const std::string& name, const Vector2 &pos, const Vector2& dims, const int pointSize, UIButton::TextPos alignText, std::function<void()> onClick, bool useTextSize, Vector2 textPos, Vector3 textColor, unsigned textWrapLength) {
    UIButton* b = new UIButton(name, mFont, onClick, pos, dims, useTextSize, Vector3{1.0f, 0.5f, 0.0f}, pointSize, textWrapLength, textPos, alignText, textColor, mGame->GetRenderer());
    mButtons.emplace_back(b);

    if (mButtons.size() == 1) {
        mSelectedButtonIndex = 0;
        b->SetHighlighted(true);
    }

    return b;
}

UIImage* UIScreen::AddImage(const std::string &imagePath, const Vector2 &pos, const Vector2 &dims, const Vector3 &color) {
    UIImage* img = new UIImage(imagePath, pos, dims, color, mGame->GetRenderer());

    mImages.emplace_back(img);

    return img;
}

UISlider* UIScreen::AddSlider(const std::string& name, const Vector2& pos, const Vector2& dims, const Vector2& sliderOffset, const Vector2& sliderSize, float minVal, float maxVal, float initialVal, int textPointSize, int valuePointSize, std::function<void(float)> onValueChanged) {
    UISlider* slider = new UISlider(name, mFont, pos, dims, sliderOffset, sliderSize, minVal, maxVal, initialVal, textPointSize, valuePointSize, onValueChanged, mGame->GetRenderer());
    mButtons.emplace_back(slider);

    if (mButtons.size() == 1) {
        mSelectedButtonIndex = 0;
        slider->SetHighlighted(true);
    }
    return slider;
}

UISkillNodeButton* UIScreen::AddSkillNodeButton(const std::string& skillId, UISkillNodeButton::NodeType nodeType, const Vector2& pos, std::function<void()> onClick, std::function<void(const std::string&)> onSelect) {
    UISkillNodeButton* nodeBtn = new UISkillNodeButton(skillId, nodeType, onClick, onSelect, pos, mGame->GetRenderer());
    mButtons.emplace_back(nodeBtn);

    if (mButtons.size() == 1) {
        mSelectedButtonIndex = 0;
        nodeBtn->SetHighlighted(true);
    }

    return nodeBtn;
}

UIButton* UIScreen::FindNeighbor(UIButton *current, const Vector2 &dir) {
    UIButton* best = nullptr;
    float bestScore = FLT_MAX;

    Vector2 currentCenter = current->GetPosition() + (current->GetSize() * 0.5f);

    for (UIButton* b : mButtons) {
        if (b == current) {
            continue;
        }

        Vector2 bCenter = b->GetPosition() + (b->GetSize() * 0.5f);
        Vector2 delta = bCenter - currentCenter;

        // Produto escalar para verificar se o botão está na direção do input
        float dotProduct = (delta.x * dir.x) + (delta.y * dir.y);

        // Se o dotProduct for <= 0, o botão está atrás ou exatamente perpendicular. Ignoramos.
        if (dotProduct <= 0) continue;

        // Distância primária (o quanto andamos na direção do input)
        float primaryDist = dotProduct;

        // Distância secundária (o quanto nos desviamos perpendicularmente do input)
        float perpDist = std::abs((delta.x * dir.y) - (delta.y * dir.x));

        // Heurística de Pontuação:
        // Penalizamos fortemente a distância perpendicular para preferir botões alinhados.
        // O multiplicador 3.0f dita a rigidez do "cone de visão".
        float score = primaryDist + (perpDist * 3.0f);

        if (score < bestScore) {
            bestScore = score;
            best = b;
        }
    }

    // Lógica de Wrap
    if (!best) {
        float worstScore = -FLT_MAX;

        for (UIButton* b : mButtons) {
            if (b == current) continue;

            Vector2 bCenter = b->GetPosition() + (b->GetSize() * 0.5f);
            Vector2 delta = bCenter - currentCenter;

            float dotProduct = (delta.x * dir.x) + (delta.y * dir.y);

            // Só consideramos botões que estão para TRÁS
            if (dotProduct >= 0) {
                continue;
            }

            float perpDist = std::abs((delta.x * dir.y) - (delta.y * dir.x));

            // Exigimos que o botão esteja minimamente alinhado para o wrap funcionar
            if (perpDist < current->GetSize().Length()) {
                // Queremos a MAIOR distância para trás, minimizando o desalinhamento
                float score = -dotProduct - (perpDist * 3.0f);
                if (score > worstScore) {
                    worstScore = score;
                    best = b;
                }
            }
        }
    }

    return best;
}
