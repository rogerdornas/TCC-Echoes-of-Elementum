//
// Created by roger on 07/06/2026.
//

#include "MapManager.h"
#include "Game.h"
#include "Renderer/Renderer.h"

MapManager::MapManager(Game* game, Renderer *renderer)
    :mGame(game)
    ,mRenderer(renderer)
    ,mBrushTexture(nullptr)
    ,mGlobalFBO(0)
    ,mGlobalTexture(0)
{
    mBrushTexture = mRenderer->GetTexture("../Assets/Sprites/Particle/ImperfectCircleBlur.png", true);
}

MapManager::~MapManager() {
    for (auto& room : mRooms) {
        if (room.maskFBO != 0) {
            glDeleteFramebuffers(1, &room.maskFBO);
        }
        if (room.maskTextureID != 0) {
            glDeleteTextures(1, &room.maskTextureID);
        }
    }
    mRooms.clear();

    glDeleteFramebuffers(1, &mGlobalFBO);
    glDeleteTextures(1, &mGlobalTexture);
}

void MapManager::LoadRoom(const std::string& levelID, const std::string& texturePath, const Vector2& canvasPos, float scaleFactor, const Vector2& boundsMin, const Vector2& boundsMax) {
    MapRoom room;
    room.levelID = levelID;
    room.mapCanvasPos = canvasPos;
    room.isDiscovered = false;
    room.scaleFactor = scaleFactor;
    room.boundsMin = boundsMin;

    room.mapTexture = mRenderer->GetTexture(texturePath);
    if (!room.mapTexture) return;

    room.size = Vector2(static_cast<float>(room.mapTexture->GetWidth()), static_cast<float>(room.mapTexture->GetHeight()));

    room.maskFBO = 0;
    room.maskTextureID = 0;

    // Configuração da Matriz Lógica
    room.cellSize = 128.0f;

    float worldWidth = boundsMax.x - boundsMin.x;
    float worldHeight = boundsMax.y - boundsMin.y;

    // Calcula o tamanho da matriz arredondando para cima
    room.gridCols = static_cast<int>(std::ceil(worldWidth / room.cellSize));
    room.gridRows = static_cast<int>(std::ceil(worldHeight / room.cellSize));

    // Inicializa a matriz inteira como 'false' (escuridão)
    room.discoveryGrid.resize(room.gridRows, std::vector<bool>(room.gridCols, false));

    mRenderer->CreateMaskFBO(static_cast<int>(room.size.x), static_cast<int>(room.size.y), room.maskFBO, room.maskTextureID);

    mRooms.push_back(room);
}

void MapManager::UpdateFogOfWar(const std::string& currentLevelID, const Vector2& playerLocalPos, float revealRadius) {
    for (auto& room : mRooms) {
        if (room.levelID == currentLevelID) {
            room.isDiscovered = true;

            Vector2 playablePos = playerLocalPos - room.boundsMin;

            // Descobre a linha e coluna na matriz lógica
            int col = static_cast<int>(playablePos.x / room.cellSize);
            int row = static_cast<int>(playablePos.y / room.cellSize);

            // Proteção de segurança contra estouro de escopo do vetor
            if (col >= 0 && col < room.gridCols && row >= 0 && row < room.gridRows) {

                // Se o jogador acabou de invadir uma célula que ainda não tinha sido visitada
                if (!room.discoveryGrid[row][col]) {
                    room.discoveryGrid[row][col] = true; // Marca como descoberta

                    // Transforma a posição da célula de volta para a coordenada do FBO
                    Vector2 fboPos = playablePos * room.scaleFactor;
                    fboPos.y = room.size.y - fboPos.y; // Inversão para o OpenGL

                    // Carimba o pincel orgânico no FBO
                    mRenderer->BindFramebuffer(room.maskFBO, static_cast<int>(room.size.x), static_cast<int>(room.size.y));
                    mRenderer->DrawFogBrush(fboPos, revealRadius, mBrushTexture, room.size);
                    mRenderer->RestoreDefaultFramebuffer();

                    // Atualiza a textura unificada do menu
                    BakeGlobalMap();
                }
            }
            break;
        }
    }
}

void MapManager::BuildGlobalCanvas() {
    if (mRooms.empty()) return;

    // Inicializa com as coordenadas da primeira sala
    float minX = mRooms[0].mapCanvasPos.x;
    float minY = mRooms[0].mapCanvasPos.y;
    float maxX = mRooms[0].mapCanvasPos.x + mRooms[0].size.x;
    float maxY = mRooms[0].mapCanvasPos.y + mRooms[0].size.y;

    // Descobre as dimensões extremas totais (incluindo valores negativos)
    for (const auto& room : mRooms) {
        if (room.mapCanvasPos.x < minX) minX = room.mapCanvasPos.x;
        if (room.mapCanvasPos.y < minY) minY = room.mapCanvasPos.y;
        if (room.mapCanvasPos.x + room.size.x > maxX) maxX = room.mapCanvasPos.x + room.size.x;
        if (room.mapCanvasPos.y + room.size.y > maxY) maxY = room.mapCanvasPos.y + room.size.y;
    }

    // Cria um offset para empurrar tudo para o espaço positivo
    // Ex: Se o minY for -194, o offset.y será +194.
    Vector2 offset(-minX, -minY);

    // Aplica esse deslocamento permanentemente em todas as salas
    for (auto& room : mRooms) {
        room.mapCanvasPos += offset;
    }

    // O tamanho real do Canvas é a distância entre o mínimo e o máximo
    mCanvasSize = Vector2(maxX - minX, maxY - minY);

    // Cria a imagem gigante com as novas dimensões exatas
    mRenderer->CreateColorFBO(static_cast<int>(mCanvasSize.x), static_cast<int>(mCanvasSize.y), mGlobalFBO, mGlobalTexture);

    BakeGlobalMap();
}

void MapManager::BakeGlobalMap() {
    if (mGlobalFBO == 0) return;

    // Trava o desenho para o nosso Canvas gigante
    mRenderer->BindFramebuffer(mGlobalFBO, static_cast<int>(mCanvasSize.x), static_cast<int>(mCanvasSize.y));

    // Limpa tudo
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    for (const auto& room : mRooms) {
        if (!room.isDiscovered) continue;

        // Adiciona metade do tamanho para que a sala seja desenhada no lugar exato
        Vector2 centerPos = room.mapCanvasPos + (room.size * 0.5f);

        // Desenha a sala na imagem gigante (Atenção ao mCanvasSize.x/y no final)
        mRenderer->DrawMapRoom(centerPos, room.size, room.mapTexture, room.maskTextureID, mCanvasSize.x, mCanvasSize.y);
    }

    mRenderer->RestoreDefaultFramebuffer();
}

bool MapManager::HasRoom(const std::string& levelID) const {
    for (const auto& room : mRooms) {
        if (room.levelID == levelID) return true;
    }
    return false;
}

Vector2 MapManager::GetRoomCanvasPosition(const std::string& levelID) const {
    for (const auto& room : mRooms) {
        if (room.levelID == levelID) return room.mapCanvasPos;
    }
    return Vector2::Zero;
}

void MapManager::LoadInitialRoom(const std::string& levelID, const std::string& texturePath, float scaleFactor, const Vector2& boundsMin, const Vector2& boundsMax) {
    if (HasRoom(levelID)) return;

    LoadRoom(levelID, texturePath, Vector2::Zero, scaleFactor, boundsMin, boundsMax);
}

void MapManager::LoadConnectedRoom(const std::string& levelID, const std::string& texturePath, float scaleFactor,
                                   const Vector2& destBoundsMin, const Vector2& destBoundsMax, const Vector2& destTriggerPos,
                                   const Vector2& originCanvasPos, const Vector2& originBoundsMin, const Vector2& originBoundsMax, const Vector2& originTriggerPos)
{
    if (HasRoom(levelID)) return;

    // Descobre a Direção da Transição na sala de origem
    float distLeft = Math::Abs(originTriggerPos.x - originBoundsMin.x);
    float distRight = Math::Abs(originTriggerPos.x - originBoundsMax.x);
    float distTop = Math::Abs(originTriggerPos.y - originBoundsMin.y);
    float distBottom = Math::Abs(originTriggerPos.y - originBoundsMax.y);

    float minDist = Math::Min(distLeft, distRight);
    minDist = Math::Min(minDist, distTop);
    minDist = Math::Min(minDist, distBottom);

    Vector2 newCanvasPos = Vector2::Zero;

    Vector2 scaledOriginBoundsSize = (originBoundsMax - originBoundsMin) * scaleFactor;
    Vector2 scaledDestBoundsSize = (destBoundsMax - destBoundsMin) * scaleFactor;

    // Algoritmo de Encaixe
    if (minDist == distRight) {
        // TRANSITANDO PARA A DIREITA
        newCanvasPos.x = originCanvasPos.x + scaledOriginBoundsSize.x;
        float originTriggerCanvasY = originCanvasPos.y + (originTriggerPos.y - originBoundsMin.y) * scaleFactor;
        newCanvasPos.y = originTriggerCanvasY - (destTriggerPos.y - destBoundsMin.y) * scaleFactor;
    }
    else if (minDist == distLeft) {
        // TRANSITANDO PARA A ESQUERDA
        newCanvasPos.x = originCanvasPos.x - scaledDestBoundsSize.x;
        float originTriggerCanvasY = originCanvasPos.y + (originTriggerPos.y - originBoundsMin.y) * scaleFactor;
        newCanvasPos.y = originTriggerCanvasY - (destTriggerPos.y - destBoundsMin.y) * scaleFactor;
    }
    else if (minDist == distBottom) {
        // TRANSITANDO PARA BAIXO
        newCanvasPos.y = originCanvasPos.y + scaledOriginBoundsSize.y;
        float originTriggerCanvasX = originCanvasPos.x + (originTriggerPos.x - originBoundsMin.x) * scaleFactor;
        newCanvasPos.x = originTriggerCanvasX - (destTriggerPos.x - destBoundsMin.x) * scaleFactor;
    }
    else if (minDist == distTop) {
        // TRANSITANDO PARA CIMA
        newCanvasPos.y = originCanvasPos.y - scaledDestBoundsSize.y;
        float originTriggerCanvasX = originCanvasPos.x + (originTriggerPos.x - originBoundsMin.x) * scaleFactor;
        newCanvasPos.x = originTriggerCanvasX - (destTriggerPos.x - destBoundsMin.x) * scaleFactor;
    }

    // Carrega a sala na posição calculada
    LoadRoom(levelID, texturePath, newCanvasPos, scaleFactor, destBoundsMin, destBoundsMax);
}

void MapManager::RestoreSavedRoom(const std::string& levelID, const std::string& texturePath, float scaleFactor, const Vector2& canvasPos, const Vector2& boundsMin, int gridCols, int gridRows) {
    if (HasRoom(levelID)) return;

    MapRoom room;
    room.levelID = levelID;
    room.mapCanvasPos = canvasPos;
    room.isDiscovered = false;
    room.scaleFactor = scaleFactor;
    room.boundsMin = boundsMin;

    room.mapTexture = mRenderer->GetTexture(texturePath);
    if (!room.mapTexture) return;

    room.size = Vector2(static_cast<float>(room.mapTexture->GetWidth()), static_cast<float>(room.mapTexture->GetHeight()));

    room.maskFBO = 0;
    room.maskTextureID = 0;

    // Cria o FBO para a sala recuperada do Save
    mRenderer->CreateMaskFBO(static_cast<int>(room.size.x), static_cast<int>(room.size.y), room.maskFBO, room.maskTextureID);

    // Configura a Matriz exatamente como estava no momento do Save
    room.cellSize = 128.0f;
    room.gridCols = gridCols;
    room.gridRows = gridRows;
    room.discoveryGrid.resize(room.gridRows, std::vector<bool>(room.gridCols, false));

    mRooms.push_back(room);
}

void MapManager::ApplyRoomGridData(const std::string& levelID, bool isDiscovered, const std::string& gridData) {
    for (auto& room : mRooms) {
        if (room.levelID == levelID) {
            room.isDiscovered = isDiscovered;

            if (!isDiscovered || gridData.empty()) return;

            // Transforma a String de '1's e '0's de volta para a matriz booleana
            int idx = 0;
            for (int r = 0; r < room.gridRows; ++r) {
                for (int c = 0; c < room.gridCols; ++c) {
                    if (idx < gridData.length()) {
                        room.discoveryGrid[r][c] = (gridData[idx] == '1');
                        idx++;
                    }
                }
            }

            // Remonta o visual borrado no FBO usando a matriz que acabamos de popular
            ReconstructFBOFromGrid(levelID);
            break;
        }
    }
}

void MapManager::ReconstructFBOFromGrid(const std::string& levelID) {
    for (auto& room : mRooms) {
        if (room.levelID == levelID && room.isDiscovered) {

            mRenderer->BindFramebuffer(room.maskFBO, static_cast<int>(room.size.x), static_cast<int>(room.size.y));

            // Percorre toda a matriz lógica buscando blocos verdadeiros
            for (int r = 0; r < room.gridRows; ++r) {
                for (int c = 0; c < room.gridCols; ++c) {
                    if (room.discoveryGrid[r][c]) {

                        // Encontra o centro da célula no espaço do mundo
                        float worldX = (c + 0.5f) * room.cellSize;
                        float worldY = (r + 0.5f) * room.cellSize;

                        Vector2 fboPos = Vector2(worldX, worldY) * room.scaleFactor;
                        fboPos.y = room.size.y - fboPos.y;

                        // Recria o rastro borrado idêntico ao original
                        mRenderer->DrawFogBrush(fboPos, mGame->GetBrushRadius(), mBrushTexture, room.size);
                    }
                }
            }

            mRenderer->RestoreDefaultFramebuffer();
            break;
        }
    }
}
