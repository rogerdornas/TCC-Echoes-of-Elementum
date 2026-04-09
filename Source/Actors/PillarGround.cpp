//
// Created by roger on 05/12/2025.
//

#include "PillarGround.h"
#include "../Components/Drawing/TileMapComponent.h"
#include "../Components/AABBComponent.h"

PillarGround::PillarGround(Game *game, float width, float height)
    :DynamicGround(game, width, height)
    ,mLifeDuration(3.5f)
    ,mLifeTimer(0.0f)
{
    mIsBreakable = true;
    mGrowSpeed = Vector2(0, 750);
    mMinWidth = width;
    mMinHeight = 0;
    mMaxWidth = width;
    mMaxHeight = 134;
    mGrowthDirection = GrowthDirection::Up;

    mDrawComponent->SetTexture(mGame->GetRenderer()->GetTexture("../Assets/Sprites/Pillar2/01.png"),
                    mMaxWidth, mMaxHeight);

    if (mDrawComponent) {
        mDrawComponent->SetDrawOrder(101);
    }

    SetIsGrowing(true);
}

void PillarGround::OnUpdate(float deltaTime) {
    DynamicGround::OnUpdate(deltaTime);

    mLifeTimer += deltaTime;
    if (mLifeTimer >= mLifeDuration) {
        DestroyEffects();
        SetState(ActorState::Destroy);
    }

    ResolveGroundCollision();

    ManageAnimations();
}

void PillarGround::ResolveGroundCollision() {
    std::vector<Ground*> grounds = GetGame()->GetGrounds();
    bool isOnGround = false;
    if (!grounds.empty()) {
        for (Ground* g : grounds) {
            if (g != this) {
                if (mAABBComponent->Intersect(*g->GetComponent<ColliderComponent>())) {
                    Vector2 collisionNormal = mAABBComponent->CollisionSide(*g->GetComponent<ColliderComponent>());
                    // Colidiu top
                    if (collisionNormal == Vector2::NegUnitY) {
                        isOnGround = true;
                    }
                    else {
                        mLifeTimer = mLifeDuration;
                    }
                }
            }
        }
    }
    if (mLifeTimer > 0.015 * mLifeDuration && !isOnGround) {
        mLifeTimer = mLifeDuration;
    }
}

void PillarGround::ManageAnimations() {
    if (mLifeTimer > mLifeDuration * 0.75f) {
        mDrawComponent->SetTexture(mGame->GetRenderer()->GetTexture("../Assets/Sprites/Pillar2/04.png"),
                mMaxWidth, mMaxHeight);
    }
    else if (mLifeTimer > mLifeDuration * 0.5f && mLifeTimer <= mLifeDuration * 0.75f) {
        mDrawComponent->SetTexture(mGame->GetRenderer()->GetTexture("../Assets/Sprites/Pillar2/03.png"),
                mMaxWidth, mMaxHeight);
    }
    else if (mLifeTimer > mLifeDuration * 0.25f && mLifeTimer <= mLifeDuration * 0.5f) {
        mDrawComponent->SetTexture(mGame->GetRenderer()->GetTexture("../Assets/Sprites/Pillar2/02.png"),
                mMaxWidth, mMaxHeight);
    }
    if (mLifeTimer <= mLifeDuration * 0.25f) {
        mDrawComponent->SetTexture(mGame->GetRenderer()->GetTexture("../Assets/Sprites/Pillar2/01.png"),
                mMaxWidth, mMaxHeight);
    }
}
