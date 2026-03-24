//
// Created by roger on 24/03/2026.
//

#include "ResolutionMenu.h"

#include "VideoMenu.h"
#include "../Game.h"

ResolutionMenu::ResolutionMenu(class Game *game, const std::string &fontName, UIScreen* videoMenu, bool isClosable)
    :UIScreen(game, fontName, isClosable)
    ,mVideoMenu(videoMenu)
{
    mResolutionsList = mGame->GetResolutionsList();
    Vector2 buttonSize(215, 41);

    SetSize(Vector2(buttonSize.x, buttonSize.y * mResolutionsList.size()));

    for (int i = 0; i < mResolutionsList.size(); i++) {
        auto* button = AddButton(std::to_string(static_cast<int>(mResolutionsList[i].x)) + " x " + std::to_string(static_cast<int>(mResolutionsList[i].y)), Vector2(0, buttonSize.y * i), buttonSize, 34, UIButton::TextPos::AlignLeft,
        [this, i]() {
            mGame->GetRenderer()->CreateRenderTarget(static_cast<int>(mResolutionsList[i].x), static_cast<int>(mResolutionsList[i].y));
            Close();
        }
        , false, Vector2(19, 0));
        button->UseBackGroundColor();
    }
}

void ResolutionMenu::Close() {
    UIScreen::Close();
    mVideoMenu->RefreshTexts();
}
