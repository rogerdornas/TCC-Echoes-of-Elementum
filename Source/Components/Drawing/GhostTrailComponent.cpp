//
// Created by roger on 15/03/2026.
//

#include "GhostTrailComponent.h"
#include "../../Actors/Actor.h"
#include "../../Game.h"
#include "AnimatorComponent.h"

GhostTrailComponent::GhostTrailComponent(Actor* owner, AnimatorComponent* animator, int drawOrder)
    :DrawComponent(owner, drawOrder)
    ,mTargetAnimator(animator)
    ,mIsEmitting(false)
    ,mSpawnTimer(0.0f)
    ,mSpawnInterval(0.04f)
    ,mGhostDuration(0.2f)
    ,mGhostColor(Vector3(0.0f, 0.8f, 1.0f))
    ,mAdditiveBlending(true)
{
}

void GhostTrailComponent::Update(float deltaTime) {
    // Envelhece os fantasmas existentes e remove os mortos
    for (auto it = mSnapshots.begin(); it != mSnapshots.end(); ) {
        it->lifeTimer += deltaTime;
        if (it->lifeTimer >= it->maxLifetime) {
            it = mSnapshots.erase(it);
        } else {
            ++it;
        }
    }

    // Gera novos fantasmas se estiver emitindo
    if (mIsEmitting) {
        mSpawnTimer += deltaTime;
        if (mSpawnTimer >= mSpawnInterval) {
            mSpawnTimer = 0.0f;

            // Cria o Snapshot pegando os dados atuais do Actor e do Animator
            GhostSnapshot newGhost;
            newGhost.position = mOwner->GetPosition();
            newGhost.scale = mOwner->GetScale();
            newGhost.rotation = mOwner->GetTransformRotation();
            newGhost.texRect = mTargetAnimator->GetCurrentTexRect();
            newGhost.lifeTimer = 0.0f;
            newGhost.maxLifetime = mGhostDuration;

            mSnapshots.push_back(newGhost);
        }
    }
}

void GhostTrailComponent::Draw(Renderer* renderer) {
    if (mSnapshots.empty() || !mIsVisible) {
        return;
    }

    Texture* tex = mTargetAnimator->GetTexture();
    Vector2 size(mTargetAnimator->GetWidth(), mTargetAnimator->GetHeight());
    Vector2 cameraPos = GetGame()->GetCamera()->GetPosCamera();

    for (const auto& ghost : mSnapshots) {
        // Calcula a opacidade (Alpha): Começa em 0.6f e vai caindo até 0.0f
        float lifePercent = ghost.lifeTimer / ghost.maxLifetime;
        float currentAlpha = 0.6f * (1.0f - lifePercent);

        renderer->DrawTexture(
            ghost.position, size, ghost.rotation,
            mGhostColor, tex, ghost.texRect,
            cameraPos, ghost.scale,
            0.9f, currentAlpha, 0.0f, mAdditiveBlending
        );
    }
}