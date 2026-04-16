//
// Created by Lucas N. Ferreira on 22/05/25.
//

#include "UIButton.h"
#include "../Renderer/Renderer.h"

UIButton::UIButton(const std::string& text, class UIFont* font, std::function<void()> onClick,
                   const Vector2& pos, const Vector2& size, bool useTextSize, const Vector3& color,
                   int pointSize , unsigned wrapLength,
                   const Vector2 &textPos, TextPos textAlign, const Vector3& textColor,
                   Renderer* renderer)
    :UIElement(pos, size, color)
    ,mOnClick(onClick)
    ,mText(nullptr)
    ,mTextAlign(textAlign)
    ,mHighlighted(false)
    ,mUseBackGroundColor(false)
    ,mUseImageSelector(true)
    ,mIsHoldable(false)
    ,mHoldTimeRequired(1.0f)
    ,mCurrentHoldTime(0.0f)
    ,mIsBeingHeld(false)
    ,mIsInteractable(true)
    ,mRenderer(renderer)
    ,mSelectorTexture(nullptr)
{
    if (text.size() > 0) {
        mText = new UIText(text, font, pointSize, wrapLength, textPos, textColor);
        if (useTextSize) {
            mSize = mText->GetSize() * Vector2(1.1f, 1.0f);
        }
    }
    std::string texPath = "../Assets/Sprites/Menus/select.png";
    mSelectorTexture = mRenderer->GetTexture(texPath);
}

UIButton::~UIButton()
{
    if (mText) {
        delete mText;
        mText = nullptr;
    }
}

void UIButton::Update(float deltaTime) {
    if (mIsBeingHeld) {
        mCurrentHoldTime += deltaTime;

        // Se o tempo chegou no limite, dispara o OnClick e reseta
        if (mCurrentHoldTime >= mHoldTimeRequired) {
            OnClick();
            mIsBeingHeld = false;
            mCurrentHoldTime = 0.0f;
        }
    } else {
        mCurrentHoldTime = 0.0f;
    }
}

void UIButton::SetHoldable(bool holdable, float timeRequired) {
    mIsHoldable = holdable;
    mHoldTimeRequired = timeRequired;
}

void UIButton::SetBeingHeld(bool held) {
    if (held && !mIsInteractable) {
        return;
    }
    mIsBeingHeld = held;
    if (!held) {
        mCurrentHoldTime = 0.0f;
    }
}

void UIButton::SetInteractable(bool interactable) {
    mIsInteractable = interactable;
    if (!mIsInteractable) {
        mIsBeingHeld = false;
        mCurrentHoldTime = 0.0f;
    }
}

void UIButton::Draw(Renderer *renderer, const Vector2 &screenPos)
{
    if (!mIsVisible) {
        return;
    }

    // Desenhar o retângulo do botão
    Vector2 drawPos = screenPos + mPosition + mSize * 0.5f;

    if (mHighlighted) {
        if (mUseImageSelector) {
            renderer->DrawTexture(drawPos + Vector2(mSize.x * -0.5f - 16, 0), Vector2(26, 26), Math::Pi, Color::White,
                                  mSelectorTexture, Vector4::UnitRect, Vector2::Zero, Vector2::One, 0.0f, 1.0f);

            renderer->DrawTexture(drawPos + Vector2(mSize.x * 0.5f + 16, 0), Vector2(26, 26), 0.0f, Color::White,
                          mSelectorTexture, Vector4::UnitRect, Vector2::Zero, Vector2::One, 0.0f, 1.0f);
        }
        if (mText) {
            mText->SetColor(Color::White);
        }

        if (mUseBackGroundColor) {
            Vector3 drawColor = Color::Black;
            renderer->DrawRect(drawPos, mSize, 0.0f, drawColor, Vector2::Zero, RendererMode::TRIANGLES, 0.5f);
        }
    }
    else {
        if (mText) {
            mText->SetColor(Vector3(0.5f, 0.5f, 0.5f));
        }
    }

    // Calcular posição do texto
    if (mText) {
        Vector2 textDrawPos = drawPos;
        switch (mTextAlign) {
            case TextPos::AlignLeft:
                textDrawPos += Vector2(-mSize.x / 2 + mText->GetSize().x / 2, 0);
            break;

            case TextPos::AlignRight:
                textDrawPos += Vector2(mSize.x / 2 - mText->GetSize().x / 2, 0);
            break;

            case TextPos::Center:
                break;
        }

        mText->Draw(renderer, textDrawPos);
    }
}

bool UIButton::ContainsPoint(const Vector2 &pt) const {
    if (pt.x < mPosition.x || pt.x > mPosition.x + mSize.x ||
        pt.y < mPosition.y || pt.y > mPosition.y + mSize.y)
    {
        return false;
    }
    return true;
}


void UIButton::OnClick()
{
    if (!mIsInteractable) {
        return;
    }
    if (mOnClick) {
        mOnClick();
    }
}

void UIButton::OnMouseClick(const Vector2 &mousePos) {
    if (!mIsInteractable) {
        return;
    }
    if (mOnClick) {
        mOnClick();
    }
}
