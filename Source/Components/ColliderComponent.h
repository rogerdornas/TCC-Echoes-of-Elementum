//
// Created by roger on 16/08/2025.
//

#pragma once

#include "Component.h"
#include "../Math.h"
#include <array>
#include <vector>

class ColliderComponent : public Component
{
public:
    enum class ColliderType { AABB, OBB };

    ColliderComponent(class Actor* owner, ColliderType type, Vector2 offset = Vector2::Zero, bool autoRegister = true);
    ~ColliderComponent();

    ColliderType GetType() const { return mType; }

    void SetActive(bool active) { mIsActive = active; }
    bool IsActive() const { return mIsActive; }

    void SetOffset(Vector2 offset) { mOffset = offset; }
    Vector2 GetOffset() { return mOffset; }

    void SetColor(const Vector3& color) { mColor = color; }

    void SetDebugDraw(bool enabled) { mDebugDraw = enabled; }

    virtual void Draw(class Renderer* renderer);

    virtual bool Intersect(ColliderComponent& other) = 0;
    virtual Vector2 ResolveCollision(ColliderComponent& other) = 0;
    virtual Vector2 CollisionSide(ColliderComponent& other) = 0;

protected:
    ColliderType mType;
    bool mIsActive;
    Vector2 mOffset;
    bool mDebugDraw;
    Vector3 mColor;
};

