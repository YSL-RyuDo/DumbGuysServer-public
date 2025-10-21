#pragma once
#include <unordered_map>
#include "Room.h"


class Room;

class RoomManager
{
private:
    std::unordered_map<int, Room> rooms;
    int nextRoomId = 1;

public:
    int CreateRoom(const RoomPlayer& p1, const RoomPlayer& p2);
    Room* GetRoom(int roomId);
    int FindRoomByPlayer(const std::string& playerId) const;
    Room* FindRoomByPlayer(const std::string& nickname);
    void RemoveRoom(int roomId);
};
