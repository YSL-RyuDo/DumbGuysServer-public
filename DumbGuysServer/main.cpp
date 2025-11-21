#include "Server.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
StoreManager* storeMgr = nullptr;

int main()
{
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        printf("WSAStartup failed.\n");
        return 1;
    }
<<<<<<< Updated upstream

    Server server(9000);
=======
    storeMgr = new StoreManager();
    Server server(7777);
>>>>>>> Stashed changes
    server.Run();

    WSACleanup();
    return 0;
}
