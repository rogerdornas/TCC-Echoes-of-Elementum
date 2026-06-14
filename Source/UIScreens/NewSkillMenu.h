//
// Created by roger on 14/06/2026.
//

#pragma once

#include "../Actors/Skill.h"
#include "../UIElements/UIScreen.h"
#include "../Game.h"

class NewSkillMenu : public UIScreen
{
public:
    NewSkillMenu(class Game* game, const std::string& fontName, bool isClosable = true, Skill::SkillType skillType = Skill::SkillType::Dash);

    void Update(float deltaTime) override;
    void Close() override;

private:
    // SISTEMA DE MENSAGEM
    static const int MAX_MESSAGE_PARTS = 5; // Suporta até 5 recortes (ex: [Texto] [Ícone] [Texto] [Ícone] [Texto])

    std::vector<UIText*> mMessageTexts;
    std::vector<UIText*> mMessageActionTexts;

    class UIFont* mIconFont;

    std::string mMessageTemplate;
    Game::InputPlayerMode mLastInputMode;
    Game::InputController mLastController;
    bool mMessageNeedsRebuild;

    void RebuildMessage();
};
