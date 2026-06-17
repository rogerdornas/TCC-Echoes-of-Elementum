//
// Created by roger on 28/05/2025.
//

#pragma once

#include "Actor.h"
#include "../Actors/Trigger.h"

class Lever : public Trigger
{
public:
    enum class LeverType {
        Lever,
        Crystal,
        Mirror
    };

    Lever(class Game *game, float width = 100.0f, float height = 100.0f, LeverType leverType = LeverType::Lever);

    void OnUpdate(float deltaTime) override;

    void Activate() { mActivate = true; mActivatingTimer = mActivatingDuration; }

private:
    void EnemyTrigger() override;

    LeverType mLeverType;
    float mHealthPoints;
    bool mSwordHit;
    bool mActivate;
    float mActivatingDuration;
    float mActivatingTimer;

    class RectComponent* mRectComponent;
    class AnimatorComponent* mDrawComponent;
};
