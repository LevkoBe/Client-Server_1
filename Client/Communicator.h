#pragma once
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include "ServerConnector.h"
#include "../CommonMethods.h"

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
	int CHUNK_SIZE = 10; // setting

	std::vector<std::string> splitString(const std::string& str, char delimiter = '\n') {
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
		CommonMethods::sendChunkedData(messageStr, CHUNK_SIZE, 'g', client.getClientSocket());

		bool responseIsOver = true;
		if (CommonMethods::receiveOptionType(client.getClientSocket()) == '+') {
			responseIsOver = false;
		}
		messageStr = CommonMethods::receiveChunkedData(client.getClientSocket()); // content of the file

		while (!responseIsOver) {
			if (CommonMethods::receiveOptionType(client.getClientSocket()) != '+') {
				responseIsOver = true;
			}
			messageStr += CommonMethods::receiveChunkedData(client.getClientSocket()); // content of the file
		}
		return messageStr;
	}

	std::string authorize(std::string& username) {

		CommonMethods::sendChunkedData(username, CHUNK_SIZE, 'u', client.getClientSocket());
		CommonMethods::receiveOptionType(client.getClientSocket());
		return CommonMethods::receiveChunkedData(client.getClientSocket());
	}

	std::string list(const std::string& filename) {
		std::string messageStr = filename;
		CommonMethods::sendChunkedData(messageStr, CHUNK_SIZE, 'l', client.getClientSocket());
		CommonMethods::receiveOptionType(client.getClientSocket());
		return CommonMethods::receiveChunkedData(client.getClientSocket());
	}

	std::string put(const std::string& filename, const std::string& content) { // creates file with data
		std::string messageStr = filename + '\n' + content;
		CommonMethods::sendChunkedData(messageStr, CHUNK_SIZE, 'f', client.getClientSocket());
		CommonMethods::receiveOptionType(client.getClientSocket());
		return CommonMethods::receiveChunkedData(client.getClientSocket());
	}


	void sendFile(const std::string& filename) {
		const int chunkSize = 1024 * 1024; // setting: chunkSize = 1024 * 1024;
		//const int chunkSize = 10 * 1024 * 1024; // setting: chunkSize = 1024 * 1024;
		char* buffer = new char[chunkSize + 1];

		std::ifstream file(filename, std::ios::binary);

		if (!file.is_open()) {
			std::cout << "Error: unable to open file.";
			delete[] buffer;
			return;
		}
		
		char indicator = 'f';
		while (!file.eof()) {
			file.read(buffer, chunkSize);
			std::streamsize bytesRead = file.gcount();
			buffer[bytesRead] = '\0';

			if (bytesRead > 0) {
				std::cout << "chunk " << indicator << std::endl;
				std::string message = filename + '\n' + buffer;

				CommonMethods::sendChunkedData(message, CHUNK_SIZE, indicator, client.getClientSocket());
				CommonMethods::receiveOptionType(client.getClientSocket());
				CommonMethods::receiveChunkedData(client.getClientSocket());
				indicator = 'a';
			}
		}
		delete[] buffer;

		file.close();
	}

	std::string add(const std::string& filename, const std::string& content) { // creates file with data
		std::string messageStr = filename + '\n' + content;
		CommonMethods::sendChunkedData(messageStr, CHUNK_SIZE, 'a', client.getClientSocket());
		CommonMethods::receiveOptionType(client.getClientSocket());
		return CommonMethods::receiveChunkedData(client.getClientSocket());
	}

	std::string put(const Content type, const std::string& filename) { // creates empty file/folder
		std::string messageStr = filename;
		switch (type) {
		case File:
			messageStr += '\n';
			CommonMethods::sendChunkedData(messageStr, CHUNK_SIZE, 'f', client.getClientSocket());
			break;
		case Directory:
			CommonMethods::sendChunkedData(messageStr, CHUNK_SIZE, 'd', client.getClientSocket());
			break;
		}
		CommonMethods::receiveOptionType(client.getClientSocket());
		return CommonMethods::receiveChunkedData(client.getClientSocket());
	}

	std::string deleteFile(const std::string& filename) {
		std::string messageStr = filename; // remove
		CommonMethods::sendChunkedData(messageStr, CHUNK_SIZE, 'r', client.getClientSocket());
		CommonMethods::receiveOptionType(client.getClientSocket());
		return CommonMethods::receiveChunkedData(client.getClientSocket());
	}

	std::string info(const std::string& filename) {
		std::string messageStr = filename;
		CommonMethods::sendChunkedData(messageStr, CHUNK_SIZE, 'i', client.getClientSocket());
		CommonMethods::receiveOptionType(client.getClientSocket());
		std::string info = CommonMethods::receiveChunkedData(client.getClientSocket());
		return info;
	}

	void stop() {
		std::string hack = "###Stop the server###";
		CommonMethods::sendChunkedData(hack, CHUNK_SIZE, '-', client.getClientSocket());
	}
};

