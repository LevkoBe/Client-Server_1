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

	char* receiveMessage() {

		// Receive data from the client
		char buffer[1024];
		memset(buffer, 0, 1024);
		int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
		if (bytesReceived > 0)
		{
			std::cout << "Received data: " << buffer << std::endl;
			// Send a response back to the client
			const char* response = "Hello, client! This is the server.";
			send(clientSocket, response, (int)strlen(response), 0);
		}
		return buffer;
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
			sendMessage(std::move(sliced));
			std::cout << "New message sent. ";
		}
		return true;
	}
};
