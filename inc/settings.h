#ifndef SETTINGS_H
#define SETTINGS_H

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/info_parser.hpp>
#include <boost/foreach.hpp>
#include <string>
#include <map>
#include <exception>
#include <iostream>

using namespace std;
using namespace boost::property_tree;

class Settings
{
    static const string DB_SCHEME;
    static const string APP_SCHEME;

    map<string, string> appSettingsMap;
    map<string, string> dbSettingsMap;

public:
    Settings();

    void parseFromFile(const string &filename);

    string getAppSettingsAt(const string &key);
    string getDBSettingsAt(const string &key);

    int getDBSettingsSize();

    void allocateDBProperties(char **props);
};

#endif // SETTINGS_H