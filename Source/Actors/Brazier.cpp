//
// Created by roger on 20/11/2025.
//

#include "Brazier.h"
#include "Light.h"
#include "Particle.h"
#include "ParticleSystem.h"
#include "../Game.h"
#include "../Random.h"
#include "../Components/AABBComponent.h"
#include "../Components/Drawing/AnimatorComponent.h"
#include "../Components/Drawing/RectComponent.h"

Brazier::Brazier(Game *game, BrazierState state)
    :Actor(game)
    ,mWidth(100)
    ,mHeight(100)
    ,mLight(nullptr)
    ,mRedLight(nullptr)
    ,mLightTimer(0.0f)
    ,mLightRadius(1000.0f)
    ,mLightIntensity(0.7f)
    ,mRedLightRadius(200.0f)
    ,mRedLightIntensity(0.85f)
    ,mBrazierState(state)
    ,mFreezeMax(120.0f)
    ,mFreezeCount(0.0f)
    ,mIntervalBetweenSmokeEmitDuration(0.1f)
    ,mIntervalBetweenSmokeEmitTimer(0.0f)
    ,mAABBComponent(nullptr)
    ,mRectComponent(nullptr)
    ,mDrawComponent(nullptr)
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

    mAABBComponent = new AABBComponent(this, v1, v3);

    // mRectComponent = new RectComponent(this, mWidth, mHeight, RendererMode::LINES);
    // mRectComponent->SetColor(Vector3(255, 0, 0));

    mDrawComponent = new AnimatorComponent(this, "../Assets/Sprites/Brazier/Brazier.png",
                                                "../Assets/Sprites/Brazier/Brazier.json",
                                                mWidth * 1.5f, mWidth * 1.82f * 1.5f, 90);

    std::vector lightOff = {8};
    mDrawComponent->AddAnimation("lightOff", lightOff);

    std::vector lightOn = {0, 1, 2, 3, 4, 5, 6, 7};
    mDrawComponent->AddAnimation("lightOn", lightOn);

    mDrawComponent->SetAnimation("lightOff");
    mDrawComponent->SetAnimFPS(20.0f);

    InitLight();
}

void Brazier::OnUpdate(float deltaTime) {
    std::vector<FireBall* > fireBalls = mGame->GetFireBalls();
    for (FireBall* f: fireBalls) {
        if (mAABBComponent->Intersect(*f->GetComponent<ColliderComponent>())) {
            f->ExplodeFireball();
            if (mBrazierState == BrazierState::LightOff) {
                mBrazierState = BrazierState::LightOn;
                mRedLight->Activate(0.4f);
                mLight->Activate(0.4f);
            }
            break;
        }
    }

    if (mBrazierState == BrazierState::LightOn) {
        if (mIntervalBetweenSmokeEmitTimer >= mIntervalBetweenSmokeEmitDuration) {
            auto* smoke = new ParticleSystem(mGame, Particle::ParticleType::BlurParticle, 80.0f, 25.0f, 0.65f, 0.2f);
            smoke->SetParticleColor(SDL_Color{130, 130, 130, 50});
            smoke->SetConeSpread(60.0f);
            smoke->SetParticleSpeedScale(0.5f);
            smoke->SetParticleGravity(false);
            smoke->SetEmitDirection(Vector2::NegUnitY);
            smoke->SetPosition(GetPosition() - Vector2(0, mHeight * 0.7f));
            smoke->SetGroundCollision(false);

            mIntervalBetweenSmokeEmitTimer = 0;
        }
    }

    if (mIntervalBetweenSmokeEmitTimer < mIntervalBetweenSmokeEmitDuration) {
        mIntervalBetweenSmokeEmitTimer += deltaTime;
    }

    ResolveFreezeParticleCollision();

    if (mLight) {
        mLight->SetPosition(GetPosition() - Vector2(0, mHeight / 2));

        mLightTimer += deltaTime;
        if (mLightTimer > Math::TwoPi * 100.0f) {
            mLightTimer -= Math::TwoPi * 100.0f;
        }

        float pulse = sinf(mLightTimer * 6.0f) * 40.0f;
        float flicker = Random::GetFloatRange(0, 10);
        float intensityPulse = sinf(mLightTimer * 4.5f) * 0.2f;

        float currentRadius = mLightRadius + pulse + flicker;
        float intensity = mLightIntensity + intensityPulse;

        mLight->SetRadius(currentRadius);
        mLight->SetMaxIntensity(intensity);
    }
    if (mRedLight) {
        mRedLight->SetPosition(GetPosition() - Vector2(0, mHeight / 2));
    }

    ManageAnimations();
}

void Brazier::InitLight() {
    mLight = new Light(mGame);
    mLight->SetRadius(mLightRadius);
    mLight->SetMaxIntensity(mLightIntensity);
    mLight->SetColor(Vector3(1.0f, 1.0f, 1.0f));
    if (mBrazierState == BrazierState::LightOff) {
        mLight->Deactivate();
    }
    else {
        mLight->Activate();
    }

    mRedLight = new Light(mGame);
    mRedLight->SetRadius(mRedLightRadius);
    mRedLight->SetMaxIntensity(mRedLightIntensity);
    mRedLight->SetColor(Vector3(0.92f, 0.37f, 0.37f));
    if (mBrazierState == BrazierState::LightOff) {
        mRedLight->Deactivate();
    }
    else {
        mRedLight->Activate();
    }
}

void Brazier::ResolveFreezeParticleCollision() {
    std::vector<Particle*> particles = mGame->GetParticles();
    if (!particles.empty()) {
        for (Particle* p : particles) {
            if (p->GetApplyFreeze() && mAABBComponent->Intersect(*p->GetComponent<ColliderComponent>())) {
                mFreezeCount += p->GetFreezeIntensity();
                if (mFreezeCount >= mFreezeMax) {
                    // congela
                    mBrazierState = BrazierState::LightOff;
                    mRedLight->Deactivate(0.4f);
                    mLight->Deactivate(0.4f);
                    mFreezeCount = 0;
                }
            }
        }
    }
}

void Brazier::ManageAnimations() {
    if (mBrazierState == BrazierState::LightOff) {
        mDrawComponent->SetAnimation("lightOff");
    }
    else if (mBrazierState == BrazierState::LightOn) {
        mDrawComponent->SetAnimation("lightOn");
    }
}
