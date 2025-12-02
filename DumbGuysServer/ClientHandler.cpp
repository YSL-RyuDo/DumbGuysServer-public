#include "ClientHandler.h"
#include <iostream>
#include <vector>
#include <thread>
#include <cctype>
#include <climits>

// 파일 상단에 추가 (예: ClientHandler.cpp 맨 위)
template <typename T>
constexpr const T& clamp(const T& v, const T& lo, const T& hi)
{
    return (v < lo) ? lo : (hi < v) ? hi : v;
}


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

                // 로그인 성공 → 로비 추가
                roomMgr->AddLobbyPlayer(nickname, socket_);

                response = "{";
                response += "\"type\":\"login_result\",";
                response += "\"result\":true,";
                response += "\"msg\":\"login ok\",";
                response += "\"nickname\":\"" + nickname + "\"";
                response += "}";
            }
            else
            {
                response = R"({"type":"login_result","result":false,"msg":"Account already logged in or invalid credentials"})";
            }
        }
		// --- 로그아웃 ---
        else if (json.find("\"cmd\":\"logout\"") != std::string::npos)
        {
            std::string nickname = findStringField(json, "nickname");

            accountMgr->SetLoggedIn(nickname, false);
            roomMgr->RemoveLobbyPlayer(nickname);

            std::cout << "[Logout] " << nickname << " logged out.\n";
            response = R"({"type":"logout_ack","msg":"logout success"})";
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
        // --- 튜토리얼 정보 요청 ---
        else if (json.find("\"cmd\":\"get_tutorial_info\"") != std::string::npos)
        {
            std::string nickname = findStringField(json, "nickname");
            if (nickname.empty())
            {
                response = R"({"type":"tutorial_info","result":false,"msg":"missing nickname"})";
            }
            else
            {
                PlayerData* data = playerDataMgr->GetByNickname(nickname);
                if (data)
                {
                    response = "{";
                    response += "\"type\":\"tutorial_info\",";
                    response += "\"result\":true,";
                    response += "\"nickname\":\"" + data->nickname + "\",";
                    response += "\"tutorialDone\":" + std::string(data->tutorialDone ? "true" : "false") + ",";
                    response += "\"tutorialStage\":" + std::to_string(data->tutorialStage);
                    response += "}";
                }
                else
                {
                    response = R"({"type":"tutorial_info","result":false,"msg":"player not found"})";
                }
            }
        }
        // --- 튜토리얼 정보 업데이트 ---
        // 튜토리얼 스테이지만 저장 (세부단계는 없음)
        else if (json.find("\"cmd\":\"set_tutorial_stage\"") != std::string::npos)
        {
            std::string nickname = findStringField(json, "nickname");
            int stage = 0;
            findIntField(json, "tutorialStage", stage);

            PlayerData* data = playerDataMgr->GetByNickname(nickname);
            if (data)
            {
                data->tutorialStage = stage;
                playerDataMgr->Save();

                std::cout << "[Tutorial] " << nickname << " stage updated to " << stage << std::endl;
                response = R"({"type":"tutorial_update_result","result":true,"msg":"stage updated"})";
            }
            else
            {
                response = R"({"type":"tutorial_update_result","result":false,"msg":"player not found"})";
            }
            }

        else if (json.find("\"cmd\":\"set_tutorial_done\"") != std::string::npos)
        {
            std::string nickname = findStringField(json, "nickname");
            int stage = 0;
            findIntField(json, "tutorialStage", stage);

            PlayerData* data = playerDataMgr->GetByNickname(nickname);
            if (data)
            {
                // 완료 상태 처리
                data->tutorialDone = true;
                data->tutorialStage = stage;

                playerDataMgr->Save();

                response = "{";
                response += "\"type\":\"tutorial_update_result\",";
                response += "\"result\":true,";
                response += "\"msg\":\"tutorial updated\",";
                response += "\"tutorialStage\":" + std::to_string(data->tutorialStage);
                response += "}";
                std::cout << "[Tutorial] Updated tutorial info for " << nickname
                    << " (Stage=" << data->tutorialStage << ")" << std::endl;
            }
            else
            {
                response = R"({"type":"tutorial_update_result","result":false,"msg":"player not found"})";
            }
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
                    roomMgr->RemoveLobbyPlayer(nickname);
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
        else if (json.find("\"cmd\":\"match_cancel\"") != std::string::npos)
        {
            std::string nickname = findStringField(json, "nickname");

            if (nickname.empty())
            {
                std::string response = R"({"type":"error","msg":"missing nickname"})";
                uint32_t len = response.size();
                sendFull(&len, 4);
                sendFull(response.data(), len);
                continue;
            }

            std::cout << "[Match] Cancel request from " << nickname << "\n";

            // 1) 매칭 큐에서 제거
            matchMgr->RemovePlayer(nickname);

            // 2) 로비 플레이어로 다시 등록
            roomMgr->AddLobbyPlayer(nickname, socket_);

            // 3) 응답 보내기
            std::string response = R"({"type":"match_cancel_ack","msg":"match canceled"})";
            uint32_t len = response.size();
            sendFull(&len, 4);
            sendFull(response.data(), len);

            continue;
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
        /*else if (json.find("\"cmd\":\"get_game_result\"") != std::string::npos)
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
            }*/
//        else if (json.find("\"cmd\":\"leave_room\"") != std::string::npos)
//        {
//            std::string nickname = findStringField(json, "nickname");
//            Room* room = roomMgr->FindRoomByPlayer(nickname);
//
//            if (!room)
//            {
//                response = R"({"type":"error","msg":"room not found"})";
//            }
//            else
//            {
//                std::cout << "[Room #" << room->roomId << "] " << nickname << " requested to leave.\n";
//
//                // --- 유저 제거 ---
//                if (room->player1.nickname == nickname)
//                {
//                    room->player1.nickname.clear();
//                    room->player1.socket = INVALID_SOCKET;
//                }
//                else if (room->player2.nickname == nickname)
//                {
//                    room->player2.nickname.clear();
//                    room->player2.socket = INVALID_SOCKET;
//                }
//
//                bool p1Exists = !room->player1.nickname.empty();
//                bool p2Exists = !room->player2.nickname.empty();
//
//                if (!p1Exists && !p2Exists)
//                {
//                    std::cout << "[Room #" << room->roomId << "] is now empty → deleting.\n";
//                    roomMgr->RemoveRoom(room->roomId);
//                }
//                else
//                {
//                    std::cout << "[Room #" << room->roomId << "] one player remains.\n";
//                }
//
//                response = R"({"type":"leave_room_ack","msg":"player left room"})";
//            }
//}
        else if (json.find("\"cmd\":\"attack\"") != std::string::npos)
        {
            std::string attacker = findStringField(json, "attacker");
            std::string target = findStringField(json, "target");
            int damage = 0;
            findIntField(json, "damage", damage);

            Room* room = roomMgr->FindRoomByPlayer(attacker);
            if (!room)
            {
                response = R"({"type":"error","msg":"room not found"})";
            }
            else
            {
                std::cout << "[Room #" << room->roomId << "] Attack from "
                    << attacker << " → " << target
                    << " (" << damage << " dmg)" << std::endl;

                SOCKET targetSocket = INVALID_SOCKET;
                SOCKET attackerSocket = INVALID_SOCKET;

                if (room->player1.nickname == attacker)
                {
                    attackerSocket = room->player1.socket;
                    targetSocket = room->player2.socket;
                }
                else if (room->player2.nickname == attacker)
                {
                    attackerSocket = room->player2.socket;
                    targetSocket = room->player1.socket;
                }

                if (targetSocket != INVALID_SOCKET && attackerSocket != INVALID_SOCKET)
                {
                    // === 공통 "player_hit" 메시지 생성 ===
                    std::string hitMsg = "{";
                    hitMsg += "\"type\":\"player_hit\",";
                    hitMsg += "\"attacker\":\"" + attacker + "\",";
                    hitMsg += "\"target\":\"" + target + "\",";
                    hitMsg += "\"damage\":" + std::to_string(damage);
                    hitMsg += "}";

                    uint32_t len = (uint32_t)hitMsg.size();

                    // 공격자에게도 보냄
                    send(attackerSocket, (char*)&len, 4, 0);
                    send(attackerSocket, hitMsg.c_str(), len, 0);

                    // 피격자에게도 보냄
                    send(targetSocket, (char*)&len, 4, 0);
                    send(targetSocket, hitMsg.c_str(), len, 0);

                    std::cout << "[Room #" << room->roomId
                        << "] Sent 'player_hit' to both attacker and target.\n";

                    response = R"({"type":"attack_ack","msg":"attack broadcasted"})";
                }
                else
                {
                    response = R"({"type":"error","msg":"attacker or target socket invalid"})";
                }
            }
            }
        else if (json.find("\"cmd\":\"attack_anim\"") != std::string::npos)
        {
            std::string attacker = findStringField(json, "attacker");
            Room* room = roomMgr->FindRoomByPlayer(attacker);

            if (!room)
            {
                response = R"({"type":"error","msg":"room not found"})";
            }
            else
            {
                SOCKET targetSocket = INVALID_SOCKET;

                if (room->player1.nickname == attacker)
                    targetSocket = room->player2.socket;
                else if (room->player2.nickname == attacker)
                    targetSocket = room->player1.socket;

                if (targetSocket != INVALID_SOCKET)
                {
                    std::string animMsg = "{";
                    animMsg += "\"type\":\"enemy_attack_anim\",";
                    animMsg += "\"attacker\":\"" + attacker + "\"";
                    animMsg += "}";

                    uint32_t len = (uint32_t)animMsg.size();
                    send(targetSocket, (char*)&len, 4, 0);
                    send(targetSocket, animMsg.c_str(), len, 0);

                    std::cout << "[Room #" << room->roomId << "] Broadcast attack_anim from "
                        << attacker << " to opponent.\n";

                    response.clear(); // 클라이언트에 별도 응답 필요 없음
                }
            }
            }
        else if (json.find("\"cmd\":\"death_anim\"") != std::string::npos)
        {
            std::string attacker = findStringField(json, "attacker");
            Room* room = roomMgr->FindRoomByPlayer(attacker);

            if (!room)
            {
                response = R"({"type":"error","msg":"room not found"})";
            }
            else
            {
                std::cout << "[Room #" << room->roomId << "] " << attacker << " died → notifying opponent." << std::endl;

                // 상대 소켓 찾기
                SOCKET targetSocket = INVALID_SOCKET;
                if (room->player1.nickname == attacker)
                    targetSocket = room->player2.socket;
                else if (room->player2.nickname == attacker)
                    targetSocket = room->player1.socket;

                if (targetSocket != INVALID_SOCKET)
                {
                    // === 상대방에게 enemy_die_anim 전송 ===
                    std::string msg = "{";
                    msg += "\"type\":\"enemy_die_anim\",";
                    msg += "\"attacker\":\"" + attacker + "\"";
                    msg += "}";

                    uint32_t len = (uint32_t)msg.size();
                    send(targetSocket, (char*)&len, 4, 0);
                    send(targetSocket, msg.c_str(), len, 0);

                    std::cout << "[Room #" << room->roomId
                        << "] Sent enemy_die_anim → opponent notified." << std::endl;

                    // 공격자(죽은 쪽)에게도 확인 메시지 보냄 (선택사항)
                    response = R"({"type":"death_ack","msg":"death broadcasted"})";
                }
                else
                {
                    response = R"({"type":"error","msg":"opponent socket invalid"})";
                }
            }
            }
        else if (json.find("\"cmd\":\"push\"") != std::string::npos)
        {
            std::string attacker = findStringField(json, "attacker");
            std::string target = findStringField(json, "target");

            Room* room = roomMgr->FindRoomByPlayer(attacker);
            if (!room)
            {
                response = R"({"type":"error","msg":"room not found"})";
            }
            else
            {
                std::cout << "[Room #" << room->roomId << "] Push from "
                    << attacker << " → " << target << std::endl;

                SOCKET targetSocket = INVALID_SOCKET;

                if (room->player1.nickname == target)
                    targetSocket = room->player1.socket;
                else if (room->player2.nickname == target)
                    targetSocket = room->player2.socket;

                if (targetSocket != INVALID_SOCKET)
                {
                    // === push_hit 패킷 생성 ===
                    std::string pushMsg = "{";
                    pushMsg += "\"type\":\"push_hit\",";
                    pushMsg += "\"attacker\":\"" + attacker + "\",";
                    pushMsg += "\"target\":\"" + target + "\"";
                    pushMsg += "}";

                    uint32_t len = (uint32_t)pushMsg.size();
                    send(targetSocket, (char*)&len, 4, 0);
                    send(targetSocket, pushMsg.c_str(), len, 0);

                    response = R"({"type":"push_ack","msg":"push sent"})";
                    std::cout << "[Room #" << room->roomId
                        << "] Sent push_hit to " << target << std::endl;
                }
                else
                {
                    response = R"({"type":"error","msg":"target not found in room"})";
                }
            }
            }
        else if (json.find("\"cmd\":\"chat\"") != std::string::npos)
        {
            std::string sender = findStringField(json, "sender");
            std::string message = findStringField(json, "message");

            Room* room = roomMgr->FindRoomByPlayer(sender);

            std::string chatMsg = "{";
            chatMsg += "\"type\":\"chat_message\",";
            chatMsg += "\"sender\":\"" + sender + "\",";
            chatMsg += "\"message\":\"" + message + "\"";
            chatMsg += "}";

            uint32_t len = (uint32_t)chatMsg.size();

            if (room)
            {
                // ===== 인게임 플레이어 간 채팅 =====
                std::cout << "[Room #" << room->roomId << "] Chat from " << sender << ": " << message << std::endl;

                auto sendToClient = [&](const std::string& targetName, SOCKET sock)
                    {
                        if (targetName == sender)
                            return;

                        if (sock != INVALID_SOCKET)
                        {
                            send(sock, (char*)&len, 4, 0);
                            send(sock, chatMsg.c_str(), len, 0);
                        }
                    };

                sendToClient(room->player1.nickname, room->player1.socket);
                sendToClient(room->player2.nickname, room->player2.socket);
            }
            else
            {
                std::cout << "[Lobby] Chat from " << sender << ": " << message << std::endl;
                roomMgr->BroadcastLobby(chatMsg, sender);
            }

            response = R"({"type":"chat_ack","msg":"chat broadcasted"})";
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
                std::string winner = (room->player1.score > room->player2.score)
                    ? room->player1.nickname
                    : room->player2.nickname;
                std::string loser = (room->player1.nickname == winner)
                    ? room->player2.nickname
                    : room->player1.nickname;

                std::string enemy = (room->player1.nickname == nickname)
                    ? room->player2.nickname
                    : room->player1.nickname;

                PlayerData* winData = playerDataMgr->GetByNickname(winner);
                PlayerData* loseData = playerDataMgr->GetByNickname(loser);

                if (winData && loseData)
                {
                    // === 공통 보상 ===
                    winData->coin0 += 10;
                    loseData->coin0 += 10;

                    // === 승자 추가 보상 ===
                    winData->coin1 += 30;
                    winData->exp += 50;
                    loseData->exp += 20;

                    // === 레벨업 ===
                    auto processLevelUp = [&](PlayerData* p)
                        {
                            int expToLevelUp = p->level * 100;
                            while (p->exp >= expToLevelUp)
                            {
                                p->exp -= expToLevelUp;
                                p->level++;
                                expToLevelUp = p->level * 100;
                                std::cout << "[LevelUp] " << p->nickname << " reached Lv." << p->level << std::endl;
                            }
                        };
                    processLevelUp(winData);
                    processLevelUp(loseData);

                    // === 랭크 스코어 계산 (랭크/점수 기반 가중치) ===
                    int rankDiff = loseData->rank - winData->rank; // 승자 기준
                    int scoreDiff = loseData->rankScore - winData->rankScore;
                    float diffFactor = (rankDiff * 10 + scoreDiff / 10.0f) / 100.0f; // 대략적인 실력차

                    // 보정: 실력차가 클수록 영향 작게, 반대로 이변이면 크게
                    int baseGain = 30;
                    int baseLoss = 20;
                    int dynamicGain = (int)(baseGain * (1.0f + diffFactor * -1.2f)); // 강한 상대일수록 보상↑
                    int dynamicLoss = (int)(baseLoss * (1.0f + diffFactor * 1.5f));  // 약한 상대에게 지면 페널티↑

                    // 최소/최대 보정
                    dynamicGain = clamp(dynamicGain, 10, 80);
                    dynamicLoss = clamp(dynamicLoss, 5, 80);

                    winData->rankScore += dynamicGain;
                    loseData->rankScore -= dynamicLoss;

                    // 0점 이하 방지
                    if (loseData->rankScore < 0)
                        loseData->rankScore = 0;

                    // === 랭크 상승/하락 처리 ===
                    auto processRank = [&](PlayerData* p)
                        {
                            const int pointsPerRank = 100;

                            // 승급
                            while (p->rankScore >= pointsPerRank && p->rank < 8)
                            {
                                p->rankScore -= pointsPerRank;
                                p->rank++;
                                std::cout << "[RankUp] " << p->nickname
                                    << " promoted to Rank " << p->rank << std::endl;
                            }

                            // 최고 랭크는 점수 누적만
                            if (p->rank >= 8 && p->rankScore > pointsPerRank * 10)
                                p->rankScore = pointsPerRank * 10;

                            // 강등
                            while (p->rankScore < 0 && p->rank > 0)
                            {
                                p->rank--;
                                p->rankScore += pointsPerRank;
                                std::cout << "[RankDown] " << p->nickname
                                    << " demoted to Rank " << p->rank << std::endl;
                            }

                            // 경계값
                            if (p->rank < 0) p->rank = 0;
                            if (p->rank > 8) p->rank = 8;
                        };

                    processRank(winData);
                    processRank(loseData);

                    playerDataMgr->Save();

                    std::cout << "[GameResult] Winner: " << winner
                        << ", Loser: " << loser
                        << " | Gain: +" << dynamicGain
                        << ", Loss: -" << dynamicLoss
                        << std::endl;
                }

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
                roomMgr->AddLobbyPlayer(nickname, socket_);
                response = R"({"type":"leave_room_ack","msg":"player left room"})";
            }
        }
        

        // --- 코인 정보 요청 ---
        else if (json.find("\"cmd\":\"get_coin_info\"") != std::string::npos)
        {
            std::string nickname = findStringField(json, "nickname");

            if (nickname.empty())
            {
                response = R"({"type":"coin_info","result":false,"msg":"missing nickname"})";
            }
            else
            {
                PlayerData* data = playerDataMgr->GetByNickname(nickname);

                if (data)
                {
                    response = "{";
                    response += "\"type\":\"coin_info\",";
                    response += "\"nickname\":\"" + data->nickname + "\",";
                    response += "\"coin0\":" + std::to_string(data->coin0) + ",";
                    response += "\"coin1\":" + std::to_string(data->coin1);
                    response += "}";
                }
                else
                {
                    response = R"({"type":"coin_info","result":false,"msg":"player not found"})";
                }
            }
            }
        else if (json.find("\"cmd\":\"get_store_items\"") != std::string::npos)
        {
            std::string category = findStringField(json, "category");
            std::cout << "[Store] Received get_store_items for category: " << category << std::endl;

            // CSV 읽기
            std::vector<StoreItem> items = storeMgr->LoadStoreCategory(category);

            // JSON 변환
            std::string response = "{";
            response += "\"type\":\"store_items\",";
            response += "\"category\":\"" + category + "\",";
            response += "\"items\":[";

            for (size_t i = 0; i < items.size(); i++)
            {
                response += "{";
                response += "\"id\":" + std::to_string(items[i].id) + ",";
                response += "\"name\":\"" + items[i].name + "\",";
                response += "\"price\":" + std::to_string(items[i].price);
                response += "}";

                if (i < items.size() - 1)
                    response += ",";
            }

            response += "]}";

            uint32_t len = response.size();
            sendFull(&len, 4);
            sendFull(response.data(), len);

            continue;
            }
        else if (json.find("\"cmd\":\"get_owned_items\"") != std::string::npos)
        {
            std::string nickname = findStringField(json, "nickname");
            std::string category = findStringField(json, "category");

            std::cout << "[Store] get_owned_items request: " << nickname
                << " / category: " << category << std::endl;

            // 닉네임 → ID 얻기
            std::string id = accountMgr->GetIdByNickname(nickname);
            if (id.empty())
            {
                std::string response = R"({"type":"owned_items","result":false,"msg":"player not found"})";
                uint32_t len = response.size();
                sendFull(&len, 4);
                sendFull(response.data(), len);
                continue;
            }

            // CSV 읽기 (OwnedItemManager 기능)
            std::vector<int> owned = ownedItemMgr->GetOwnedList(id, category);

            if (owned.empty())
            {
                std::string response = R"({"type":"owned_items","result":false,"msg":"read failed"})";
                uint32_t len = response.size();
                sendFull(&len, 4);
                sendFull(response.data(), len);
                continue;
            }

            // JSON 생성
            std::string response = "{";
            response += "\"type\":\"owned_items\",";
            response += "\"category\":\"" + category + "\",";
            response += "\"owned\":[";

            for (size_t i = 0; i < owned.size(); i++)
            {
                response += std::to_string(owned[i]);
                if (i < owned.size() - 1)
                    response += ",";
            }

            response += "]}";

            uint32_t len = response.size();
            sendFull(&len, 4);
            sendFull(response.data(), len);

            std::cout << "[Store] Sent owned_items for " << nickname
                << " / " << category << std::endl;

            continue;
            }
        else if (json.find("\"cmd\":\"buy_item\"") != std::string::npos)
        {
            std::string nickname = findStringField(json, "nickname");
            std::string category = findStringField(json, "category");
            int id = -1;
            findIntField(json, "id", id);

            std::string accId = accountMgr->GetIdByNickname(nickname);
            if (accId.empty())
            {
                std::string response = R"({"type":"buy_item_result","result":false,"msg":"player not found"})";
                uint32_t len = response.size();
                sendFull(&len, 4);
                sendFull(response.data(), len);
                continue;
            }

            // CSV에서 해당 아이템의 가격 얻기
            StoreItem item = storeMgr->GetItem(category, id);

            // 플레이어 정보
            PlayerData* data = playerDataMgr->GetByNickname(nickname);
            if (!data)
            {
                std::string response = R"({"type":"buy_item_result","result":false,"msg":"player not found"})";
                uint32_t len = response.size();
                sendFull(&len, 4);
                sendFull(response.data(), len);
                continue;
            }

            // 코인 부족 체크
            if (data->coin1 < item.price)
            {
                std::string response = R"({"type":"buy_item_result","result":false,"msg":"not enough coin"})";
                uint32_t len = response.size();
                sendFull(&len, 4);
                sendFull(response.data(), len);
                continue;
            }

            // 구매 처리 : 코인 차감 + CSV 수정
            data->coin1 -= item.price;
            playerDataMgr->Save();

            ownedItemMgr->AddOwnedItem(accId, category, id); // CSV 저장

            // 응답 전송
            std::string response = "{";
            response += "\"type\":\"buy_item_result\",";
            response += "\"result\":true,";
            response += "\"category\":\"" + category + "\",";
            response += "\"id\":" + std::to_string(id) + ",";
            response += "\"coin1\":" + std::to_string(data->coin1);
            response += "}";

            uint32_t len = response.size();
            sendFull(&len, 4);
            sendFull(response.data(), len);

            std::cout << "[Store] Item purchased: " << nickname << " → "
                << category << id << std::endl;

            continue;
        }
        else if (json.find("\"cmd\":\"gacha\"") != std::string::npos)
        {
            std::string nickname = findStringField(json, "nickname");

            std::cout << "[Gacha] Request from " << nickname << "\n";

            // --- 1. PlayerData 가져오기 ---
            PlayerData* data = playerDataMgr->GetByNickname(nickname);
            if (!data)
            {
                std::string response = R"({"type":"gacha_result","result":false,"msg":"player not found"})";
                uint32_t len = response.size();
                sendFull(&len, 4);
                sendFull(response.data(), len);
                continue;
            }

            // --- 2. 가챠 비용 확인 ---
            const int GACHA_COST = 1000;

            if (data->coin0 < GACHA_COST)
            {
                std::string response = R"({"type":"gacha_result","result":false,"msg":"not enough coin0"})";
                uint32_t len = response.size();
                sendFull(&len, 4);
                sendFull(response.data(), len);
                std::cout << "[Gacha] Not enough coin0\n";
                continue;
            }

            // --- 3. coin0 차감 ---
            data->coin0 -= GACHA_COST;
            playerDataMgr->Save();

            // --- 4. 랜덤 카테고리 ---
            std::vector<std::string> categories = {
                "Accessories", "Faces", "FullBody", "Glasses",
                "Gloves", "Hairstyle", "Hat", "Outerwear", "Pants", "Shoes"
            };
            int randCat = rand() % categories.size();
            std::string category = categories[randCat];

            // --- 5. 카테고리 아이템 로드 ---
            std::vector<StoreItem> items = storeMgr->LoadStoreCategory(category);
            if (items.empty())
            {
                std::cout << "[Gacha] Category empty: " << category << "\n";
                continue;
            }

            // --- 6. 랜덤 아이템 ---
            int idx = rand() % items.size();
            StoreItem picked = items[idx];

            // --- 7. Owned 저장 ---
            // AccountId 필요 → PlayerData는 nickname 보유
            // AccountManager 에서 id 찾기
            std::string accId = accountMgr->GetIdByNickname(nickname);

            if (!accId.empty())
                ownedItemMgr->AddOwnedItem(accId, category, picked.id);

            // --- 8. 응답 ---
            std::string response = "{";
            response += "\"type\":\"gacha_result\",";
            response += "\"result\":true,";
            response += "\"category\":\"" + category + "\",";
            response += "\"id\":" + std::to_string(picked.id) + ",";
            response += "\"coin0\":" + std::to_string(data->coin0);
            response += "}";

            uint32_t len = response.size();
            sendFull(&len, 4);
            sendFull(response.data(), len);

            std::cout << "[Gacha] SUCCESS → " << category << picked.id << "\n";

            continue;
            }
        else if (json.find("\"cmd\":\"change_appearance\"") != std::string::npos)
        {
            std::string nickname = findStringField(json, "nickname");
            std::string category = findStringField(json, "category");
            int id = -1;
            findIntField(json, "id", id);

            std::string accId = accountMgr->GetIdByNickname(nickname);
            if (accId.empty())
            {
                std::string response = R"({"type":"appearance_update","result":false,"msg":"player not found"})";
                uint32_t len = response.size();
                sendFull(&len, 4);
                sendFull(response.data(), len);
                continue;
            }

            // 1) appearanceMap 내부 데이터 얻기
            AppearanceData* data = appearanceMgr->GetAppearance(accId);

            // 1-2) 없으면 새로 생성
            if (!data)
            {
                appearanceMgr->AddNewAppearance(accId);
                data = appearanceMgr->GetAppearance(accId);
            }

            // 2) 해당 카테고리만 수정
            if (category == "FullBody") data->FullBody = id;
            else if (category == "Hat") data->Hat = id;
            else if (category == "Accessories") data->Accessories = id;
            else if (category == "Faces") data->Faces = id;
            else if (category == "Glasses") data->Glasses = id;
            else if (category == "Gloves") data->Gloves = id;
            else if (category == "Hairstyle") data->Hairstyle = id;
            else if (category == "Outerwear") data->Outerwear = id;
            else if (category == "Pants") data->Pants = id;
            else if (category == "Shoes") data->Shoes = id;

            // 3) 전체 저장 (파일 rewrite)
            appearanceMgr->Save();

            // 4) 응답 생성
            std::string response = "{";
            response += "\"type\":\"appearance_update\",";
            response += "\"result\":true,";
            response += "\"FullBody\":" + std::to_string(data->FullBody) + ",";
            response += "\"Hat\":" + std::to_string(data->Hat) + ",";
            response += "\"Accessories\":" + std::to_string(data->Accessories) + ",";
            response += "\"Faces\":" + std::to_string(data->Faces) + ",";
            response += "\"Glasses\":" + std::to_string(data->Glasses) + ",";
            response += "\"Gloves\":" + std::to_string(data->Gloves) + ",";
            response += "\"Hairstyle\":" + std::to_string(data->Hairstyle) + ",";
            response += "\"Outerwear\":" + std::to_string(data->Outerwear) + ",";
            response += "\"Pants\":" + std::to_string(data->Pants) + ",";
            response += "\"Shoes\":" + std::to_string(data->Shoes);
            response += "}";

            uint32_t len = response.size();
            sendFull(&len, 4);
            sendFull(response.data(), len);

            continue;
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
