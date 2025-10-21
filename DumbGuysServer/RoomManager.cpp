#include "RoomManager.h"
#include "Room.h"
#include <iostream>

int RoomManager::CreateRoom(const RoomPlayer& p1, const RoomPlayer& p2)
{
    int roomId = nextRoomId++;

    // 새로운 Room 생성 및 등록
    Room newRoom(roomId, p1, p2);
    rooms[roomId] = newRoom;

    std::cout << "[RoomManager] Room #" << roomId
        << " created between " << p1.nickname
        << " and " << p2.nickname << std::endl;

    // 각 클라이언트에게 방 입장 메시지를 보낼 수도 있음
    std::string msg1 = "{ \"type\":\"room_joined\", \"roomId\":" + std::to_string(roomId) +
        ", \"opponent\":\"" + p2.nickname + "\" }";
    std::string msg2 = "{ \"type\":\"room_joined\", \"roomId\":" + std::to_string(roomId) +
        ", \"opponent\":\"" + p1.nickname + "\" }";

    uint32_t len1 = (uint32_t)msg1.size();
    send(p1.socket, (char*)&len1, 4, 0);
    send(p1.socket, msg1.c_str(), len1, 0);

    uint32_t len2 = (uint32_t)msg2.size();
    send(p2.socket, (char*)&len2, 4, 0);
    send(p2.socket, msg2.c_str(), len2, 0);

    return roomId;
}

Room* RoomManager::GetRoom(int roomId)
{
    auto it = rooms.find(roomId);
    if (it != rooms.end())
    {
        return &it->second;
    }
    return nullptr;
}

int RoomManager::FindRoomByPlayer(const std::string& playerId) const
{
    for (const auto& kv : rooms)
    {
        const Room& r = kv.second;
        if (r.player1.id == playerId || r.player2.id == playerId)
            return kv.first; // roomId
    }
    return -1;
}

Room* RoomManager::FindRoomByPlayer(const std::string& nickname)
{
    for (auto it = rooms.begin(); it != rooms.end(); ++it)
    {
        Room& room = it->second;
        if (room.HasPlayer(nickname))
            return &room;
    }
    return nullptr;
}

void RoomManager::RemoveRoom(int roomId)
{
    auto it = rooms.find(roomId);
    if (it != rooms.end())
    {
        rooms.erase(it);
        std::cout << "[RoomManager] Room #" << roomId << " deleted.\n";
    }
}
