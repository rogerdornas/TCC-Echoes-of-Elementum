//
// Created by roger on 10/04/2026.
//
#pragma once

#include "UIButton.h"
#include <string>

class UISkillNodeButton: public UIButton
{
public:
    enum class NodeState {
        Locked,
        Available,
        Unlocked
    };

    enum class NodeType {
        Earth,
        Fire,
        Ice,
        Lightning,
        Neutral
    };

    UISkillNodeButton(const std::string& skillId,
                      NodeType nodeType,
                      std::function<void()> onClick,
                      std::function<void(const std::string&)> onSelect,
                      const Vector2& pos,
                      Renderer* renderer);

    ~UISkillNodeButton();

    void SetState(NodeState state);
    NodeState GetState() const { return mState; }

    std::string GetSkillId() { return mSkillId ;}

    void SetIcon(const std::string& iconPath);

    // Intercepta o foco para atualizar o painel direito automaticamente
    void SetHighlighted(bool sel) override;

    void SetIsSelectedNode(bool isSelected) { mIsSelectedNode = isSelected; }

    void Draw(Renderer* renderer, const Vector2 &screenPos) override;

private:
    std::string mSkillId;
    NodeState mState;

    // Callback para quando o botão ganha foco
    std::function<void(const std::string&)> mOnSelect;

    // Texturas dos Hexágonos
    Texture* mLockedTexture;
    Texture* mAvailableTexture;
    Texture* mUnlockedTexture;

    bool mIsSelectedNode = false;

    // Ícone da habilidade
    // Texture* mIconTexture;
};

