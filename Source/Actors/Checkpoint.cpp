//
// Created by roger on 18/06/2025.
//

#include "Checkpoint.h"
#include "../Game.h"
#include "../Components/AABBComponent.h"
#include "../Components/Drawing/AnimatorComponent.h"
#include "../Components/Drawing/RectComponent.h"
#include "../UIElements/UIScreen.h"
#include "../UIScreens/CheckPointMenu.h"

Checkpoint::Checkpoint(class Game *game, float width, float height, Vector2 position)
    :Actor(game)
    ,mWidth(width)
    ,mHeight(height)
    ,mDrawComponent(nullptr)
    ,mAABBComponent(nullptr)
{
    SetPosition(position);

    Vector2 v1(-mWidth / 2, -mHeight / 2);
    Vector2 v2(mWidth / 2, -mHeight / 2);
    Vector2 v3(mWidth / 2, mHeight / 2);
    Vector2 v4(-mWidth / 2, mHeight / 2);

    std::vector<Vector2> vertices;
    vertices.emplace_back(v1);
    vertices.emplace_back(v2);
    vertices.emplace_back(v3);
    vertices.emplace_back(v4);

    mAABBComponent = new AABBComponent(this, v1, v3);

    // mDrawComponent = new AnimatorComponent(this, "../Assets/Sprites/CheckPoint/CheckPoint.png", \
    //                                         "../Assets/Sprites/CheckPoint/CheckPoint.json",
    //                                         mWidth * 1.1f, mHeight * 1.1f, 201);

    mDrawComponent = new AnimatorComponent(this, "../Assets/Sprites/CheckPoint/checkpointKaren.png", \
                                        "",
                                        mWidth * 1.1f, mHeight * 1.1f, 201);

    // std::vector<int> idle = {0, 1, 2, 3, 4, 5};
    // mDrawComponent->AddAnimation("idle", idle);
    //
    // mDrawComponent->SetAnimation("idle");
    // mDrawComponent->SetAnimFPS(10.0f);
}

void Checkpoint::OnProcessInput(const Uint8 *keyState, SDL_GameController &controller) {
    Player* player = mGame->GetPlayer();
    if (player->GetIsOnGround() && mAABBComponent->Intersect(*player->GetComponent<ColliderComponent>())) {
        if (mGame->IsActionPressed(Game::Action::OpenStore, keyState, &controller)) {
            mGame->SetCheckPointPosition(GetPosition());
            mGame->SetCheckpointGameScenePath(mGame->GetCurrentLevelPath());
            mGame->SetCheckPointMoney(player->GetMoney());

            new CheckPointMenu(mGame, "../Assets/Fonts/K2D-Bold.ttf");
        }
    }
}
