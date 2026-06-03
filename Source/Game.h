#pragma once

#include <SDL.h>
#include <vector>
#include "Renderer/Renderer.h"
#include "Camera.h"
#include "Actors/Ground.h"
#include "Actors/Player.h"
#include "Actors/FireBall.h"
#include "Actors/Enemies/Enemy.h"
#include "Math.h"
#include <string>
#include <unordered_map>
#include "AudioSystem.h"
#include "Cutscene.h"
#include "Map.h"
#include "SaveManager.h"
#include "SkillTreeManager.h"
#include "WaveManager.h"

class Game
{
public:
    const int DEAD_ZONE = 20000;
    const float mTransitionTime = 0.1f;

    // Estados de movimento do analógico vertical
    enum class StickState {
        Neutral,
        Up,
        Down,
        Left,
        Right
    };

    enum class GameScene {
        MainMenu,
        Gameplay
    };

    enum class SceneManagerState {
        None,
        Entering,
        Active,
        Exiting
    };

    enum class GamePlayState {
        Playing,
        Paused,
        GameOver,
        LevelComplete,
        Leaving,
        Cutscene,
        Menu
    };

    enum class InputPlayerMode {
        Keyboard,
        Mouse,
        Controller
    };

    enum class InputController {
        Xbox,
        Playstation
    };

    enum class Action {
        Up,
        Down,
        MoveLeft,
        MoveRight,
        Jump,
        Attack,
        Dash,
        Skill1,
        Skill2,
        Heal,
        Hook,
        OpenStore,
        Map,
        Look,
        ChangeMode,
        Invalid
    };

    struct InputBinding {
        SDL_Scancode key = SDL_SCANCODE_UNKNOWN;
        Uint8 mouseButton = 0;
        SDL_GameControllerButton btn = SDL_CONTROLLER_BUTTON_INVALID;
        SDL_GameControllerAxis axis = SDL_CONTROLLER_AXIS_INVALID;
    };

    Game(int windowWidth, int windowHeight, int FPS);

    bool Initialize();
    void RunLoop();
    void Shutdown();
    void Quit() { mIsRunning = false; }

    // Actor functions
    void UpdateActors(float deltaTime);
    void AddActor(class Actor *actor);
    void RemoveActor(class Actor *actor);

    // Renderer
    class Renderer* GetRenderer() { return mRenderer; }

    // Draw functions
    void AddDrawable(class DrawComponent *drawable);
    void RemoveDrawable(class DrawComponent *drawable);
    void SortDrawables();

    // Window functions
    SDL_Window* GetWindow() const { return mWindow; }
    int GetWindowWidth() const { return mWindowWidth; }
    int GetWindowHeight() const { return mWindowHeight; }
    float GetLogicalWindowWidth() const { return mLogicalWindowWidth; }
    float GetLogicalWindowHeight() const { return mLogicalWindowHeight; }

    // Game-specific
    void AddGround(class Ground* g);
    void RemoveGround(class Ground* g);
    std::vector<class Ground*> &GetGrounds() { return mGrounds; }
    Ground* GetGroundById(int id);

    class Player* GetPlayer() const { return mPlayer; }

    void AddCollider(class ColliderComponent* collider);
    void RemoveCollider(class ColliderComponent *collider);

    void UpdateCamera(float deltaTime);
    class Camera* GetCamera() const { return mCamera; }

    void AddFireBall(class FireBall* f);
    void RemoveFireball(class FireBall* f);
    std::vector<class FireBall*> &GetFireBalls() { return mFireBalls; }

    void AddParticle(class Particle* p);
    void RemoveParticle(class Particle* p);
    std::vector<class Particle*> &GetParticles() { return mParticles; }

    void AddProjectile(class Projectile* p);
    void RemoveProjectile(class Projectile* p);
    std::vector<class Projectile*> &GetProjectiles() { return mProjectiles; }

    void AddMoney(class Money* m);
    void RemoveMoney(class Money* m);
    std::vector<class Money*> &GetMoneys() { return mMoneys; }

    void AddHookPoint(class HookPoint* hp);
    void RemoveHookPoint(class HookPoint* hp);
    std::vector<class HookPoint*> &GetHookPoints() { return mHookPoints; }

    void AddLaserShooter(class LaserShooter* ls);
    void RemoveLaserShooter(class LaserShooter* ls);
    std::vector<class LaserShooter*> &GetLaserShooters() { return mLaserShooters; }
    LaserShooter* GetLaserShooterById(int id);

    void AddSpawnPoint(const std::string& id, const Vector2& pos);
    Vector2 GetSpawnPointPosition(const std::string& id) const;
    std::vector<Vector2> GetSpawnPointsPositions();

    void CreateWaveManager(std::string wavesFilePath);
    void RemoveWaveManager() { delete mWaveManager; mWaveManager = nullptr; }

    void AddEnemy(class Enemy* e);
    void RemoveEnemy(class Enemy* e);
    std::vector<class Enemy*> &GetEnemies() { return mEnemies; }
    Enemy* GetEnemyById(int id);

    void SetBackToCheckpoint() { mBackToCheckpoint = true; }
    void InitCrossFade(float duration);

    int **GetLevelData() const { return mLevelData; }
    int **GetLevelDataDynamicGrounds() const { return mLevelDataDynamicGrounds; }

    Texture* GetTileSheet() const { return mTileSheet; }
    std::unordered_map<int, Vector4> GetTileSheetData() { return mTileSheetData; }

    Texture* GetDecorationsTileSheet() const { return mDecorationsTileSheet; }
    std::vector<Vector4> GetDecorationsTileSheetData() const { return mDecorationsTileSheetData; }
    std::vector<std::string> GetDecorationsName() { return mDecorationsName; }
    const std::string& GetDecorationNameFromGid(int gid) const;

    float GetTileSize() { return mTileSize; }
    int GetOriginalTileSize() { return mOriginalTileSize; }

    SDL_Color GetGroundParticleColor() const { return mGroundParticleColor; }

    // Loading functions
    class UIFont* LoadFont(const std::string& fileName);
    SDL_Texture *LoadTexture(const std::string &texturePath);

    void BackToMenu();
    void ResetPlayerAndSkillTree();
    void RebindKeyboard(class UIText* text, Action action);
    void RebindController(class UIText* text, Action action);
    void CancelRebind();
    void ResetKeyboardToDefault();
    void ResetKeyboardAndMouseToDefault();
    void ResetControllerToDefault();
    std::string GetIconStringForAction(Action action, bool forceKeyboard = false, bool forceController = false);

    int GetFPS() const { return mFPS; }

    void ActiveHitStop(float duration = 0.15f)
    {
        mHitstopDelayActive = true;
        mHitstopDelayTimer = 0;
        mHitstopTimer = 0;
        mHitstopTimer = mHitstopDuration - duration;
    }

    void ActiveDamageFlash();

    void SetIsSlowMotion(bool slowMotion) { mIsSlowMotion = slowMotion; }
    bool IsSlowMotion() const { return  mIsSlowMotion; }

    float GetRightAxisX() const { return mRawRightAxisX; }
    float GetRightAxisY() const { return mRawRightAxisY; }

    // Audio functions
    class AudioSystem* GetAudio() const { return mAudio; }
    void SetMusicHandle(SoundHandle music) { mMusicHandle = music; }
    SoundHandle GetMusicHandle() const { return mMusicHandle; }
    void SetBossMusicHandle(SoundHandle music) { mBossMusic = music; }
    SoundHandle GetBossMusicHandle() const { return mBossMusic; }
    void StartBossMusic(SoundHandle music);
    void StopBossMusic();

    // UI functions
    void PushUI(class UIScreen* screen) { mUIStack.emplace_back(screen); }
    std::vector<class UIScreen*>& GetUIStack() { return mUIStack; }
    class HUD* GetHUD() const { return mHUD; }

    // Scene management
    void LoadNextLevel(const std::string& levelPath, float transitionTime = 0.0f);
    void UnloadScene();
    GameScene GetGameScene() const { return mGameScene; }
    std::string GetCurrentLevelPath() const { return mCurrentLevelPath; }

    void TogglePause();

    void SetGamePlayState(GamePlayState state) { mGamePlayState = state; }
    GamePlayState GetGamePlayState() const { return mGamePlayState; }

    InputPlayerMode GetInputPlayerMode() const { return mInputPlayerMode; }
    InputController GetInputController() const { return mInputController; }
    void DetectTControllerType();

    SkillTreeManager* GetSkillTreeManager() const { return mSkillTreeManager; }

    void PlayFinalGoodCutscene();
    void PlayFinalEvilCutscene();
    void SetIsPlayingFinalCutscene() { mIsPlayingFinalCutscene = true; }

    std::vector<std::string> mGoodCutscenes;
    std::vector<GameScene> mGoodCutsceneScenes;
    std::vector<std::string> mEvilCutscenes;
    int mCutsceneIndex;

    void SetCheckPointPosition(Vector2 pos) { mCheckpointPosition = pos; }
    Vector2 GetCheckPointPosition() const { return mCheckpointPosition; }
    void SetCheckpointGameScenePath(std::string scene) { mCheckpointGameScenePath = scene; }
    std::string GetCheckpointGameScenePath() const { return mCheckpointGameScenePath; }
    void SetCheckPointMoney(int money) { mCheckPointMoney = money; }
    int GetCheckPointMoney() const { return mCheckPointMoney; }
    void SetGoingToNextLevel() { mGoingToNextLevel = true; }
    bool GetGoingToNextLevel() const { return mGoingToNextLevel; }
    void SetCurrentCutscene(Cutscene* cutscene) { mCurrentCutscene = cutscene; }
    void SetHitByLava() { mHitByLava = true; }
    void SetLavaRespawnPosition(Vector2 lavaRespawnPosition) { mLavaRespawnPosition = lavaRespawnPosition; }
    void SetPlayerStartPositionId(int id) { mPlayerStartPositionId = id; }

    void SetTotalPlayTime(float totalPlayTime) { mTotalPlayTime = totalPlayTime; }
    float GetTotalPlayTime() const { return mTotalPlayTime; }

    void SetSaveSlot(int saveSlot) { mSaveSlot = saveSlot; }
    SaveManager* GetSaveManager() { return mSaveManager; }

    SceneManagerState GetSceneManagerState() const { return mSceneManagerState; }

    void SaveGame();
    void LoadGame();
    void SetWorldState(const std::unordered_map<std::string, bool> &worldState) { mWorldState = worldState; }
    std::unordered_map<std::string, bool> GetWorldState() { return mWorldState; }
    void SetWorldFlag(const std::string& key, bool value);
    bool GetWorldFlag(const std::string& key) const;

    void SetTargetZoom(float targetZoom) { mTargetZoom = targetZoom; }
    void SetZoomSpeed(float zoomSpeed) { mZoomSpeed = zoomSpeed; }

    std::vector<Vector2> GetResolutionsList() { return mResolutions; }

    // Converte uma Action para sua representação em string
    std::string ActionToString(Action action);
    Action StringToAction(const std::string& str);
    void SaveBindingsToFile(const std::string& filename);
    void LoadBindingsFromFile(const std::string& filename);
    bool IsActionPressed(Action action, const Uint8* keyboardState, SDL_GameController* controller);
    std::unordered_map<Action, InputBinding> GetInputBinding() { return mInputBindings; }

    void LoadLevelSelectMenu();

private:
    void ProcessInput();
    void UpdateGame();
    void GenerateOutput();

    // Load Level
    void LoadObjects(const nlohmann::json& mapData);
    void LoadLevel(const std::string &fileName, const nlohmann::json& mapData, bool hasTileSet = true);
    bool ShouldLoadObject(const std::string& condition);
    SDL_Color HexToColor(std::string hex);

    void SwapKeyboardBinding(SDL_Scancode newKey, Uint8 newMouseBtn);
    void SwapControllerBinding(SDL_GameControllerButton newBtn, SDL_GameControllerAxis newAxis);

    // All the actors in the game
    std::vector<class Actor *> mActors;
    std::vector<class Actor *> mPendingActors;

    // All the draw components
    std::vector<class DrawComponent *> mDrawables;

    // SDL stuff
    SDL_Window* mWindow;
    AudioSystem* mAudio;
    class Renderer* mRenderer;

    // Window properties
    int mWindowWidth;
    int mWindowHeight;
    const float mOriginalWindowWidth = 1920;
    const float mOriginalWindowHeight = 1080;
    float mLogicalWindowWidth;
    float mLogicalWindowHeight;

    // Track elapsed time since game start
    Uint32 mTicksCount;

    // Track if we're updating actors right now
    bool mIsRunning;
    bool mUpdatingActors;

    int mFPS;

    bool mIsPaused;
    bool mBackToCheckpoint;
    bool mIsCrossFading;
    float mCrossFadeDuration;
    float mCrossFadeTimer;

    // Camera
    class Camera *mCamera;
    float mZoom;
    float mTargetZoom;
    float mZoomSpeed;

    // Game-specific
    class Player *mPlayer;
    std::vector<class ColliderComponent*> mColliders;
    std::vector<class Ground*> mGrounds;
    std::vector<class FireBall*> mFireBalls;
    std::vector<class Particle*> mParticles;
    std::vector<class Projectile*> mProjectiles;
    std::vector<class Money*> mMoneys;
    std::vector<class HookPoint*> mHookPoints;
    std::vector<class LaserShooter*> mLaserShooters;
    std::vector<class Enemy*> mEnemies;
    std::map<SDL_JoystickID, SDL_GameController*> mControllers;
    SDL_GameController* mCurrentController;

    class HUD *mHUD;
    std::vector<class Checkpoint*> mCheckPoints;
    std::unordered_map<std::string, Vector2> mSpawnPoints;

    WaveManager* mWaveManager;

    UIText* mNewButtonText;
    bool mWaitingForKey;
    Action mBindingAction;
    bool mWaitingForButton;

    std::unordered_map<Action, InputBinding> mInputBindings;

    int mPlayerDeathCounter;

    // Player State
    Vector2 mCheckpointPosition;
    std::string mCheckpointGameScenePath;
    int mCheckPointMoney;
    bool mGoingToNextLevel;
    Vector2 mLavaRespawnPosition;
    bool mHitByLava;
    int mPlayerStartPositionId;

    int mSaveSlot;
    class SaveData* mSaveData;
    class SaveManager* mSaveManager;
    float mTotalPlayTime;

    // World State
    std::unordered_map<std::string, bool> mWorldState;

    // Level data
    int **mLevelData;
    int **mLevelDataDynamicGrounds;

    Texture* mTileSheet;
    std::unordered_map<int, Vector4> mTileSheetData;
    Texture* mDecorationsTileSheet;
    std::vector<Vector4> mDecorationsTileSheetData;
    std::vector<std::string> mDecorationsName;
    std::string mCurrentTileSheetPath;
    std::string mCurrentDecorationsPath;
    std::unordered_map<int, std::string> mGidToDecorationName;
    int mDecorationsFirstGid = 1;

    int mLevelWidth;
    int mLevelHeight;
    float mTileSize;
    int mOriginalTileSize = 32;
    Map* mMap;
    bool mShowMap;
    bool mGroundBehindPlayer;
    bool mUseGroundPadding;
    bool mUseGrassParticle;
    SDL_Color mGroundParticleColor;

    bool mHitstopActive;
    float mHitstopDuration;
    float mHitstopTimer;
    bool mHitstopDelayActive;
    float mHitstopDelayDuration;
    float mHitstopDelayTimer;

    bool mDamageFlashActive;
    float mDamageFlashDuration;
    float mDamageFlashTimer;

    bool mIsSlowMotion;
    bool mIsAccelerated;

    StickState mLeftStickStateY;
    StickState mLeftStickStateX;
    StickState mRightStickStateY;
    StickState mRightStickStateX;
    float mRawRightAxisX;
    float mRawRightAxisY;

    SoundHandle mMusicHandle;
    SoundHandle mBossMusic;
    std::string mCurrentMusic;
    float mPauseMusicVolumeScale;

    // Scene management
    void UpdateSceneManager(float deltaTime);
    void ChangeScene();
    SceneManagerState mSceneManagerState;
    float mSceneManagerTimer;
    float mFadeDuration;
    Uint8 mFadeAlpha;

    // All the UI elements
    std::vector<class UIScreen*> mUIStack;
    UIScreen* mLastTopUIScreen;
    std::unordered_map<std::string, class UIFont*> mFonts;
    UIScreen* mPauseMenu;
    UIScreen* mLevelSelectMenu;

    std::vector<Vector2> mResolutions = {
        Vector2(640, 360),
        Vector2(854, 480),
        Vector2(1280, 720),
        Vector2(1920, 1080)
    };

    GamePlayState mGamePlayState;

    // If is playing on controller or keyboard or mouse
    InputPlayerMode mInputPlayerMode;
    InputController mInputController;

    // Track level state
    GameScene mGameScene;
    GameScene mNextScene;
    std::string mCurrentLevelPath;
    std::string mNextLevelPath;

    Cutscene* mCurrentCutscene;
    bool mIsPlayingFinalCutscene;

    SkillTreeManager* mSkillTreeManager;

    // Background images
    void DrawParallaxBackground(Texture* background);
    void DrawParallaxLayer(Texture* texture, float parallaxFactor, int y, int h);
    void DrawParallaxLayers(std::vector<Texture*> backgroundLayers);

    bool mUseParallaxBackground;
    std::vector<Texture*> mBackgroundLayers;
    Texture* mBackGroundTexture;
};
