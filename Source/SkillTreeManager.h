//
// Created by roger on 10/04/2026.
//

#pragma once

#include <string>
#include <vector>
#include <unordered_map>

struct SkillNode {
    std::string id;
    std::string name;
    std::string skillType;
    std::string description;
    std::string howToUse;
    int goldCost;
    int earthStoneCost;
    int fireStoneCost;
    int iceStoneCost;
    int lightningStoneCost;

    std::vector<std::vector<std::string>> prerequisites;

    std::string effectType;
    std::string effectValue;

    bool isUnlocked = false;
};

class SkillTreeManager {
public:
    SkillTreeManager();

    bool ArePrerequisitesMet(const std::string& skillId);
    bool CanUnlock(const std::string& skillId, class Player* player);
    SkillNode* UnlockSkill(const std::string& skillId);
    SkillNode* GetNode(const std::string& id);

    std::vector<std::string> GetUnlockedNodesIDs() const;
    void LoadUnlockedNodes(const std::vector<std::string>& unlockedIds, class PlayerSkillManager* playerSkills);

private:
    void LoadTreeFromJson(const std::string& filepath);
    std::unordered_map<std::string, SkillNode> mSkillTree;
};
