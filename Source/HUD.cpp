//
// Created by Lucas N. Ferreira on 08/12/23.
//

#include "HUD.h"
#include "Game.h"
#include "UIElements/UIText.h"

HUD::HUD(class Game* game, const std::string& fontName)
    :UIScreen(game, fontName, false)
    ,mSpeedHPDecrease(200.0f)
    ,mSpeedHPIncrease(200.0f)
    ,mNumOfSubManaBars(mGame->GetPlayer()->GetMaxMana() / 30.0f)
    ,mFireColor(216.0f / 255.0f, 136.0f / 255.0f, 34.0f / 255.0f)
    ,mLightningColor(71.0f / 255.0f, 167.0f / 255.0f, 169.0f / 255.0f)
    ,mIceColor(16.0f / 255.0f, 101.0f / 255.0f, 137.0f / 255.0f)
    ,mEarthColor(141.0f / 255.0f, 82.0f / 255.0f, 68.0f / 255.0f)
    ,mFireIconSize(Vector2(37, 46))
    ,mLightningIconSize(Vector2(36, 46))
    ,mIceIconSize(Vector2(47, 60))
    ,mEarthIconSize(Vector2(49, 62))
    ,mWaitToDecreaseDuration(0.7f)
    ,mWaitToDecreaseTimer(0.0f)
    ,mWaitToDecreaseManaDuration(0.7f)
    ,mWaitToDecreaseManaTimer(0.0f)
    ,mPlayerDie(false)
{
    auto* player = mGame->GetPlayer();
    float HPBarX = 50;
    float HPBarY = 50;
    float HPBarWidth = player->GetMaxHealthPoints() * 5;
    float HPBarHeight = 30;

    float ManaBarX = 50;
    float ManaBarY = 85;
    float ManaBarWidth = player->GetMaxMana() * 2.5;
    float ManaBarHeight = 30;

    float bossHPBarX = mGame->GetRenderer()->GetVirtualWidth() * 0.15f;
    float bossHPBarY = mGame->GetRenderer()->GetVirtualHeight() * 0.95f;
    float bossHPBarWidth = mGame->GetRenderer()->GetVirtualWidth() * 0.7f;
    float bossHPBarHeight = 30;

    mHPBar = {HPBarX, HPBarY,HPBarWidth,HPBarHeight};
    mDamageTakenBar = mHPBar;
    mDamageTakenBar.w = 0;
    mHPRemainingBar = mHPBar;
    mHPGrowingBar = mHPBar;

    mManaBar = {ManaBarX, ManaBarY, ManaBarWidth, ManaBarHeight};
    mManaUsedBar = mManaBar;
    mManaUsedBar.w = 0;
    mManaRemainingBar = mManaBar;

    mBossHPBar = {bossHPBarX, bossHPBarY,bossHPBarWidth,bossHPBarHeight};
    mBossDamageTakenBar = mBossHPBar;
    mBossHPRemainingBar = mBossHPBar;
    mBossHPGrowingBar = mBossHPBar;

    mSlowMotionBarPos = Vector2(75.0f, 190.0f);
    mSlowMotionBarRadius = 28.0f;
    mSlowMotionBarThickness = 7.0f;

    mPlayerHealCount = AddText(std::to_string(player->GetHealCount()),
                                Vector2(65, 138),
                                POINT_SIZE);

    if (player->GetHealCount() == 0) {
        mPotion = AddImage("../Assets/Sprites/Healingpotions/empty.png", Vector2(96, 139), Vector2(32, 32));
    }
    else if (player->GetHealCount() == 1) {
        mPotion = AddImage("../Assets/Sprites/Healingpotions/bemVazia.png", Vector2(96, 139), Vector2(32, 32));
    }
    else if (player->GetHealCount() == 2) {
        mPotion = AddImage("../Assets/Sprites/Healingpotions/meioVazia.png", Vector2(96, 139), Vector2(32, 32));
    }
    else if (player->GetHealCount() == 3 || player->GetHealCount() == 4) {
        mPotion = AddImage("../Assets/Sprites/Healingpotions/cheia.png", Vector2(96, 139), Vector2(32, 32));
    }

    AddImage("../Assets/Sprites/Money/CristalSmall.png", Vector2(1770, 68), Vector2(18.0f, 31.5f));

    mPlayerMoney = AddText(std::to_string(player->GetMoney()),
                                Vector2::Zero,
                                POINT_SIZE);
    mPlayerMoney->SetPosition(Vector2(1790 + mPlayerMoney->GetSize().x / 2, 65));

    Vector2 elementalIconSize(38, 48);
    switch (player->GetElementalMode()) {
        case Player::ElementalMode::Fire:
            mElementalMode = AddImage("../Assets/Sprites/HUD/Fire.png", mSlowMotionBarPos, mFireIconSize);
        break;

        case Player::ElementalMode::Ice:
            mElementalMode = AddImage("../Assets/Sprites/HUD/Ice.png", mSlowMotionBarPos, mIceIconSize);
        break;

        case Player::ElementalMode::Earth:
            mElementalMode = AddImage("../Assets/Sprites/HUD/Earth.png", mSlowMotionBarPos, mEarthIconSize);
        break;

        case Player::ElementalMode::Lightning:
            mElementalMode = AddImage("../Assets/Sprites/HUD/Lightning.png", mSlowMotionBarPos, mLightningIconSize);
        break;

        default:
            mElementalMode = AddImage("../Assets/Sprites/HUD/Fire.png", mSlowMotionBarPos, mFireIconSize);
        break;
    }
}

HUD::~HUD()
{
}

void HUD::Update(float deltaTime) {
    auto* player = mGame->GetPlayer();
    if (!mPlayerDie) {
        float playerHealthPoints = player->GetHealthPoints() / player->GetMaxHealthPoints();
        if (playerHealthPoints < 0) {
            playerHealthPoints = 0;
        }
        mHPRemainingBar.w = mHPBar.w * playerHealthPoints;

        if (mHPGrowingBar.w < mHPRemainingBar.w) {
            mHPGrowingBar.w += mSpeedHPIncrease * deltaTime;
            if (mHPGrowingBar.w > mHPRemainingBar.w) {
                mHPGrowingBar.w = mHPRemainingBar.w;
            }
        }
        else {
            mHPGrowingBar.w = mHPRemainingBar.w;
        }

        float playerMana = player->GetMana() / player->GetMaxMana();
        if (playerMana < 0) {
            playerMana = 0;
        }
        mManaRemainingBar.w = mManaBar.w * playerMana;

        std::string playerHealCount = std::to_string(player->GetHealCount());
        mPlayerHealCount->SetText(playerHealCount);

        if (player->GetHealCount() == 0) {
            mPotion->SetImage("../Assets/Sprites/Healingpotions/empty.png");
        }
        else if (player->GetHealCount() == 1) {
            mPotion->SetImage("../Assets/Sprites/Healingpotions/bemVazia.png");
        }
        else if (player->GetHealCount() == 2) {
            mPotion->SetImage("../Assets/Sprites/Healingpotions/meioVazia.png");
        }
        else if (player->GetHealCount() == 3 || player->GetHealCount() == 4) {
            mPotion->SetImage("../Assets/Sprites/Healingpotions/cheia.png");
        }

        switch (player->GetElementalMode()) {
            case Player::ElementalMode::Fire:
                mElementalMode->SetImage("../Assets/Sprites/HUD/Fire.png");
                mElementalMode->SetSize(mFireIconSize);
            break;

            case Player::ElementalMode::Ice:
                mElementalMode->SetImage("../Assets/Sprites/HUD/Ice.png");
                mElementalMode->SetSize(mIceIconSize);
            break;

            case Player::ElementalMode::Earth:
                mElementalMode->SetImage("../Assets/Sprites/HUD/Earth.png");
                mElementalMode->SetSize(mEarthIconSize);
            break;

            case Player::ElementalMode::Lightning:
                mElementalMode->SetImage("../Assets/Sprites/HUD/Lightning.png");
                mElementalMode->SetSize(mLightningIconSize);
            break;

            default:
                mElementalMode->SetImage("../Assets/Sprites/HUD/Fire.png");
                mElementalMode->SetSize(mFireIconSize);
            break;
        }

        std::string playerMoney = std::to_string(player->GetMoney());
        mPlayerMoney->SetText(playerMoney);
        mPlayerMoney->SetPosition(Vector2(1790 + mPlayerMoney->GetSize().x / 2, 65));
    }

    if (mDamageTakenBar.w > mHPGrowingBar.w) {
        mWaitToDecreaseTimer += deltaTime;
        if (mWaitToDecreaseTimer >= mWaitToDecreaseDuration) {
            mDamageTakenBar.w -= mSpeedHPDecrease * deltaTime;
        }
    }
    else {
        mDamageTakenBar.w = mHPGrowingBar.w;
        mWaitToDecreaseTimer = 0;
    }

    if (mManaUsedBar.w > mManaRemainingBar.w) {
        mWaitToDecreaseManaTimer += deltaTime;
        if (mWaitToDecreaseManaTimer >= mWaitToDecreaseManaDuration) {
            mManaUsedBar.w -= mSpeedHPDecrease * deltaTime;
        }
    }
    else {
        mManaUsedBar.w = mManaRemainingBar.w;
        mWaitToDecreaseManaTimer = 0;
    }

    if (player->Died()) {
        mPlayerDie = true;
    }

    // Boss HP bars
    for (int i = 0; i < mBossLifeBars.size(); i++) {
        // deslocamento das barras de boss
        mBossLifeBars[i].bossHPBar.y = mBossHPBar.y - 50 * i;
        mBossLifeBars[i].bossDamageTakenBar.y = mBossHPBar.y - 50 * i;
        mBossLifeBars[i].bossHPRemainingBar.y = mBossHPBar.y - 50 * i;
        mBossLifeBars[i].bossHPGrowingBar.y = mBossHPBar.y - 50 * i;

        float bossHealthPoints = mBossLifeBars[i].boss->GetHealthPoints() / mBossLifeBars[i].boss->GetMaxHealthPoints();
        if (bossHealthPoints < 0) {
            bossHealthPoints = 0;
        }
        mBossLifeBars[i].bossHPRemainingBar.w = mBossLifeBars[i].bossHPBar.w * bossHealthPoints;

        if (mBossLifeBars[i].bossHPGrowingBar.w < mBossLifeBars[i].bossHPRemainingBar.w) {
            mBossLifeBars[i].bossHPGrowingBar.w += mSpeedHPIncrease * deltaTime;
            if (mBossLifeBars[i].bossHPGrowingBar.w > mBossLifeBars[i].bossHPRemainingBar.w) {
                mBossLifeBars[i].bossHPGrowingBar.w = mBossLifeBars[i].bossHPRemainingBar.w;
            }
        }
        else {
            mBossLifeBars[i].bossHPGrowingBar.w = mBossLifeBars[i].bossHPRemainingBar.w;
        }

        if (mBossLifeBars[i].bossDamageTakenBar.w > mBossLifeBars[i].bossHPGrowingBar.w) {
            mBossLifeBars[i].waitToDecreaseTimer += deltaTime;
            if (mBossLifeBars[i].waitToDecreaseTimer >= mWaitToDecreaseDuration) {
                mBossLifeBars[i].bossDamageTakenBar.w -= mSpeedHPDecrease * deltaTime;
            }
        }
        else {
            mBossLifeBars[i].bossDamageTakenBar.w = mBossLifeBars[i].bossHPGrowingBar.w;
            mBossLifeBars[i].waitToDecreaseTimer = 0;
        }

        if (mBossLifeBars[i].boss && mBossLifeBars[i].boss->GetState() == ActorState::Destroy) {
            EndBossFight(mBossLifeBars[i].boss);
            i--;
        }
    }
}

void HUD::StartBossFight(class Enemy *boss) {
    for (auto it = mBossLifeBars.begin(); it != mBossLifeBars.end(); ) {
        if (it->boss == boss) {
            return;
        }
        else {
            ++it;
        }
    }

    BossLifeBar bossLifeBar;
    bossLifeBar.bossHPBar = mBossHPBar;
    bossLifeBar.bossDamageTakenBar = mBossDamageTakenBar;
    bossLifeBar.bossHPRemainingBar = mBossHPRemainingBar;
    bossLifeBar.bossHPGrowingBar = mBossHPGrowingBar;
    bossLifeBar.boss = boss;
    bossLifeBar.waitToDecreaseTimer = 0.0f;

    mBossLifeBars.emplace_back(bossLifeBar);
}

void HUD::EndBossFight(class Enemy* boss) {
    for (auto it = mBossLifeBars.begin(); it != mBossLifeBars.end(); ) {
        if (it->boss == boss) {
            it = mBossLifeBars.erase(it);
            break;
        } else {
            // Só avança se não tiver apagado nada
            ++it;
        }
    }
}


void HUD::IncreaseHPBar() {
    mHPBar.w = mGame->GetPlayer()->GetMaxHealthPoints() * 5;
}

void HUD::IncreaseManaBar() {
    mManaBar.w = mGame->GetPlayer()->GetMaxMana() * 2.5;
    mNumOfSubManaBars = mGame->GetPlayer()->GetMaxMana() / 30.0f;
}

void HUD::Draw(Renderer *renderer) {
    if (!mIsVisible) {
        return;
    }
    DrawSlowMotionBar(renderer);

    for (UIImage* image : mImages) {
        image->Draw(renderer, mPos);
    }

    for (UIButton* button : mButtons) {
        button->Draw(renderer, mPos);
    }

    for (UIText* text : mTexts) {
        text->Draw(renderer, mPos);
    }

    DrawLifeBar(renderer);
    DrawManaBar(renderer);
    if (!mBossLifeBars.empty()) {
        DrawBossLifeBar(renderer);
    }
}

void HUD::DrawLifeBar(Renderer *renderer) {
    SDL_Rect HPBar;
    HPBar.x = static_cast<int>(mHPBar.x);
    HPBar.y = static_cast<int>(mHPBar.y);
    HPBar.w = static_cast<int>(mHPBar.w);
    HPBar.h = static_cast<int>(mHPBar.h);

    SDL_Rect DamageTakenBar;
    DamageTakenBar.x = static_cast<int>(mDamageTakenBar.x);
    DamageTakenBar.y = static_cast<int>(mDamageTakenBar.y);
    DamageTakenBar.w = static_cast<int>(mDamageTakenBar.w);
    DamageTakenBar.h = static_cast<int>(mDamageTakenBar.h);

    SDL_Rect HPRemainingBar;
    HPRemainingBar.x = static_cast<int>(mHPRemainingBar.x);
    HPRemainingBar.y = static_cast<int>(mHPRemainingBar.y);
    HPRemainingBar.w = static_cast<int>(mHPRemainingBar.w);
    HPRemainingBar.h = static_cast<int>(mHPRemainingBar.h);

    SDL_Rect HPGrowingBar;
    HPGrowingBar.x = static_cast<int>(mHPGrowingBar.x);
    HPGrowingBar.y = static_cast<int>(mHPGrowingBar.y);
    HPGrowingBar.w = static_cast<int>(mHPGrowingBar.w);
    HPGrowingBar.h = static_cast<int>(mHPGrowingBar.h);

    renderer->DrawRect(Vector2(HPBar.x, HPBar.y) + Vector2(HPBar.w, HPBar.h) / 2, Vector2(HPBar.w, HPBar.h), 0.0f,
                         Vector3(40 / 255.0f, 40 / 255.0f, 40 / 255.0f), Vector2::Zero, RendererMode::TRIANGLES, 150 / 255.0f);

    renderer->DrawRect(Vector2(DamageTakenBar.x, DamageTakenBar.y) + Vector2(DamageTakenBar.w, DamageTakenBar.h) / 2, Vector2(DamageTakenBar.w, DamageTakenBar.h), 0.0f,
                     Vector3(240 / 255.0f, 234 / 255.0f, 95 / 255.0f), Vector2::Zero, RendererMode::TRIANGLES, 255 / 255.0f);

    renderer->DrawRect(Vector2(HPRemainingBar.x, HPRemainingBar.y) + Vector2(HPRemainingBar.w, HPRemainingBar.h) / 2, Vector2(HPRemainingBar.w, HPRemainingBar.h), 0.0f,
                     Vector3(242 / 255.0f, 121 / 255.0f, 123 / 255.0f), Vector2::Zero, RendererMode::TRIANGLES, 100 / 255.0f);

    renderer->DrawRect(Vector2(HPGrowingBar.x, HPGrowingBar.y) + Vector2(HPGrowingBar.w, HPGrowingBar.h) / 2, Vector2(HPGrowingBar.w, HPGrowingBar.h), 0.0f,
                     Vector3(242 / 255.0f, 90 / 255.0f, 70 / 255.0f), Vector2::Zero, RendererMode::TRIANGLES, 255 / 255.0f);

}

void HUD::DrawManaBar(Renderer *renderer) {
    SDL_Rect ManaBar;
    ManaBar.x = static_cast<int>(mManaBar.x);
    ManaBar.y = static_cast<int>(mManaBar.y);
    ManaBar.w = static_cast<int>(mManaBar.w);
    ManaBar.h = static_cast<int>(mManaBar.h);

    SDL_Rect ManaUsedBar;
    ManaUsedBar.x = static_cast<int>(mManaUsedBar.x);
    ManaUsedBar.y = static_cast<int>(mManaUsedBar.y);
    ManaUsedBar.w = static_cast<int>(mManaUsedBar.w);
    ManaUsedBar.h = static_cast<int>(mManaUsedBar.h);

    SDL_Rect ManaRemainingBar;
    ManaRemainingBar.x = static_cast<int>(mManaRemainingBar.x);
    ManaRemainingBar.y = static_cast<int>(mManaRemainingBar.y);
    ManaRemainingBar.w = static_cast<int>(mManaRemainingBar.w);
    ManaRemainingBar.h = static_cast<int>(mManaRemainingBar.h);

    renderer->DrawRect(Vector2(ManaBar.x, ManaBar.y) + Vector2(ManaBar.w, ManaBar.h) / 2, Vector2(ManaBar.w, ManaBar.h), 0.0f,
                 Vector3(40 / 255.0f, 40 / 255.0f, 40 / 255.0f), Vector2::Zero, RendererMode::TRIANGLES, 150 / 255.0f);

    renderer->DrawRect(Vector2(ManaUsedBar.x, ManaUsedBar.y) + Vector2(ManaUsedBar.w, ManaUsedBar.h) / 2, Vector2(ManaUsedBar.w, ManaUsedBar.h), 0.0f,
             Vector3(240 / 255.0f, 234 / 255.0f, 95 / 255.0f), Vector2::Zero, RendererMode::TRIANGLES, 255 / 255.0f);

    renderer->DrawRect(Vector2(ManaRemainingBar.x, ManaRemainingBar.y) + Vector2(ManaRemainingBar.w, ManaRemainingBar.h) / 2, Vector2(ManaRemainingBar.w, ManaRemainingBar.h), 0.0f,
             Vector3(65 / 255.0f, 188 / 255.0f, 217 / 255.0f), Vector2::Zero, RendererMode::TRIANGLES, 255 / 255.0f);

    for (int i = 1; i < mNumOfSubManaBars; i++) {
        renderer->DrawLine(Vector2(mManaBar.x + i * mManaBar.w / mNumOfSubManaBars, mManaBar.y),
                           Vector2(mManaBar.x + i * mManaBar.w / mNumOfSubManaBars, mManaBar.y + mManaBar.h),
                           Vector3(0, 0, 0), 3.0f, Vector2::Zero, 1.0f);
    }
}

void HUD::DrawBossLifeBar(Renderer *renderer) {
    for (int i = 0; i < mBossLifeBars.size(); i++) {
        SDL_Rect HPBar;
        HPBar.x = static_cast<int>(mBossLifeBars[i].bossHPBar.x);
        HPBar.y = static_cast<int>(mBossLifeBars[i].bossHPBar.y);
        HPBar.w = static_cast<int>(mBossLifeBars[i].bossHPBar.w);
        HPBar.h = static_cast<int>(mBossLifeBars[i].bossHPBar.h);

        SDL_Rect DamageTakenBar;
        DamageTakenBar.x = static_cast<int>(mBossLifeBars[i].bossDamageTakenBar.x);
        DamageTakenBar.y = static_cast<int>(mBossLifeBars[i].bossDamageTakenBar.y);
        DamageTakenBar.w = static_cast<int>(mBossLifeBars[i].bossDamageTakenBar.w);
        DamageTakenBar.h = static_cast<int>(mBossLifeBars[i].bossDamageTakenBar.h);

        SDL_Rect HPRemainingBar;
        HPRemainingBar.x = static_cast<int>(mBossLifeBars[i].bossHPRemainingBar.x);
        HPRemainingBar.y = static_cast<int>(mBossLifeBars[i].bossHPRemainingBar.y);
        HPRemainingBar.w = static_cast<int>(mBossLifeBars[i].bossHPRemainingBar.w);
        HPRemainingBar.h = static_cast<int>(mBossLifeBars[i].bossHPRemainingBar.h);

        SDL_Rect HPGrowingBar;
        HPGrowingBar.x = static_cast<int>(mBossLifeBars[i].bossHPGrowingBar.x);
        HPGrowingBar.y = static_cast<int>(mBossLifeBars[i].bossHPGrowingBar.y);
        HPGrowingBar.w = static_cast<int>(mBossLifeBars[i].bossHPGrowingBar.w);
        HPGrowingBar.h = static_cast<int>(mBossLifeBars[i].bossHPGrowingBar.h);

        renderer->DrawRect(Vector2(HPBar.x, HPBar.y) + Vector2(HPBar.w, HPBar.h) / 2, Vector2(HPBar.w, HPBar.h), 0.0f,
                         Vector3(40 / 255.0f, 40 / 255.0f, 40 / 255.0f), Vector2::Zero, RendererMode::TRIANGLES, 150 / 255.0f);

        renderer->DrawRect(Vector2(DamageTakenBar.x, DamageTakenBar.y) + Vector2(DamageTakenBar.w, DamageTakenBar.h) / 2, Vector2(DamageTakenBar.w, DamageTakenBar.h), 0.0f,
                     Vector3(240 / 255.0f, 234 / 255.0f, 95 / 255.0f), Vector2::Zero, RendererMode::TRIANGLES, 255 / 255.0f);

        renderer->DrawRect(Vector2(HPRemainingBar.x, HPRemainingBar.y) + Vector2(HPRemainingBar.w, HPRemainingBar.h) / 2, Vector2(HPRemainingBar.w, HPRemainingBar.h), 0.0f,
                     Vector3(242 / 255.0f, 121 / 255.0f, 123 / 255.0f), Vector2::Zero, RendererMode::TRIANGLES, 100 / 255.0f);

        renderer->DrawRect(Vector2(HPGrowingBar.x, HPGrowingBar.y) + Vector2(HPGrowingBar.w, HPGrowingBar.h) / 2, Vector2(HPGrowingBar.w, HPGrowingBar.h), 0.0f,
                 Vector3(242 / 255.0f, 90 / 255.0f, 70 / 255.0f), Vector2::Zero, RendererMode::TRIANGLES, 255 / 255.0f);
    }
}

void HUD::DrawSlowMotionBar(Renderer* renderer) {
    auto* player = mGame->GetPlayer();
    float timer = player->GetRadialMenuSlowMotionTimer();
    float duration = player->GetRadialMenuSlowMotionDuration();
    bool isCharging = player->IsSlowMotionCharging();

    if (duration <= 0.0f) return;

    // Calcula a porcentagem de preenchimento (0.0f a 1.0f)
    float ratio = timer / duration;
    ratio = std::clamp(ratio, 0.0f, 1.0f);

    // Configurações visuais do círculo
    int numSegments = 128;
    float angleStep = Math::TwoPi / numSegments;

    // Cores
    Vector3 bgColor = Vector3(40 / 255.0f, 40 / 255.0f, 40 / 255.0f); // Fundo escuro

    // LÓGICA DE CORES E OPACIDADE
    Vector3 fillColor;
    float alpha = 1.0f;

    switch (player->GetElementalMode()) {
        case Player::ElementalMode::Fire:
            fillColor = mFireColor;
        break;

        case Player::ElementalMode::Lightning:
            fillColor = mLightningColor;
        break;

        case Player::ElementalMode::Ice:
            fillColor = mIceColor;
        break;

        case Player::ElementalMode::Earth:
            fillColor = mEarthColor;
        break;
    }

    if (isCharging) {
        // Faz a opacidade pulsar usando o tempo do jogo ou do sistema
        float time = SDL_GetTicks() / 60.0f;
        alpha = 0.5f + (std::sin(time) * 0.5f); // Varia suavemente entre 0.0 e 1.0

        // Evita que fique 100% invisível
        if (alpha < 0.2f) {
            alpha = 0.2f;
        }
    }
    else {
        alpha = 1.0f; // Opacidade máxima
    }

    renderer->DrawCircularBar(mSlowMotionBarPos, mSlowMotionBarRadius, mSlowMotionBarThickness, ratio, fillColor, bgColor, alpha);
}
