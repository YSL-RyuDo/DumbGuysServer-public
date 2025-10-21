#include "AppearanceManager.h"

AppearanceManager::AppearanceManager() { Load(); }

void AppearanceManager::Load()
{
    appearanceMap.clear();
    std::ifstream file(filename);
    if (!file.is_open()) return;

    std::string line;
    std::getline(file, line); // header skip
    while (std::getline(file, line))
    {
        std::stringstream ss(line);
        AppearanceData data;
        std::string value;

        std::getline(ss, data.id, ',');
        ss >> data.FullBody; ss.ignore();
        ss >> data.Hat; ss.ignore();
        ss >> data.Accessories; ss.ignore();
        ss >> data.Faces; ss.ignore();
        ss >> data.Glasses; ss.ignore();
        ss >> data.Gloves; ss.ignore();
        ss >> data.Hairstyle; ss.ignore();
        ss >> data.Outerwear; ss.ignore();
        ss >> data.Pants; ss.ignore();
        ss >> data.Shoes;

        appearanceMap[data.id] = data;
    }
    file.close();
}

void AppearanceManager::Save()
{
    std::ofstream file(filename);
    file << "id,FullBody,Hat,Accessories,Faces,Glasses,Gloves,Hairstyle,Outerwear,Pants,Shoes\n";

    for (auto& pair : appearanceMap)
    {
        const auto& data = pair.second;
        file << data.id << ","
            << data.FullBody << ","
            << data.Hat << ","
            << data.Accessories << ","
            << data.Faces << ","
            << data.Glasses << ","
            << data.Gloves << ","
            << data.Hairstyle << ","
            << data.Outerwear << ","
            << data.Pants << ","
            << data.Shoes << "\n";
    }

    file.close();
}


bool AppearanceManager::Exists(const std::string& id)
{
    return appearanceMap.find(id) != appearanceMap.end();
}

void AppearanceManager::AddNewAppearance(const std::string& id)
{
    AppearanceData data{ id, 0,0,0,0,0,0,0,0,0,0 };
    appearanceMap[id] = data;
    Save();
}

AppearanceData* AppearanceManager::GetAppearance(const std::string& id)
{
    if (!Exists(id)) return nullptr;
    return &appearanceMap[id];
}
