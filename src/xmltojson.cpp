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

int parseBlocks() {
    std::vector<std::string> names;
    std::vector<std::string> descs;
    std::vector<std::string> shortDescs;
    std::vector<std::string> masss;
    std::vector<std::string> costs;
    std::vector<std::string> inputss;
    std::vector<std::string> outputss;
    int blockNum = 0;
    
    using namespace std::string_literals;

    // regex
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

    for (const auto & entry : fs::directory_iterator(dir)) {
        std::string line;
        std::ifstream readxml;
        readxml.open(entry.path());
        
        std::cout << "reading block #" << blockNum <<"\n";
        inputss.push_back("");
        outputss.push_back("");
        int in[8];
        int out[8];
        for(int i = 0; i < 8; i++) {
            in[i] = false;
            out[i] = false;
        }
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
                        std::string logicType = "unknown";
                        if(rawMatched[1].str() == "0") {
                            if(in[0] == false) {
                                logicType = "boolean";
                                in[0] = true;
                            } else {
                                logicType = "";
                            }
                        } else if(rawMatched[1].str() == "1") {
                            if(in[1] == false) {
                                logicType = "number";
                                in[1] = true;
                            } else {
                                logicType = "";
                            }
                        } else if(rawMatched[1].str() == "2") {
                            if(in[2] == false) {
                                logicType = "power";
                                in[2] = true;
                            } else {
                                logicType = "";
                            }
                        } else if(rawMatched[1].str() == "3") {
                            if(in[3] == false) {
                                logicType = "fluid";
                                in[3] = true;
                            } else {
                                logicType = "";
                            }
                        } else if(rawMatched[1].str() == "4") {
                            if(in[4] == false) {
                                logicType = "electric";
                                in[4] = true;
                            } else {
                                logicType = "";
                            }
                        } else if(rawMatched[1].str() == "5") {
                            if(in[5] == false) {
                                logicType = "composite";
                                in[5] = true;
                            } else {
                                logicType = "";
                            }
                        } else if(rawMatched[1].str() == "6") {
                            if(in[6] == false) {
                                logicType = "video";
                                in[6] = true;
                            } else {
                                logicType = "";
                            }
                        } else if(rawMatched[1].str() == "7") {
                            if(in[7] == false) {
                                logicType = "audio";
                                in[7] = true;
                            } else {
                                logicType = "";
                            }
                        } else if(rawMatched[1].str() == "8") {
                            if(in[8] == false) {
                                logicType = "rope";
                                in[8] = true;
                            } else {
                                logicType = "";
                            }
                        } 
                        
                        if(logicType.compare("")) {
                            if(!inputss[blockNum].empty()) {
                                inputss[blockNum] = inputss[blockNum] + ", ";
                            }
                            inputss[blockNum] = inputss[blockNum] + logicType;
                        }
                    }
                }
                if(std::regex_search(lineToRead, rawMatched, outputs)) {
                    if(rawMatched.size() == 2) {
                        std::string logicType = "unknown";
                        if(rawMatched[1].str() == "0") {
                            if(out[0] == false) {
                                logicType = "boolean";
                                out[0] = true;
                            } else {
                                logicType = "";
                            }
                        } else if(rawMatched[1].str() == "1") {
                            if(out[1] == false) {
                                logicType = "number";
                                out[1] = true;
                            } else {
                                logicType = "";
                            }
                        } else if(rawMatched[1].str() == "2") {
                            if(out[2] == false) {
                                logicType = "power";
                                out[2] = true;
                            } else {
                                logicType = "";
                            }
                        } else if(rawMatched[1].str() == "3") {
                            if(out[3] == false) {
                                logicType = "fluid";
                                out[3] = true;
                            } else {
                                logicType = "";
                            }
                        } else if(rawMatched[1].str() == "4") {
                            if(out[4] == false) {
                                logicType = "electric";
                                out[4] = true;
                            } else {
                                logicType = "";
                            }
                        } else if(rawMatched[1].str() == "5") {
                            if(out[5] == false) {
                                logicType = "composite";
                                out[5] = true;
                            } else {
                                logicType = "";
                            }
                        } else if(rawMatched[1].str() == "6") {
                            if(out[6] == false) {
                                logicType = "video";
                                out[6] = true;
                            } else {
                                logicType = "";
                            }
                        } else if(rawMatched[1].str() == "7") {
                            if(out[7] == false) {
                                logicType = "audio";
                                out[7] = true;
                            } else {
                                logicType = "";
                            }
                        } else if(rawMatched[1].str() == "8") {
                            if(out[8] == false) {
                                logicType = "rope";
                                out[8] = true;
                            } else {
                                logicType = "";
                            }
                        }
                        if(logicType.compare("")) {
                            if(!outputss[blockNum].empty()) {
                                outputss[blockNum] = outputss[blockNum] + ", ";
                            }
                            outputss[blockNum] = outputss[blockNum] + logicType;
                        }
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
                writejson << "        \"shortDesc\":" << shortDescs[i] << ",\n"; // writes shortDesc
                writejson << "        \"mass\":" << masss[i] << ",\n"; // writes mass
                writejson << "        \"cost\":" << costs[i] << ",\n"; // writes cost
                writejson << "        \"inputs\":\"" << inputss[i] << "\",\n"; // writes inputs
                writejson << "        \"outputs\":\"" << outputss[i] << "\"\n"; // writes outputs
                writejson << "    }"; // end of block data
            }
        }
        writejson << "]"; // writes ] at the end of the file
        writejson.close();
        std::cout << "Finished Writing Json!";
    } else {
        std::cout << "error creating and opening json file!";
        }
    return true;
}

int parseLogic() {
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
                        parseBlocks();
                    } else if(i == 1) { // logic
                        parseLogic();
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