//
// Created by roger on 07/06/2026.
//

#pragma once

#include "../UIElements/UIScreen.h"
#include "../Math.h"

class MapMenu : public UIScreen {
public:
    MapMenu(class Game* game, const std::string& fontName, bool isClosable = true);

    void Update(float deltaTime) override;
    void Draw(class Renderer* renderer) override;
    void Close() override;

    // Overrides de Input para navegação no mapa
    void HandleKeyPress(int key, int controllerButton, int leftControllerAxisY, int leftControllerAxisX, int rightControllerAxisY, int rightControllerAxisX) override;
    void HandleMousePress(const Vector2& virtualMousePos) override;
    void HandleMouseRelease(const Vector2& virtualMousePos) override;
    void HandleMouseMotion(const Vector2& virtualMousePos) override;

private:
    class Texture* mPlayerIcon;
    class UIFont* mIconFont;
    UIText* mZoomTutorial;

    Vector2 mTargetPan;
    Vector2 mCurrentPan;
    float mTargetZoom;
    float mCurrentZoom;

    // Controle de Mouse Drag
    bool mIsDragging;
    Vector2 mLastMousePos;
};