#pragma once
#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include "ClientsHandler.h"

namespace fs = std::filesystem;

enum Content {
	File,
	Directory
};

class Executor
{
	ClientsHandler server;
public:
	Executor() {};

	std::string receiveMessage() {
		return server.receiveChunkedData();
	}

	void sendMessage(std::string& message, const char operationType = '-') {
		server.sendChunkedData(message, 10, operationType);
	}

	char receiveOptionType() {
		return server.receiveOptionType();
	}

	std::string fullPath(const std::string& filename) {
		return (fs::path("serverFolder/") / filename).string();
	}

	void get(const std::string& filename) {
		const int chunkSize = 1024; // setting: chunkSize = 1024 * 1024;
		char* buffer = new char[chunkSize + 1];

		std::ifstream file(filename, std::ios::binary);

		if (!file.is_open()) {
			std::string message = "Error: unable to open file.";
			sendMessage(message);
			delete[] buffer;
			return;
		}

		while (!file.eof()) {
			file.read(buffer, chunkSize);
			std::streamsize bytesRead = file.gcount();
			buffer[bytesRead] = '\0';

			if (bytesRead > 0) {
				char indicator = (!file.eof()) ? '+' : '-';
				std::cout << "chunk " << indicator << std::endl;
				std::string message = buffer;
				sendMessage(message, indicator);
			}

		}

		delete[] buffer;

		file.close();
	}

	std::string list(const std::string& directory) {
		std::string result;
		try {
			for (const auto& entry : fs::directory_iterator(directory)) {
				result += entry.path().filename().string() + '\n';
			}
			sendMessage(result);
			return result;
		}
		catch (const std::exception& e) {
			result = "Error: " + std::string(e.what());
			sendMessage(result);
			return result;
		}
	}

	std::string file(const std::string& str) { // const std::string& name, const std::string& content
		return std::string(); // operation success -> 1/0
	}

	std::string directory(const std::string& name) {
		return std::string(); // operation success -> 1/0
	}

	std::string remove(const std::string& str) { // const std::string& name
		return std::string(); // operation success -> 1/0
	}
	
	std::vector<std::string> info(const std::string& str) { // const Content type, const std::string& name, const std::string& content
		return std::vector<std::string>(); // Key-Value pairs
	}


};

