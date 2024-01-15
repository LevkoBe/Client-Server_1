#include <iostream>
#include "Communicator.h"

int main()
{
	Communicator communicator;

	const char* filename;
	filename = "doc.txt";

	std::cout << communicator.get(filename);

	return 0;
}