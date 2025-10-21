#include "ClientHandler.h"
#include <iostream>
#include <vector>
#include <thread>
#include <cctype>
#include <climits>


ClientHandler::ClientHandler(
    SOCKET clientSocket,
    sockaddr_in clientAddr,
    UserAccountManager* accountMgr,
    PlayerDataManager* playerDataMgr,
    AppearanceManager* appearanceMgr,
    OwnedItemManager* ownedItemMgr,
    RoomManager* roomMgr,
    MatchManager* matchMgr
)
    : socket_(clientSocket),
    addr_(clientAddr),
    accountMgr(accountMgr),
    playerDataMgr(playerDataMgr),
    appearanceMgr(appearanceMgr),
    ownedItemMgr(ownedItemMgr),
    roomMgr(roomMgr),
    matchMgr(matchMgr)
{
}

bool ClientHandler::recvFull(void* buffer, int length)
{
    char* ptr = (char*)buffer;
    int received = 0;
    while (received < length)
    {
        int n = recv(socket_, ptr + received, length - received, 0);
        if (n <= 0) return false;
        received += n;
    }
    return true;
}

bool ClientHandler::sendFull(const void* buffer, int length)
{
    const char* ptr = (const char*)buffer;
    int sent = 0;
    while (sent < length)
    {
        int n = send(socket_, ptr + sent, length - sent, 0);
        if (n <= 0) return false;
        sent += n;
    }
    return true;
}

static std::string findStringField(const std::string& json, const std::string& key)
{
    size_t pos = json.find("\"" + key + "\":\"");
    if (pos == std::string::npos) return "";
    pos += key.size() + 4;
    size_t end = json.find("\"", pos);
    if (end == std::string::npos) return "";
    return json.substr(pos, end - pos);
}

static bool findIntField(const std::string& json, const std::string& key, int& outVal)
{
    size_t pos = json.find("\"" + key + "\":");
    if (pos == std::string::npos) return false;
    pos += key.size() + 3;
    while (pos < json.size() && std::isspace(static_cast<unsigned char>(json[pos]))) ++pos;

    bool neg = false;
    if (pos < json.size() && json[pos] == '-') { neg = true; ++pos; }

    long long val = 0;
    bool hasDigit = false;
    while (pos < json.size() && std::isdigit(static_cast<unsigned char>(json[pos])))
    {
        hasDigit = true;
        val = val * 10 + (json[pos] - '0');
        ++pos;
    }

    if (!hasDigit) return false;
    if (neg) val = -val;
    if (val < INT_MIN || val > INT_MAX) return false;
    outVal = static_cast<int>(val);
    return true;
}

static bool findFloatField(const std::string& json, const std::string& key, float& outVal)
{
    size_t pos = json.find("\"" + key + "\":");
    if (pos == std::string::npos) return false;
    pos += key.size() + 3;

    // 공백 스킵
    while (pos < json.size() && std::isspace(static_cast<unsigned char>(json[pos]))) ++pos;

    size_t end = pos;
    // 숫자, 소수점, 부호, e/E 모두 허용
    while (end < json.size() &&
        (std::isdigit(static_cast<unsigned char>(json[end])) ||
            json[end] == '.' || json[end] == '-' || json[end] == 'e' || json[end] == 'E' || json[end] == '+'))
    {
        ++end;
    }

    std::string numStr = json.substr(pos, end - pos);
    try {
        outVal = std::stof(numStr);
        return true;
    }
    catch (...) {
        return false;
    }
}


void ClientHandler::Run()
{
    char addrStr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &addr_.sin_addr, addrStr, INET_ADDRSTRLEN);
    int port = ntohs(addr_.sin_port);
    std::cout << "[+] Client connected: " << addrStr << ":" << port << std::endl;

    while (true)
    {
        uint32_t len = 0;
        if (!recvFull(&len, 4)) break;

        std::vector<char> buf(len);
        if (!recvFull(buf.data(), len)) break;

        std::string json(buf.begin(), buf.end());
        if (json.find("\"cmd\":\"player_move\"") == std::string::npos)
            std::cout << "[Recv] From " << addrStr << ":" << port << ": " << json << std::endl;


        std::string response;

        // --- 회원가입 ---
        if (json.find("\"cmd\":\"signup\"") != std::string::npos)
        {
            std::string id = findStringField(json, "id");
            std::string pw = findStringField(json, "password");
            std::string nick = findStringField(json, "nickname");

            if (id.empty() || pw.empty() || nick.empty())
            {
                response = R"({"type":"signup_result","result":false,"msg":"missing fields"})";
            }
            else if (accountMgr->Exists(id))
            {
                response = R"({"type":"signup_result","result":false,"msg":"id already exists"})";
            }
            else if (accountMgr->NicknameExists(nick))
            {
                response = R"({"type":"signup_result","result":false,"msg":"nickname already exists"})";
            }
            else
            {
                accountMgr->AddAccount(id, pw, nick);
                playerDataMgr->AddNewPlayer(id, nick);
                appearanceMgr->AddNewAppearance(id);
                ownedItemMgr->AddNewOwnedList(id);
                response = R"({"type":"signup_result","result":true,"msg":"signup ok"})";
            }
        }
        // --- 로그인 ---
        else if (json.find("\"cmd\":\"login\"") != std::string::npos)
        {
            std::string id = findStringField(json, "id");
            std::string pw = findStringField(json, "password");

            if (accountMgr->ValidateLogin(id, pw))
            {
                std::string nickname = accountMgr->GetNicknameById(id);
                response = "{";
                response += "\"type\":\"login_result\",";
                response += "\"result\":true,";
                response += "\"msg\":\"login ok\",";
                response += "\"nickname\":\"" + nickname + "\"";
                response += "}";
            }
            else
            {
                response = R"({"type":"login_result","result":false,"msg":"invalid credentials"})";
            }
        }
        // --- 플레이어 정보 요청 ---
        else if (json.find("\"cmd\":\"get_player_info\"") != std::string::npos)
        {
            std::string nick = findStringField(json, "nickname");
            if (nick.empty())
            {
                response = R"({"type":"player_info","result":false,"msg":"missing nickname"})";
            }
            else
            {
                PlayerData* data = playerDataMgr->GetByNickname(nick);
                if (data)
                {
                    response = "{";
                    response += "\"type\":\"player_info\",";
                    response += "\"nickname\":\"" + data->nickname + "\",";
                    response += "\"level\":" + std::to_string(data->level) + ",";
                    response += "\"exp\":" + std::to_string(data->exp) + ",";
                    response += "\"coin0\":" + std::to_string(data->coin0) + ",";
                    response += "\"coin1\":" + std::to_string(data->coin1) + ",";
                    response += "\"rank\":" + std::to_string(data->rank) + ",";
                    response += "\"rankScore\":" + std::to_string(data->rankScore) + ",";
                    response += "\"icon\":" + std::to_string(data->icon) + ",";
                    response += "\"iconBorder\":" + std::to_string(data->iconBorder);
                    response += "}";
                }
                else
                {
                    response = R"({"type":"player_info","result":false,"msg":"player not found"})";
                }
            }
        }
        // --- 외형 정보 요청 ---
        else if (json.find("\"cmd\":\"get_appearance_info\"") != std::string::npos)
        {
            std::string nickname = findStringField(json, "nickname");
            std::string id = accountMgr->GetIdByNickname(nickname);
            AppearanceData* app = appearanceMgr->GetAppearance(id);

            if (app)
            {
                response = "{";
                response += "\"type\":\"appearance_info\",";
                response += "\"nickname\":\"" + nickname + "\",";
                response += "\"FullBody\":" + std::to_string(app->FullBody) + ",";
                response += "\"Hat\":" + std::to_string(app->Hat) + ",";
                response += "\"Accessories\":" + std::to_string(app->Accessories) + ",";
                response += "\"Faces\":" + std::to_string(app->Faces) + ",";
                response += "\"Glasses\":" + std::to_string(app->Glasses) + ",";
                response += "\"Gloves\":" + std::to_string(app->Gloves) + ",";
                response += "\"Hairstyle\":" + std::to_string(app->Hairstyle) + ",";
                response += "\"Outerwear\":" + std::to_string(app->Outerwear) + ",";
                response += "\"Pants\":" + std::to_string(app->Pants) + ",";
                response += "\"Shoes\":" + std::to_string(app->Shoes);
                response += "}";
            }
            else
                response = R"({"type":"appearance_info","result":false,"msg":"not found"})";
        }
        else if (json.find("\"cmd\":\"get_enemy_info\"") != std::string::npos)
        {
            std::string nickname = findStringField(json, "nickname");
            std::string id = accountMgr->GetIdByNickname(nickname);
            AppearanceData* app = appearanceMgr->GetAppearance(id);

            if (app)
            {
                response = "{";
                response += "\"type\":\"enemy_info\",";
                response += "\"nickname\":\"" + nickname + "\",";
                response += "\"FullBody\":" + std::to_string(app->FullBody) + ",";
                response += "\"Hat\":" + std::to_string(app->Hat) + ",";
                response += "\"Accessories\":" + std::to_string(app->Accessories) + ",";
                response += "\"Faces\":" + std::to_string(app->Faces) + ",";
                response += "\"Glasses\":" + std::to_string(app->Glasses) + ",";
                response += "\"Gloves\":" + std::to_string(app->Gloves) + ",";
                response += "\"Hairstyle\":" + std::to_string(app->Hairstyle) + ",";
                response += "\"Outerwear\":" + std::to_string(app->Outerwear) + ",";
                response += "\"Pants\":" + std::to_string(app->Pants) + ",";
                response += "\"Shoes\":" + std::to_string(app->Shoes);
                response += "}";
            }
            else
                response = R"({"type":"enemy_info","result":false,"msg":"enemy not found"})";
                }
        // --- 매칭 요청 ---
        else if (json.find("\"cmd\":\"request_match\"") != std::string::npos)
        {
            std::string nickname = findStringField(json, "nickname");
            if (nickname.empty())
            {
                response = R"({"type":"error","msg":"missing nickname"})";
            }
            else
            {
                PlayerData* data = playerDataMgr->GetByNickname(nickname);
                if (!data)
                {
                    response = R"({"type":"error","msg":"player not found"})";
                }
                else
                {
                    MatchPlayer player;
                    player.id = accountMgr->GetIdByNickname(nickname);
                    player.nickname = nickname;
                    player.rank = data->rank;
                    player.rankScore = data->rankScore;
                    player.level = data->level;
                    player.socket = socket_;

                    matchMgr->AddPlayer(player);
                    response = R"({"type":"match_wait","msg":"waiting for opponent"})";
                }
            }
        }
        else if (json.find("\"cmd\":\"match_ready\"") != std::string::npos)
        {
            std::string nickname = findStringField(json, "nickname");
            if (nickname.empty())
            {
                response = R"({"type":"error","msg":"missing nickname"})";
            }
            else
            {
                Room* room = roomMgr->FindRoomByPlayer(nickname);
                if (!room)
                {
                    response = R"({"type":"error","msg":"room not found"})";
                }
                else
                {
                    // 준비 상태 표시
                    room->SetPlayerReady(nickname, true);
                    std::cout << "[Room #" << room->roomId << "] " << nickname << " is ready ("
                        << room->ReadyCount() << "/2)\n";

                    // 둘 다 준비 완료 시 → match_start 전송
                    if (room->IsBothReady())
                    {
                        std::cout << "[Room #" << room->roomId << "] Both ready → match_start sent!\n";

                        std::string startMsg = R"({"type":"match_start","msg":"both ready"})";

                        uint32_t len1 = (uint32_t)startMsg.size();
                        send(room->player1.socket, (char*)&len1, 4, 0);
                        send(room->player1.socket, startMsg.c_str(), len1, 0);

                        uint32_t len2 = (uint32_t)startMsg.size();
                        send(room->player2.socket, (char*)&len2, 4, 0);
                        send(room->player2.socket, startMsg.c_str(), len2, 0);

                        continue; // 여기서 끝내야 아래 ack 전송 안 함
                    }

                    // 아직 한 명만 준비된 경우
                    response = R"({"type":"match_ready_ack","msg":"waiting for other player"})";
                }
            }
        }

        else if (json.find("\"cmd\":\"map_choose_start_request\"") != std::string::npos)
        {
            std::string nickname = findStringField(json, "nickname");
            Room* room = roomMgr->FindRoomByPlayer(nickname);

            if (!room)
            {
                response = R"({"type":"error","msg":"room not found"})";
            }
            else
            {
                // --- 맵 선택 초기화 ---
                room->mapChoice1.mapIndex = -2;
                room->mapChoice2.mapIndex = -2;
                room->selectedMap = -1;

                // --- 사용된 맵 로그 출력 ---
                std::cout << "[Room #" << room->roomId << "] Starting new round. Used maps: ";
                for (int m : room->usedMaps)
                    std::cout << m << " ";
                std::cout << std::endl;

                // --- JSON 구성 함수 (usedMaps 포함) ---
                auto buildMsg = [&](const RoomPlayer& self, const RoomPlayer& enemy)
                    {
                        std::string msg = "{";
                        msg += "\"type\":\"map_choose_start\",";
                        msg += "\"playerNickname\":\"" + self.nickname + "\",";
                        msg += "\"enemyNickname\":\"" + enemy.nickname + "\",";
                        msg += "\"playerScore\":" + std::to_string(self.score) + ",";
                        msg += "\"enemyScore\":" + std::to_string(enemy.score) + ",";
                        msg += "\"usedMaps\":[";

                        for (size_t i = 0; i < room->usedMaps.size(); ++i)
                        {
                            msg += std::to_string(room->usedMaps[i]);
                            if (i < room->usedMaps.size() - 1) msg += ",";
                        }

                        msg += "]}";
                        return msg;
                    };

                // --- 각 플레이어별 메시지 생성 ---
                std::string msgP1 = buildMsg(room->player1, room->player2);
                std::string msgP2 = buildMsg(room->player2, room->player1);

                // --- 전송 ---
                uint32_t len1 = (uint32_t)msgP1.size();
                send(room->player1.socket, (char*)&len1, 4, 0);
                send(room->player1.socket, msgP1.c_str(), len1, 0);

                uint32_t len2 = (uint32_t)msgP2.size();
                send(room->player2.socket, (char*)&len2, 4, 0);
                send(room->player2.socket, msgP2.c_str(), len2, 0);

                std::cout << "[Room #" << room->roomId << "] Sent map_choose_start to both players (map index reset, used maps sent).\n";
            }
            }

        else if (json.find("\"cmd\":\"select_map\"") != std::string::npos)
        {
            std::string nickname = findStringField(json, "nickname");
            int mapIndex = -999;
            if (!findIntField(json, "mapIndex", mapIndex))
            {
                response = R"({"type":"error","msg":"invalid mapIndex"})";
            }
            else
            {
                Room* room = roomMgr->FindRoomByPlayer(nickname);
                if (!room)
                {
                    response = R"({"type":"error","msg":"room not found"})";
                }
                else
                {
                    if (room->player1.nickname == nickname)
                        room->mapChoice1.mapIndex = mapIndex;
                    else if (room->player2.nickname == nickname)
                        room->mapChoice2.mapIndex = mapIndex;

                    if (room->mapChoice1.mapIndex != -2 && room->mapChoice2.mapIndex != -2)
                    {
                        if (std::find(room->usedMaps.begin(), room->usedMaps.end(), mapIndex) != room->usedMaps.end() && mapIndex != -1)
                        {
                            response = R"({"type":"error","msg":"map already used"})";
                            std::cout << "[Room #" << room->roomId << "] " << nickname
                                << " tried to select used map " << mapIndex << std::endl;
                            continue;
                        }

                        int chosenMap = -1;

                        int choice1 = room->mapChoice1.mapIndex;
                        int choice2 = room->mapChoice2.mapIndex;

                        if (choice1 == -1) choice1 = rand() % 3;
                        if (choice2 == -1) choice2 = rand() % 3;

                        if (choice1 == choice2)
                            chosenMap = choice1;
                        else
                            chosenMap = (rand() % 2 == 0) ? choice1 : choice2;

                        room->selectedMap = chosenMap;
                        room->usedMaps.push_back(chosenMap);

                        std::string res = "{ \"type\":\"map_result\", \"selectedMap\":" + std::to_string(chosenMap) + " }";

                        uint32_t len = (uint32_t)res.size();
                        send(room->player1.socket, (char*)&len, 4, 0);
                        send(room->player1.socket, res.c_str(), len, 0);
                        send(room->player2.socket, (char*)&len, 4, 0);
                        send(room->player2.socket, res.c_str(), len, 0);

                        std::cout << "[Room #" << room->roomId << "] Final Map: " << chosenMap << std::endl;
                    }
                    else
                    {
                        response = R"({"type":"map_wait","msg":"waiting for opponent selection"})";
                    }
                }
            }
            }

        else if (json.find("\"cmd\":\"get_spawn_info\"") != std::string::npos)
        {
            std::string nickname = findStringField(json, "nickname");
            Room* room = roomMgr->FindRoomByPlayer(nickname);

            if (!room)
            {
                response = R"({"type":"error","msg":"room not found"})";
            }
            else
            {
                // 이미 스폰이 결정되지 않았다면 한 번만 결정
                if (!room->spawnAssigned)
                {
                    // 방 생성 시 이미 랜덤하게 A/B 배정됨
                    room->spawnAssigned = true;
                    std::cout << "[Room #" << room->roomId << "] Spawn assigned ("
                        << room->player1.nickname << " = A, "
                        << room->player2.nickname << " = B)" << std::endl;
                }

                std::string playerPos, enemyNick;

                if (room->player1.nickname == nickname)
                {
                    playerPos = room->player1.spawnPos;
                    enemyNick = room->player2.nickname;
                }
                else
                {
                    playerPos = room->player2.spawnPos;
                    enemyNick = room->player1.nickname;
                }

                response = "{";
                response += "\"type\":\"spawn_info\",";
                response += "\"playerNickname\":\"" + nickname + "\",";
                response += "\"enemyNickname\":\"" + enemyNick + "\",";
                response += "\"playerPosition\":\"" + playerPos + "\"";
                response += "}";

                std::cout << "[Spawn] " << nickname << " → position " << playerPos << std::endl;
            }
            }
        else if (json.find("\"cmd\":\"player_move\"") != std::string::npos)
        {
            std::string nickname = findStringField(json, "nickname");
            Room* room = roomMgr->FindRoomByPlayer(nickname);

            if (!room)
            {
                response = R"({"type":"error","msg":"room not found"})";
            }
            else
            {
                float px, py, pz, rx, ry, rz;

                if (!findFloatField(json, "px", px) ||
                    !findFloatField(json, "py", py) ||
                    !findFloatField(json, "pz", pz) ||
                    !findFloatField(json, "rx", rx) ||
                    !findFloatField(json, "ry", ry) ||
                    !findFloatField(json, "rz", rz))
                {
                    response = R"({"type":"error","msg":"invalid float field"})";
                    std::cout << "[Error] Invalid float in player_move from " << nickname << std::endl;
                }
                else
                {
                    SOCKET targetSocket = INVALID_SOCKET;
                    if (room->player1.nickname == nickname)
                        targetSocket = room->player2.socket;
                    else if (room->player2.nickname == nickname)
                        targetSocket = room->player1.socket;

                    if (targetSocket != INVALID_SOCKET)
                    {
                        std::string moveMsg = "{";
                        moveMsg += "\"type\":\"enemy_move\",";
                        moveMsg += "\"nickname\":\"" + nickname + "\",";
                        moveMsg += "\"px\":" + std::to_string(px) + ",";
                        moveMsg += "\"py\":" + std::to_string(py) + ",";
                        moveMsg += "\"pz\":" + std::to_string(pz) + ",";
                        moveMsg += "\"rx\":" + std::to_string(rx) + ",";
                        moveMsg += "\"ry\":" + std::to_string(ry) + ",";
                        moveMsg += "\"rz\":" + std::to_string(rz);
                        moveMsg += "}";

                        uint32_t len = (uint32_t)moveMsg.size();
                        send(targetSocket, (char*)&len, 4, 0);
                        send(targetSocket, moveMsg.c_str(), len, 0);
                    }
                }
            }
            }
        else if (json.find("\"cmd\":\"player_win\"") != std::string::npos)
        {
            std::string nickname = findStringField(json, "nickname");
            Room* room = roomMgr->FindRoomByPlayer(nickname);

            if (!room)
            {
                response = R"({"type":"error","msg":"room not found"})";
            }
            else
            {
                // === 점수 갱신 ===
                if (room->player1.nickname == nickname)
                    room->player1.score++;
                else if (room->player2.nickname == nickname)
                    room->player2.score++;

                std::cout << "[Room #" << room->roomId << "] Score updated: "
                    << room->player1.nickname << " "
                    << room->player1.score << " : "
                    << room->player2.score << " "
                    << room->player2.nickname << std::endl;

                std::string scoreMsg = "{";
                scoreMsg += "\"type\":\"score_update\",";
                scoreMsg += "\"player1\":\"" + room->player1.nickname + "\",";
                scoreMsg += "\"player2\":\"" + room->player2.nickname + "\",";
                scoreMsg += "\"score1\":" + std::to_string(room->player1.score) + ",";
                scoreMsg += "\"score2\":" + std::to_string(room->player2.score);
                scoreMsg += "}";

                uint32_t len = (uint32_t)scoreMsg.size();
                send(room->player1.socket, (char*)&len, 4, 0);
                send(room->player1.socket, scoreMsg.c_str(), len, 0);
                send(room->player2.socket, (char*)&len, 4, 0);
                send(room->player2.socket, scoreMsg.c_str(), len, 0);

                if (room->player1.score >= 2 || room->player2.score >= 2)
                {
                    std::string winner = (room->player1.score > room->player2.score)
                        ? room->player1.nickname
                        : room->player2.nickname;

                    std::string endMsg = "{";
                    endMsg += "\"type\":\"game_end\",";
                    endMsg += "\"winner\":\"" + winner + "\"";
                    endMsg += "}";

                    uint32_t lenEnd = (uint32_t)endMsg.size();
                    send(room->player1.socket, (char*)&lenEnd, 4, 0);
                    send(room->player1.socket, endMsg.c_str(), lenEnd, 0);
                    send(room->player2.socket, (char*)&lenEnd, 4, 0);
                    send(room->player2.socket, endMsg.c_str(), lenEnd, 0);

                    std::cout << "[Room #" << room->roomId << "] Game ended! Winner: " << winner << std::endl;

                    room->usedMaps.clear();
                }
                else
                {
                    std::string nextRoundMsg = R"({"type":"round_end","msg":"next round"})";
                    uint32_t len2 = (uint32_t)nextRoundMsg.size();
                    send(room->player1.socket, (char*)&len2, 4, 0);
                    send(room->player1.socket, nextRoundMsg.c_str(), len2, 0);
                    send(room->player2.socket, (char*)&len2, 4, 0);
                    send(room->player2.socket, nextRoundMsg.c_str(), len2, 0);
                }
            }
            }

            // === 결과 요청 ===
        else if (json.find("\"cmd\":\"get_game_result\"") != std::string::npos)
        {
            std::string nickname = findStringField(json, "nickname");
            Room* room = roomMgr->FindRoomByPlayer(nickname);

            if (!room)
            {
                response = R"({"type":"error","msg":"room not found"})";
            }
            else
            {
                std::string enemy = (room->player1.nickname == nickname)
                    ? room->player2.nickname
                    : room->player1.nickname;

                std::string winner = (room->player1.score > room->player2.score)
                    ? room->player1.nickname
                    : room->player2.nickname;

                response = "{";
                response += "\"type\":\"game_result\",";
                response += "\"winner\":\"" + winner + "\",";
                response += "\"enemy\":\"" + enemy + "\"";
                response += "}";
            }
            }
        else if (json.find("\"cmd\":\"leave_room\"") != std::string::npos)
        {
            std::string nickname = findStringField(json, "nickname");
            Room* room = roomMgr->FindRoomByPlayer(nickname);

            if (!room)
            {
                response = R"({"type":"error","msg":"room not found"})";
            }
            else
            {
                std::cout << "[Room #" << room->roomId << "] " << nickname << " requested to leave.\n";

                // --- 유저 제거 ---
                if (room->player1.nickname == nickname)
                {
                    room->player1.nickname.clear();
                    room->player1.socket = INVALID_SOCKET;
                }
                else if (room->player2.nickname == nickname)
                {
                    room->player2.nickname.clear();
                    room->player2.socket = INVALID_SOCKET;
                }

                bool p1Exists = !room->player1.nickname.empty();
                bool p2Exists = !room->player2.nickname.empty();

                if (!p1Exists && !p2Exists)
                {
                    std::cout << "[Room #" << room->roomId << "] is now empty → deleting.\n";
                    roomMgr->RemoveRoom(room->roomId);
                }
                else
                {
                    std::cout << "[Room #" << room->roomId << "] one player remains.\n";
                }

                response = R"({"type":"leave_room_ack","msg":"player left room"})";
            }
}


        // --- 알 수 없는 명령 ---
        else
        {
            response = R"({"type":"error","msg":"unknown command"})";
        }

        if (!response.empty())
        {
            uint32_t sendLen = (uint32_t)response.size();
            sendFull(&sendLen, 4);
            sendFull(response.data(), sendLen);
        }

        if (!(json.find("\"cmd\":\"player_move\"") != std::string::npos ||
            response.find("\"type\":\"enemy_move\"") != std::string::npos))
        {
            std::cout << "[Send] To " << addrStr << ":" << port << ": " << response << std::endl;
        }
    }

    std::cout << "[-] Client disconnected: " << addrStr << ":" << port << std::endl;
    closesocket(socket_);
}
