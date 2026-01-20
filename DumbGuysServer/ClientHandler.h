#pragma once
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")

#include "UserAccountManager.h"
#include "PlayerDataManager.h"
#include "AppearanceManager.h"
#include "OwnedItemManager.h"
#include "RoomManager.h"
#include "MatchManager.h"
#include "StoreManager.h"

extern StoreManager* storeMgr;

class ClientHandler
{
public:
    ClientHandler(
        SOCKET clientSocket,
        sockaddr_in clientAddr,
        UserAccountManager* accountMgr,
        PlayerDataManager* playerDataMgr,
        AppearanceManager* appearanceMgr,
        OwnedItemManager* ownedItemMgr,
        RoomManager* roomMgr,
        MatchManager* matchMgr
    );

    void Run();

private:
    SOCKET socket_;
    sockaddr_in addr_;
    UserAccountManager* accountMgr;
    PlayerDataManager* playerDataMgr;
    AppearanceManager* appearanceMgr;
    OwnedItemManager* ownedItemMgr;
    RoomManager* roomMgr;
    MatchManager* matchMgr;

    std::string accountId;
    std::string nickname;

    bool recvFull(void* buffer, int length);
    bool sendFull(const void* buffer, int length);
};
