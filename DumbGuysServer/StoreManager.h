#pragma once
#include <vector>
#include <string>

struct StoreItem
{
    int id;
    std::string name;
    int price;
};

class StoreManager
{
public:
    StoreManager() {}
    std::vector<StoreItem> LoadStoreCategory(const std::string& category);
    std::string CategoryToFile(const std::string& category);
    StoreItem GetItem(const std::string& category, int id);
};
