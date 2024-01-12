#pragma once

#include <iostream>
#include <WinSock2.h>
#include <Ws2tcpip.h>

// Linking the library needed for network communication
#pragma comment(lib, "ws2_32.lib")

class ServerConnector
{
	WSADATA wsaData;
	int port;
	PCWSTR serverIp;
	SOCKET clientSocket;

public:
	ServerConnector() {
		// Initialize Winsock
		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		{
			std::cerr << "WSAStartup failed" << std::endl;
			return;
		}

		// Client configuration
		port = 12345;
		serverIp = L"127.0.0.1";
		clientSocket = socket(AF_INET, SOCK_STREAM, 0);
		if (clientSocket == INVALID_SOCKET)
		{
			std::cerr << "Error creating socket: " << WSAGetLastError() << std::endl;
			WSACleanup();
			return;
		}
	}

	~ServerConnector() {
		closesocket(clientSocket);
		WSACleanup();
	}

	bool connectToServer(PCWSTR serverIp, int port) {
		this->serverIp = serverIp;
		this->port = port;

		sockaddr_in serverAddr;
		serverAddr.sin_family = AF_INET;
		serverAddr.sin_port = htons(port);
		InetPton(AF_INET, serverIp, &serverAddr.sin_addr);

		if (connect(clientSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR) {
			std::cerr << "Connect failed with error: " << WSAGetLastError() << std::endl;
			return false;
		}
		return true;
	}

	void sendMessage(const char* message) {
		send(clientSocket, message, (int)strlen(message), 0);
	}

	void receiveMessage() {
		char buffer[32767];
		memset(buffer, 0, 32767);
		int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
		if (bytesReceived > 0) {
			std::cout << "Received from server: " << buffer << std::endl;
		}
	}
};
