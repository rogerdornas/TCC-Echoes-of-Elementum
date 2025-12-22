//
// Created by roger on 18/12/2025.
//

#include "LightningBarrier.h"
#include "../Game.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/AABBComponent.h"
#include "../Components/Drawing/AnimatorComponent.h"
#include "../Components/Drawing/RectComponent.h"

LightningBarrier::LightningBarrier(Game *game, float width, float height, bool isMoving, float movingDuration, Vector2 velocity)
    :Actor(game)
    ,mWidth(width)
    ,mHeight(height)
    ,mDamage(10.0f)
    ,mIsMoving(isMoving)
    ,mMovingDuration(movingDuration)
    ,mMovingTimer(0.0f)
    ,mVelocity(velocity * mGame->GetScale())
    ,mAlreadyHitPlayer(false)

    ,mRectComponent(nullptr)
    ,mDrawComponent(nullptr)
    ,mRigidBodyComponent(nullptr)
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

    // mRectComponent = new RectComponent(this, mWidth, mHeight, RendererMode::LINES);
    // mRectComponent->SetColor(Vector3(255, 0, 0));

    mDrawComponent = new AnimatorComponent(this, "../Assets/Sprites/LightningBarrier/LightningBarrier.png",
                                                "../Assets/Sprites/LightningBarrier/LightningBarrier.json",
                                                mWidth * 1.2f, mHeight * 1.1f, 98);

    std::vector<int> idle = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
    mDrawComponent->AddAnimation("idle", idle);

    mDrawComponent->SetAnimation("idle");
    mDrawComponent->SetAnimFPS(14.0f);

    mRigidBodyComponent = new RigidBodyComponent(this, 1, 40000, 40000);
    mAABBComponent = new AABBComponent(this, v1, v3);

    if (mIsMoving) {
        mRigidBodyComponent->SetVelocity(mVelocity);
    }
}

void LightningBarrier::OnUpdate(float deltaTime) {
    if (mIsMoving) {
        mMovingTimer += deltaTime;
        if (mMovingTimer > mMovingDuration) {
            mVelocity *= -1;
            mRigidBodyComponent->SetVelocity(mVelocity);
            mMovingTimer = 0;
        }
    }

    ResolveEnemyCollision();
    ResolvePlayerCollision();
}

void LightningBarrier::SetIsMoving(bool isMoving) {
    mIsMoving = isMoving;
    if (mIsMoving == true) {
        mRigidBodyComponent->SetVelocity(mVelocity);
    }
    else {
        mRigidBodyComponent->SetVelocity(Vector2::Zero);
    }
}

void LightningBarrier::ResolveEnemyCollision() {
    std::vector<Enemy*> enemies = mGame->GetEnemies();
    if (!enemies.empty()) {
        for (Enemy* e : enemies) {
            if (mAABBComponent->Intersect(*e->GetComponent<ColliderComponent>())) {
                e->ReceiveHit(mDamage * 1000, Vector2::UnitY);
            }
        }
    }
}

void LightningBarrier::ResolvePlayerCollision() {
    Player* player = mGame->GetPlayer();
    if (player->IsLightningDashing()) {
        return;
    }

    if (mAABBComponent->Intersect(*player->GetComponent<ColliderComponent>())) {
        if (!mAlreadyHitPlayer) {
            mAlreadyHitPlayer = true;
            player->Stop();
            player->SetIsInvulnerable(false);
            Vector2 knockBackDirection;
            if (player->GetPosition().x <= GetPosition().x) {
                knockBackDirection = Vector2::NegUnitX;
            }
            else {
                knockBackDirection = Vector2::UnitX;
            }
            player->ReceiveHit(mDamage, knockBackDirection, DamageType::Environment);
        }
    }
    else {
        mAlreadyHitPlayer = false;
    }
}
