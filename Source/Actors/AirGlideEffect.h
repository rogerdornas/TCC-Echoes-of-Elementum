//
// Created by roger on 29/12/2025.
//

#pragma once

#include "Actor.h"

class AirGlideEffect : public Actor
{
public:
    AirGlideEffect(Game* game, Actor* owner);

    void OnUpdate(float deltaTime) override;
    void Activate();
    void Deactivate();

private:
    float mWidth;
    float mHeight;
    float mMaxWidth;
    float mMaxHeight;
    float mMinWidth;
    float mMinHeight;
    float mIncreaseRate;
    bool mActivated;
    bool mIsDeactivating;
    Actor* mOwner;

    class AnimatorComponent* mDrawComponent;
    class RectComponent* mRectComponent;
};

