# include "../inc/ServerManager.hpp"


ServerManager::ServerManager() {}
ServerManager::~ServerManager(){}

/**
 * Start all servers on ports specified in the config file
 */
void    ServerManager::setupServers(std::vector<ServerConfig> servers)
{
    _servers = servers;

    for(std::vector<ServerConfig>::iterator it = _servers.begin(); it != _servers.end(); ++it)
        it->setupServer();
	std::cout << "fd in server = " << _servers[0].getFd() << std::endl;
}

/**
 * Runs main loop that goes through all file descriptors from 0 till the biggest fd in the set.
 * - check file descriptors returend from select():
 *      if server fd --> accept new client
 *      if client fd in read_set --> read message from client
 *      if client fd in write_set --> send response to client
 */
void    ServerManager::runServers()
{
    fd_set recv_set_cpy;
    fd_set write_set_cpy;

    _biggest_fd = 0;
    setupSelect();
    while(true)
    {
        recv_set_cpy = _recv_fd_pool;
        write_set_cpy = _write_fd_pool;

        if( select(FD_SETSIZE, &recv_set_cpy, &write_set_cpy, NULL, NULL) < 0 )
        {
            std::cerr << " webserv: select error " << strerror(errno) << std::endl;
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i <= _biggest_fd; ++i)
        {

            if(FD_ISSET(i, &write_set_cpy))
            {
                sendResponse(i);
            }
            else if(FD_ISSET(i, &recv_set_cpy))
            {
                if(_servers_map.count(i))
                    acceptNewConnection(_servers_map.find(i)->second);
                else
                    readRequest(i);
            }

        }
    }
}

/**
 * Accept new incomming connection.  
 * Create new Client object and add it to _client_map
 * Add client socket to _recv_fd_pool
*/
void    ServerManager::acceptNewConnection(ServerConfig &serv)
{
    struct sockaddr_in client_address;
    long  client_address_size = sizeof(client_address);
    int client_sock;
    Client  new_client(serv);
    
    if ( (client_sock = accept(serv.getFd(), (struct sockaddr *)&client_address,
     (socklen_t*)&client_address_size)) == -1)
    {
        std::cerr << " webserv: accept error " << strerror(errno) <<std::endl;
        exit(EXIT_FAILURE);
    }

    FD_SET(client_sock, &_recv_fd_pool);

    if(fcntl(client_sock, F_SETFL, O_NONBLOCK) < 0)
    {
        std::cerr << " webserv: fcntl error" << strerror(errno) <<std::endl;
        exit(EXIT_FAILURE);
    }

    new_client.setSocket(client_sock);
    _clients_map.insert(std::make_pair(client_sock, new_client));
    _biggest_fd = (--_clients_map.end())->first;
    std::cout << "Connection From: " << inet_ntoa(new_client.getAddress().sin_addr) << std::endl;
}

/*
    initialize recv+write fd_sets and add all server listening sockets to _recv_fd_pool. 
*/
void    ServerManager::setupSelect()
{
    FD_ZERO(&_recv_fd_pool);
    FD_ZERO(&_write_fd_pool);
    
    // adds servers sockets to _recv_fd_pool set
    for(std::vector<ServerConfig>::iterator it = _servers.begin(); it != _servers.end(); ++it)
    {
        if (listen(it->getFd(), 10) == -1) 
        {
            std::cerr << " webserv: listen error: " << strerror(errno) << std::endl;
            exit(EXIT_FAILURE);
        }
        std::cout << "Server fd = " << it->getFd() << " is listening" << std::endl;
        if(fcntl(it->getFd(), F_SETFL, O_NONBLOCK) < 0)
        {
            std::cerr << " webserv: fcntl error" << strerror(errno) <<std::endl;
            exit(EXIT_FAILURE);
        }
        FD_SET(it->getFd(), &_recv_fd_pool);
        _servers_map.insert(std::make_pair(it->getFd(), *it));
    }
    // at this stage _biggest_fd will belong to the last server created.
    _biggest_fd = _servers.back().getFd();

}

/**
 * Build the response and send it to client. 
 * If no error was found in request and Connection header value is keep-alive,
 * connection is kept, otherwise connection will be closed.
 */
void    ServerManager::sendResponse(int &i)
{
    _clients_map[i].buildResponse();
    send(i, _clients_map[i].getResponse().c_str(), _clients_map[i].getResponseLength(), 0);
    send(i, _clients_map[i].getResponseBody(), _clients_map[i].getResponseBodyLength(), 0);
    if(_clients_map[i].keepAlive() == false || _clients_map[i].requestError() ||
       _clients_map[i].getResponseCode() == 404)
    {
        FD_CLR(i, &_write_fd_pool);
        close(i);
        _clients_map.erase(i);
        if(_clients_map.empty())
            _biggest_fd = (--_servers_map.end())->first;
    }
    else
    {
        FD_CLR(i, &_write_fd_pool);
        FD_SET(i, &_recv_fd_pool);
        _clients_map[i].clearRequest();
        _clients_map[i].clearResponse();
    }
}

/**
 * - Reads data from client and feed it to the parser.
 * Once parser is done or an error was found in the request,
 * socket will be moved from _recv_fd_pool to _write_fd_pool 
 * and response will be sent on the next iteration of select().
 */
void    ServerManager::readRequest(int &i)
{
    char    buffer[8192];
    int     bytes_read = 0;

    std::cout << "Message from: " << inet_ntoa(_clients_map[i].getAddress().sin_addr) << " Socket no : " <<
    i << std::endl;

    bytes_read = read(i, buffer, sizeof(buffer));
    
    if(bytes_read < 0)
    {
        std::cerr << " webserv: read error" << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }
    else if(bytes_read != 0)
    {
        _clients_map[i].feedData(buffer, bytes_read);
        memset(buffer, 0, sizeof(buffer));
    }

    if (_clients_map[i].requestError()) // if error was found in request, send 400 bad_request and close connection after sending.
    {
        std::cout << "Bad Request, Connection Closed !" << std::endl; 
        _clients_map[i].setRespError(_clients_map[i].requestError());
        FD_CLR(i, &_recv_fd_pool);
        FD_SET(i, &_write_fd_pool);
    }
    else if(_clients_map[i].parsingCompleted()) // 1 = parsing completed and we can work on the response.
    {
        FD_CLR(i, &_recv_fd_pool);
        FD_SET(i, &_write_fd_pool); // move socket i from recive fd_set to write fd_set so response can be sent on next iteration
    }
}
