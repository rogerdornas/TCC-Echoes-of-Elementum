//
// Created by roger on 09/04/2026.
//

#pragma once

#include "../UIElements/UIScreen.h"

class CheckPointMenu : public UIScreen
{
public:
    CheckPointMenu(class Game* game, const std::string& fontName, bool isClosable = true);
    void Update(float deltaTime) override;
    void Close() override;

    void HandleKeyRelease(int key, int controllerButton) override;

private:
    float mDelayToCanClose;
};

