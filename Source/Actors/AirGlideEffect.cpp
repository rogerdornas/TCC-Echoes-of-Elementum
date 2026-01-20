//
// Created by roger on 29/12/2025.
//

#include "AirGlideEffect.h"
#include "../Game.h"
#include "../Components/Drawing/AnimatorComponent.h"
#include "../Components/Drawing/RectComponent.h"

AirGlideEffect::AirGlideEffect(Game *game, Actor* owner)
    :Actor(game)
    ,mWidth(30)
    ,mHeight(30)
    ,mMaxWidth(170)
    ,mMaxHeight(170)
    ,mMinWidth(30)
    ,mMinHeight(30)
    ,mIncreaseRate(1270)
    ,mActivated(false)
    ,mIsDeactivating(false)
    ,mOwner(owner)
    ,mDrawComponent(nullptr)
    ,mRectComponent(nullptr)
{

    // mRectComponent = new RectComponent(this, mWidth, mHeight, RendererMode::LINES);
    // mRectComponent->SetColor(Vector3(0, 255, 0));

    mDrawComponent = new AnimatorComponent(this, "../Assets/Sprites/AirGlide2/AirGlide.png",
                                       "../Assets/Sprites/AirGlide2/AirGlide.json",
                                       mWidth, mHeight, 1003);

    std::vector idle = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
    mDrawComponent->AddAnimation("idle", idle);

    mDrawComponent->SetAnimation("idle");
    mDrawComponent->SetAnimFPS(14);
}

void AirGlideEffect::OnUpdate(float deltaTime) {
    if (mIsDeactivating) {
        if (mWidth > mMinWidth) {
            mWidth -= mIncreaseRate * deltaTime;
            if (mWidth < mMinWidth) {
                mWidth = mMinWidth;
            }
            if (mRectComponent) {
                mRectComponent->SetWidth(mWidth);
            }
            if (mDrawComponent) {
                mDrawComponent->SetWidth(mWidth);
            }
        }
        if (mHeight > mMinHeight) {
            mHeight -= mIncreaseRate * deltaTime;
            if (mHeight < mMinHeight) {
                mHeight = mMinHeight;
            }
            if (mRectComponent) {
                mRectComponent->SetHeight(mHeight);
            }
            if (mDrawComponent) {
                mDrawComponent->SetHeight(mHeight);
            }
        }
        if (mWidth == mMinWidth && mHeight == mMinHeight) {
            SetState(ActorState::Paused);
            mActivated = false;
            if (mRectComponent) {
                mRectComponent->SetVisible(false);
            }
            if (mDrawComponent) {
                mDrawComponent->SetVisible(false);
            }
        }
    }
    else {
        if (mWidth < mMaxWidth) {
            mWidth += mIncreaseRate * deltaTime;
            if (mWidth > mMaxWidth) {
                mWidth = mMaxWidth;
            }
            if (mRectComponent) {
                mRectComponent->SetWidth(mWidth);
            }
            if (mDrawComponent) {
                mDrawComponent->SetWidth(mWidth);
            }
        }
        if (mHeight < mMaxHeight) {
            mHeight += mIncreaseRate * deltaTime;
            if (mHeight > mMaxHeight) {
                mHeight = mMaxHeight;
            }
            if (mRectComponent) {
                mRectComponent->SetHeight(mHeight);
            }
            if (mDrawComponent) {
                mDrawComponent->SetHeight(mHeight);
            }
        }
    }

    SetPosition(mOwner->GetPosition() + Vector2(-10 * mOwner->GetForward().x, 0));
}

void AirGlideEffect::Activate() {
    if (!mActivated) {
        mActivated = true;
        mIsDeactivating = false;
        if (mRectComponent) {
            mRectComponent->SetVisible(true);
        }
        if (mDrawComponent) {
            mDrawComponent->SetVisible(true);
        }
        mWidth = mMinWidth;
        mHeight = mMinHeight;
    }
}

void AirGlideEffect::Deactivate() {
    // SetState(ActorState::Paused);
    // mActivated = false;
    mIsDeactivating = true;
    // if (mRectComponent) {
    //     mRectComponent->SetVisible(false);
    // }
    // if (mDrawComponent) {
    //     mDrawComponent->SetVisible(false);
    // }
}
