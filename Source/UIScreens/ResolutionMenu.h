//
// Created by roger on 24/03/2026.
//

#pragma once

#include "../UIElements/UIScreen.h"

class ResolutionMenu : public UIScreen
{
public:
    ResolutionMenu(class Game* game, const std::string& fontName, UIScreen* videoMenu, bool isClosable = true);
    void Close() override;
private:
    std::vector<Vector2> mResolutionsList;
    UIScreen* mVideoMenu;
};
