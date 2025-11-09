#include "PlayerDataManager.h"
#include <sstream>

PlayerDataManager::PlayerDataManager()
{
    Load();
}

void PlayerDataManager::Load()
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cout << "[PlayerData] No existing file found. A new one will be created." << std::endl;
        return;
    }

    std::string line;
    bool isHeader = true;
    while (std::getline(file, line))
    {
        if (line.empty()) continue;
        if (isHeader)
        {
            isHeader = false;
            continue;
        }

        std::stringstream ss(line);
        PlayerData data;

        std::getline(ss, data.id, ',');
        std::getline(ss, data.nickname, ',');

        ss >> data.level; ss.ignore();
        ss >> data.exp; ss.ignore();
        ss >> data.coin0; ss.ignore();
        ss >> data.coin1; ss.ignore();
        ss >> data.rank; ss.ignore();
        ss >> data.rankScore; ss.ignore();
        ss >> data.icon; ss.ignore();
        ss >> data.iconBorder; ss.ignore();

        int doneFlag = 0;
        int stage = 0;
        ss >> doneFlag; ss.ignore();
        ss >> stage;

        data.tutorialDone = (doneFlag != 0);
        data.tutorialStage = stage;

        players[data.id] = data;
    }

    std::cout << "[PlayerData] Loaded " << players.size() << " records." << std::endl;
    file.close();
}

void PlayerDataManager::Save()
{
    std::ofstream file(filename, std::ios::trunc);
    if (!file.is_open())
    {
        std::cerr << "[PlayerData] Failed to open file for saving." << std::endl;
        return;
    }

    file << "id,nickname,level,exp,coin0,coin1,rank,rankScore,icon,iconBorder,tutorialDone,tutorialStage\n";

    for (auto& entry : players)
    {
        const auto& data = entry.second;
        file << data.id << ","
            << data.nickname << ","
            << data.level << ","
            << data.exp << ","
            << data.coin0 << ","
            << data.coin1 << ","
            << data.rank << ","
            << data.rankScore << ","
            << data.icon << ","
            << data.iconBorder << ","
            << (data.tutorialDone ? 1 : 0) << ","
            << data.tutorialStage << "\n";
    }

    file.close();
    std::cout << "[PlayerData] Saved " << players.size() << " records." << std::endl;
}

bool PlayerDataManager::Exists(const std::string& id)
{
    return players.find(id) != players.end();
}

void PlayerDataManager::AddNewPlayer(const std::string& id, const std::string& nickname)
{
    if (Exists(id))
    {
        std::cout << "[PlayerData] Player already exists: " << id << std::endl;
        return;
    }

    PlayerData data;
    data.id = id;
    data.nickname = nickname;
    data.level = 1;
    data.exp = 0;
    data.coin0 = 0;
    data.coin1 = 0;
    data.rank = 0;
    data.rankScore = 0;
    data.icon = 0;
    data.iconBorder = 0;
    data.tutorialDone = false;
    data.tutorialStage = 0;

    players[id] = data;
    Save();

    std::cout << "[PlayerData] Created new player data for: " << nickname << std::endl;
}

PlayerData* PlayerDataManager::GetByNickname(const std::string& nickname)
{
    for (auto& entry : players)
    {
        if (entry.second.nickname == nickname)
            return &entry.second;
    }
    return nullptr;
}
