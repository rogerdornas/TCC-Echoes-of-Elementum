//
// Created by roger on 20/01/2026.
//

#pragma once

#include "Component.h"
#include "ColliderComponent.h"
#include <vector>
#include <string>

// Estrutura para guardar os dados extras
struct CombatBox
{
    std::string tag;
    bool isHitbox; // true = Causa Dano, false = Recebe Dano
    int damage;
    bool active;

    ColliderComponent* collider = nullptr;
};

struct HitResult
{
    bool isValid = false;       // Houve colisão?
    int damage = 0;             // Dano calculado
    std::string hitTag;         // Nome da caixa que bateu (Ex: "Espada")
    std::string hurtTag;        // Nome da caixa que apanhou (Ex: "Cabeca")
    class Actor* victim = nullptr; // Ponteiro para quem apanhou
};

class CombatBoxComponent: public Component
{
public:
    CombatBoxComponent(class Actor* owner);
    ~CombatBoxComponent();

    void Update(float deltaTime) override;

    std::vector<CombatBox>& GetBoxes() { return mBoxes; }
    CombatBox* GetBox(const std::string& tag);

    void SetDebugDraw(bool enabled);

    // Fábricas
    void AddAABBBox(const std::string& tag, bool isHitbox, Vector2 min, Vector2 max, Vector2 offset = Vector2::Zero, int damage = 0);
    void AddOBBBox(const std::string& tag, bool isHitbox, Vector2 halfSize, Vector2 offset = Vector2::Zero, int damage = 0);

    // Lógica
    void SetBoxActive(const std::string& tag, bool active);
    void SetAllBoxesActive(bool active);
    void SetBoxOffset(const std::string& tag, Vector2 offset);
    void SetBoxHalfSize(const std::string& tag, Vector2 halfSize);

    HitResult CheckAttackAgainst(CombatBoxComponent* target);
    HitResult CheckReceiveAttack(CombatBoxComponent* attacker);

private:
    std::vector<CombatBox> mBoxes;
    bool mDebugDraw;
};
