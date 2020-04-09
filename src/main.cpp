#include "restdbserviceserver.h"
#include "settings.h"

#include <memory> // std::auto_ptr
#include <iostream>
#include <string>
#include <set>
#include <exception>
#include <iostream>

// include log4cxx header files.
#include "log4cxx/logger.h"
#include "log4cxx/basicconfigurator.h"
#include "log4cxx/propertyconfigurator.h"
#include "log4cxx/helpers/exception.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/info_parser.hpp>
#include <boost/foreach.hpp>

using namespace std;

using namespace log4cxx;
using namespace log4cxx::helpers;

using namespace boost::property_tree;

std::unique_ptr<RestDBServiceServer> g_http;
std::unique_ptr<Settings> settings;

LoggerPtr logger(Logger::getLogger("RestServerApp"));

void startServer(const utility::string_t &address, int argc, char *argv[]) {
    uri_builder uri(address);
    uri.append_path(U("dbdemo/"));
    auto addr = uri.to_uri().to_string();

    g_http = std::make_unique<RestDBServiceServer>(addr, argc, argv);
    g_http->Open().wait();

    LOG4CXX_INFO(logger, "Server started.")
}

void shutDownServer() {
    g_http->Close().wait();
    return;
}

void configureLogger(const string &filename) {
    File f(filename);
    PropertyConfigurator::configure(f);
}

int main(int argc, char *argv[]) {
    settings = std::make_unique<Settings>();
    settings->parseFromFile("test.conf");

    configureLogger(settings->getAppSettingsAt("logFileName"));
    
    int propertiesCapacity = (settings->getDBSettingsSize() * 2) + 1;
    char *properties[propertiesCapacity];
    settings->allocateDBProperties(properties);

    utility::string_t address = U("http://localhost:" + settings->getAppSettingsAt("port"));
    startServer(address, propertiesCapacity, properties);

    std::string line;
    std::getline(std::cin, line);

    shutDownServer();
    return 0;
}