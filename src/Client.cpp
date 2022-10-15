# include "../inc/Client.hpp"

Client::Client() : _total_bytes_read(0) {}

Client::~Client() {}

Client::Client(ServerConfig &server): _total_bytes_read(0)
{
    _response.setServer(server);
    _request.setMaxBodySize(_server.getClientMaxBodySize());
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

size_t      Client::getTotalBytes()
{
    return(_total_bytes_read);
}

void        Client::feedData(char *data, size_t size)
{
    _request.feed(data, size);
}

bool        Client::parsingCompleted()
{
    return (_request.parsingCompleted());
}

short       Client::requestError()
{
    return (_request.errorCode());
}

void        Client::clearRequest()
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

std::string     Client::getResponse()
{
    return (_response.getContent());
}

size_t          Client::getResponseLength()
{

    return (_response.getContent().length());
}

const   char    *Client::getResponseBody()
{
    return (_response.getBody());
}

size_t           Client::getResponseBodyLength()
{
    return (_response.getBodyLength());
}

void             Client::clearResponse()
{
    _response.clearResponse();
}

int              Client::getResponseCode()
{
    return (_response.getCode());
}

void             Client::setRespError(short error_code)
{
    _response.errResponse(error_code);
}
