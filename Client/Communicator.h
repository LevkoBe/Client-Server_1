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
	int CHUNK_SIZE = 10;

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
		std::string messageStr = filename;
		client.sendChunkedData(messageStr, CHUNK_SIZE, 'g');

		bool responseIsOver = true;
		if (client.receiveOptionType() == '+') {
			responseIsOver = false;
		}
		client.receiveChunkedData(); // content of the file

		while (!responseIsOver) {
			if (client.receiveOptionType() != '+') {
				responseIsOver = true;
			}
			client.receiveChunkedData(); // content of the file
		}
		return "The request was processed successfully.\n";
	}

	std::string list(const std::string& filename) {
		std::string messageStr = filename;
		client.sendChunkedData(messageStr, CHUNK_SIZE, 'l');
		client.receiveOptionType();
		std::string str = client.receiveChunkedData();
		//return splitString(str); // folder contents
		return str;
	}

	bool put(const std::string& filename, std::string& content) { // creates file with data
		std::string messageStr = filename + '|' + content;
		return client.sendChunkedData(messageStr, CHUNK_SIZE, 'f');
	}

	bool put(const Content type, const std::string& filename) { // creates empty file/folder
		std::string messageStr = filename + '|';
		switch (type) {
		case File:
			client.sendChunkedData(messageStr, CHUNK_SIZE, 'f');
			break;
		case Directory:
			client.sendChunkedData(messageStr, CHUNK_SIZE, 'd');
			break;
		}
		return client.receiveApproval();
	}

	bool deleteFile(const std::string& filename) {
		std::string messageStr = filename; // remove
		client.sendChunkedData(messageStr, CHUNK_SIZE, 'r');
		return client.receiveApproval();
	}

	std::vector<std::string> info(const std::string& filename) {
		std::string messageStr = filename;
		client.sendChunkedData(messageStr, CHUNK_SIZE, 'i');
		std::string str = client.receiveChunkedData();
		return splitString(str);
	}

	void stop() {
		std::string hack = "###Stop server###";
		client.sendChunkedData(hack, CHUNK_SIZE, '-');
	}
};

