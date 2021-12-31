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
std::string version = "0.1.3";
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

std::string base_name(std::string const & path) { // gets file name, without the path
  return path.substr(path.find_last_of("/\\") + 1);
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
    std::vector<std::string> fileNames;
    std::vector<std::string> descs;
    std::vector<std::string> shortDescs;
    std::vector<std::string> inputss;
    std::vector<std::string> outputss;
    std::vector<std::string> categories;
    std::vector<std::string> masss;
    std::vector<std::string> costs;
    std::vector<std::string> deprecateds;

    std::vector<std::string> blacklistedPaths;
    std::vector<std::string> blacklistedBlocks;
    int blockNum = 0;
    
    using namespace std::string_literals;

    // regex
    // name
    auto nameStart = "<definition name="s;
    auto nameEnd = " category="s;
    std::regex name(nameStart + "(.*)" + nameEnd);

    // category
    auto categoryStart = " category=\""s;
    auto categoryEnd = "\" type="s;
    std::regex category(categoryStart + "(.*)" + categoryEnd);
    
    // desc
    auto descStart = "<tooltip_properties description="s;
    auto descEnd = " short_description="s;
    std::regex desc(descStart + "(.*)" + descEnd);

    // shortdesc
    auto shortDescStart = "short_description="s;
    auto shortDescEnd = "/>"s;
    std::regex shortDesc(shortDescStart + "(.*)" + shortDescEnd);

    // mass
    auto massStart = " mass=\""s;
    auto massEnd = "\" value="s;
    std::regex mass(massStart + "(.*)" + massEnd);

    // cost
    auto costStart = "value=\""s;
    auto costEnd = "\" flags="s;
    std::regex cost(costStart + "(.*)" + costEnd);

    // inputs
    auto inputsStart = "mode=\"1\" type=\""s;
    auto inputsEnd = "\" description=\""s;
    std::regex inputs(inputsStart + "(.*)" + inputsEnd);
    
    //outputs
    auto outputsStart = "mode=\"0\" type=\""s;
    auto outputsEnd = "\" description=\""s;
    std::regex outputs(outputsStart + "(.*)" + outputsEnd);

    //deprecated
    std::regex deprecated("Deprecated");

    // blacklist
    auto blacklistStart = "\"fileName\":\""s;
    auto blastlistEnd = "\""s;
    std::regex blacklisted(blacklistStart + "(.*)" + blastlistEnd);
    std::ifstream readConfig;
    std::string line;
    std::cout << "current directory: " << std::filesystem::current_path();


    // starts reading config for the blacklist
    readConfig.open("..\\src\\blockBlacklist.json"); 
    if(readConfig.is_open()){
        while(getline(readConfig, line)) {
            std::smatch rawMatched;
            auto lineToRead = line + ""s;
            if(std::regex_search(lineToRead, rawMatched, blacklisted)) {
                if(rawMatched.size() == 2) {
                    blacklistedPaths.push_back(rawMatched[1].str());
                }
            }
        }
    } else {
        std::cout << "error, blockBlacklist did not open!";
        return false;
    }
    readConfig.close();


    // reads each block file, and with their id it assignes if its blacklisted or not
    int blacklistNum = 0;
    for (const auto & entry : fs::directory_iterator(dir)) { 
        int blacklistedChecks = 0;
        std::cout << "Checking if block #" << blacklistNum << " is Blacklisted... ( ";
        for(int i = 0; i < blacklistedPaths.size(); i++) {
            std::smatch rawMatched;
            std::string fileName{entry.path().string()};
            std::regex blacklistedToCheck(blacklistedPaths[i]);
            if(std::regex_search(fileName, rawMatched, blacklistedToCheck)) {
                blacklistedChecks++;
            }
        }
        if(blacklistedChecks > 0) { // if it is blacklised
            blacklistedBlocks.push_back("true"); // is blacklisted
            std::cout << "true )\n";
        } else { // if its not
            blacklistedBlocks.push_back("false"); // isn't blacklisted
            std::cout << "false )\n";
        }
        blacklistNum++;
    }


    // reads all block files
    blacklistNum = 0; 
    for (const auto & entry : fs::directory_iterator(dir)) {
        if(blacklistedBlocks[blacklistNum] == "false") { // if the block is not blacklisted
            std::string line;
            std::ifstream readxml;
            readxml.open(entry.path());
            
            std::cout << "reading block #" << blockNum;
            inputss.push_back("");
            outputss.push_back("");
            deprecateds.push_back("false");
            int in[8];
            int out[8];
            for(int i = 0; i < 8; i++) {
                in[i] = false;
                out[i] = false;
            }
            std::cout << " File Name: " << base_name(entry.path().string()) << "\n";
            fileNames.push_back("Stormworks/rom/data/definitions/" + base_name(entry.path().string()));
            if(readxml.is_open()){
                while(getline(readxml, line)) {
                    std::smatch rawMatched;
                    auto lineToRead = line + ""s;
                    if(std::regex_search(lineToRead, rawMatched, name)) { // gets "name" value 
                        if(rawMatched.size() == 2) {
                            names.push_back(rawMatched[1].str());
                        }
                    }
                    if(std::regex_search(lineToRead, rawMatched, deprecated)) { // checks if its deprecated or not, via the name of the item, checks for "(Deprecated)" in the name
                        deprecateds[blockNum] = "true";
                    }
                    else if(std::regex_search(lineToRead, rawMatched, desc)) { // gets "description" value 
                        if(rawMatched.size() == 2) {
                            descs.push_back(rawMatched[1].str());
                        }
                    }
                    if(std::regex_search(lineToRead, rawMatched, shortDesc)) { // gets "short_description" value
                        if(rawMatched.size() == 2) {
                            shortDescs.push_back(rawMatched[1].str());
                        }
                    }
                    else if(std::regex_search(lineToRead, rawMatched, inputs)) { // gets all inputs, logic and physical, only shows one of each type
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
                    else if(std::regex_search(lineToRead, rawMatched, outputs)) { // gets all outputs, logic and physical, only shows one of each type
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
                    if(std::regex_search(lineToRead, rawMatched, category)) { // gets "category" value 
                        if(rawMatched.size() == 2) {
                            categories.push_back(rawMatched[1].str());
                        }
                    }
                    if(std::regex_search(lineToRead, rawMatched, mass)) { // gets "mass" value 
                        if(rawMatched.size() == 2) {
                            masss.push_back(rawMatched[1].str());
                        }
                    }
                    if(std::regex_search(lineToRead, rawMatched, cost)) { // gets "value" value 
                        if(rawMatched.size() == 2) {
                            costs.push_back(rawMatched[1].str());
                        }
                    }
                }
            }
            blockNum++;
            readxml.close();
        }
        blacklistNum++;
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
                std::cout << "writing block #" << i << " Name: " << names[i] <<"\n";
                if(i == 0) {
                    writejson << "    {\n";
                } else {
                    writejson << ",\n    {\n";
                }
                writejson << "        \"name\":" << names[i] << ",\n"; // writes name
                writejson << "        \"fileName\":\"" << fileNames[i] << "\",\n"; // writes file Name
                writejson << "        \"path\":\"public/assets/Blocks/\",\n"; // writes path
                writejson << "        \"desc\":" << descs[i] << ",\n"; // writes desc
                writejson << "        \"shortDesc\":" << shortDescs[i] << ",\n"; // writes shortDesc
                writejson << "        \"inputs\":\"" << inputss[i] << "\",\n"; // writes inputs
                writejson << "        \"outputs\":\"" << outputss[i] << "\",\n"; // writes outputs
                writejson << "        \"category\":" << categories[i] << ",\n"; // writes category
                writejson << "        \"mass\":" << masss[i] << ",\n"; // writes mass
                writejson << "        \"cost\":" << costs[i] << ",\n"; // writes cost
                writejson << "        \"deprecated\":" << deprecateds[i] << "\n"; // writes if its deprecated or not
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
                std::cout << attempts << " Attempts remaining...\n> ";
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