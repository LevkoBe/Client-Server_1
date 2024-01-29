#include "ClientsHandler.h"

std::mutex m;

ClientsHandler::ClientsHandler()
{
    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        std::cerr << "WSAStartup failed" << std::endl;
        return;
    }
    // Server configuration
    port = 12345;
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET)
    {
        std::cerr << "Error creating socket: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return;
    }

    if (!bindSocket()) return;

    if (!listenIncomingConnections()) return;

    m.lock();
    std::cout << "Server is listening on port " << port << std::endl;
    m.unlock();

    // if (!acceptClientConnection()) return;
}

ClientsHandler::~ClientsHandler()
{
    for (auto& clientSocket : clientSockets) {
        closesocket(clientSocket);
    }
    closesocket(serverSocket);
    WSACleanup();
}

bool ClientsHandler::bindSocket()
{
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    if (bind(serverSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR)
    {
        std::cerr << "Bind failed with error: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return false;
    }
    return true;
}

bool ClientsHandler::listenIncomingConnections()
{
    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        std::cerr << "Listen failed with error: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return false;
    }
    return true;
}

SOCKET ClientsHandler::acceptClientConnection()
{
    SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);
    if (clientSocket == INVALID_SOCKET)
    {
        std::cerr << "Accept failed with error: " << WSAGetLastError() << std::endl;
        return INVALID_SOCKET;
    }
    clientSockets.push_back(clientSocket);
    return clientSocket;
}