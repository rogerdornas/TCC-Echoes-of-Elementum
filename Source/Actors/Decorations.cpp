//
// Created by roger on 03/10/2025.
//

#include "Decorations.h"
#include "../Game.h"
#include "../Components/Drawing/AnimatorComponent.h"
#include "../Components/Drawing/RectComponent.h"
#include "../Json.h"
#include <fstream>

Decorations::Decorations(Game *game, float width, float height, std::string imagePath, std::string decorationName,
                         float fps, int numFrames, bool animated, int gid, float rotation,
                         int drawOrder, Vector2 parallaxFactor,
                         Vector3 textureColor, float textureFactor)
    :Actor(game)
    ,mWidth(width)
    ,mHeight(height)
    ,mFPS(fps)
    ,mNumFrames(numFrames)
    ,mAnimated(animated)
    ,mDrawComponent(nullptr)
    ,mRectComponent(nullptr)
{
    int FLIPPED_HORIZONTALLY_FLAG = 0x80000000;
    int FLIPPED_VERTICALLY_FLAG   = 0x40000000;

    bool flippedHorizontally = (gid & FLIPPED_HORIZONTALLY_FLAG) != 0;
    bool flippedVertically   = (gid & FLIPPED_VERTICALLY_FLAG) != 0;

    if (flippedHorizontally) {
        SetScale(Vector2(-1, GetScale().y));
    }
    if (flippedVertically) {
        SetScale(Vector2(GetScale().x, -1));
    }

    SetRotation(rotation);
    SetTransformRotation(rotation);

    mImagePath = "../Assets/" + imagePath;

    Vector2 v1(-mWidth / 2, -mHeight / 2);
    Vector2 v2(mWidth / 2, -mHeight / 2);
    Vector2 v3(mWidth / 2, mHeight / 2);
    Vector2 v4(-mWidth / 2, mHeight / 2);

    std::vector<Vector2> vertices;
    vertices.emplace_back(v1);
    vertices.emplace_back(v2);
    vertices.emplace_back(v3);
    vertices.emplace_back(v4);

    // mDrawPolygonComponent = new DrawPolygonComponent(this, vertices, {0, 255, 0, 255});

    if (mAnimated) {
        mDrawComponent = new AnimatorComponent(this,
                                            mImagePath + ".png",
                                            mImagePath + ".json",
                                            mWidth, mHeight, drawOrder);

        std::vector<int> idle(mNumFrames);
        std::iota(idle.begin(), idle.end(), 0);
        mDrawComponent->AddAnimation("idle", idle);

        mDrawComponent->SetAnimation("idle");
        mDrawComponent->SetAnimFPS(mFPS);
    }
    else {
        mDrawComponent = new AnimatorComponent(this,
                                    mImagePath + ".png",
                                    mImagePath + ".json",
                                    mWidth, mHeight, drawOrder, true);

        auto decorationsName = mGame->GetDecorationsName();

        // Encontra o índice apenas da decoração atual
        auto it = std::find(decorationsName.begin(), decorationsName.end(), decorationName);
        if (it != decorationsName.end()) {
            int index = std::distance(decorationsName.begin(), it);

            mDrawComponent->AddAnimation(decorationName, {index});
            mDrawComponent->SetAnimation(decorationName);
        } else {
            SDL_Log("Aviso: Decoração %s não encontrada", decorationName.c_str());
        }

        mDrawComponent->SetAnimation(decorationName);
        mDrawComponent->SetAnimFPS(mFPS);
    }

    mDrawComponent->SetColor(textureColor);
    mDrawComponent->SetTextureFactor(textureFactor);
    mDrawComponent->SetParallaxFactor(parallaxFactor);
}

void Decorations::OnUpdate(float deltaTime) {

}
