//
// Created by roger on 26/04/2025.
//

#include "Sword.h"
#include "../Game.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/AABBComponent.h"
#include "../Components/CombatBoxComponent.h"
#include "../Components/Drawing/AnimatorComponent.h"
#include "../Components/Drawing/RectComponent.h"

Sword::Sword(class Game *game, Actor *owner, float width, float height, float duration, float damage)
    :Actor(game)
    ,mWidth(width)
    ,mHeight(height)
    ,mHurtSizeScale(0.3f)
    ,mDuration(duration)
    ,mDurationTimer(mDuration)
    ,mDamage(damage)
    ,mOwner(owner)
    ,mDrawComponent(nullptr)
    ,mRectComponent(nullptr)
    ,mCombatBoxComponent(nullptr)
{
    Vector2 v1(-mWidth / 2, -mHeight / 2);
    Vector2 v2(mWidth / 2, -mHeight / 2);
    Vector2 v3(mWidth / 2, mHeight / 2);
    Vector2 v4(-mWidth / 2, mHeight / 2);

    std::vector<Vector2> vertices;
    vertices.emplace_back(v1);
    vertices.emplace_back(v2);
    vertices.emplace_back(v3);
    vertices.emplace_back(v4);

    const std::string swordAssets = "../Assets/Sprites/Sword Slash/";

    // mRectComponent = new RectComponent(this, mWidth, mHeight, RendererMode::LINES);
    // mRectComponent->SetColor(Vector3(37, 218, 255));

    mDrawComponent = new AnimatorComponent(this, swordAssets + "SwordSlash.png",
                                                       swordAssets + "SwordSlash.json",
                                                       mWidth, mHeight, 1001);

    const std::vector slash = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 9, 9, 9, 9};
    mDrawComponent->AddAnimation("slash", slash);

    const std::vector end = {9};
    mDrawComponent->AddAnimation("end", end);

    mDrawComponent->SetAnimation("end");
    const float fps = 9.0f / mDuration;
    mDrawComponent->SetAnimFPS(fps);


    mRigidBodyComponent = new RigidBodyComponent(this, 1, 40000, 1800);
    mAABBComponent = new AABBComponent(this, v1, v3);

    mCombatBoxComponent = new CombatBoxComponent(this);
    mCombatBoxComponent->AddAABBBox("sword", true, v1, v3);
    mCombatBoxComponent->AddAABBBox("ground", false, v1 * Vector2(1, mHurtSizeScale), v3 * Vector2(1, mHurtSizeScale));
    // mCombatBoxComponent->SetDebugDraw(true);
}

void Sword::OnUpdate(float deltaTime) {
    mDurationTimer += deltaTime;
    if (mDurationTimer >= mDuration) {
        Deactivate();
    }
    else {
        Activate();
        // Troca rotação da espada para horizontal ou vertical
        bool isHorizontal = true;
        if (GetRotation() == 0 || GetRotation() == Math::Pi) {
            isHorizontal = true;
        }

        if (GetRotation() == Math::Pi / 2 || GetRotation() == 3 * Math::Pi / 2) {
            isHorizontal = false;
        }

        float offset = 0;
        if (isHorizontal) {
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

            if (mCombatBoxComponent) {
                mCombatBoxComponent->SetBoxHalfSize("sword", Vector2(mWidth / 2, mHeight / 2));
                mCombatBoxComponent->SetBoxHalfSize("ground", Vector2(mWidth / 2, (mHeight / 2) * mHurtSizeScale));
            }

            if (mRectComponent) {
                // mDrawPolygonComponent->SetVertices(vertices);
                mRectComponent->SetWidth(mWidth);
                mRectComponent->SetHeight(mHeight);
            }

            if (mDrawComponent) {
                mDrawComponent->SetWidth(mWidth);
                mDrawComponent->SetHeight(mHeight);
                // mDrawAnimatedComponent->UseRotation(false);
            }
            offset = mWidth * 0.35f;
        }
        else {
            Vector2 v1(-mHeight / 2, -mWidth / 2);
            Vector2 v2(mHeight / 2, -mWidth / 2);
            Vector2 v3(mHeight / 2, mWidth / 2);
            Vector2 v4(-mHeight / 2, mWidth / 2);
            std::vector<Vector2> vertices;
            vertices.emplace_back(v1);
            vertices.emplace_back(v2);
            vertices.emplace_back(v3);
            vertices.emplace_back(v4);

            if (auto* aabb = dynamic_cast<AABBComponent*>(mAABBComponent)) {
                aabb->SetMin(v1);
                aabb->SetMax(v3);
            }

            if (mCombatBoxComponent) {
                mCombatBoxComponent->SetBoxHalfSize("sword", Vector2(mHeight / 2, mWidth / 2));
                mCombatBoxComponent->SetBoxHalfSize("ground", Vector2((mHeight / 2) * mHurtSizeScale, mWidth / 2));
            }

            if (mRectComponent) {
                // mDrawPolygonComponent->SetVertices(vertices);
                mRectComponent->SetWidth(mWidth);
                mRectComponent->SetHeight(mHeight);
            }

            if (mDrawComponent) {
                mDrawComponent->SetWidth(mWidth);
                mDrawComponent->SetHeight(mHeight);
                // mDrawAnimatedComponent->UseRotation(true);
            }
            offset = mWidth * 0.35f;
        }
        SetPosition(Vector2(mOwner->GetPosition() + GetForward() * offset));
    }
}

void Sword::Activate()
{
    mAABBComponent->SetActive(true); // reativa colisão
    mCombatBoxComponent->SetAllBoxesActive(true);
    if (mRectComponent) {
        mRectComponent->SetVisible(true);
    }

    if (mDrawComponent) {
        mDrawComponent->SetVisible(true);
        mDrawComponent->SetAnimation("slash");
    }
}

void Sword::Deactivate()
{
    SetState(ActorState::Paused);
    mAABBComponent->SetActive(false); // desativa colisão
    mCombatBoxComponent->SetAllBoxesActive(false);
    if (mRectComponent) {
        mRectComponent->SetVisible(false);
    }

    if (mDrawComponent) {
        mDrawComponent->SetVisible(false);
        mDrawComponent->SetAnimation("end");
    }
    mDurationTimer = 0;
}

void Sword::ChangeResolution(float oldScale, float newScale) {
    mWidth = mWidth / oldScale * newScale;
    mHeight = mHeight / oldScale * newScale;
    SetPosition(Vector2(GetPosition().x / oldScale * newScale, GetPosition().y / oldScale * newScale));

    // if (mDrawAnimatedComponent) {
    //     mDrawAnimatedComponent->SetWidth(mWidth);
    //     mDrawAnimatedComponent->SetHeight(mHeight);
    // }

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

    // if (mDrawPolygonComponent) {
    //     mDrawPolygonComponent->SetVertices(vertices);
    // }
}
