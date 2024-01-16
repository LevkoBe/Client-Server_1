#include <iostream>
#include "Communicator.h"

// KEYWORDS:
// setting
// todo

int main()
{
	Communicator communicator;

	const char* filename;
	filename = "doc.txt";
	//filename = "sampleMb.txt";
	//filename = "sample2Mb.txt";
	//filename = "doc.txt.txt"; // <-- smallest
	//filename = "sampleGb.txt"; // <-- biggest


	int command;
	while (true)
	{
		bool running = true;
		std::cout << "Please, enter a command to execute: ";
		std::cin >> command;
		switch (command)
		{
		case 1:
			std::cout << communicator.get(filename);
			break;
		case 2:
			std::cout << communicator.list(".");
			break;
		case 3:
			communicator.stop();
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