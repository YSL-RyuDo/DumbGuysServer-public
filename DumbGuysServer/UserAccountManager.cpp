#include "UserAccountManager.h"
#include <fstream>
#include <sstream>
#include <iostream>

UserAccountManager::UserAccountManager()
{
    Load();
}

void UserAccountManager::Load()
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cout << "[Account] No UserAccount.csv found. It will be created later.\n";
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
        std::string id, pw, nick;
        std::getline(ss, id, ',');
        std::getline(ss, pw, ',');
        std::getline(ss, nick, ',');

        accounts[id] = { pw, nick };
    }

    std::cout << "[Account] Loaded " << accounts.size() << " accounts.\n";
    file.close();
}

void UserAccountManager::Save()
{
    std::ofstream file(filename, std::ios::trunc);
    if (!file.is_open())
    {
        std::cerr << "[Account] Failed to open file for saving.\n";
        return;
    }

    file << "id,password,nickname\n";

    for (auto& entry : accounts)
    {
        const std::string& id = entry.first;
        const auto& pair = entry.second;

        file << id << "," << pair.first << "," << pair.second << "\n";
    }

    file.close();
    std::cout << "[Account] Saved " << accounts.size() << " accounts.\n";
}

bool UserAccountManager::Exists(const std::string& id)
{
    return accounts.find(id) != accounts.end();
}

bool UserAccountManager::NicknameExists(const std::string& nickname)
{
    for (auto& pair : accounts)
    {
        if (pair.second.second == nickname)
            return true;
    }
    return false;
}

//bool UserAccountManager::ValidateLogin(const std::string& id, const std::string& pw)
//{
//    auto it = accounts.find(id);
//    return (it != accounts.end() && it->second.first == pw);
//}

bool UserAccountManager::ValidateLogin(const std::string& id, const std::string& pw)
{
    auto it = accounts.find(id);
    if (it == accounts.end()) return false;
    if (it->second.first != pw) return false;

    const std::string& nickname = it->second.second;
    if (loggedInUsers.find(nickname) != loggedInUsers.end())
    {
        std::cout << "[Login] Duplicate login detected for: " << nickname << std::endl;
        return false; // 이미 로그인 중
    }

    loggedInUsers.insert(nickname);
    return true;
}


bool UserAccountManager::IsLoggedIn(const std::string& nickname)
{
    return loggedInUsers.find(nickname) != loggedInUsers.end();
}

void UserAccountManager::SetLoggedIn(const std::string& nickname, bool state)
{
    if (state)
        loggedInUsers.insert(nickname);
    else
        loggedInUsers.erase(nickname);
}

bool UserAccountManager::AddAccount(const std::string& id, const std::string& pw, const std::string& nickname)
{
    if (Exists(id) || NicknameExists(nickname))
        return false;

    accounts[id] = { pw, nickname };
    Save();
    return true;
}

std::string UserAccountManager::GetNicknameById(const std::string& id)
{
    auto it = accounts.find(id);
    if (it != accounts.end())
        return it->second.second; // (password, nickname)
    return "";
}

std::string UserAccountManager::GetIdByNickname(const std::string& nickname)
{
    for (auto it = accounts.begin(); it != accounts.end(); ++it)
    {
        const std::string& storedNickname = it->second.second; // (password, nickname)
        if (storedNickname == nickname)
            return it->first;
    }
    return "";
}
