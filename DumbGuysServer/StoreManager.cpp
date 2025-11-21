#include "StoreManager.h"
#include <fstream>
#include <sstream>
#include <iostream>

std::vector<StoreItem> StoreManager::LoadStoreCategory(const std::string& category)
{
    std::vector<StoreItem> result;

    std::string filePath = CategoryToFile(category);

    if (filePath.empty())
    {
        std::cout << "[Store] Unknown category: " << category << std::endl;
        return result;
    }

    std::ifstream file(filePath);
    if (!file.is_open())
    {
        std::cout << "[Store] Failed to open CSV: " << filePath << std::endl;
        return result;
    }

    std::string line;
    bool first = true;

    while (std::getline(file, line))
    {
        if (first) { first = false; continue; }

        std::stringstream ss(line);
        std::string idStr, nameStr, priceStr;

        getline(ss, idStr, ',');
        getline(ss, nameStr, ',');
        getline(ss, priceStr, ',');

        StoreItem item;
        item.id = std::stoi(idStr);
        item.name = nameStr;
        item.price = std::stoi(priceStr);

        result.push_back(item);
    }

    return result;
}


std::string StoreManager::CategoryToFile(const std::string& category)
{
    if (category == "Accessories") return "shop_Accessories.csv";
    if (category == "Faces") return "shop_Faces.csv";
    if (category == "FullBody") return "shop_Fullbody.csv";
    if (category == "Glasses") return "shop_Glasses.csv";
    if (category == "Gloves") return "shop_Gloves.csv";
    if (category == "Hairstyle") return "shop_Hairstyle.csv";
    if (category == "Hat") return "shop_Hat.csv";
    if (category == "Outerwear") return "shop_Outerwear.csv";
    if (category == "Pants") return "shop_Pants.csv";
    if (category == "Shoes") return "shop_Shoes.csv";

    return "";
}

StoreItem StoreManager::GetItem(const std::string& category, int id)
{
    std::vector<StoreItem> items = LoadStoreCategory(category);

    for (const auto& item : items)
    {
        if (item.id == id)
            return item;
    }

    // 찾지 못한 경우 기본값 반환
    StoreItem empty;
    empty.id = -1;
    empty.name = "";
    empty.price = 0;
    return empty;
}
