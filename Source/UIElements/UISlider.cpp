//
// Created by roger on 03/03/2026.
//

#include "UISlider.h"
#include "../Renderer/Renderer.h"
#include <algorithm>

UISlider::UISlider(const std::string& name, class UIFont* font,
                   const Vector2& pos, const Vector2& size,
                   const Vector2& sliderOffset, const Vector2& sliderSize,
                   float minValue, float maxValue, float initialValue,
                   std::function<void(float)> onValueChanged)
    // Inicializamos o UIButton base (passando nullptr para o onClick padrão)
    :UIButton(name, font, nullptr, pos, size, Vector3(30, 30, 30), 40, 1024, Vector2::Zero, TextPos::AlignLeft)
    ,mSliderOffset(sliderOffset)
    ,mSliderSize(sliderSize)
    ,mMinValue(minValue)
    ,mMaxValue(maxValue)
    ,mValue(initialValue)
    ,mOnValueChanged(onValueChanged)
{
    mStep = (mMaxValue - mMinValue) / 10.0f; // Divide em 10 passos por padrão

    mValueText = new UIText("0%", font, 40, 1024, Vector2::Zero, Vector2::Zero, Color::White);
    SetValue(mValue);
}

UISlider::~UISlider() {
    delete mValueText;
    mValueText = nullptr;
}

void UISlider::Draw(Renderer* renderer, const Vector2& screenPos) {
    // Desenhar o fundo do botão total (Container)
    Vector2 containerCenter = screenPos + mPosition + mSize / 2.0f;
    Vector3 containerColor = mHighlighted ? Vector3(0.3f, 0.3f, 0.3f) : Vector3(0.15f, 0.15f, 0.15f);
    renderer->DrawRect(containerCenter, mSize, 0.0f, containerColor, Vector2::Zero, RendererMode::TRIANGLES);

    // Desenhar o texto base
    Vector2 textDrawPos = containerCenter;
    textDrawPos += Vector2(-mSize.x / 2 + mText->GetSize().x / 2 + 10.0f, 0); // Padding na esquerda
    mText->Draw(renderer, textDrawPos);

    // Desenhar o fundo da barra do Slider
    Vector2 trackTopLeft = screenPos + mPosition + mSliderOffset;
    Vector2 trackCenter = trackTopLeft + mSliderSize / 2.0f;
    renderer->DrawRect(trackCenter, mSliderSize, 0.0f, Vector3(0.1f, 0.1f, 0.1f), Vector2::Zero, RendererMode::TRIANGLES);

    // Desenhar a barra preenchida
    float fraction = (mValue - mMinValue) / (mMaxValue - mMinValue);
    if (fraction > 0.0f) {
        Vector2 fillSize(mSliderSize.x * fraction, mSliderSize.y);
        Vector2 fillCenter = trackTopLeft + fillSize / 2.0f;
        Vector3 fillColor = mHighlighted ? Vector3(1.0f, 0.5f, 0.0f) : Vector3(0.78f, 0.39f, 0.0f); // Laranja mais claro se selecionado
        renderer->DrawRect(fillCenter, fillSize, 0.0f, fillColor, Vector2::Zero, RendererMode::TRIANGLES);
    }

    // Desenhar o texto da porcentagem
    // Posiciona o texto logo à direita da barra do slider
    Vector2 percentPos = trackTopLeft + Vector2(mSliderSize.x + mValueText->GetSize().x / 2 + 10.0f, mSliderSize.y / 2.0f);
    mValueText->Draw(renderer, percentPos);
}

void UISlider::UpdateValueText() {
    float fraction = (mValue - mMinValue) / (mMaxValue - mMinValue);
    int percentage = static_cast<int>(std::round(fraction * 100.0f));
    mValueText->SetText(std::to_string(percentage) + "%");
}

void UISlider::SetValue(float value) {
    mValue = std::clamp(value, mMinValue, mMaxValue);
    UpdateValueText();

    if (mOnValueChanged) {
        mOnValueChanged(mValue);
    }
}

void UISlider::Increase() { SetValue(mValue + mStep); }
void UISlider::Decrease() { SetValue(mValue - mStep); }

void UISlider::OnMouseClick(const Vector2& mousePos) {
    // O clique deve ser calculado em relação à barra interna, não ao botão inteiro
    float sliderStartX = mPosition.x + mSliderOffset.x;

    float localX = mousePos.x - sliderStartX;

    // Se clicar antes do início da barra, zera. Se passar, vai ao máximo.
    if (localX < 0.0f) localX = 0.0f;
    if (localX > mSliderSize.x) localX = mSliderSize.x;

    float fraction = localX / mSliderSize.x;
    SetValue(mMinValue + fraction * (mMaxValue - mMinValue));
}