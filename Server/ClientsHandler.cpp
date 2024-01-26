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
        //closesocket(serverSocket);
        //WSACleanup();
        return INVALID_SOCKET;
    }
    clientSockets.push_back(clientSocket);
    return clientSocket;
}

char ClientsHandler::receiveOptionType(SOCKET clientSocket)
{
    // Receive data from the client
    char option;
    int bytesReceived = recv(clientSocket, reinterpret_cast<char*>(&option), sizeof(char), 0);
    if (bytesReceived > 0)
    {
        m.lock();
        std::cout << "-!----------Received data: " << option << std::endl;
        m.unlock();
    }
    return option;
}

std::string ClientsHandler::receiveChunkedData(SOCKET clientSocket)
{
    // Receive chunk size
    int chunkSize;
    int bytesReceived = recv(clientSocket, reinterpret_cast<char*>(&chunkSize), sizeof(int), 0);
    if (bytesReceived == SOCKET_ERROR || bytesReceived == 0)
    {
        std::cerr << "Error in receiving chunk size." << std::endl;
        return "";
    }

    // Receive total size
    int totalSize;
    bytesReceived = recv(clientSocket, reinterpret_cast<char*>(&totalSize), sizeof(int), 0);
    if (bytesReceived == SOCKET_ERROR || bytesReceived == 0)
    {
        std::cerr << "Error in receiving total size." << std::endl;
        return "";
    }

    m.lock();
    std::cout << "-!-Chunk size received: " << chunkSize << std::endl;
    std::cout << "-!-Total size received: " << totalSize << std::endl;
    m.unlock();

    // Receive message
    std::string assembledData;
    int totalReceived = 0;

    char* buffer = new char[chunkSize + 1];
    while (totalReceived < totalSize)
    {
        int remaining = totalSize - totalReceived;
        int currentChunkSize = (remaining < chunkSize) ? remaining : chunkSize;
        int bytesReceived = recv(clientSocket, buffer, currentChunkSize, 0);
        buffer[bytesReceived] = '\0';

        if (bytesReceived == SOCKET_ERROR || bytesReceived == 0)
        {
            std::cerr << "Error in receiving chunked data." << std::endl;
            return "";
        }

        assembledData.append(buffer, bytesReceived);
        totalReceived += bytesReceived;
    }

    m.lock();
    std::cout << "-!-Assembled data on the server: " << assembledData << std::endl;
    m.unlock();
    delete[] buffer;
    return assembledData;
}

bool ClientsHandler::sendChunkedData(const std::string& messageStr, const int chunkSize, const char operationType, SOCKET clientSocket)
{
    const char* message = messageStr.c_str();
    int dataSize = strlen(message);
    // Send operation type
    if (send(clientSocket, reinterpret_cast<const char*>(&operationType), sizeof(char), 0) == SOCKET_ERROR)
    {
        std::cerr << "Failed to send type of the operation." << std::endl;
        return false;
    }
    // Send chunk size
    if (send(clientSocket, reinterpret_cast<const char*>(&chunkSize), sizeof(int), 0) == SOCKET_ERROR)
    {
        m.lock();
        std::cerr << "Failed to send chunk size." << std::endl;
        m.unlock();
        return false;
    }
    // Send total size first
    if (send(clientSocket, reinterpret_cast<const char*>(&dataSize), sizeof(int), 0) == SOCKET_ERROR)
    {
        m.lock();
        std::cerr << "Failed to send total size." << std::endl;
        m.unlock();
        return false;
    }

    int totalSent = 0;

    while (totalSent < dataSize)
    {
        int remaining = dataSize - totalSent;
        int currentChunkSize = (remaining < chunkSize) ? remaining : chunkSize;

        if (send(clientSocket, message + totalSent, currentChunkSize, 0) == SOCKET_ERROR)
        {
            m.lock();
            std::cerr << "Failed to send chunked data." << std::endl;
            m.unlock();
            break;
        }

        totalSent += currentChunkSize;
    }

    return true;
}

ClientsHandler::~ClientsHandler()
{
    for (auto& clientSocket : clientSockets)
    {
        closesocket(clientSocket);
    }
    closesocket(serverSocket);
    WSACleanup();
}
