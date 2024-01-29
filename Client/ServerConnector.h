#pragma once

#include <iostream>
#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <conio.h>

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

	SOCKET getClientSocket() {
		return clientSocket;
	}

	bool connectToServer(PCWSTR serverIp, int port) {
		this->serverIp = serverIp;
		this->port = port;

		sockaddr_in serverAddr;
		serverAddr.sin_family = AF_INET;
		serverAddr.sin_port = htons(port);
		InetPton(AF_INET, serverIp, &serverAddr.sin_addr);

		while (connect(clientSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR) {
			std::cerr << "\nConnect failed with error: " << WSAGetLastError() << std::endl;
			std::cerr << "Please, press any key to retry. " << std::endl;
			std::cout << _getch() << std::endl;
			//return false;
		}
		return true;
	}

	char receiveApproval() {
		char approval;
		int bytesReceived = recv(clientSocket, reinterpret_cast<char*>(&approval), sizeof(char), 0);
		if (bytesReceived > 0) {
			std::cout << "[------------Received data: " << approval << "------------]" << std::endl;
		}
		return approval;
	}
};
