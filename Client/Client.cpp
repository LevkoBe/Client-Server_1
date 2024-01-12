#include <iostream>
#include "ServerConnector.h"

int main()
{
	ServerConnector client;
	PCWSTR serverIP = L"127.0.0.1";
	int port = 12345;
	client.connectToServer(serverIP, port);

	// till this stndard. Incapsulate in class
	// Send, receive --> for files changes

	const char* message;
	
	message = "Hello, server! How are you?";
	client.sendMessage(message);
	client.receiveMessage();

	// Cleanup is in the ServerConnector destructor
	return 0;
}