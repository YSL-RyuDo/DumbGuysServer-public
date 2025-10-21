#include "Room.h"
#include <iostream>

Room::Room() : roomId(-1) {}

Room::Room(int id, const RoomPlayer& p1, const RoomPlayer& p2)
    : roomId(id), player1(p1), player2(p2) {
    mapChoice1.nickname = p1.nickname;
    mapChoice2.nickname = p2.nickname;
    mapChoice1.mapIndex = -2;
    mapChoice2.mapIndex = -2;
}

void Room::SetPlayerReady(const std::string& nickname, bool ready)
{
    if (player1.nickname == nickname)
        player1.isReady = ready;
    else if (player2.nickname == nickname)
        player2.isReady = ready;
}

int Room::ReadyCount() const
{
    return (player1.isReady ? 1 : 0) + (player2.isReady ? 1 : 0);
}

bool Room::IsBothReady() const
{
    return player1.isReady && player2.isReady;
}

bool Room::HasPlayer(const std::string& nickname) const
{
    return player1.nickname == nickname || player2.nickname == nickname;
}
