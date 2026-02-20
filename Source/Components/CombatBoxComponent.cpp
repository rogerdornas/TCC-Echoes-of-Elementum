//
// Created by roger on 20/01/2026.
//

#include "CombatBoxComponent.h"
#include "AABBComponent.h"
#include "OBBComponent.h"
#include "../Game.h"
#include "../Actors/Actor.h"

CombatBoxComponent::CombatBoxComponent(class Actor *owner)
    :Component(owner)
    ,mDebugDraw(false)
{
}

CombatBoxComponent::~CombatBoxComponent() {
    for (auto& box : mBoxes) {
        delete box.collider;
    }
}

void CombatBoxComponent::Update(float deltaTime) {
    for (auto& box : mBoxes) {
        if (box.active) {
            if (auto* obb = dynamic_cast<OBBComponent*>(box.collider)) {
                obb->Update(deltaTime);
            }
        }
    }
}

CombatBox CombatBoxComponent::GetBox(const std::string &tag) {
    CombatBox result;
    for (auto& box : mBoxes) {
        if (box.tag == tag) {
            return box;
        }
    }
    return result;
}

void CombatBoxComponent::SetDebugDraw(bool enabled) {
    mDebugDraw = enabled;
    for (auto& box : mBoxes) {
        box.collider->SetDebugDraw(mDebugDraw);
    }
}

void CombatBoxComponent::AddAABBBox(const std::string &tag, bool isHitbox, Vector2 min, Vector2 max, Vector2 offset, int damage) {
    CombatBox box;
    box.tag = tag;
    box.isHitbox = isHitbox;
    box.damage = damage;
    box.active = true;

    box.collider = new AABBComponent(mOwner, min, max, offset, false);
    if (isHitbox) {
        box.collider->SetColor(Vector3(1, 0, 0));
    }
    else {
        box.collider->SetColor(Vector3(0, 0, 1));
    }
    box.collider->SetDebugDraw(mDebugDraw);

    mBoxes.push_back(std::move(box));
}

void CombatBoxComponent::AddOBBBox(const std::string& tag, bool isHitbox, Vector2 halfSize, Vector2 offset, int damage) {
    CombatBox box;
    box.tag = tag;
    box.isHitbox = isHitbox;
    box.damage = damage;
    box.active = true;

    box.collider = new OBBComponent(mOwner, halfSize, offset, false);
    if (isHitbox) {
        box.collider->SetColor(Vector3(1, 0, 0));
    }
    else {
        box.collider->SetColor(Vector3(0, 0, 1));
    }
    box.collider->SetDebugDraw(mDebugDraw);

    mBoxes.push_back(std::move(box));
}

void CombatBoxComponent::SetBoxActive(const std::string& tag, bool active) {
    for (auto& box : mBoxes) {
        if (box.tag == tag) {
            box.active = active;
            box.collider->SetActive(active);
            return;
        }
    }
}

void CombatBoxComponent::SetAllBoxesActive(bool active) {
    for (auto& box : mBoxes) {
        box.active = active;
        box.collider->SetActive(active);
    }
}

void CombatBoxComponent::SetBoxOffset(const std::string& tag, Vector2 offset) {
    for (auto& box : mBoxes) {
        if (box.tag == tag) {
            box.collider->SetOffset(offset);
            return;
        }
    }
}

void CombatBoxComponent::SetBoxHalfSize(const std::string& tag, Vector2 halfSize) {
    for (auto& box : mBoxes) {
        if (box.tag == tag) {
            if (auto aabb = dynamic_cast<AABBComponent*>(box.collider)) {
                aabb->SetMin(halfSize * -1);
                aabb->SetMax(halfSize);
            }
            if (auto obb = dynamic_cast<OBBComponent*>(box.collider)) {
                obb->SetHalfSize(halfSize);
            }
            return;
        }
    }
}

HitResult CombatBoxComponent::CheckAttackAgainst(CombatBoxComponent* target) {
    HitResult result;
    result.isValid = false;

    if (!target || !target->GetOwner()) return result;

    // 1. BROAD PHASE (Otimização)
    // Verifica se os atores estão minimamente perto.
    // Se a distância entre eles for maior que 1000px, nem roda o loop complexo.
    Vector2 myPos = mOwner->GetPosition();
    Vector2 targetPos = target->GetOwner()->GetPosition();
    if (Vector2::Distance(myPos, targetPos) > 1000) {
        return result;
    }

    // 2. NARROW PHASE (Loop N x M)
    // Minhas Hitboxes vs Hurtboxes Dele
    for (const auto& myBox : mBoxes) {
        if (!myBox.isHitbox || !myBox.active) continue;

        for (const auto& theirBox : target->GetBoxes()) {
            if (theirBox.isHitbox || !theirBox.active) continue;

            if (myBox.collider->Intersect(*theirBox.collider)) {
                result.isValid = true;
                result.damage = myBox.damage;
                result.hitTag = myBox.tag;
                result.hurtTag = theirBox.tag;
                result.victim = target->GetOwner();

                return result;
            }
        }
    }
    return result;
}

HitResult CombatBoxComponent::CheckReceiveAttack(CombatBoxComponent* attacker) {
    HitResult result;
    result.isValid = false;

    if (!attacker || !attacker->GetOwner()) return result;

    // 1. BROAD PHASE (Otimização)
    // Verifica se os atores estão minimamente perto.
    // Se a distância entre eles for maior que 1000px, nem roda o loop complexo.
    Vector2 myPos = mOwner->GetPosition();
    Vector2 targetPos = attacker->GetOwner()->GetPosition();
    if (Vector2::Distance(myPos, targetPos) > 1000) {
        return result;
    }

    // 2. NARROW PHASE (Loop N x M)
    // Minhas Hurtboxes vs Hitboxes Dele
    for (const auto& myBox : mBoxes) {
        if (myBox.isHitbox || !myBox.active) continue;

        for (const auto& theirBox : attacker->GetBoxes()) {
            if (!theirBox.isHitbox || !theirBox.active) continue;

            if (myBox.collider->Intersect(*theirBox.collider)) {
                result.isValid = true;
                result.damage = myBox.damage;
                result.hitTag = theirBox.tag;
                result.hurtTag = myBox.tag;
                result.victim = attacker->GetOwner();

                return result;
            }
        }
    }
    return result;
}