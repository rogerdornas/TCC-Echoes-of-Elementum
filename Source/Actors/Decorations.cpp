//
// Created by roger on 03/10/2025.
//

#include "Decorations.h"

#include "Effect.h"
#include "../Game.h"
#include "../Components/Drawing/AnimatorComponent.h"
#include "../Components/AABBComponent.h"
#include "ParticleSystem.h"
#include "../Json.h"
#include "../Components/RigidBodyComponent.h"

Decorations::Decorations(Game *game, float width, float height, std::string imagePath, std::string decorationName,
                         float fps, int numFrames, bool animated, int gid, float rotation,
                         int drawOrder, Vector2 parallaxFactor,
                         Vector3 textureColor, float textureFactor, bool isDestructible)
    :Actor(game)
    ,mWidth(width)
    ,mHeight(height)
    ,mFPS(fps)
    ,mNumFrames(numFrames)
    ,mAnimated(animated)
    ,mBaseRotation(rotation)
    ,mAngularVelocity(0.0f)
    ,mCooldownDuration(0.5f)
    ,mCooldownTimer(0.0f)
    ,mHasWindBalance(false)
    ,mTimeActive(0.05f)
    ,mWindAmplitude(Math::ToRadians(5.0f))
    ,mWindSpeed(3.0f)
    ,mPositionInitialized(false)
    ,mIsMoving(false)
    ,mMovingDuration(0.0f)
    ,mMovingTimer(0.0f)
    ,mMoveVelocity(0, 0)
    ,mDrawComponent(nullptr)
    ,mColliderComponent(nullptr)
{
    int FLIPPED_HORIZONTALLY_FLAG = 0x80000000;
    int FLIPPED_VERTICALLY_FLAG   = 0x40000000;

    bool flippedHorizontally = (gid & FLIPPED_HORIZONTALLY_FLAG) != 0;
    bool flippedVertically   = (gid & FLIPPED_VERTICALLY_FLAG) != 0;

    if (flippedHorizontally) {
        SetScale(Vector2(-1, GetScale().y));
    }
    if (flippedVertically) {
        SetScale(Vector2(GetScale().x, -1));
    }

    SetRotation(rotation);
    SetTransformRotation(rotation);

    mImagePath = "../Assets/" + imagePath;

    Vector2 v1(-mWidth / 2, -mHeight / 2);
    Vector2 v2(mWidth / 2, -mHeight / 2);
    Vector2 v3(mWidth / 2, mHeight / 2);
    Vector2 v4(-mWidth / 2, mHeight / 2);

    std::vector<Vector2> vertices;
    vertices.emplace_back(v1);
    vertices.emplace_back(v2);
    vertices.emplace_back(v3);
    vertices.emplace_back(v4);

    // mDrawPolygonComponent = new DrawPolygonComponent(this, vertices, {0, 255, 0, 255});

    if (mAnimated) {
        mDrawComponent = new AnimatorComponent(this,
                                            mImagePath + ".png",
                                            mImagePath + ".json",
                                            mWidth, mHeight, drawOrder,
                                            false, false);

        std::vector<int> idle(mNumFrames);
        std::iota(idle.begin(), idle.end(), 0);
        mDrawComponent->AddAnimation("idle", idle);

        mDrawComponent->SetAnimation("idle");
        mDrawComponent->SetAnimFPS(mFPS);
    }
    else {
        mDrawComponent = new AnimatorComponent(this,
                                    mImagePath + ".png",
                                    mImagePath + ".json",
                                    mWidth, mHeight, drawOrder,
                                    true, false);

        auto decorationsName = mGame->GetDecorationsName();

        // Encontra o índice apenas da decoração atual
        auto it = std::find(decorationsName.begin(), decorationsName.end(), decorationName);
        if (it != decorationsName.end()) {
            int index = std::distance(decorationsName.begin(), it);

            mDrawComponent->AddAnimation(decorationName, {index});
            mDrawComponent->SetAnimation(decorationName);
        } else {
            SDL_Log("Aviso: Decoração %s não encontrada", decorationName.c_str());
        }

        mDrawComponent->SetAnimation(decorationName);
        mDrawComponent->SetAnimFPS(mFPS);
    }

    mDrawComponent->SetColor(textureColor);
    mDrawComponent->SetTextureFactor(textureFactor);
    mDrawComponent->SetParallaxFactor(parallaxFactor);

    if (isDestructible) {
        mColliderComponent = new AABBComponent(this, v1, v3);
        // mColliderComponent->SetDebugDraw(true);
    }
}

void Decorations::OnUpdate(float deltaTime) {
    if (!mPositionInitialized) {
        mBasePosition = GetPosition();
        mPositionInitialized = true;
    }

    // LÓGICA DE MOVIMENTO
    if (mIsMoving && mMovingDuration > 0.0f) {
        mMovingTimer += deltaTime;

        // Inverte a direção ao completar a duração
        if (mMovingTimer > mMovingDuration) {
            mMoveVelocity = mMoveVelocity * -1.0f;
            mMovingTimer -= mMovingDuration;
        }

        // Curva de suavização
        float progress = mMovingTimer / mMovingDuration;
        float smoothMultiplier = Math::PiOver2 * std::sin(Math::Pi * progress);

        mBasePosition.x += mMoveVelocity.x * smoothMultiplier * deltaTime;
        mBasePosition.y += mMoveVelocity.y * smoothMultiplier * deltaTime;
    }

    // CÁLCULOS DE PROPORÇÃO DE TAMANHO
    float currentArea = mWidth * mHeight;
    float baseArea = 6000.0f; // Área de referência

    float agilityFactor = std::clamp(baseArea / currentArea, 0.2f, 3.0f);

    float massFactor = std::clamp(currentArea / baseArea, 0.5f, 8.0f);

    if (mCooldownTimer < mCooldownDuration) {
        mCooldownTimer += deltaTime;
    }

    // Calcula qual é o "centro" atual (com ou sem vento)
    float currentTargetRotation = mBaseRotation;

    if (mHasWindBalance) {
        mTimeActive += deltaTime;

        float phaseOffset = (mBasePosition.x * 0.01f) + (mBasePosition.y * 0.01f);
        float localWindSpeed = mWindSpeed + std::fmod(std::abs(mBasePosition.x * 0.05f), 1.5f);

        // APLICA O FATOR DE AGILIDADE NA AMPLITUDE DO VENTO
        float windOffset = std::sin((mTimeActive * localWindSpeed) + phaseOffset) * (mWindAmplitude * agilityFactor);
        currentTargetRotation += windOffset;
    }

    if (mColliderComponent) {
        auto* player = mGame->GetPlayer();

        // Descobre se a planta está de cabeça para baixo para inverter a reação
        float orientationSign = std::cos(mBaseRotation) < 0.0f ? -1.0f : 1.0f;

        // Interação com o Player
        if (mColliderComponent->Intersect(*player->GetComponent<ColliderComponent>())) {
            float playerVelX = player->GetComponent<RigidBodyComponent>()->GetVelocity().x;

            if (mCooldownTimer >= mCooldownDuration && std::abs(playerVelX) > 0.1f) {

                // Aplica um impulso inicial e inverte se estiver no teto
                float hitImpulse = 8.0f * agilityFactor;
                mAngularVelocity = ((playerVelX > 0) ? hitImpulse : -hitImpulse) * orientationSign;

                // Reinicia o cooldown
                mCooldownTimer = 0;
            }
        }

        // Interação com bola de fogo
        std::vector<FireBall* > fireBalls = mGame->GetFireBalls();
        for (FireBall* f: fireBalls) {
            if (mColliderComponent->Intersect(*f->GetComponent<ColliderComponent>())) {
                float fireBallVelX = f->GetComponent<RigidBodyComponent>()->GetVelocity().x;

                if (mCooldownTimer >= mCooldownDuration && std::abs(fireBallVelX) > 0.1f) {

                    // Aplica o impulso invertendo também pelo teto
                    float hitImpulse = 8.0f * agilityFactor;
                    mAngularVelocity = ((fireBallVelX > 0) ? hitImpulse : -hitImpulse) * orientationSign;

                    // Reinicia o cooldown
                    mCooldownTimer = 0;
                }
            }
        }

        // Interação da Espada
        auto* playerSword = player->GetSword();
        if (mColliderComponent->Intersect(*playerSword->GetComponent<ColliderComponent>())) {
            SetState(ActorState::Destroy);

            auto* sparkEffect = new Effect(mGame);
            sparkEffect->SetDuration(0.15f);
            float positionY = Math::Clamp(playerSword->GetPosition().y, GetPosition().y - mHeight * 0.4f, GetPosition().y + mHeight * 0.4f);
            sparkEffect->SetPosition(Vector2(GetPosition().x, positionY));
            sparkEffect->SetEffect(TargetEffect::SwordHit);

            // APLICA O FATOR DE MASSA NA QUANTIDADE DE PARTÍCULAS
            float particleCount = 150.0f * massFactor;

            auto* grass = new ParticleSystem(mGame, Particle::ParticleType::Grass,
                                      12.17f, 5.5f, particleCount, 1.1f, 0.1f);

            grass->SetEmitArea(Vector2(mWidth * 0.8f, mHeight * 0.8f));
            grass->SetParticleGravity(true);
            grass->SetParticleGravityForce(450.0f);
            grass->SetEmitDirection(Vector2::NegUnitY);
            grass->SetGroundCollision(false);
            grass->SetConeSpread(20.0f);
            grass->SetParticleSpeedScale(0.3f);
            grass->SetParticleColor(SDL_Color{static_cast<Uint8>(mDrawComponent->GetColor().x), static_cast<Uint8>(mDrawComponent->GetColor().y), static_cast<Uint8>(mDrawComponent->GetColor().z), 255});
            grass->SetParticleTextureFactor(mDrawComponent->GetTextureFactor());
            grass->SetParticleAutoRotate(true);
            grass->SetParticleRotationSpeed(1.0f);
            grass->SetParticleDrawOrder(mDrawComponent->GetDrawOrder());
            grass->SetPosition(GetPosition());
        }
    }

    // Física da Mola para o "Vento"
    float stiffness = 120.0f;
    float damping = 10.0f;

    float currentRotation = GetRotation();
    float displacement = currentRotation - currentTargetRotation;

    float springForce = -stiffness * displacement;
    float dampingForce = -damping * mAngularVelocity;
    float angularAcceleration = springForce + dampingForce;

    mAngularVelocity += angularAcceleration * deltaTime;

    // Calcula a nova rotação
    float newRotation = currentRotation + (mAngularVelocity * deltaTime);
    SetRotation(newRotation);
    SetTransformRotation(newRotation);

    // Simulação do Pivô Matemático
    // Define onde fica a raiz no espaço local da imagem (metade da altura para baixo)
    Vector2 localPivot(0.0f, mHeight * 0.5f);

    // Calcula a "Âncora" no mundo: onde a raiz DEVE ficar presa (usando a rotação e posição originais da fase)
    float cosBase = std::cos(mBaseRotation);
    float sinBase = std::sin(mBaseRotation);
    Vector2 baseRotatedPivot(
        localPivot.x * cosBase - localPivot.y * sinBase,
        localPivot.x * sinBase + localPivot.y * cosBase
    );
    Vector2 rootWorldAnchor = mBasePosition + baseRotatedPivot;

    // Calcula onde o pivô parou agora com a nova rotação do vento
    float cosCurrent = std::cos(newRotation);
    float sinCurrent = std::sin(newRotation);
    Vector2 currentRotatedPivot(
        localPivot.x * cosCurrent - localPivot.y * sinCurrent,
        localPivot.x * sinCurrent + localPivot.y * cosCurrent
    );

    // Move o centro do Actor para compensar a diferença
    SetPosition(Vector2(rootWorldAnchor.x - currentRotatedPivot.x, rootWorldAnchor.y - currentRotatedPivot.y));
}

void Decorations::SetMovement(bool isMoving, Vector2 velocity, float duration) {
    mIsMoving = isMoving;
    mMoveVelocity = velocity;
    mMovingDuration = duration;
    mMovingTimer = 0.0f;
}