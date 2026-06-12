//
// Created by roger on 07/06/2026.
//

#include "MapMenu.h"
#include "../Game.h"
#include "../IconDictionary.h"
#include "../Renderer/Renderer.h"
#include "../MapManager.h"

MapMenu::MapMenu(Game* game, const std::string& fontName, bool isClosable)
    :UIScreen(game, fontName, isClosable)
    ,mPlayerIcon(nullptr)
    ,mIconFont(nullptr)
    ,mZoomTutorial(nullptr)
    ,mTargetZoom(0.45f)
    ,mCurrentZoom(0.45f)
    ,mIsDragging(false)
{
    mGame->TogglePause();

    mIconFont = mGame->LoadFont("../Assets/Fonts/Buttons.ttf");

    // Define o tamanho da tela para pegar cliques no fundo
    SetSize(Vector2(1920, 1080));
    SetPosition(Vector2::Zero);

    auto* background = AddImage("../Assets/Sprites/Menus/FundoPreto.png", Vector2(1920, 1080) * 0.5f, Vector2(1920, 1080));
    background->SetAlpha(0.4f);

    background = AddImage("../Assets/Sprites/Menus/FundoPretoArredondado.png", Vector2(960, 600), Vector2(1700, 900));
    background->SetAlpha(0.4f);

    AddText("MAPA", Vector2(960, 200), 40, Vector3(1.0f, 1.0f, 1.0f));

    AddText("ZOOM", Vector2(1600, 1000), 30, Vector3(1.0f, 1.0f, 1.0f));

    mZoomTutorial = new UIText("", mIconFont, 40, 1024, Vector2(1690, 1000), Color::White);
    mTexts.push_back(mZoomTutorial);

    std::string currentLevel = mGame->GetCurrentLevelPath();
    const auto& rooms = mGame->GetMapManager()->GetRooms();

    mTargetPan = Vector2::Zero;
    for (const auto& room : rooms) {
        if (room.levelID == currentLevel) {
            // Centraliza o pan no meio do Canvas dessa sala
            mTargetPan = room.mapCanvasPos + (room.size * 0.5f);
            break;
        }
    }
    mCurrentPan = mTargetPan;

    mPlayerIcon = game->GetRenderer()->GetTexture("../Assets/Sprites/EsquiloFire/Idle01.png");
}

void MapMenu::Update(float deltaTime) {
    UIScreen::Update(deltaTime);

    // MOVIMENTO CONTÍNUO DO MAPA
    float panSpeed = 600.0f;
    Vector2 panInput = Vector2::Zero;

    // Converte a direção segurada em um vetor de movimento
    if (mHeldDirection == NavDirection::Up) {
        panInput.y -= 1.0f;
    }
    else if (mHeldDirection == NavDirection::Down) {
        panInput.y += 1.0f;
    }
    else if (mHeldDirection == NavDirection::Left) {
        panInput.x -= 1.0f;
    }
    else if (mHeldDirection == NavDirection::Right) {
        panInput.x += 1.0f;
    }

    // Se estiver segurando uma direção, move o TargetPan
    if (panInput.LengthSq() > 0.0f) {
        panInput.Normalize();
        mTargetPan += panInput * panSpeed * deltaTime * (1.0f / mTargetZoom);
    }

    // Lerp para suavizar o movimento da câmera
    mCurrentPan = Vector2::Lerp(mCurrentPan, mTargetPan, 10.0f * deltaTime);
    mCurrentZoom = Math::Lerp(mCurrentZoom, mTargetZoom, 15.0f * deltaTime);

    // Atualiza Zoom tutorial
    if (mGame->GetInputPlayerMode() == Game::InputPlayerMode::Controller) {
        if (mGame->GetInputController() == Game::InputController::Xbox) {
            mZoomTutorial->SetText(std::string(Icons::XboxLB) + Icons::XboxRB);
        }
        else if (mGame->GetInputController() == Game::InputController::Playstation) {
            mZoomTutorial->SetText(std::string(Icons::PlayL1) + Icons::PlayR1);
        }
    }
    else {
        mZoomTutorial->SetText("QE");
    }
}

void MapMenu::Draw(Renderer* renderer) {
    UIScreen::Draw(renderer);

    // INÍCIO DO CLIPPING
    renderer->EnableScissor(160.0f, 250.0f, 1600.0f, 700.0f);

    // Ajusta o Centro da Tela para ser o centro da janela de mapa (960, 600)
    Vector2 mapWindowCenter(960.0f, 600.0f);

    unsigned int globalTexID = mGame->GetMapManager()->GetGlobalMapTexture();
    Vector2 canvasSize = mGame->GetMapManager()->GetGlobalMapSize();

    if (globalTexID != 0) {
        // Encontra o ponto central do Canvas
        Vector2 canvasCenter = canvasSize * 0.5f;

        // Alinha o mCurrentPan (no Canvas) com o centro da Tela
        Vector2 screenPos = mapWindowCenter + (canvasCenter - mCurrentPan) * mCurrentZoom;

        Vector2 screenSize = canvasSize * mCurrentZoom;

        Vector4 flipY(0.0f, 1.0f, 1.0f, -1.0f);
        renderer->DrawTextureByID(screenPos, screenSize, 0.0f, Vector3(1.0f, 1.0f, 1.0f), globalTexID, flipY, Vector2::Zero, Vector2::One, 1, 0.6f);
    }

    // DESENHA O ÍCONE DO JOGADOR
    std::string currentLevel = mGame->GetCurrentLevelPath();
    const auto& rooms = mGame->GetMapManager()->GetRooms();

    for (const auto& room : rooms) {
        if (room.levelID == currentLevel) {
            // Pega a posição física do jogador no mundo
            Vector2 playerWorldPos = mGame->GetPlayer()->GetPosition();

            // Transforma em posição relativa à área útil da sala
            Vector2 playablePos = playerWorldPos - room.boundsMin;

            // Aplica a escala para descobrir onde ele está na arte do mapa
            Vector2 playerMapPos = playablePos * room.scaleFactor;

            // Converte essa posição local para o espaço global do Canvas
            Vector2 playerCanvasPos;
            playerCanvasPos.x = room.mapCanvasPos.x + playerMapPos.x;
            playerCanvasPos.y = room.mapCanvasPos.y + playerMapPos.y;

            // Aplica o Pan e o Zoom da UI
            Vector2 iconScreenPos = mapWindowCenter + (playerCanvasPos - mCurrentPan) * mCurrentZoom;

            // Desenha o ícone
            Vector2 iconSize(90.0f, 100.0f);

            renderer->DrawTexture(iconScreenPos, iconSize, 0.0f, Vector3(1.0f, 1.0f, 1.0f), mPlayerIcon);
            break;
        }
    }
    // FIM DO CLIPPING
    renderer->DisableScissor();
}

void MapMenu::HandleKeyPress(int key, int controllerButton, int leftControllerAxisY, int leftControllerAxisX, int rightControllerAxisY, int rightControllerAxisX) {
    // Chama o base para garantir que os botões da UI funcionem
    UIScreen::HandleKeyPress(key, controllerButton, leftControllerAxisY, leftControllerAxisX, rightControllerAxisY, rightControllerAxisX);

    // ZOOM
    // Teclado (Q e E) e Controle (Shoulders)
    const float zoomFactor = 1.25f;

    if (key == SDLK_e || controllerButton == SDL_CONTROLLER_BUTTON_RIGHTSHOULDER) {
        mTargetZoom *= zoomFactor;
    }
    else if (key == SDLK_q || controllerButton == SDL_CONTROLLER_BUTTON_LEFTSHOULDER) {
        mTargetZoom /= zoomFactor;
    }

    // Trava os limites do zoom
    mTargetZoom = Math::Clamp(mTargetZoom, 0.1f, 2.0f);

    auto inputBinding = mGame->GetInputBinding();

    // Registra a direção apenas no primeiro clique
    if (key == SDLK_UP ||
        key == SDL_GetKeyFromScancode(inputBinding[Game::Action::Up].key) ||
        controllerButton == SDL_CONTROLLER_BUTTON_DPAD_UP ||
        leftControllerAxisY < 0 || rightControllerAxisY < 0)
    {
        if (mHeldDirection != NavDirection::Up) {
            mHeldDirection = NavDirection::Up;
        }
    }
    else if (key == SDLK_DOWN ||
             key == SDL_GetKeyFromScancode(inputBinding[Game::Action::Down].key) ||
             controllerButton == SDL_CONTROLLER_BUTTON_DPAD_DOWN ||
             leftControllerAxisY > 0 || rightControllerAxisY > 0)
    {
        if (mHeldDirection != NavDirection::Down) {
            mHeldDirection = NavDirection::Down;
        }
    }
    else if (key == SDLK_LEFT ||
             key == SDL_GetKeyFromScancode(inputBinding[Game::Action::MoveLeft].key) ||
             controllerButton == SDL_CONTROLLER_BUTTON_DPAD_LEFT ||
             leftControllerAxisX < 0 || rightControllerAxisX < 0)
    {
        if (mHeldDirection != NavDirection::Left) {
            mHeldDirection = NavDirection::Left;
        }
    }
    else if (key == SDLK_RIGHT ||
             key == SDL_GetKeyFromScancode(inputBinding[Game::Action::MoveRight].key) ||
             controllerButton == SDL_CONTROLLER_BUTTON_DPAD_RIGHT ||
             leftControllerAxisX > 0 || rightControllerAxisX > 0)
    {
        if (mHeldDirection != NavDirection::Right) {
            mHeldDirection = NavDirection::Right;
        }
    }
}

void MapMenu::HandleMousePress(const Vector2& virtualMousePos) {
    UIScreen::HandleMousePress(virtualMousePos);

    if (mDraggedButton == nullptr) {
        mIsDragging = true;
        mLastMousePos = virtualMousePos;
    }
}

void MapMenu::HandleMouseRelease(const Vector2& virtualMousePos) {
    UIScreen::HandleMouseRelease(virtualMousePos);
    mIsDragging = false;
}

void MapMenu::HandleMouseMotion(const Vector2& virtualMousePos) {
    UIScreen::HandleMouseMotion(virtualMousePos);

    if (mIsDragging) {
        // Calcula a diferença do movimento do mouse
        Vector2 delta = mLastMousePos - virtualMousePos;

        // Aplica ao pan e divide pelo zoom para que o mapa acompanhe perfeitamente a seta
        mTargetPan += delta * (1.0f / mCurrentZoom);

        mLastMousePos = virtualMousePos;
    }
}

void MapMenu::Close() {
    UIScreen::Close();
    mGame->TogglePause();
}
