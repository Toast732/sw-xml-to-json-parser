#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <regex>
#include <fstream>
#include <bitset>
#include <ctime>
#include <chrono>
#include <windows.h>


namespace fs = std::filesystem;

// variables
bool started = false;
bool stop = false;
std::string xmjVersion = "0.1.5";
std::string commandPrefix = "-xmj ";
std::string commands[2] = {"--getBlocks", "--getLogic"};
std::string commandPaths[2] = {" (stormworksfolderpath)", " (stormworksfolderpath)"};
fs::path dir;

int isHiddenFlag = 536870912;

std::string path;
std::string defLoc = "\\rom\\data\\definitions";
std::string verLoc = "\\";
std::string verFileName = "log.txt";

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
    std::cout << "Stormworks XML to Json Parser. Version " << xmjVersion << ". \nCreated by Toastery.\n\n";
    help();
    return true;
}

auto getTime() {
    return std::chrono::system_clock::now();
}

std::string getDate() {
    time_t rawTime;
    struct tm * timeInfo;
    char buffer [80];
    time(&rawTime);
    timeInfo = localtime(&rawTime);
    strftime(buffer, 80, "%Y-%m-%d %H-%M-%S", timeInfo);
    return buffer;
}

std::string getVersion() {
    using namespace std::string_literals;
    auto versionStart = "version: "s;
    auto versionEnd = ""s;
    std::regex versions(versionStart + "(.*)" + versionEnd);

    std::ifstream readVersion;
    readVersion.open(path + verLoc + verFileName);

    std::string line;
    std::string swVersion = "noversionfound";

    if(readVersion.is_open()) {
        while(getline(readVersion, line)) {
            std::smatch rawMatched;
            auto lineToRead = line + ""s;
            if(std::regex_search(lineToRead, rawMatched, versions)) { // gets "name" value 
                if(rawMatched.size() == 2) {
                    swVersion = rawMatched[1].str();
                }
            }
        }
    } else {
        std::cout << "Error opening " + path + verLoc + verFileName;
    }
    readVersion.close();
    return swVersion;
}

int parseBlocks() {
    // log
    auto totalStartTimer = getTime();
    std::vector<std::string> success; // used in the log to check if everything succeeded
    std::vector<std::string> errorReadingBlocksId; // stores which blocks had issues with reading its file by id
    std::vector<std::string> errorReadingBlocksFile; // stores which blocks had issues with reading its file by file

    // stats
    int totalBlocks = 0; // the total blocks - num blacklisted - num deprecated - num hidden
    int totalHidden = 0; // the total blocks - num blacklisted - num deprecated - num visible
    int totalDeprecated = 0; // the total blocks - num blacklisted - num not deprecated

    // actual script continues here
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
    std::vector<unsigned> flags;
    std::vector<std::string> isHiddens;

    std::vector<std::string> blacklistedPaths;
    std::vector<std::string> blacklistedBlocks;

    std::string hiddenItemSuffix[] = {"_b", "_head", " child", "_b_fluid", " b", "_b_round"};
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

    auto flagStart = "\" flags=\""s;
    auto flagEnd = "\" tags="s;
    std::regex flag(flagStart + "(.*)" + flagEnd);

    // blacklist
    auto blacklistStart = "\"fileName\":\""s;
    auto blastlistEnd = "\""s;
    std::regex blacklisted(blacklistStart + "(.*)" + blastlistEnd);
    std::ifstream readConfig;
    std::string line;
    std::cout << "current directory: " << std::filesystem::current_path();


    // starts reading config for the blacklist
    auto readBlacklistTimerStart = getTime();
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
        success.push_back("Reading Blacklist: Ok");
    } else {
        success.push_back("Reading Blacklist: Error");
        std::cout << "error, blockBlacklist did not open!";
        return false;
    }
    readConfig.close();
    auto readBlacklistTimerEnd = getTime();


    // reads each block file, and with their id it assignes if its blacklisted or not
    auto setBlacklistTimerStart = getTime();
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
    auto setBlacklistTimerEnd = getTime();

    // reads all block files
    auto readAllTimerStart = getTime();
    blacklistNum = 0;
    for (const auto & entry : fs::directory_iterator(dir)) {
        if(blacklistedBlocks[blacklistNum] == "false") { // if the block is not blacklisted
            std::string line;
            std::ifstream readxml;
            readxml.open(entry.path());
            
            std::cout << "reading block #" << blockNum;
            inputss.push_back("");
            outputss.push_back("");
            flags.push_back(0);
            isHiddens.push_back("true");

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
                        totalDeprecated++;
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
                    if(std::regex_search(lineToRead, rawMatched, flag)) { // gets "flags" value 
                        if(rawMatched.size() == 2) {
                            flags[blockNum] = std::stoi(rawMatched[1].str());
                            // if the block is hidden in the inventory
                            if ((flags[blockNum] & isHiddenFlag) == false) { // if the is hidden flag is false
                                // checks if the file name contains a suffix that is related to being a 2nd part of a block, and is hidden, if it goes through all of the suffix without a hit, it will be marked as not hidden
                                int hiddenSuffixCheck = 0;
                                for(int bID = 0; bID <= sizeof(hiddenItemSuffix)/sizeof(*hiddenItemSuffix); bID++) {
                                    if(fileNames[blockNum].substr(fileNames[blockNum].find_last_of(".") - hiddenItemSuffix[bID].length()) != hiddenItemSuffix[bID] + ".xml") {
                                        hiddenSuffixCheck++;
                                    }
                                }
                                if( hiddenSuffixCheck == sizeof(hiddenItemSuffix)/sizeof(*hiddenItemSuffix)) {
                                    isHiddens[blockNum] = "false";
                                }
                            }
                            
                        }
                    }
                }
            } else {
                errorReadingBlocksId.push_back("blockNum");
                errorReadingBlocksFile.push_back(base_name(entry.path().string()));
            }
            blockNum++;
            readxml.close();
        }
        blacklistNum++;
    }
    if(errorReadingBlocksId.size() >= 1) {
        success.push_back("Reading Blocks: Error");
    } else {
        success.push_back("Reading Blocks: Ok");
    }
    auto readAllTimerEnd = getTime();

    // validates variables for mistakes
    auto validateTimerStart = getTime();
    blockNum = 0;
    blacklistNum = 0;
    for (const auto & entry : fs::directory_iterator(dir)) {
        if(blacklistedBlocks[blacklistNum] == "false") { // if the block is not blacklisted
            std::cout << "\nValidating block #" << blockNum << " File Name: " << base_name(entry.path().string()) << " Status: ";
            if(isHiddens[blockNum] == "true" && flags[blockNum] == 0) { // checks if it mistakenly said it was hidden in the inventory, when it isn't
                isHiddens[blockNum] = "false";
                std::cout << "fixed";
            } else {
                std::cout << "ok";
            }
            if(isHiddens[blockNum] == "true") {
                if(deprecateds[blockNum] == "false") {
                    totalHidden++;
                }
            }
            blockNum++;
        }
        blacklistNum++;
    }
    totalBlocks = blockNum - totalDeprecated - totalHidden;
    auto validateTimerEnd = getTime();
    auto removeTimerStart = getTime();
    std::cout << "\nAttempting to remove previous output...";
    if( remove( "sw_blocks.json" ) != 0 ) {
        std::cout << "\nError deleting file\n";
    } else {
        std::cout << "\nFile successfully deleted\n";
    }
    auto removeTimerEnd = getTime();
    auto writeJsonTimerStart = getTime();
    std::ofstream writejson;
    writejson.open ("sw_blocks.json");
    std::cout << "\ncurrent dir:" << dir << "\n";
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
                writejson << "        \"deprecated\":" << deprecateds[i] << ",\n"; // writes if its deprecated or not
                writejson << "        \"isHidden\":" << isHiddens[i] << "\n"; // writes if the item is hidden in the inventory
                //writejson << "        \"flags\":\"" << std::bitset<32>(flags[i]) << "\",\n"; //writes the flags in bitwise form - debugging
                writejson << "    }"; // end of block data
            }
        }
        writejson << "]"; // writes ] at the end of the file
        writejson.close();
        std::cout << "\nFinished writing sw_blocks.json\n";
        success.push_back("Writing Json: Ok");
    } else {
        std::cout << "\nerror creating and opening json file!\n";
        success.push_back("Writing Json: Error");
    }
    auto writeJsonTimerEnd = getTime();
    // output log
    auto totalEndTimer = getTime();
    std::string timeStamp = getDate();
    std::chrono::duration<double> totalElapsedTime = totalEndTimer-totalStartTimer;
    std::cout << "\nTotal Time Elapsed: " << totalElapsedTime.count() << "s\n";
    std::cout << "Date: " << timeStamp << "\n";

    std::chrono::duration<double> readBlacklistTimer = readBlacklistTimerEnd-readBlacklistTimerStart;
    std::chrono::duration<double> setBlacklistTimer = setBlacklistTimerEnd-setBlacklistTimerStart;
    std::chrono::duration<double> readAllTimer = readAllTimerEnd-readAllTimerStart;
    std::chrono::duration<double> validateTimer = validateTimerEnd-validateTimerStart;
    std::chrono::duration<double> writeJsonTimer = writeJsonTimerEnd-writeJsonTimerStart;

    std::string gameVersion = getVersion();
    std::error_code errCode;
    fs::path logDir = "logs/" + gameVersion + "/blocks/" + timeStamp;
    bool logDirWasCreated = fs::create_directories(logDir, errCode);
    if(logDirWasCreated) {
        std::cout << "\nLog directory made successfully\n\n";
    }
    std::ofstream writeLog;
    writeLog.open("logs/" + gameVersion + "/blocks/" + timeStamp + "/debuglog.txt");
    if(writeLog.is_open()) {
        writeLog << "Game Version: " << gameVersion << "\n"; // writes the game's version
        writeLog << "XMJ version: " << xmjVersion << "\n\n"; // writes this program's version
        for(int i = 0; i < success.size(); i++) {
            if(!success[i].empty()) {
                writeLog << success[i] << "\n";
            } else {
                std::cout << "ERROR: apparently " << success[i] << " is empty!\n";
            }
        }
        writeLog << "\nTotal Elapsed Time: " << totalElapsedTime << "\n";
        writeLog << "Reading Blacklist Time Taken: " << readBlacklistTimer << "\n";
        writeLog << "Setting Blacklist Time Taken: " << setBlacklistTimer << "\n";
        writeLog << "Read All Blocks Time Taken: " << readAllTimer << "\n";
        writeLog << "Validate Blocks Time Taken: " << validateTimer << "\n";
        writeLog << "Write Json Time Taken: " << writeJsonTimer << "\n\n";
        writeLog << "Total Amount of Blocks: " << totalBlocks << "\n";
        writeLog << "Total Hidden Blocks: " << totalHidden << "\n";
        writeLog << "Total Deprecated Blocks: " << totalDeprecated << "\n";
    } else {
        std::cout << "\nError from create dir: " << errCode.message() << " 1 = created, 0 = not created: " << logDirWasCreated << " \n";
        std::cout << "\nError opening logs\\" << gameVersion << "\\blocks\\" << timeStamp << "\\debuglog.txt\n";
    }
    writeLog.close();

    return start();
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