#pragma once
#include <string>
#include <unordered_map>

class UserAccountManager
{
private:
    std::unordered_map<std::string, std::pair<std::string, std::string>> accounts; // id -> (pw, nickname)
    const std::string filename = "UserAccount.csv";

public:
    UserAccountManager();
    void Load();
    void Save();
    bool Exists(const std::string& id);
    bool NicknameExists(const std::string& nickname);
    bool ValidateLogin(const std::string& id, const std::string& pw);
    bool AddAccount(const std::string& id, const std::string& pw, const std::string& nickname);
    std::string GetNicknameById(const std::string& id);
    std::string GetIdByNickname(const std::string& nickname);
};
