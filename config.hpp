#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <iostream>
#include <fstream>
#include "external/json.hpp"
#include <codecvt>


using json = nlohmann::json;



class Config {
public:
    bool debug{};
    int interval{};
    std::string dataFormat{};
    std::string dataFilePath{};

    Config() {}

    bool isValid() {
        // Check if the interval isnt too low 
        if (interval < 5000) {
            std::cout << "Interval is too low, minimum is 5000ms" << std::endl;
            return false;
        }

        // Check if the user set an allowed output format
        if (!(dataFormat == "XML" || dataFormat == "JSON" || dataFormat == "CSV")) {
            std::cout << "Invalid dataFormat " << dataFormat << "! Allowed formats: XML, JSON, CSV";
            return false;
        }

        // Check if the data file path is writable
        std::ofstream testFile(dataFilePath, std::ios::app); // Use std::ios::app to append to the file (not modify its contents)
        if (!testFile) {
            std::cout << "Cannot write to file " << dataFilePath << "! Please check the path and permissions." << std::endl;
            return false;
        }
        testFile.close(); 

        return true;
    }

    // Function to load config from a JSON file
    bool loadFromFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open file " << filename << std::endl;
            return false;
        }

        try {
            json j;
            file >> j;
            debug = j.value("debug", true); 
            interval = j.value("interval", 0); // Use default value if not set
            dataFormat = j.value("dataFormat", "JSON"); // Default to JSON if not set
            dataFilePath = j.value("dataFilePath", ""); // Default to an empty string if not set

            // Validate the loaded configuration
            if (!isValid()) {
                std::cerr << "Invalid configuration settings." << std::endl;
                return false;
            }

            return true;
        }
        catch (json::exception& e) {
            std::cerr << "Error parsing JSON: " << e.what() << std::endl;
            return false;
        }
    }

    bool Update() {
        return loadFromFile(m_configPath);
    }
private:
    std::string m_configPath;
};

extern Config g_Config;

#endif //CONFIG_HPP