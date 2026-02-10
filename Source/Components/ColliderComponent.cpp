//
// Created by roger on 16/08/2025.
//

#include "ColliderComponent.h"
#include "../Game.h"
#include "../Actors/Actor.h"

ColliderComponent::ColliderComponent(class Actor *owner, ColliderType type, Vector2 offset, bool autoRegister)
    :Component(owner, 100 , autoRegister)
    ,mType(type)
    ,mIsActive(true)
    ,mOffset(offset)
    ,mDebugDraw(false)
    ,mColor(Vector3::One)
{
    mOwner->GetGame()->AddCollider(this);
}

ColliderComponent::~ColliderComponent()
{
    mOwner->GetGame()->RemoveCollider(this);
}

void ColliderComponent::Draw(class Renderer *renderer) { }
