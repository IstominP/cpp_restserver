// file      : user.hxx
// copyright : not copyrighted - public domain

#ifndef USER_HXX
#define USER_HXX

#include <string>

#include <odb/core.hxx>
#include <odb/version.hxx>

#pragma db object
class user
{
public:
    user (const std::string& username, const std::string& password)
        : username_ (username), password_ (password) {
    }

    const std::string& username() const {
        return username_;
    }

    const std::string& password() const {
        return password_;
    }

    const unsigned long id() const {
        return id_;
    }

    void password(const std::string& password) {
        password_.assign(password);
    }

private:
    friend class odb::access;

    user() {}

    #pragma db id auto
    unsigned long id_;

    std::string username_;
    std::string password_;
};

#endif // USER_HXX