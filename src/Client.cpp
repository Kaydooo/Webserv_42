# include "../inc/Client.hpp"

Client::Client() :_request_length(0) {}

Client::~Client() {}

void    Client::setSocket(int &sock)
{
    _client_socket = sock;
}

void    Client::setAddress(sockaddr_in &addr)
{
    _client_address =  addr;
}

int     Client::getSocket()
{
    return (_client_socket);
}

HttpRequest     &Client::getRequest()
{
    return (_request);
}

struct sockaddr_in    Client::getAddress()
{
    return (_client_address);
}

void        Client::feedData(char *data, size_t size)
{
    _request.feed(data, size);
}

bool        Client::requestState()
{
    return (_request.parsingCompleted());
}

bool        Client::requestError()
{
    return (_request.errorCode());
}

void        Client::clearForNextRequest()
{
    _request.clear();
}

bool        Client::keepAlive()
{
    return (_request.keepAlive());
}

