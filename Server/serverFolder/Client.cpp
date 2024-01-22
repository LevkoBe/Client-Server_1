#include <iostream>
#include <fstream>
#include "Communicator.h"

// search for 'setting'

enum CommandType {
    Exit = 0,
    GetFile,
    ListDirectory,
    PutFile,
    PutExistingFile,
    CreateEmptyFile,
    CreateEmptyDirectory,
    DeleteFileOrDirectory,
    GetFileInfo,
    AddToFile
};

std::string getFileContent(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "Sorry, can't open the file.\n";
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();

    return buffer.str();
}

int main() {
    Communicator communicator;

    std::string filename;
    std::string content;

    int command;
    while (true) {
        std::cout << "\n\n\nPlease enter a command to execute:\n"
            << "0: Exit\n"
            << "1: Get File\n"
            << "2: List Directory\n"
            << "3: Put File\n"
            << "4: Put existing File from local directory\n"
            << "5: Create Empty File\n"
            << "6: Create Empty Directory\n"
            << "7: Delete File or Directory\n"
            << "8: Get File Information\n"
            << "9: Add some content to existing file\n";

        std::cout << "\n\nEnter command: ";
        std::cin >> command;

        switch (command) {
        case Exit:
            std::cout << "Exiting...\n";
            communicator.stop();
            return 0;
        case GetFile:
            std::cout << "Enter filename to get: ";
            std::cin >> filename;
            std::cout << communicator.get(filename);
            break;
        case ListDirectory:
            std::cout << "Enter directory path to list (for default, enter '.'): ";
            std::cin >> filename;
            std::cout << communicator.list(filename);
            break;
        case PutFile:
            std::cout << "Enter filename to put: ";
            std::cin >> filename;
            std::cout << "Enter content for the file: ";
            std::cin.ignore(); // Clear the newline character from the buffer
            std::getline(std::cin, content);
            std::cout << communicator.put(filename, content);
            break;
        case PutExistingFile:
            std::cout << "Enter filename to put: ";
            std::cin >> filename;
            communicator.sendFile(filename);
            break;
        case CreateEmptyFile:
            std::cout << "Enter filename for the empty file: ";
            std::cin >> filename;
            std::cout << communicator.put(File, filename);
            break;
        case CreateEmptyDirectory:
            std::cout << "Enter directory name for the empty directory: ";
            std::cin >> filename;
            std::cout << communicator.put(Directory, filename);
            break;
        case DeleteFileOrDirectory:
            std::cout << "Enter filename or directory path to delete: ";
            std::cin >> filename;
            std::cout << communicator.deleteFile(filename);
            break;
        case GetFileInfo:
            std::cout << "Enter filename to get information: ";
            std::cin >> filename;
            std::cout << communicator.info(filename);
            break;
        case AddToFile:
            std::cout << "Enter filename to which you want to add content: ";
            std::cin >> filename;
            std::cout << "Enter content for the file: ";
            std::cin.ignore(); // Clear the newline character from the buffer
            std::getline(std::cin, content);
            std::cout << communicator.add(filename, content);
            break;
        default:
            std::cout << "Invalid command. Please try again.\n";
            break;
        }
    }

    return 0;
}
