//
// Created by roger on 23/05/2025.
//

#include "Trigger.h"
#include "../Camera.h"
#include "../DialogueSystem.h"
#include "../Game.h"
#include "../Components/AABBComponent.h"
#include "../Components/Drawing/RectComponent.h"
#include "../Actors/Ground.h"
#include "../Actors/DynamicGround.h"

Trigger::Trigger(class Game *game, float width, float height)
    :Actor(game)
    ,mWidth(width)
    ,mHeight(height)
    ,mTarget(Target::Nothing)
    ,mDestroy(false)
    ,mRectComponent(nullptr)

{
    Vector2 v1(-mWidth / 2, -mHeight / 2);
    Vector2 v2(mWidth / 2, -mHeight / 2);
    Vector2 v3(mWidth / 2, mHeight / 2);
    Vector2 v4(-mWidth / 2, mHeight / 2);

    std::vector<Vector2> vertices;
    vertices.emplace_back(v1);
    vertices.emplace_back(v2);
    vertices.emplace_back(v3);
    vertices.emplace_back(v4);

    // mRectComponent = new RectComponent(this, mWidth, mHeight, RendererMode::LINES);
    // mRectComponent->SetColor(Vector3(160, 32, 240));

    // mDrawPolygonComponent = new DrawPolygonComponent(this, vertices, {160, 32, 240, 255});
    mAABBComponent = new AABBComponent(this, v1, v3);
}

void Trigger::SetTarget(std::string target) {
    if (target == "Camera") {
        mTarget = Target::Camera;
        return;
    }
    if (target == "DynamicGround") {
        mTarget = Target::DynamicGround;
        return;
    }
    if (target == "Ground") {
        mTarget = Target::Ground;
        return;
    }
    if (target == "Game") {
        mTarget = Target::Game;
        return;
    }
    if (target == "Renderer") {
        mTarget = Target::Renderer;
        return;
    }
    if (target == "Enemy") {
        mTarget = Target::Enemy;
        return;
    }
    if (target == "Dialogue") {
        mTarget = Target::Dialogue;
        return;
    }
    if (target == "Cutscene") {
        mTarget = Target::Cutscene;
        return;
    }
    if (target == "Player") {
        mTarget = Target::Player;
        return;
    }
}

void Trigger::SetEvent(std::string event) {
    if (event == "FollowPlayerLimited") {
        mEvent = Event::FollowPlayerLimited;
        return;
    }

    if (event == "SetIsGrowing") {
        mEvent = Event::SetIsGrowing;
        return;
    }
    if (event == "SetIsDecreasing") {
        mEvent = Event::SetIsDecreasing;
        return;
    }
    if (event == "SetIsDecreasingAfterKillingEnemies") {
        mEvent = Event::SetIsDecreasingAfterKillingEnemies;
        return;
    }

    if (event == "SetIsMoving") {
        mEvent = Event::SetIsMoving;
        return;
    }

    if (event == "ChangeScene") {
        mEvent = Event::ChangeScene;
        return;
    }
    if (event == "StartArena") {
        mEvent = Event::StartArena;
        return;
    }
    if (event == "ChangeWorldState") {
        mEvent = Event::ChangeWorldState;
        return;
    }
    if (event == "ChangeZoom") {
        mEvent = Event::ChangeZoom;
        return;
    }

    if (event == "ChangeAmbientLight") {
        mEvent = Event::ChangeAmbientLight;
        return;
    }

    if (event == "SpotPlayer") {
        mEvent = Event::SpotPlayer;
        return;
    }
    if (event == "GolemVulnerable") {
        mEvent = Event::GolemVulnerable;
        return;
    }

    if (event == "StartDialogue") {
        mEvent = Event::StartDialogue;
        return;
    }

    if (event == "StartCutscene") {
        mEvent = Event::StartCutscene;
        return;
    }

    if (event == "InvertControls") {
        mEvent = Event::InvertControls;
        return;
    }

    if (event == "RevertControls") {
        mEvent = Event::RevertControls;
        return;
    }
}

void Trigger::SetScene(std::string scene) {
    if (scene == "LevelTeste") {
        mScene = Game::GameScene::LevelTeste;
        return;
    }
    if (scene == "Room0") {
        mScene = Game::GameScene::Room0;
        return;
    }
    if (scene == "Room1") {
        mScene = Game::GameScene::Room1;
        return;
    }
    if (scene == "Room2") {
        mScene = Game::GameScene::Room2;
        return;
    }
    if (scene == "Room3") {
        mScene = Game::GameScene::Room3;
        return;
    }
    if (scene == "Room4") {
        mScene = Game::GameScene::Room4;
        return;
    }
    if (scene == "Room5") {
        mScene = Game::GameScene::Room5;
        return;
    }
    if (scene == "Room6") {
        mScene = Game::GameScene::Room6;
        return;
    }
    if (scene == "Room7") {
        mScene = Game::GameScene::Room7;
        return;
    }
    if (scene == "Desafios") {
        mScene = Game::GameScene::Desafios;
        return;
    }
    if (scene == "Cave") {
        mScene = Game::GameScene::Cave;
        return;
    }
    if (scene == "MirrorBoss") {
        mScene = Game::GameScene::MirrorBoss;
        return;
    }
    if (scene == "Coliseu") {
        mScene = Game::GameScene::Coliseu;
        return;
    }
    if (scene == "Level1") {
        mScene = Game::GameScene::Level1;
        return;
    }
    if (scene == "Level2") {
        mScene = Game::GameScene::Level2;
        return;
    }
    if (scene == "Level3") {
        mScene = Game::GameScene::Level3;
        return;
    }
    if (scene == "Level4") {
        mScene = Game::GameScene::Level4;
        return;
    }
    if (scene == "Level5") {
        mScene = Game::GameScene::Level5;
        return;
    }
}

void Trigger::SetWavesPath(const std::string &wavesPath) {
    mWavesPath = "../Assets/EnemiesWaves/" + wavesPath + ".json";
}


void Trigger::OnUpdate(float deltaTime) {
    Player* player = mGame->GetPlayer();
    if (mAABBComponent->Intersect(*player->GetComponent<ColliderComponent>())) {
        switch (mTarget) {
            case Target::Camera:
                CameraTrigger();
                break;

            case Target::DynamicGround:
                DynamicGroundTrigger();
                break;

            case Target::Ground:
                GroundTrigger();
                break;

            case Target::Game:
                GameTrigger();
                break;

            case Target::Renderer:
                RendererTrigger();
                break;

            case Target::Enemy:
                EnemyTrigger();
                break;

            case Target::Dialogue:
                DialogueTrigger();
                break;

            case Target::Cutscene:
                CutsceneTrigger();
                break;

            case Target::Player:
                PlayerTrigger();
                break;

            default:
                break;
        }
        if (mDestroy) {
            SetState(ActorState::Destroy);
        }
    }
}

void Trigger::CameraTrigger() {
    Camera* camera = mGame->GetCamera();
    switch (mEvent) {
        case Event::FollowPlayerLimited:
            camera->SetLimitMinCameraPosition(mLimitMinCameraPosition);
            camera->SetLimitMaxCameraPosition(mLimitMaxCameraPosition);
            camera->ChangeCameraMode(CameraMode::FollowPlayerLimited);
            break;

        default:
            break;
    }
}

void Trigger::DynamicGroundTrigger() {
    std::vector<Ground *> grounds = mGame->GetGrounds();
    std::vector<Enemy *> enemies = mGame->GetEnemies();
    bool allEnemiesDie = true;
    switch (mEvent) {
        case Event::SetIsGrowing:
            for (int id : mGroundsIds) {
                Ground* g = mGame->GetGroundById(id);
                DynamicGround* dynamicGround = dynamic_cast<DynamicGround*>(g);
                if (dynamicGround) {
                    dynamicGround->SetIsGrowing(true);
                }
            }
            break;

        case Event::SetIsDecreasing:
            for (int id : mGroundsIds) {
                Ground* g = mGame->GetGroundById(id);
                DynamicGround* dynamicGround = dynamic_cast<DynamicGround*>(g);
                if (dynamicGround) {
                    dynamicGround->SetIsDecreasing(true);
                }
            }
            break;

        case Event::SetIsDecreasingAfterKillingEnemies:
            for (int id : mEnemiesIds) {
                Enemy *e = mGame->GetEnemyById(id);
                if (e != nullptr) {
                    allEnemiesDie = false;
                }
            }
            if (allEnemiesDie) {
                for (int id : mGroundsIds) {
                    Ground *g = mGame->GetGroundById(id);
                    DynamicGround* dynamicGround = dynamic_cast<DynamicGround*>(g);
                    if (dynamicGround) {
                        dynamicGround->SetIsDecreasing(true);
                    }
                }
            }
            break;

        default:
            break;
    }
}

void Trigger::GroundTrigger() {
    std::vector<Ground *> grounds = mGame->GetGrounds();
    switch (mEvent) {
        case Event::SetIsMoving:
            for (int id : mGroundsIds) {
                Ground *g = mGame->GetGroundById(id);
                if (g) {
                    g->SetIsMoving(true);
                }
            }
            break;
        default:
            break;
    }
}

void Trigger::GameTrigger() {
    switch (mEvent) {
        case Event::ChangeScene:
            mGame->GetAudio()->StopAllSounds();
            mGame->SetPlayerStartPositionId(mPlayerStartPositionId);
            mGame->SetGameScene(mScene, 0.5f);
            mGame->SetGoingToNextLevel();
            break;

        case Event::StartArena:
            mGame->CreateWaveManager(mWavesPath);
            break;

        case Event::ChangeWorldState:
            mGame->SetWorldFlag(mWorldState, mWorldStateFlag);
            break;

        case Event::ChangeZoom:
            mGame->SetTargetZoom(mTargetZoom);
            mGame->SetZoomSpeed(mZoomSpeed);
            break;

        default:
            break;
    }
}

void Trigger::RendererTrigger() {
    switch (mEvent) {
        case Event::ChangeAmbientLight:
            mGame->GetRenderer()->SetAmbientLight(mAmbientColor, mAmbientIntensity);
            break;

        default:
            break;
    }
}

void Trigger::EnemyTrigger() {
    std::vector<Enemy *> enemies = mGame->GetEnemies();
    switch (mEvent) {
        case Event::SpotPlayer:
            for (int id : mEnemiesIds) {
                Enemy *e = mGame->GetEnemyById(id);
                if (e) {
                    e->SetSpottedPlayer(true);
                }
            }
            break;

        default:
            break;
    }
}

void Trigger::DialogueTrigger() {
    auto* dialogue = new DialogueSystem(mGame, "../Assets/Fonts/K2D-Bold.ttf", mDialoguePath);
}

void Trigger::CutsceneTrigger() {
    auto* cutscene = new Cutscene(mGame, mCutsceneId, "../Assets/Cutscenes/Cutscenes.json");
    cutscene->Start();
    mGame->SetCurrentCutscene(cutscene);
    mGame->SetGamePlayState(Game::GamePlayState::Cutscene);
}

void Trigger::PlayerTrigger() {
    auto* player = mGame->GetPlayer();
    switch (mEvent) {
        case Event::InvertControls:
            player->SetInvertControls(true);
            break;

        case Event::RevertControls:
            player->SetInvertControls(false);
            break;

        default:
            break;
    }
}
