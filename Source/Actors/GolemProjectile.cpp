//
// Created by roger on 06/04/2026.
//

#include "GolemProjectile.h"
#include "Light.h"
#include "LightningEffect.h"
#include <cmath>

#include "ParticleSystem.h"
#include "../Game.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/OBBComponent.h"
#include "../Components/CombatBoxComponent.h"
#include "../Components/Drawing/AnimatorComponent.h"

GolemProjectile::GolemProjectile(Game *game)
    :Actor(game)
    ,mWidth(95.0f)
    ,mHeight(38.0f)
    ,mSpeed(700.0f)
    ,mDuration(30.0f)
    ,mTimer(0.0f)
    ,mDamage(10.0f)
    ,mTurnSpeed(4.5f)
    ,mDrawComponent(nullptr)
    ,mRigidBodyComponent(nullptr)
    ,mOBBComponent(nullptr)
    ,mCombatBoxComponent(nullptr)
{
    mDrawComponent = new AnimatorComponent(this,
                                        "../Assets/Sprites/GolemProjectile/GolemProjectile.png",
                                        "../Assets/Sprites/GolemProjectile/GolemProjectile.json",
                                         mWidth * 1.3f, mHeight * 1.3f, 1001);

    std::vector<int> idle = {0, 1, 2, 3, 5, 4};
    mDrawComponent->AddAnimation("idle", idle);

    mDrawComponent->SetAnimation("idle");
    mDrawComponent->SetAnimFPS(8.0f);

    mRigidBodyComponent = new RigidBodyComponent(this, 1, 40000, 40000);

    mOBBComponent = new OBBComponent(this, Vector2(mWidth * 0.5f, mHeight * 0.5f));

    mCombatBoxComponent = new CombatBoxComponent(this);
    mCombatBoxComponent->AddOBBBox("hitbox", true, Vector2(mWidth * 0.5f, mHeight * 0.5f));
    // mCombatBoxComponent->SetDebugDraw(true);
}

void GolemProjectile::OnUpdate(float deltaTime) {
    mTimer += deltaTime;
    if (mTimer >= mDuration) {
        Destroy();
        return;
    }

    Player* player = mGame->GetPlayer();
    if (player) {
        Vector2 toPlayer = player->GetPosition() - GetPosition();

        if (toPlayer.LengthSq() > 0.0001f) {
            toPlayer.Normalize();

            float baseTargetAngle = Math::Atan2(toPlayer.y, toPlayer.x);

            // GERAÇÃO DE RUÍDO
            float noise = (Math::Sin(mTimer * 3.0f) * 0.3f) + (Math::Cos(mTimer * 3.3f) * 0.1f);

            float targetAngle = baseTargetAngle + noise;

            float currentAngle = GetRotation();
            float diff = targetAngle - currentAngle;

            // Normaliza a diferença
            while (diff < -Math::Pi) diff += 2.0f * Math::Pi;
            while (diff > Math::Pi) diff -= 2.0f * Math::Pi;

            // Interpola o ângulo
            currentAngle += diff * mTurnSpeed * deltaTime;

            SetRotation(currentAngle);
            SetTransformRotation(currentAngle);
        }
    }

    if (mRigidBodyComponent) {
        mRigidBodyComponent->SetVelocity(GetForward() * mSpeed);
    }

    ResolveGroundCollision();
    ResolvePlayerCollision();
}

void GolemProjectile::ResolveGroundCollision() {
    std::vector<Ground*> grounds = mGame->GetGrounds();
    if (!grounds.empty()) {
        for (Ground* g : grounds) {
            if (mOBBComponent->Intersect(*g->GetComponent<ColliderComponent>())) {
                Destroy();
            }
        }
    }
}

void GolemProjectile::ResolvePlayerCollision() {
    Player* player = mGame->GetPlayer();
    HitResult hitResult = mCombatBoxComponent->CheckAttackAgainst(player->GetComponent<CombatBoxComponent>());
    if (hitResult.isValid) {
        player->ReceiveHit(mDamage, GetForward());
        Destroy();
    }
}

void GolemProjectile::Destroy() {
    SetState(ActorState::Destroy);

    auto* smoke = new ParticleSystem(mGame, Particle::ParticleType::BlurParticle, 80.0f, 25.0f, 0.35f, 0.3f);
    smoke->SetParticleColor(SDL_Color{12, 101, 169, 50});
    smoke->SetConeSpread(360.0f);
    smoke->SetParticleSpeedScale(0.2f);
    smoke->SetParticleGravity(false);
    smoke->SetEmitDirection(Vector2::Zero);
    smoke->SetPosition(GetPosition());
    smoke->SetGroundCollision(false);
}
