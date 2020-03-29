#include "keyvaluedbengine.h"

#include "log4cxx/logger.h"

using namespace log4cxx;
using namespace std;

LoggerPtr loggerDB(Logger::getLogger("KeyValueDB"));

void DisplayJSON(json::value const &jvalue)
{
    cout << jvalue.serialize() << endl;
}

void RequestWorker(http_request &request,
                   function<void(json::value const &, json::value &)> handler)
{
    auto result = json::value::object();
    request
        .extract_json()
        .then([&result, &handler](pplx::task<json::value> task) {
            try
            {
                LOG4CXX_INFO(loggerDB, "Task reached. ");
                auto const &jvalue = task.get();
                if (!jvalue.is_null())
                {
                    DisplayJSON(jvalue);
                    handler(jvalue, result);
                }
            }
            catch (http_exception const &e)
            {
                wcout << "Ошибка! " << e.what() << endl;
            }
        })
        .wait();

    request.reply(status_codes::OK, result);
}

KeyValueDbEngine::KeyValueDbEngine(int argc, char *argv[])
{
    db = create_database(argc, argv);
    createNewUser("pasha", "3469755");

    LOG4CXX_INFO(loggerDB, "Database connection reserved.")
}

void KeyValueDbEngine::get(http_request &request)
{
    LOG4CXX_INFO(loggerDB, "GET Reached.");
    RequestWorker(
            request,
            [&](json::value const &jvalue, json::value &result) {
                LOG4CXX_INFO(loggerDB, "Request with parameters " << jvalue.serialize());
                
                for (auto const &e : jvalue.as_object())
                {   
                    if (e.second.is_string())
                    {
                        auto key = e.first;
                        if (key == U("username")) {
                            auto value = e.second.as_string();

                            std::unique_ptr<user> u(getUserByName(value));
                            // user u(getUserByName(value));

                            result = json::value::parse(U("{ \"username\": \"" + u->username() + "\", \"password\": \"" + u->password() + "\" }"));
                        }
                    }
                }
            });
}

void KeyValueDbEngine::post(http_request &request)
{
    RequestWorker(
        request,
        [&](json::value const &jvalue, json::value &result) {
            DisplayJSON(jvalue);

            for (auto const &e : jvalue.as_array())
            {
                if (e.is_string())
                {
                    auto key = e.as_string();
                    auto pos = storage.find(key);

                    if (pos == storage.end())
                    {
                        result[key] = json::value::string("not found");
                    }
                    else
                    {
                        result[pos->first] = json::value::string(pos->second);
                    }
                }
            }
        });
}

void KeyValueDbEngine::put(http_request &request)
{
    RequestWorker(
        request,
        [&](json::value const &jvalue, json::value &result) {
            DisplayJSON(jvalue);

            for (auto const &e : jvalue.as_object())
            {
                if (e.second.is_string())
                {
                    auto key = e.first;
                    auto value = e.second.as_string();

                    if (storage.find(key) == storage.end())
                    {
                        result[key] = json::value::string("<put>");
                    }
                    else
                    {
                        result[key] = json::value::string("<updated>");
                    }

                    storage[key] = value;
                }
            }
        });
}

void KeyValueDbEngine::del(http_request &request)
{
}

long KeyValueDbEngine::createNewUser(const string& username, const string& password) {
    user newUser(username, password);

    transaction t(db->begin());
    auto newUserId = db->persist(newUser);
    t.commit();

    LOG4CXX_INFO(loggerDB, "New user created with id " << newUserId);
    return newUserId;
}

std::unique_ptr<user> KeyValueDbEngine::getUserByName(const string &username) {
// user& KeyValueDbEngine::getUserByName(const string &username) {
    typedef odb::query<user> query;
    typedef odb::result<user> result;

    std::unique_ptr<user> nullUser = std::make_unique<user>("null", "null");
    // user nullUser("null", "null");

    try {
        transaction t(db->begin());

        for (user& u: db->query<user>(query::username == username)) {
            // LOG4CXX_INFO(loggerDB, "User get from database with username " << u.username() << " and password " << u.password());
            return std::make_unique<user>(u.username(), u.password());
            // return u;
        }

        t.commit();
    } catch (const odb::exception &e) {
        LOG4CXX_ERROR(loggerDB, "Exception rised " << e.what());
        return nullUser;
    }
}

long KeyValueDbEngine::updateUserPassword(long userId, const string &password) {
    try {
        transaction t(db->begin());

        std::auto_ptr<user> u(db->load<user>(userId));
        u->password(password);
        db->update(*u);

        t.commit();

        return userId;
    } catch (const odb::exception &e) {
        LOG4CXX_ERROR(loggerDB, "Exception rised " << e.what());
        return 0;
    }
}
