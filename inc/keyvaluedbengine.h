#ifndef KEYVALUEDBENGINE_H
#define KEYVALUEDBENGINE_H

#include "cpprest/http_listener.h"
#include <memory> // std::auto_ptr

#include <odb/database.hxx>
#include <odb/transaction.hxx>

#include "database.hxx" // create_database

#include "user.hxx"
#include "user-odb.hxx"

using namespace std;
using namespace odb::core;

using namespace web;
using namespace http;
using namespace web::http::experimental::listener;

class KeyValueDbEngine
{
    std::map<utility::string_t, utility::string_t> storage;
    std::auto_ptr<database> db;

public:
    KeyValueDbEngine(int argc, char *argv[]);

    void get(http_request &request);
    void post(http_request &request);
    void put(http_request &request);
    void del(http_request &request);

    std::unique_ptr<user> getUserByName(const string& username);
    // user &getUserByName(const string& username);
    long createNewUser(const string& username, const string& password);
    long updateUserPassword(long userId, const string& password);
    void deleteUserByName(const string& username);
};

#endif // KEYVALUEDBENGINE_H
