#include <iostream>
#include <thread>
#include "Executor.h"

Executor executor;

void handleClient(SOCKET clientSocket) {

	while (true) // New request
	{
		bool running = true;
		char optionType = executor.receiveOptionType(clientSocket);
		std::string message = executor.receiveMessage(clientSocket);
		switch (optionType)
		{
		case 'g':
			executor.get(executor.fullPath(message), clientSocket);
			break;
		case 'l':
			executor.list(executor.fullPath(message), clientSocket);
			break;
		case 'f':
			executor.file(executor.fullPath(message), clientSocket);
			break;
		case 'd':
			executor.directory(executor.fullPath(message), clientSocket);
			break;
		case 'r':
			executor.remove(executor.fullPath(message), clientSocket); // change return type
			break;
		case 'i':
			executor.info(executor.fullPath(message), clientSocket);
			break;
		case 'a':
			executor.addToFile(executor.fullPath(message), clientSocket);
			break;
		case '-':
			//running = false;
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
	// Accept and handle incoming connections
	while (true) {
		// Accept a client connection
		SOCKET clientSocket = executor.acceptClientConnection();
		if (clientSocket == INVALID_SOCKET) {
			//std::cerr << "Error accepting client connection: " << strerror(errno) << std::endl;
			continue;
		}
		// Create a new thread to handle the client
		threads.emplace_back(handleClient, clientSocket);
	}
	// Join threads to wait for their completion (this won't be reached in this example)
	for (auto& thread : threads) {
		thread.join();
	}

	return 0;
}