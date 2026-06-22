//
// Created by roger on 02/05/2025.
//

#include "Enemy.h"
#include "../Effect.h"
#include "../Light.h"
#include "../LightningEffect.h"
#include "../Money.h"
#include "../ParticleSystem.h"
#include "../../Game.h"
#include "../../Random.h"
#include "../../Components/RigidBodyComponent.h"
#include "../../Components/AABBComponent.h"
#include "../../Components/Drawing/AnimatorComponent.h"
#include "../../Components/Drawing/RectComponent.h"
#include "../../Components/CombatBoxComponent.h"

Enemy::Enemy(Game* game)
    :Actor(game)
    ,mIsOnGround(true)
    ,mMoneyDrop(0)
    ,mKnockBackXActive(true)
    ,mKnockBackYActive(true)
    ,mKnockBackSpeed(0.0f)
    ,mKnockBackTimer(0.0f)
    ,mKnockBackDuration(0.0f)
    ,mCameraShakeStrength(60.0f)
    ,mIsFlashing(false)
    ,mFlashDuration(0.07f)
    ,mFlashTimer(mFlashDuration)
    ,mBloodColor({226, 90, 70, 255})
    ,mPlayerSpotted(false)
    ,mOffscreenLimit(0.2f)
    ,mEnemyCollision(true)
    ,mIsStunned(false)
    ,mStunDuration(0.3f)
    ,mStunTimer(0.0f)
    ,mStunEffect(nullptr)
    ,mIsFrozen(false)
    ,mFreezeMax(50.0f)
    ,mFreezeCount(0)
    ,mFrozenDecayRate(mFreezeMax / 3.0f)
    ,mFreezeDecayDuration(1.5f)
    ,mFreezeDecayTimer(0.0f)
    ,mFreezeDecayRate(20.0f)
    ,mFreezeEffect(nullptr)
    ,mRectComponent(nullptr)
    ,mDrawComponent(nullptr)
    ,mCombatBoxComponent(nullptr)
{
    game->AddEnemy(this);
}

Enemy::~Enemy() {
    mGame->RemoveEnemy(this);
}

void Enemy::SetSize(float width, float height, bool setCombatBox) {
    Vector2 v1(-width / 2, -height / 2);
    Vector2 v2(width / 2, -height / 2);
    Vector2 v3(width / 2, height / 2);
    Vector2 v4(-width / 2, height / 2);

    std::vector<Vector2> vertices;
    vertices.emplace_back(v1);
    vertices.emplace_back(v2);
    vertices.emplace_back(v3);
    vertices.emplace_back(v4);

    // mRectComponent = new RectComponent(this, mWidth, mHeight, RendererMode::LINES);
    // mRectComponent->SetColor(Vector3(245, 0, 0));

    mRigidBodyComponent = new RigidBodyComponent(this, 1, 40000, 40000);
    mColliderComponent = new AABBComponent(this, v1, v3);

    if (setCombatBox) {
        mCombatBoxComponent = new CombatBoxComponent(this);
        mCombatBoxComponent->AddAABBBox("hitbox", true, v1, v3);
        mCombatBoxComponent->AddAABBBox("hurtbox", false, v1, v3);
        // mCombatBoxComponent->SetDebugDraw(true);
    }
}

void Enemy::ReceiveHit(float damage, Vector2 knockBackDirection, bool knockBack) {
    mHealthPoints -= damage;
    if (knockBack) {
        Vector2 knockBackSpeed(knockBackDirection * mKnockBackSpeed);
        if (mKnockBackXActive) {
            mRigidBodyComponent->SetVelocity(mRigidBodyComponent->GetVelocity() + Vector2(knockBackSpeed.x, 0));
        }
        if (mKnockBackYActive) {
            mRigidBodyComponent->SetVelocity(mRigidBodyComponent->GetVelocity() + Vector2(0, knockBackSpeed.y));
        }
        mKnockBackTimer = 0;
    }
    mIsFlashing = true;
    mFlashTimer = 0;
    mPlayerSpotted = true;

    auto* blood = new ParticleSystem(mGame, Particle::ParticleType::SolidParticle, 10.0f * 1.2f, 10.0f, 170.0, 3.0, 0.07f);
    blood->SetPosition(GetPosition());
    blood->SetEmitDirection(knockBackDirection);
    blood->SetParticleSpeedScale(1);
    blood->SetParticleColor(mBloodColor);
    blood->SetParticleGravity(true);
    blood->SetConeSpread(65.0f);

    auto* circleBlur = new Effect(mGame);
    circleBlur->SetDuration(0.3);
    circleBlur->SetSize((GetWidth() + GetHeight()) / 2 * 3.5f);
    circleBlur->SetActor(*this);
    circleBlur->SetColor(SDL_Color{226, 90, 70, 150});
    circleBlur->SetEffect(TargetEffect::Circle);

    if (IsOnScreen()) {
        mGame->GetAudio()->PlayVariantSound("HitEnemy/HitEnemy.wav", 4);
    }
}

void Enemy::Stun(float duration) {
    if (mIsStunned) {
        return;
    }

    mRigidBodyComponent->SetVelocity(Vector2::Zero);
    mStunDuration = duration;
    mStunTimer = 0;
    mIsStunned = true;

    Vector2 startPosition(GetPosition() - Vector2(mWidth * 0.5f, 0.0f));
    Vector2 endPosition(GetPosition() + Vector2(mWidth * 0.5f, 0.0f));
    mStunEffect = new LightningEffect(mGame, this, mStunDuration);
    mStunEffect->StartEffect(startPosition, endPosition);
    mStunEffect->SetLightningGenerationIntervalDuration(0.05f);
    mStunEffect->SetNumBolts(6);
    mStunEffect->SetSpeadRadius(5.0f);
    mStunEffect->SetGenerations(4);
    mStunEffect->SetMaxOffset(mHeight * 0.8f);
    mStunEffect->SetGlowThickness(5.0f);
    mStunEffect->SetCoreThickness(1.0f);
}

void Enemy::ManageStun(float deltaTime) {
    if (mIsStunned) {
        mStunTimer += deltaTime;
        mRigidBodyComponent->SetVelocity(Vector2::Zero);
        if (mDrawComponent) {
            mDrawComponent->SetAnimFPS(0);
        }
        if (mStunEffect) {
            Vector2 startPosition(GetPosition() - Vector2(mWidth * 0.5f, 0.0f));
            Vector2 endPosition(GetPosition() + Vector2(mWidth * 0.5f, 0.0f));
            mStunEffect->SetStartPosition(startPosition);
            mStunEffect->SetEndPosition(endPosition);
        }
        if (mStunTimer >= mStunDuration) {
            mIsStunned = false;
        }
    }
}

void Enemy::ReceiveFreeze(float freezeDamage, float freezeIntensity) {
    mHealthPoints -= freezeDamage;
    mFreezeCount += freezeIntensity;
    mFreezeCount = std::min(mFreezeCount, mFreezeMax);

    if (mIsFrozen) {
        return;
    }
    mFreezeDecayTimer = 0;
    mPlayerSpotted = true;

    if (mFreezeCount >= mFreezeMax) {
        // congela
        mRigidBodyComponent->SetVelocity(Vector2::Zero);
        mIsFrozen = true;
        // mFreezeCount = 0;
        //nuvem de gelo
        float particleSize = 0.75f * (GetWidth() + GetHeight() / 2);
        mFreezeEffect = new ParticleSystem(mGame, Particle::ParticleType::BlurParticle, particleSize * 1.2f, particleSize, 80.0f, 0.35f, 1000.0f);
        mFreezeEffect->SetParticleColor(SDL_Color{100, 200, 255, 15});
        mFreezeEffect->SetConeSpread(360.0f);
        mFreezeEffect->SetParticleSpeedScale(0.2f);
        mFreezeEffect->SetParticleGravity(false);
        mFreezeEffect->SetEmitDirection(GetForward());
        mFreezeEffect->SetGroundCollision(false);
        mFreezeEffect->SetPosition(GetPosition());
    }
}

void Enemy::ManageFreezing(float deltaTime) {
    if (mIsFrozen) {
        mRigidBodyComponent->SetVelocity(Vector2::Zero);
        if (mDrawComponent) {
            mDrawComponent->SetAnimFPS(0);
        }

        if (mFreezeEffect) {
            mFreezeEffect->SetPosition(GetPosition());
        }

        if (mHealthPoints <= 0) {
            mFreezeEffect->EndParticleSystem();
        }

        float freezeMult = mGame->GetPlayer()->GetSkillManager()->GetFreezeDurationMultiplier();

        mFreezeCount -= (mFrozenDecayRate / freezeMult) * deltaTime;

        if (mFreezeCount <= 0) {
            Unfreeze();
            mIsFrozen = false;
            mFreezeCount = 0;
        }
    }
    else if (mFreezeCount > 0) {
        mFreezeDecayTimer += deltaTime;
        if (mFreezeDecayTimer >= mFreezeDecayDuration) {
            mFreezeCount -= mFreezeDecayRate * deltaTime;
            if (mFreezeCount < 0) {
                mFreezeCount = 0;
            }
        }
    }

    // --- ENVIANDO PARA O SHADER ---
    // Calcula a porcentagem (0.0 a 1.0)
    float freezeLevel = mFreezeCount / mFreezeMax;

    // Garante que se estiver no estado Frozen, o visual é 100%
    if (mIsFrozen) {
        freezeLevel = 1.0f;
    }

    // Passa para o componente de desenho
    if (mDrawComponent) {
        mDrawComponent->SetFreezeLevel(freezeLevel);
    }
}
void Enemy::Unfreeze() {
    mIsFrozen = false;
    mFreezeCount = 0;
    if (mFreezeEffect) {
        mFreezeEffect->EndParticleSystem();
    }
}

bool Enemy::Died() {
    if (mHealthPoints > 0) {
        return false;
    }
    if (mHealthPoints <= 0) {
        SetState(ActorState::Destroy);
        if (mStunEffect) {
            mStunEffect->StopEffect();
        }

        if (IsOnScreen()) {
            mGame->GetAudio()->PlaySound("KillEnemy/KillEnemy1.wav");

            mGame->GetCamera()->StartCameraShake(0.3, mCameraShakeStrength);

            auto* blood = new ParticleSystem(mGame, Particle::ParticleType::SolidParticle, 15.0f * 1.2f, 15.0f, 300.0, 3.0, 0.07f);
            blood->SetPosition(GetPosition());
            blood->SetEmitDirection(Vector2::UnitY);
            blood->SetParticleSpeedScale(0.8);
            blood->SetParticleColor(mBloodColor);
            blood->SetParticleGravity(true);
            blood->SetConeSpread(360.0f);

            auto* circleBlur = new Effect(mGame);
            circleBlur->SetDuration(1.0);
            circleBlur->SetSize((GetWidth() + GetHeight()) / 2 * 5.5f);
            circleBlur->SetActor(*this);
            circleBlur->SetColor(SDL_Color{226, 90, 70, 150});
            circleBlur->SetEffect(TargetEffect::Circle);
            circleBlur->ActorDestroyed();

            std::vector<Money*> moneys = mGame->GetMoneys();

            // Primeiro dropa 1 dinheiro de cada
            if (mMoneyDrop >= 1) {
                for (Money* m: moneys) {
                    if (m->GetState() == ActorState::Paused && m->GetMoneyType() == Money::MoneyType::Small) {
                        m->SetState(ActorState::Active);
                        m->SetPosition(GetPosition());
                        mMoneyDrop -= 1;
                        break;
                    }
                }
            }
            if (mMoneyDrop >= 5) {
                for (Money* m: moneys) {
                    if (m->GetState() == ActorState::Paused && m->GetMoneyType() == Money::MoneyType::Medium) {
                        m->SetState(ActorState::Active);
                        m->SetPosition(GetPosition());
                        mMoneyDrop -= 5;
                        break;
                    }
                }
            }
            if (mMoneyDrop >= 10) {
                for (Money* m: moneys) {
                    if (m->GetState() == ActorState::Paused && m->GetMoneyType() == Money::MoneyType::Large) {
                        m->SetState(ActorState::Active);
                        m->SetPosition(GetPosition());
                        mMoneyDrop -= 10;
                        break;
                    }
                }
            }

            // Cria grandes
            while (mMoneyDrop >= 10) {
                for (Money* m: moneys) {
                    if (m->GetState() == ActorState::Paused && m->GetMoneyType() == Money::MoneyType::Large) {
                        m->SetState(ActorState::Active);
                        m->SetPosition(GetPosition());
                        mMoneyDrop -= 10;
                        break;
                    }
                }
            }

            // Cria médios
            while (mMoneyDrop >= 5) {
                for (Money* m: moneys) {
                    if (m->GetState() == ActorState::Paused && m->GetMoneyType() == Money::MoneyType::Medium) {
                        m->SetState(ActorState::Active);
                        m->SetPosition(GetPosition());
                        mMoneyDrop -= 5;
                        break;
                    }
                }
            }

            // Cria pequenos
            while (mMoneyDrop >= 1) {
                for (Money* m: moneys) {
                    if (m->GetState() == ActorState::Paused && m->GetMoneyType() == Money::MoneyType::Small) {
                        m->SetState(ActorState::Active);
                        m->SetPosition(GetPosition());
                        mMoneyDrop -= 1;
                        break;
                    }
                }
            }
            return true;
        }
    }
    return false;
}

void Enemy::ResolveEnemyCollision() {
    if (!mEnemyCollision || mIsFrozen) {
        return;
    }

    std::vector<Enemy* > enemies = mGame->GetEnemies();
    if (!enemies.empty()) {
        for (Enemy* e: enemies) {
            if (e != this) {
                if (e->GetEnemyCollision() && !e->IsFrozen()) {
                    if (mColliderComponent->Intersect(*e->GetComponent<ColliderComponent>())) {
                        mColliderComponent->ResolveCollision(*e->GetComponent<ColliderComponent>());
                    }
                }
            }
        }
    }
}

void Enemy::ResolveGroundCollision() {
    mIsOnGround = false;
    Vector2 collisionNormal(Vector2::Zero);
    std::vector<Ground*> grounds = GetGame()->GetGrounds();
    if (!grounds.empty()) {
        for (Ground* g : grounds) {
            if (!g->GetIsSpike()) { // Colisão com ground
                if (mColliderComponent->Intersect(*g->GetComponent<ColliderComponent>())) {
                    collisionNormal = mColliderComponent->ResolveCollision(*g->GetComponent<ColliderComponent>());
                }
                else {
                    collisionNormal = Vector2::Zero;
                }

                // colidiu top
                if (collisionNormal == Vector2::NegUnitY) {
                    mIsOnGround = true;
                }
            }
            else if (g->GetIsSpike()) { // Colisão com spikes
                if (mColliderComponent->Intersect(*g->GetComponent<ColliderComponent>())) {
                    collisionNormal = mColliderComponent->ResolveCollision(*g->GetComponent<ColliderComponent>());
                    // Colidiu top
                    if (collisionNormal == Vector2::NegUnitY) {
                        ReceiveHit(10, Vector2::NegUnitY);
                    }
                    // Colidiu bot
                    if (collisionNormal == Vector2::UnitY) {
                        ReceiveHit(10, Vector2::UnitY);
                    }
                    //Colidiu left
                    if (collisionNormal == Vector2::NegUnitX) {
                        ReceiveHit(10, Vector2::NegUnitX);
                    }
                    //Colidiu right
                    if (collisionNormal == Vector2::UnitX) {
                        ReceiveHit(10, Vector2::UnitX);
                    }
                    mKnockBackTimer = 0;
                }
            }
        }
    }
}

bool Enemy::IsOnScreen() {
    return (GetPosition().x < mGame->GetCamera()->GetPosCamera().x + mGame->GetRenderer()->GetZoomedWidth()  + mGame->GetRenderer()->GetZoomedWidth() * mOffscreenLimit &&
            GetPosition().x > mGame->GetCamera()->GetPosCamera().x  - mGame->GetRenderer()->GetZoomedWidth() * mOffscreenLimit &&
            GetPosition().y > mGame->GetCamera()->GetPosCamera().y - mGame->GetRenderer()->GetZoomedHeight() * mOffscreenLimit &&
            GetPosition().y < mGame->GetCamera()->GetPosCamera().y + mGame->GetRenderer()->GetZoomedHeight() + mGame->GetRenderer()->GetZoomedHeight() * mOffscreenLimit);
}