#pragma once
#include <string>
#include <unordered_map>
#include <fstream>
#include <iostream>

struct PlayerData
{
    std::string id;
    std::string nickname;
    int level;
    int exp;
    int coin0;
    int coin1;
    int rank;
    int rankScore;
    int icon;
    int iconBorder;
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