#include <iostream>
#include <thread>
#include "Executor.h"

Executor executor;

void handleClient(SOCKET clientSocket) {

	std::string username = "";
	while (true) // New request
	{
		bool running = true;
		char optionType = executor.receiveOptionType(clientSocket);
		std::string message = executor.receiveMessage(clientSocket);
		switch (optionType)
		{
		case 'u': // user registration
			username = message;
			executor.checkFolder(username, clientSocket);
			break;
		case 'g':
			executor.get(executor.fullPath(message, username), clientSocket);
			break;
		case 'l':
			executor.list(executor.fullPath(message, username), clientSocket);
			break;
		case 'f':
			executor.file(executor.fullPath(message, username), clientSocket);
			break;
		case 'd':
			executor.directory(executor.fullPath(message, username), clientSocket);
			break;
		case 'r':
			executor.remove(executor.fullPath(message, username), clientSocket); // change return type
			break;
		case 'i':
			executor.info(executor.fullPath(message, username), clientSocket);
			break;
		case 'a':
			executor.addToFile(executor.fullPath(message, username), clientSocket);
			break;
		case '-':
			running = false;
			break;
		default:
			break;
		}
		if (!running) {
			break;
		}
	}
}

int main()
{

	std::vector<std::thread> threads;
	while (true) {
		SOCKET clientSocket = executor.acceptClientConnection();
		if (clientSocket == INVALID_SOCKET) {
			continue;
		}
		threads.emplace_back(handleClient, clientSocket);
	}
	for (auto& thread : threads) {
		thread.join();
	}

	return 0;
}

 //cd C:\Users\1levk\source\repos\Client-Server_1\x64\Debug
 //Client.exe