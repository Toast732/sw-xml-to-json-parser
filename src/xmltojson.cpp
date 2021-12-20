#include <iostream>
#include <vector>
#include <string>

// variables
bool started = false;
bool stop = false;
std::string version = "0.1";
std::string commandPrefix = "-xmj ";
std::string commands[2] = {"--getBlocks", "--getLogic"};
std::string commandPaths[2] = {" (stormworksfolderpath)", " (stormworksfolderpath)"};

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

int main() {
    while(stop == false) {
        if(started == false) { // if never started, send the welcome message
            started = start();
        }

        std::string selectedCommand = getInput();
        bool commandExists = false;
        for(int i = 0; i < sizeof(commands)/sizeof(commands[0]); i++) {
            if(selectedCommand == commandPrefix + commands[i]){
                //std::cout << "\nenter the file path of stormworks\ndefault: C:\\Program Files\\Steam\\steamapps\\common\\Stormworks";
                commandExists = true;
            }
        }
        if(commandExists == false) {
            std::cout << "\nCommand not found!\n";
            help();
        }
    }
    
}