#include <iostream>
#include "Executor.h"

int main()
{
	Executor executor;

	while (true) // New request
	{
		bool running = true;
		char optionType = executor.receiveOptionType();
		std::string message = executor.receiveMessage();
		std::string response = std::string();
		switch (optionType)
		{
		case 'g':
			executor.get(executor.fullPath(message));
			break;
		case 'l':
			response = executor.list(executor.fullPath(message));
			break;
		case 'f':
			response = executor.file(message);
			break;
		case 'd':
			response = executor.directory(message);
			break;
		case 'r':
			response = executor.remove(message);
			break;
		case 'i':
			//response = executor.info(message);
			break;
		case '-':
			running = false;
		default:
			break;
		}
		if (!running) {
			break;
		}
	}

	return 0;
}