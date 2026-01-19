#pragma once
#include <string>
#include <unordered_map>
#include <fstream>
#include <iostream>
#include <sstream>

struct AppearanceData
{
    std::string id;
    int FullBody;
    int Hat;
    int Accessories;
    int Faces;
    int Glasses;
    int Gloves;
    int Hairstyle;
    int Outerwear;
    int Pants;
    int Shoes;
};

class AppearanceManager
{
private:
    const std::string filename = "appearance.csv";
    std::unordered_map<std::string, AppearanceData> appearanceMap;

public:
    AppearanceManager();
    void Load();
    void Save();
    bool Exists(const std::string& id);
    void AddNewAppearance(const std::string& id);
    AppearanceData* GetAppearance(const std::string& id);
};
