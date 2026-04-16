//
// Created by roger on 10/04/2026.
//

#include "UISkillNodeButton.h"
#include "../Renderer/Renderer.h"

UISkillNodeButton::UISkillNodeButton(const std::string &skillId,
                                     NodeType nodeType,
                                     std::function<void()> onClick,
                                     std::function<void(const std::string &)> onSelect,
                                     const Vector2 &pos, Renderer *renderer)
    :UIButton("", nullptr, onClick, pos, Vector2(31, 31), false, Color::White, 0, 0, Vector2::Zero, TextPos::Center, Color::White, renderer)
    ,mSkillId(skillId)
    ,mState(NodeState::Locked)
    ,mOnSelect(onSelect)
    // ,mIconTexture(nullptr)
{
    switch (nodeType) {
        case NodeType::Earth:
            mAvailableTexture = mRenderer->GetTexture("../Assets/Sprites/SkillTree/EarthNodeAvailable.png");
            mUnlockedTexture = mRenderer->GetTexture("../Assets/Sprites/SkillTree/EarthNodeUnlocked.png");
            break;

        case NodeType::Fire:
            mAvailableTexture = mRenderer->GetTexture("../Assets/Sprites/SkillTree/FireNodeAvailable.png");
            mUnlockedTexture = mRenderer->GetTexture("../Assets/Sprites/SkillTree/FireNodeUnlocked.png");
            break;

        case NodeType::Ice:
            mAvailableTexture = mRenderer->GetTexture("../Assets/Sprites/SkillTree/IceNodeAvailable.png");
            mUnlockedTexture = mRenderer->GetTexture("../Assets/Sprites/SkillTree/IceNodeUnlocked.png");
            break;

        case NodeType::Lightning:
            mAvailableTexture = mRenderer->GetTexture("../Assets/Sprites/SkillTree/LightningNodeAvailable.png");
            mUnlockedTexture = mRenderer->GetTexture("../Assets/Sprites/SkillTree/LightningNodeUnlocked.png");
            break;

        case NodeType::Neutral:
            mAvailableTexture = mRenderer->GetTexture("../Assets/Sprites/SkillTree/NeutralNodeAvailable.png");
            mUnlockedTexture = mRenderer->GetTexture("../Assets/Sprites/SkillTree/NeutralNodeUnlocked.png");
            break;
    }
    mLockedTexture = mRenderer->GetTexture("../Assets/Sprites/SkillTree/NodeLocked.png");

    // Removemos os comportamentos visuais do botão padrão
    mUseImageSelector = false;
    mUseBackGroundColor = false;

    SetHoldable(true, 0.6f);
}

UISkillNodeButton::~UISkillNodeButton() {
    mLockedTexture = nullptr;
    mAvailableTexture = nullptr;
    mUnlockedTexture = nullptr;
//     mIconTexture = nullptr;
}

void UISkillNodeButton::SetState(NodeState state) {
    mState = state;

    SetInteractable(mState == NodeState::Available);
}

void UISkillNodeButton::SetIcon(const std::string& iconPath) {
    // if (!iconPath.empty()) {
    //     mIconTexture = mRenderer->GetTexture(iconPath);
    // }
}

void UISkillNodeButton::SetHighlighted(bool sel) {
    bool changed = (mHighlighted != sel);

    UIButton::SetHighlighted(sel);

    // Se o botão acabou de ser selecionado
    if (sel && changed && mOnSelect) {
        mOnSelect(mSkillId);
    }
}

void UISkillNodeButton::Draw(Renderer* renderer, const Vector2 &screenPos) {
    if (!mIsVisible) {
        return;
    }

    Vector2 drawPos = screenPos + mPosition + mSize * 0.5f;

    Texture* currentHex = mLockedTexture;
    if (mState == NodeState::Available) currentHex = mAvailableTexture;
    else if (mState == NodeState::Unlocked) currentHex = mUnlockedTexture;

    float holdPercent = mHoldTimeRequired > 0.0f ? (mCurrentHoldTime / mHoldTimeRequired) : 0.0f;

    // O botão fica grande se estiver selecionado, e cresce ainda mais conforme segura
    float scale = (mHighlighted || mIsSelectedNode) ? 1.3f : 1.0f;
    scale += holdPercent * 0.3f; // Cresce +30% durante o tempo de seguro

    if (currentHex) {
        renderer->DrawTexture(drawPos, mSize * scale, 0.0f, Color::White, currentHex,
                              Vector4::UnitRect, Vector2::Zero, Vector2::One, 1.0f, 1.0f);
    }

    if (holdPercent > 0.0f && holdPercent < 1.0f) {
        Vector2 barMax(40.0f, 6.0f); // Tamanho da barra
        Vector2 barPos = drawPos + Vector2(0, mSize.y * 0.8f);

        // Fundo preto
        renderer->DrawRect(barPos, barMax, 0.0f, Color::Black, Vector2::Zero, RendererMode::TRIANGLES, 0.8f);
        // Preenchimento branco
        renderer->DrawRect(barPos, Vector2(barMax.x * holdPercent, barMax.y), 0.0f, Color::White, Vector2::Zero, RendererMode::TRIANGLES, 1.0f);
    }

    // Desenha o Ícone por cima
    // if (mIconTexture) {
    //     // Reduzimos o tamanho do ícone para caber dentro do hexágono
    //     renderer->DrawTexture(drawPos, (mSize * 0.6f) * scale, 0.0f, Color::White, mIconTexture,
    //                           Vector4::UnitRect, Vector2::Zero, Vector2::One, 0.0f, 1.0f);
    // }
}
