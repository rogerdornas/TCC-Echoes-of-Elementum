//
// Created by roger on 14/03/2026.
//

#include "DeleteProfileMenu.h"
#include "../Game.h"

DeleteProfileMenu::DeleteProfileMenu(class Game *game, const std::string &fontName, const SelectProfileMenu::ProfileData& profileData, int slot, bool isClosable)
    :UIScreen(game, fontName, isClosable)
    ,mSlot(slot)
{
    SetSize(Vector2(1920, 1080));
    SetPosition(Vector2::Zero);

    Vector2 profileButtonSize(535, 115);
    Vector2 profileButtonPos(692, 406);
    Vector2 locationPos(710, 437);
    Vector2 moneyImagePos(725, 485);
    Vector2 moneyCountPos(745, 485);
    Vector2 totalPlayTimePos(1215, 437);

    auto* background = AddImage("../Assets/Sprites/Menus/FundoPreto.png", GetSize() / 2, GetSize());
    background->SetAlpha(0.6f);

    background = AddImage("../Assets/Sprites/Menus/FundoPreto.png", profileButtonPos + profileButtonSize * 0.5f, profileButtonSize);
    background->SetAlpha(0.5f);

    auto* text = AddText(profileData.location, Vector2::Zero,  34);
    text->SetPosition(locationPos + Vector2(text->GetSize().x * 0.5f, 0) + Vector2(0, profileButtonSize.y * 0));

    AddImage("../Assets/Sprites/Money/CristalSmall.png", moneyImagePos + Vector2(0, profileButtonSize.y * 0), Vector2(16, 28));

    text = AddText(std::to_string(profileData.money), Vector2::Zero,  30);
    text->SetPosition(moneyCountPos + Vector2(text->GetSize().x * 0.5f, 0) + Vector2(0, profileButtonSize.y * 0));

    text = AddText(SecondsToFormatedTime(profileData.totalPlayTime), Vector2::Zero,  20);
    text->SetPosition(totalPlayTimePos - Vector2(text->GetSize().x * 0.5f, 0) + Vector2(0, profileButtonSize.y * 0));

    text = AddText("DESEJA EXCLUIR?", Vector2::Zero,  34);
    text->SetPosition(Vector2(960, 600));

    AddImage("../Assets/Sprites/Menus/titulo_sublinhado.png", Vector2(960, 632), Vector2(360, 30));

    AddButton("SIM", Vector2(920, 670), Vector2::Zero, 34, UIButton::TextPos::Center,
    [this]() {
        mGame->GetSaveManager()->DeleteSave(mSlot);
        Close();
    }
    , true);

    AddButton("NÃO", Vector2(917, 735), Vector2::Zero, 34, UIButton::TextPos::Center,
    [this]() {
        Close();
    }
    , true);

}

void DeleteProfileMenu::Draw(class Renderer *renderer) {
    if (!mIsVisible) {
        return;
    }
    UIScreen::Draw(renderer);

    mGame->GetRenderer()->DrawLine(Vector2(692, 406), Vector2(1227, 406), Vector3(0.5f, 0.5f, 0.5f), 1, Vector2::Zero, 1.0f);
    mGame->GetRenderer()->DrawLine(Vector2(692, 521), Vector2(1227, 521), Vector3(0.5f, 0.5f, 0.5f), 1, Vector2::Zero, 1.0f);
}

std::string DeleteProfileMenu::SecondsToFormatedTime(float seconds) {
    int secondsInt = static_cast<int>(seconds);
    int hours = secondsInt / 3600;

    int minutes = (secondsInt % 3600) / 60;

    std::ostringstream oss;

    oss << hours << "h "
        << std::setfill('0') << std::setw(2) << minutes << "m";

    return oss.str();
}

void DeleteProfileMenu::Close() {
    UIScreen::Close();
    new SelectProfileMenu(mGame, "../Assets/Fonts/K2D-Bold.ttf");
}
