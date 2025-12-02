#include "MatchManager.h"
#include <iostream>
#include <cmath>
#include <chrono>
#include <Windows.h>
#undef max
#undef min
#include <algorithm>

MatchManager::MatchManager(RoomManager* mgr) : roomMgr(mgr) {}

void MatchManager::AddPlayer(const MatchPlayer& player)
{
    {
        std::lock_guard<std::mutex> lock(mtx);
        MatchPlayer copy = player;
        copy.timeAdded = GetTickCount64() / 1000.0; // 초 단위 시간
        waitingPlayers.push_back(copy);

        std::cout << "[Match] Added: " << copy.nickname
            << " (Rank:" << copy.rank << ", Score:" << copy.rankScore
            << ", Lv:" << copy.level << ")\n";
    }

    CheckMatch();
}

bool MatchManager::IsMatchable(const MatchPlayer& a, const MatchPlayer& b)
{
    double now = GetTickCount64() / 1000.0;
    double waitA = now - a.timeAdded;
    double waitB = now - b.timeAdded;

    double maxWait = std::max(waitA, waitB);

    int allowedRankDiff = 1;
    int allowedScoreDiff = 300;
    int allowedLevelDiff = 5;

    if (maxWait > 10) allowedLevelDiff += 5;
    if (maxWait > 20) allowedScoreDiff += 300;
    if (maxWait > 30) allowedRankDiff += 1;
    if (maxWait > 45) allowedLevelDiff += 10;

    int rankDiff = abs(a.rank - b.rank);
    int scoreDiff = abs(a.rankScore - b.rankScore);
    int levelDiff = abs(a.level - b.level);

    std::cout << "[Match] Checking: " << a.nickname << " vs " << b.nickname
        << " | RankDiff:" << rankDiff << " ScoreDiff:" << scoreDiff << " LevelDiff:" << levelDiff
        << " | Allowed (R:" << allowedRankDiff << ", S:" << allowedScoreDiff << ", L:" << allowedLevelDiff << ")\n";

    return (rankDiff <= allowedRankDiff &&
        scoreDiff <= allowedScoreDiff &&
        levelDiff <= allowedLevelDiff);
}

void MatchManager::CheckMatch()
{
    std::lock_guard<std::mutex> lock(mtx);
    if (waitingPlayers.size() < 2) return;

    for (size_t i = 0; i < waitingPlayers.size(); ++i)
    {
        for (size_t j = i + 1; j < waitingPlayers.size(); ++j)
        {
            if (IsMatchable(waitingPlayers[i], waitingPlayers[j]))
            {
                MatchPlayer p1 = waitingPlayers[i];
                MatchPlayer p2 = waitingPlayers[j];

                bool swap = (rand() % 2 == 0);
                if (swap) std::swap(p1, p2);

                // RoomPlayer 생성
                RoomPlayer r1{ p1.id, p1.nickname, p1.socket, p1.level, p1.rank, p1.rankScore };
                RoomPlayer r2{ p2.id, p2.nickname, p2.socket, p2.level, p2.rank, p2.rankScore };

                r1.spawnPos = "A";
                r2.spawnPos = "B";

                int roomId = roomMgr->CreateRoom(r1, r2);

                std::string res1 = "{ \"type\":\"match_found\", \"roomId\":" + std::to_string(roomId) +
                    ", \"opponent\":\"" + p2.nickname + "\" }";
                std::string res2 = "{ \"type\":\"match_found\", \"roomId\":" + std::to_string(roomId) +
                    ", \"opponent\":\"" + p1.nickname + "\" }";

                uint32_t len1 = (uint32_t)res1.size();
                send(p1.socket, (char*)&len1, 4, 0);
                send(p1.socket, res1.c_str(), len1, 0);

                uint32_t len2 = (uint32_t)res2.size();
                send(p2.socket, (char*)&len2, 4, 0);
                send(p2.socket, res2.c_str(), len2, 0);

                if (j > i)
                {
                    waitingPlayers.erase(waitingPlayers.begin() + j);
                    waitingPlayers.erase(waitingPlayers.begin() + i);
                }

                std::cout << "[Match] Matched " << p1.nickname << " & " << p2.nickname
                    << " → Room #" << roomId
                    << " (" << r1.nickname << "=A, " << r2.nickname << "=B)"
                    << std::endl;

                return; // 한 번에 하나의 매치만 처리
            }
        }
    }
}

void MatchManager::RemovePlayer(const std::string& nickname)
{
    for (auto it = waitingPlayers.begin(); it != waitingPlayers.end(); ++it)
    {
        if (it->nickname == nickname)
        {
            waitingPlayers.erase(it);
            std::cout << "[Match] Player removed from queue: " << nickname << "\n";
            return;
        }
    }

    std::cout << "[Match] Player not found in queue: " << nickname << "\n";
}
