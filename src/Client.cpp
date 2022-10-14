# include "../inc/Client.hpp"

Client::Client() {}

Client::~Client() {}

Client::Client(ServerConfig &server) 
{
    _response.setServer(server);
}

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

void        Client::buildResponse()
{
    _response.setRequest(_request);
    _response.buildResponse();
}

std::string Client::getResponse()
{
    return (_response.getContent());
}

size_t              Client::getResponseLength()
{

    return (_response.getContent().length());
}

const   char                *Client::getResponseBody()
{
    return (_response.getBody());
}

size_t              Client::getResponseBodyLength()
{
    return (_response.getBodyLength());
}

bool                Client::badRequest()
{
    if(_request.errorCode() == 400 || _request.errorCode() == 405) // add all errors that should close the connection here.
        return (true);
    return(false);
}

void                Client::clearResponse()
{
    _response.clearResponse();
}

int                 Client::responseCode()
{
    return (_response.getCode());
}
