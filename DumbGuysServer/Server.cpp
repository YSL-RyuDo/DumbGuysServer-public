#include "Server.h"
#include <iostream>
#include <thread>

Server::Server(int port) : port_(port) {}

void Server::Run()
{
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET)
    {
        std::cerr << "[Server] Failed to create socket.\n";
        return;
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port_);

    if (bind(serverSocket, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR)
    {
        std::cerr << "[Server] Bind failed.\n";
        closesocket(serverSocket);
        return;
    }

    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        std::cerr << "[Server] Listen failed.\n";
        closesocket(serverSocket);
        return;
    }

    std::cout << "[Server] Listening on port " << port_ << "...\n";

    // === 매니저 생성 ===
    UserAccountManager accountMgr;
    PlayerDataManager playerDataMgr;
    AppearanceManager appearanceMgr;
    OwnedItemManager ownedItemMgr;
    RoomManager roomMgr;
    MatchManager matchMgr(&roomMgr);

    // === 클라이언트 연결 대기 ===
    while (true)
    {
        sockaddr_in clientAddr{};
        int clientSize = sizeof(clientAddr);
        SOCKET clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientSize);
        if (clientSocket == INVALID_SOCKET) continue;

        std::thread([=, &accountMgr, &playerDataMgr, &appearanceMgr, &ownedItemMgr, &roomMgr, &matchMgr]() mutable {
            ClientHandler handler(
                clientSocket,
                clientAddr,
                &accountMgr,
                &playerDataMgr,
                &appearanceMgr,
                &ownedItemMgr,
                &roomMgr,
                &matchMgr
            );
            handler.Run();
            }).detach();
    }

    closesocket(serverSocket);
    std::cout << "[Server] Shutdown complete.\n";
}
