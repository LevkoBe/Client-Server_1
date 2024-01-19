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
		switch (optionType)
		{
		case 'g':
			executor.get(executor.fullPath(message));
			break;
		case 'l':
			executor.list(executor.fullPath(message));
			break;
		case 'f':
			executor.file(executor.fullPath(message));
			break;
		case 'd':
			executor.directory(executor.fullPath(message));
			break;
		case 'r':
			executor.remove(executor.fullPath(message)); // change return type
			break;
		case 'i':
			executor.info(executor.fullPath(message));
			break;
		case 'a':
			executor.addToFile(executor.fullPath(message));
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

	return 0;
}