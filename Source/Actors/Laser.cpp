//
// Created by roger on 05/04/2026.
//

#include "Laser.h"

#include "LaserExplosion.h"
#include "../Game.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/AABBComponent.h"
#include "../Components/CombatBoxComponent.h"
#include "../Components/Drawing/AnimatorComponent.h"

Laser::Laser(Game *game, float height, float duration, float damage)
    :Actor(game)
    ,mWidth(0.0f)
    ,mHeight(height)
    ,mBaseHeight(height)
    ,mMaxWidth(mBaseHeight * 315.79f)
    ,mExtraWidth(30.0f)
    ,mDuration(duration)
    ,mTimer(0.0f)
    ,mDamage(damage)
    ,mStartPositionOffset(0.0f)
    ,mWidthOffset(mBaseHeight * 1.1f)
    ,mPlayerCollision(true)
    ,mCropBeginning(true)
    ,mShowLaserExplosion(true)
    ,mHitNormal(Vector2::Zero)
    ,mLaserExplosion(nullptr)
    ,mRigidBodyComponent(nullptr)
    ,mCombatBoxComponent(nullptr)
    ,mDrawComponent(nullptr)
{
    mDrawComponent = new AnimatorComponent(this, "../Assets/Sprites/Laser2/Laser.png",
                                                "../Assets/Sprites/Laser2/Laser.json",
                                                mMaxWidth, mHeight * 2.32f, 96);

    std::vector<int> idle = {0, 1, 2, 3, 4, 5};
    mDrawComponent->AddAnimation("idle", idle);

    mDrawComponent->SetAnimation("idle");
    mDrawComponent->SetAnimFPS(6.0f / mDuration);

    mRigidBodyComponent = new RigidBodyComponent(this, 1, 40000, 40000);

    mCombatBoxComponent = new CombatBoxComponent(this);
    mCombatBoxComponent->AddOBBBox("hitbox", true, Vector2(mWidth * 0.5f, mHeight * 0.5f));
    mCombatBoxComponent->AddOBBBox("groundCollision", false, Vector2(mWidth * 0.5f, 6.0f));
    // mCombatBoxComponent->SetDebugDraw(true);
}

void Laser::UpdateLaserTransform() {
    float hitBoxWidth = mWidth - mWidthOffset;
    if (hitBoxWidth < 0.0f) {
        hitBoxWidth = 0.0f;
    }

    float hitBoxOffset = (mWidth - hitBoxWidth) * 0.5f;

    Vector2 directionalOffset = GetForward() * hitBoxOffset;

    mCombatBoxComponent->SetBoxHalfSize("hitbox", Vector2(hitBoxWidth * 0.5f, mHeight * 0.5f));
    mCombatBoxComponent->SetBoxOffset("hitbox", directionalOffset);

    mCombatBoxComponent->SetBoxHalfSize("groundCollision", Vector2(hitBoxWidth * 0.5f, 6.0f));
    mCombatBoxComponent->SetBoxOffset("groundCollision", directionalOffset);

    float offset = mBaseHeight * -1.19f + mStartPositionOffset;

    SetPosition(mStartPosition + GetForward() * (mWidth * 0.5f + offset));
}

void Laser::UpdateLaserExplosion() {
    mLaserExplosion->SetPosition(GetPosition() + GetForward() * (mWidth * 0.5f) - GetForward() * mExtraWidth);

    float finalRotation = GetRotation();

    // Se houve colisão com alguma parede/chão (Normal é diferente de zero)
    if (mHitNormal.LengthSq() > 0.001f) {

        Vector2 inwardNormal = mHitNormal * -1.0f;

        // Descobre o ângulo base da parede (Ex: Chão = PI/2, Teto = -PI/2)
        float baseAngle = Math::Atan2(inwardNormal.y, inwardNormal.x);
        float laserAngle = GetRotation();

        // Calcula a diferença entre onde o laser olha e a parede
        float diff = laserAngle - baseAngle;

        // Normaliza a diferença para ficar no intervalo de -PI a PI
        while (diff <= -Math::Pi) diff += 2.0f * Math::Pi;
        while (diff > Math::Pi) diff -= 2.0f * Math::Pi;

        // Margem de 10 graus
        float maxMargin = Math::ToRadians(10.0f);

        // Prende a rotação da explosão dentro da margem da parede
        diff = Math::Clamp(diff, -maxMargin, maxMargin);

        finalRotation = baseAngle + diff;
    }

    mLaserExplosion->SetRotation(finalRotation);
    mLaserExplosion->SetTransformRotation(finalRotation);
}

void Laser::CalculateInstantWidth() {
    mWidth = mWidthOffset;
    float fastStep = 50.0f;
    mHitNormal = Vector2::Zero;
    Vector2 lastHitNormal = Vector2::Zero;

    const std::vector<Ground*>& grounds = mGame->GetGrounds();
    auto* player = mGame->GetPlayer();

    while (mWidth < mMaxWidth) {
        mWidth += fastStep;
        UpdateLaserTransform(); // Atualiza a hitbox

        bool hitGround = false;

        // Checa se bateu
        if (mCombatBoxComponent->GetBox("groundCollision").collider->Intersect(*player->GetComponent<ColliderComponent>())) {
            hitGround = true;
            lastHitNormal = mCombatBoxComponent->GetBox("groundCollision").collider->CollisionSide(*player->GetComponent<ColliderComponent>());
        }

        if (!hitGround) {
            for (Ground* g : grounds) {
                if (mCombatBoxComponent->GetBox("groundCollision").collider->Intersect(*g->GetComponent<ColliderComponent>())) {
                    hitGround = true;
                    lastHitNormal = mCombatBoxComponent->GetBox("groundCollision").collider->CollisionSide(*g->GetComponent<ColliderComponent>());
                    break;
                }
            }
        }

        // OTIMIZAÇÃO E REFINAMENTO:
        if (hitGround) {
            // Recuo de 10 em 10 para mais precisão
            while (hitGround && mWidth > 0.0f) {
                mWidth -= 10.0f;
                UpdateLaserTransform();

                hitGround = false;

                // Checa se bateu
                if (mCombatBoxComponent->GetBox("groundCollision").collider->Intersect(*player->GetComponent<ColliderComponent>())) {
                    hitGround = true;
                    lastHitNormal = mCombatBoxComponent->GetBox("groundCollision").collider->CollisionSide(*player->GetComponent<ColliderComponent>());
                }

                if (!hitGround) {
                    for (Ground* g : grounds) {
                        if (mCombatBoxComponent->GetBox("groundCollision").collider->Intersect(*g->GetComponent<ColliderComponent>())) {
                            hitGround = true;
                            lastHitNormal = mCombatBoxComponent->GetBox("groundCollision").collider->CollisionSide(*g->GetComponent<ColliderComponent>());
                            break;
                        }
                    }
                }
            }

            // Depois de recuar e achar o ponto logo antes da parede, encerramos a busca
            mHitNormal = lastHitNormal;
            break;
        }
    }

    mWidth += mExtraWidth;

    float offset = mBaseHeight * -1.19f + mStartPositionOffset;
    float minSafeWidth = -offset + mExtraWidth;

    if (mWidth < minSafeWidth) {
        mWidth = minSafeWidth;
    }

    if (mWidth > mMaxWidth) {
        mWidth = mMaxWidth;
    }

    UpdateLaserTransform();
}

void Laser::OnUpdate(float deltaTime) {
    mTimer += deltaTime;
    if (mTimer >= mDuration) {
        SetState(ActorState::Destroy);
        return;
    }

    if (mTimer < mDuration * 0.8f) {
        CalculateInstantWidth();
    }

    // Transparencia
    float alpha = Math::Clamp((mDuration - mTimer) / (mDuration * 0.15f), 0.0f, 1.0f);
    mDrawComponent->SetAlpha(alpha);

    // EFEITO DE PULSAÇÃO DA ALTURA
    float pulseFrequency = 60.0f;
    float pulseAmplitude = mBaseHeight * 0.04f;

    // A altura atual fica oscilando em torno da altura base
    mHeight = mBaseHeight + Math::Sin(mTimer * pulseFrequency) * pulseAmplitude;

    // EFEITO DE ENCOLHER
    float shrinkStartTime = mDuration * 0.8f; // Marca onde começam os últimos 20%
    float shrinkFactor = 1.0f;

    if (mTimer > shrinkStartTime) {
        float timeLeft = mDuration - mTimer;         // Quanto tempo falta para morrer
        float shrinkDuration = mDuration * 0.2f;     // Quanto tempo dura a fase de encolhimento

        // Vai de 1.0 (início do encolhimento) até 0.0 (morte)
        shrinkFactor = timeLeft / shrinkDuration;
    }

    // Aplica o encolhimento na altura do laser (afina o raio)
    mHeight *= shrinkFactor;

    // Atualiza a altura do componente de desenho para refletir a nova altura afinada
    mDrawComponent->SetHeight(mHeight * 2.32f);

    float percentageWidth = mWidth / mMaxWidth;
    if (percentageWidth > 1.0f) percentageWidth = 1.0f;

    if (mCropBeginning) {
        mDrawComponent->SetFrameCrop(0.0f, 0.0f, percentageWidth, 1.0f);
    }
    else {
        mDrawComponent->SetFrameCrop(1.0f - percentageWidth, 0.0f, percentageWidth, 1.0f);
    }

    if (mPlayerCollision) {
        ResolvePlayerCollision();
    }
    if (mShowLaserExplosion && mLaserExplosion) {
        UpdateLaserExplosion();
    }
}

void Laser::Initialize() {
    CalculateInstantWidth();

    float percentageWidth = mWidth / mMaxWidth;
    if (percentageWidth > 1.0f) percentageWidth = 1.0f;

    if (mCropBeginning) {
        mDrawComponent->SetFrameCrop(0.0f, 0.0f, percentageWidth, 1.0f);
    }
    else {
        mDrawComponent->SetFrameCrop(1.0f - percentageWidth, 0.0f, percentageWidth, 1.0f);
    }
}

void Laser::ResolvePlayerCollision() {
    Player* player = mGame->GetPlayer();
    HitResult hitResult = mCombatBoxComponent->CheckAttackAgainst(player->GetComponent<CombatBoxComponent>());
    if (hitResult.isValid) {
        player->ReceiveHit(mDamage, GetForward());
    }
}

void Laser::ShowLaserExplosion(const bool showLaserExplosion) {
    mShowLaserExplosion = showLaserExplosion;
    if (mShowLaserExplosion) {
        mLaserExplosion = new LaserExplosion(mGame, mHeight, mDamage, mDuration * 0.9f);
    }
    else {
        if (mLaserExplosion) {
            mLaserExplosion->SetState(ActorState::Destroy);
            mLaserExplosion = nullptr;
        }
    }
}
