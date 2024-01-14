#include <iostream>
#include "Communicator.h"

int main()
{
	Communicator communicator;

	const char* filename;
	filename = "some filename";

	communicator.get(filename);

	return 0;
}