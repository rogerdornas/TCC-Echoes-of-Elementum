//
// Created by roger on 03/10/2025.
//

#pragma once
#include <string>
#include "Actor.h"

class Decorations : public  Actor
{
public:
    Decorations(Game* game, float width, float height, std::string imagePath, std::string decorationName,
                float fps, int numFrames, bool animated, int gid, float rotation,
                int drawOrder, Vector2 parallaxFactor = Vector2(1.0f, 1.0f),
                Vector3 textureColor = Color::White, float textureFactor = 1.0f);

    void OnUpdate(float deltaTime) override;

private:
    float mWidth;
    float mHeight;
    std::string mImagePath;
    float mFPS;
    int mNumFrames;
    bool mAnimated;

    class RectComponent* mRectComponent;
    class AnimatorComponent* mDrawComponent;
};

