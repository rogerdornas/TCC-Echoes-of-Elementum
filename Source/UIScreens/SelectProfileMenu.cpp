//
// Created by roger on 13/03/2026.
//

#include "SelectProfileMenu.h"
#include "../Game.h"
#include "../Json.h"
#include <fstream>
#include "DeleteProfileMenu.h"

SelectProfileMenu::SelectProfileMenu(class Game *game, const std::string &fontName, bool isClosable)
    :UIScreen(game, fontName, isClosable)
    ,mSaveFolder("../Saves")
{
    GetProfileData();

    SetSize(Vector2(1920, 1080));
    SetPosition(Vector2::Zero);

    Vector2 profileButtonSize(535, 115);
    Vector2 profileButtonPos(692, 406);
    Vector2 deleteButtonPos(1303, 450);
    float distanceBetweenDeleteButton = 115;
    Vector2 locationPos(710, 437);
    Vector2 moneyImagePos(725, 485);
    Vector2 moneyCountPos(745, 485);
    Vector2 totalPlayTimePos(1215, 437);

    // AddImage("../Assets/Sprites/Background/Menu6.png", GetSize() / 2, GetSize());

    auto* background = AddImage("../Assets/Sprites/Menus/FundoPreto.png", GetSize() / 2, GetSize());
    background->SetAlpha(0.6f);

    auto* text = AddText("SELECIONAR PERFIL", Vector2::Zero,  34);
    text->SetPosition(Vector2(960, 340));

    AddImage("../Assets/Sprites/Menus/titulo_sublinhado.png", Vector2(960, 372), Vector2(360, 30));

    auto* button = AddButton("", profileButtonPos + Vector2(0, profileButtonSize.y * 0), profileButtonSize, 34, UIButton::TextPos::Center,
    [this]() {
        mGame->ResetPlayerAndSkillTree();

        mGame->SetSaveSlot(1);
        mGame->LoadGame();
    }
    , false);
    button->UseBackGroundColor();

    button = AddButton("", profileButtonPos + Vector2(0, profileButtonSize.y * 1), profileButtonSize, 34, UIButton::TextPos::Center,
    [this]() {
        mGame->ResetPlayerAndSkillTree();

        mGame->SetSaveSlot(2);
        mGame->LoadGame();
    }
    , false);
    button->UseBackGroundColor();

    button = AddButton("", profileButtonPos + Vector2(0, profileButtonSize.y * 2), profileButtonSize, 34, UIButton::TextPos::Center,
    [this]() {
        mGame->ResetPlayerAndSkillTree();

        mGame->SetSaveSlot(3);
        mGame->LoadGame();
    }
    , false);
    button->UseBackGroundColor();

    button = AddButton("", profileButtonPos + Vector2(0, profileButtonSize.y * 3), profileButtonSize, 34, UIButton::TextPos::Center,
    [this]() {
        mGame->ResetPlayerAndSkillTree();

        mGame->SetSaveSlot(4);
        mGame->LoadGame();
    }
    , false);
    button->UseBackGroundColor();

    if (mProfilesData[0].totalPlayTime > 0) {
        text = AddText(mProfilesData[0].location, Vector2::Zero,  34);
        text->SetPosition(locationPos + Vector2(text->GetSize().x * 0.5f, 0) + Vector2(0, profileButtonSize.y * 0));

        AddImage("../Assets/Sprites/Money/CristalSmall.png", moneyImagePos + Vector2(0, profileButtonSize.y * 0), Vector2(16, 28));

        text = AddText(std::to_string(mProfilesData[0].money), Vector2::Zero,  30);
        text->SetPosition(moneyCountPos + Vector2(text->GetSize().x * 0.5f, 0) + Vector2(0, profileButtonSize.y * 0));

        text = AddText(SecondsToFormatedTime(mProfilesData[0].totalPlayTime), Vector2::Zero,  20);
        text->SetPosition(totalPlayTimePos - Vector2(text->GetSize().x * 0.5f, 0) + Vector2(0, profileButtonSize.y * 0));
    }
    else {
        text = AddText("NOVO JOGO", Vector2::Zero,  34);
        text->SetPosition(Vector2(960, profileButtonPos.y + profileButtonSize.y * 0.5f + profileButtonSize.y * 0));
    }

    if (mProfilesData[1].totalPlayTime > 0) {
        text = AddText(mProfilesData[1].location, Vector2::Zero,  34);
        text->SetPosition(locationPos + Vector2(text->GetSize().x * 0.5f, 0) + Vector2(0, profileButtonSize.y * 1));

        AddImage("../Assets/Sprites/Money/CristalSmall.png", moneyImagePos + Vector2(0, profileButtonSize.y * 1), Vector2(16, 28));

        text = AddText(std::to_string(mProfilesData[1].money), Vector2::Zero,  30);
        text->SetPosition(moneyCountPos + Vector2(text->GetSize().x * 0.5f, 0) + Vector2(0, profileButtonSize.y * 1));

        text = AddText(SecondsToFormatedTime(mProfilesData[1].totalPlayTime), Vector2::Zero,  20);
        text->SetPosition(totalPlayTimePos - Vector2(text->GetSize().x * 0.5f, 0) + Vector2(0, profileButtonSize.y * 1));
    }
    else {
        text = AddText("NOVO JOGO", Vector2::Zero,  34);
        text->SetPosition(Vector2(960, profileButtonPos.y + profileButtonSize.y * 0.5f + profileButtonSize.y * 1));
    }

    if (mProfilesData[2].totalPlayTime > 0) {
        text = AddText(mProfilesData[2].location, Vector2::Zero,  34);
        text->SetPosition(locationPos + Vector2(text->GetSize().x * 0.5f, 0) + Vector2(0, profileButtonSize.y * 2));

        AddImage("../Assets/Sprites/Money/CristalSmall.png", moneyImagePos + Vector2(0, profileButtonSize.y * 2), Vector2(16, 28));

        text = AddText(std::to_string(mProfilesData[2].money), Vector2::Zero,  30);
        text->SetPosition(moneyCountPos + Vector2(text->GetSize().x * 0.5f, 0) + Vector2(0, profileButtonSize.y * 2));

        text = AddText(SecondsToFormatedTime(mProfilesData[2].totalPlayTime), Vector2::Zero,  20);
        text->SetPosition(totalPlayTimePos - Vector2(text->GetSize().x * 0.5f, 0) + Vector2(0, profileButtonSize.y * 2));
    }
    else {
        text = AddText("NOVO JOGO", Vector2::Zero,  34);
        text->SetPosition(Vector2(960, profileButtonPos.y + profileButtonSize.y * 0.5f + profileButtonSize.y * 2));
    }

    if (mProfilesData[3].totalPlayTime > 0) {
        text = AddText(mProfilesData[3].location, Vector2::Zero,  34);
        text->SetPosition(locationPos + Vector2(text->GetSize().x * 0.5f, 0) + Vector2(0, profileButtonSize.y * 3));

        AddImage("../Assets/Sprites/Money/CristalSmall.png", moneyImagePos + Vector2(0, profileButtonSize.y * 3), Vector2(16, 28));

        text = AddText(std::to_string(mProfilesData[3].money), Vector2::Zero,  30);
        text->SetPosition(moneyCountPos + Vector2(text->GetSize().x * 0.5f, 0) + Vector2(0, profileButtonSize.y * 3));

        text = AddText(SecondsToFormatedTime(mProfilesData[3].totalPlayTime), Vector2::Zero,  20);
        text->SetPosition(totalPlayTimePos - Vector2(text->GetSize().x * 0.5f, 0) + Vector2(0, profileButtonSize.y * 3));
    }
    else {
        text = AddText("NOVO JOGO", Vector2::Zero,  34);
        text->SetPosition(Vector2(960, profileButtonPos.y + profileButtonSize.y * 0.5f + profileButtonSize.y * 3));
    }

    if (mProfilesData[0].totalPlayTime > 0) {
        AddButton("EXCLUIR", deleteButtonPos + Vector2(0, distanceBetweenDeleteButton * 0), Vector2::Zero, 25, UIButton::TextPos::Center,
        [this]() {
            new DeleteProfileMenu(mGame, "../Assets/Fonts/K2D-Bold.ttf", mProfilesData[0], 1);
            Close();
        }
        , true);
    }

    if (mProfilesData[1].totalPlayTime > 0) {
        AddButton("EXCLUIR", deleteButtonPos + Vector2(0, distanceBetweenDeleteButton * 1), Vector2::Zero, 25, UIButton::TextPos::Center,
        [this]() {
            new DeleteProfileMenu(mGame, "../Assets/Fonts/K2D-Bold.ttf", mProfilesData[1], 2);
            Close();
        }
        , true);
    }

    if (mProfilesData[2].totalPlayTime > 0) {
        AddButton("EXCLUIR", deleteButtonPos + Vector2(0, distanceBetweenDeleteButton * 2), Vector2::Zero, 25, UIButton::TextPos::Center,
        [this]() {
            new DeleteProfileMenu(mGame, "../Assets/Fonts/K2D-Bold.ttf", mProfilesData[2], 3);
            Close();
        }
        , true);
    }

    if (mProfilesData[3].totalPlayTime > 0) {
        AddButton("EXCLUIR", deleteButtonPos + Vector2(0, distanceBetweenDeleteButton * 3), Vector2::Zero, 25, UIButton::TextPos::Center,
        [this]() {
            new DeleteProfileMenu(mGame, "../Assets/Fonts/K2D-Bold.ttf", mProfilesData[3], 4);
            Close();
        }
        , true);
    }

    AddButton("VOLTAR", Vector2(887, 930), Vector2::Zero, 34, UIButton::TextPos::Center,
    [this]() {
        Close();
    }
    , true);
}

void SelectProfileMenu::Draw(class Renderer *renderer) {
    if (!mIsVisible) {
        return;
    }
    UIScreen::Draw(renderer);

    mGame->GetRenderer()->DrawLine(Vector2(692, 406), Vector2(1227, 406), Vector3(0.5f, 0.5f, 0.5f), 1, Vector2::Zero, 1.0f);
    mGame->GetRenderer()->DrawLine(Vector2(692, 521), Vector2(1227, 521), Vector3(0.5f, 0.5f, 0.5f), 1, Vector2::Zero, 1.0f);
    mGame->GetRenderer()->DrawLine(Vector2(692, 636), Vector2(1227, 636), Vector3(0.5f, 0.5f, 0.5f), 1, Vector2::Zero, 1.0f);
    mGame->GetRenderer()->DrawLine(Vector2(692, 751), Vector2(1227, 751), Vector3(0.5f, 0.5f, 0.5f), 1, Vector2::Zero, 1.0f);
    mGame->GetRenderer()->DrawLine(Vector2(692, 866), Vector2(1227, 866), Vector3(0.5f, 0.5f, 0.5f), 1, Vector2::Zero, 1.0f);
}

void SelectProfileMenu::GetProfileData() {
    const int MAX_SLOTS = 4;

    mProfilesData.resize(MAX_SLOTS);

    for (int i = 0; i < MAX_SLOTS; i++) {
        std::string filePath = mSaveFolder + "/slot_" + std::to_string(i + 1) + ".json";
        std::ifstream file(filePath);

        if (!file.is_open()) {
            continue;
        }

        nlohmann::json j;
        file >> j;

        mProfilesData[i].location = j["game"]["scene"];
        mProfilesData[i].totalPlayTime = j["game"]["total_play_time"];
        mProfilesData[i].money = j["player"]["money"];

        file.close();
    }
}

std::string SelectProfileMenu::SecondsToFormatedTime(float seconds) {
    int secondsInt = static_cast<int>(seconds);
    int hours = secondsInt / 3600;

    int minutes = (secondsInt % 3600) / 60;

    std::ostringstream oss;

    oss << hours << "h "
        << std::setfill('0') << std::setw(2) << minutes << "m";

    return oss.str();
}
