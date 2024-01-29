#pragma once

#include <iostream>
#include <vector>
#include <mutex>
#include <WinSock2.h>
#include <Ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

class ClientsHandler
{
    WSADATA wsaData;
    std::vector<SOCKET> clientSockets;
    SOCKET serverSocket;
    int port;

    bool bindSocket();

    bool listenIncomingConnections();

public:
    ClientsHandler();

    SOCKET acceptClientConnection();

    ~ClientsHandler();
};
