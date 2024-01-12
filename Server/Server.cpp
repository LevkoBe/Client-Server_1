#include <iostream>
#include "ClientsHandler.h"

int main()
{
	ClientsHandler server;
	server.receiveMessage();

	while (false)
	{
		char cmd = 0;
		server.receiveMessage();
		switch (cmd)
		{
		case 1:
			break;
		default:
			break;
		}
	}

	// Clean up in ClientsHandler destructor
	return 0;
}