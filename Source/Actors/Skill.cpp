//
// Created by roger on 30/06/2025.
//

#include "Skill.h"
#include "../Game.h"
#include "../PlayerSkillManager.h"
#include "../Components/AABBComponent.h"
#include "../Components/Drawing/AnimatorComponent.h"
#include "../Components/Drawing/RectComponent.h"
#include "../UIScreens/NewSkillMenu.h"

Skill::Skill(class Game *game, SkillType skill)
    :Actor(game)
    ,mSkill(skill)
    ,mWidth(64)
    ,mHeight(64)
    ,mSkillMessage(nullptr)
    ,mDrawComponent(nullptr)
    ,mRectComponent(nullptr)
{
    // Componente visual
    Vector2 v1(-mWidth/2, -mHeight/2);
    Vector2 v2(mWidth/2, -mHeight/2);
    Vector2 v3(mWidth/2, mHeight/2);
    Vector2 v4(-mWidth/2, mHeight/2);

    std::vector<Vector2> vertices;
    vertices.emplace_back(v1);
    vertices.emplace_back(v2);
    vertices.emplace_back(v3);
    vertices.emplace_back(v4);

    // mDrawPolygonComponent = new DrawPolygonComponent(this, vertices, SDL_Color{255, 255, 0, 255}, 5000);
    mAABBComponent = new AABBComponent(this, v1, v3);

    mDrawComponent = new AnimatorComponent(this, "../Assets/Sprites/Skill/Skill.png",
                                                    "../Assets/Sprites/Skill/Skill.json",
                                                    mWidth * 2.5f, mHeight * 2.5f);

    std::vector idle = {0, 1, 2, 3, 4, 5, 6, 7};
    mDrawComponent->AddAnimation("idle", idle);

    mDrawComponent->SetAnimation("idle");
    mDrawComponent->SetAnimFPS(10.0f);
}

void Skill::OnUpdate(float deltaTime) {
    Player* player = mGame->GetPlayer();

    if (mAABBComponent->Intersect(*player->GetComponent<ColliderComponent>())) {
        SetPlayerSkill();
        new NewSkillMenu(mGame, "../Assets/Fonts/K2D-Bold.ttf", true, mSkill);
        SetState(ActorState::Destroy);
    }
}

void Skill::SetPlayerSkill() {
    Player* player = mGame->GetPlayer();

    switch (mSkill) {
        case SkillType::Dash:
            player->GetSkillManager()->UnlockMechanic("dash");
            mSkill = SkillType::Dash;
            break;

        case SkillType::WallSlide:
            player->GetSkillManager()->UnlockMechanic("wall_slide");
            mSkill = SkillType::WallSlide;
            break;

        case SkillType::DoubleJump:
            player->GetSkillManager()->UnlockMechanic("double_jump");
            mSkill = SkillType::DoubleJump;
            break;

        case SkillType::Hook:
            player->GetSkillManager()->UnlockMechanic("hook");
            mSkill = SkillType::Hook;
        break;

        case SkillType::TimeControl:
            break;
    }
}
