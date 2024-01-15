#pragma once
#include <iostream>
#include <sstream>
#include <vector>
#include "ServerConnector.h"

/*
CRUD:
Create -- put (creates file)
Read   -- get (reads file)
Update ~~ put (updates folder)
Delete -- delete (deletes file)
*/

enum Content {
	File,
	Directory
};

class Communicator
{
	ServerConnector client;

	std::vector<std::string> splitString(const std::string& str, char delimiter = '|') {
		std::string part;
		std::vector<std::string> parts;
		std::istringstream stream(str);
		while (std::getline(stream, part, delimiter)) {
			parts.push_back(part);
		}
		return parts;
	}
public:
	Communicator() {
		PCWSTR serverIP = L"127.0.0.1";
		int port = 12345;
		client.connectToServer(serverIP, port);
	}
	
	std::string get(const std::string& filename) {
		std::string messageStr = "g" + filename;
		const char* message = messageStr.c_str();
		client.sendChunkedData(message, strlen(message), 10, 'g');
		return client.receiveMessage(); // content of the file
	}

	std::vector<std::string> list(const std::string& filename) {
		std::string messageStr = filename;
		const char* message = messageStr.c_str();
		client.sendChunkedData(message, strlen(message), 10, 'l');
		std::string str = client.receiveMessage();
		return splitString(str); // folder contents
	}

	bool put(const std::string& filename, std::string& content) { // creates file with data
		std::string messageStr = filename + '|' + content;
		const char* message = messageStr.c_str();
		return client.sendChunkedData(message, strlen(message), 10, 'f');
	}

	bool put(const Content type, const std::string& filename) { // creates empty file/folder
		const char* message{};
		std::string messageStr = filename + '|';
		message = messageStr.c_str();
		switch (type) {
		case File:
			client.sendChunkedData(message, strlen(message), 10, 'f');
			break;
		case Directory:
			client.sendChunkedData(message, strlen(message), 10, 'd');
			break;
		}
		return client.receiveApproval();
	}

	bool deleteFile(const std::string& filename) {
		std::string messageStr = "r" + filename; // remove
		const char* message = messageStr.c_str();
		client.sendChunkedData(message, strlen(message), 10, 'r');
		return client.receiveApproval();
	}

	std::vector<std::string> info(const std::string& filename) {
		std::string messageStr = "i" + filename;
		const char* message = messageStr.c_str();
		client.sendChunkedData(message, strlen(message), 10, 'i');
		std::string str = client.receiveMessage();
		return splitString(str);
	}
};

