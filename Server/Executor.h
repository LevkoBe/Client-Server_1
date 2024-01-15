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

	void sendMessage() {
		server.sendMessage("fine");
	}

	char receiveOptionType() {
		return server.receiveOptionType();
	}

	std::string get(const std::string& filename) {
		std::ifstream file(filename);
		if (file.is_open()) {
			std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
			file.close();
			server.sendMessage(content.c_str());
			return content;
		}
		else {
			return "Error: unable to open file.";
		}
	}

	std::string get(const char* filepath, const std::string& filename) {
		std::string fullPath = (fs::path(filepath) / filename).string();
		return get(fullPath.c_str());
	}

	std::string list(const std::string& directory) {
		return std::string(); // folder contents
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

