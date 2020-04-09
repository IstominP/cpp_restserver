#include "keyvaluedbengine.h"
#include "log4cxx/logger.h"

using namespace log4cxx;
using namespace std;

LoggerPtr loggerDB(Logger::getLogger("KeyValueDB"));

void DisplayJSON(json::value const &jvalue) {
    cout << jvalue.serialize() << endl;
}

void RequestWorker(http_request &request,
                   function<void(json::value const &, json::value &)> handler) {
    auto result = json::value::object();
    request
        .extract_json()
        .then([&result, &handler](pplx::task<json::value> task) {
            try {
                LOG4CXX_INFO(loggerDB, "Task reached. ");
                auto const &jvalue = task.get();
                if (!jvalue.is_null()) {
                    DisplayJSON(jvalue);
                    handler(jvalue, result);
                }
            } catch (http_exception const &e) {
                wcout << "Ошибка! " << e.what() << endl;
            }
        })
        .wait();

    request.reply(status_codes::OK, result);
}

KeyValueDbEngine::KeyValueDbEngine(int argc, char *argv[]) {
    db = create_database(argc, argv);
    LOG4CXX_INFO(loggerDB, "Database connection reserved.")
}

void KeyValueDbEngine::put(http_request &request) {
    RequestWorker(request, [&](json::value const &jvalue, json::value &result) {
            DisplayJSON(jvalue);

            for (auto const &e : jvalue.as_object()) {
                if (e.second.is_string()) {
                    auto key = e.first;
                    auto value = e.second.as_string();

                    auto id = getUserIdByName(key);
                    LOG4CXX_INFO(loggerDB, "Updating user by id: " << id);
                    updateUserPassword(id, value);
                }
            }
        });
}

unsigned long KeyValueDbEngine::getUserIdByName(const string &username) {
    typedef odb::query<user> query;

    std::unique_ptr<user> nullUser = std::make_unique<user>("null", "null");

    try {
        transaction t(db->begin());

        for (user& u: db->query<user>(query::username == username)) {
            return u.id();
        }

        t.commit();
    } catch (const odb::exception &e) {
        LOG4CXX_ERROR(loggerDB, "Exception rised " << e.what());
        return 0;
    }
}

long KeyValueDbEngine::updateUserPassword(long userId, const string &password) {
    try {
        transaction t(db->begin());

        std::auto_ptr<user> u(db->load<user>(userId));
        LOG4CXX_INFO(loggerDB, "Updating user: " << u->username());
        u->password(password);
        db->update(*u);

        t.commit();

        return userId;
    } catch (const odb::exception &e) {
        LOG4CXX_ERROR(loggerDB, "Exception rised " << e.what());
        return 0;
    }
}

void KeyValueDbEngine::get(http_request &request) {
    LOG4CXX_INFO(loggerDB, "GET Reached.");
    RequestWorker(request, [&](json::value const &jvalue, json::value &result) {                
                for (auto const &e : jvalue.as_object()) {   
                    if (e.second.is_string()) {
                        auto key = e.first;
                        if (key == U("username")) {
                            auto value = e.second.as_string();

                            std::unique_ptr<user> u(getUserByName(value));

                            result = json::value::parse(U("{ \"username\": \"" + u->username() + "\", \"password\": \"" + u->password() + "\" }"));
                        }
                    }
                }
            });
}

std::unique_ptr<user> KeyValueDbEngine::getUserByName(const string &username) {
    typedef odb::query<user> query;

    std::unique_ptr<user> nullUser = std::make_unique<user>("null", "null");

    try {
        transaction t(db->begin());

        for (user& u: db->query<user>(query::username == username)) {
            return std::make_unique<user>(u.username(), u.password());
        }

        t.commit();
    } catch (const odb::exception &e) {
        LOG4CXX_ERROR(loggerDB, "Exception rised " << e.what());
        return nullUser;
    }
}

void KeyValueDbEngine::post(http_request &request) {
    RequestWorker(
        request, [&](json::value const &jvalue, json::value &result) {
            string name;
            string password;

            for (auto const &e : jvalue.as_object()) {
                if (e.second.is_string()) {
                    auto key = e.first;
                    if (key == U("username")) {
                        name = e.second.as_string();
                    }

                    if (key == U("password")) {
                        password = e.second.as_string();
                    }
                }
            }

            auto id = createNewUser(name, password);

            if (id == -1) {
                LOG4CXX_WARN(loggerDB, "User already exists");
            }
        });
}

long KeyValueDbEngine::createNewUser(const string& username, const string& password) {
    typedef odb::query<user> query;

    try {
        transaction t(db->begin());
        for (user& u: db->query<user>(query::username == username)) {
            return -1;
        }

        user newUser(username, password);

        auto newUserId = db->persist(newUser);
        t.commit();


        LOG4CXX_INFO(loggerDB, "New user created with id " << newUserId);
        return newUserId;
    } catch (const odb::exception &e) {
        LOG4CXX_ERROR(loggerDB, "Exception rised " << e.what());
        return -1;
    }
}

void KeyValueDbEngine::del(http_request &request) {
}
