//
// Created by roger on 13/03/2026.
//

#pragma once

#include "../UIElements/UIScreen.h"

class KeyboardMenu : public UIScreen
{
public:
    KeyboardMenu(class Game* game, const std::string& fontName, bool isClosable = true);
    void Draw(class Renderer *renderer) override;

private:
    Vector2 mButtonSize;
    Vector2 mButtonPos;
    Vector2 mButtonPos2;
    Vector2 mButtonTextOffset;
    Vector2 mTextPos;
    Vector2 mTextPos2;
};


