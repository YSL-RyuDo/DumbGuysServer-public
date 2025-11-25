#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

class OwnedItemManager
{
private:
    struct Category
    {
        std::string name;
        int count;
    };

    std::vector<Category> categories = {
        {"FullBody",3},{"Hat",6},{"Accessories",6},{"Faces",4},{"Glasses",3},
        {"Gloves",3},{"Hairstyle",4},{"Outerwear",5},{"Pants",4},{"Shoes",5}
    };

public:
    OwnedItemManager();

    // 회원가입 시 기본 CSV 생성
    void AddNewOwnedList(const std::string& id);

    // 아이템 보유 여부 확인
    bool IsOwned(const std::string& id, const std::string& category, int index);

    // 아이템 획득 처리
    void AddOwnedItem(const std::string& id, const std::string& category, int index);

    std::vector<int> GetOwnedList(const std::string& id, const std::string& category);

    bool HasOwnedItem(const std::string& id, const std::string& category, int index);
};
