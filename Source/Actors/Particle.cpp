//
// Created by roger on 16/04/2025.
//

#include "Particle.h"
#include "../Actors/ParticleSystem.h"
#include "../Game.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/AABBComponent.h"
#include "../Random.h"
#include "../Components/Drawing/AnimatorComponent.h"
#include "../Components/Drawing/RectComponent.h"

Particle::Particle(Game* game, ParticleType particleType)
    :Actor(game)
    ,mParticleType(particleType)
    ,mWidth(8.0f)
    ,mHeight(8.0f)
    ,mLifeTDuration(0.0f)
    ,mLifeTimer(0.0f)
    ,mGroundCollision(true)
    ,mEnemyCollision(false)
    ,mTexturePath("../Assets/Sprites/Effects/ImperfectCircleBlur.png")
    ,mColor(SDL_Color{255, 255, 255, 255})
    ,mGravity(true)
    ,mGravityForce(2000.0f)
    ,mDirection(Vector2::Zero)
    ,mSpeedScale(1.0f)
    ,mApplyDamage(false)
    ,mApplyFreeze(false)
    ,mFadeIn(false)
    ,mAutoRotate(false)
    ,mRotationSpeed(10.0f)
    ,mFreezeDamage(1.0f)
    ,mFreezeIntensity(1.2f)
    ,mDrawComponent(nullptr)
    ,mRectComponent(nullptr)
    ,mRigidBodyComponent(nullptr)
    ,mAABBComponent(nullptr)
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

    switch (mParticleType) {
        case ParticleType::SolidParticle:
            mDrawComponent = new AnimatorComponent(this, "../Assets/Sprites/Particle/Ellipse.png", "",
                                                        static_cast<int>(mWidth * 1.6),
                                                        static_cast<int>(mHeight * 1.6),
                                                    5000);

            mDrawComponent->SetTextureFactor(0.0f);
            break;

        case ParticleType::BlurParticle:
            mDrawComponent = new AnimatorComponent(this, "../Assets/Sprites/Particle/ImperfectCircleBlur.png", "",
                                                        static_cast<int>(mWidth * 1.6),
                                                        static_cast<int>(mHeight * 1.6),
                                                    5000);

            mDrawComponent->SetTextureFactor(0.0f);
            break;

        case ParticleType::Grass:
            mDrawComponent = new AnimatorComponent(this, "../Assets/Sprites/Particle/Grass5.png", "",
                                                        static_cast<int>(mWidth * 1.6),
                                                        static_cast<int>(mHeight * 1.6),
                                                    5000);

            mDrawComponent->SetTextureFactor(1.0f);
            break;
    }

    SetRotation(Random::GetFloatRange(0, Math::TwoPi));

    mRigidBodyComponent = new RigidBodyComponent(this, 0.1);
    mAABBComponent = new AABBComponent(this, v1, v3);

    mGame->AddParticle(this);
}

Particle::~Particle() {
    mGame->RemoveParticle(this);
}

void Particle::SetSize(float width, float height) {
    float scale = Random::GetFloatRange(0.5, 1.5);
    mWidth = width * scale;
    mHeight = height * scale;
}

void Particle::SetRotationSpeed(float rotationSpeed) {
    mRotationSpeed = Random::GetFloatRange(rotationSpeed * 0.5, rotationSpeed * 1.5);
}

void Particle::SetDirection(Vector2 direction) {
    mDirection = direction;
    if (mDirection.x == 1.0f) {
        // Right
        Vector2 minForce = Vector2(300.0f * mSpeedScale, -800.0f * mSpeedScale);
        Vector2 maxForce = Vector2(800.0f * mSpeedScale, -140.0f * mSpeedScale);

        Vector2 randForce = Random::GetVector(minForce, maxForce);
        mRigidBodyComponent->SetVelocity(mRigidBodyComponent->GetVelocity() + randForce);
    }
    else if (mDirection.x == -1.0f) {
        // Left
        Vector2 minForce = Vector2(-300.0f * mSpeedScale, -800.0f * mSpeedScale);
        Vector2 maxForce = Vector2(-800.0f * mSpeedScale, -140.0f * mSpeedScale);

        Vector2 randForce = Random::GetVector(minForce, maxForce);
        mRigidBodyComponent->SetVelocity(mRigidBodyComponent->GetVelocity() + randForce);
    }
    else if (mDirection.y == -1.0f) {
        // Up
        Vector2 minForce = Vector2(-300.0f * mSpeedScale, -1300.0f * mSpeedScale);
        Vector2 maxForce = Vector2(300.0f * mSpeedScale, -450.0f * mSpeedScale);

        Vector2 randForce = Random::GetVector(minForce, maxForce);
        mRigidBodyComponent->SetVelocity(mRigidBodyComponent->GetVelocity() + randForce);
    }
    else if (mDirection.y == 1.0f) {
        // down
        Vector2 minForce = Vector2(-300.0f * mSpeedScale, -800.0f * mSpeedScale);
        Vector2 maxForce = Vector2(300.0f * mSpeedScale, 140.0f * mSpeedScale);

        Vector2 randForce = Random::GetVector(minForce, maxForce);
        mRigidBodyComponent->SetVelocity(mRigidBodyComponent->GetVelocity() + randForce);
    }
    else {
        Vector2 minForce = Vector2(-300.0f * mSpeedScale, -300.0f * mSpeedScale);
        Vector2 maxForce = Vector2(300.0f * mSpeedScale, 300.0f * mSpeedScale);

        Vector2 randForce = Random::GetVector(minForce, maxForce);
        mRigidBodyComponent->SetVelocity(mRigidBodyComponent->GetVelocity() + randForce);
    }
}

void Particle::SetVelocity(Vector2 velocity) {
    if (mRigidBodyComponent) {
        mRigidBodyComponent->SetVelocity(velocity);
    }
}

void Particle::UpdateFade() {
    float t = mLifeTimer / mLifeTDuration;

    float alphaCurve = 1.0f - (t * t * t * t);

    // Lógica de Fade In
    if (mFadeIn) {
        // Multiplica por 5 para que t=0.2 (20% da vida) resulte em 1.0.
        float fadeInFactor = std::min(1.0f, t * 5.0f);
        alphaCurve *= fadeInFactor;
    }

    if (mDrawComponent) {
        // Aplica a curva à cor original
        float alpha = (mColor.a / 255.0f) * alphaCurve;

        // Garante que não fique negativo ou estoure (opcional, mas seguro)
        alpha = std::max(0.0f, std::min(alpha, 1.0f));

        mDrawComponent->SetAlpha(alpha);
    }
}

void Particle::OnUpdate(float deltaTime)
{
    mLifeTimer += deltaTime;
    if (mLifeTimer >= mLifeTDuration) {
        Deactivate();
    }
    else {
        Activate();

        UpdateFade();

        // Rotation
        if (mAutoRotate) {
            float currentRotation = GetRotation();
            float newRotation = currentRotation + (mRotationSpeed * deltaTime);
            SetRotation(newRotation);
            SetTransformRotation(newRotation);
        }
        else {
            Vector2 velocity = mRigidBodyComponent->GetVelocity();
            if (velocity.Length() != 0) {
                velocity.Normalize();
            }
            SetRotation(Math::Atan2(velocity.y, velocity.x));
            SetTransformRotation(Math::Atan2(velocity.y, velocity.x));
        }

        // Gravidade
        if (mGravity) {
            mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x,
                                                     mRigidBodyComponent->GetVelocity().y +
                                                     mGravityForce * deltaTime));
        }

        if (mApplyDamage) {
            ApplyEnemyDamage();
        }
        if (mApplyFreeze) {
            ApplyEnemyFreeze();
        }

        if (mGroundCollision) {
            std::vector<Ground*> grounds = mGame->GetGrounds();
            if (!grounds.empty()) {
                for (Ground* g : grounds) {
                    if (mAABBComponent->Intersect(*g->GetComponent<ColliderComponent>())) {
                        Deactivate();
                        auto* blood = new ParticleSystem(mGame, mParticleType, 6.0f * 1.2f, 6.0f, 100.0, 0.09, 0.05f);
                        blood->SetPosition(GetPosition());
                        blood->SetGroundCollision(false);
                        blood->SetParticleSpeedScale(0.3f);
                        blood->SetParticleColor(mColor);
                        blood->SetParticleGravity(true);
                        blood->SetConeSpread(360.0f);
                    }
                }
            }
        }
        if (mEnemyCollision) {
            std::vector<Enemy*> enemies = mGame->GetEnemies();
            if (!enemies.empty()) {
                for (Enemy* e : enemies) {
                    if (mAABBComponent->Intersect(*e->GetComponent<ColliderComponent>())) {
                        Deactivate();
                        auto* blood = new ParticleSystem(mGame, mParticleType, 6.0f * 1.2f, 6.0f, 100.0, 0.09, 0.05f);
                        blood->SetPosition(GetPosition());
                        blood->SetGroundCollision(false);
                        blood->SetParticleSpeedScale(0.3f);
                        blood->SetParticleColor(mColor);
                        blood->SetParticleGravity(true);
                        blood->SetConeSpread(360.0f);
                    }
                }
            }
        }
    }
}

void Particle::Activate() {
    // if (mDrawParticleComponent) {
    //     mDrawParticleComponent->SetColor(mColor);
    // }
    if (mDrawComponent) {
        mDrawComponent->SetColor(Vector3(mColor.r / 255.0f, mColor.g / 255.0f, mColor.b / 255.0f));
        mDrawComponent->SetAlpha(mColor.a / 255.0f);
    }

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

    if (auto* aabb = dynamic_cast<AABBComponent*>(mAABBComponent)) {
        aabb->SetMin(v1);
        aabb->SetMax(v3);
    }

    mAABBComponent->SetActive(true); // reativa colisão
    if (mRectComponent) {
        // mDrawPolygonComponent->SetVertices(vertices);
        mRectComponent->SetWidth(mWidth);
        mRectComponent->SetHeight(mHeight);
        mRectComponent->SetVisible(true);
    }
    if (mDrawComponent) {
        mDrawComponent->SetWidth(mWidth * 1.6);
        mDrawComponent->SetHeight(mHeight * 1.6);
        mDrawComponent->SetVisible(true);
    }
}


void Particle::Deactivate() {
    SetState(ActorState::Paused);
    mLifeTimer = 0;
    mRigidBodyComponent->SetVelocity(Vector2::Zero);
    mAABBComponent->SetActive(false); // desativa colisão
    mDrawComponent->SetAdditiveBlending(false);
    if (mRectComponent) {
        mRectComponent->SetVisible(false);
    }
    if (mDrawComponent) {
        mDrawComponent->SetVisible(false);
    }
}

void Particle::ApplyEnemyDamage() {
    std::vector<Enemy*> enemies = mGame->GetEnemies();
    if (!enemies.empty()) {
        for (Enemy* e : enemies) {
            if (mAABBComponent->Intersect(*e->GetComponent<ColliderComponent>())) {
                e->ReceiveHit(1.0f, Vector2::Zero);
            }
        }
    }
}

void Particle::ApplyEnemyFreeze() {
    std::vector<Enemy*> enemies = mGame->GetEnemies();
    if (!enemies.empty()) {
        for (Enemy* e : enemies) {
            if (mAABBComponent->Intersect(*e->GetComponent<ColliderComponent>())) {
                e->ReceiveFreeze(mFreezeDamage, mFreezeIntensity);
            }
        }
    }
}
