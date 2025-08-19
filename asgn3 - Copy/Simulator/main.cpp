#include "ComparativeSimulator.h"
#include "CompetitiveSimulator.h"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <string>
#include <algorithm>

namespace fs = std::filesystem;

void printUsage(const std::string& error = "") {
    std::cerr << "\nUsage:\n"
              << "Comparative: ./simulator_<submitter_ids> -comparative game_map=<game_map_filename> game_managers_folder=<game_managers_folder> algorithm1=<algorithm_so_filename> algorithm2=<algorithm_so_filename> [num_threads=<num>] [-verbose]\n"
              << "Competition: ./simulator_<submitter_ids> -competition game_maps_folder=<game_maps_folder> game_manager=<game_manager_so_filename> algorithms_folder=<algorithms_folder> [num_threads=<num>] [-verbose]\n";
    if (!error.empty()) std::cerr << "Error: " << error << std::endl;
}

bool fileExists(const std::string& path) {
    std::ifstream f(path);
    return f.good();
}

bool folderHasFiles(const std::string& path) {
    if (!fs::is_directory(path)) return false;
    for (const auto& entry : fs::directory_iterator(path))
        if (entry.is_regular_file()) return true;
    return false;
}

//std::unordered_map<std::string, std::string> parseArgs(int argc, char* argv[], std::vector<std::string>& flags, std::string& unrecognized) {
//    std::unordered_map<std::string, std::string> argMap;
//    for (int i = 1; i < argc; ++i) {
//        std::string s = argv[i];
//        s.erase(0, s.find_first_not_of(' ')); // Trim leading
//        s.erase(s.find_last_not_of(' ') + 1); // Trim trailing
//
//        if (s[0] == '-') {
//            flags.push_back(s);
//        } else {
//            auto pos = s.find('=');
//            if (pos == std::string::npos) {
//                unrecognized = s;
//                continue;
//            }
//            auto key = s.substr(0, pos);
//            auto val = s.substr(pos+1);
//            // Trim spaces around '='
//            key.erase(key.find_last_not_of(' ')+1);
//            key.erase(0, key.find_first_not_of(' '));
//            val.erase(val.find_last_not_of(' ')+1);
//            val.erase(0, val.find_first_not_of(' '));
//            argMap[key] = val;
//        }
//    }
//    return argMap;
//}


std::unordered_map<std::string, std::string>
parseArgs(int argc, char* argv[], std::vector<std::string>& flags, std::string& unrecognized) {
    auto trim = [](std::string& t) {
        auto l = t.find_first_not_of(' ');
        auto r = t.find_last_not_of(' ');
        if (l == std::string::npos) { t.clear(); return; }
        t = t.substr(l, r - l + 1);
    };

    std::unordered_map<std::string, std::string> argMap;
    std::string pendingKey;
    bool waitingForValueAfterEquals = false;

    for (int i = 1; i < argc; ++i) {
        std::string s = argv[i];
        trim(s);
        if (s.empty()) continue;

        if (s[0] == '-') {  // flag
            flags.push_back(s);
            continue;
        }

        if (!pendingKey.empty()) {
            // We had "key" (maybe saw "=" already). Consume '=' or value.
            if (waitingForValueAfterEquals) {
                // We expect the value now.
                trim(s);
                if (s == "=") { unrecognized = "="; break; }
                argMap[pendingKey] = s;
                pendingKey.clear();
                waitingForValueAfterEquals = false;
                continue;
            } else if (s == "=") {
                // Pattern: key = ...
                waitingForValueAfterEquals = true;
                continue;
            } else {
                // Pattern: key value   (not supported) -> error
                unrecognized = s;
                break;
            }
        }

        // No pending key. Try forms with '=' in the same token.
        auto pos = s.find('=');
        if (pos != std::string::npos) {
            std::string key = s.substr(0, pos);
            std::string val = s.substr(pos + 1);
            trim(key); trim(val);
            if (key.empty()) { unrecognized = s; break; }

            if (!val.empty()) {
                argMap[key] = val;
            } else {
                // key=  value could be next token
                waitingForValueAfterEquals = true;
                pendingKey = key;
            }
            continue;
        }

        // Maybe it’s just a bare key and next token is '='.
        pendingKey = s;
    }

    // If we ended while still expecting a value
    if (!unrecognized.empty()) return argMap;
    if (!pendingKey.empty()) {
        if (waitingForValueAfterEquals) {
            unrecognized = pendingKey + "=<missing>";
        } else {
            unrecognized = pendingKey; // bare key with no '='
        }
    }

    return argMap;
}


int main(int argc, char* argv[]) {
    try {
        if (argc < 2) {
            printUsage("No mode specified.");
            return 1;
        }

        std::vector<std::string> flags;
        std::string unrecognized;
        auto argMap = parseArgs(argc, argv, flags, unrecognized);
        if (!unrecognized.empty()){
            printUsage("Unrecognized argument: " + unrecognized);
            return 1;
        }
        bool isComparative = false, isCompetition = false;
        for (const auto& f : flags) {
            if (f == "-comparative") isComparative = true;
            if (f == "-competition") isCompetition = true;
        }
        if (isComparative == isCompetition) { // must be exactly one mode
            printUsage("You must specify exactly one mode: -comparative or -competition");
            return 1;
        }

        // Define required keys for each mode
        std::vector<std::string> requiredKeys, folderKeys, fileKeys;
        if (isComparative) {
            requiredKeys = {"game_map", "game_managers_folder", "algorithm1", "algorithm2"};
            fileKeys = {"game_map", "algorithm1", "algorithm2"};
            folderKeys = {"game_managers_folder"};
        } else { // competition
            requiredKeys = {"game_maps_folder", "game_manager", "algorithms_folder"};
            folderKeys = {"game_maps_folder", "algorithms_folder"};
            fileKeys = {"game_manager"};
        }
        // num_threads is optional
        // -verbose is optional flag

        // Check for unsupported keys
        for (const auto& pair : argMap) {
            if (std::find(requiredKeys.begin(), requiredKeys.end(), pair.first) == requiredKeys.end()
                && pair.first != "num_threads") {
                printUsage("Unsupported command line argument: " + pair.first);
                return 1;
            }
        }

        std::vector<std::string> allowedFlags = isComparative
                                                ? std::vector<std::string>{"-comparative", "-verbose"}
                                                : std::vector<std::string>{"-competition", "-verbose"};

        for (const auto& flag : flags) {
            if (std::find(allowedFlags.begin(), allowedFlags.end(), flag) == allowedFlags.end()) {
                printUsage("Unsupported command line flag: " + flag);
                return 1;
            }
        }

        // Check for missing arguments
        for (const auto& key : requiredKeys) {
            if (argMap.find(key) == argMap.end()) {
                printUsage("Missing required argument: " + key);
                return 1;
            }
        }

        // Check files exist
        for (const auto& key : fileKeys) {
            if (!fileExists(argMap[key])) {
                printUsage("File argument '" + key + "' does not exist or cannot be opened (" + argMap[key] + ")");
                return 1;
            }
        }
        // Check folders exist and have at least one file
        for (const auto& key : folderKeys) {
            if (!fs::exists(argMap[key]) || !fs::is_directory(argMap[key]) || !folderHasFiles(argMap[key])) {
                printUsage("Folder argument '" + key + "' does not exist, cannot be traversed, or contains no valid files (" + argMap[key] + ")");
                return 1;
            }
        }

        // Optional arguments
        int num_threads = 1;
        if (argMap.count("num_threads")) {
            try {
                num_threads = std::stoi(argMap["num_threads"]);
            } catch (...) {
                printUsage("num_threads argument is not a valid integer");
                return 1;
            }
        }
        bool verbose = std::find(flags.begin(), flags.end(), "-verbose") != flags.end();



        if (isComparative) {
            // Extract the arguments for clarity
            const std::string& gameMap = argMap["game_map"];
            const std::string& gameManagersFolder = argMap["game_managers_folder"];
            const std::string& alg1 = argMap["algorithm1"];
            const std::string& alg2 = argMap["algorithm2"];

            ComparativeSimulator sim(gameMap, gameManagersFolder, alg1, alg2, verbose, num_threads);
            sim.simulate();

        } else { // isCompetition
            const std::string& gameMapsFolder = argMap["game_maps_folder"];
            const std::string& algorithmsFolder = argMap["algorithms_folder"];
            const std::string& gameManagerFile = argMap["game_manager"];

            CompetitiveSimulator sim(gameMapsFolder, gameManagerFile, algorithmsFolder, verbose, num_threads);
            sim.simulate();
        }
    }
    catch (const Simulator::MissingFilesException& ex) {
        std::cerr << "Error: " << ex.message << std::endl;
        return 1;
    }
    catch (const std::exception& ex) {
        std::cerr << "Unhandled exception: " << ex.what() << std::endl;
        return 2;

    } catch (...) {
        std::cerr << "Unknown error occurred." << std::endl;
        return 3;
    }
    return 0;
}
