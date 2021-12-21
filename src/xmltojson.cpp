#include <iostream>
#include <vector>
#include <string>
#include <filesystem>

namespace fs = std::filesystem;

// variables
bool started = false;
bool stop = false;
std::string version = "0.1";
std::string commandPrefix = "-xmj ";
std::string commands[2] = {"--getBlocks", "--getLogic"};
std::string commandPaths[2] = {" (stormworksfolderpath)", " (stormworksfolderpath)"};


std::string defLoc = "\\rom\\data\\definitions";

std::string getInput() {
    std::string input = "";
    std::cin >> input;
    return input;
}

int help() {
    std::cout << "Availaible Commands:\n\n-xmj\n";
    for(int i = 0; i < sizeof(commands)/sizeof(commands[0]); i++) {
        std::cout << "  " << commands[i] <<"\n";
    }
    std::cout << "\nExample Usage: " << commandPrefix << commands[0] << "\n> ";
    return true;
}

int start() {
    // Startup Message
    std::cout << "Stormworks XML to Json Parser. Version " << version << ". \nCreated by Toastery.\n\n";
    help();
    return true;
}

int checkFile(bool correctPath) {
    int attempts = 1;
    while(correctPath == false) {
        std::string path;
        getline(std::cin, path);
        fs::path dir = path + defLoc;
        std::cout << "\nChecking if " << dir << " is a valid directory...\n";
        if(fs::exists(dir)) {
            correctPath = true;
            std::cout << "\nPath Valid!\n";
        } else {
            std::cout << "\nError: " << "Invalid Path" << "\n\n";
            if(0 >= attempts) {
                return false;
            } else {
                std::cout << attempts << " Attempts remaining...\n>";
                attempts--;
            }
        }
    }
    return true;
}

int main() {
    while(stop == false) {
        if(started == false) { // if never started, send the welcome message
            started = start();
        }
        std::string selectedCommand;
        getline(std::cin, selectedCommand);
        bool commandExists = false;
        for(int i = 0; i < sizeof(commands)/sizeof(commands[0]); i++) {
            if(selectedCommand == commandPrefix + commands[i]){
                std::cout << "\nEnter the file path of stormworks\nDefault: C:\\Program Files\\Steam\\steamapps\\common\\Stormworks" << "\n>";
                bool correctPath = checkFile(false);
                commandExists = true;
                if(correctPath == false) {
                    help();
                }
            }
        }
        if(commandExists == false) {
            std::cout << "\nCommand not found!\n";
            std::cout << commandPrefix + commands[0] << "\n" << selectedCommand;
            help();
        }
    }
    
}