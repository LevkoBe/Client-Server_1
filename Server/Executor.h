#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <vector>
#include <ctime>
#include "ClientsHandler.h"

namespace fs = std::filesystem;

enum Content {
	File,
	Directory
};

class Executor
{
	ClientsHandler server;

	std::vector<std::string> splitStringInTwo(const std::string& str, char delimiter = '\n') {
		std::string part;
		std::vector<std::string> parts;
		std::istringstream stream(str);

		std::getline(stream, part, delimiter); // read the filename
		parts.push_back(part);
		std::getline(stream, part, '\0'); // read all the rest
		parts.push_back(part);
		return parts;
	}
public:
	Executor() {};

	std::string receiveMessage() {
		return server.receiveChunkedData();
	}

	void sendMessage(const std::string& message, const char operationType = '-') {
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

	void list(const std::string& directory) {
		std::string result;
		try {
			for (const auto& entry : fs::directory_iterator(directory)) {
				result += entry.path().filename().string() + '\n';
			}
		}
		catch (const std::exception& e) {
			result = "Error: " + std::string(e.what());
			return;
		}
		sendMessage(result);
	}

	void file(const std::string& str) {
		std::vector<std::string> file = splitStringInTwo(str);
		std::string message;

		if (file.size() != 2) {
			message = "Sorry, the message received is: '" + str + "', but expected were name and content of a file.";
			sendMessage(message);
			return;
		}

		std::string filename = file[0];
		std::string fileContent = file[1];

		std::ofstream outputFile(filename);
		if (!outputFile.is_open()) {
			message = "Sorry, the file is already in use.";
			sendMessage(message);
			return;
		}

		outputFile << fileContent;
		outputFile.close();

		message = "File succesfully written!";
		sendMessage(message);
		return;
	}

	void directory(const std::string& folderPath) {
		std::string message;

		try {
			fs::create_directory(folderPath);
			message = "Folder created succesfully!";
		}
		catch (const std::exception& e) {
			message = "Error creating folder.";
			std::cerr << "Error creating folder: " << e.what() << std::endl;
		}
		sendMessage(message);
	}

	void remove(const std::string& folderPath) {
		std::string message;

		try {
			fs::remove_all(folderPath);
			message = "Removed succcesfully.";
		}
		catch (const std::exception& e) {
			message = "Error occured in deleting file/folder.";
			std::cerr << "Error deleting file/folder: " << e.what() << std::endl;
		}
		sendMessage(message);
	}
	
	void info(const std::string& filename) {
		std::stringstream information;
		std::string message;

		try {
			fs::path filePath = filename;
			std::string lastMod = std::to_string(fs::last_write_time(filePath).time_since_epoch().count());

			if (fs::exists(filePath)) {
				information << "File/Directory Information for: " << filePath << "\n";
				information << "--------------------------------\n";
				information << "Size: " << fs::file_size(filePath) << " bytes\n";

				auto lastWriteTime = fs::last_write_time(filePath);
				auto lastWriteTimeTT = std::chrono::time_point_cast<std::chrono::system_clock::duration>(lastWriteTime - fs::file_time_type::clock::now() + std::chrono::system_clock::now());
				auto lastWriteTimeTP = std::chrono::system_clock::to_time_t(lastWriteTimeTT);

				std::tm lastWriteTimeTM;
				if (localtime_s(&lastWriteTimeTM, &lastWriteTimeTP) == 0) {
					char buffer[80];
					std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &lastWriteTimeTM);

					information << "Last Modified: " << buffer << "\n";
				}
				else {
					information << "Error converting time.\n";
				}				information << "Is Directory: " << (fs::is_directory(filePath) ? "Yes" : "No") << "\n";
				information << "Is Regular File: " << (fs::is_regular_file(filePath) ? "Yes" : "No") << "\n";
			}
			else {
				information << "Error: File or directory not found.";
			}
			message = information.str();
		}
		catch (const std::exception& e) {
			message = "Error occured in getting info.";
			std::cerr << "Error getting info about the file/folder: " << e.what() << std::endl;
		}
		sendMessage(message);
	}


};

