#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <regex>
#include <fstream>


namespace fs = std::filesystem;

// variables
bool started = false;
bool stop = false;
std::string version = "0.1";
std::string commandPrefix = "-xmj ";
std::string commands[2] = {"--getBlocks", "--getLogic"};
std::string commandPaths[2] = {" (stormworksfolderpath)", " (stormworksfolderpath)"};
fs::path dir;


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

int readBlocks() {
    std::vector<std::string> names;
    std::vector<std::string> descs;
    std::vector<std::string> shortDescs;
    std::vector<std::string> masss;
    std::vector<std::string> costs;
    std::vector<std::string> inputss;
    std::vector<std::string> outputss;
    int blockNum = 0;
    for (const auto & entry : fs::directory_iterator(dir)) {
        std::string line;
        std::ifstream readxml;
        readxml.open(entry.path());
        using namespace std::string_literals;

        // name
        auto nameStart = "<definition name="s;
        auto nameEnd = " category="s;
        std::regex name(nameStart + "(.*)" + nameEnd);
        
        // desc
        auto descStart = "<tooltip_properties description="s;
        auto descEnd = " short_description="s;
        std::regex desc(descStart + "(.*)" + descEnd);

        // shortdesc
        auto shortDescStart = "short_description="s;
        auto shortDescEnd = "/>"s;
        std::regex shortDesc(shortDescStart + "(.*)" + shortDescEnd);

        // mass
        auto massStart = " mass="s;
        auto massEnd = " value="s;
        std::regex mass(massStart + "(.*)" + massEnd);

        // cost
        auto costStart = "value="s;
        auto costEnd = " flags="s;
        std::regex cost(costStart + "(.*)" + costEnd);

        // inputs
        auto inputsStart = "mode=\"1\" type=\""s;
        auto inputsEnd = "\" description=\""s;
        std::regex inputs(inputsStart + "(.*)" + inputsEnd);
        
        //outputs
        auto outputsStart = "mode=\"0\" type=\""s;
        auto outputsEnd = "\" description=\""s;
        std::regex outputs(outputsStart + "(.*)" + outputsEnd);


        std::cout << "reading block #" << blockNum <<"\n";
        inputss.push_back("");
        outputss.push_back("");
        bool i0 = false;
        bool i1 = false;
        bool i2 = false;
        bool i3 = false;
        bool i4 = false;
        bool i5 = false;
        bool i6 = false;
        bool i7 = false;
        bool o0 = false;
        bool o1 = false;
        bool o2 = false;
        bool o3 = false;
        bool o4 = false;
        bool o5 = false;
        bool o6 = false;
        bool o7 = false;
        if(readxml.is_open()){
            while(getline(readxml, line)) {
                std::smatch rawMatched;
                auto lineToRead = line + ""s;
                if(std::regex_search(lineToRead, rawMatched, name)) {
                    if(rawMatched.size() == 2) {
                        names.push_back(rawMatched[1].str());
                    }
                }
                if(std::regex_search(lineToRead, rawMatched, desc)) {
                    if(rawMatched.size() == 2) {
                        descs.push_back(rawMatched[1].str());
                    }
                }
                if(std::regex_search(lineToRead, rawMatched, shortDesc)) {
                    if(rawMatched.size() == 2) {
                        shortDescs.push_back(rawMatched[1].str());
                    }
                }
                if(std::regex_search(lineToRead, rawMatched, mass)) {
                    if(rawMatched.size() == 2) {
                        masss.push_back(rawMatched[1].str());
                    }
                }
                if(std::regex_search(lineToRead, rawMatched, cost)) {
                    if(rawMatched.size() == 2) {
                        costs.push_back(rawMatched[1].str());
                    }
                }
                if(std::regex_search(lineToRead, rawMatched, inputs)) {
                    if(rawMatched.size() == 2) {
                        if(!inputss[blockNum].empty()) {
                            inputss[blockNum] = inputss[blockNum] + ", ";
                        }
                        std::string logicType = "unknown";
                        if(rawMatched[1].str() == "0") {
                            logicType = "boolean";
                        } else if(rawMatched[1].str() == "1") {
                            if(i0 == false) {
                                logicType = "number";
                            } else {
                                logicType = "";
                            }
                        } else if(rawMatched[1].str() == "2") {
                            if(i1 == false) {
                                logicType = "power";
                            } else {
                                logicType = "";
                            }
                        } else if(rawMatched[1].str() == "3") {
                            if(i2 == false) {
                                logicType = "fluid";
                            } else {
                                logicType = "";
                            }
                        } else if(rawMatched[1].str() == "4") {
                            if(i3 == false) {
                                logicType = "electric";
                            } else {
                                logicType = "";
                            }
                        } else if(rawMatched[1].str() == "5") {
                            if(i4 == false) {
                                logicType = "composite";
                            } else {
                                logicType = "";
                            }
                        } else if(rawMatched[1].str() == "6") {
                            if(i5 == false) {
                                logicType = "video";
                            } else {
                                logicType = "";
                            }
                        } else if(rawMatched[1].str() == "7") {
                            if(i6 == false) {
                                logicType = "audio";
                            } else {
                                logicType = "";
                            }
                        } else if(rawMatched[1].str() == "8") {
                            if(i7 == false) {
                                logicType = "rope";
                            } else {
                                logicType = "";
                            }
                        } 
                        inputss[blockNum] = inputss[blockNum] + logicType;
                    }
                }
                if(std::regex_search(lineToRead, rawMatched, outputs)) {
                    if(rawMatched.size() == 2) {
                        if(!outputss[blockNum].empty()) {
                            outputss[blockNum] = outputss[blockNum] + ", ";
                        }
                        std::string logicType = "unknown";
                        if(rawMatched[1].str() == "0") {
                            logicType = "boolean";
                        } else if(rawMatched[1].str() == "1") {
                            if(i0 == false) {
                                logicType = "number";
                            } else {
                                logicType = "";
                            }
                        } else if(rawMatched[1].str() == "2") {
                            if(i1 == false) {
                                logicType = "power";
                            } else {
                                logicType = "";
                            }
                        } else if(rawMatched[1].str() == "3") {
                            if(i2 == false) {
                                logicType = "fluid";
                            } else {
                                logicType = "";
                            }
                        } else if(rawMatched[1].str() == "4") {
                            if(i3 == false) {
                                logicType = "electric";
                            } else {
                                logicType = "";
                            }
                        } else if(rawMatched[1].str() == "5") {
                            if(i4 == false) {
                                logicType = "composite";
                            } else {
                                logicType = "";
                            }
                        } else if(rawMatched[1].str() == "6") {
                            if(i5 == false) {
                                logicType = "video";
                            } else {
                                logicType = "";
                            }
                        } else if(rawMatched[1].str() == "7") {
                            if(i6 == false) {
                                logicType = "audio";
                            } else {
                                logicType = "";
                            }
                        } else if(rawMatched[1].str() == "8") {
                            if(i7 == false) {
                                logicType = "rope";
                            } else {
                                logicType = "";
                            }
                        } 
                        outputss[blockNum] = outputss[blockNum] + logicType;
                    }
                }
            }
        }
        blockNum++;
        readxml.close();
    }
    std::cout << "Attempting to remove previous output...";
    if( remove( "sw_blocks.json" ) != 0 ) {
        std::cout << "\nError deleting file\n";
    } else {
        std::cout << "\nFile successfully deleted\n";
    }
    std::ofstream writejson;
    writejson.open ("sw_blocks.json");
    if(writejson.is_open())
    {
        writejson << "[\n"; // opens file with [
        for(int i = 0; i < names.size(); i++) {
            if(!names[i].empty()) {
                std::cout << "writing block #" << i << "\n";
                if(i == 0) {
                    writejson << "    {\n";
                } else {
                    writejson << ",\n    {\n";
                }
                writejson << "        \"name\":" << names[i] << ",\n"; // writes name
                writejson << "        \"path\":\"public/assets/Blocks/\",\n"; // writes path
                writejson << "        \"desc\":" << descs[i] << ",\n"; // writes desc
                writejson << "        \"shortDesc\":" << shortDescs[i] << ",\n";
                writejson << "        \"mass\":" << masss[i] << ",\n";
                writejson << "        \"cost\":" << costs[i] << ",\n";
                writejson << "        \"inputs\":\"" << inputss[i] << "\",\n";
                writejson << "        \"outputs\":\"" << outputss[i] << "\"\n";
                writejson << "    }";
            }
        }
        writejson << "]"; // writes ] at the end of the file
        writejson.close();
        std::cout << "json finished writing!";
    } else {
        std::cout << "error creating and opening json file!";
        }
    return true;
}

int readLogic() {
    for (const auto & entry : fs::directory_iterator(dir)) {
        std::cout << entry.path() << std::endl;
    }
    return true;
}

int checkFile(bool correctPath) {
    int attempts = 1;
    while(correctPath == false) {
        std::string path;
        getline(std::cin, path);
        dir = path + defLoc;
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
                std::cout << "\nEnter the file path of stormworks\nDefault: C:\\Program Files\\Steam\\steamapps\\common\\Stormworks" << "\n> ";
                bool correctPath = checkFile(false);
                commandExists = true;
                if(correctPath == false) {
                    help();
                } else {
                    if(i == 0) { // block
                        readBlocks();
                    } else if(i == 1) { // logic
                        readLogic();
                    }
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