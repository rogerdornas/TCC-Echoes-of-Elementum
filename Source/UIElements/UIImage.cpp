//
// Created by Lucas N. Ferreira on 28/05/25.
//

#include "UIImage.h"
#include "../Renderer/Renderer.h"

UIImage::UIImage(const std::string &imagePath, const Vector2 &pos, const Vector2 &size, const Vector3 &color, Renderer* renderer, bool smooth)
    :UIElement(pos, size, color)
    ,mTexture(nullptr)
    ,mRenderer(renderer)
    ,mIsSmooth(smooth)
{
    mTexture = mRenderer->GetTexture(imagePath, mIsSmooth);
}

UIImage::~UIImage()
{
    // if (mTexture) {
    //     mTexture->Unload();
    //     delete mTexture;
    //     mTexture = nullptr;
    // }
    // if (mTexture) {
    //     SDL_DestroyTexture(mTexture);
    //     mTexture = nullptr;
    // }
}

void UIImage::SetImage(const std::string& imagePath)
{
    mTexture = mRenderer->GetTexture(imagePath, mIsSmooth);

    // if (mTexture != nullptr) {
    //     SDL_DestroyTexture(mTexture);
    //     mTexture = nullptr;
    // }
    //
    // SDL_Surface* surface = IMG_Load(imagePath.c_str());
    //
    // if (!surface) {
    //     SDL_Log("Failed to load image: %s", IMG_GetError());
    // }
    //
    // mTexture = SDL_CreateTextureFromSurface(mRenderer, surface);
    // SDL_FreeSurface(surface);
}

void UIImage::Draw(Renderer* renderer, const Vector2 &screenPos)
{
    if (!mIsVisible || !mTexture) {
        return;
    }

    Vector2 pos = mPosition + screenPos;
    mRenderer->DrawTexture(pos, mSize, mRotation, Color::White, mTexture, Vector4::UnitRect,
                           Vector2::Zero, Vector2::One, 1.0f, mAlpha);
}
