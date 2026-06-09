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
    ,mTargetZoom(0.5f)
    ,mCurrentZoom(0.5f)
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

    Vector2 screenCenter(renderer->GetVirtualWidth() / 2.0f, renderer->GetVirtualHeight() / 2.0f);

    unsigned int globalTexID = mGame->GetMapManager()->GetGlobalMapTexture();
    Vector2 canvasSize = mGame->GetMapManager()->GetGlobalMapSize();

    if (globalTexID != 0) {
        // Encontra o ponto central do Canvas
        Vector2 canvasCenter = canvasSize * 0.5f;

        // Alinha o mCurrentPan (no Canvas) com o centro da Tela
        Vector2 screenPos = screenCenter + (canvasCenter - mCurrentPan) * mCurrentZoom;

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
            Vector2 iconScreenPos = screenCenter + (playerCanvasPos - mCurrentPan) * mCurrentZoom;

            // Desenha o ícone
            Vector2 iconSize(90.0f, 100.0f);

            renderer->DrawTexture(iconScreenPos, iconSize, 0.0f, Vector3(1.0f, 1.0f, 1.0f), mPlayerIcon);
            break;
        }
    }
}

void MapMenu::HandleKeyPress(int key, int controllerButton, int leftControllerAxisY, int leftControllerAxisX, int rightControllerAxisY, int rightControllerAxisX) {
    // Chama o base para garantir que os botões da UI funcionem
    UIScreen::HandleKeyPress(key, controllerButton, leftControllerAxisY, leftControllerAxisX, rightControllerAxisY, rightControllerAxisX);

    // ZOOM
    // Teclado (Q e E) e Controle (Shoulders)
    if (key == SDLK_e || controllerButton == SDL_CONTROLLER_BUTTON_RIGHTSHOULDER) {
        mTargetZoom += 0.2f;
    }
    else if (key == SDLK_q || controllerButton == SDL_CONTROLLER_BUTTON_LEFTSHOULDER) {
        mTargetZoom -= 0.2f;
    }

    // Trava os limites do zoom
    mTargetZoom = Math::Clamp(mTargetZoom, 0.35f, 2.2f);

    // MOVIMENTO (PAN)
    // O movimento pelo HandleKeyPress é um "nudge" (um pequeno empurrão a cada clique na tecla).
    // Para funcionar bem, aumentamos o valor do passo, já que não é multiplicado por deltaTime aqui.
    float panStep = 50.0f;
    Vector2 panInput = Vector2::Zero;

    // Teclado (Setas e WASD) usando SDLK_
    if (key == SDLK_RIGHT || key == SDLK_d) panInput.x -= 1.0f;
    if (key == SDLK_LEFT  || key == SDLK_a) panInput.x += 1.0f;
    if (key == SDLK_DOWN  || key == SDLK_s) panInput.y -= 1.0f;
    if (key == SDLK_UP    || key == SDLK_w) panInput.y += 1.0f;

    // Controle (Analógico Esquerdo ou D-PAD)

    int deadzone = 10000;
    if (leftControllerAxisX > deadzone || controllerButton == SDL_CONTROLLER_BUTTON_DPAD_RIGHT) panInput.x -= 1.0f;
    if (leftControllerAxisX < -deadzone || controllerButton == SDL_CONTROLLER_BUTTON_DPAD_LEFT) panInput.x += 1.0f;
    if (leftControllerAxisY > deadzone || controllerButton == SDL_CONTROLLER_BUTTON_DPAD_DOWN) panInput.y -= 1.0f;
    if (leftControllerAxisY < -deadzone || controllerButton == SDL_CONTROLLER_BUTTON_DPAD_UP) panInput.y += 1.0f;

    // Se houve algum input direcional, aplicamos ao TargetPan compensando o Zoom
    if (panInput.LengthSq() > 0.0f) {
        // Normaliza para não mover mais rápido nas diagonais
        panInput.Normalize();
        mTargetPan += panInput * panStep * (1.0f / mTargetZoom);
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
