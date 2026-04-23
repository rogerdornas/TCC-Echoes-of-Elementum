//
// Created by roger on 16/03/2026.
//

#include "LightningSpear.h"
#include "Light.h"
#include "LightningEffect.h"
#include <cmath>
#include "../Game.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/AABBComponent.h"
#include "../Components/CombatBoxComponent.h"
#include "../Components/Drawing/AnimatorComponent.h"
#include "../Components/Drawing/GhostTrailComponent.h"

LightningSpear::LightningSpear(Game *game)
    :Actor(game)
    ,mWidth(150.0f)
    ,mHeight(30.0f)
    ,mSpeed(3500.0f)
    ,mDuration(3.0f)
    ,mTimer(mDuration)
    ,mDamage(15.0f)
    ,mLightningEffect(nullptr)
    ,mLightningEffectWidth(250.0f)
    ,mNumEnemiesHit(0)
    ,mMaxNumEnemiesHit(6)
    ,mShockChainRange(500.0f)
    ,mDrawComponent(nullptr)
    ,mRigidBodyComponent(nullptr)
    ,mAABBComponent(nullptr)
    ,mCombatBoxComponent(nullptr)
    ,mGhostTrailComponent(nullptr)
{
    mDrawComponent = new AnimatorComponent(this,
                                        "../Assets/Sprites/LightningSpear/LightningSpear.png",
                                        "../Assets/Sprites/LightningSpear/LightningSpear.json",
                                         mWidth * 1.2f, mWidth * 1.11f * 1.2f, 1001);

    std::vector<int> idle = {0};
    mDrawComponent->AddAnimation("idle", idle);

    mDrawComponent->SetAnimation("idle");
    mDrawComponent->SetAnimFPS(2.0f);

    mGhostTrailComponent = new GhostTrailComponent(this, mDrawComponent);
    mGhostTrailComponent->SetSpawnInterval(0.01f);
    mGhostTrailComponent->SetGhostDuration(0.1f);

    mLightningEffect = new LightningEffect(mGame, this, mDuration);
    mLightningEffect->SetNumBolts(4);
    mLightningEffect->SetSpeadRadius(25.0f);
    mLightningEffect->SetGenerations(3);
    mLightningEffect->SetMaxOffset(40.0f);
    mLightningEffect->SetGlowThickness(5.0f);
    mLightningEffect->SetCoreThickness(1.0f);

    Vector2 v1(-mWidth/2, -mHeight/2);
    Vector2 v2(mWidth/2, -mHeight/2);
    Vector2 v3(mWidth/2, mHeight/2);
    Vector2 v4(-mWidth/2, mHeight/2);

    mRigidBodyComponent = new RigidBodyComponent(this, 1, 40000, 1800);
    mAABBComponent = new AABBComponent(this, v1, v3);

    mCombatBoxComponent = new CombatBoxComponent(this);
    mCombatBoxComponent->AddAABBBox("hitbox", true, v1, v3);
    // mCombatBoxComponent->SetDebugDraw(true);
}

void LightningSpear::OnUpdate(float deltaTime) {
    mTimer += deltaTime;
    if (mTimer >= mDuration) {
        Deactivate();
        return;
    }

    mLightningEffect->SetStartPosition(GetPosition() - Vector2(mWidth * 0.4f, 0) * GetForward());
    mLightningEffect->SetEndPosition(GetPosition() - Vector2(mLightningEffectWidth, 0) * GetForward());

    ResolveGroundCollision();
    ResolveEnemyCollision();
}

void LightningSpear::Activate() {
    mTimer = 0;
    mEnemiesHit.clear();
    mNumEnemiesHit = 0;

    if (mDrawComponent) {
        mDrawComponent->SetVisible(true);
    }
    if (mGhostTrailComponent) {
        mGhostTrailComponent->SetIsEmitting(true);
    }
    mLightningEffect->StartEffect(GetPosition() - Vector2(mWidth * 0.4f, 0) * GetForward(), GetPosition() - Vector2(mLightningEffectWidth, 0) * GetForward());

    mAABBComponent->SetActive(true);
    mCombatBoxComponent->SetAllBoxesActive(true);
    mRigidBodyComponent->SetVelocity(GetForward() * mSpeed);
}

void LightningSpear::Deactivate() {
    mAABBComponent->SetActive(false);
    mCombatBoxComponent->SetAllBoxesActive(false);
    mRigidBodyComponent->SetVelocity(Vector2::Zero);
    mLightningEffect->StopEffect();
    if (mDrawComponent) {
        mDrawComponent->SetVisible(false);
    }
    if (mGhostTrailComponent) {
        mGhostTrailComponent->SetIsEmitting(false);
    }
}

void LightningSpear::ResolveGroundCollision() {
    std::vector<Ground*> grounds = mGame->GetGrounds();
    if (!grounds.empty()) {
        for (Ground* g : grounds) {
            if (mAABBComponent->Intersect(*g->GetComponent<ColliderComponent>())) {
                Vector2 startPosition(GetPosition() + GetForward() * Vector2(mWidth * 0.35f, 0) - Vector2(40.0f, 0.0f));
                Vector2 endPosition(GetPosition() + GetForward() * Vector2(mWidth * 0.35f, 0) + Vector2(40.0f, 0.0f));
                auto* explosion = new LightningEffect(mGame, this, 0.2f);
                explosion->StartEffect(startPosition, endPosition);
                explosion->SetLightningGenerationIntervalDuration(0.05f);
                explosion->SetNumBolts(8);
                explosion->SetSpeadRadius(5.0f);
                explosion->SetGenerations(4);
                explosion->SetMaxOffset(50.0f);
                explosion->SetGlowThickness(5.0f);
                explosion->SetCoreThickness(1.0f);
                Deactivate();
            }
        }
    }
}

void LightningSpear::ResolveEnemyCollision() {
    std::vector<Enemy*> enemies = mGame->GetEnemies();
    if (enemies.empty()) return;

    for (Enemy* e : enemies) {
        if (std::find(mEnemiesHit.begin(), mEnemiesHit.end(), e) != mEnemiesHit.end()) {
            continue;
        }

        HitResult hitResult = mCombatBoxComponent->CheckAttackAgainst(e->GetComponent<CombatBoxComponent>());
        if (hitResult.isValid) {
            ProcessChainLightning(e);
            break;
        }
    }
}

void LightningSpear::ProcessChainLightning(Enemy* firstTarget) {
    // Esconde e para a lança principal instantaneamente
    if (mDrawComponent) {
        mDrawComponent->SetVisible(false);
    }
    if (mGhostTrailComponent) {
        mGhostTrailComponent->SetIsEmitting(false);
    }
    mRigidBodyComponent->SetVelocity(Vector2::Zero);
    mAABBComponent->SetActive(false);
    mCombatBoxComponent->SetAllBoxesActive(false);
    mLightningEffect->StopEffect();

    Enemy* currentTarget = firstTarget;

    currentTarget->ReceiveHit(mDamage * mGame->GetPlayer()->GetSkillManager()->GetLightningSpearDamageMultiplier(), Vector2::Zero, false);
    mEnemiesHit.push_back(currentTarget);
    mNumEnemiesHit = 1;

    if (mGame->GetPlayer()->GetSkillManager()->CanLightningSpearChainShock()) {
        while (mNumEnemiesHit < mMaxNumEnemiesHit) {
            // Efeito no alvo
            currentTarget->Stun();

            Enemy* nextTarget = FindClosestEnemy(currentTarget->GetPosition());
            if (!nextTarget) {
                break;
            }

            // Calcula o dano reduzido (perde 15% a cada pulo)
            float currentDamage = mDamage * mGame->GetPlayer()->GetSkillManager()->GetLightningSpearDamageMultiplier() * std::pow(0.85f, mNumEnemiesHit);

            nextTarget->ReceiveHit(currentDamage, Vector2::Zero, false);

            // Cria a ponte de raio entre os dois inimigos!
            SpawnChainVisual(currentTarget->GetPosition(), nextTarget->GetPosition());

            // Atualiza a lista para o próximo pulo
            mEnemiesHit.push_back(nextTarget);
            mNumEnemiesHit++;
            currentTarget = nextTarget;
        }
    }

    Deactivate();
}

void LightningSpear::SpawnChainVisual(Vector2 startPos, Vector2 endPos) {
    // Cria um EFEITO NOVO que dura apenas 0.3 segundos na tela
    LightningEffect* chainLink = new LightningEffect(mGame, this, 0.1f, true);

    chainLink->StartEffect(startPos, endPos);
    chainLink->SetNumBolts(4);
    chainLink->SetSpeadRadius(5.0f);
    chainLink->SetGenerations(4);
    chainLink->SetMaxOffset(40.0f);
    chainLink->SetGlowThickness(5.0f);
    chainLink->SetCoreThickness(1.0f);
}

// 3. O RADAR DE INIMIGOS PRÓXIMOS
Enemy* LightningSpear::FindClosestEnemy(const Vector2& fromPos) {
    Enemy* closestEnemy = nullptr;
    float minDistanceSq = mShockChainRange * mShockChainRange;

    for (Enemy* e : mGame->GetEnemies()) {
        if (std::find(mEnemiesHit.begin(), mEnemiesHit.end(), e) != mEnemiesHit.end()) continue;
        if (e->GetState() == ActorState::Destroy) continue;

        float distSq = (e->GetPosition() - fromPos).LengthSq();
        if (distSq < minDistanceSq) {
            minDistanceSq = distSq;
            closestEnemy = e;
        }
    }
    return closestEnemy;
}