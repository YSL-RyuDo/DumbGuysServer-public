#pragma once
#include <string>
#include <unordered_map>
#include <fstream>
#include <iostream>

struct PlayerData
{
    std::string id;
    std::string nickname;
    int level = 1;
    int exp = 0;
    int coin0 = 0;
    int coin1 = 0;
    int rank = 0;
    int rankScore = 0;
    int icon = 0;
    int iconBorder = 0;
    bool tutorialDone = false;
    int tutorialStage = 0;
};

class PlayerDataManager
{
private:
    const std::string filename = "PlayerData.csv";

public:
    PlayerDataManager();
    void Load();
    void Save();
    bool Exists(const std::string& id);
    void AddNewPlayer(const std::string& id, const std::string& nickname);
    PlayerData* GetByNickname(const std::string& nickname);

private:
    std::unordered_map<std::string, PlayerData> players;
};
