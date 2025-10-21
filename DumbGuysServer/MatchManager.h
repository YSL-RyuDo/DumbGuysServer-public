#pragma once
#include <string>
#include <vector>
#include <mutex>
#include <WinSock2.h>
#include "RoomManager.h"

struct MatchPlayer
{
    std::string id;
    std::string nickname;
    int level;
    int rank;
    int rankScore;
    SOCKET socket;
    double timeAdded;
};

class MatchManager
{
private:
    std::vector<MatchPlayer> waitingPlayers;
    std::mutex mtx;
    RoomManager* roomMgr;

public:
    MatchManager(RoomManager* roomMgr);
    void AddPlayer(const MatchPlayer& player);

private:
    void CheckMatch();
    bool IsMatchable(const MatchPlayer& a, const MatchPlayer& b);
};
