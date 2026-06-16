//
// Created by roger on 16/06/2026.
//

#pragma once

#include "../UIElements/UIScreen.h"

class ScenarioSelectorMenu : public UIScreen
{
public:
    ScenarioSelectorMenu(class Game* game, const std::string& fontName, bool isClosable = true);

private:
    void LoadScenario(const std::string &scenarioName, Vector2 startPosition);
};

