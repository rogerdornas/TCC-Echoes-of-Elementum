//
// Created by roger on 19/12/2025.
//

#include "GroundSlamImpactEffect.h"
#include "../Game.h"
#include "../Components/Drawing/AnimatorComponent.h"
#include "../Components/Drawing/RectComponent.h"
#include "../Components/AABBComponent.h"

GroundSlamImpactEffect::GroundSlamImpactEffect(Game *game, float duration)
    :Actor(game)
    ,mWidth(400)
    ,mHeight(115)
    ,mDuration(duration)
    ,mTimer(mDuration)
    ,mGrowSpeed(900)
    ,mMaxWidth(370)
    ,mDamage(15)
    ,mDrawComponent(nullptr)
    ,mRectComponent(nullptr)
    ,mAABBComponent(nullptr)
{
    Vector2 v1(-mWidth/2, -mHeight/2);
    Vector2 v2(mWidth/2, -mHeight/2);
    Vector2 v3(mWidth/2, mHeight/2);
    Vector2 v4(-mWidth/2, mHeight/2);

    std::vector<Vector2> vertices;
    vertices.emplace_back(v1);
    vertices.emplace_back(v2);
    vertices.emplace_back(v3);
    vertices.emplace_back(v4);

    mAABBComponent = new AABBComponent(this, v1, v3);

    // mRectComponent = new RectComponent(this, mWidth, mHeight, RendererMode::LINES);
    // mRectComponent->SetColor(Vector3(255, 0, 0));

    mDrawComponent = new AnimatorComponent(this, "../Assets/Sprites/GroundSlamImpact/GroundSlamImpact.png",
                                           "../Assets/Sprites/GroundSlamImpact/GroundSlamImpact.json",
                                           mWidth, mHeight, 1001);

    std::vector idle = {4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16};
    mDrawComponent->AddAnimation("idle", idle);

    // std::vector idle = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11};
    // mDrawComponent->AddAnimation("idle", idle);

    mDrawComponent->SetAnimation("idle");
    mDrawComponent->SetAnimFPS(31 / mDuration);

    // mDrawComponent->SetAnimation("idle");
    // mDrawComponent->SetAnimFPS(30 / mDuration);
}

void GroundSlamImpactEffect::OnUpdate(float deltaTime) {
    mTimer += deltaTime;
    if (mTimer >= mDuration) {
        if (mDrawComponent) {
            mDrawComponent->SetVisible(false);
        }
        if (mRectComponent) {
            mRectComponent->SetVisible(false);
        }
        mAABBComponent->SetActive(false);
    }
    else {
        mWidth += mGrowSpeed * deltaTime;
        if (mWidth > mMaxWidth) {
            mWidth = mMaxWidth;
        }

        Vector2 v1(-mWidth / 2, -mHeight / 2);
        Vector2 v2(mWidth / 2, -mHeight / 2);
        Vector2 v3(mWidth / 2, mHeight / 2);
        Vector2 v4(-mWidth / 2, mHeight / 2);

        std::vector<Vector2> vertices;
        vertices.emplace_back(v1);
        vertices.emplace_back(v2);
        vertices.emplace_back(v3);
        vertices.emplace_back(v4);

        if (auto* aabb = dynamic_cast<AABBComponent*>(mAABBComponent)) {
            aabb->SetMin(v1);
            aabb->SetMax(v3);
        }
        if (mRectComponent) {
            mRectComponent->SetWidth(mWidth);
            mRectComponent->SetHeight(mHeight);
        }

        ResolveEnemyCollision();

        if (mTimer >= mDuration * 0.6f) {
            if (mDrawComponent) {
                mDrawComponent->SetAlpha(1 - (mTimer - mDuration * 0.6f) / (mDuration * 0.4f));
            }
            if (mRectComponent) {
                mRectComponent->SetVisible(false);
            }
            mAABBComponent->SetActive(false);
        }
    }
}

void GroundSlamImpactEffect::Start(Vector2 position) {
    SetPosition(position);
    mTimer = 0;
    mWidth = 10;
    Vector2 v1(-mWidth / 2, -mHeight / 2);
    Vector2 v2(mWidth / 2, -mHeight / 2);
    Vector2 v3(mWidth / 2, mHeight / 2);
    Vector2 v4(-mWidth / 2, mHeight / 2);

    std::vector<Vector2> vertices;
    vertices.emplace_back(v1);
    vertices.emplace_back(v2);
    vertices.emplace_back(v3);
    vertices.emplace_back(v4);

    if (auto* aabb = dynamic_cast<AABBComponent*>(mAABBComponent)) {
        aabb->SetMin(v1);
        aabb->SetMax(v3);
    }

    mEnemiesHit.clear();
    if (mDrawComponent) {
        mDrawComponent->ResetAnimationTimer();
        mDrawComponent->SetVisible(true);
        mDrawComponent->SetAlpha(1.0f);
    }
    if (mRectComponent) {
        mRectComponent->SetWidth(mWidth);
        mRectComponent->SetHeight(mHeight);
        mRectComponent->SetVisible(true);
    }
    mAABBComponent->SetActive(true);
}

void GroundSlamImpactEffect::ResolveEnemyCollision() {
    std::vector<Enemy*> enemies = mGame->GetEnemies();
    if (!enemies.empty()) {
        for (Enemy* e : enemies) {
            if (mAABBComponent->Intersect(*e->GetComponent<ColliderComponent>())) {
                float dist = GetPosition().x - e->GetPosition().x;

                auto it = std::find(mEnemiesHit.begin(), mEnemiesHit.end(), e);
                if (it == mEnemiesHit.end()) {
                    if (dist < 0) {
                        e->ReceiveHit(mDamage, Vector2(0.7f, -0.8f));
                    }
                    else {
                        e->ReceiveHit(mDamage, Vector2(-0.7f, -0.8f));
                    }
                    mEnemiesHit.push_back(e);
                }
            }
        }
    }
}