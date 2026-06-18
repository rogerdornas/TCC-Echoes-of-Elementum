//
// Created by Lucas N. Ferreira on 22/05/25.
//

#pragma once

#include <string>
#include <SDL.h>
#include "../Math.h"
#include "UIElement.h"

class UIText : public UIElement {
public:
    UIText(const std::string& text, class UIFont* font, int pointSize = 40, const unsigned wrapLength = 1024,
           const Vector2 &pos = Vector2::Zero, const Vector3& color = Color::White);

    ~UIText();

    void SetText(const std::string& name);
    void SetPointSize(float pointSize) { mPointSize = pointSize; }
    void SetWrapLength(unsigned int wrapLength) { mWrapLength = wrapLength; }
    void SetColor(const Vector3 &color) override;
    void SetFont(class UIFont* font) { mFont = font; }
    void Draw(Renderer* renderer, const Vector2 &screenPos) override;

protected:
    std::string mText;
    class UIFont* mFont;
    class Texture *mTextTexture;

    float mPointSize;
    unsigned int mWrapLength;
};
