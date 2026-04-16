//
// Created by roger on 10/04/2026.
//

#include "SkillTreeManager.h"
#include "PlayerSkillManager.h"
#include "Actors/Player.h"
#include "Json.h"
#include <fstream>

SkillTreeManager::SkillTreeManager() {
    LoadTreeFromJson("../SkillTree/skills.json");
}

bool SkillTreeManager::ArePrerequisitesMet(const std::string &skillId) {
    auto it = mSkillTree.find(skillId);
    if (it == mSkillTree.end()) {
        return false;
    }

    const SkillNode& node = it->second;

    // Verifica se os nós anteriores estão desbloqueados
    for (const auto& orGroup : node.prerequisites) {
        bool isGroupSatisfied = false;

        for (const auto& reqId : orGroup) {
            auto reqIt = mSkillTree.find(reqId);
            if (reqIt != mSkillTree.end() && reqIt->second.isUnlocked) {
                isGroupSatisfied = true;
                break;
            }
        }

        if (!isGroupSatisfied) {
            return false;
        }
    }

    return true;
}

bool SkillTreeManager::CanUnlock(const std::string &skillId, Player* player) {
    auto it = mSkillTree.find(skillId);
    if (it == mSkillTree.end()) {
        return false;
    }

    const SkillNode& node = it->second;

    // Se já está desbloqueada ou pontos insuficientes
    if (node.isUnlocked ||
        player->GetMoney() < node.goldCost ||
        player->GetStone("earthStone") < node.earthStoneCost ||
        player->GetStone("fireStone") < node.fireStoneCost ||
        player->GetStone("iceStone") < node.iceStoneCost ||
        player->GetStone("lightningStone") < node.lightningStoneCost)
    {
        return false;
    }

    for (const auto& orGroup : node.prerequisites) {
        bool isGroupSatisfied = false;
        // Passa pelos caminhos possíveis dentro deste grupo (OR)
        for (const auto& reqId : orGroup) {
            auto reqIt = mSkillTree.find(reqId);

            // Se achou o nó e ele está desbloqueado, o grupo inteiro passou!
            if (reqIt != mSkillTree.end() && reqIt->second.isUnlocked) {
                isGroupSatisfied = true;
                break; // Não precisa checar os outros caminhos (OR) deste grupo
            }
        }

        // Se checamos o grupo inteiro e NENHUM caminho estava liberado, a habilidade não pode ser comprada
        if (!isGroupSatisfied) {
            return false;
        }
    }

    return true;
}


SkillNode* SkillTreeManager::UnlockSkill(const std::string &skillId) {
    auto it = mSkillTree.find(skillId);
    if (it != mSkillTree.end()) {
        it->second.isUnlocked = true;
        return &(it->second);
    }
    return nullptr;
}

SkillNode* SkillTreeManager::GetNode(const std::string &id) {
    auto it = mSkillTree.find(id);
    if (it != mSkillTree.end()) {
        return &(it->second);
    }
    return nullptr;
}

void SkillTreeManager::LoadTreeFromJson(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        SDL_Log("Erro: Nao foi possivel abrir o arquivo da arvore de habilidades: %s", filepath.c_str());
        return;
    }

    nlohmann::json j;
    file >> j;

    mSkillTree.clear();

    if (j.contains("skills") && j["skills"].is_array()) {
        for (const auto& item : j["skills"]) {
            SkillNode node;

            // Textos básicos
            node.id = item.value("id", "undefined");
            node.name = item.value("name", "Sem Nome");
            node.skillType = item.value("skillType", "Geral");
            node.description = item.value("description", "Sem descrição.");
            node.howToUse = item.value("howToUse", "");

            // Custos
            if (item.contains("costs")) {
                const auto& costs = item["costs"];
                node.goldCost = costs.value("gold", 0);
                node.earthStoneCost = costs.value("earthStone", 0);
                node.fireStoneCost = costs.value("fireStone", 0);
                node.iceStoneCost = costs.value("iceStone", 0);
                node.lightningStoneCost = costs.value("lightningStone", 0);
            }

            // Efeitos
            if (item.contains("effect")) {
                const auto& effect = item["effect"];
                node.effectType = effect.value("type", "");
                node.effectValue = effect.value("value", "");
            }

            // Pré-requisitos
            if (item.contains("prerequisites") && item["prerequisites"].is_array()) {
                for (const auto& orGroupJson : item["prerequisites"]) {
                    std::vector<std::string> orGroup;

                    if (orGroupJson.is_array()) {
                        for (const auto& req : orGroupJson) {
                            orGroup.push_back(req.get<std::string>());
                        }
                    } else if (orGroupJson.is_string()) {
                        orGroup.push_back(orGroupJson.get<std::string>());
                    }

                    node.prerequisites.push_back(orGroup);
                }
            }

            node.isUnlocked = false;

            mSkillTree[node.id] = node;
        }
    }
}

std::vector<std::string> SkillTreeManager::GetUnlockedNodesIDs() const {
    std::vector<std::string> unlocked;
    for (const auto& pair : mSkillTree) {
        if (pair.second.isUnlocked) {
            unlocked.push_back(pair.first);
        }
    }
    return unlocked;
}

void SkillTreeManager::LoadUnlockedNodes(const std::vector<std::string> &unlockedIds, class PlayerSkillManager* playerSkills) {
    for (const auto& id : unlockedIds) {
        auto it = mSkillTree.find(id);
        if (it != mSkillTree.end()) {
            it->second.isUnlocked = true;
            playerSkills->ApplySkillEffect(it->second.effectType, it->second.effectValue);
        }
    }
}
