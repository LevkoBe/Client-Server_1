#pragma once

#include <string>
#include <WinSock2.h>

class CommonMethods {
public:
    static bool sendChunkedData(const std::string& messageStr, const int chunkSize, const char operationType, SOCKET clientSocket)
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
            //m.lock();
            std::cerr << "Failed to send chunk size." << std::endl;
            //m.unlock();
            return false;
        }
        // Send total size first
        if (send(clientSocket, reinterpret_cast<const char*>(&dataSize), sizeof(int), 0) == SOCKET_ERROR)
        {
            //m.lock();
            std::cerr << "Failed to send total size." << std::endl;
            //m.unlock();
            return false;
        }

        int totalSent = 0;

        while (totalSent < dataSize)
        {
            int remaining = dataSize - totalSent;
            int currentChunkSize = (remaining < chunkSize) ? remaining : chunkSize;

            if (send(clientSocket, message + totalSent, currentChunkSize, 0) == SOCKET_ERROR)
            {
                //m.lock();
                std::cerr << "Failed to send chunked data." << std::endl;
                //m.unlock();
                break;
            }

            totalSent += currentChunkSize;
        }

        return true;
    }

    static std::string receiveChunkedData(SOCKET clientSocket)
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

        //m.lock();
        std::cout << "-!-Chunk size received: " << chunkSize << std::endl;
        std::cout << "-!-Total size received: " << totalSize << std::endl;
        //m.unlock();

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

        //m.lock();
        std::cout << "-!-Assembled data on the server: " << assembledData << std::endl;
        //m.unlock();
        delete[] buffer;
        return assembledData;
    }
    
    static char receiveOptionType(SOCKET clientSocket)
    {
        // Receive data from the client
        char option;
        int bytesReceived = recv(clientSocket, reinterpret_cast<char*>(&option), sizeof(char), 0);
        if (bytesReceived == SOCKET_ERROR) {
            return '-';
        }
        if (bytesReceived > 0)
        {
            //m.lock();
            std::cout << "-!----------Received data: " << option << std::endl;
            //m.unlock(); // todo
        }
        return option;
    }

};
