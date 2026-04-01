//
// Created by Lucas N. Ferreira on 22/05/25.
//

#pragma once

#include <functional>
#include <string>
#include "UIFont.h"
#include "UIText.h"
#include "UIElement.h"
#include "../Math.h"

class UIButton : public UIElement
{
public:
    enum class TextPos {
        AlignLeft,
        AlignRight,
        Center
    };

    UIButton(const std::string& text, class UIFont* font, std::function<void()> onClick,
             const Vector2& pos, const Vector2& size, bool useTextSize, const Vector3& color = Vector3(30, 30, 30),
             int pointSize = 72, const unsigned wrapLength = 1024,
             const Vector2& textPos = Vector2::Zero,
             const TextPos textAlign = TextPos::Center,
             const Vector3& textColor = Color::White,
             Renderer* renderer = nullptr);

    ~UIButton();

    // Set the name of the button
    UIText* GetText() const { return mText; }
    void Draw(Renderer* renderer, const Vector2 &screenPos) override;

    void SetHighlighted(bool sel) { mHighlighted = sel; }
    bool GetHighlighted() const { return mHighlighted; }

    void UseBackGroundColor() { mUseBackGroundColor = true; }
    void RemoveImageSelector() { mUseImageSelector = false; }

    // Returns true if the point is within the button's bounds
    bool ContainsPoint(const Vector2& pt) const;

    virtual bool IsDraggable() const { return false; }

    // Called when button is clicked
    void OnClick();
    virtual void OnMouseClick(const Vector2& mousePos) { OnClick(); }

protected:
    // Callback function
    std::function<void()> mOnClick;

    // Button name
    UIText* mText;

    // Alinhamento do texto do botão
    TextPos mTextAlign;

    // Check if the button is highlighted
    bool mHighlighted;

    bool mUseBackGroundColor;
    bool mUseImageSelector;

    Renderer* mRenderer;
    Texture* mSelectorTexture;
};