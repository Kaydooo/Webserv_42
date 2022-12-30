# include "../inc/Client.hpp"

Client::Client()
{
    _last_msg_time = time(NULL);
}

Client::~Client() {}

/* Copy constructor */
Client::Client(const Client &other)
{
	if (this != &other)
	{
		this->_client_socket = other._client_socket;
		this->_client_address = other._client_address;
		this->request = other.request;
		this->response = other.response;
		this->server = other.server;
		this->_last_msg_time = other._last_msg_time;

	}
	return ;
}

/* Assinment operator */
Client &Client::operator=(const Client & rhs)
{
	if (this != &rhs)
	{
		this->_client_socket = rhs._client_socket;
		this->_client_address = rhs._client_address;
		this->request = rhs.request;
		this->response = rhs.response;
		this->server = rhs.server;
		this->_last_msg_time = rhs._last_msg_time;
	}
	return (*this);
}

Client::Client(ServerConfig &server)
{
    setServer(server);
    request.setMaxBodySize(server.getClientMaxBodySize());
    _last_msg_time = time(NULL);
}

void    Client::setSocket(int &sock)
{
    _client_socket = sock;
}

void    Client::setAddress(sockaddr_in &addr)
{
    _client_address =  addr;
}

void    Client::setServer(ServerConfig &server)
{
    response.setServer(server);
}


const int     &Client::getSocket() const
{
    return (_client_socket);
}

const HttpRequest   &Client::getRequest() const
{
    return (request);
}

const struct sockaddr_in    &Client::getAddress() const
{
    return (_client_address);
}

const time_t     &Client::getLastTime() const
{
    return (_last_msg_time);
}


void        Client::buildResponse()
{
    response.setRequest(this->request);
    response.buildResponse();
}

void             Client::updateTime()
{
    _last_msg_time = time(NULL);
}

void             Client::clearClient()
{
    response.clear();
    request.clear();
}