#include "restdbserviceserver.h"
#include "keyvaluedbengine.h"

std::unique_ptr<KeyValueDbEngine> k_storage;

RestDBServiceServer::RestDBServiceServer(utility::string_t url, int argc, char *argv[])
    : m_listener(url)
{
    k_storage = std::make_unique<KeyValueDbEngine>(argc, argv);

    using namespace std::placeholders;
    m_listener.support(
        methods::GET,
        std::bind(&RestDBServiceServer::HandleGet, this, _1));
    m_listener.support(
        methods::PUT,
        std::bind(&RestDBServiceServer::HandlePut, this, _1));
    m_listener.support(
        methods::POST,
        std::bind(&RestDBServiceServer::HandlePost, this, _1));
    m_listener.support(
        methods::DEL,
        std::bind(&RestDBServiceServer::HandleDelete, this, _1));
}

void RestDBServiceServer::HandleGet(http_request message)
{
    k_storage->get(message);
}

void RestDBServiceServer::HandlePost(http_request message)
{
    k_storage->post(message);
}

void RestDBServiceServer::HandlePut(http_request message)
{
    k_storage->put(message);
}

void RestDBServiceServer::HandleDelete(http_request message)
{
    k_storage->get(message);
}
