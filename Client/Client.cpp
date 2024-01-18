#include <iostream>
#include "Communicator.h"

// KEYWORDS:
// setting
// todo

int main()
{
	Communicator communicator;

	std::string filename;
	std::string content;


	int command;
	while (true)
	{
		bool running = true;

		std::cout << "Please, enter a command to execute: ";
		std::cin >> command;
		switch (command)
		{
		case 0:
			communicator.stop();
			running = false;
			break;
		case 1:
			filename = "doc.txt";
			//filename = "sampleMb.txt";
			//filename = "sample2Mb.txt";
			//filename = "doc.txt.txt"; // <-- smallest
			//filename = "sampleGb.txt"; // <-- biggest
			std::cout << communicator.get(filename);
			break;
		case 2:
			std::cout << communicator.list(".");
			break;
		case 3:
			filename = "newFile.txt";
			content = "This is some content.";
			std::cout << communicator.put(filename, content);
			break;
		case 4:
			filename = "emptyFile";
			std::cout << communicator.put(File, filename);
			break;
		case 5:
			filename = "emptyDirectory";
			std::cout << communicator.put(Directory, filename);
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