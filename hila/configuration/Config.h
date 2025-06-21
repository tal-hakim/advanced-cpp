#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <unordered_map>
#include <fstream>
#include <iostream>
#include <sstream>

 /* We used ChatGpt to learn how to work with configuraiton file*/

class Config {
private:
    std::unordered_map<std::string, std::string> settings; // Stores key-value pairs from the configuration file
    
    Config();  // Private constructor to enforce singleton pattern
    void parseLine(const std::string& line); // Parses a single line of "key=value" format and stores it in settings
    
public:
    static Config& getInstance();  // Returns the singleton instance of Config

    ~Config() = default;  // Default destructor
    Config(const Config&) = delete; // Disable copy constructor
    Config& operator=(const Config&) = delete;  // Disable copy assignment operator
    Config(Config&&) = delete; // Disable move constructor
    Config& operator=(Config&&) = delete; // Disable move assignment operator

    bool loadFromFile(const std::string& filename); // Loads configuration from a file, returns true if successful
    std::string get(const std::string& key, const std::string& default_val = "") const; // Returns the string value for a given key, or default_val if not found
    int getInt(const std::string& key, int default_val = 0) const; // Returns the integer value for a given key, or default_val if not found or conversion fails
    bool getBool(const std::string& key, bool default_val = false) const;  // Returns the boolean value for a given key ("true" or "1" as true), or default_val if not found 
};

#endif // CONFIG_H