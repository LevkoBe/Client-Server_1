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

	bool sendChunkedData(const char* data, int dataSize, int chunkSize, char operationType) {
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

			if (send(clientSocket, data + totalSent, currentChunkSize, 0) == SOCKET_ERROR) {
				std::cerr << "Failed to send chunked data." << std::endl;
				break;
			}

			totalSent += currentChunkSize;
			std::cout << "Sent chunk of size " << currentChunkSize << std::endl;
		}
		return true;
	}


	std::string receiveMessage() {
		char buffer[1024];
		memset(buffer, 0, sizeof(buffer));
		int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
		if (bytesReceived > 0) {
			std::cout << "Received from server: " << buffer << std::endl;
		}
		return std::string(buffer);
	}


	char receiveApproval() {
		char buffer[2]; // todo: find correct size
		memset(buffer, 0, 2);
		int bytesReceived = recv(clientSocket, buffer, sizeof(char), 0);
		if (bytesReceived > 0) {
			std::cout << "Received from server: " << buffer << std::endl;
		}
		return buffer[0];
	}
};
