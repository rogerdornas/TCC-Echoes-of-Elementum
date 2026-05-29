//
// Created by Lucas N. Ferreira on 22/05/25.
//

#pragma once

#include "../Math.h"
#include <SDL.h>

class Renderer;

class UIElement {
public:
    UIElement(const Vector2 &pos, const Vector2 &size, const Vector3 &color);

    // Getters/setters
    const Vector2& GetPosition() const { return mPosition; }
    void SetPosition(const Vector2 &pos) { mPosition = pos; }

    const Vector2& GetSize() const { return mSize; }
    void SetSize(const Vector2 &size) { mSize = size; }

    const Vector3& GetColor() const { return mColor; }
    virtual void SetColor(const Vector3 &color) { mColor = color; }

    const float& GetRotation() const { return mRotation; }
    void SetRotation(const float &rotation) { mRotation = rotation; }

    void SetIsVisible(bool isVisible) { mIsVisible = isVisible; }

    float GetAlpha() const { return mAlpha; }
    void SetAlpha(float alpha) { mAlpha = alpha; }

    virtual void Draw(Renderer* renderer, const Vector2 &screenPos) {}

protected:
    Vector2 mPosition;
    Vector2 mSize;
    Vector3 mColor;
    float mRotation;
    bool mIsVisible;
    float mAlpha;
};
