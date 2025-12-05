//
// Created by roger on 05/12/2025.
//

#pragma once

#include <string>
#include "UIElements/UIScreen.h"

class RadialMenu : public UIScreen {
public:
    // radius: distância do centro até os botões
    RadialMenu(class Game* game, const std::string& fontName, float radius = 200.0f);
    ~RadialMenu() override;

    // Sobrescrevemos para não permitir movimentação padrão de vizinhos
    void HandleKeyPress(int key, int controllerButton, int leftControllerAxisY, int leftControllerAxisX, int rightControllerAxisY, int rightControllerAxisX) override;

    // Sobrescrevemos para detectar ângulo em relação ao centro da tela
    void HandleMouseMotion(const Vector2& virtualMousePos) override;

    // Função auxiliar para adicionar opções facilmente
    void AddRadialOption(const std::string& name, std::function<void()> onClick, const std::string& iconPath = "");

    void SetSelectedOption(int index);

    void Close() override;

    void Draw(class Renderer* renderer) override;

private:
    float mRadius;
    Vector2 mCenterPos;

    // Recalcula a posição de todos os botões em círculo
    void RearrangeButtons();

    // Converte um vetor de input (x, y) em um índice de botão selecionado
    int GetIndexFromInput(float x, float y);
};

