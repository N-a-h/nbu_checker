
#include <iostream>
#include <sstream>

#define WIN32_LEAN_AND_MEAN
#include "httplib.h"

#include "tinyxml2.h"
#include "json.hpp"

/// <summary>
/// Requests data from NBU api
/// </summary>
/// <param name="url">api endpoint</param>
/// <returns>body on success, empty string on fail </returns>
static std::string getApiData(const std::string& url) {
    httplib::SSLClient cli("bank.gov.ua");
    cli.set_read_timeout(10, 0);
    auto res = cli.Get(url.c_str());
    if (res && res->status == 200) {
        return res->body;
    }
    return "";
}

using json = nlohmann::json;

/// <summary>
/// Validates requested JSON data
/// </summary>
/// <param name="jsonData">data to validate</param>
/// <returns>true on success</returns>
static bool validateJSON(const std::string& jsonData) {
    try {
        auto j = json::parse(jsonData);
        for (const auto& item : j) {
            if (!item.is_object() ||
                item.find("r030") == item.end() ||
                item.find("txt") == item.end() ||
                item.find("rate") == item.end() ||
                item.find("cc") == item.end() ||
                item.find("exchangedate") == item.end()) {
                return false;  
            }
        }
        return true;  
    }
    catch (...) {
        return false; 
    }
}


/// <summary>
/// Converts json-formatted data to CSV
/// </summary>
/// <param name="jsonData">json to convert</param>
/// <returns>processed csv or nothing on bad json</returns>
static std::string jsonToCSV(const std::string& jsonData) {
    auto j = json::parse(jsonData);
    std::stringstream csv;
    if (!j.empty() && j.front().is_object()) {
        for (auto it = j.front().begin(); it != j.front().end(); ++it) {
            csv << it.key() << (it != --j.front().end() ? "," : "\n");
        }
        for (const auto& item : j) {
            for (auto it = item.begin(); it != item.end(); ++it) {
                csv << it.value() << (it != --item.end() ? "," : "\n");
            }
        }
    }
    return csv.str();
}


/// <summary>
/// Validates fetched XML document
/// </summary>
/// <param name="xmlData">data to validate</param>
/// <returns>true on success</returns>

static bool validateXML(const std::string& xmlData) {
    tinyxml2::XMLDocument doc;
    doc.Parse(xmlData.c_str());
    const tinyxml2::XMLElement* root = doc.FirstChildElement("exchange");
    if (!root) return false;

    for (const tinyxml2::XMLElement* currency = root->FirstChildElement("currency"); currency != nullptr; currency = currency->NextSiblingElement("currency")) {
        if (!currency->FirstChildElement("r030") ||
            !currency->FirstChildElement("txt") ||
            !currency->FirstChildElement("rate") ||
            !currency->FirstChildElement("cc") ||
            !currency->FirstChildElement("exchangedate")) {
            return false;
        }
    }
    return true;
}