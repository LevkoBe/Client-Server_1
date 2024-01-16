#pragma once

#include <iostream>
#include <WinSock2.h>
#include <Ws2tcpip.h>

// Linking the library needed for network communication
#pragma comment(lib, "ws2_32.lib")

class ClientsHandler
{
	WSADATA wsaData;
	SOCKET clientSocket;
	SOCKET serverSocket;
	int port;

	bool bindSocket() {
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

	bool listenIncomingConnections() {

		if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR)
		{
			std::cerr << "Listen failed with error: " << WSAGetLastError() << std::endl;
			closesocket(serverSocket);
			WSACleanup();
			return false;
		}
		return true;
	}

	bool acceptClientConnection() {
		clientSocket = accept(serverSocket, nullptr, nullptr);
		if (clientSocket == INVALID_SOCKET)
		{
			std::cerr << "Accept failed with error: " << WSAGetLastError() << std::endl;
			closesocket(serverSocket);
			WSACleanup();
			return false;
		}
		return true;
	}

public:
	ClientsHandler() {
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
		std::cout << "Server is listening on port " << port << std::endl;

		if (!acceptClientConnection()) return;
	}

	~ClientsHandler() {
		closesocket(clientSocket);
		closesocket(serverSocket);
		WSACleanup();
	}

	char receiveOptionType() {
		// Receive data from the client
		char option;
		int bytesReceived = recv(clientSocket, reinterpret_cast<char*>(&option), sizeof(char), 0);
		if (bytesReceived > 0) {
			std::cout << "-!----------Received data: " << option << std::endl;
		}
		return option;
	}

	std::string receiveChunkedData() {
		// Receive chunk size
		int chunkSize;
		int bytesReceived = recv(clientSocket, reinterpret_cast<char*>(&chunkSize), sizeof(int), 0);
		if (bytesReceived == SOCKET_ERROR || bytesReceived == 0) {
			std::cerr << "Error in receiving chunk size." << std::endl;
			return "";
		}

		// Receive total size
		int totalSize;
		bytesReceived = recv(clientSocket, reinterpret_cast<char*>(&totalSize), sizeof(int), 0);
		if (bytesReceived == SOCKET_ERROR || bytesReceived == 0) {
			std::cerr << "Error in receiving total size." << std::endl;
			return "";
		}

		std::cout << "-!-Chunk size received: " << chunkSize << std::endl;
		std::cout << "-!-Total size received: " << totalSize << std::endl;

		// Receive message
		std::string assembledData;
		int totalReceived = 0;

		char* buffer = new char[chunkSize + 1];
		while (totalReceived < totalSize) {
			int remaining = totalSize - totalReceived;
			int currentChunkSize = (remaining < chunkSize) ? remaining : chunkSize;
			int bytesReceived = recv(clientSocket, buffer, currentChunkSize, 0);
			buffer[bytesReceived] = '\0';

			if (bytesReceived == SOCKET_ERROR || bytesReceived == 0) {
				std::cerr << "Error in receiving chunked data." << std::endl;
				return "";
			}

			assembledData.append(buffer, bytesReceived);
			totalReceived += bytesReceived;
			//std::cout << "Received chunk of size " << bytesReceived << std::endl;
		}

		std::cout << "-!-Assembled data on the server: " << assembledData << std::endl;
		delete[] buffer;
		return assembledData;
	}

	bool sendChunkedData(std::string& messageStr, int chunkSize, char operationType) {
		const char* message = messageStr.c_str();
		int dataSize = strlen(message);
		// Send operation type
		if (send(clientSocket, reinterpret_cast<const char*>(&operationType), sizeof(char), 0) == SOCKET_ERROR) {
			std::cerr << "Failed to send type of the operation." << std::endl;
			return false;
		}
		// Send chunk size
		if (send(clientSocket, reinterpret_cast<const char*>(&chunkSize), sizeof(int), 0) == SOCKET_ERROR) {
			std::cerr << "Failed to send chunk size." << std::endl;
			return false;
		}
		// Send total size first
		if (send(clientSocket, reinterpret_cast<const char*>(&dataSize), sizeof(int), 0) == SOCKET_ERROR) {
			std::cerr << "Failed to send total size." << std::endl;
			return false;
		}

		int totalSent = 0;

		while (totalSent < dataSize) {
			int remaining = dataSize - totalSent;
			int currentChunkSize = (remaining < chunkSize) ? remaining : chunkSize;

			if (send(clientSocket, message + totalSent, currentChunkSize, 0) == SOCKET_ERROR) {
				std::cerr << "Failed to send chunked data." << std::endl;
				break;
			}

			totalSent += currentChunkSize;
			//std::cout << "Sent chunk of size " << currentChunkSize << std::endl;
		}

		return true;
	}

};

