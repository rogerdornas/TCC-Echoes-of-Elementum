//
// Created by Lucas N. Ferreira on 22/05/25.
//

#include "UIText.h"
#include "UIFont.h"
#include "../Renderer/Renderer.h"
#include "../Renderer/Texture.h"

UIText::UIText(const std::string &text, class UIFont* font, int pointSize, const unsigned wrapLength,
               const Vector2 &pos, const Vector3 &color)
    :UIElement(pos, Vector2::Zero, color)
    ,mFont(font)
    ,mPointSize(pointSize)
    ,mWrapLength(wrapLength)
    ,mTextTexture(nullptr)
{
    SetText(text);
}

UIText::~UIText()
{
    if (mTextTexture)
    {
        mTextTexture->Unload();
        delete mTextTexture;
        mTextTexture = nullptr;
    }
}

void UIText::SetText(const std::string &text)
{
    if (mTextTexture)
    {
        // mTextTexture->Unload();
        delete mTextTexture;
        mTextTexture = nullptr;
    }

    mTextTexture = mFont->RenderText(text, mColor, static_cast<int>(mPointSize), mWrapLength);
    mText = text;

    if (mTextTexture)
    {
        SetSize(Vector2(static_cast<float>(mTextTexture->GetWidth()),
                        static_cast<float>(mTextTexture->GetHeight())));
    }
}

void UIText::SetColor(const Vector3 &color) {
    mColor = color;
    SetText(mText);
}

void UIText::Draw(Renderer *renderer, const Vector2 &screenPos)
{
    if (!mIsVisible) {
        return;
    }
    if (!mTextTexture)
        return;

    Vector2 pos = mPosition + screenPos;
    renderer->DrawTexture(pos, mSize, 0.0f, mColor, mTextTexture);

    // SDL_Rect titleQuad;
    // titleQuad.x = mPosition.x + screenPos.x;
    // titleQuad.y = mPosition.y + screenPos.y;
    // titleQuad.w = mSize.x;
    // titleQuad.h = mSize.y;
    //
    // SDL_RenderCopyEx(renderer, mTextTexture, nullptr, &titleQuad, 0.0, nullptr, SDL_FLIP_NONE);
}
