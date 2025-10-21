#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")

#include "ClientHandler.h"
#include "UserAccountManager.h"
#include "PlayerDataManager.h"
#include "AppearanceManager.h"
#include "OwnedItemManager.h"
#include "MatchManager.h"
#include "RoomManager.h"

class Server
{
private:
    int port_;

public:
    Server(int port);
    void Run();
};
