#include <algorithm>
#include <vector>
#include "Game.h"
#include "IconDictionary.h"
#include "Components/Drawing/DrawComponent.h"
#include "Components/RigidBodyComponent.h"
#include "Random.h"
#include "Actors/ParticleSystem.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include "CSV.h"
#include "Json.h"
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include "HUD.h"
#include "MapManager.h"
#include "SaveData.h"
#include "SaveManager.h"
#include "Actors/AmbientParticleArea.h"
#include "Actors/Brazier.h"
#include "Actors/Enemies/BushMonster.h"
#include "Actors/Checkpoint.h"
#include "Actors/Enemies/CloneEnemy.h"
#include "Actors/Decorations.h"
#include "Actors/Enemies/DragonFly.h"
#include "UIElements/UIScreen.h"
#include "Actors/DynamicGround.h"
#include "Actors/Enemies/Fox.h"
#include "Actors/Enemies/Frog.h"
#include "Actors/Lever.h"
#include "Actors/Trigger.h"
#include "Actors/Fairy.h"
#include "Actors/Enemies/EnemySimple.h"
#include "Actors/Enemies/FlyingEnemySimple.h"
#include "Actors/Enemies/FlyingGolem.h"
#include "Actors/Enemies/FlyingShooterEnemy.h"
#include "Actors/Enemies/FlyingSpawnerEnemy.h"
#include "Actors/Enemies/Golem.h"
#include "Actors/Enemies/HookEnemy.h"
#include "Actors/HookPoint.h"
#include "Actors/LaserShooter.h"
#include "Actors/Lava.h"
#include "Actors/Light.h"
#include "Actors/LightningBarrier.h"
#include "Actors/Enemies/LittleBat.h"
#include "Actors/Enemies/Mantis.h"
#include "Actors/Enemies/MirrorBoss.h"
#include "Actors/Money.h"
#include "Actors/Enemies/Moth.h"
#include "Actors/Enemies/Mushroom.h"
#include "Actors/Enemies/OrangeSlime.h"
#include "Actors/Projectile.h"
#include "Actors/Enemies/Bat.h"
#include "Actors/Enemies/Frogger.h"
#include "Actors/Enemies/Snake.h"
#include "Actors/Enemies/StoneGolem.h"
#include "Components/AABBComponent.h"
#include "Components/CombatBoxComponent.h"
#include "Components/DashComponent.h"
#include "Components/Drawing/AnimatorComponent.h"
#include "Components/Drawing/DrawRopeComponent.h"
#include "Components/Drawing/GhostTrailComponent.h"
#include "Components/Drawing/RectComponent.h"
#include "UIScreens/MainMenu.h"
#include "UIScreens/MapMenu.h"
#include "UIScreens/PauseAdvancedMenu.h"
#include "UIScreens/PauseMenu.h"
#include "UIScreens/ThankYouScreen.h"

std::vector<int> ParseIntList(const std::string& str) {
    std::vector<int> result;
    std::stringstream ss(str);
    std::string item;

    while (std::getline(ss, item, ',')) {
        if (!item.empty()) {
            result.push_back(std::stoi(item));
        }
    }

    return result;
}

Game::Game(int windowWidth, int windowHeight, int FPS)
    :mBackToCheckpoint(false)
    ,mWindow(nullptr)
    ,mRenderer(nullptr)
    ,mWindowWidth(windowWidth)
    ,mWindowHeight(windowHeight)
    ,mLogicalWindowWidth(windowWidth)
    ,mLogicalWindowHeight(windowHeight)
    ,mTicksCount(0)
    ,mIsRunning(true)
    ,mUpdatingActors(false)
    ,mFPS(FPS)
    ,mIsPaused(false)
    ,mIsCrossFading(false)
    ,mCrossFadeDuration(0.0f)
    ,mCrossFadeTimer(0.0f)
    ,mZoom(1.0f)
    ,mTargetZoom(1.0f)
    ,mZoomSpeed(1.0f)
    ,mCamera(nullptr)
    ,mPlayer(nullptr)
    ,mLevelData(nullptr)
    ,mLevelDataDynamicGrounds(nullptr)
    ,mGroundBehindPlayer(true)
    ,mUseGroundPadding(false)
    ,mUseGrassParticle(false)
    ,mGroundParticleColor({58, 147, 89, 255})
    ,mIsConnectingMapRoom(false)
    ,mBrushRadius(500.0f)
    ,mTileSheet(nullptr)
    ,mDecorationsTileSheet(nullptr)
    ,mCurrentController(nullptr)
    ,mMapManager(nullptr)
    ,mLastMapOriginCanvasPos(0, 0)
    ,mLastMapOriginBoundsMin(0, 0)
    ,mLastMapOriginBoundsMax(0, 0)
    ,mLastMapOriginTriggerPos(0, 0)
    ,mHitstopActive(false)
    ,mHitstopDuration(0.15f)
    ,mHitstopTimer(0.0f)
    ,mHitstopDelayActive(false)
    ,mHitstopDelayDuration(0.01f)
    ,mHitstopDelayTimer(0.0f)
    ,mDamageFlashActive(false)
    ,mDamageFlashDuration(0.6f)
    ,mDamageFlashTimer(0.0f)
    ,mIsSlowMotion(false)
    ,mIsAccelerated(false)
    ,mSaveSlot(0)
    ,mSaveData(nullptr)
    ,mSaveManager(nullptr)
    ,mPlayerDeathCounter(0)
    ,mCheckpointPosition(Vector2::Zero)
    ,mCheckpointGameScenePath("Room2/Room2")
    ,mLavaRespawnPosition(Vector2::Zero)
    ,mHitByLava(false)
    ,mPlayerStartPositionId(0)
    ,mCheckPointMoney(0)
    ,mGoingToNextLevel(false)
    ,mIsPlayingAdvancedMode(false)
    ,mInputPlayerMode(InputPlayerMode::Keyboard)
    ,mLeftStickStateY(StickState::Neutral)
    ,mLeftStickStateX(StickState::Neutral)
    ,mRightStickStateY(StickState::Neutral)
    ,mRightStickStateX(StickState::Neutral)
    ,mRawRightAxisX(0.0f)
    ,mRawRightAxisY(0.0f)
    ,mPauseMusicVolumeScale(0.45f)
    ,mWaveManager(nullptr)
    ,mNewButtonText(nullptr)
    ,mWaitingForKey(false)
    ,mWaitingForButton(false)
    ,mIsPlayingFinalCutscene(false)
    ,mCurrentCutscene(nullptr)
    ,mBackGroundTexture(nullptr)
    ,mUseParallaxBackground(false)
    ,mAudio(nullptr)
    ,mHUD(nullptr)
    ,mLastTopUIScreen(nullptr)
    ,mPauseMenu(nullptr)
    ,mMapMenu(nullptr)
    ,mLevelSelectMenu(nullptr)
    ,mSceneManagerState(SceneManagerState::None)
    ,mFadeDuration(0.4f)
    ,mSceneManagerTimer(0.0f)
    ,mFadeAlpha(0)
    ,mGameScene(GameScene::MainMenu)
    ,mNextScene(GameScene::MainMenu)
    ,mCurrentLevelPath("MainMenu")
    ,mNextLevelPath("MainMenu")
{
}

bool Game::Initialize()
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) != 0)
    {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return false;
    }

    // Init SDL Image
    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags))
    {
        SDL_Log("Unable to initialize SDL_image: %s", IMG_GetError());
        return false;
    }

    mWindow = SDL_CreateWindow("Echoes of Elementum", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                               mWindowWidth, mWindowHeight,
                               // SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
                               SDL_WINDOW_RESIZABLE | SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_OPENGL);

    if (!mWindow) {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        return false;
    }

    mRenderer = new Renderer(mWindow, this);
    mRenderer->Initialize(mWindowWidth, mWindowHeight);

    // Initialize SDL_ttf
    if (TTF_Init() != 0)
    {
        SDL_Log("Failed to initialize SDL_ttf");
        return false;
    }

    // Initialize SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1) {
        SDL_Log("Failed to initialize SDL_mixer");
        return false;
    }

    // Informe o renderer sobre a mudança
    if (mRenderer) {
        mRenderer->OnWindowResize(mWindowWidth, mWindowHeight);
    }

    mLogicalWindowWidth = mRenderer->GetVirtualWidth();
    mLogicalWindowHeight = mRenderer->GetVirtualHeight();

    LoadBindingsFromFile("../Assets/InputBindings/InputBindings.json");

    Random::Init();

    // Load Audios
    mAudio = new AudioSystem(16);
    mAudio->CacheSound("Hornet.wav");
    mAudio->CacheSound("MantisLords.wav");
    mAudio->CacheSound("HollowKnight.wav");

    // Load Final cutscenes
    mGoodCutscenes = {"ShowLevel2", "ShowLevel3", "ShowLevel4"};
    // mGoodCutsceneScenes = {GameScene::Level2, GameScene::Level3, GameScene::Level4};
    mEvilCutscenes = {};
    mCutsceneIndex = 0;

    mTicksCount = SDL_GetTicks();

    LoadNextLevel("MainMenu");

    mSkillTreeManager = new SkillTreeManager();

    mSaveData = new SaveData(this);
    mSaveManager = new SaveManager(this);

    return true;
}

void Game::LoadNextLevel(const std::string &levelPath, float transitionTime) {
    if (mSceneManagerState == SceneManagerState::None) {
        mNextLevelPath = levelPath;
        if (levelPath == "MainMenu" || levelPath == "ThankYouScreen") {
            mNextScene = GameScene::MainMenu;
        }
        else {
            mNextScene = GameScene::Gameplay;
        }
        mSceneManagerState = SceneManagerState::Entering;
        mSceneManagerTimer = transitionTime;
    }
}

void Game::ChangeScene()
{
    mZoom = 1.0f;
    mTargetZoom = 1.0f;
    if (mCamera) {
        mCamera->SetZoom(mZoom);
    }

    // Unload current Scene
    UnloadScene();

    mRenderer->SetAmbientLight(Vector3(1, 1, 1), 0.8f);

    mIsSlowMotion = false;
    mIsAccelerated = false;

    if (mGamePlayState != GamePlayState::Cutscene) {
        if (mGameScene == GameScene::MainMenu) {
            mAudio->StopSound(mMusicHandle);
        }
        // mAudio->StopAllSounds();
        // mAudio->StopSound(mMusicHandle);
        mAudio->StopSound(mBossMusic);
    }

    const std::string backgroundAssets = "../Assets/Sprites/Background/";
    const std::string levelsAssets = "../Assets/Levels/";

    if (mNextScene != GameScene::MainMenu) {
        // Pool de Fireballs
        for (int i = 0; i < 30; i++) {
            new FireBall(this);
        }

        // Pool de Partículas
        for (int i = 0; i < 150; i++) {
            new Particle(this, Particle::ParticleType::SolidParticle);
        }
        for (int i = 0; i < 600; i++) {
            new Particle(this, Particle::ParticleType::BlurParticle);
        }
        for (int i = 0; i < 400; i++) {
            new Particle(this, Particle::ParticleType::Grass);
        }

        // Pool de Projectiles
        for (int i = 0; i < 50; i++) {
            new Projectile(this, Projectile::ProjectileType::Acid);
        }
        for (int i = 0; i < 50; i++) {
            new Projectile(this, Projectile::ProjectileType::OrangeBall);
        }

        // Pool de Moneys
        for (int i = 0; i < 50; i++) {
            new Money(this, Money::MoneyType::Small);
        }
        for (int i = 0; i < 50; i++) {
            new Money(this, Money::MoneyType::Medium);
        }
        for (int i = 0; i < 50; i++) {
            new Money(this, Money::MoneyType::Large);
        }

        // carrega companheiro só no prólogo
        if (mNextLevelPath == "0-Prologue/Prologue") {
            auto* fairy = new Fairy(this);
        }

        // Volta player
        if (mPlayer) {
            if (mPlayer->GetComponent<AnimatorComponent>()) {
                mPlayer->GetComponent<AnimatorComponent>()->SetVisible(true);
            }
            if (mPlayer->GetComponent<RectComponent>()) {
                mPlayer->GetComponent<RectComponent>()->SetVisible(true);
            }
            if (mPlayer->GetComponent<GhostTrailComponent>()) {
                mPlayer->GetComponent<GhostTrailComponent>()->SetVisible(true);
            }
        }

        mAudio->SetCategoryModifier(SoundCategory::Music, 1.0f);

        // Guarda último level que o player estava
        mIsPaused = false;
        mRenderer->DeactivateAllEffects();
    }
    else {
        // Se está no menu, pausa draw de player
        if (mPlayer) {
            if (mPlayer->GetComponent<AnimatorComponent>()) {
                mPlayer->GetComponent<AnimatorComponent>()->SetVisible(false);
            }
            if (mPlayer->GetComponent<RectComponent>()) {
                mPlayer->GetComponent<RectComponent>()->SetVisible(false);
            }
            if (mPlayer->GetComponent<DrawRopeComponent>()) {
                mPlayer->GetComponent<DrawRopeComponent>()->SetVisible(false);
            }
            if (mPlayer->GetComponent<GhostTrailComponent>()) {
                mPlayer->GetComponent<GhostTrailComponent>()->SetVisible(false);
            }
        }
        mAudio->SetCategoryModifier(SoundCategory::Music, 1.0f);
    }

    // Reset gameplay state
    if (!mIsPlayingFinalCutscene) {
        mGamePlayState = GamePlayState::Playing;
    }

    // Scene Manager FSM: using if/else instead of switch
    if (mNextScene == GameScene::MainMenu) {
        if (mNextLevelPath == "MainMenu") {
            mUseParallaxBackground = false;
            mGamePlayState = GamePlayState::Menu;
            mCutsceneIndex = 0;
            mBackGroundTexture = mRenderer->GetTexture(backgroundAssets + "Menu6.png");

            std::string musicFile = "HollowKnight.wav";

            // Initialize main menu actors
            auto* background = new UIScreen(this, "../Assets/Fonts/K2D-Bold.ttf", false);
            background->AddImage("../Assets/Sprites/Background/Menu6.png", Vector2(mOriginalWindowWidth, mOriginalWindowHeight) * 0.5f, Vector2(mOriginalWindowWidth, mOriginalWindowHeight));
            new MainMenu(this, "../Assets/Fonts/K2D-Bold.ttf", false);

            if (mAudio->GetSoundState(mMusicHandle) != SoundState::Playing || mCurrentMusic != musicFile) {
                mAudio->StopSound(mMusicHandle);
                mMusicHandle = mAudio->PlaySound(musicFile.c_str(), true, SoundCategory::Music);
                mCurrentMusic = musicFile;
            }
            mBossMusic.Reset();
        }
        else if (mNextLevelPath == "ThankYouScreen") {
            mUseParallaxBackground = false;
            mGamePlayState = GamePlayState::Menu;
            mCutsceneIndex = 0;
            mBackGroundTexture = mRenderer->GetTexture(backgroundAssets + "Menu6.png");

            std::string musicFile = "HollowKnight.wav";

            // Initialize main menu actors
            new ThankYouScreen(this, "../Assets/Fonts/K2D-Bold.ttf", false);

            if (mAudio->GetSoundState(mMusicHandle) != SoundState::Playing || mCurrentMusic != musicFile) {
                mAudio->StopSound(mMusicHandle);
                mMusicHandle = mAudio->PlaySound(musicFile.c_str(), true, SoundCategory::Music);
                mCurrentMusic = musicFile;
            }
            mBossMusic.Reset();
        }
    }
    else if (mNextScene == GameScene::Gameplay) {
        // Carrega o JSON da fase para extrair as propriedades ANTES de instanciar os objetos
        std::ifstream file(levelsAssets + mNextLevelPath + ".json");
        if (file.is_open()) {
            nlohmann::json mapData;
            file >> mapData;

            // Valores Padrão
            mGroundBehindPlayer = true;
            mUseGroundPadding = false;
            mUseGrassParticle = false;
            mGroundParticleColor = {58, 147, 89, 255};
            mUseParallaxBackground = true;
            std::string musicFile = "Greenpath.wav";

            if (mapData.contains("properties")) {
                for (const auto& prop : mapData["properties"]) {
                    std::string propName = prop["name"];

                    if (propName == "GroundBehindPlayer") mGroundBehindPlayer = prop["value"];
                    else if (propName == "UseGroundPadding") mUseGroundPadding = prop["value"];
                    else if (propName == "UseGrassParticle") mUseGrassParticle = prop["value"];
                    else if (propName == "GroundParticleColor") mGroundParticleColor = HexToColor(prop["value"]);
                    else if (propName == "Music") musicFile = prop["value"];
                    else if (propName == "BackgroundLayers") {
                        std::string bgPaths = prop["value"];
                        std::stringstream ss(bgPaths);
                        std::string item;
                        while (std::getline(ss, item, ',')) {
                            mBackgroundLayers.emplace_back(mRenderer->GetTexture("../Assets/Sprites/Background/" + item));
                        }
                    }
                }
            }

            // Toca a música se for diferente da atual
            if (mAudio->GetSoundState(mMusicHandle) != SoundState::Playing || mCurrentMusic != musicFile) {
                mAudio->StopSound(mMusicHandle);
                mMusicHandle = mAudio->PlaySound(musicFile.c_str(), true, SoundCategory::Music);
                mCurrentMusic = musicFile;
            }

            LoadLevel(levelsAssets + mNextLevelPath + ".json", mapData, true);
        }

        mCamera = new Camera(this, Vector2(mPlayer->GetPosition().x - mLogicalWindowWidth / 2,
                                           mPlayer->GetPosition().y - mLogicalWindowHeight / 2));
        mCamera->SetCameraMinBound(mCameraMinBound);
        mCamera->SetCameraMaxBound(mCameraMaxBound);

        mHUD = new HUD(this, "../Assets/Fonts/K2D-Bold.ttf");
        mBossMusic.Reset();
    }

    // Set new scene
    mGameScene = mNextScene;
    mCurrentLevelPath = mNextLevelPath;
}

void Game::LoadLevelSelectMenu() {
    float virtualWidth = mRenderer->GetVirtualWidth();
    float virtualHeight = mRenderer->GetVirtualHeight();

    mLevelSelectMenu = new UIScreen(this, "../Assets/Fonts/K2D-Bold.ttf");
    mLevelSelectMenu->SetSize(Vector2(virtualWidth * 0.8f, virtualHeight * 0.85f));
    mLevelSelectMenu->SetPosition(Vector2(virtualWidth * 0.1f, virtualHeight * 0.13f));

    const auto buttonSize = Vector2(mLevelSelectMenu->GetSize().x * 0.8f, 50);
    const auto buttonPointSize = static_cast<int>(34);
    const auto buttonPos = Vector2(mLevelSelectMenu->GetSize().x * 0.1f, 0.0f);

    mLevelSelectMenu->AddImage("../Assets/Sprites/Menus/Fundo2.png", mLevelSelectMenu->GetSize() / 2, mLevelSelectMenu->GetSize());

    std::string name = "   PRÓLOGO";
    mLevelSelectMenu->AddButton(name, buttonPos + Vector2(0, 2 * 35),
        buttonSize, buttonPointSize, UIButton::TextPos::AlignLeft,
        [this]()
        {
            mGoingToNextLevel = true;
            LoadNextLevel("0-Prologue/Prologue", 0.5f);
        });

    name = "   1 - FLORESTA";
    mLevelSelectMenu->AddButton(name, buttonPos + Vector2(0, 4 * 35),
        buttonSize, buttonPointSize, UIButton::TextPos::AlignLeft,
        [this]()
        {
            mGoingToNextLevel = true;
            LoadNextLevel("1-Musgo/Musgo", 0.5f);
        });

    name = "   2 - FOGO";
    mLevelSelectMenu->AddButton(name, buttonPos + Vector2(0, 6 * 35),
        buttonSize, buttonPointSize, UIButton::TextPos::AlignLeft,
        [this]()
        {
            mGoingToNextLevel = true;
            LoadNextLevel("2-Run/Run", 0.5f);
        });

    name = "   3 - PÂNTANO";
    mLevelSelectMenu->AddButton(name, buttonPos + Vector2(0, 8 * 35),
        buttonSize, buttonPointSize, UIButton::TextPos::AlignLeft,
        [this]()
        {
            mGoingToNextLevel = true;
            LoadNextLevel("3-Swamp/Swamp", 0.5f);
        });

    name = "   4 - NEVE";
    mLevelSelectMenu->AddButton(name, buttonPos + Vector2(0, 10 * 35),
        buttonSize, buttonPointSize, UIButton::TextPos::AlignLeft,
        [this]()
        {
            mGoingToNextLevel = true;
            LoadNextLevel("4-Pain/Pain", 0.5f);
        });

    name = "   ROOM 4";
    mLevelSelectMenu->AddButton(name, buttonPos + Vector2(0, 12 * 35),
        buttonSize, buttonPointSize, UIButton::TextPos::AlignLeft,
        [this]()
        {
            mGoingToNextLevel = true;
            LoadNextLevel("Room4/Room4", 0.5f);
        });

    name = "   TESTE";
    mLevelSelectMenu->AddButton(name, buttonPos + Vector2(0, 14 * 35),
        buttonSize, buttonPointSize, UIButton::TextPos::AlignLeft,
        [this]()
        {
            mGoingToNextLevel = true;
            LoadNextLevel("Forest/Forest", 0.5f);
        });

    name = "   COLISEU";
    mLevelSelectMenu->AddButton(name, buttonPos + Vector2(0, 16 * 35),
        buttonSize, buttonPointSize, UIButton::TextPos::AlignLeft,
        [this]()
        {
            mGoingToNextLevel = true;
            LoadNextLevel("Coliseu/Coliseu", 0.5f);
        });

    name = "   ROOM 0";
    mLevelSelectMenu->AddButton(name, buttonPos + Vector2(0, 18 * 35),
        buttonSize, buttonPointSize, UIButton::TextPos::AlignLeft,
        [this]()
        {
            mGoingToNextLevel = true;
            LoadNextLevel("Room0/Room0", 0.5f);
        });

    name = "VOLTAR";
    mLevelSelectMenu->AddButton(name, buttonPos + Vector2(0.0f, mLevelSelectMenu->GetSize().y - buttonSize.y * 1.2f),
        buttonSize, buttonPointSize, UIButton::TextPos::Center,
        [this]() { mLevelSelectMenu->Close(); });
}

void Game::BackToMenu() {
    SaveGame();
    LoadNextLevel("MainMenu", 0.5f);
}

void Game::ResetPlayerAndSkillTree() {
    delete mPlayer;
    mPlayer = nullptr;
    mPlayerDeathCounter = 0;
    delete mSkillTreeManager;
    mSkillTreeManager = nullptr;
    mSkillTreeManager = new SkillTreeManager();

    if (mMapManager) {
        delete mMapManager;
        mMapManager = nullptr;
    }
    mMapManager = new MapManager(this, mRenderer);
}

void Game::RebindKeyboard(UIText *text, Action action) {
    mWaitingForKey = true;
    mNewButtonText = text;
    mBindingAction = action;
}

void Game::RebindController(UIText *text, Action action) {
    mWaitingForButton = true;
    mNewButtonText = text;
    mBindingAction = action;
}

void Game::CancelRebind() {
    if (mNewButtonText) {
        std::string oldIcon = GetIconStringForAction(mBindingAction, mWaitingForKey, mWaitingForButton);

        mNewButtonText->SetFont(LoadFont("../Assets/Fonts/Buttons.ttf"));

        mNewButtonText->SetPointSize(54);
        mNewButtonText->SetText(oldIcon);

        mNewButtonText = nullptr;
    }

    mWaitingForKey = false;
    mWaitingForButton = false;
}

void Game::ResetKeyboardToDefault() {
    mInputBindings[Action::Up].key         = SDL_SCANCODE_UP;
    mInputBindings[Action::Down].key       = SDL_SCANCODE_DOWN;
    mInputBindings[Action::MoveLeft].key   = SDL_SCANCODE_LEFT;
    mInputBindings[Action::MoveRight].key  = SDL_SCANCODE_RIGHT;
    mInputBindings[Action::Jump].key       = SDL_SCANCODE_Z;
    mInputBindings[Action::Attack].key     = SDL_SCANCODE_X;
    mInputBindings[Action::Dash].key       = SDL_SCANCODE_C;
    mInputBindings[Action::Skill1].key     = SDL_SCANCODE_A;
    mInputBindings[Action::Skill2].key     = SDL_SCANCODE_D;
    mInputBindings[Action::Heal].key       = SDL_SCANCODE_V;
    mInputBindings[Action::Hook].key       = SDL_SCANCODE_S;
    mInputBindings[Action::OpenStore].key  = SDL_SCANCODE_SPACE;
    mInputBindings[Action::Map].key        = SDL_SCANCODE_LCTRL;
    mInputBindings[Action::Look].key       = SDL_SCANCODE_LALT;
    mInputBindings[Action::ChangeMode].key = SDL_SCANCODE_LSHIFT;
    mInputBindings[Action::Pause].key      = SDL_SCANCODE_ESCAPE;

    mInputBindings[Action::Up].mouseButton         = 0;
    mInputBindings[Action::Down].mouseButton       = 0;
    mInputBindings[Action::MoveLeft].mouseButton   = 0;
    mInputBindings[Action::MoveRight].mouseButton  = 0;
    mInputBindings[Action::Jump].mouseButton       = 0;
    mInputBindings[Action::Attack].mouseButton     = 0;
    mInputBindings[Action::Dash].mouseButton       = 0;
    mInputBindings[Action::Skill1].mouseButton     = 0;
    mInputBindings[Action::Skill2].mouseButton     = 0;
    mInputBindings[Action::Heal].mouseButton       = 0;
    mInputBindings[Action::Hook].mouseButton       = 0;
    mInputBindings[Action::OpenStore].mouseButton  = 0;
    mInputBindings[Action::Map].mouseButton        = 0;
    mInputBindings[Action::Look].mouseButton       = 0;
    mInputBindings[Action::ChangeMode].mouseButton = 0;
    mInputBindings[Action::Pause].mouseButton      = 0;

    SaveBindingsToFile("../Assets/InputBindings/InputBindings.json");
}

void Game::ResetKeyboardAndMouseToDefault() {
    mInputBindings[Action::Up].key         = SDL_SCANCODE_W;
    mInputBindings[Action::Down].key       = SDL_SCANCODE_S;
    mInputBindings[Action::MoveLeft].key   = SDL_SCANCODE_A;
    mInputBindings[Action::MoveRight].key  = SDL_SCANCODE_D;
    mInputBindings[Action::Jump].key       = SDL_SCANCODE_SPACE;
    mInputBindings[Action::Attack].key     = SDL_SCANCODE_UNKNOWN;
    mInputBindings[Action::Dash].key       = SDL_SCANCODE_UNKNOWN;
    mInputBindings[Action::Skill1].key     = SDL_SCANCODE_Q;
    mInputBindings[Action::Skill2].key     = SDL_SCANCODE_E;
    mInputBindings[Action::Heal].key       = SDL_SCANCODE_F;
    mInputBindings[Action::Hook].key       = SDL_SCANCODE_R;
    mInputBindings[Action::OpenStore].key  = SDL_SCANCODE_C;
    mInputBindings[Action::Map].key        = SDL_SCANCODE_TAB;
    mInputBindings[Action::Look].key       = SDL_SCANCODE_LCTRL;
    mInputBindings[Action::ChangeMode].key = SDL_SCANCODE_LSHIFT;
    mInputBindings[Action::Pause].key      = SDL_SCANCODE_ESCAPE;

    mInputBindings[Action::Up].mouseButton         = 0;
    mInputBindings[Action::Down].mouseButton       = 0;
    mInputBindings[Action::MoveLeft].mouseButton   = 0;
    mInputBindings[Action::MoveRight].mouseButton  = 0;
    mInputBindings[Action::Jump].mouseButton       = 0;
    mInputBindings[Action::Attack].mouseButton     = 1;
    mInputBindings[Action::Dash].mouseButton       = 3;
    mInputBindings[Action::Skill1].mouseButton     = 0;
    mInputBindings[Action::Skill2].mouseButton     = 0;
    mInputBindings[Action::Heal].mouseButton       = 0;
    mInputBindings[Action::Hook].mouseButton       = 0;
    mInputBindings[Action::OpenStore].mouseButton  = 0;
    mInputBindings[Action::Map].mouseButton        = 0;
    mInputBindings[Action::Look].mouseButton       = 0;
    mInputBindings[Action::ChangeMode].mouseButton = 0;
    mInputBindings[Action::Pause].mouseButton      = 0;

    SaveBindingsToFile("../Assets/InputBindings/InputBindings.json");
}

void Game::ResetControllerToDefault() {
    mInputBindings[Action::Up].btn  = SDL_CONTROLLER_BUTTON_DPAD_UP;
    mInputBindings[Action::Up].axis = SDL_CONTROLLER_AXIS_INVALID;

    mInputBindings[Action::Down].btn  = SDL_CONTROLLER_BUTTON_DPAD_DOWN;
    mInputBindings[Action::Down].axis = SDL_CONTROLLER_AXIS_INVALID;

    mInputBindings[Action::MoveLeft].btn  = SDL_CONTROLLER_BUTTON_DPAD_LEFT;
    mInputBindings[Action::MoveLeft].axis = SDL_CONTROLLER_AXIS_INVALID;

    mInputBindings[Action::MoveRight].btn  = SDL_CONTROLLER_BUTTON_DPAD_RIGHT;
    mInputBindings[Action::MoveRight].axis = SDL_CONTROLLER_AXIS_INVALID;

    mInputBindings[Action::Jump].btn  = SDL_CONTROLLER_BUTTON_A;
    mInputBindings[Action::Jump].axis = SDL_CONTROLLER_AXIS_INVALID;

    mInputBindings[Action::Attack].btn  = SDL_CONTROLLER_BUTTON_X;
    mInputBindings[Action::Attack].axis = SDL_CONTROLLER_AXIS_INVALID;

    mInputBindings[Action::Dash].btn  = SDL_CONTROLLER_BUTTON_RIGHTSHOULDER;
    mInputBindings[Action::Dash].axis = SDL_CONTROLLER_AXIS_INVALID;

    mInputBindings[Action::Skill1].btn  = SDL_CONTROLLER_BUTTON_B;
    mInputBindings[Action::Skill1].axis = SDL_CONTROLLER_AXIS_INVALID;

    mInputBindings[Action::Skill2].btn  = SDL_CONTROLLER_BUTTON_Y;
    mInputBindings[Action::Skill2].axis = SDL_CONTROLLER_AXIS_INVALID;

    mInputBindings[Action::Heal].btn  = SDL_CONTROLLER_BUTTON_INVALID;
    mInputBindings[Action::Heal].axis = SDL_CONTROLLER_AXIS_TRIGGERRIGHT;

    mInputBindings[Action::Hook].btn  = SDL_CONTROLLER_BUTTON_LEFTSHOULDER;
    mInputBindings[Action::Hook].axis = SDL_CONTROLLER_AXIS_INVALID;

    mInputBindings[Action::OpenStore].btn  = SDL_CONTROLLER_BUTTON_RIGHTSTICK;
    mInputBindings[Action::OpenStore].axis = SDL_CONTROLLER_AXIS_INVALID;

    mInputBindings[Action::Map].btn  = SDL_CONTROLLER_BUTTON_BACK;
    mInputBindings[Action::Map].axis = SDL_CONTROLLER_AXIS_INVALID;

    mInputBindings[Action::Look].btn  = SDL_CONTROLLER_BUTTON_INVALID;
    mInputBindings[Action::Look].axis = SDL_CONTROLLER_AXIS_INVALID;

    mInputBindings[Action::ChangeMode].btn  = SDL_CONTROLLER_BUTTON_INVALID;
    mInputBindings[Action::ChangeMode].axis = SDL_CONTROLLER_AXIS_TRIGGERLEFT;

    mInputBindings[Action::Pause].btn  = SDL_CONTROLLER_BUTTON_START;
    mInputBindings[Action::Pause].axis = SDL_CONTROLLER_AXIS_INVALID;

    SaveBindingsToFile("../Assets/InputBindings/InputBindings.json");
}

std::string Game::GetIconStringForAction(Action action, bool forceKeyboard, bool forceController) {
    InputBinding binding = mInputBindings[action];

    InputPlayerMode currentMode = GetInputPlayerMode();

    if (forceKeyboard) {
        currentMode = InputPlayerMode::Keyboard;
    }
    else if (forceController) {
        currentMode = InputPlayerMode::Controller;
    }

    if (currentMode == InputPlayerMode::Keyboard ||
        currentMode == InputPlayerMode::Mouse)
    {
        if (binding.key != SDL_SCANCODE_UNKNOWN) {
            switch(binding.key) {
                // Teclas Especiais (mapeadas na Private Use Area do IcoMoon)
                case SDL_SCANCODE_LALT:         return Icons::KeyAlt;
                case SDL_SCANCODE_DOWN:         return Icons::KeyArrowDown;
                case SDL_SCANCODE_LEFT:         return Icons::KeyArrowLeft;
                case SDL_SCANCODE_RIGHT:        return Icons::KeyArrowRight;
                case SDL_SCANCODE_UP:           return Icons::KeyArrowUp;
                case SDL_SCANCODE_BACKSPACE:    return Icons::KeyBackSpace;
                case SDL_SCANCODE_CAPSLOCK:     return Icons::KeyCapsLock;
                case SDL_SCANCODE_LCTRL:        return Icons::KeyCtrl;
                case SDL_SCANCODE_DELETE:       return Icons::KeyDelete;
                case SDL_SCANCODE_RETURN:       return Icons::KeyEnter;
                case SDL_SCANCODE_ESCAPE:       return Icons::KeyEscape;
                case SDL_SCANCODE_F1:           return Icons::KeyF1;
                case SDL_SCANCODE_F2:           return Icons::KeyF2;
                case SDL_SCANCODE_F3:           return Icons::KeyF3;
                case SDL_SCANCODE_F4:           return Icons::KeyF4;
                case SDL_SCANCODE_F5:           return Icons::KeyF5;
                case SDL_SCANCODE_F6:           return Icons::KeyF6;
                case SDL_SCANCODE_F7:           return Icons::KeyF7;
                case SDL_SCANCODE_F8:           return Icons::KeyF8;
                case SDL_SCANCODE_F9:           return Icons::KeyF9;
                case SDL_SCANCODE_F10:          return Icons::KeyF10;
                case SDL_SCANCODE_F11:          return Icons::KeyF11;
                case SDL_SCANCODE_F12:          return Icons::KeyF12;
                case SDL_SCANCODE_NUMLOCKCLEAR: return Icons::KeyNumLock;
                case SDL_SCANCODE_PRINTSCREEN:  return Icons::KeyPrintScreen;
                case SDL_SCANCODE_LSHIFT:       return Icons::KeyShift;
                case SDL_SCANCODE_SPACE:        return Icons::KeySpace;
                case SDL_SCANCODE_TAB:          return Icons::KeyTab;
                case SDL_SCANCODE_LGUI:         return Icons::KeyWindows;
                case SDL_SCANCODE_RALT:         return Icons::KeyAlt;
                case SDL_SCANCODE_RCTRL:        return Icons::KeyCtrl;
                case SDL_SCANCODE_RSHIFT:       return Icons::KeyShift;
                case SDL_SCANCODE_KP_MINUS:     return "-";
                case SDL_SCANCODE_KP_PLUS:      return "+";
                case SDL_SCANCODE_KP_MULTIPLY:  return "*";
                case SDL_SCANCODE_KP_DIVIDE:    return "/";
                case SDL_SCANCODE_KP_PERIOD:    return ".";
                case SDL_SCANCODE_KP_0:         return "0";
                case SDL_SCANCODE_KP_1:         return "1";
                case SDL_SCANCODE_KP_2:         return "2";
                case SDL_SCANCODE_KP_3:         return "3";
                case SDL_SCANCODE_KP_4:         return "4";
                case SDL_SCANCODE_KP_5:         return "5";
                case SDL_SCANCODE_KP_6:         return "6";
                case SDL_SCANCODE_KP_7:         return "7";
                case SDL_SCANCODE_KP_8:         return "8";
                case SDL_SCANCODE_KP_9:         return "9";

                // Fallback Inteligente para teclado
                default: {
                    std::string keyName = SDL_GetScancodeName(binding.key);
                    return keyName;
                }
            }
        }

        switch(binding.mouseButton) {
            case SDL_BUTTON_LEFT:   return Icons::MouseLeft;
            case SDL_BUTTON_RIGHT:  return Icons::MouseRight;
            case SDL_BUTTON_MIDDLE: return Icons::MouseMiddle;
            default: return "?";
        }
    }
    else if (currentMode == InputPlayerMode::Controller) {
        if (mInputController == InputController::Xbox) {
            switch(binding.btn) {
                case SDL_CONTROLLER_BUTTON_A: return Icons::XboxA;
                case SDL_CONTROLLER_BUTTON_B: return Icons::XboxB;
                case SDL_CONTROLLER_BUTTON_X: return Icons::XboxX;
                case SDL_CONTROLLER_BUTTON_Y: return Icons::XboxY;
                case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:  return Icons::XboxLB;
                case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER: return Icons::XboxRB;
                case SDL_CONTROLLER_BUTTON_LEFTSTICK: return Icons::XboxL3;
                case SDL_CONTROLLER_BUTTON_RIGHTSTICK: return Icons::XboxR3;
                case SDL_CONTROLLER_BUTTON_START: return Icons::XboxMenu;
                case SDL_CONTROLLER_BUTTON_BACK: return Icons::XboxView;
                default: break;
            }

            if (binding.axis == SDL_CONTROLLER_AXIS_TRIGGERLEFT)  return Icons::XboxLT;
            if (binding.axis == SDL_CONTROLLER_AXIS_TRIGGERRIGHT) return Icons::XboxRT;

            return "?";
        }
        else if (mInputController == InputController::Playstation) {
            switch(binding.btn) {
                case SDL_CONTROLLER_BUTTON_A: return Icons::PlayCross;
                case SDL_CONTROLLER_BUTTON_B: return Icons::PlayCircle;
                case SDL_CONTROLLER_BUTTON_X: return Icons::PlaySquare;
                case SDL_CONTROLLER_BUTTON_Y: return Icons::PlayTriangle;
                case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:  return Icons::PlayL1;
                case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER: return Icons::PlayR1;
                case SDL_CONTROLLER_BUTTON_LEFTSTICK: return Icons::PlayL3;
                case SDL_CONTROLLER_BUTTON_RIGHTSTICK: return Icons::PlayR3;
                case SDL_CONTROLLER_BUTTON_START: return Icons::PlayOptions;
                case SDL_CONTROLLER_BUTTON_BACK: return Icons::PlayShare;
                default: break;
            }

            if (binding.axis == SDL_CONTROLLER_AXIS_TRIGGERLEFT)  return Icons::PlayL2;
            if (binding.axis == SDL_CONTROLLER_AXIS_TRIGGERRIGHT) return Icons::PlayR2;

            return "?";
        }
        else {
            switch(binding.btn) {
                case SDL_CONTROLLER_BUTTON_A: return Icons::XboxA;
                case SDL_CONTROLLER_BUTTON_B: return Icons::XboxB;
                case SDL_CONTROLLER_BUTTON_X: return Icons::XboxX;
                case SDL_CONTROLLER_BUTTON_Y: return Icons::XboxY;
                case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:  return Icons::XboxLB;
                case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER: return Icons::XboxRB;
                case SDL_CONTROLLER_BUTTON_LEFTSTICK: return Icons::XboxL3;
                case SDL_CONTROLLER_BUTTON_RIGHTSTICK: return Icons::XboxR3;
                case SDL_CONTROLLER_BUTTON_START: return Icons::XboxMenu;
                case SDL_CONTROLLER_BUTTON_BACK: return Icons::XboxView;
                default: break;
            }

            if (binding.axis == SDL_CONTROLLER_AXIS_TRIGGERLEFT)  return Icons::XboxLT;
            if (binding.axis == SDL_CONTROLLER_AXIS_TRIGGERRIGHT) return Icons::XboxRT;

            return "?";
        }
    }

    return "?";
}

void Game::SwapKeyboardBinding(SDL_Scancode newKey, Uint8 newMouseBtn) {
    auto oldBinding = mInputBindings[mBindingAction];

    // Procura se alguma outra ação já usa essa tecla OU esse botão do mouse
    for (auto& pair : mInputBindings) {
        if (pair.first != mBindingAction) {
            bool conflict = false;

            // Checa conflito de teclado
            if (newKey != SDL_SCANCODE_UNKNOWN && pair.second.key == newKey) {
                conflict = true;
            }
            // Checa conflito de mouse
            if (newMouseBtn != 0 && pair.second.mouseButton == newMouseBtn) {
                conflict = true;
            }

            if (conflict) {
                // Passa o mapeamento antigo completo para a ação conflitante
                pair.second.key = oldBinding.key;
                pair.second.mouseButton = oldBinding.mouseButton;
                break;
            }
        }
    }

    // Define a nova tecla para a ação atual
    mInputBindings[mBindingAction].key = newKey;
    mInputBindings[mBindingAction].mouseButton = newMouseBtn;
    SaveBindingsToFile("../Assets/InputBindings/InputBindings.json");
}

void Game::SwapControllerBinding(SDL_GameControllerButton newBtn, SDL_GameControllerAxis newAxis) {
    auto oldBinding = mInputBindings[mBindingAction];

    // Procura se alguma outra ação já usa esse botão ou eixo
    for (auto& pair : mInputBindings) {
        if (pair.first != mBindingAction) {
            bool conflict = false;

            // Checa conflito de botão
            if (newBtn != SDL_CONTROLLER_BUTTON_INVALID && pair.second.btn == newBtn) {
                conflict = true;
            }
            // Checa conflito de eixo (gatilhos)
            if (newAxis != SDL_CONTROLLER_AXIS_INVALID && pair.second.axis == newAxis) {
                conflict = true;
            }

            if (conflict) {
                // Passa o mapeamento antigo completo para a ação conflitante
                pair.second.btn = oldBinding.btn;
                pair.second.axis = oldBinding.axis;
                break;
            }
        }
    }

    // Define o novo mapeamento para a ação atual
    mInputBindings[mBindingAction].btn = newBtn;
    mInputBindings[mBindingAction].axis = newAxis;
    SaveBindingsToFile("../Assets/InputBindings/InputBindings.json");
}

void Game::LoadObjects(const nlohmann::json& mapData) {
    // Carrega primeiro O CameraBounds
    for (const auto &layer: mapData["layers"]) {
        if (layer["name"] == "CameraBounds") {
            for (const auto &obj: layer["objects"]) {
                float x = obj["x"];
                float y = obj["y"];
                float width = obj["width"];
                float height = obj["height"];

                mCameraMinBound = Vector2(x, y);
                mCameraMaxBound = Vector2(x + width, y + height);
            }
        }
    }

    for (const auto &layer: mapData["layers"]) {
        // Pula o CameraBounds pois já foi processado no Passo 1
        if (layer["name"] == "CameraBounds") {
            continue;
        }
        if (layer["name"] == "Grounds") {
            for (const auto &obj: layer["objects"]) {
                std::string name = obj["name"];
                float xOriginal = obj["x"];
                float yOriginal = obj["y"];
                float widthOriginal = obj["width"];
                float heightOriginal = obj["height"];
                float x = obj["x"];
                float y = obj["y"];
                float width = obj["width"];
                float height = obj["height"];
                int id = obj["id"];
                bool isSpike = false;
                bool isMoving = false;
                bool isBreakable = false;
                float respawnPositionX = 0.0f;
                float respawnPositionY = 0.0f;
                float movingDuration = 0.0f;
                float speedX = 0.0f;
                float speedY = 0.0f;
                float growSpeedX = 0.0f;
                float growSpeedY = 0.0f;
                int growthDirection = 0;
                float minHeight = 0.0f;
                float minWidth = 0.0f;
                float startWidth = 0.0f;
                float startHeight = 0.0f;
                bool isOscillating = false;
                std::string condition;

                if (obj.contains("properties")) {
                    for (const auto &prop: obj["properties"]) {
                        std::string propName = prop["name"];
                        if (propName == "Spike") {
                            isSpike = prop["value"];
                        }
                        else if (propName == "Moving") {
                            isMoving = prop["value"];
                        }
                        else if (propName == "Breakable") {
                            isBreakable = prop["value"];
                        }
                        else if (propName == "RespawnPositionX") {
                            respawnPositionX = static_cast<float>(prop["value"]);
                        }
                        else if (propName == "RespawnPositionY") {
                            respawnPositionY = static_cast<float>(prop["value"]);
                        }
                        else if (propName == "MovingDuration") {
                            movingDuration = prop["value"];
                        }
                        else if (propName == "SpeedX") {
                            speedX = prop["value"];
                        }
                        else if (propName == "SpeedY") {
                            speedY = prop["value"];
                        }
                        else if (propName == "GrowSpeedX") {
                            growSpeedX = static_cast<float>(prop["value"]);
                        }
                        else if (propName == "GrowSpeedY") {
                            growSpeedY = static_cast<float>(prop["value"]);
                        }
                        else if (propName == "GrowthDirection") {
                            growthDirection = static_cast<int>(prop["value"]);
                        }
                        else if (propName == "MinHeight") {
                            minHeight = static_cast<float>(prop["value"]);
                        }
                        else if (propName == "MinWidth") {
                            minWidth = static_cast<float>(prop["value"]);
                        }
                        else if (propName == "StartHeight") {
                            startHeight = static_cast<float>(prop["value"]);
                        }
                        else if (propName == "StartWidth") {
                            startWidth = static_cast<float>(prop["value"]);
                        }
                        else if (propName == "Oscillate") {
                            isOscillating = static_cast<float>(prop["value"]);
                        }
                        else if (propName == "Condition") {
                            condition = prop["value"];
                        }
                    }
                }

                if (!ShouldLoadObject(condition)) {
                    continue;
                }

                if (name == "DynamicGround") {
                    auto* dynamicGround = new DynamicGround(this, startWidth, startHeight, isSpike, isMoving, movingDuration, Vector2(speedX, speedY), mGroundBehindPlayer, mUseGroundPadding);
                    dynamicGround->SetId(id);
                    dynamicGround->SetIsBreakable(isBreakable);
                    dynamicGround->SetRespawnPosition(Vector2(respawnPositionX, respawnPositionY));
                    dynamicGround->SetIsOscillating(isOscillating);
                    dynamicGround->SetMaxWidth(width);
                    dynamicGround->SetMaxHeight(height);
                    dynamicGround->SetMinWidth(minWidth);
                    dynamicGround->SetMinHeight(minHeight);
                    dynamicGround->SetGrowSpeed(Vector2(growSpeedX, growSpeedY));
                    switch (growthDirection) {
                        case 0:
                            dynamicGround->SetGrowDirection(GrowthDirection::Up);
                            dynamicGround->SetPosition(Vector2(x + width / 2, y + height - startHeight / 2));
                        break;

                        case 1:
                            dynamicGround->SetGrowDirection(GrowthDirection::Down);
                            dynamicGround->SetPosition(Vector2(x + width / 2, y + startHeight / 2));
                        break;

                        case 2:
                            dynamicGround->SetGrowDirection(GrowthDirection::Left);
                            dynamicGround->SetPosition(Vector2(x + width - startWidth / 2, y + height / 2));
                        break;

                        case 3:
                            dynamicGround->SetGrowDirection(GrowthDirection::Right);
                            dynamicGround->SetPosition(Vector2(x + startWidth / 2, y + height / 2));
                        break;
                    }
                    dynamicGround->SetStartingPosition(Vector2(x + width / 2, y + height / 2));
                    dynamicGround->SetTilesIndex(widthOriginal, heightOriginal, xOriginal, yOriginal);
                }
                else {
                    auto* ground = new Ground(this, width, height, isSpike, isMoving, movingDuration, Vector2(speedX, speedY), mGroundBehindPlayer, mUseGroundPadding);
                    ground->SetId(id);
                    ground->SetIsBreakable(isBreakable);
                    ground->SetPosition(Vector2(x + width / 2, y + height / 2));
                    ground->SetRespawnPosition(Vector2(respawnPositionX, respawnPositionY));
                    ground->SetStartingPosition(Vector2(x + width / 2, y + height / 2));
                    ground->SetTilesIndex(widthOriginal, heightOriginal, xOriginal, yOriginal);
                }
            }
        }
        if (layer["name"] == "Background1" || layer["name"] == "Background2" || layer["name"] == "Decorations" || layer["name"] == "DecorationsForeground" || layer["name"] == "Foreground1" || layer["name"] == "Foreground2") {
            float parallaxX = 1.0f;
            float parallaxY = 1.0f;
            if (layer.contains("parallaxx")) {
                parallaxX = layer["parallaxx"];
            }
            if (layer.contains("parallaxy")) {
                parallaxY = layer["parallaxy"];
            }
            Vector3 textureColor = Color::White;
            float textureFactor = 1.0f;
            if (layer.contains("properties")) {
                for (const auto &prop: layer["properties"]) {
                    std::string propName = prop["name"];
                    if (propName == "TextureColorR") {
                        textureColor.x = prop["value"];
                    }
                    if (propName == "TextureColorG") {
                        textureColor.y = prop["value"];
                    }
                    if (propName == "TextureColorB") {
                        textureColor.z = prop["value"];
                    }
                    if (propName == "TextureFactor") {
                        textureFactor = prop["value"];
                    }
                }
            }
            textureColor /= 255.0f;
            for (const auto &obj: layer["objects"]) {
                float x = obj["x"];
                float y = obj["y"];
                float width = obj["width"];
                float height = obj["height"];
                float rotation = Math::ToRadians(obj["rotation"]);
                int rawGid = 0;

                if (obj.contains("gid")) {
                    rawGid = obj["gid"];
                }

                // Limpar as flags do Tiled para obter o GID real
                const unsigned FLIPPED_HORIZONTALLY_FLAG = 0x80000000;
                const unsigned FLIPPED_VERTICALLY_FLAG   = 0x40000000;
                const unsigned FLIPPED_DIAGONALLY_FLAG   = 0x20000000;

                int cleanGid = rawGid & ~(FLIPPED_HORIZONTALLY_FLAG | FLIPPED_VERTICALLY_FLAG | FLIPPED_DIAGONALLY_FLAG);

                std::string decorationName = "";

                if (cleanGid >= mDecorationsFirstGid) {
                    int normalizedGid = (cleanGid - mDecorationsFirstGid) + 1;
                    decorationName = GetDecorationNameFromGid(normalizedGid);
                }

                // Fallback: Se não achar no JSON de mapa, tenta pegar da propriedade manual
                if (decorationName.empty() && obj.contains("name") && !std::string(obj["name"]).empty()) {
                    decorationName = obj["name"];
                }

                std::string imagePath;
                float fps = 1.0f;
                int numFrames = 1;
                bool animated = false;
                bool destructible = false;
                bool windBalance = false;
                bool isMoving = false;
                float movingDuration = 0.0f;
                float speedX = 0.0f;
                float speedY = 0.0f;

                if (obj.contains("properties")) {
                    for (const auto &prop: obj["properties"]) {
                        std::string propName = prop["name"];
                        if (propName == "Path") {
                            imagePath = prop["value"];
                        }
                        if (propName == "FPS") {
                            fps = prop["value"];
                        }
                        if (propName == "NumFrames") {
                            numFrames = prop["value"];
                        }
                        if (propName == "Animated") {
                            animated = prop["value"];
                        }
                        if (propName == "Destructible") {
                            destructible = prop["value"];
                        }
                        if (propName == "WindBalance") {
                            windBalance = prop["value"];
                        }
                        else if (propName == "Moving") {
                            isMoving = prop["value"];
                        }
                        else if (propName == "MovingDuration") {
                            movingDuration = prop["value"];
                        }
                        else if (propName == "SpeedX") {
                            speedX = prop["value"];
                        }
                        else if (propName == "SpeedY") {
                            speedY = prop["value"];
                        }
                    }
                }
                int drawOrder;
                if (layer["name"] == "Background1") {
                    drawOrder = 50;
                }
                if (layer["name"] == "Background2") {
                    drawOrder = 70;
                }
                if (layer["name"] == "Decorations") {
                    drawOrder = 200;
                }
                if (layer["name"] == "DecorationsForeground") {
                    drawOrder = 1005;
                }
                if (layer["name"] == "Foreground1") {
                    drawOrder = 6000;
                }
                if (layer["name"] == "Foreground2") {
                    drawOrder = 7000;
                }

                float dx = width / 2.0f;
                float dy = height / 2.0f;

                float rotatedDx = (dx * cos(rotation)) + (dy * sin(rotation));
                float rotatedDy = (dx * sin(rotation)) - (dy * cos(rotation));

                auto* decoration = new Decorations(this, width, height, imagePath, decorationName, fps, numFrames, animated, rawGid, rotation, drawOrder, Vector2(parallaxX, parallaxY), textureColor, textureFactor, destructible);
                decoration->SetPosition(Vector2(x + rotatedDx, y + rotatedDy));
                decoration->SetWindBalance(windBalance);
                decoration->SetMovement(isMoving, Vector2(speedX, speedY), movingDuration);
            }
        }
        if (layer["name"] == "Light") {
            for (const auto &obj: layer["objects"]) {
                float x = obj["x"];
                float y = obj["y"];
                float intensity = 0.0f;
                float radius = 0.0f;
                Vector3 color = Vector3::One;

                if (obj.contains("properties")) {
                    for (const auto &prop: obj["properties"]) {
                        std::string propName = prop["name"];
                        if (propName == "Radius") {
                            radius = prop["value"];
                        }
                        else if (propName == "Intensity") {
                            intensity = prop["value"];
                        }
                        else if (propName == "ColorR") {
                            color.x = prop["value"];
                        }
                        else if (propName == "ColorG") {
                            color.y = prop["value"];
                        }
                        else if (propName == "ColorB") {
                            color.z = prop["value"];
                        }
                    }
                }
                auto* light = new Light(this);
                light->SetPosition(Vector2(x, y));
                light->SetRadius(radius);
                light->SetMaxIntensity(intensity);
                light->SetColor(color);
                light->Activate();
            }
        }
        if (layer["name"] == "Brazier") {
            for (const auto &obj: layer["objects"]) {
                float x = obj["x"];
                float y = obj["y"];
                float width = obj["width"];
                float height = obj["height"];
                bool brazierOn = false;

                if (obj.contains("properties")) {
                    for (const auto &prop: obj["properties"]) {
                        std::string propName = prop["name"];
                        if (propName == "BrazierOn") {
                            brazierOn = prop["value"];
                        }
                    }
                }
                if (brazierOn) {
                    auto* brazier = new Brazier(this, Brazier::BrazierState::LightOn);
                    brazier->SetPosition(Vector2(x + width / 2, y + height / 2));
                }
                else {
                    auto* brazier = new Brazier(this, Brazier::BrazierState::LightOff);
                    brazier->SetPosition(Vector2(x + width / 2, y + height / 2));
                }
            }
        }
        if (layer["name"] == "Lava") {
            for (const auto &obj: layer["objects"]) {
                float x = obj["x"];
                float y = obj["y"];
                float width = obj["width"];
                float height = obj["height"];
                float respawnPositionX = 0.0f;
                float respawnPositionY = 0.0f;
                bool isMoving = false;
                float movingDuration = 0.0f;
                float speedX = 0.0f;
                float speedY = 0.0f;

                if (obj.contains("properties")) {
                    for (const auto &prop: obj["properties"]) {
                        std::string propName = prop["name"];
                        if (propName == "RespawnPositionX") {
                            respawnPositionX = static_cast<float>(prop["value"]);
                        }
                        else if (propName == "RespawnPositionY") {
                            respawnPositionY = static_cast<float>(prop["value"]);
                        }
                        else if (propName == "Moving") {
                            isMoving = prop["value"];
                        }
                        else if (propName == "MovingDuration") {
                            movingDuration = prop["value"];
                        }
                        else if (propName == "SpeedX") {
                            speedX = prop["value"];
                        }
                        else if (propName == "SpeedY") {
                            speedY = prop["value"];
                        }
                    }
                }
                auto* lava = new Lava(this, width, height, isMoving, movingDuration, Vector2(speedX, speedY));
                lava->SetPosition(Vector2(x + width / 2, y + height / 2));
                lava->SetRespawnPosition(Vector2(respawnPositionX, respawnPositionY));
            }
        }
        if (layer["name"] == "LightningBarrier") {
            for (const auto &obj: layer["objects"]) {
                float x = obj["x"];
                float y = obj["y"];
                float width = obj["width"];
                float height = obj["height"];
                bool isMoving = false;
                float movingDuration = 0.0f;
                float speedX = 0.0f;
                float speedY = 0.0f;

                if (obj.contains("properties")) {
                    for (const auto &prop: obj["properties"]) {
                        std::string propName = prop["name"];
                        if (propName == "Moving") {
                            isMoving = prop["value"];
                        }
                        else if (propName == "MovingDuration") {
                            movingDuration = prop["value"];
                        }
                        else if (propName == "SpeedX") {
                            speedX = prop["value"];
                        }
                        else if (propName == "SpeedY") {
                            speedY = prop["value"];
                        }
                    }
                }
                auto* lightningBarrier = new LightningBarrier(this, width, height, isMoving, movingDuration, Vector2(speedX, speedY));
                lightningBarrier->SetPosition(Vector2(x + width / 2, y + height / 2));
            }
        }
        if (layer["name"] == "Triggers") {
            for (const auto &obj: layer["objects"]) {
                float x = obj["x"];
                float y = obj["y"];
                float width = obj["width"];
                float height = obj["height"];
                bool destroy = false;
                std::string target;
                std::string event;
                std::string grounds;
                std::string enemies;
                std::string skillName;
                std::vector<int> groundsIds;
                std::vector<int> enemiesIds;
                float fixedCameraPositionX = 0;
                float fixedCameraPositionY = 0;
                Vector2 limitMinCameraPosition(Vector2::Zero);
                Vector2 limitMaxCameraPosition(Vector2::Zero);
                std::string scene;
                int playerStartPositionId = 0;
                std::string wavePath;
                std::string worldState;
                bool worldStateFlag = false;
                float targetZoom = 1.0f;
                float zoomSpeed = 1.0f;
                Vector3 ambientColor(1, 1, 1);
                float ambientIntensity = 1;
                std::string dialoguePath;
                std::string cutsceneId;
                std::string tutorialText;
                std::string condition;
                if (obj.contains("properties")) {
                    for (const auto &prop: obj["properties"]) {
                        std::string propName = prop["name"];
                        if (propName == "Target") {
                            target = prop["value"];
                        }
                        else if (propName == "Event") {
                            event = prop["value"];
                        }
                        else if (propName == "Destroy") {
                            destroy = prop["value"];
                        }
                        else if (propName == "Grounds") {
                            grounds = prop["value"];
                        }
                        else if (propName == "Enemies") {
                            enemies = prop["value"];
                        }
                        else if (propName == "SkillName") {
                            skillName = prop["value"];
                        }
                        else if (propName == "FixedCameraPositionX") {
                            fixedCameraPositionX = prop["value"];
                        }
                        else if (propName == "FixedCameraPositionY") {
                            fixedCameraPositionY = prop["value"];
                        }
                        else if (propName == "LimitMinCameraPositionX") {
                            limitMinCameraPosition.x = prop["value"];
                        }
                        else if (propName == "LimitMinCameraPositionY") {
                            limitMinCameraPosition.y = prop["value"];
                        }
                        else if (propName == "LimitMaxCameraPositionX") {
                            limitMaxCameraPosition.x = prop["value"];
                        }
                        else if (propName == "LimitMaxCameraPositionY") {
                            limitMaxCameraPosition.y = prop["value"];
                        }
                        else if (propName == "Scene") {
                            scene = prop["value"];
                        }
                        else if (propName == "PlayerStartPositionId") {
                            playerStartPositionId = prop["value"];
                        }
                        else if (propName == "Waves") {
                            wavePath = prop["value"];
                        }
                        else if (propName == "WorldState") {
                            worldState = prop["value"];
                        }
                        else if (propName == "WorldStateFlag") {
                            worldStateFlag = prop["value"];
                        }
                        else if (propName == "TargetZoom") {
                            targetZoom = prop["value"];
                        }
                        else if (propName == "ZoomSpeed") {
                            zoomSpeed = prop["value"];
                        }
                        else if (propName == "AmbientColorR") {
                            ambientColor.x = prop["value"];
                        }
                        else if (propName == "AmbientColorG") {
                            ambientColor.y = prop["value"];
                        }
                        else if (propName == "AmbientColorB") {
                            ambientColor.z = prop["value"];
                        }
                        else if (propName == "AmbientIntensity") {
                            ambientIntensity = prop["value"];
                        }
                        else if (propName == "FilePath") {
                            dialoguePath = prop["value"];
                        }
                        else if (propName == "CutsceneId") {
                            cutsceneId = prop["value"];
                        }
                        else if (propName == "TutorialText") {
                            tutorialText = prop["value"];
                        }
                        else if (propName == "Condition") {
                            condition = prop["value"];
                        }
                    }
                }

                if (!ShouldLoadObject(condition)) {
                    continue;
                }

                groundsIds = ParseIntList(grounds);
                enemiesIds = ParseIntList(enemies);

                std::string worldStateCondition = condition;
                if (condition[0] == '!') {
                    worldStateCondition = condition.substr(1);
                }

                auto* trigger = new Trigger(this, width, height);
                trigger->SetPosition(Vector2(x + width / 2, y + height / 2));
                trigger->SetTarget(target);
                trigger->SetEvent(event);
                trigger->SetDestroy(destroy);
                trigger->SetCondition(worldStateCondition);
                trigger->SetGroundsIds(groundsIds);
                trigger->SetEnemiesIds(enemiesIds);
                trigger->SetSkillName(skillName);
                trigger->SetFixedCameraPosition(Vector2(fixedCameraPositionX, fixedCameraPositionY));
                trigger->SetLimitMinCameraPosition(limitMinCameraPosition);
                trigger->SetLimitMaxCameraPosition(limitMaxCameraPosition);
                trigger->SetNextLevelPath(scene);
                trigger->SetPlayerStartPositionId(playerStartPositionId);
                trigger->SetWavesPath(wavePath);
                trigger->SetWorldState(worldState);
                trigger->SetWorldStateFlag(worldStateFlag);
                trigger->SetTargetZoom(targetZoom);
                trigger->SetZoomSpeed(zoomSpeed);
                trigger->SetAmbientColor(ambientColor);
                trigger->SetAmbientIntensity(ambientIntensity);
                trigger->SetDialoguePath(dialoguePath);
                trigger->SetCutsceneId(cutsceneId);
                trigger->SetTutorialText(tutorialText);
            }
        }
        if (layer["name"] == "Camera") {
            for (const auto &obj: layer["objects"]) {
                float x = obj["x"];
                float y = obj["y"];
                float width = obj["width"];
                float height = obj["height"];
                bool destroy = false;
                std::string target;
                std::string event;
                float fixedCameraPositionX = 0;
                float fixedCameraPositionY = 0;
                Vector2 limitMinCameraPosition(Vector2::Zero);
                Vector2 limitMaxCameraPosition(Vector2::Zero);
                std::string condition;
                if (obj.contains("properties")) {
                    for (const auto &prop: obj["properties"]) {
                        std::string propName = prop["name"];
                        if (propName == "Target") {
                            target = prop["value"];
                        }
                        else if (propName == "Event") {
                            event = prop["value"];
                        }
                        else if (propName == "Destroy") {
                            destroy = prop["value"];
                        }
                        else if (propName == "FixedCameraPositionX") {
                            fixedCameraPositionX = prop["value"];
                        }
                        else if (propName == "FixedCameraPositionY") {
                            fixedCameraPositionY = prop["value"];
                        }
                        else if (propName == "LimitMinCameraPositionX") {
                            limitMinCameraPosition.x = prop["value"];
                        }
                        else if (propName == "LimitMinCameraPositionY") {
                            limitMinCameraPosition.y = prop["value"];
                        }
                        else if (propName == "LimitMaxCameraPositionX") {
                            limitMaxCameraPosition.x = prop["value"];
                        }
                        else if (propName == "LimitMaxCameraPositionY") {
                            limitMaxCameraPosition.y = prop["value"];
                        }
                        else if (propName == "Condition") {
                            condition = prop["value"];
                        }
                    }
                }

                if (!ShouldLoadObject(condition)) {
                    continue;
                }

                auto* trigger = new Trigger(this, width, height);
                trigger->SetPosition(Vector2(x + width / 2, y + height / 2));
                trigger->SetTarget(target);
                trigger->SetEvent(event);
                trigger->SetDestroy(destroy);
                trigger->SetFixedCameraPosition(Vector2(fixedCameraPositionX, fixedCameraPositionY));
                trigger->SetLimitMinCameraPosition(limitMinCameraPosition);
                trigger->SetLimitMaxCameraPosition(limitMaxCameraPosition);
            }
        }

        if (layer["name"] == "Levers") {
            for (const auto &obj: layer["objects"]) {
                float x = obj["x"];
                float y = obj["y"];
                float width = obj["width"];
                float height = obj["height"];
                std::string target;
                std::string event;
                std::string grounds;
                std::string enemies;
                std::vector<int> groundsIds;
                std::vector<int> enemiesIds;
                float fixedCameraPositionX = 0;
                float fixedCameraPositionY = 0;
                std::string worldState;
                bool worldStateFlag = false;
                std::string condition;
                if (obj.contains("properties")) {
                    for (const auto &prop: obj["properties"]) {
                        std::string propName = prop["name"];
                        if (propName == "Target") {
                            target = prop["value"];
                        }
                        else if (propName == "Event") {
                            event = prop["value"];
                        }
                        else if (propName == "Grounds") {
                            grounds = prop["value"];
                        }
                        else if (propName == "Enemies") {
                            enemies = prop["value"];
                        }
                        else if (propName == "FixedCameraPositionX") {
                            fixedCameraPositionX = prop["value"];
                        }
                        else if (propName == "FixedCameraPositionY") {
                            fixedCameraPositionY = prop["value"];
                        }
                        else if (propName == "WorldState") {
                            worldState = prop["value"];
                        }
                        else if (propName == "WorldStateFlag") {
                            worldStateFlag = prop["value"];
                        }
                        else if (propName == "Condition") {
                            condition = prop["value"];
                        }
                    }
                }

                if ((target == "DynamicGround" || target == "Ground") && !grounds.empty()) {
                    groundsIds = ParseIntList(grounds);
                }
                if (target == "Enemy") {
                    enemiesIds = ParseIntList(enemies);
                }
                auto* lever = new Lever(this);
                lever->SetPosition(Vector2(x + width / 2, y + height / 2));
                lever->SetTarget(target);
                lever->SetEvent(event);
                lever->SetGroundsIds(groundsIds);
                lever->SetEnemiesIds(enemiesIds);
                lever->SetFixedCameraPosition(Vector2(fixedCameraPositionX, fixedCameraPositionY));
                lever->SetWorldState(worldState);
                lever->SetWorldStateFlag(worldStateFlag);

                if (!ShouldLoadObject(condition)) {
                    lever->Activate();
                }
            }
        }

        if (layer["name"] == "HookPoints") {
            for (const auto &obj: layer["objects"]) {
                float x = obj["x"];
                float y = obj["y"];

                auto* hookPoint = new HookPoint(this);
                hookPoint->SetPosition(Vector2(x, y));
            }
        }

        if (layer["name"] == "SpawnPoint") {
            for (const auto &obj: layer["objects"]) {
                float x = obj["x"];
                float y = obj["y"];
                std::string id;

                if (obj.contains("properties")) {
                    for (const auto &prop: obj["properties"]) {
                        std::string propName = prop["name"];
                        if (propName == "id") {
                            id = prop["value"];
                        }
                    }
                }
                AddSpawnPoint(id, Vector2(x, y));
            }
        }

        if (layer["name"] == "Enemies") {
            for (const auto &obj: layer["objects"]) {
                std::string name = obj["name"];
                int id = obj["id"];
                float x = obj["x"];
                float y = obj["y"];
                float MinPosX = 0;
                float MaxPosX = 0;
                float MinPosY = 0;
                float MaxPosY = 0;
                std::string condition;
                if (name == "Enemy Simple") {
                    auto* enemySimple = new EnemySimple(this);
                    enemySimple->SetPosition(Vector2(x, y));
                    enemySimple->SetId(id);
                }
                else if (name == "Flying Enemy") {
                    auto* flyingEnemySimple = new FlyingEnemySimple(this);
                    flyingEnemySimple->SetPosition(Vector2(x, y));
                    flyingEnemySimple->SetId(id);
                }
                else if (name == "FlyingShooterEnemy") {
                    auto* flyingShooterEnemy = new FlyingShooterEnemy(this);
                    flyingShooterEnemy->SetPosition(Vector2(x, y));
                    flyingShooterEnemy->SetId(id);
                }
                else if (name == "Mantis") {
                    auto* mantis = new Mantis(this);
                    mantis->SetPosition(Vector2(x, y));
                    mantis->SetId(id);
                }
                else if (name == "FlyingGolem") {
                    auto* flyingGolem = new FlyingGolem(this);
                    flyingGolem->SetPosition(Vector2(x, y));
                    flyingGolem->SetId(id);
                }
                else if (name == "DragonFly") {
                    auto* dragonFly = new DragonFly(this);
                    dragonFly->SetPosition(Vector2(x, y));
                    dragonFly->SetId(id);
                }
                else if (name == "FlyingSpawnerEnemy") {
                    auto* flyingSpawnerEnemy = new FlyingSpawnerEnemy(this);
                    flyingSpawnerEnemy->SetPosition(Vector2(x, y));
                    flyingSpawnerEnemy->SetId(id);
                }
                else if (name == "LittleBat") {
                    auto* littleBat = new LittleBat(this);
                    littleBat->SetPosition(Vector2(x, y));
                    littleBat->SetId(id);
                }
                else if (name == "Snake") {
                    auto* snake = new Snake(this);
                    snake->SetPosition(Vector2(x, y));
                    snake->SetId(id);
                }
                else if (name == "Mushroom") {
                    auto* mushroom = new Mushroom(this);
                    mushroom->SetPosition(Vector2(x, y));
                    mushroom->SetId(id);
                }
                else if (name == "OrangeSlime") {
                    auto* orangeSlime = new OrangeSlime(this);
                    orangeSlime->SetPosition(Vector2(x, y));
                    orangeSlime->SetId(id);
                }
                else if (name == "Bat") {
                    auto* bat = new Bat(this);
                    bat->SetPosition(Vector2(x, y));
                    bat->SetId(id);
                }
                else if (name == "LaserShooter") {
                    int direction = 0;
                    bool continuousShooting = true;
                    float cooldown = 2.0f;
                    float castDuration = 1.5f;
                    float shootDuration = 1.0f;
                    if (obj.contains("properties")) {
                        for (const auto &prop: obj["properties"]) {
                            std::string propName = prop["name"];
                            if (propName == "Direction") {
                                direction = prop["value"];
                            }
                            if (propName == "ContinuousShooting") {
                                continuousShooting = prop["value"];
                            }
                            if (propName == "Cooldown") {
                                cooldown = prop["value"];
                            }
                            if (propName == "CastDuration") {
                                castDuration = prop["value"];
                            }
                            if (propName == "ShootDuration") {
                                shootDuration = prop["value"];
                            }
                        }
                    }
                    auto* laserShooter = new LaserShooter(this);
                    laserShooter->SetPosition(Vector2(x, y));
                    laserShooter->SetId(id);
                    laserShooter->SetContinuousShooting(continuousShooting);
                    laserShooter->SetIdleDuration(cooldown);
                    laserShooter->SetCastDuration(castDuration);
                    laserShooter->SetShootDuration(shootDuration);
                    switch (direction) {
                        case 0:
                            laserShooter->SetRotation(3.0f * Math::PiOver2);
                            laserShooter->SetTransformRotation(3.0f * Math::PiOver2);
                        break;

                        case 1:
                            laserShooter->SetRotation(Math::PiOver2);
                            laserShooter->SetTransformRotation(Math::PiOver2);
                        break;

                        case 2:
                            laserShooter->SetRotation(Math::Pi);
                            laserShooter->SetScale(Vector2(-1, 1));
                        break;

                        case 3:
                            laserShooter->SetRotation(0.0f);
                            laserShooter->SetScale(Vector2(1, 1));
                        break;

                        default:
                            laserShooter->SetRotation(0.0f);
                        break;
                    }
                }
                else if (name == "CloneEnemy") {
                    auto* cloneEnemy = new CloneEnemy(this);
                    cloneEnemy->SetPosition(Vector2(x, y));
                    cloneEnemy->SetId(id);
                }
                else if (name == "Fox") {
                    if (obj.contains("properties")) {
                        for (const auto &prop: obj["properties"]) {
                            std::string propName = prop["name"];
                            if (propName == "Condition") {
                                condition = prop["value"];
                            }
                        }
                    }

                    if (!ShouldLoadObject(condition)) {
                        continue;
                    }

                    auto* fox = new Fox(this);
                    fox->SetPosition(Vector2(x, y));
                    fox->SetId(id);
                }
                else if (name == "Frog") {
                    if (obj.contains("properties")) {
                        for (const auto &prop: obj["properties"]) {
                            std::string propName = prop["name"];
                            if (propName == "MinPosX") {
                                MinPosX = static_cast<float>(prop["value"]);
                            }
                            else if (propName == "MaxPosX") {
                                MaxPosX = static_cast<float>(prop["value"]);
                            }
                            else if (propName == "MinPosY") {
                                MinPosY =static_cast<float>(prop["value"]);
                            }
                            else if (propName == "MaxPosY") {
                                MaxPosY = static_cast<float>(prop["value"]);
                            }
                            else if (propName == "Condition") {
                                condition = prop["value"];
                            }
                        }
                    }

                    if (!ShouldLoadObject(condition)) {
                        continue;
                    }

                    auto* frog = new Frog(this);
                    frog->SetPosition(Vector2(x, y));
                    frog->SetId(id);
                    frog->SetArenaMinPos(Vector2(MinPosX, MinPosY));
                    frog->SetArenaMaxPos(Vector2(MaxPosX, MaxPosY));
                }
                else if (name == "Frogger") {
                    if (obj.contains("properties")) {
                        for (const auto &prop: obj["properties"]) {
                            std::string propName = prop["name"];
                            if (propName == "MinPosX") {
                                MinPosX = static_cast<float>(prop["value"]);
                            }
                            else if (propName == "MaxPosX") {
                                MaxPosX = static_cast<float>(prop["value"]);
                            }
                            else if (propName == "MinPosY") {
                                MinPosY =static_cast<float>(prop["value"]);
                            }
                            else if (propName == "MaxPosY") {
                                MaxPosY = static_cast<float>(prop["value"]);
                            }
                            else if (propName == "Condition") {
                                condition = prop["value"];
                            }
                        }
                    }

                    if (!ShouldLoadObject(condition)) {
                        continue;
                    }

                    auto* frogger = new Frogger(this);
                    frogger->SetPosition(Vector2(x, y));
                    frogger->SetId(id);
                    frogger->SetArenaMinPos(Vector2(MinPosX, MinPosY));
                    frogger->SetArenaMaxPos(Vector2(MaxPosX, MaxPosY));
                }
                else if (name == "Moth") {
                    if (obj.contains("properties")) {
                        for (const auto &prop: obj["properties"]) {
                            std::string propName = prop["name"];
                            if (propName == "Condition") {
                                condition = prop["value"];
                            }
                        }
                    }

                    if (!ShouldLoadObject(condition)) {
                        continue;
                    }

                    auto* moth = new Moth(this);
                    moth->SetPosition(Vector2(x, y));
                    moth->SetId(id);
                }
                else if (name == "BushMonster") {
                    if (obj.contains("properties")) {
                        for (const auto &prop: obj["properties"]) {
                            std::string propName = prop["name"];
                            if (propName == "Condition") {
                                condition = prop["value"];
                            }
                        }
                    }

                    if (!ShouldLoadObject(condition)) {
                        continue;
                    }

                    auto* bushMonster = new BushMonster(this);
                    bushMonster->SetPosition(Vector2(x, y));
                    bushMonster->SetId(id);
                }
                else if (name == "Golem") {
                    if (obj.contains("properties")) {
                        for (const auto &prop: obj["properties"]) {
                            std::string propName = prop["name"];
                            if (propName == "MinPosX") {
                                MinPosX = static_cast<float>(prop["value"]);
                            }
                            else if (propName == "MaxPosX") {
                                MaxPosX = static_cast<float>(prop["value"]);
                            }
                            else if (propName == "MinPosY") {
                                MinPosY = static_cast<float>(prop["value"]);
                            }
                            else if (propName == "MaxPosY") {
                                MaxPosY = static_cast<float>(prop["value"]);
                            }
                            else if (propName == "Condition") {
                                condition = prop["value"];
                            }
                        }
                    }

                    if (!ShouldLoadObject(condition)) {
                        continue;
                    }

                    auto* golem = new Golem(this);
                    golem->SetPosition(Vector2(x, y));
                    golem->SetId(id);
                    golem->SetArenaMinPos(Vector2(MinPosX, MinPosY));
                    golem->SetArenaMaxPos(Vector2(MaxPosX, MaxPosY));
                }
                else if (name == "HookEnemy") {
                    if (obj.contains("properties")) {
                        for (const auto &prop: obj["properties"]) {
                            std::string propName = prop["name"];
                            if (propName == "Condition") {
                                condition = prop["value"];
                            }
                        }
                    }

                    if (!ShouldLoadObject(condition)) {
                        continue;
                    }

                    auto* hookEnemy = new HookEnemy(this);
                    hookEnemy->SetPosition(Vector2(x, y));
                    hookEnemy->SetId(id);
                }
                else if (name == "MirrorBoss") {
                    if (obj.contains("properties")) {
                        for (const auto &prop: obj["properties"]) {
                            std::string propName = prop["name"];
                            if (propName == "MinPosX") {
                                MinPosX = static_cast<float>(prop["value"]);
                            }
                            else if (propName == "MaxPosX") {
                                MaxPosX = static_cast<float>(prop["value"]);
                            }
                            else if (propName == "MinPosY") {
                                MinPosY = static_cast<float>(prop["value"]);
                            }
                            else if (propName == "MaxPosY") {
                                MaxPosY = static_cast<float>(prop["value"]);
                            }
                            else if (propName == "Condition") {
                                condition = prop["value"];
                            }
                        }
                    }

                    if (!ShouldLoadObject(condition)) {
                        continue;
                    }

                    auto* mirrorBoss = new MirrorBoss(this);
                    mirrorBoss->SetPosition(Vector2(x, y));
                    mirrorBoss->SetId(id);
                    mirrorBoss->SetArenaMinPos(Vector2(MinPosX, MinPosY));
                    mirrorBoss->SetArenaMaxPos(Vector2(MaxPosX, MaxPosY));
                }
                else if (name == "StoneGolem") {
                    std::string leftLaserShooters;
                    std::string rightLaserShooters;
                    std::string topLaserShooters;
                    if (obj.contains("properties")) {
                        for (const auto &prop: obj["properties"]) {
                            std::string propName = prop["name"];
                            if (propName == "MinPosX") {
                                MinPosX = static_cast<float>(prop["value"]);
                            }
                            else if (propName == "MaxPosX") {
                                MaxPosX = static_cast<float>(prop["value"]);
                            }
                            else if (propName == "MinPosY") {
                                MinPosY = static_cast<float>(prop["value"]);
                            }
                            else if (propName == "MaxPosY") {
                                MaxPosY = static_cast<float>(prop["value"]);
                            }
                            if (propName == "Condition") {
                                condition = prop["value"];
                            }
                            if (propName == "LeftLaserShooters") {
                                leftLaserShooters = prop["value"];
                            }
                            if (propName == "RightLaserShooters") {
                                rightLaserShooters = prop["value"];
                            }
                            if (propName == "TopLaserShooters") {
                                topLaserShooters = prop["value"];
                            }
                        }
                    }

                    if (!ShouldLoadObject(condition)) {
                        continue;
                    }

                    auto* stoneGolem = new StoneGolem(this);
                    stoneGolem->SetPosition(Vector2(x, y));
                    stoneGolem->SetId(id);
                    stoneGolem->SetArenaMinPos(Vector2(MinPosX, MinPosY));
                    stoneGolem->SetArenaMaxPos(Vector2(MaxPosX, MaxPosY));
                    stoneGolem->SetLeftLaserShooters(ParseIntList(leftLaserShooters));
                    stoneGolem->SetRightLaserShooters(ParseIntList(rightLaserShooters));
                    stoneGolem->SetTopLaserShooters(ParseIntList(topLaserShooters));
                }
            }
        }
        if (layer["name"] == "Checkpoint") {
            for (const auto &obj: layer["objects"]) {
                float x = obj["x"];
                float y = obj["y"];
                float width = obj["width"];
                float height = obj["height"];
                Vector2 cameraPosition(Vector2::Zero);

                if (obj.contains("properties")) {
                    for (const auto &prop: obj["properties"]) {
                        std::string propName = prop["name"];
                        if (propName == "CameraPositionX") {
                            cameraPosition.x = prop["value"];
                        }
                        if (propName == "CameraPositionY") {
                            cameraPosition.y = prop["value"];
                        }
                    }
                }
                auto checkpoint = new Checkpoint(this, width, height, Vector2(x + width / 2, y + height / 2));
            }
        }
        if (layer["name"] == "AmbientParticleArea") {
            for (const auto &obj: layer["objects"]) {
                float x = obj["x"];
                float y = obj["y"];
                float width = obj["width"];
                float height = obj["height"];
                float emitRate = 3;
                Vector3 color = Color::White;

                if (obj.contains("properties")) {
                    for (const auto &prop: obj["properties"]) {
                        std::string propName = prop["name"];
                        if (propName == "EmitRate") {
                            emitRate = prop["value"];
                        }
                        if (propName == "ColorR") {
                            color.x = prop["value"];
                        }
                        if (propName == "ColorG") {
                            color.y = prop["value"];
                        }
                        if (propName == "ColorB") {
                            color.z = prop["value"];
                        }
                    }
                }

                auto ambientParticleArea = new AmbientParticleArea(this, width, height, emitRate, color);
                ambientParticleArea->SetPosition(Vector2(x + width / 2, y + height / 2));
            }
        }
        if (layer["name"] == "Player") {
            for (const auto &obj: layer["objects"]) {
                float x = obj["x"];
                float y = obj["y"];
                int playerStartPositionId = 0;
                Vector2 enteringLevelVelocity(Vector2::Zero);
                float enteringOffset = 0;

                if (obj.contains("properties")) {
                    for (const auto &prop: obj["properties"]) {
                        std::string propName = prop["name"];
                        if (propName == "StartPositionId") {
                            playerStartPositionId = prop["value"];
                        }
                        if (propName == "EnteringLevelSpeedX") {
                            enteringLevelVelocity.x = static_cast<float>(prop["value"]);
                        }
                        if (propName == "EnteringLevelSpeedY") {
                            enteringLevelVelocity.y = static_cast<float>(prop["value"]);
                        }
                        if (propName == "EnteringOffset") {
                            enteringOffset = prop["value"];
                        }
                    }
                }

                if (playerStartPositionId != mPlayerStartPositionId) {
                    continue;
                }

                if (mPlayer) {
                    mPlayer->SetSword();
                    mPlayer->SetJumpEffects();
                    mPlayer->SetIsGrassParticle(mUseGrassParticle);
                    mPlayer->InitLight();
                    mPlayer->GetComponent<DashComponent>()->InitDashEffect();
                    mPlayer->GetComponent<RigidBodyComponent>()->SetVelocity(Vector2::Zero);
                    mPlayer->SetIsDead(false);
                    mPlayer->SetState(ActorState::Active);
                    mPlayer->SetInvertControls(false);
                    mPlayer->Stop();
                    mPlayer->ResetCooldown();
                    mPlayer->GetComponent<AABBComponent>()->SetActive(true);
                    mPlayer->GetComponent<CombatBoxComponent>()->SetAllBoxesActive(true);
                    if (mGoingToNextLevel) {
                        mPlayer->SetPosition(Vector2(x, y));
                        mPlayer->SetIsEnteringLevel(enteringLevelVelocity, enteringOffset);

                        // Salva jogo
                        SaveGame();

                        mGoingToNextLevel = false;
                    }
                    else {
                        mPlayer->SetPosition(mCheckpointPosition);

                        // Salva jogo
                        SaveGame();
                    }

                    // Faz isso para o player ser sempre o último a ser atualizado a cada frame
                    RemoveActor(mPlayer);
                    AddActor(mPlayer);
                }
                else {
                    mPlayer = new Player(this);
                    mSaveData->ApplyToPlayer();
                    mCheckPointMoney = mPlayer->GetMoney();
                    mPlayer->SetIsGrassParticle(mUseGrassParticle);
                    // Começa jogo com 80% da vida
                    if (mSaveData->GetTotalPlayTime() == 0.0f) {
                        mPlayer->SetHealthPoints(mPlayer->GetMaxHealthPoints() * 0.8f);
                    }
                }
                // LÓGICA DO MAPA
                if (mMapManager && mNextLevelPath != "MainMenu") {
                    // Monta o caminho exato do PNG
                    std::string mapImgPath = "../Assets/Levels/" + mNextLevelPath + "Map.png";

                    Vector2 mapMaxBound(mLevelWidth * mTileSize, mLevelHeight * mTileSize);

                    if (mIsConnectingMapRoom) {
                        // Acha o trigger mais perto de onde o jogador acabou de nascer
                        Vector2 destTriggerPos = Vector2::Zero;
                        float closestDist = 9999999.0f;

                        for (Trigger* t : mTriggers) {
                            if (t->GetEvent() == Trigger::Event::ChangeScene) {
                                float dist = (t->GetPosition() - mPlayer->GetPosition()).LengthSq();
                                if (dist < closestDist) {
                                    closestDist = dist;
                                    destTriggerPos = t->GetPosition();
                                }
                            }
                        }

                        mMapManager->LoadConnectedRoom(
                            mNextLevelPath, mapImgPath, 0.25f,
                            Vector2::Zero, mapMaxBound, destTriggerPos,
                            mLastMapOriginCanvasPos, Vector2::Zero, mLastMapOriginBoundsMax, mLastMapOriginTriggerPos
                        );

                        mIsConnectingMapRoom = false; // Reset da transição
                    } else {
                        // Se não está transicionando (New Game ou Checkpoint), carrega como inicial.
                        mMapManager->LoadInitialRoom(mNextLevelPath, mapImgPath, 0.25f, Vector2::Zero, mapMaxBound);
                    }

                    // Bake the map global
                    mMapManager->BuildGlobalCanvas();
                }
            }
            mPlayerStartPositionId = 0;
        }
    }
}

void Game::LoadLevel(const std::string &fileName, const nlohmann::json& mapData, bool hasTileSet) {
    ClearTriggers();
    // Extrai o diretório base do arquivo de mapa atual
    std::string baseDirectory = "";
    size_t lastSlashPos = fileName.find_last_of("/\\");
    if (lastSlashPos != std::string::npos) {
        baseDirectory = fileName.substr(0, lastSlashPos + 1);
    }

    // Lê altura, largura e tileSize
    int height = int(mapData["height"]);
    int width = int(mapData["width"]);
    float tileSize = static_cast<float>(mapData["tilewidth"]);
    mLevelHeight = height;
    mLevelWidth = width;
    mTileSize = tileSize;
    mOriginalTileSize = tileSize;

    mDecorationsFirstGid = 1; // Reseta para o padrão

    // Variáveis para guardar os caminhos relativos lidos do Tiled
    std::string mainTilesetSource = "";
    std::string decorationsSource = "";

    if (mapData.contains("tilesets")) {
        for (const auto& ts : mapData["tilesets"]) {
            std::string source = ts["source"];
            // Se o arquivo .tsx conter "Decorations", salvamos o firstgid e o source dele
            if (source.find("Decorations") != std::string::npos) {
                mDecorationsFirstGid = ts["firstgid"];
                decorationsSource = source;
            } else if (mainTilesetSource.empty()) {
                // Assumimos que o primeiro tileset encontrado (que não seja de decorations) é o principal
                mainTilesetSource = source;
            }
        }
    }

    std::string newDecorationsPath = "";
    std::string newDecorationsJson = "";
    std::string newDecorationsGidToName = "";

    // Define os caminhos das decorações dinamicamente baseados no source do .tsx
    if (!decorationsSource.empty()) {
        size_t extPos = decorationsSource.rfind(".tsx");
        std::string sourceNoExt = (extPos != std::string::npos) ? decorationsSource.substr(0, extPos) : decorationsSource;

        newDecorationsPath = baseDirectory + sourceNoExt + ".png";
        newDecorationsJson = baseDirectory + sourceNoExt + ".json";
        newDecorationsGidToName = baseDirectory + sourceNoExt + "GidToName.json";

        // Normaliza a string
        newDecorationsPath = std::filesystem::path(newDecorationsPath).lexically_normal().string();
        newDecorationsJson = std::filesystem::path(newDecorationsJson).lexically_normal().string();
        newDecorationsGidToName = std::filesystem::path(newDecorationsGidToName).lexically_normal().string();
    }
    // Fallback: Mantém a lógica de properties para compatibilidade com mapas antigos
    else if (mapData.contains("properties")) {
        for (const auto &prop: mapData["properties"]) {
            std::string propName = prop["name"];
            if (propName == "DecorationsPath") {
                newDecorationsPath = "../Assets/" + prop["value"].get<std::string>() + ".png";
                newDecorationsJson = "../Assets/" + prop["value"].get<std::string>() + ".json";
                newDecorationsGidToName = "../Assets/" + prop["value"].get<std::string>() + "GidToName.json";

                // Normaliza a string
                newDecorationsPath = std::filesystem::path(newDecorationsPath).lexically_normal().string();
                newDecorationsJson = std::filesystem::path(newDecorationsJson).lexically_normal().string();
                newDecorationsGidToName = std::filesystem::path(newDecorationsGidToName).lexically_normal().string();
            }
        }
    }

    // Lê matrizes de tiles
    for (const auto& layer : mapData["layers"]) {
        if (layer["name"] == "Camada de Blocos 1") {
            std::vector<int> data = layer["data"];
            int** matrix = new int*[height];
            for (int i = 0; i < height; ++i) {
                matrix[i] = new int[width];
                for (int j = 0; j < width; ++j) {
                    matrix[i][j] = data[i * width + j];
                }
            }
            mLevelData = matrix;
        } else if (layer["name"] == "DynamicGrounds") {
            std::vector<int> data = layer["data"];
            int** matrix = new int*[height];
            for (int i = 0; i < height; ++i) {
                matrix[i] = new int[width];
                for (int j = 0; j < width; ++j) {
                    matrix[i][j] = data[i * width + j];
                }
            }
            mLevelDataDynamicGrounds = matrix;
        }
    }

    // Load tilesheet texture
    if (hasTileSet && !mainTilesetSource.empty()) {
        // Monta o caminho do Tileset Principal usando o caminho lido do Tiled
        size_t extPos = mainTilesetSource.rfind(".tsx");
        std::string sourceNoExt = (extPos != std::string::npos) ? mainTilesetSource.substr(0, extPos) : mainTilesetSource;

        std::string newTileSheetTexturePath = baseDirectory + sourceNoExt + ".png";

        // Normaliza a string (ex: "Assets/Levels/Room2/../../Tilesets/Mossy.png" vira "Assets/Tilesets/Mossy.png")
        std::string normalizedNewTileSheetTexturePath = std::filesystem::path(newTileSheetTexturePath).lexically_normal().string();

        // Verifica se o Tileset mudou
        if (normalizedNewTileSheetTexturePath != mCurrentTileSheetPath)
        {
            // Limpa os dados antigos, pois o mapa mudou
            mTileSheetData.clear();
            mCurrentTileSheetPath = normalizedNewTileSheetTexturePath;

            mTileSheet = mRenderer->GetTexture(mCurrentTileSheetPath);

            // Load novo tilesheet data concatenando "TileSet.json" no final do nome base
            std::string tileSheetDataPath = baseDirectory + sourceNoExt + ".json";

            std::ifstream tileSheetFile(tileSheetDataPath);
            if (!tileSheetFile.is_open()) {
                SDL_Log("Erro ao abrir JSON do TileSet Principal: %s", tileSheetDataPath.c_str());
            } else {
                nlohmann::json tileSheetData = nlohmann::json::parse(tileSheetFile);

                int textureWidth = mTileSheet->GetWidth();
                int textureHeight = mTileSheet->GetHeight();

                for (const auto &tile: tileSheetData["sprites"]) {
                    std::string tileFileName = tile["fileName"];
                    int x = tile["x"].get<int>();
                    int y = tile["y"].get<int>();
                    int w = tile["width"].get<int>();
                    int h = tile["height"].get<int>();

                    size_t dotPos = tileFileName.find('.');
                    std::string numberStr = tileFileName.substr(0, dotPos);
                    int index = std::stoi(numberStr);

                    // Normaliza para [0, 1]
                    float u = static_cast<float>(x) / textureWidth;
                    float v = static_cast<float>(y) / textureHeight;
                    float uw = static_cast<float>(w) / textureWidth;
                    float vh = static_cast<float>(h) / textureHeight;

                    mTileSheetData[index] = Vector4(u, v, uw, vh);
                }
            }
        }
        else
        {
            mTileSheet = mRenderer->GetTexture(mCurrentTileSheetPath);
        }
    }

    if (!newDecorationsPath.empty() && newDecorationsPath != mCurrentDecorationsPath) {
        mDecorationsTileSheetData.clear();
        mDecorationsName.clear();
        mCurrentDecorationsPath = newDecorationsPath;

        mDecorationsTileSheet = mRenderer->GetTexture(mCurrentDecorationsPath);

        std::ifstream spriteSheetFile(newDecorationsJson);
        if (spriteSheetFile.is_open()) {
            nlohmann::json spriteSheetData = nlohmann::json::parse(spriteSheetFile);

            auto textureWidth1 = static_cast<float>(spriteSheetData["meta"]["size"]["w"].get<int>());
            auto textureHeight1 = static_cast<float>(spriteSheetData["meta"]["size"]["h"].get<int>());

            for(const auto& frame : spriteSheetData["frames"]) {
                std::string filename = frame["filename"];
                // Remove a extensão ".png"
                size_t lastindex = filename.find_last_of(".");
                if (lastindex != std::string::npos) {
                    filename = filename.substr(0, lastindex);
                }
                int x = frame["frame"]["x"].get<int>();
                int y = frame["frame"]["y"].get<int>();
                int w = frame["frame"]["w"].get<int>();
                int h = frame["frame"]["h"].get<int>();

                mDecorationsTileSheetData.emplace_back(static_cast<float>(x)/textureWidth1, static_cast<float>(y)/textureHeight1,
                                              static_cast<float>(w)/textureWidth1, static_cast<float>(h)/textureHeight1);

                mDecorationsName.emplace_back(filename);
            }
        } else {
            SDL_Log("Erro ao abrir JSON da Decoracao: %s", newDecorationsJson.c_str());
        }

        std::ifstream mapFile(newDecorationsGidToName);
        if (mapFile.is_open()) {
            nlohmann::json gidMapJson = nlohmann::json::parse(mapFile);

            for (auto& [key, value] : gidMapJson.items()) {
                mGidToDecorationName[std::stoi(key)] = value;
            }
        }
    }
    else if (!newDecorationsPath.empty()) {
        mDecorationsTileSheet = mRenderer->GetTexture(mCurrentDecorationsPath);
    }

    // Cria objetos
    LoadObjects(mapData);
}

const std::string& Game::GetDecorationNameFromGid(int gid) const {
    auto it = mGidToDecorationName.find(gid);
    if (it != mGidToDecorationName.end()) {
        return it->second;
    }
    static std::string empty = "";
    return empty;
}

bool Game::ShouldLoadObject(const std::string &condition) {
    if (condition.empty()) return true;

    bool negate = condition[0] == '!';
    std::string flag = negate ? condition.substr(1) : condition;

    bool value = mWorldState[flag];
    return negate ? !value : value;
}

SDL_Color Game::HexToColor(std::string hex) {
    SDL_Color color = {58, 147, 89, 255};

    // Remove o símbolo '#'
    if (!hex.empty() && hex[0] == '#') {
        hex.erase(0, 1);
    }

    uint32_t hexValue = std::stoul(hex, nullptr, 16);

    // Extrai os canais dependendo do tamanho da string
    if (hex.length() == 8) {
        // Formato ARGB (ex: ff3a9359)
        color.a = (hexValue >> 24) & 0xFF;
        color.r = (hexValue >> 16) & 0xFF;
        color.g = (hexValue >> 8)  & 0xFF;
        color.b = hexValue         & 0xFF;
    }
    else if (hex.length() == 6) {
        // Formato RGB padrão (ex: 3a9359)
        color.r = (hexValue >> 16) & 0xFF;
        color.g = (hexValue >> 8)  & 0xFF;
        color.b = hexValue         & 0xFF;
        color.a = 255;
    }

    return color;
}

void Game::RunLoop()
{
    while (mIsRunning)
    {
        ProcessInput();
        UpdateGame();
        GenerateOutput();
    }
}

void Game::ProcessInput()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_QUIT:
                Quit();
                break;

            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED ||
                    event.window.event == SDL_WINDOWEVENT_RESIZED) 
                {
                    mWindowWidth = event.window.data1;
                    mWindowHeight = event.window.data2;

                    // Pega o NOVO tamanho em PIXELS
                    int newWidth, newHeight;
                    SDL_GL_GetDrawableSize(mWindow, &newWidth, &newHeight);

                    if (mRenderer) {
                        mRenderer->OnWindowResize(static_cast<float>(newWidth), 
                                                static_cast<float>(newHeight));
                    }

                    mLogicalWindowWidth = mRenderer->GetVirtualWidth();
                    mLogicalWindowHeight = mRenderer->GetVirtualHeight();
                }
                break;

            case SDL_KEYDOWN:
                if (mWaitingForKey) {
                    SDL_Scancode sc = event.key.keysym.scancode;

                    if (sc == SDL_SCANCODE_ESCAPE) {
                        CancelRebind();
                        break;
                    }

                    SwapKeyboardBinding(sc, 0);

                    if (!mUIStack.empty()) {
                        mUIStack.back()->RefreshTexts();
                    }

                    // sai do modo de captura
                    mWaitingForKey = false;
                    mNewButtonText = nullptr;
                }
                else if (mWaitingForButton) {
                    CancelRebind();
                    break;
                }

                else if (mGamePlayState != GamePlayState::GameOver) {
                    mInputPlayerMode = InputPlayerMode::Keyboard;
                    // Handle key press for UI screens
                    if (!mUIStack.empty()) {
                        mUIStack.back()->HandleKeyPress(event.key.keysym.sym, SDL_CONTROLLER_BUTTON_INVALID, 0, 0, 0, 0);
                    }

                    if (event.key.keysym.sym == SDLK_ESCAPE) {
                        if (mCurrentLevelPath != "MainMenu" &&
                            mGamePlayState != GamePlayState::Cutscene)
                        {
                            if (mIsPaused) {
                                for (auto iter = mUIStack.rbegin(); iter != mUIStack.rend(); ++iter) {
                                    if ((*iter)->IsClosable() && (*iter)->GetState() != UIScreen::UIState::Closing) {
                                        (*iter)->Close();
                                        break;
                                    }
                                }
                            }
                            else {
                                if (mIsPlayingAdvancedMode) {
                                    mPauseMenu = new PauseAdvancedMenu(this, "../Assets/Fonts/K2D-Bold.ttf");
                                }
                                else {
                                    mPauseMenu = new PauseMenu(this, "../Assets/Fonts/K2D-Bold.ttf");
                                }
                            }
                        }
                        else if (mCurrentLevelPath == "MainMenu") {
                            for (auto iter = mUIStack.rbegin(); iter != mUIStack.rend(); ++iter) {
                                if ((*iter)->IsClosable() && (*iter)->GetState() != UIScreen::UIState::Closing) {
                                    (*iter)->Close();
                                    break;
                                }
                            }
                        }
                    }

                    if (SDL_GetScancodeFromKey(event.key.keysym.sym) == mInputBindings[Action::Map].key) {
                        if (mCurrentLevelPath != "MainMenu" &&
                            mGamePlayState != GamePlayState::Cutscene)
                        {
                            if (mIsPaused) {
                                for (auto iter = mUIStack.rbegin(); iter != mUIStack.rend(); ++iter) {
                                    if ((*iter)->IsClosable() && (*iter)->GetState() != UIScreen::UIState::Closing) {
                                        (*iter)->Close();
                                        break;
                                    }
                                }
                            }
                            else {
                                mMapMenu = new MapMenu(this, "../Assets/Fonts/K2D-Bold.ttf");
                            }
                        }
                    }

                    // if (event.key.keysym.sym == SDLK_8) {
                    //     Quit();
                    // }
                    //
                    // if (event.key.keysym.sym == SDLK_5) {
                    //     mIsSlowMotion = !mIsSlowMotion;
                    //     mIsAccelerated = false;
                    // }
                    //
                    // if (event.key.keysym.sym == SDLK_6) {
                    //     mIsAccelerated = !mIsAccelerated;
                    //     mIsSlowMotion = false;
                    // }
                    //
                    // if (event.key.keysym.sym == SDLK_1) {
                    //     // mTargetZoom = 1.5f;
                    //     mTargetZoom += 0.05f;
                    // }
                    // if (event.key.keysym.sym == SDLK_2) {
                    //     // mTargetZoom = 0.5f;
                    //     mTargetZoom -= 0.2f;
                    // }
                    // if (event.key.keysym.sym == SDLK_3) {
                    //     mTargetZoom = 1.0f;
                    // }
                }
                break;

            case SDL_KEYUP:
                if (!mUIStack.empty()) {
                    mUIStack.back()->HandleKeyRelease(event.key.keysym.sym, SDL_CONTROLLER_BUTTON_INVALID);
                }
                break;

            case SDL_CONTROLLERBUTTONDOWN: {
                SDL_JoystickID instanceID = event.cbutton.which;
                if (mControllers.find(instanceID) != mControllers.end()) {
                    if (mCurrentController != mControllers[instanceID]) {
                        mCurrentController = mControllers[instanceID];
                        DetectTControllerType();
                    }
                }
                if (mWaitingForButton) {
                    auto button = event.cbutton.button;
                    if (!(button == SDL_CONTROLLER_BUTTON_A ||
                        button == SDL_CONTROLLER_BUTTON_B ||
                        button == SDL_CONTROLLER_BUTTON_X ||
                        button == SDL_CONTROLLER_BUTTON_Y ||
                        button == SDL_CONTROLLER_BUTTON_LEFTSHOULDER ||
                        button == SDL_CONTROLLER_BUTTON_RIGHTSHOULDER))
                    {
                        CancelRebind();
                        break;
                    }

                    SwapControllerBinding(static_cast<SDL_GameControllerButton>(button), SDL_CONTROLLER_AXIS_INVALID);

                    if (!mUIStack.empty()) {
                        mUIStack.back()->RefreshTexts();
                    }

                    mWaitingForButton = false;
                    mNewButtonText = nullptr;
                }
                else if (mWaitingForKey) {
                    CancelRebind();
                    break;
                }

                else if (mGamePlayState != GamePlayState::GameOver) {
                    mInputPlayerMode = InputPlayerMode::Controller;

                    // Handle key press for UI screens
                    if (!mUIStack.empty()) {
                        mUIStack.back()->HandleKeyPress(-1, event.cbutton.button, 0, 0, 0, 0);
                    }

                    if (event.cbutton.button == SDL_CONTROLLER_BUTTON_START) {
                        if (mCurrentLevelPath != "MainMenu" &&
                            mGamePlayState != GamePlayState::Cutscene)
                        {
                            if (mIsPaused) {
                                if (mUIStack.back() == mPauseMenu) {
                                    mPauseMenu->Close();
                                }
                            }
                            else {
                                if (mIsPlayingAdvancedMode) {
                                    mPauseMenu = new PauseAdvancedMenu(this, "../Assets/Fonts/K2D-Bold.ttf");
                                }
                                else {
                                    mPauseMenu = new PauseMenu(this, "../Assets/Fonts/K2D-Bold.ttf");
                                }
                            }
                        }
                    }

                    // Apertar B para sair dos menus
                    if (event.cbutton.button == SDL_CONTROLLER_BUTTON_B) {
                        if (mCurrentLevelPath != "MainMenu" &&
                            mGamePlayState != GamePlayState::Cutscene)
                        {
                            if (mIsPaused) {
                                for (auto iter = mUIStack.rbegin(); iter != mUIStack.rend(); ++iter) {
                                    if ((*iter)->IsClosable() && (*iter)->GetState() != UIScreen::UIState::Closing) {
                                        (*iter)->Close();
                                        break;
                                    }
                                }
                            }
                        }
                        else if (mCurrentLevelPath == "MainMenu") {
                            for (auto iter = mUIStack.rbegin(); iter != mUIStack.rend(); ++iter) {
                                if ((*iter)->IsClosable() && (*iter)->GetState() != UIScreen::UIState::Closing) {
                                    (*iter)->Close();
                                    break;
                                }
                            }
                        }
                    }

                    if (event.cbutton.button == mInputBindings[Action::Map].btn) {
                        if (mCurrentLevelPath != "MainMenu" &&
                            mGamePlayState != GamePlayState::Cutscene)
                        {
                            if (mIsPaused) {
                                if (mUIStack.back() == mMapMenu) {
                                    mMapMenu->Close();
                                }
                            }
                            else {
                                mMapMenu = new MapMenu(this, "../Assets/Fonts/K2D-Bold.ttf");
                            }
                        }
                    }
                }
                break;
            }

            case SDL_CONTROLLERBUTTONUP:
                if (!mUIStack.empty()) {
                    mUIStack.back()->HandleKeyRelease(-1, event.cbutton.button);
                }
                break;

            case SDL_CONTROLLERAXISMOTION: {
                if (Math::Abs(event.caxis.value) > DEAD_ZONE) {
                    SDL_JoystickID instanceID = event.caxis.which;
                    if (mControllers.find(instanceID) != mControllers.end()) {
                        if (mCurrentController != mControllers[instanceID]) {
                            mCurrentController = mControllers[instanceID];
                            DetectTControllerType();
                        }
                    }
                }
                if (event.caxis.axis == SDL_CONTROLLER_AXIS_RIGHTX) {
                    mRawRightAxisX = event.caxis.value;
                }
                if (event.caxis.axis == SDL_CONTROLLER_AXIS_RIGHTY) {
                    mRawRightAxisY = event.caxis.value;
                }
                if (mWaitingForButton) {
                    // Verifica se é o gatilho esquerdo ou direito
                    if (event.caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERLEFT ||
                        event.caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERRIGHT)
                    {
                        if (event.caxis.value > DEAD_ZONE) {
                            SwapControllerBinding(SDL_CONTROLLER_BUTTON_INVALID, static_cast<SDL_GameControllerAxis>(event.caxis.axis));

                            if (!mUIStack.empty()) {
                                mUIStack.back()->RefreshTexts();
                            }

                            mWaitingForButton = false;
                            mNewButtonText = nullptr;
                        }
                    }
                }
                else if (mWaitingForKey) {
                    if (Math::Abs(event.caxis.value) > DEAD_ZONE) {
                        CancelRebind();
                    }
                    break;
                }

                else if (mGamePlayState != GamePlayState::GameOver) {
                    if (Math::Abs(event.caxis.value) > DEAD_ZONE) {
                        mInputPlayerMode = InputPlayerMode::Controller;
                    }

                    if (!mUIStack.empty()) {
                        if (event.caxis.axis == SDL_CONTROLLER_AXIS_LEFTY) {
                            int valueY = event.caxis.value;

                            if (valueY < -DEAD_ZONE) {
                                if (mLeftStickStateY != StickState::Up) {
                                    mLeftStickStateY = StickState::Up;
                                    if (!mUIStack.empty()) {
                                        mUIStack.back()->HandleKeyPress(-1, SDL_CONTROLLER_BUTTON_INVALID, valueY, 0, 0, 0);
                                    }
                                }
                            }
                            else if (valueY > DEAD_ZONE) {
                                if (mLeftStickStateY != StickState::Down) {
                                    mLeftStickStateY = StickState::Down;
                                    if (!mUIStack.empty()) {
                                        mUIStack.back()->HandleKeyPress(-1, SDL_CONTROLLER_BUTTON_INVALID, valueY, 0, 0, 0);
                                    }
                                }
                            }
                            else {
                                // Voltou à zona morta
                                if (mLeftStickStateY != StickState::Neutral) {
                                    UIScreen::NavDirection releasedDir = (mLeftStickStateY == StickState::Up) ? UIScreen::NavDirection::Up : UIScreen::NavDirection::Down;

                                    mLeftStickStateY = StickState::Neutral;

                                    if (!mUIStack.empty()) {
                                        mUIStack.back()->CancelDirectionalHold(releasedDir);
                                    }
                                }
                            }
                        }

                        if (event.caxis.axis == SDL_CONTROLLER_AXIS_LEFTX) {
                            int valueX = event.caxis.value;

                            if (valueX < -DEAD_ZONE) {
                                if (mLeftStickStateX != StickState::Left) {
                                    mLeftStickStateX = StickState::Left;
                                    if (!mUIStack.empty()) {
                                        mUIStack.back()->HandleKeyPress(-1, SDL_CONTROLLER_BUTTON_INVALID, 0, valueX, 0, 0);
                                    }
                                }
                            }
                            else if (valueX > DEAD_ZONE) {
                                if (mLeftStickStateX != StickState::Right) {
                                    mLeftStickStateX = StickState::Right;
                                    if (!mUIStack.empty()) {
                                        mUIStack.back()->HandleKeyPress(-1, SDL_CONTROLLER_BUTTON_INVALID, 0, valueX, 0, 0);
                                    }
                                }
                            }
                            else {
                                // Voltou à zona morta
                                if (mLeftStickStateX != StickState::Neutral) {
                                    UIScreen::NavDirection releasedDir = (mLeftStickStateX == StickState::Left) ? UIScreen::NavDirection::Left : UIScreen::NavDirection::Right;

                                    mLeftStickStateX = StickState::Neutral;

                                    if (!mUIStack.empty()) {
                                        mUIStack.back()->CancelDirectionalHold(releasedDir);
                                    }
                                }
                            }
                        }
                        if (event.caxis.axis == SDL_CONTROLLER_AXIS_RIGHTY) {
                            int valueY = event.caxis.value;

                            if (valueY < -DEAD_ZONE) {
                                if (mRightStickStateY != StickState::Up) {
                                    mRightStickStateY = StickState::Up;
                                    if (!mUIStack.empty()) {
                                        mUIStack.back()->HandleKeyPress(-1, SDL_CONTROLLER_BUTTON_INVALID, 0, 0, valueY, 0);
                                    }
                                }
                            }
                            else if (valueY > DEAD_ZONE) {
                                if (mRightStickStateY != StickState::Down) {
                                    mRightStickStateY = StickState::Down;
                                    if (!mUIStack.empty()) {
                                        mUIStack.back()->HandleKeyPress(-1, SDL_CONTROLLER_BUTTON_INVALID, 0, 0, valueY, 0);
                                    }
                                }
                            }
                            else {
                                // Voltou à zona morta
                                if (mRightStickStateY != StickState::Neutral) {
                                    UIScreen::NavDirection releasedDir = (mRightStickStateY == StickState::Up) ? UIScreen::NavDirection::Up : UIScreen::NavDirection::Down;

                                    mRightStickStateY = StickState::Neutral;

                                    if (!mUIStack.empty()) {
                                        mUIStack.back()->CancelDirectionalHold(releasedDir);
                                    }
                                }
                            }
                        }

                        if (event.caxis.axis == SDL_CONTROLLER_AXIS_RIGHTX) {
                            int valueX = event.caxis.value;

                            if (valueX < -DEAD_ZONE) {
                                if (mRightStickStateX != StickState::Left) {
                                    mRightStickStateX = StickState::Left;
                                    if (!mUIStack.empty()) {
                                        mUIStack.back()->HandleKeyPress(-1, SDL_CONTROLLER_BUTTON_INVALID, 0, 0, 0, valueX);
                                    }
                                }
                            }
                            else if (valueX > DEAD_ZONE) {
                                if (mRightStickStateX != StickState::Right) {
                                    mRightStickStateX = StickState::Right;
                                    if (!mUIStack.empty()) {
                                        mUIStack.back()->HandleKeyPress(-1, SDL_CONTROLLER_BUTTON_INVALID, 0, 0, 0, valueX);
                                    }
                                }
                            }
                            else {
                                // Voltou à zona morta
                                if (mRightStickStateX != StickState::Neutral) {
                                    UIScreen::NavDirection releasedDir = (mRightStickStateX == StickState::Left) ? UIScreen::NavDirection::Left : UIScreen::NavDirection::Right;

                                    mRightStickStateX = StickState::Neutral;

                                    if (!mUIStack.empty()) {
                                        mUIStack.back()->CancelDirectionalHold(releasedDir);
                                    }
                                }
                            }
                        }
                    }
                }
                break;
            }

            case SDL_MOUSEBUTTONDOWN:
                if (mWaitingForKey) {
                    Uint8 btn = event.button.button;

                    SwapKeyboardBinding(SDL_SCANCODE_UNKNOWN, btn);

                    if (!mUIStack.empty()) {
                        mUIStack.back()->RefreshTexts();
                    }
                    mWaitingForKey = false;
                    mNewButtonText = nullptr;
                }
                else if (mWaitingForButton) {
                    CancelRebind();
                    break;
                }

                else if (mGamePlayState != GamePlayState::GameOver) {
                    mInputPlayerMode = InputPlayerMode::Mouse;

                    // Handle mouse for UI screens
                    if (!mUIStack.empty()) {
                        // Obter coordenadas da tela (física)
                        Vector2 screenPos(static_cast<float>(event.button.x), static_cast<float>(event.button.y));

                        // Converter para coordenadas virtuais
                        Vector2 virtualPos = mRenderer->ScreenToVirtual(screenPos);

                        // Passar as coordenadas limpas para a UI
                        mUIStack.back()->HandleMousePress(virtualPos);
                    }
                }
                break;

            case SDL_MOUSEBUTTONUP:
                if (mGamePlayState != GamePlayState::GameOver) {
                    // Handle mouse release for UI screens
                    if (!mUIStack.empty()) {
                        Vector2 screenPos(static_cast<float>(event.button.x), static_cast<float>(event.button.y));
                        Vector2 virtualPos = mRenderer->ScreenToVirtual(screenPos);

                        // Avisa a tela atual que o mouse foi solto
                        mUIStack.back()->HandleMouseRelease(virtualPos);
                    }
                }
                break;

            case SDL_MOUSEMOTION:
                if (mWaitingForKey || mWaitingForButton) {
                    break;
                }
                mInputPlayerMode = InputPlayerMode::Mouse;

                // Handle mouse for UI screens
                if (!mUIStack.empty()) {
                    // Obter coordenadas da tela (física)
                    Vector2 screenPos(static_cast<float>(event.motion.x), static_cast<float>(event.motion.y));

                    // Converter para coordenadas virtuais
                    Vector2 virtualPos = mRenderer->ScreenToVirtual(screenPos);

                    // Passar as coordenadas limpas para a UI
                    mUIStack.back()->HandleMouseMotion(virtualPos);
                }
                break;

            case SDL_CONTROLLERDEVICEADDED: {
                SDL_GameController* newCtrl = SDL_GameControllerOpen(event.cdevice.which);
                if (newCtrl) {
                    SDL_JoystickID id = SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(newCtrl));
                    mControllers[id] = newCtrl;

                    if (!mCurrentController) {
                        mCurrentController = newCtrl;
                    }
                    DetectTControllerType();
                }
                break;
            }

            case SDL_CONTROLLERDEVICEREMOVED: {
                SDL_JoystickID id = event.cdevice.which;
                auto it = mControllers.find(id);

                if (it != mControllers.end()) {
                    if (mCurrentController == it->second) {
                        mCurrentController = nullptr;
                    }

                    SDL_GameControllerClose(it->second);
                    mControllers.erase(it);

                    if (!mCurrentController && !mControllers.empty()) {
                        mCurrentController = mControllers.begin()->second;
                        DetectTControllerType();
                    }
                }
                break;
            }

            default:
                break;
        }
    }

    const Uint8* state = SDL_GetKeyboardState(nullptr);

    if (mGamePlayState == GamePlayState::Playing &&
        !mGoingToNextLevel) {
        if (!mIsPaused) {
            if (mHitstopActive) {}
            else {
                for (auto actor: mActors) {
                    actor->ProcessInput(state, *mCurrentController);
                }
            }
        }
    }
}

void Game::TogglePause() {
    if (mCurrentLevelPath != "MainMenu" &&
        mGamePlayState != GamePlayState::Cutscene)
    {
        mIsPaused = !mIsPaused;
        if (mIsPaused) {
            // if (mAudio->GetSoundState(mMusicHandle) == SoundState::Playing) {
            //     mAudio->PauseSound(mMusicHandle);
            // }
            // if (mAudio->GetSoundState(mBossMusic) == SoundState::Playing) {
            //     mAudio->PauseSound(mBossMusic);
            // }
            mRenderer->SetEffectIntensity(PostProcessEffect::Blur, 2.0f);
            mAudio->SetCategoryModifier(SoundCategory::Music, mPauseMusicVolumeScale);
            mGamePlayState = GamePlayState::Paused;
        }
        else {
            // if (mAudio->GetSoundState(mBossMusic) == SoundState::Paused) {
            //     mAudio->ResumeSound(mBossMusic);
            // }
            // else if (mAudio->GetSoundState(mMusicHandle) == SoundState::Paused) {
            //     mAudio->ResumeSound(mMusicHandle);
            // }
            mRenderer->SetEffectIntensity(PostProcessEffect::Blur, 0.0f);
            mAudio->SetCategoryModifier(SoundCategory::Music, 1.0f);
            mGamePlayState = GamePlayState::Playing;
        }
        mPlayer->SetCanJump(false);
        mPlayer->SetPrevSkill1Pressed(true);
    }
}


void Game::UpdateGame()
{
    // while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 1000.0 / mFPS));

    const auto frameDuration = static_cast<Uint32>(1000.0f / mFPS);
    Uint32 now = SDL_GetTicks();
    if (now < mTicksCount + frameDuration)
        SDL_Delay((mTicksCount + frameDuration) - now);

    float deltaTime = static_cast<float>(SDL_GetTicks() - mTicksCount) / 1000.0f;
    if (deltaTime > 0.05f) {
        deltaTime = 0.05f;
    }

    mTicksCount = SDL_GetTicks();

    if (mGamePlayState != GamePlayState::Paused && mGamePlayState != GamePlayState::Menu) {
        mTotalPlayTime += deltaTime;
    }

    // testes para alterar velocidade do jogo
    if (mIsSlowMotion) {
        deltaTime *= 0.3;
    }
    if (mIsAccelerated) {
        deltaTime *= 1.5;
    }

    // SDL_SetRenderDrawColor(mRenderer, 0, 0, 0, 255); // Usado para deixar as bordas em preto
    // SDL_RenderClear(mRenderer);

    // Update all actors and pending actors
    if (!mIsPaused) {
        if (mHitstopDelayActive) {
            if (mHitstopDelayTimer < mHitstopDelayDuration) {
                mHitstopDelayTimer += deltaTime;
            }
            else {
                mHitstopDelayActive = false;
                mHitstopActive = true;
            }
        }
        if (mHitstopActive) {
            if (mHitstopTimer < mHitstopDuration) {
                mHitstopTimer += deltaTime;
            }
            else {
                mHitstopActive = false;
            }
        }
        else {
            UpdateActors(deltaTime);
            if (mWaveManager) {
                mWaveManager->Update(deltaTime);
            }
            // if (mHUD) {
            //     mHUD->Update(deltaTime);
            // }
        }

        if (mDamageFlashActive) {
            if (mDamageFlashTimer < mDamageFlashDuration) {
                mDamageFlashTimer += deltaTime;

                float progress = mDamageFlashTimer / mDamageFlashDuration;

                float maxIntensity = 0.5f;

                float intensity = maxIntensity * (1.0f - progress);

                mRenderer->SetEffectIntensity(PostProcessEffect::DamageFlash, intensity);
            }
            else {
                mRenderer->SetEffectIntensity(PostProcessEffect::DamageFlash, 0.0f);
                mDamageFlashActive = false;
            }
        }
    }

    if (mIsPlayingFinalCutscene) {
        PlayFinalGoodCutscene();
    }

    // Update cutscene
    if (mCurrentCutscene) {
        mCurrentCutscene->Update(deltaTime);
        if (mCurrentCutscene->IsComplete()) {
            delete mCurrentCutscene;          // libera memória manualmente
            mCurrentCutscene = nullptr;       // evita dangling pointer
            SetGamePlayState(Game::GamePlayState::Playing);
        }
    }

    if (mGamePlayState == GamePlayState::Cutscene || mGamePlayState == GamePlayState::GameOver) {
        if (mHUD) {
            mHUD->SetIsVisible(false);
        }
    }
    else {
        if (mHUD) {
            mHUD->SetIsVisible(true);
        }
    }

    mAudio->Update(deltaTime);

    // Reinsert UI screens
    for (auto ui : mUIStack) {
        // if (ui != mHUD) {
            if (ui->GetState() == UIScreen::UIState::Active) {
                ui->Update(deltaTime);
            }
        // }
    }

    // Delete any UIElements that are closed
    auto iter = mUIStack.begin();
    while (iter != mUIStack.end()) {
        if ((*iter)->GetState() == UIScreen::UIState::Closing) {
            delete *iter;
            *iter = nullptr;
            iter = mUIStack.erase(iter);
        } else {
            ++iter;
        }
    }

    UIScreen* currentTopScreen = mUIStack.empty() ? nullptr : mUIStack.back();

    if (currentTopScreen != nullptr) {
        currentTopScreen->SetIsVisible(true);

        // Se a tela do topo for diferente da que estava no último frame
        if (currentTopScreen != mLastTopUIScreen && mInputPlayerMode == InputPlayerMode::Mouse) {
            int mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);

            Vector2 screenPos(static_cast<float>(mouseX), static_cast<float>(mouseY));
            Vector2 virtualPos = mRenderer->ScreenToVirtual(screenPos);

            currentTopScreen->HandleMouseMotion(virtualPos);
        }
    }

    // Atualiza a variável para a checagem no próximo frame
    mLastTopUIScreen = currentTopScreen;

    if (mInputPlayerMode == InputPlayerMode::Keyboard || mInputPlayerMode == InputPlayerMode::Controller ||
        mGamePlayState == GamePlayState::Playing ||
        mGamePlayState == GamePlayState::LevelComplete ||
        mGamePlayState == GamePlayState::Cutscene ||
        mGamePlayState == GamePlayState::GameOver)
    {
        // Esconde o cursor
        SDL_ShowCursor(SDL_DISABLE);
        SDL_SetWindowMouseGrab(mWindow, SDL_TRUE);
    }
    else {
        SDL_ShowCursor(SDL_ENABLE);
        SDL_SetWindowMouseGrab(mWindow, SDL_FALSE);
    }

    if (mMapManager && mCurrentLevelPath != "MainMenu" && mGamePlayState == GamePlayState::Playing) {
        mMapManager->UpdateFogOfWar(mCurrentLevelPath, mPlayer->GetPosition(), mBrushRadius);
    }

    if (mIsCrossFading) {
        if (mCrossFadeTimer < mCrossFadeDuration) {
            if (mCrossFadeTimer < mCrossFadeDuration * 0.4f) {
                float progress = mCrossFadeTimer / (mCrossFadeDuration * 0.4f);
                mFadeAlpha = static_cast<Uint8>(progress * 255.0f);
            }
            else if (mCrossFadeTimer >= mCrossFadeDuration * 0.4f && mCrossFadeTimer < mCrossFadeDuration * 0.6f) {
                mFadeAlpha = 255;
                if (mHitByLava) {
                    mPlayer->SetPosition(mLavaRespawnPosition);
                    mPlayer->SetState(ActorState::Active);
                    mPlayer->GetComponent<AABBComponent>()->SetActive(true);
                    mPlayer->GetComponent<CombatBoxComponent>()->SetAllBoxesActive(true);
                }
            }
            else if (mCrossFadeTimer >= mCrossFadeDuration * 0.6f) {
                float progress = 1 - (mCrossFadeTimer - (mCrossFadeDuration * 0.6f)) / (mCrossFadeDuration * 0.4f);
                mFadeAlpha = static_cast<Uint8>(progress * 255.0f);
            }
            mCrossFadeTimer += deltaTime;
        }
        else {
            mIsCrossFading = false;
            if (mHitByLava) {
                mPlayer->SetState(ActorState::Active);
                mPlayer->GetComponent<AABBComponent>()->SetActive(true);
                mHitByLava = false;
            }
        }
    }

    if (mBackToCheckpoint) {
        LoadNextLevel(mCheckpointGameScenePath, 1.5f);
        mPlayer->ResetHealthPoints();
        mPlayer->ResetMana();
        mPlayer->ResetHealCount();
        SaveGame();
        mBackToCheckpoint = false;
    }

    // Zoom
    if (mZoom != mTargetZoom) {
        if (std::abs(mTargetZoom - mZoom) > 0.005f)
        {
            mZoom = mZoom + (mTargetZoom - mZoom) * mZoomSpeed * deltaTime;

            if (std::abs(mTargetZoom - mZoom) < 0.005f) {
                mZoom = mTargetZoom;
            }
            if (mCamera) {
                mCamera->SetZoom(mZoom);
            }
        }
    }

    UpdateCamera(deltaTime);

    UpdateSceneManager(deltaTime);
}

void Game::UpdateSceneManager(float deltaTime)
{
    if (mSceneManagerState == SceneManagerState::Entering) {
        mSceneManagerTimer -= deltaTime;

        // Cálculo proporcional da opacidade
        if (mSceneManagerTimer <= mFadeDuration) {
            float progress = 1.0f - (mSceneManagerTimer / mFadeDuration);
            mFadeAlpha = static_cast<Uint8>(progress * 255.0f);
        }

        if (mSceneManagerTimer <= 0.0f) {
            // mSceneManagerTimer = TRANSITION_TIME;  // Reinicia timer para próxima fase
            mSceneManagerTimer = mTransitionTime;  // Reinicia timer para próxima fase
            mSceneManagerState = SceneManagerState::Active;
            mFadeAlpha = 0;
        }
    }

    if (mSceneManagerState == SceneManagerState::Active) {
        mSceneManagerTimer -= deltaTime;
        if (mSceneManagerTimer <= 0.0f) {
            ChangeScene();  // Realiza a troca de cena
            mSceneManagerTimer = mFadeDuration;
            mSceneManagerState = SceneManagerState::Exiting;
        }
    }

    if (mSceneManagerState == SceneManagerState::Exiting) {
        mSceneManagerTimer -= deltaTime;

        // Cálculo proporcional da opacidade
        float progress = mSceneManagerTimer / mFadeDuration;
        mFadeAlpha = static_cast<Uint8>(progress * 255.0f);

        if (mSceneManagerTimer <= 0.0f) {
            // mSceneManagerTimer = TRANSITION_TIME;  // Reinicia timer para próxima fase
            mSceneManagerTimer = mTransitionTime;  // Reinicia timer para próxima fase
            mSceneManagerState = SceneManagerState::None;
            mFadeAlpha = 0;
        }
    }
}

void Game::UpdateActors(float deltaTime)
{
    mUpdatingActors = true;
    for (auto actor: mActors)
        actor->Update(deltaTime);

    mUpdatingActors = false;

    for (auto pending: mPendingActors)
        mActors.emplace_back(pending);

    mPendingActors.clear();

    std::vector<Actor* > deadActors;
    for (auto actor: mActors)
        if (actor->GetState() == ActorState::Destroy)
            deadActors.emplace_back(actor);

    for (auto actor: deadActors)
        delete actor;
}

void Game::UpdateCamera(float deltaTime) {
    if (!mCamera) {
        return;
    }
    mCamera->Update(deltaTime);
}

void Game::PushUI(class UIScreen *screen) {
    if (!mUIStack.empty()) {
        mUIStack.back()->ResetInputState();
    }

    mUIStack.push_back(screen);
}

void Game::ActiveDamageFlash() {
    mDamageFlashActive = true;
    mDamageFlashTimer = 0.0f;
}

void Game::AddGround(class Ground* g) { mGrounds.emplace_back(g); }

void Game::RemoveGround(class Ground* g) {
    auto iter = std::find(mGrounds.begin(), mGrounds.end(), g);
    if (iter != mGrounds.end()) {
        mGrounds.erase(iter);
    }
}

Ground* Game::GetGroundById(int id) {
    for (Ground* g : mGrounds) {
        if (g->GetId() == id) {
            return g;
        }
    }
    return nullptr;
}

void Game::AddFireBall(class FireBall* f) { mFireBalls.emplace_back(f); }

void Game::RemoveFireball(class FireBall* f) {
    auto iter = std::find(mFireBalls.begin(), mFireBalls.end(), f);
    if (iter != mFireBalls.end()) {
        mFireBalls.erase(iter);
    }
}

void Game::AddParticle(class Particle* p) { mParticles.emplace_back(p); }

void Game::RemoveParticle(class Particle* p) {
    auto iter = std::find(mParticles.begin(), mParticles.end(), p);
    if (iter != mParticles.end()) {
        mParticles.erase(iter);
    }
}

void Game::AddProjectile(class Projectile* p) { mProjectiles.emplace_back(p); }

void Game::RemoveProjectile(class Projectile* p) {
    auto iter = std::find(mProjectiles.begin(), mProjectiles.end(), p);
    if (iter != mProjectiles.end()) {
        mProjectiles.erase(iter);
    }
}

void Game::AddMoney(class Money* m) { mMoneys.emplace_back(m); }

void Game::RemoveMoney(class Money* m) {
    auto iter = std::find(mMoneys.begin(), mMoneys.end(), m);
    if (iter != mMoneys.end()) {
        mMoneys.erase(iter);
    }
}

void Game::AddHookPoint(class HookPoint* hp) { mHookPoints.emplace_back(hp); }

void Game::RemoveHookPoint(class HookPoint* hp) {
    auto iter = std::find(mHookPoints.begin(), mHookPoints.end(), hp);
    if (iter != mHookPoints.end()) {
        mHookPoints.erase(iter);
    }
}

void Game::AddLaserShooter(class LaserShooter* ls) { mLaserShooters.emplace_back(ls); }

void Game::RemoveLaserShooter(class LaserShooter* ls) {
    auto iter = std::find(mLaserShooters.begin(), mLaserShooters.end(), ls);
    if (iter != mLaserShooters.end()) {
        mLaserShooters.erase(iter);
    }
}

LaserShooter* Game::GetLaserShooterById(int id) {
    for (LaserShooter* ls : mLaserShooters) {
        if (ls->GetId() == id) {
            return ls;
        }
    }
    return nullptr;
}

void Game::AddSpawnPoint(const std::string &id, const Vector2 &pos) {
    mSpawnPoints[id] = pos;
}

Vector2 Game::GetSpawnPointPosition(const std::string &id) const {
    auto it = mSpawnPoints.find(id);
    if (it != mSpawnPoints.end()) {
        return it->second;
    }
    throw std::runtime_error("SpawnPoint não encontrado: " + id);
}

std::vector<Vector2> Game::GetSpawnPointsPositions() {
    std::vector<Vector2> positions;

    for (const auto& par : mSpawnPoints) {
        positions.emplace_back(par.second);
    }

    return positions;
}

void Game::CreateWaveManager(const std::string &wavesFilePath, const std::string &arenaCondition) {
    if (!mWaveManager) {
        mWaveManager = new WaveManager(this);
        mWaveManager->LoadFromJson(wavesFilePath);
        mWaveManager->SetArenaCondition(arenaCondition);
        mWaveManager->Start();
    }
}


void Game::AddEnemy(class Enemy* e) { mEnemies.emplace_back(e); }

void Game::RemoveEnemy(class Enemy* e) {
    auto iter = std::find(mEnemies.begin(), mEnemies.end(), e);
    if (iter != mEnemies.end()) {
        mEnemies.erase(iter);
    }
}

Enemy* Game::GetEnemyById(int id) {
    for (Enemy* e : mEnemies) {
        if (e->GetId() == id) {
            return e;
        }
    }
    return nullptr;
}

void Game::AddTrigger(class Trigger* t) { mTriggers.emplace_back(t); }

void Game::RemoveTrigger(class Trigger* t) {
    auto iter = std::find(mTriggers.begin(), mTriggers.end(), t);
    if (iter != mTriggers.end()) {
        mTriggers.erase(iter);
    }
}

void Game::AddActor(Actor* actor) {
    if (mUpdatingActors) {
        mPendingActors.emplace_back(actor);
    }
    else {
        mActors.emplace_back(actor);
    }
}

void Game::RemoveActor(Actor* actor) {
    auto iter = std::find(mPendingActors.begin(), mPendingActors.end(), actor);
    if (iter != mPendingActors.end()) {
        // Swap to end of vector and pop off (avoid erase copies)
        std::iter_swap(iter, mPendingActors.end() - 1);
        mPendingActors.pop_back();
    }

    iter = std::find(mActors.begin(), mActors.end(), actor);
    if (iter != mActors.end()) {
        // Swap to end of vector and pop off (avoid erase copies)
        std::iter_swap(iter, mActors.end() - 1);
        mActors.pop_back();
    }
}

void Game::AddDrawable(class DrawComponent *drawable)
{
    mDrawables.emplace_back(drawable);

    SortDrawables();
}

void Game::RemoveDrawable(class DrawComponent *drawable)
{
    auto iter = std::find(mDrawables.begin(), mDrawables.end(), drawable);
    mDrawables.erase(iter);
}

void Game::SortDrawables() {
    std::stable_sort(mDrawables.begin(), mDrawables.end(),[](DrawComponent* a, DrawComponent* b) {
    return a->GetDrawOrder() < b->GetDrawOrder();
    });
}

void Game::AddCollider(class ColliderComponent *collider)
{
    mColliders.emplace_back(collider);
}

void Game::RemoveCollider(class ColliderComponent *collider) {
    auto iter = std::find(mColliders.begin(), mColliders.end(), collider);
    mColliders.erase(iter);
}

void Game::InitCrossFade(float duration) {
    mIsCrossFading = true;
    mCrossFadeTimer = 0;
    mCrossFadeDuration = duration;
}


void Game::StartBossMusic(SoundHandle music) {
    mBossMusic = music;
    mAudio->PauseSound(mMusicHandle);
}

void Game::StopBossMusic() {
    mAudio->StopSound(mBossMusic);
    mBossMusic.Reset();
    mAudio->ResumeSound(mMusicHandle);
}

void Game::SaveGame() {
    mSaveData->CaptureFromGame();
    mSaveManager->SaveGame(mSaveData, mSaveSlot);
}

void Game::LoadGame() {
    mSaveData = mSaveManager->LoadGame(mSaveSlot);
    mSaveData->ApplyToGame();
    mSaveData->ApplyWorldState();
    mSaveData->ApplyConfigs();
}

void Game::LoadAdvancedModeGame(const std::string &gameScenePath, Vector2 lastCheckpointPosition) {
    mSaveData = mSaveManager->LoadGame(mSaveSlot);
    mSaveData->SetGameScenePath(gameScenePath);
    mSaveData->SetLastCheckpointPosition(lastCheckpointPosition);

    // if (mGameScene != GameScene::MainMenu) {
    //     SaveGame();
    // }

    mSaveData->ApplyToGame();

    mSaveData->ApplyWorldState();
    mSaveData->ApplyConfigs();
}

void Game::SetWorldFlag(const std::string &key, bool value) {
    mWorldState[key] = value;
}

bool Game::GetWorldFlag(const std::string &key) const {
    auto it = mWorldState.find(key);
    if (it != mWorldState.end())
        return it->second;
    return false; // padrão se não existe
}

void Game::DetectTControllerType() {
    // Retorna o tipo de controle mapeado pelo SDL
    SDL_GameControllerType tipo = SDL_GameControllerGetType(mCurrentController);

    switch (tipo) {
        case SDL_CONTROLLER_TYPE_XBOX360:
        case SDL_CONTROLLER_TYPE_XBOXONE:
            mInputController = InputController::Xbox;
            break;

        case SDL_CONTROLLER_TYPE_PS3:
        case SDL_CONTROLLER_TYPE_PS4:
        case SDL_CONTROLLER_TYPE_PS5:
            mInputController = InputController::Playstation;
            break;

        default:
            mInputController = InputController::Xbox;
            break;
    }
}

void Game::PlayFinalGoodCutscene() {
    if (mCutsceneIndex > 0) {
        mPlayer->SetPosition(Vector2(-100, -100));
    }
    if (mCutsceneIndex < mGoodCutscenes.size()) {
        mGamePlayState = GamePlayState::Cutscene;
        if (mCurrentCutscene == nullptr) {
            if (mGameScene != mGoodCutsceneScenes[mCutsceneIndex]) {
                // SetGameScene(mGoodCutsceneScenes[mCutsceneIndex], 1.5f);
            }
            else {
                mCurrentCutscene = new Cutscene(this, mGoodCutscenes[mCutsceneIndex], "../Assets/Cutscenes/Cutscenes.json");
                mCutsceneIndex++;
            }
        }
    }
    else {
        if (mCurrentCutscene == nullptr) {
            // SetGameScene(GameScene::MainMenu, 1.5f);
            // mGoingToNextLevel = true;
            mIsPlayingFinalCutscene = false;
        }
    }
}

void Game::PlayFinalEvilCutscene() {

}

std::string Game::ActionToString(Action action) {
    switch (action) {
        case Action::Up:         return "Up";
        case Action::Down:       return "Down";
        case Action::MoveLeft:   return "MoveLeft";
        case Action::MoveRight:  return "MoveRight";
        case Action::Jump:       return "Jump";
        case Action::Attack:     return "Attack";
        case Action::Dash:       return "Dash";
        case Action::Skill1:     return "Skill1";
        case Action::Skill2:     return "Skill2";
        case Action::Heal:       return "Heal";
        case Action::Hook:       return "Hook";
        case Action::OpenStore:  return "OpenStore";
        case Action::Map:        return "Map";
        case Action::Look:       return "Look";
        case Action::ChangeMode: return "ChangeMode";
        case Action::Pause:      return "Pause";
        default:                 return "Unknown";
    }
}

Game::Action Game::StringToAction(const std::string &str) {
    if (str == "Up")         return Action::Up;
    if (str == "Down")       return Action::Down;
    if (str == "MoveLeft")   return Action::MoveLeft;
    if (str == "MoveRight")  return Action::MoveRight;
    if (str == "Jump")       return Action::Jump;
    if (str == "Attack")     return Action::Attack;
    if (str == "Dash")       return Action::Dash;
    if (str == "Skill1")     return Action::Skill1;
    if (str == "Skill2")     return Action::Skill2;
    if (str == "Heal")       return Action::Heal;
    if (str == "Hook")       return Action::Hook;
    if (str == "OpenStore")  return Action::OpenStore;
    if (str == "Map")        return Action::Map;
    if (str == "Look")       return Action::Look;
    if (str == "ChangeMode") return Action::ChangeMode;
    if (str == "Pause")      return Action::Pause;

    return Action::Invalid;
}

void Game::SaveBindingsToFile(const std::string &filename) {
    nlohmann::json j;

    for (const auto& pair : mInputBindings) {
        Action action = pair.first;
        InputBinding binding = pair.second;

        // Cria um objeto JSON para o binding atual
        nlohmann::json bindingJson;
        bindingJson["key"] = static_cast<int>(binding.key);
        bindingJson["mouse"] = static_cast<int>(binding.mouseButton);
        bindingJson["btn"] = static_cast<int>(binding.btn);
        bindingJson["axis"] = static_cast<int>(binding.axis);

        // Usa a string da Action como chave no JSON principal
        j[ActionToString(action)] = bindingJson;
    }

    // Abre o arquivo e escreve o JSON formatado
    std::ofstream file(filename);
    if (file.is_open()) {
        // j.dump(4) formata o JSON com 4 espaços de indentação para ficar legível
        file << j.dump(4);
        file.close();
    } else {
        std::cerr << "Erro ao abrir o arquivo para salvar os controles: " << filename << std::endl;
    }
}

void Game::LoadBindingsFromFile(const std::string &filename) {
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Arquivo de controles '" << filename << "' nao encontrado. Criando controles padrao." << std::endl;

        // --- Crie aqui seus controles padrão ---
        mInputBindings[Action::MoveLeft]  = {SDL_SCANCODE_A, 0, SDL_CONTROLLER_BUTTON_DPAD_LEFT, SDL_CONTROLLER_AXIS_INVALID};
        mInputBindings[Action::MoveRight] = {SDL_SCANCODE_D, 0, SDL_CONTROLLER_BUTTON_DPAD_RIGHT, SDL_CONTROLLER_AXIS_INVALID};
        mInputBindings[Action::Jump]      = {SDL_SCANCODE_SPACE, 0, SDL_CONTROLLER_BUTTON_A, SDL_CONTROLLER_AXIS_INVALID};
        mInputBindings[Action::Attack]    = {SDL_SCANCODE_J, 0, SDL_CONTROLLER_BUTTON_X, SDL_CONTROLLER_AXIS_INVALID};
        // ... adicione todos os outros padrões

        // Salva os padrões para que o arquivo exista na próxima vez
        SaveBindingsToFile(filename);
        return;
    }

    try {
        nlohmann::json j;
        file >> j;

        mInputBindings.clear(); // Limpa os bindings antigos antes de carregar os novos

        // Itera sobre todos os elementos do JSON
        for (auto& [key_str, value] : j.items()) {
            Action action = StringToAction(key_str);

            SDL_Scancode scancode = SDL_SCANCODE_UNKNOWN;
            Uint8 mouseBtn = 0;
            SDL_GameControllerButton button = SDL_CONTROLLER_BUTTON_INVALID;
            SDL_GameControllerAxis axis = SDL_CONTROLLER_AXIS_INVALID;

            // Verifica se as chaves "key" e "btn" existem antes de tentar acessá-las
            if (value.contains("key") && value["key"].is_number()) {
                scancode = static_cast<SDL_Scancode>(value["key"].get<int>());
            }
            if (value.contains("mouse") && value["mouse"].is_number()) {
                mouseBtn = static_cast<Uint8>(value["mouse"].get<int>());
            }
            if (value.contains("btn") && value["btn"].is_number()) {
                button = static_cast<SDL_GameControllerButton>(value["btn"].get<int>());
            }
            if (value.contains("axis") && value["axis"].is_number()) {
                axis = static_cast<SDL_GameControllerAxis>(value["axis"].get<int>());
            }

            mInputBindings[action] = {scancode, mouseBtn, button, axis};
        }

    } catch (nlohmann::json::parse_error& e) {
        std::cerr << "Erro de parse no JSON: " << e.what() << '\n'
                  << "ID da excecao: " << e.id << '\n'
                  << "Byte do erro: " << e.byte << std::endl;
    }
}

bool Game::IsActionPressed(Action action, const Uint8 *keyboardState, SDL_GameController *controller) {
    auto& binding = mInputBindings[action];

    // Teclado
    if (binding.key != SDL_SCANCODE_UNKNOWN && keyboardState[binding.key]) return true;

    // Mouse
    if (binding.mouseButton != 0) {
        Uint32 mouseState = SDL_GetMouseState(NULL, NULL);
        // O macro SDL_BUTTON retorna uma máscara de bit para o botão pressionado
        if (mouseState & SDL_BUTTON(binding.mouseButton)) {
            return true;
        }
    }

    // Controle
    if (controller) {
        // Checa Botão Tradicional
        if (binding.btn != SDL_CONTROLLER_BUTTON_INVALID) {
            if (SDL_GameControllerGetButton(controller, binding.btn)) return true;
        }

        // Checa Gatilho (Eixo)
        if (binding.axis != SDL_CONTROLLER_AXIS_INVALID) {
            Sint16 axisValue = SDL_GameControllerGetAxis(controller, binding.axis);
            // Se o gatilho for pressionado mais que o threshold
            if (axisValue > DEAD_ZONE) return true;
        }
    }

    return false;
}

void Game::GenerateOutput()
{
    // Clear back buffer
    mRenderer->Clear();

    mRenderer->BeginGameDraw();

    if (mCamera) {
        if (mUseParallaxBackground) {
            DrawParallaxLayers(mBackgroundLayers);
        }
        else {
            // Vector2 size(3858, 800);
            // Vector2 position(1800 + size.x / 2, 3584 + size.y / 2);
            Vector2 size(1920, 1080);
            Vector2 position(2790, 2742);
            // Vector2 size(2000, 2000);
            // Vector2 position(2000, 2500);
            // DrawParallaxBackground(mBackGroundTexture); // desenha o fundo com repetição horizontal
            mRenderer->DrawTexture(position,
                       size, 0.0f, Color::White,
                       mBackGroundTexture, Vector4::UnitRect, mCamera->GetPosCamera());
        }
        //     // Ordem de desenho: mais distantes primeiro
        //     // DrawParallaxLayer(mSky,        0.1f, 0, mWindowHeight / 2);  // camada mais distante
        //     // DrawParallaxLayer(mMountains,  0.3f, mWindowHeight / 4, mWindowHeight / 3);  // montanhas ao fundo
        //     // DrawParallaxLayer(mTreesBack,  0.5f, mWindowHeight / 3, mWindowHeight / 2);  // árvores distantes
        //     // DrawParallaxLayer(mTreesFront, 0.7f, mWindowHeight / 2, mWindowHeight / 2);  // árvores próximas
    }
    else {
        // mRenderer->DrawTexture(Vector2(mRenderer->GetZoomedWidth() / 2, mRenderer->GetZoomedHeight() / 2),
        //                        Vector2(mRenderer->GetZoomedWidth(), mRenderer->GetZoomedHeight()), 0.0f, Color::White,
        //                        mBackGroundTexture, Vector4::UnitRect);
    }

    for (auto drawable: mDrawables) {
        drawable->Draw(mRenderer);
    }

    //Desenha Debug de colliders
    if (mCamera) {
        for (auto collider: mColliders) {
            collider->Draw(mRenderer);
        }
    }

    // Draw all UI screens
    mRenderer->BeginUIDraw();
    for (auto ui :mUIStack)
    {
        ui->Draw(mRenderer);
    }

    if (mSceneManagerState == SceneManagerState::Entering ||
        mSceneManagerState == SceneManagerState::Exiting ||
        mIsCrossFading)
    {
        mRenderer->DrawFade(mFadeAlpha / 255.0f);
    }
    else if (mSceneManagerState == SceneManagerState::Active)
    {
        mRenderer->DrawFade(1.0f);
    }

    // Swap front buffer and back buffer
    mRenderer->Present();
}

// SDL_Texture* Game::LoadTexture(const std::string &texturePath)
// {
//     SDL_Surface* surface = IMG_Load(texturePath.c_str());
//     if (!surface)
//     {
//         SDL_Log("Falha ao carregar imagem %s: %s", texturePath.c_str(), IMG_GetError());
//         return nullptr;
//     }
//
//     SDL_Texture* texture = SDL_CreateTextureFromSurface(mRenderer, surface);
//     SDL_FreeSurface(surface); // Libera a superfície, já não é mais necessária
//     surface = nullptr;
//
//     if (!texture)
//     {
//         SDL_Log("Falha ao criar textura a partir de %s: %s", texturePath.c_str(), SDL_GetError());
//         return nullptr;
//     }
//     return texture;
// }

UIFont* Game::LoadFont(const std::string& fileName)
{
    auto iter = mFonts.find(fileName);
    if (iter != mFonts.end()) {
        return iter->second;
    }

    // Fonte ainda não carregada, cria nova instância
    UIFont* font = new UIFont(mRenderer);
    if (font->Load(fileName)) {
        mFonts.emplace(fileName, font);
        return font;
    }
    else {
        // Falha no carregamento — limpa e retorna nullptr
        font->Unload();
        delete font;
        SDL_Log("Falha ao carregar fonte: %s", fileName.c_str());
        return nullptr;
    }
}

void Game::UnloadScene()
{
    if (mGamePlayState != GamePlayState::Cutscene) {
        mGamePlayState = GamePlayState::GameOver;
    }

    if (mPlayer) {
        mPlayer->SetState(ActorState::Paused);
    }

    for (auto it = mActors.begin(); it != mActors.end(); ) {
        Actor* actor = *it;
        if (actor != mPlayer) {
            it = mActors.erase(it);
            delete actor;
        } else {
            ++it;
        }
    }

    // Delete UI screens
    for (auto ui : mUIStack) {
        delete ui;
    }
    mUIStack.clear();
    mHUD = nullptr;

    // Delete level data
    if (mLevelData != nullptr) {
        for (int i = 0; i < mLevelHeight; ++i) {
            if (mLevelData[i] != nullptr)
                delete[] mLevelData[i];
        }
    }
    delete[] mLevelData;
    mLevelData = nullptr;

    // Delete level data Dynamic Grounds
    if (mLevelDataDynamicGrounds != nullptr) {
        for (int i = 0; i < mLevelHeight; ++i) {
            if (mLevelDataDynamicGrounds[i] != nullptr)
                delete[] mLevelDataDynamicGrounds[i];
        }
    }
    delete[] mLevelDataDynamicGrounds;
    mLevelDataDynamicGrounds = nullptr;

    // mTileSheetData.clear();
    // mDecorationsTileSheetData.clear();

    mSpawnPoints.clear();

    if (mBackGroundTexture) {
        mBackGroundTexture = nullptr;
    }

    std::vector<std::string> texturesToKeep;
    texturesToKeep.push_back("../Assets/Sprites/EsquiloFire/Esquilo.png"); // Mantém o Player

    // Se já carregamos um mapa antes, mantém a textura dele
    if (!mCurrentTileSheetPath.empty()) {
        texturesToKeep.push_back(mCurrentTileSheetPath);
    }
    if (!mCurrentDecorationsPath.empty()) {
        texturesToKeep.push_back(mCurrentDecorationsPath);
    }

    // Limpa todas as texturas, exceto as que estão na lista
    // mRenderer->UnloadUnusedTextures(texturesToKeep);
    // mRenderer->PrintUsedTextures();

    mRenderer->ClearLights();

    mBackgroundLayers.clear();

    delete mCamera;
    mCamera = nullptr;

    delete mWaveManager;
    mWaveManager = nullptr;
}

void Game::Shutdown()
{
    delete mPlayer;
    mPlayer = nullptr;

    delete mSkillTreeManager;
    mSkillTreeManager = nullptr;

    delete mMapManager;

    UnloadScene();

    for (auto font : mFonts) {
        font.second->Unload();
        delete font.second;
    }

    for (auto& pair : mControllers) {
        if (pair.second) {
            SDL_GameControllerClose(pair.second);
        }
    }

    mControllers.clear();
    mCurrentController = nullptr;

    mFonts.clear();

    // Destroi audio
    delete mAudio;
    mAudio = nullptr;

    // Destroy Save Data
    delete mSaveData;
    delete mSaveManager;

    Mix_CloseAudio();

    Mix_Quit();
    TTF_Quit();
    IMG_Quit();

    mRenderer->Shutdown();
    delete mRenderer;
    mRenderer = nullptr;

    SDL_DestroyWindow(mWindow);
    SDL_Quit();
}

void Game::DrawParallaxBackground(Texture* background)
{
    float parallaxFactor = 0.55f; // fundo se move mais devagar que a câmera

    int bgWidth = background->GetWidth();
    int bgHeight = background->GetHeight();

    // Calcula o offset horizontal com base na câmera
    int offsetX = static_cast<int>(mCamera->GetPosCamera().x * parallaxFactor) % bgWidth;
    if (offsetX < 0) offsetX += bgWidth;

    // Desenha blocos horizontais suficientes para cobrir a largura da janela
    for (int x = -offsetX; x < mLogicalWindowWidth; x += bgWidth)
    {
        Vector2 position(x + bgWidth / 2, mLogicalWindowHeight / 2);
        Vector2 size(bgWidth, static_cast<int>(mLogicalWindowHeight));

        // Desenho via renderer — sem influência da câmera (passamos offset manualmente)
        mRenderer->DrawTexture(
            position,          // posição
            size,              // tamanho
            0.0f,              // sem rotação
            Vector3(1.0f, 1.0f, 1.0f), // cor normal
            background,        // textura
            Vector4::UnitRect,
            // GetCamera()->GetPosCamera(),
            Vector2(0.0f, 0.0f), // cameraPos = 0 para não aplicar deslocamento do Renderer
            Vector2(1.0f, 1.0f),
            1.0f               // textureFactor
        );
    }
}

void Game::DrawParallaxLayer(Texture* texture, float parallaxFactor, int y, int h)
{
    if (!texture) return;

    int texW = texture->GetWidth();
    // int texH = texture->GetHeight();

    int offsetX = static_cast<int>(mCamera->GetPosCamera().x * parallaxFactor) % texW;
    if (offsetX < 0) offsetX += texW;

    // Cor branca (sem alteração)
    Vector3 color(1.0f, 1.0f, 1.0f);
    // Nenhum recorte de textura (usar textura inteira)
    Vector4 texRect(0.0f, 0.0f, 1.0f, 1.0f);
    // Nenhuma rotação
    float rotation = 0.0f;
    // Escala padrão (já está aplicado em texW/texH)
    Vector2 scale(1.0f, 1.0f);
    // Fator de textura 1.0 (mostrar textura normalmente)
    float textureFactor = 1.0f;

    Vector2 screenSpaceCamera(0.0f, 0.0f);

    for (int x = -offsetX; x < mLogicalWindowWidth; x += texW)
    {
        Vector2 pos(static_cast<float>(x + texW / 2), static_cast<float>(y + h / 2));
        Vector2 size(static_cast<float>(texW), static_cast<float>(h));

        mRenderer->DrawTexture(
            pos,                 // posição central do quad
            size,                // tamanho
            rotation,            // sem rotação
            color,               // cor
            texture,             // textura a desenhar
            texRect,             // retângulo completo da textura
            screenSpaceCamera,   // posição da câmera para paralaxe
            scale,               // escala padrão
            textureFactor        // fator de textura
        );
    }
}

void Game::DrawParallaxLayers(std::vector<Texture*> layers) {
    const float minParallax = 0.1f;
    const float maxParallax = 0.8f;

    // Pegamos a câmera REAL
    Vector2 realCameraPos = mCamera->GetPosCamera();

    // Pegamos a largura e altura reais da visão da câmera afetadas pelo zoom
    float viewWidth = mRenderer->GetZoomedWidth();
    float viewHeight = mRenderer->GetZoomedHeight();

    for (size_t i = 0; i < layers.size(); ++i) {
        Texture* tex = layers[i];
        if (!tex) continue;

        float t = (layers.size() > 1) ? static_cast<float>(i) / (layers.size() - 1) : 0.0f;
        float parallaxFactor = minParallax + t * (maxParallax - minParallax);

        // Ajustamos a escala da textura para manter a proporção (Aspect Ratio) correta com o Zoom
        float scale = viewHeight / static_cast<float>(tex->GetHeight());
        float texW = static_cast<float>(tex->GetWidth()) * scale;
        float texH = viewHeight;

        // Cálculo do offset visual (Onde deve aparecer na tela)
        float scrollOffset = std::fmod(realCameraPos.x * parallaxFactor, texW);
        if (scrollOffset < 0) scrollOffset += texW;

        for (float screenX = -scrollOffset; screenX < viewWidth + texW; screenX += texW) {
            // Primeiro calculamos a posição na TELA (Screen Space)
            Vector2 screenPos(
                screenX + (texW / 2.0f),
                texH / 2.0f
            );

            // Convertemos de volta para "World Space" Falso
            Vector2 worldPos = screenPos + realCameraPos;

            // CORREÇÃO 1: Micro-sobreposição (Overlap)
            // Adicionamos 1.5f (ou 2.0f) pixels na largura na hora de desenhar.
            // O laço de repetição continua avançando de 'texW' em 'texW', mas o desenho
            // sangra minimamente para a direita. Como o background é repetitivo,
            // isso é imperceptível e aniquila as linhas pretas (seams) geradas pelo std::floor.
            Vector2 size(texW + 1.5f, texH);

            mRenderer->DrawTexture(
                worldPos,
                size,
                0.0f,
                Color::White,
                tex,
                Vector4::UnitRect,
                realCameraPos
            );
        }
    }
}
