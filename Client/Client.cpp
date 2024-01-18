#include <iostream>
#include "Communicator.h"

// search for 'setting'

enum CommandType {
    Exit = 0,
    GetFile,
    ListDirectory,
    PutFile,
    CreateEmptyFile,
    CreateEmptyDirectory,
    DeleteFileOrDirectory,
    GetFileInfo
};

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
            << "4: Create Empty File\n"
            << "5: Create Empty Directory\n"
            << "6: Delete File or Directory\n"
            << "7: Get File Information\n";

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
        default:
            std::cout << "Invalid command. Please try again.\n";
            break;
        }
    }

    return 0;
}
