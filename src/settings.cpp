#include "settings.h"

#include "log4cxx/logger.h"

using namespace log4cxx;
using namespace std;

LoggerPtr loggerSettings(Logger::getLogger("KeyValueDB"));

const string Settings::DB_SCHEME = string("settings.db");
const string Settings::APP_SCHEME = string("settings.app");

Settings::Settings() {

}

void Settings::parseFromFile(const string &filename) {
    ptree pt;
    read_info(filename, pt); 
    
    BOOST_FOREACH(ptree::value_type &v, pt.get_child(APP_SCHEME)) {
        appSettingsMap.insert(pair<string,string>(v.first.data(), v.second.data()));
    }

    BOOST_FOREACH(ptree::value_type &v, pt.get_child(DB_SCHEME)) {
        dbSettingsMap.insert(pair<string,string>(v.first.data(), v.second.data()));
    }

    LOG4CXX_INFO(loggerSettings, "Settings parsed.")
}

string Settings::getAppSettingsAt(const string &key) {
    return appSettingsMap.at(key);
}

string Settings::getDBSettingsAt(const string &key) {
    return dbSettingsMap.at(key);
}

char *getCharsFromString(string value) {
    char *cstr = new char[value.length() + 1];
    strcpy(cstr, value.c_str());

    return cstr;
}

int Settings::getDBSettingsSize() {
    return dbSettingsMap.size();
}

void Settings::allocateDBProperties(char **props) {
    int dbSettingsIndex = 0;

    string programName = "restserver";

    props[dbSettingsIndex] = getCharsFromString(programName);
    dbSettingsIndex += 1;

    for (auto it = dbSettingsMap.begin(); it != dbSettingsMap.end(); ++it) {
        string key = "--" + it->first;
        
        props[dbSettingsIndex] = getCharsFromString(key);
        dbSettingsIndex += 1;

        props[dbSettingsIndex] = getCharsFromString(it->second);
        dbSettingsIndex += 1;
    }
}