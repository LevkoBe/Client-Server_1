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
		char buffer[2];
		memset(buffer, 0, 2);
		int bytesReceived = recv(clientSocket, buffer, sizeof(char), 0);
		buffer[1] = '\0';
		if (bytesReceived > 0) {
			std::cout << "Received data: " << buffer << std::endl;
		}
		return buffer[0];
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

		std::cout << "Chunk size received: " << chunkSize << std::endl;
		std::cout << "Total size received: " << totalSize << std::endl;

		// Receive message
		std::string assembledData;
		int totalReceived = 0;

		char* buffer = nullptr;
		while (totalReceived < totalSize) {
			buffer = new char[chunkSize];
			int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);

			if (bytesReceived == SOCKET_ERROR || bytesReceived == 0) {
				std::cerr << "Error in receiving chunked data." << std::endl;
				return "";
			}

			assembledData.append(buffer, bytesReceived);
			totalReceived += bytesReceived;
			std::cout << "Received chunk of size " << bytesReceived << std::endl;
		}

		std::cout << "Assembled data on the server: " << assembledData << std::endl;
		delete[] buffer;
		return assembledData;
	}

	bool sendMessage(const char* message) {
		int length = (int)strlen(message);
		if (length <= 1024) {
			try {
				send(clientSocket, message, (int)strlen(message), 0);
			}
			catch (const std::exception&) {
				return false;
			}
			return true;
		}
		for (int i = 0; i < length / 1023; i++) {
			char* sliced = new char[1024];
			std::memcpy(sliced, message + i * 1023, 1023);
			sliced[1023] = '\0';
			sendMessage(std::move(sliced)); // question: memory deallocation
			delete[] sliced;
			std::cout << "New message sent. ";
		}
		return true;
	}
};

