//
// Created by roger on 16/06/2026.
//

#include "ScenarioSelectorMenu.h"
#include "../Game.h"

ScenarioSelectorMenu::ScenarioSelectorMenu(class Game *game, const std::string &fontName, bool isClosable)
    :UIScreen(game, fontName, isClosable)
{
    SetSize(Vector2(1920, 1080));
    SetPosition(Vector2::Zero);

    auto* background = AddImage("../Assets/Sprites/Menus/FundoPreto.png", Vector2(1920, 1080) * 0.5f, Vector2(1920, 1080));
    background->SetAlpha(0.6f);

    background = AddImage("../Assets/Sprites/Menus/FundoPretoArredondado.png", Vector2(960, 600), Vector2(1700, 900));
    background->SetAlpha(0.4f);

    AddText("SELEÇÃO DE CENÁRIO", Vector2(960, 340), 40, Vector3(1.0f, 1.0f, 1.0f));

    Vector2 buttonSize(450, 100);
    Vector2 buttonPos(800, 370);
    float distanceBetweenButtons = 70.0f;

    AddButton("GOLEM DE CRISTAL", buttonPos + Vector2(0, 0 * distanceBetweenButtons),
                 buttonSize, 34, UIButton::TextPos::AlignLeft,
    [this]() {
        LoadScenario("ModoAvancado/Golem de Cristal", Vector2(598, 1135));
    }
    , false);

    AddButton("GOLEM DE FOGO", buttonPos + Vector2(0, 1 * distanceBetweenButtons),
             buttonSize, 34, UIButton::TextPos::AlignLeft,
    [this]() {
        LoadScenario("ModoAvancado/Golem de Fogo", Vector2(598, 1903));
    }
    , false);

    AddButton("RAPOSA GUERREIRA", buttonPos + Vector2(0, 2 * distanceBetweenButtons),
            buttonSize, 34, UIButton::TextPos::AlignLeft,
    [this]() {
        LoadScenario("ModoAvancado/Raposa Guerreira", Vector2(650, 1014));
    }
    , false);

    AddButton("FANTASMA ESPELHADO", buttonPos + Vector2(0, 3 * distanceBetweenButtons),
            buttonSize, 34, UIButton::TextPos::AlignLeft,
    [this]() {
        LoadScenario("ModoAvancado/Fantasma Espelhado", Vector2(1622, 3439));
    }
    , false);

    AddButton("ESPREITADOR VERDE", buttonPos + Vector2(0, 4 * distanceBetweenButtons),
            buttonSize, 34, UIButton::TextPos::AlignLeft,
    [this]() {
        LoadScenario("ModoAvancado/Espreitador Verde", Vector2(1088, 1810));
    }
    , false);

    AddButton("IRMÃOS GANCHO", buttonPos + Vector2(0, 5 * distanceBetweenButtons),
            buttonSize, 34, UIButton::TextPos::AlignLeft,
    [this]() {
        LoadScenario("ModoAvancado/Irmaos Gancho", Vector2(918, 1133));
    }
    , false);

    AddButton("ARENA DOS ESQUECIDOS", buttonPos + Vector2(0, 6 * distanceBetweenButtons),
            buttonSize, 34, UIButton::TextPos::AlignLeft,
    [this]() {
        LoadScenario("ModoAvancado/Arena dos Esquecidos", Vector2(384, 3180));
    }
    , false);

    AddButton("VOLTAR", Vector2(887, 930), Vector2::Zero, 34, UIButton::TextPos::Center,
    [this]() {
        Close();
    }
    , true);
}

void ScenarioSelectorMenu::LoadScenario(const std::string &scenarioName, Vector2 startPosition) {
    if (mGame->GetGameScene() == Game::GameScene::MainMenu) {
        if (mGame->GetSceneManagerState() == Game::SceneManagerState::None) {
            mGame->ResetPlayerAndSkillTree();
            mGame->SetSaveSlot(5);
            mGame->LoadAdvancedModeGame(scenarioName, startPosition);
        }
    }
    else {
        mGame->SetGoingToNextLevel();
        mGame->LoadNextLevel(scenarioName, 0.5f);
    }

    mGame->SetIsPlayingAdvancedMode(true);
}
