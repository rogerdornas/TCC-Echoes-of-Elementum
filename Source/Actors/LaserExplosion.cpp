//
// Created by roger on 06/04/2026.
//

#include "LaserExplosion.h"

#include "ParticleSystem.h"
#include "../Game.h"
#include "../Components/CombatBoxComponent.h"
#include "../Components/Drawing/AnimatorComponent.h"

LaserExplosion::LaserExplosion(Game *game, float width, float damage, float duration)
    :Actor(game)
    ,mWidth(width)
    ,mHeight(width)
    ,mDuration(duration)
    ,mTimer(0.0f)
    ,mDamage(damage)
    ,mCombatBoxComponent(nullptr)
    ,mDrawComponent(nullptr)
{
    mDrawComponent = new AnimatorComponent(this, "../Assets/Sprites/LaserExplosion2/LaserExplosion.png",
                                                "../Assets/Sprites/LaserExplosion2/LaserExplosion.json",
                                                mWidth * 2.5f, mHeight * 2.5f, 97);

    std::vector<int> idle = {0, 1, 2, 3};
    mDrawComponent->AddAnimation("idle", idle);

    mDrawComponent->SetAnimation("idle");
    mDrawComponent->SetAnimFPS(17.0f);

    mCombatBoxComponent = new CombatBoxComponent(this);
    mCombatBoxComponent->AddOBBBox("hitbox", true, Vector2(mWidth * 0.5f, mHeight * 0.5f));
    // mCombatBoxComponent->SetDebugDraw(true);

    ParticleEffect();
}

void LaserExplosion::OnUpdate(float deltaTime) {
    mTimer += deltaTime;
    if (mTimer >= mDuration) {
        SetState(ActorState::Destroy);
        return;
    }

    ResolvePlayerCollision();

    if (mSplash) {
        mSplash->SetPosition(GetPosition());
        mSplash->SetEmitDirection(GetForward() * -1);
    }
}

void LaserExplosion::ResolvePlayerCollision() {
    Player* player = mGame->GetPlayer();
    HitResult hitResult = mCombatBoxComponent->CheckAttackAgainst(player->GetComponent<CombatBoxComponent>());
    if (hitResult.isValid) {
        player->ReceiveHit(mDamage, GetForward());
    }
}

void LaserExplosion::ParticleEffect() {
    mSplash = new ParticleSystem(mGame, Particle::ParticleType::SolidParticle, 10.0f * 1.2f, 10.0f, 200.0f, 0.15f, mDuration);
    mSplash->SetPosition(GetPosition());
    mSplash->SetParticleColor(SDL_Color{12, 101, 169, 255});
    mSplash->SetConeSpread(110.0f);
    mSplash->SetEmitDirection(GetForward() * -1);
    mSplash->SetParticleSpeedScale(0.95f);
    mSplash->SetParticleGravity(false);
    mSplash->SetGroundCollision(false);
    mSplash->SetAdditiveBlending(true);
    mSplash->SetParticleDrawOrder(98);
}
