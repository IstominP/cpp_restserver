#ifndef RESTDBSERVICESERVER_H
#define RESTDBSERVICESERVER_H

#include "cpprest/http_listener.h"

using namespace web;
using namespace http;
using namespace web::http::experimental::listener;

class RestDBServiceServer
{
public:
    RestDBServiceServer(utility::string_t url, int argc, char *argv[]);
    pplx::task<void> Open() { return m_listener.open(); }
    pplx::task<void> Close() { return m_listener.close(); }

private:
    void HandleGet(http_request message);
    void HandlePut(http_request message);
    void HandlePost(http_request message);
    void HandleDelete(http_request message);

    http_listener m_listener;
};

#endif // RESTDBSERVICESERVER_H
