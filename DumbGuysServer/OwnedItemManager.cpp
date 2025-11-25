#include "OwnedItemManager.h"

OwnedItemManager::OwnedItemManager() {}

void OwnedItemManager::AddNewOwnedList(const std::string& id)
{
    for (auto& cat : categories)
    {
        std::string filename = "owned_" + cat.name + ".csv";

        // 파일이 없으면 헤더 생성
        std::ifstream check(filename);
        bool newFile = !check.is_open();
        check.close();

        std::ofstream file(filename, std::ios::app);
        if (!file.is_open())
        {
            std::cerr << "[OwnedItemManager] Failed to open " << filename << "\n";
            continue;
        }

        if (newFile)
        {
            file << "id";
            for (int i = 0; i < cat.count; ++i)
                file << "," << cat.name << i;
            file << "\n";
        }

        // 기본 아이템 0만 보유
        file << id;
        for (int i = 0; i < cat.count; ++i)
            file << "," << (i == 0 ? 1 : 0);
        file << "\n";
        file.close();
    }
    std::cout << "[OwnedItemManager] Default owned files created for " << id << "\n";
}

bool OwnedItemManager::IsOwned(const std::string& id, const std::string& category, int index)
{
    std::string filename = "owned_" + category + ".csv";
    std::ifstream file(filename);
    if (!file.is_open()) return false;

    std::string line;
    std::getline(file, line); // header skip
    while (std::getline(file, line))
    {
        std::stringstream ss(line);
        std::string token;
        std::getline(ss, token, ',');
        if (token != id) continue;

        for (int i = 0; i <= index; ++i)
            std::getline(ss, token, ',');
        file.close();
        return token == "1";
    }
    file.close();
    return false;
}

void OwnedItemManager::AddOwnedItem(const std::string& id, const std::string& category, int index)
{
    std::string filename = "owned_" + category + ".csv";
    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "[OwnedItemManager] File not found: " << filename << "\n";
        return;
    }

    // 파일 전체 읽기
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(file, line)) lines.push_back(line);
    file.close();

    if (lines.size() < 2) return;

    // 헤더 분석
    std::vector<std::string> headers;
    std::stringstream headerStream(lines[0]);
    std::string h;
    while (std::getline(headerStream, h, ',')) headers.push_back(h);

    // 수정
    for (size_t i = 1; i < lines.size(); ++i)
    {
        std::stringstream ss(lines[i]);
        std::vector<std::string> tokens;
        std::string tok;
        while (std::getline(ss, tok, ',')) tokens.push_back(tok);

        if (tokens.size() == headers.size() && tokens[0] == id)
        {
            tokens[index + 1] = "1"; // index 0 -> column 1
            std::ostringstream os;
            for (size_t j = 0; j < tokens.size(); ++j)
            {
                os << tokens[j];
                if (j + 1 < tokens.size()) os << ",";
            }
            lines[i] = os.str();
            break;
        }
    }

    // 다시 쓰기
    std::ofstream out(filename);
    for (auto& l : lines) out << l << "\n";
    out.close();

    std::cout << "[OwnedItemManager] Item unlocked: " << category << " index " << index << " for " << id << "\n";
}

std::vector<int> OwnedItemManager::GetOwnedList(const std::string& id, const std::string& category)
{
    std::vector<int> result;

    std::string filename = "owned_" + category + ".csv";
    std::ifstream file(filename);

    if (!file.is_open())
    {
        std::cerr << "[OwnedItemManager] File not found: " << filename << "\n";
        return result;
    }

    std::string line;
    std::getline(file, line); // header skip

    while (std::getline(file, line))
    {
        std::stringstream ss(line);
        std::string token;

        // 첫 번째 값(id)
        std::getline(ss, token, ',');
        if (token != id)
            continue;

        // 나머지 값을 읽음
        while (std::getline(ss, token, ','))
        {
            result.push_back(std::stoi(token));
        }
        break;
    }

    file.close();
    return result;
}

bool OwnedItemManager::HasOwnedItem(const std::string& id, const std::string& category, int index)
{
    std::string filename = "owned_" + category + ".csv";
    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "[OwnedItemManager] File not found: " << filename << "\n";
        return false;
    }

    std::string line;
    std::getline(file, line); // header skip

    while (std::getline(file, line))
    {
        std::stringstream ss(line);
        std::string token;

        // 첫 번째 값은 계정 ID
        std::getline(ss, token, ',');
        if (token != id) continue;

        // index 번째 아이템까지 스킵
        for (int i = 0; i <= index; ++i)
        {
            if (!std::getline(ss, token, ',')) break;
        }

        file.close();
        return token == "1";
    }

    file.close();
    return false;
}
