#include <iostream>
#include "Executor.h"

int main()
{
	Executor executor;

	for (int i = 0; i < 100; i++) // New request
	{
		char optionType = executor.receiveOptionType(); // todo: remake into std::string
		std::string message = executor.receiveMessage();
		std::cout << i << std::endl;
		switch (optionType)
		{
		case 'g':
			executor.get("serverFolder/", message);
			break;
		case 'l':
			executor.list(message);
			break;
		case 'f':
			executor.file(message);
			break;
		case 'd':
			executor.directory(message);
			break;
		case 'r':
			executor.remove(message);
			break;
		case 'i':
			executor.info(message);
			break;
		default:
			break;
		}
		executor.sendMessage();
	}

	return 0;
}