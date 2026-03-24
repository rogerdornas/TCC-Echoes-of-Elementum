//
// Created by roger on 13/03/2026.
//

#pragma once

#include "../UIElements/UIScreen.h"

class VideoMenu : public UIScreen
{
public:
    VideoMenu(class Game* game, const std::string& fontName, bool isClosable = true);
private:
    void RefreshTexts() override;

    UIText* mResolutionText;
    Vector2 mFullScreenTextPos;
    Vector2 mResolutionTextPos;
};
