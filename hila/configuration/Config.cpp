#include "Config.h"

Config::Config() {}

Config &Config::getInstance()
{
    static Config instance;
    return instance;
}

void Config::parseLine(const std::string &line)
{
    size_t pos = line.find('=');
    if (pos == std::string::npos)
        return;

    std::string key = line.substr(0, pos);
    std::string value = line.substr(pos + 1);

    settings[key] = value;
}

bool Config::loadFromFile(const std::string &filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Cannot open config file: " << filename << std::endl;
        return false;
    }

    try
    {
        std::string line;
        while (std::getline(file, line))
        {
            parseLine(line);
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error reading config file: " << e.what() << std::endl;
        file.close();
        return false;
    }

    file.close();
    return true;
}

std::string Config::get(const std::string &key, const std::string &default_val) const
{
    auto it = settings.find(key);
    if (it != settings.end())
    {
        return it->second;
    }
    return default_val;
}

int Config::getInt(const std::string &key, int default_val) const
{
    auto it = settings.find(key);
    if (it != settings.end())
    {
        try
        {
            return std::stoi(it->second);
        }
        catch (...)
        {
            return default_val;
        }
    }
    return default_val;
}

bool Config::getBool(const std::string &key, bool default_val) const
{
    auto it = settings.find(key);
    if (it != settings.end())
    {
        std::string val = it->second;
        return (val == "true" || val == "1");
    }
    return default_val;
}