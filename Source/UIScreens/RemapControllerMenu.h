//
// Created by roger on 17/03/2026.
//

#pragma once

#include "../Game.h"
#include "../UIElements/UIScreen.h"

class RemapControllerMenu : public UIScreen
{
public:
    RemapControllerMenu(class Game* game, const std::string& fontName, bool isClosable = true);
    void Draw(class Renderer *renderer) override;
    void Close() override;
private:
    std::string GetBindingText(Game::Action action) const;
    void RefreshTexts() override;

    Vector2 mButtonSize;
    Vector2 mButtonPos;
    Vector2 mButtonPos2;
    Vector2 mButtonTextOffset;
    Vector2 mTextPos;
    Vector2 mTextPos2;
};

