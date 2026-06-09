//
// Created by roger on 07/06/2026.
//

#pragma once

#include <vector>
#include <string>
#include "Math.h"

class Renderer;
class Texture;

struct MapRoom {
    std::string levelID;
    Vector2 mapCanvasPos;
    Vector2 size;          // Largura e altura da arte da sala
    Texture* mapTexture;
    unsigned int maskFBO;
    unsigned int maskTextureID;
    bool isDiscovered;     // Se o jogador já pisou aqui
    float scaleFactor;     // Ex: Se a fase tem 2000px de largura e a arte do mapa tem 200px, a escala é 0.1f
    Vector2 boundsMin;
    float cellSize;        // Ex: 128.0f unidades do mundo por bloco
    int gridCols;
    int gridRows;
    std::vector<std::vector<bool>> discoveryGrid; // Matriz que guarda o que foi visto
};

class MapManager {
public:
    MapManager(class Game* game, Renderer* renderer);
    ~MapManager();

    // Registra uma nova sala no sistema
    void LoadRoom(const std::string& levelID, const std::string& texturePath, const Vector2& canvasPos, float scaleFactor, const Vector2& boundsMin, const Vector2& boundsMax);

    // Pinta o "pincel" no FBO da sala atual.
    void UpdateFogOfWar(const std::string& currentLevelID, const Vector2& playerLocalPos, float revealRadius);

    void BuildGlobalCanvas();
    void BakeGlobalMap();

    unsigned int GetGlobalMapTexture() const { return mGlobalTexture; }
    Vector2 GetGlobalMapSize() const { return mCanvasSize; }

    const std::vector<MapRoom>& GetRooms() const { return mRooms; }

    bool HasRoom(const std::string& levelID) const;

    // Retorna a posição do Canvas de uma sala específica
    Vector2 GetRoomCanvasPosition(const std::string& levelID) const;

    // Carrega a primeira sala (posição zero)
    void LoadInitialRoom(const std::string& levelID, const std::string& texturePath, float scaleFactor, const Vector2& boundsMin, const Vector2& boundsMax);

    // Conecta uma nova sala a partir dos dados da sala anterior
    void LoadConnectedRoom(const std::string& levelID, const std::string& texturePath, float scaleFactor,
                           const Vector2& destBoundsMin, const Vector2& destBoundsMax, const Vector2& destTriggerPos,
                           const Vector2& originCanvasPos, const Vector2& originBoundsMin, const Vector2& originBoundsMax, const Vector2& originTriggerPos);

    // Restaura uma sala exatamente como estava no SaveData
    void RestoreSavedRoom(const std::string& levelID, const std::string& texturePath, float scaleFactor, const Vector2& canvasPos, const Vector2& boundsMin, int gridCols, int gridRows);

    void ApplyRoomGridData(const std::string& levelID, bool isDiscovered, const std::string& gridData);

    void ReconstructFBOFromGrid(const std::string& levelID);

private:
    class Game* mGame;
    Renderer* mRenderer;
    Texture* mBrushTexture; // Textura de um círculo com gradiente branco para o centro
    std::vector<MapRoom> mRooms;

    unsigned int mGlobalFBO;
    unsigned int mGlobalTexture;
    Vector2 mCanvasSize;
};