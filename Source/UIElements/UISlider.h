//
// Created by roger on 03/03/2026.
//

#pragma once
#include "UIButton.h"

class UISlider : public UIButton
{
public:
    UISlider(const std::string& name, class UIFont* font,
             const Vector2& pos, const Vector2& size,
             const Vector2& sliderOffset, const Vector2& sliderSize,
             float minValue, float maxValue, float initialValue,
             int textPointSize, int valuePointSize,
             std::function<void(float)> onValueChanged,
             Renderer* renderer = nullptr);

    ~UISlider();

    void Draw(Renderer* renderer, const Vector2& screenPos) override;

    bool IsDraggable() const override { return true; }

    //Calcula o clique baseado apenas na área da barra interna
    void OnMouseClick(const Vector2& mousePos) override;

    // Métodos para o teclado/controle
    void Increase();
    void Decrease();

    float GetValue() const { return mValue; }
    void SetValue(float value);

private:
    void UpdateValueText();

    float mMinValue;
    float mMaxValue;
    float mValue;
    float mStep; // Quanto aumenta/diminui a cada clique na setinha

    Vector2 mSliderOffset; // Posição relativa ao top-left do botão
    Vector2 mSliderSize;

    // Texto dinâmico da porcentagem
    UIText* mValueText;

    std::function<void(float)> mOnValueChanged;
};