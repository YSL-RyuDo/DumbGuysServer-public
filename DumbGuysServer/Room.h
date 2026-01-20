#pragma once
#include <string>
#include <vector>
#include <WinSock2.h>
#include <chrono>

struct LastMoveState
{
    float px = 0.f;
    float py = 0.f;
    float pz = 0.f;

    float rx = 0.f;
    float ry = 0.f;
    float rz = 0.f;

    std::chrono::steady_clock::time_point lastSendTime;
    bool initialized = false;
};


struct RoomPlayer
{
    std::string id;
    std::string nickname;
    SOCKET socket;
    int level;
    int rank;
    int rankScore;
    bool isReady = false;
	int score = 0; // 현재 게임에서의 점수
    std::string spawnPos;
    LastMoveState lastMove;
};

struct MapSelection
{
    std::string nickname;
    int mapIndex = -2;
};

class Room
{
public:
    int roomId;
    RoomPlayer player1;
    RoomPlayer player2;
    int readyCount = 0;

    int selectedMap = -1;
    MapSelection mapChoice1;
    MapSelection mapChoice2;

    std::vector<int> usedMaps; // 이미 사용된 맵들

    bool spawnAssigned = false;

    Room(); // 기본 생성자
    Room(int id, const RoomPlayer& p1, const RoomPlayer& p2); // 선언

    void SetPlayerReady(const std::string& nickname, bool ready);
    int ReadyCount() const;
    bool IsBothReady() const;
    bool HasPlayer(const std::string& nickname) const;
};
