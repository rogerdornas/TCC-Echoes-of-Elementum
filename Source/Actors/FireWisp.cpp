//
// Created by roger on 10/03/2026.
//

#include "FireWisp.h"
#include "../Game.h"
#include "Light.h"
#include "Particle.h"
#include "ParticleSystem.h"
#include "../PlayerSkillManager.h"
#include "../Random.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/CombatBoxComponent.h"

FireWisp::FireWisp(Game *game)
    :Actor(game)
    ,mFireWispState(State::Idle)

    ,mWidth(20.0f)
    ,mHeight(20.0f)

    ,mLight(nullptr)
    ,mCoreLight(nullptr)
    ,mLightTimer(0.0f)
    ,mLightRadius(910.0f)
    ,mLightIntensity(0.8f)
    ,mCoreLightRadius(260.0f)
    ,mCoreLightIntensity(0.95f)
    ,mSpeed(400.0f)
    ,mLerpSpeed(4.0f)
    ,mOffsetPosition(Vector2(60, -60))
    ,mDuration(10.0f)
    ,mHoverTimer(0.0f)

    ,mCanAttackEnemies(true)
    ,mDamage(10.0f)
    ,mAttackRange(350.0f)
    ,mAttackIntervalDuration(1.0f)
    ,mAttackIntervalTimer(mAttackIntervalDuration * 0.5f)
    ,mWindupDuration(0.55f)
    ,mWindupTimer(0.0f)
    ,mAttackSpeed(1600.0f)
    ,mTargetEnemy(nullptr)

    ,mExplosionDuration(0.5f)
    ,mExplosionTimer(0.0f)
    ,mAttackDuration(0.6f)
    ,mAttackTimer(0.0f)
    ,mColorNormal(1.0f, 0.8f, 0.75f)
    ,mColorWindup(0.8f, 0.9f, 1.0f)
    ,mColorExplosion(1.0f, 0.6f, 0.4f)

    ,mSmoke(nullptr)
    ,mSpark(nullptr)
    ,mFlame(nullptr)
    ,mRigidBodyComponent(nullptr)
    ,mCombatBoxComponent(nullptr)
{
    mRigidBodyComponent = new RigidBodyComponent(this, 1, 40000, 40000);

    mCombatBoxComponent = new CombatBoxComponent(this);
    mCombatBoxComponent->AddAABBBox("fire", true, Vector2(-mWidth / 2, -mHeight / 2), Vector2(mWidth / 2, mHeight / 2));
    // mCombatBoxComponent->SetDebugDraw(true);

    SetPosition(mGame->GetPlayer()->GetPosition());
    InitParticles();
    InitLight();
}

void FireWisp::OnUpdate(float deltaTime) {
    mDuration -= deltaTime;
    if (mDuration < 0) {
        mLight->Deactivate(0.4f);
        mCoreLight->Deactivate(0.4f);
        SetState(ActorState::Destroy);
    }

    if (mFireWispState == State::Idle) {
        mHoverTimer += deltaTime;
        if (mHoverTimer > Math::TwoPi * 100.0f) {
            mHoverTimer -= Math::TwoPi * 100.0f;
        }

        auto* player = mGame->GetPlayer();
        Vector2 playerPos = player->GetPosition();
        Vector2 baseTarget = playerPos + mOffsetPosition * Vector2(player->GetForward().x, 1);

        // Movimento suave principal (Lento e amplo)
        float hoverX = cosf(mHoverTimer * 1.5f) * 25.0f; // Vai 25 pixels para frente/trás
        float hoverY = sinf(mHoverTimer * 2.0f) * 30.0f; // Vai 30 pixels para cima/baixo

        // Movimento caótico secundário (Rápido e curto - simula a agitação do fogo)
        float jitterX = cosf(mHoverTimer * 3.7f) * 8.0f;
        float jitterY = sinf(mHoverTimer * 5.3f) * 8.0f;

        Vector2 organicOffset(hoverX + jitterX, hoverY + jitterY);
        Vector2 finalTargetPos = baseTarget + organicOffset;

        mRigidBodyComponent->SetVelocity((finalTargetPos - GetPosition()) * mLerpSpeed);
    }
    else if (mFireWispState == State::Windup) {
        mWindupTimer += deltaTime;

        // Calcula a direção OPOSITA ao inimigo
        Vector2 toEnemy = mTargetEnemy->GetPosition() - GetPosition();
        toEnemy.Normalize();

        // Recua lentamente (pega impulso)
        mRigidBodyComponent->SetVelocity(toEnemy * -150.0f);

        if (mWindupTimer >= mWindupDuration) {
            mFireWispState = State::Attacking;
            // Calcula a direção do ataque final (travado no alvo)
            Vector2 attackDir = mTargetEnemy->GetPosition() - GetPosition();
            attackDir.Normalize();
            mAttackVelocity = attackDir * mAttackSpeed;
        }
    }
    else if (mFireWispState == State::Attacking) {
        mRigidBodyComponent->SetVelocity(mAttackVelocity);

        mAttackTimer += deltaTime;
        if (mAttackTimer >= mAttackDuration) {
            mFireWispState = State::Idle;
            mAttackTimer = 0.0f;
            mAttackIntervalTimer = 0.0f;
        }
    }

    if (mFlame) {
        mFlame->SetPosition(GetPosition());
    }
    if (mSpark) {
        mSpark->SetPosition(GetPosition() + Vector2(0, -10));
    }
    if (mSmoke) {
        mSmoke->SetPosition(GetPosition() + Vector2(0, -13));
    }

    AttackEnemies(deltaTime);
    UpdateLight(deltaTime);
}

void FireWisp::UpdateLight(float deltaTime) {
    mLightTimer += deltaTime;
    if (mLightTimer > Math::TwoPi * 100.0f) {
        mLightTimer -= Math::TwoPi * 100.0f;
    }

    float targetRadius = 910.0f;
    float targetIntensity = 0.8f;
    Vector3 targetColor = mColorNormal;

    float coreTargetRadius = 260.0f;
    float coreTargetIntensity = 0.95f;

    // MÁQUINA DE ESTADOS DA LUZ
    if (mExplosionTimer > 0.0f) {
        // EXPLOSÃO (Decaindo de volta pro normal)
        mExplosionTimer -= deltaTime;
        // t vai de 1.0 (explosão máxima) para 0.0 (voltou ao normal)
        float t = mExplosionTimer / mExplosionDuration;
        if (t < 0.0f) t = 0.0f;

        // O raio explode para 2500 e volta para 910 suavemente
        targetRadius = 910.0f + (1590.0f * t);
        targetIntensity = 0.8f + (2.2f * t); // Intensidade vai a 3.0 no pico

        // Interpolação da cor
        targetColor = mColorNormal * (1.0f - t) + mColorExplosion * t;

        coreTargetRadius = 260.0f + (500.0f * t);
        coreTargetIntensity = 0.95f + (1.5f * t);
    }
    else if (mFireWispState == State::Windup) {
        // RECUO (Contraindo e esquentando)
        // t vai de 0.0 (início) para 1.0 (fim do recuo)
        float t = mWindupTimer / mWindupDuration;
        if (t > 1.0f) {
            t = 1.0f;
        }

        // Raio principal encolhe de 910 para 400
        targetRadius = 910.0f - (510.0f * t);
        targetIntensity = 0.8f + (0.7f * t); // Brilho sobe para 1.5

        targetColor = mColorNormal * (1.0f - t) + mColorWindup * t;

        coreTargetRadius = 260.0f - (110.0f * t); // Core fica bem pequeno
        coreTargetIntensity = 0.95f + (0.5f * t);
    }
    else if (mFireWispState == State::Attacking) {
        // TIRO (Mantém a energia concentrada)
        targetRadius = 300.0f;
        targetIntensity = 1.5f;
        targetColor = mColorWindup;
        coreTargetRadius = 100.0f;
        coreTargetIntensity = 1.45f;
    }

    // APLICA O TREMOR ORGÂNICO SOBRE OS VALORES BASE
    if (mLight) {
        mLight->SetPosition(GetPosition());
        float pulse = sinf(mLightTimer * 3.0f) * 40.0f;
        float flicker = Random::GetFloatRange(0, 10);
        float intensityPulse = sinf(mLightTimer * 4.5f) * 0.2f;

        mLight->SetRadius(targetRadius + pulse + flicker);
        mLight->SetMaxIntensity(targetIntensity + intensityPulse);
        mLight->SetColor(targetColor);
    }

    if (mCoreLight) {
        mCoreLight->SetPosition(GetPosition());
        float pulse = sinf(mLightTimer * 4.5f) * 40.0f;
        float flicker = Random::GetFloatRange(0, 10);
        float intensityPulse = sinf(mLightTimer * 6.75f) * 0.2f;

        mCoreLight->SetRadius(coreTargetRadius + pulse + flicker);
        mCoreLight->SetMaxIntensity(coreTargetIntensity + intensityPulse);
        // O Core sempre tende ao branco puro
        mCoreLight->SetColor(targetColor * 1.2f);
    }
}

void FireWisp::AttackEnemies(float deltaTime) {
    if (!mGame->GetPlayer()->GetSkillManager()->CanFireWispAttack()) {
        return;
    }

    // Se o alvo morreu antes do hit, cancela o ataque e volta pro ombro
    if (mFireWispState != State::Idle &&
       (mTargetEnemy == nullptr || mTargetEnemy->GetState() == ActorState::Destroy))
    {
        mFireWispState = State::Idle;
        mAttackIntervalTimer = 0.0f;
        mTargetEnemy = nullptr;
        return;
    }

    mAttackIntervalTimer += deltaTime;
    if (mAttackIntervalTimer >= mAttackIntervalDuration && mFireWispState == State::Idle) {
        std::vector<Enemy*> enemies = mGame->GetEnemies();
        if (!enemies.empty()) {
            for (Enemy* e : enemies) {
                if (e->GetState() != ActorState::Destroy && (e->GetPosition() - GetPosition()).Length() <= mAttackRange) {
                    mFireWispState = State::Windup;
                    mTargetEnemy = e;
                    mWindupTimer = 0.0f;
                    break;
                }
            }
        }
    }
    if (mFireWispState == State::Attacking) {
        std::vector<Enemy*> enemies = mGame->GetEnemies();
        if (!enemies.empty()) {
            for (Enemy* e : enemies) {
                HitResult hitResult = mCombatBoxComponent->CheckAttackAgainst(e->GetComponent<CombatBoxComponent>());
                if (hitResult.isValid) {
                    Vector2 knockBackDirection = e->GetPosition() - GetPosition();
                    if (knockBackDirection.Length() > 0) {
                        knockBackDirection.Normalize();
                    }
                    e->ReceiveHit(mDamage, knockBackDirection);
                    e->Unfreeze();
                    mGame->GetCamera()->StartCameraShake(0.2f, 90.0f);

                    // DISPARA O EFEITO DE EXPLOSÃO DA LUZ
                    mExplosionTimer = mExplosionDuration;

                    mFireWispState = State::Idle;
                    mAttackTimer = 0.0f;
                    mAttackIntervalTimer = 0.0f;
                    mTargetEnemy = nullptr;
                }
            }
        }
    }
}

void FireWisp::InitParticles() {
    mFlame = new ParticleSystem(mGame, Particle::ParticleType::BlurParticle, 15.0f, 90.0f, 0.2f, mDuration);
    mFlame->SetPosition(GetPosition());
    mFlame->SetParticleColor(SDL_Color{255, 148, 10, 255});
    mFlame->SetConeSpread(30.0f);
    mFlame->SetEmitDirection(Vector2::NegUnitY);
    mFlame->SetParticleSpeedScale(0.4f);
    mFlame->SetParticleGravity(false);
    mFlame->SetGroundCollision(false);
    mFlame->SetAdditiveBlending(true);
    mFlame->SetParticleDrawOrder(5002);

    mSpark = new ParticleSystem(mGame, Particle::ParticleType::SolidParticle, 3, 30.0, 0.3, mDuration);
    mSpark->SetPosition(GetPosition());
    mSpark->SetParticleColor(SDL_Color{255, 100, 10, 255});
    mSpark->SetConeSpread(270.0f);
    mSpark->SetEmitDirection(Vector2::NegUnitY);
    mSpark->SetParticleSpeedScale(0.2f);
    mSpark->SetParticleGravity(false);
    mSpark->SetGroundCollision(false);
    mSpark->SetAdditiveBlending(true);
    mSpark->SetParticleDrawOrder(5001);

    mSmoke = new ParticleSystem(mGame, Particle::ParticleType::BlurParticle, 40.0f, 45.0f, 0.5f, mDuration);
    mSmoke->SetPosition(GetPosition());
    mSmoke->SetParticleColor(SDL_Color{40, 40, 40, 100});
    mSmoke->SetConeSpread(45.0f);
    mSmoke->SetEmitDirection(Vector2::NegUnitY);
    mSmoke->SetParticleSpeedScale(0.2f);
    mSmoke->SetParticleGravity(false);
    mSmoke->SetGroundCollision(false);
    mSmoke->SetAdditiveBlending(false);
    mSmoke->SetParticleDrawOrder(5001);
}

void FireWisp::InitLight() {
    mLight = new Light(mGame);
    mLight->SetRadius(mLightRadius);
    mLight->SetMaxIntensity(mLightIntensity);
    mLight->SetColor(Vector3(1.0f, 0.8f, 0.75f));
    mLight->Activate(0.4f);

    mCoreLight = new Light(mGame);
    mCoreLight->SetRadius(mCoreLightRadius);
    mCoreLight->SetMaxIntensity(mCoreLightIntensity);
    mCoreLight->SetColor(Vector3(1.0f, 0.45f, 0.45f));
    mCoreLight->Activate(0.4f);
}
