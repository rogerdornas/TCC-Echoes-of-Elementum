//
// Created by roger on 10/04/2026.
//

#include "SkillMenu.h"
#include "../Game.h"
#include "../PlayerSkillManager.h"
#include <cfloat>

SkillMenu::SkillMenu(class Game *game, const std::string &fontName, bool isClosable)
    :UIScreen(game, fontName, isClosable)
    ,mPlayerGoldTextPosition(1224, 387)
    ,mPlayerFireStoneTextPosition(1346, 387)
    ,mPlayerEarthStoneTextPosition(1402, 387)
    ,mPlayerIceStoneTextPosition(1458, 387)
    ,mPlayerLightningStoneTextPosition(1514, 387)
    ,mTitleTextPosition(1200, 451)
    ,mTypeTextPosition(1200, 492)
    ,mDescriptionTextPosition(1200, 568)
    ,mHowToUseTextPosition(1200, 692)
    ,mGoldCostTextPosition(1231, 772)
    ,mStoneCostTextPosition(1231, 820)
    ,mUnlockTextPosition(1200, 862)
{
    SetSize(Vector2(1920, 1080));
    SetPosition(Vector2::Zero);

    auto* background = AddImage("../Assets/Sprites/Menus/FundoPreto.png", Vector2(1920, 1080) * 0.5f, Vector2(1920, 1080));
    background->SetAlpha(0.6f);

    auto* background2 = AddImage("../Assets/Sprites/Menus/FundoPreto.png", Vector2(1920, 1080) * 0.5f, Vector2(1840, 1040));
    background2->SetAlpha(0.4f);

    auto* text = AddText("HABILIDADES", Vector2::Zero,  34);
    text->SetPosition(Vector2(960, 135));

    AddImage("../Assets/Sprites/Menus/titulo_sublinhado.png", Vector2(960, 168), Vector2(360, 30));

    // Painel direito
    int wrapLength = 650;

    mPlayerGoldImage = AddImage("../Assets/Sprites/Money/CristalSmall.png", Vector2(1210, 398), Vector2(21, 36));
    mPlayerFireStoneImage = AddImage("../Assets/Sprites/Money/CristalLarge.png", Vector2(1331, 398), Vector2(24, 42));
    mPlayerEarthStoneImage = AddImage("../Assets/Sprites/Money/CristalLarge.png", Vector2(1387, 398), Vector2(24, 42));
    mPlayerIceStoneImage = AddImage("../Assets/Sprites/Money/CristalLarge.png", Vector2(1443, 398), Vector2(24, 42));
    mPlayerLightningStoneImage = AddImage("../Assets/Sprites/Money/CristalLarge.png", Vector2(1499, 398), Vector2(24, 42));

    mPlayerGoldText = AddText("", Vector2::Zero, 20);
    mPlayerGoldText->SetPosition(mPlayerGoldTextPosition);

    mPlayerFireStoneText = AddText("", Vector2::Zero, 20);
    mPlayerFireStoneText->SetPosition(mPlayerFireStoneTextPosition);

    mPlayerEarthStoneText = AddText("", Vector2::Zero, 20);
    mPlayerEarthStoneText->SetPosition(mPlayerEarthStoneTextPosition);

    mPlayerIceStoneText = AddText("", Vector2::Zero, 20);
    mPlayerIceStoneText->SetPosition(mPlayerIceStoneTextPosition);

    mPlayerLightningStoneText = AddText("", Vector2::Zero, 20);
    mPlayerLightningStoneText->SetPosition(mPlayerLightningStoneTextPosition);

    mTitleText = AddText("", Vector2::Zero, 34, Color::White, wrapLength);
    mTitleText->SetPosition(mTitleTextPosition);

    mTypeText = AddText("", Vector2::Zero, 20, Color::White, wrapLength);
    mTypeText->SetPosition(mTypeTextPosition);

    mDescriptionTitle = AddText("DESCRIÇÃO", Vector2::Zero, 20, Vector3(0.64f, 0.64f, 0.64f), wrapLength);
    mDescriptionTitle->SetPosition(Vector2(1200, 543) + mDescriptionTitle->GetSize() * 0.5f);

    mDescriptionText = AddText("", Vector2::Zero, 20, Color::White, wrapLength);
    mDescriptionText->SetPosition(mDescriptionTextPosition);

    mHowToUseTitle = AddText("COMO USAR", Vector2::Zero, 20, Vector3(0.64f, 0.64f, 0.64f), wrapLength);
    mHowToUseTitle->SetPosition(Vector2(1200, 666) + mHowToUseTitle->GetSize() * 0.5f);

    mHowToUseText = AddText("", Vector2::Zero, 20, Color::White, wrapLength);
    mHowToUseText->SetPosition(mHowToUseTextPosition);

    mUnlockCostTitle = AddText("NECESSÁRIO", Vector2::Zero, 20, Vector3(0.64f, 0.64f, 0.64f), wrapLength);
    mUnlockCostTitle->SetPosition(Vector2(1200, 737) + mUnlockCostTitle->GetSize() * 0.5f);

    mGoldImage = AddImage("../Assets/Sprites/Money/CristalSmall.png", Vector2(1210, 783), Vector2(21, 36));

    mGoldCostText = AddText("", Vector2::Zero, 20);
    mGoldCostText->SetPosition(mGoldCostTextPosition);

    mStoneImage = AddImage("../Assets/Sprites/Money/CristalLarge.png", Vector2(1210, 831), Vector2(24, 42));

    mStoneCostText = AddText("", Vector2::Zero, 20);
    mStoneCostText->SetPosition(mStoneCostTextPosition);

    mUnlockText = AddText("", Vector2::Zero, 34);
    mUnlockText->SetPosition(mUnlockTextPosition);

    AddImage("../Assets/Sprites/HUD/Fire.png", Vector2(565, 241), Vector2(60, 76));

    AddImage("../Assets/Sprites/HUD/Lightning.png", Vector2(918, 600), Vector2(65, 83));

    AddImage("../Assets/Sprites/HUD/Ice.png", Vector2(210, 600), Vector2(75, 95));

    AddImage("../Assets/Sprites/HUD/Earth.png", Vector2(565, 946), Vector2(90, 114));


    // Fire Tree
    AddSkillNodeButton(
        "fire_ball",
        UISkillNodeButton::NodeType::Fire,
        Vector2(550, 287),
        [this]() { AttemptUnlockSkill(); },
        [this](const std::string& id) { SelectSkill(id); }
    );

    AddSkillNodeButton(
        "fire_ball_damage_1",
        UISkillNodeButton::NodeType::Fire,
        Vector2(511, 326),
        [this]() { AttemptUnlockSkill(); },
        [this](const std::string& id) { SelectSkill(id); }
    );

    AddSkillNodeButton(
        "sword_damage_fire",
        UISkillNodeButton::NodeType::Neutral,
        Vector2(589, 326),
        [this]() { AttemptUnlockSkill(); },
        [this](const std::string& id) { SelectSkill(id); }
    );

    AddSkillNodeButton(
        "fire_wisp",
        UISkillNodeButton::NodeType::Fire,
        Vector2(550, 365),
        [this]() { AttemptUnlockSkill(); },
        [this](const std::string& id) { SelectSkill(id); }
    );

    AddSkillNodeButton(
        "mana_max_fire",
        UISkillNodeButton::NodeType::Neutral,
        Vector2(511, 404),
        [this]() { AttemptUnlockSkill(); },
        [this](const std::string& id) { SelectSkill(id); }
    );

    AddSkillNodeButton(
        "fire_wisp_attack",
        UISkillNodeButton::NodeType::Fire,
        Vector2(589, 404),
        [this]() { AttemptUnlockSkill(); },
        [this](const std::string& id) { SelectSkill(id); }
    );

    AddSkillNodeButton(
        "fire_ball_damage_2",
        UISkillNodeButton::NodeType::Fire,
        Vector2(550, 443),
        [this]() { AttemptUnlockSkill(); },
        [this](const std::string& id) { SelectSkill(id); }
    );

    AddSkillNodeButton(
        "fire_ball_damage_3",
        UISkillNodeButton::NodeType::Fire,
        Vector2(550, 498),
        [this]() { AttemptUnlockSkill(); },
        [this](const std::string& id) { SelectSkill(id); }
    );

    // Lightning Tree
    AddSkillNodeButton(
        "lightning_spear",
        UISkillNodeButton::NodeType::Lightning,
        Vector2(845, 582),
        [this]() { AttemptUnlockSkill(); },
        [this](const std::string& id) { SelectSkill(id); }
    );

    AddSkillNodeButton(
        "lightning_spear_chain_shock",
        UISkillNodeButton::NodeType::Lightning,
        Vector2(806, 543),
        [this]() { AttemptUnlockSkill(); },
        [this](const std::string& id) { SelectSkill(id); }
    );

    AddSkillNodeButton(
        "max_health_points_lightning",
        UISkillNodeButton::NodeType::Neutral,
        Vector2(806, 621),
        [this]() { AttemptUnlockSkill(); },
        [this](const std::string& id) { SelectSkill(id); }
    );

    AddSkillNodeButton(
        "frenzy_mode",
        UISkillNodeButton::NodeType::Lightning,
        Vector2(767, 582),
        [this]() { AttemptUnlockSkill(); },
        [this](const std::string& id) { SelectSkill(id); }
    );

    AddSkillNodeButton(
        "sword_range_lightning",
        UISkillNodeButton::NodeType::Neutral,
        Vector2(728, 543),
        [this]() { AttemptUnlockSkill(); },
        [this](const std::string& id) { SelectSkill(id); }
    );

    AddSkillNodeButton(
        "frenzy_mode_duration",
        UISkillNodeButton::NodeType::Lightning,
        Vector2(728, 621),
        [this]() { AttemptUnlockSkill(); },
        [this](const std::string& id) { SelectSkill(id); }
    );

    AddSkillNodeButton(
        "lightning_spear_damage",
        UISkillNodeButton::NodeType::Lightning,
        Vector2(689, 582),
        [this]() { AttemptUnlockSkill(); },
        [this](const std::string& id) { SelectSkill(id); }
    );

    AddSkillNodeButton(
        "lightning_dash_damage",
        UISkillNodeButton::NodeType::Lightning,
        Vector2(635, 582),
        [this]() { AttemptUnlockSkill(); },
        [this](const std::string& id) { SelectSkill(id); }
    );


    // Ice Tree
    AddSkillNodeButton(
        "freeze",
        UISkillNodeButton::NodeType::Ice,
        Vector2(255, 582),
        [this]() { AttemptUnlockSkill(); },
        [this](const std::string& id) { SelectSkill(id); }
    );

    AddSkillNodeButton(
        "freeze_duration",
        UISkillNodeButton::NodeType::Ice,
        Vector2(294, 621),
        [this]() { AttemptUnlockSkill(); },
        [this](const std::string& id) { SelectSkill(id); }
    );

    AddSkillNodeButton(
        "sword_attack_speed_ice",
        UISkillNodeButton::NodeType::Neutral,
        Vector2(294, 543),
        [this]() { AttemptUnlockSkill(); },
        [this](const std::string& id) { SelectSkill(id); }
    );

    AddSkillNodeButton(
        "glide",
        UISkillNodeButton::NodeType::Ice,
        Vector2(333, 582),
        [this]() { AttemptUnlockSkill(); },
        [this](const std::string& id) { SelectSkill(id); }
    );

    AddSkillNodeButton(
        "heal_count_ice",
        UISkillNodeButton::NodeType::Neutral,
        Vector2(372, 621),
        [this]() { AttemptUnlockSkill(); },
        [this](const std::string& id) { SelectSkill(id); }
    );

    AddSkillNodeButton(
        "ice_test_1",
        UISkillNodeButton::NodeType::Ice,
        Vector2(372, 543),
        [this]() { AttemptUnlockSkill(); },
        [this](const std::string& id) { SelectSkill(id); }
    );

    AddSkillNodeButton(
        "ice_test_2",
        UISkillNodeButton::NodeType::Ice,
        Vector2(411, 582),
        [this]() { AttemptUnlockSkill(); },
        [this](const std::string& id) { SelectSkill(id); }
    );

    AddSkillNodeButton(
        "ice_test_3",
        UISkillNodeButton::NodeType::Ice,
        Vector2(466, 582),
        [this]() { AttemptUnlockSkill(); },
        [this](const std::string& id) { SelectSkill(id); }
    );


    // Earth Tree
    AddSkillNodeButton(
        "ground_slam",
        UISkillNodeButton::NodeType::Earth,
        Vector2(550, 877),
        [this]() { AttemptUnlockSkill(); },
        [this](const std::string& id) { SelectSkill(id); }
    );

    AddSkillNodeButton(
        "earth_test_1",
        UISkillNodeButton::NodeType::Earth,
        Vector2(589, 838),
        [this]() { AttemptUnlockSkill(); },
        [this](const std::string& id) { SelectSkill(id); }
    );

    AddSkillNodeButton(
        "heal_amount_earth",
        UISkillNodeButton::NodeType::Neutral,
        Vector2(511, 838),
        [this]() { AttemptUnlockSkill(); },
        [this](const std::string& id) { SelectSkill(id); }
    );

    AddSkillNodeButton(
        "create_pillar",
        UISkillNodeButton::NodeType::Earth,
        Vector2(550, 799),
        [this]() { AttemptUnlockSkill(); },
        [this](const std::string& id) { SelectSkill(id); }
    );

    AddSkillNodeButton(
        "sword_damage_earth",
        UISkillNodeButton::NodeType::Neutral,
        Vector2(589, 760),
        [this]() { AttemptUnlockSkill(); },
        [this](const std::string& id) { SelectSkill(id); }
    );

    AddSkillNodeButton(
        "earth_test_2",
        UISkillNodeButton::NodeType::Earth,
        Vector2(511, 760),
        [this]() { AttemptUnlockSkill(); },
        [this](const std::string& id) { SelectSkill(id); }
    );

    AddSkillNodeButton(
        "earth_test_3",
        UISkillNodeButton::NodeType::Earth,
        Vector2(550, 721),
        [this]() { AttemptUnlockSkill(); },
        [this](const std::string& id) { SelectSkill(id); }
    );

    AddSkillNodeButton(
        "earth_test_4",
        UISkillNodeButton::NodeType::Earth,
        Vector2(550, 666),
        [this]() { AttemptUnlockSkill(); },
        [this](const std::string& id) { SelectSkill(id); }
    );




    AddSkillNodeButton(
        "sword_damage_1",
        UISkillNodeButton::NodeType::Neutral,
        Vector2(603, 529),
        [this]() { AttemptUnlockSkill(); },
        [this](const std::string& id) { SelectSkill(id); }
    );

    AddSkillNodeButton(
        "sword_damage_2",
        UISkillNodeButton::NodeType::Neutral,
        Vector2(497, 529),
        [this]() { AttemptUnlockSkill(); },
        [this](const std::string& id) { SelectSkill(id); }
    );

    AddSkillNodeButton(
        "sword_damage_3",
        UISkillNodeButton::NodeType::Neutral,
        Vector2(497, 635),
        [this]() { AttemptUnlockSkill(); },
        [this](const std::string& id) { SelectSkill(id); }
    );

    AddSkillNodeButton(
        "sword_damage_4",
        UISkillNodeButton::NodeType::Neutral,
        Vector2(603, 635),
        [this]() { AttemptUnlockSkill(); },
        [this](const std::string& id) { SelectSkill(id); }
    );




    AddButton("VOLTAR", Vector2(887, 930), Vector2::Zero, 34, UIButton::TextPos::Center,
    [this]() {
        Close();
    }
    , true);

    RefreshUI();

    // Seleciona uma habilidade padrão ao abrir o menu
    SelectSkill("fire_ball");
}

UISkillNodeButton* SkillMenu::GetNodeButton(const std::string& skillId) {
    for (auto* btn : mButtons) {
        if (auto* nodeBtn = dynamic_cast<UISkillNodeButton*>(btn)) {
            if (nodeBtn->GetSkillId() == skillId) {
                return nodeBtn;
            }
        }
    }
    return nullptr;
}

// Preenche o painel da direita
void SkillMenu::SelectSkill(const std::string& skillId) {
    mSelectedSkillId = skillId;

    UISkillNodeButton::NodeState currentState = UISkillNodeButton::NodeState::Locked;

    // Atualiza o visual de todos os botões da tela
    for (auto* button : mButtons) {
        if (UISkillNodeButton* nodeBtn = dynamic_cast<UISkillNodeButton*>(button)) {
            bool isSelected = (nodeBtn->GetSkillId() == skillId);
            nodeBtn->SetIsSelectedNode(isSelected);

            if (isSelected) {
                currentState = nodeBtn->GetState();
            }
        }
    }

    // Verifica se a UI deve ficar bloqueada
    if (currentState == UISkillNodeButton::NodeState::Locked) {
        mPlayerGoldImage->SetIsVisible(false);
        mPlayerFireStoneImage->SetIsVisible(false);
        mPlayerEarthStoneImage->SetIsVisible(false);
        mPlayerIceStoneImage->SetIsVisible(false);
        mPlayerLightningStoneImage->SetIsVisible(false);

        mPlayerGoldText->SetIsVisible(false);
        mPlayerFireStoneText->SetIsVisible(false);
        mPlayerEarthStoneText->SetIsVisible(false);
        mPlayerIceStoneText->SetIsVisible(false);
        mPlayerLightningStoneText->SetIsVisible(false);

        mTitleText->SetText("BLOQUEADO");
        mTitleText->SetPosition(mTitleTextPosition + mTitleText->GetSize() * 0.5f);

        mTypeText->SetIsVisible(false);
        mDescriptionTitle->SetIsVisible(false);
        mDescriptionText->SetIsVisible(false);
        mHowToUseTitle->SetIsVisible(false);
        mHowToUseText->SetIsVisible(false);
        mUnlockCostTitle->SetIsVisible(false);
        mGoldImage->SetIsVisible(false);
        mGoldCostText->SetIsVisible(false);
        mStoneImage->SetIsVisible(false);
        mStoneCostText->SetIsVisible(false);
        mUnlockText->SetIsVisible(false);

        return;
    }
    else {
        // Torna a UI visível caso estivesse invisível antes
        mPlayerGoldImage->SetIsVisible(true);
        mPlayerFireStoneImage->SetIsVisible(true);
        mPlayerEarthStoneImage->SetIsVisible(true);
        mPlayerIceStoneImage->SetIsVisible(true);
        mPlayerLightningStoneImage->SetIsVisible(true);

        mPlayerGoldText->SetIsVisible(true);
        mPlayerFireStoneText->SetIsVisible(true);
        mPlayerEarthStoneText->SetIsVisible(true);
        mPlayerIceStoneText->SetIsVisible(true);
        mPlayerLightningStoneText->SetIsVisible(true);

        mTypeText->SetIsVisible(true);
        mDescriptionTitle->SetIsVisible(true);
        mDescriptionText->SetIsVisible(true);
        mHowToUseTitle->SetIsVisible(true);
        mHowToUseText->SetIsVisible(true);
        mUnlockCostTitle->SetIsVisible(true);
        mGoldImage->SetIsVisible(true);
        mGoldCostText->SetIsVisible(true);
        mStoneImage->SetIsVisible(true);
        mStoneCostText->SetIsVisible(true);
        mUnlockText->SetIsVisible(true);

        if (currentState == UISkillNodeButton::NodeState::Unlocked) {
            mUnlockText->SetText("DESBLOQUEADO");
            mUnlockText->SetColor(Color::White);
            mUnlockText->SetPosition(mUnlockTextPosition + mUnlockText->GetSize() * 0.5f);
        }
        else if (currentState == UISkillNodeButton::NodeState::Available) {
            // Verifica se tem dinheiro para mudar o texto
            if (mGame->GetSkillTreeManager()->CanUnlock(skillId, mGame->GetPlayer())) {
                mUnlockText->SetText("SEGURE PARA DESBLOQUEAR");
                mUnlockText->SetColor(Color::White);
            } else {
                mUnlockText->SetText("RECURSOS INSUFICIENTES");
                mUnlockText->SetColor(Vector3(1.0f, 0.3f, 0.3f));
            }
            mUnlockText->SetPosition(mUnlockTextPosition + mUnlockText->GetSize() * 0.5f);
        }
    }

    auto* treeManager = mGame->GetSkillTreeManager();
    SkillNode* node = treeManager->GetNode(skillId);

    if (node) {
        mPlayerGoldText->SetText(std::to_string(mGame->GetPlayer()->GetMoney()));
        mPlayerGoldText->SetPosition(mPlayerGoldTextPosition + mPlayerGoldText->GetSize() * 0.5f);

        mPlayerFireStoneText->SetText(std::to_string(mGame->GetPlayer()->GetStone("fireStone")));
        mPlayerFireStoneText->SetPosition(mPlayerFireStoneTextPosition + mPlayerFireStoneText->GetSize() * 0.5f);

        mPlayerEarthStoneText->SetText(std::to_string(mGame->GetPlayer()->GetStone("earthStone")));
        mPlayerEarthStoneText->SetPosition(mPlayerEarthStoneTextPosition + mPlayerEarthStoneText->GetSize() * 0.5f);

        mPlayerIceStoneText->SetText(std::to_string(mGame->GetPlayer()->GetStone("iceStone")));
        mPlayerIceStoneText->SetPosition(mPlayerIceStoneTextPosition + mPlayerIceStoneText->GetSize() * 0.5f);

        mPlayerLightningStoneText->SetText(std::to_string(mGame->GetPlayer()->GetStone("lightningStone")));
        mPlayerLightningStoneText->SetPosition(mPlayerLightningStoneTextPosition + mPlayerLightningStoneText->GetSize() * 0.5f);


        mTitleText->SetText(node->name);
        mTitleText->SetPosition(mTitleTextPosition + mTitleText->GetSize() * 0.5f);

        mTypeText->SetText(node->skillType);
        mTypeText->SetPosition(mTypeTextPosition + mTypeText->GetSize() * 0.5f);

        if (node->skillType == "HABILIDADE DE FOGO") {
            mTypeText->SetColor(Vector3(255 / 255.0f, 101 / 255.0f, 34 / 255.0f));
        } else if (node->skillType == "HABILIDADE DE TERRA") {
            mTypeText->SetColor(Vector3(123 / 255.0f, 59 / 255.0f, 45 / 255.0f));
        } else if (node->skillType == "HABILIDADE DE GELO") {
            mTypeText->SetColor(Vector3(0 / 255.0f, 165 / 255.0f, 255 / 255.0f));
        } else if (node->skillType == "HABILIDADE DE RAIO") {
            mTypeText->SetColor(Vector3(255 / 255.0f, 186 / 255.0f, 0 / 255.0f));
        } else if (node->skillType == "HABILIDADE NEUTRA") {
            mTypeText->SetColor(Vector3(200 / 255.0f, 200 / 255.0f, 200 / 255.0f));
        }

        mDescriptionText->SetText(node->description);
        mDescriptionText->SetPosition(mDescriptionTextPosition + mDescriptionText->GetSize() * 0.5f);

        mHowToUseText->SetText(node->howToUse);
        mHowToUseText->SetPosition(mHowToUseTextPosition + mHowToUseText->GetSize() * 0.5f);

        mGoldCostText->SetText(std::to_string(node->goldCost));
        mGoldCostText->SetPosition(mGoldCostTextPosition + mGoldCostText->GetSize() * 0.5f);

        mStoneCostText->SetText(std::to_string(node->fireStoneCost));
        mStoneCostText->SetPosition(mStoneCostTextPosition + mStoneCostText->GetSize() * 0.5f);
    }
}

void SkillMenu::AttemptUnlockSkill() {
    if (mSelectedSkillId.empty()) return;

    auto* treeManager = mGame->GetSkillTreeManager();
    auto* player = mGame->GetPlayer();

    if (treeManager->CanUnlock(mSelectedSkillId, player)) {
        SkillNode* node = treeManager->GetNode(mSelectedSkillId);

        if (node) {
            // Desconta recursos
            player->SetMoney(player->GetMoney() - node->goldCost);
            // player->SetStone("fireStone", player->GetStone("fireStone") - node->fireStoneCost);

            // Desbloqueia e aplica efeitos
            treeManager->UnlockSkill(mSelectedSkillId);
            player->GetSkillManager()->ApplySkillEffect(node->effectType, node->effectValue);

            SelectSkill(mSelectedSkillId);
            RefreshUI();
        }
    }
}

void SkillMenu::RefreshUI() {
    auto* treeManager = mGame->GetSkillTreeManager();
    auto* player = mGame->GetPlayer();

    for (auto* button : mButtons) {
        if (UISkillNodeButton* nodeBtn = dynamic_cast<UISkillNodeButton*>(button)) {
            SkillNode* nodeData = treeManager->GetNode(nodeBtn->GetSkillId());

            if (nodeData->isUnlocked) {
                nodeBtn->SetState(UISkillNodeButton::NodeState::Unlocked);
            }
            else if (treeManager->ArePrerequisitesMet(nodeBtn->GetSkillId())) {
                nodeBtn->SetState(UISkillNodeButton::NodeState::Available);

                // Só permite segurar/interagir se tiver os recursos
                if (treeManager->CanUnlock(nodeBtn->GetSkillId(), player)) {
                    nodeBtn->SetInteractable(true);
                } else {
                    nodeBtn->SetInteractable(false);
                }
            }
            else {
                nodeBtn->SetState(UISkillNodeButton::NodeState::Locked);
            }
        }
    }
    SelectSkill(mSelectedSkillId);
}

void SkillMenu::Draw(Renderer *renderer) {
    if (!mIsVisible) return;

    // Desenha os fundos e imagens (Atrás de tudo)
    for (UIImage* image : mImages) {
        image->Draw(renderer, mPos);
    }

    // DESENHA AS LINHAS DE CONEXÃO
    auto* treeManager = mGame->GetSkillTreeManager();

    for (auto* btn : mButtons) {
        auto* nodeBtn = dynamic_cast<UISkillNodeButton*>(btn);
        if (!nodeBtn) continue;

        SkillNode* nodeData = treeManager->GetNode(nodeBtn->GetSkillId());
        if (!nodeData) continue;

        Vector2 endPos = mPos + nodeBtn->GetPosition() + (nodeBtn->GetSize() * 0.5f);

        for (const auto& orGroup : nodeData->prerequisites) {
            for (const auto& reqId : orGroup) {

                // Acha o botão do pré-requisito
                UISkillNodeButton* reqBtn = GetNodeButton(reqId);
                SkillNode* reqData = treeManager->GetNode(reqBtn->GetSkillId());

                Vector3 lineColor = Vector3(0.4f, 0.4f, 0.4f);
                if (reqBtn) {
                    Vector2 startPos = mPos + reqBtn->GetPosition() + (reqBtn->GetSize() * 0.5f);

                    if (reqData->isUnlocked) {
                        if (reqData->skillType == "HABILIDADE DE FOGO") {
                            lineColor = Vector3(255 / 255.0f, 101 / 255.0f, 34/ 255.0f);
                        }
                        else if (reqData->skillType == "HABILIDADE DE TERRA") {
                            lineColor = Vector3(123 / 255.0f, 59 / 255.0f, 45 / 255.0f);
                        }
                        else if (reqData->skillType == "HABILIDADE DE GELO") {
                            lineColor = Vector3(0 / 255.0f, 165 / 255.0f, 255 / 255.0f);
                        }
                        else if (reqData->skillType == "HABILIDADE DE RAIO") {
                            lineColor = Vector3(255 / 255.0f, 186 / 255.0f, 0 / 255.0f);
                        }
                        else if (reqData->skillType == "HABILIDADE NEUTRA") {
                            lineColor = Vector3(255 / 255.0f, 255 / 255.0f, 255 / 255.0f);
                        }
                    }

                    renderer->DrawLine(startPos, endPos, lineColor, 3, Vector2::Zero, 1.0f);
                }
            }
        }
    }

    for (UIButton* button : mButtons) {
        button->Draw(renderer, mPos);
    }

    for (UIText* text : mTexts) {
        text->Draw(renderer, mPos);
    }
}

UIButton *SkillMenu::FindNeighbor(UIButton *current, const Vector2 &dir) {
    UIButton* best = nullptr;
    float bestScore = FLT_MAX;

    for (UIButton* b : mButtons) {
        if (b == current) continue;

        // Calcula a distância entre os botões
        Vector2 delta = b->GetPosition() - current->GetPosition();
        float distSq = delta.x * delta.x + delta.y * delta.y;

        if (distSq < 0.0001f) continue;

        float dist = std::sqrt(distSq);

        // Direção normalizada até o botão alvo
        Vector2 deltaNorm = Vector2(delta.x / dist, delta.y / dist);

        // O Produto Escalar (Dot) mede o alinhamento das direções.
        // 1.0 = Perfeitamente alinhado | 0.0 = 90 graus de diferença | -1.0 = Direção oposta
        float dot = (deltaNorm.x * dir.x) + (deltaNorm.y * dir.y);

        // Se o dot for maior que 0.4, o botão está dentro de um cone à frente da direção desejada
        if (dot > 0.4f) {
            float score = dist * (2.0f - dot);

            if (score < bestScore) {
                bestScore = score;
                best = b;
            }
        }
    }

    return best;
}
