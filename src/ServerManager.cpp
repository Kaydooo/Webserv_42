# include "../inc/ServerManager.hpp"

ServerManager::ServerManager() {}

ServerManager::~ServerManager(){}

void    ServerManager::setupServers(std::vector<ServerConfig> servers)
{
    _servers = servers;

    for(std::vector<ServerConfig>::iterator it = _servers.begin(); it != _servers.end(); ++it)
    {
        it->setupServer();
    }
	std::cout << "fd in server = " << _servers[0].getFd() << std::endl;
}

void    ServerManager::runServers()
{
    fd_set pool_cpy;
    _biggest_fd = 0;
    setupSelect();
    while(true)
    {
        pool_cpy = _recv_fd_pool;
        if( select(FD_SETSIZE, &pool_cpy, NULL, NULL, NULL) < 0 )
        {
            std::cerr << " webserv: select error " << strerror(errno) << std::endl;
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i <= _biggest_fd + 3; ++i)
        {

            if(FD_ISSET(i, &pool_cpy))
            {
                if(_servers_map.count(i))
                {
                    acceptNewConnection(_servers_map.find(i)->second);
                }
                else
                    handleRequest(i);
            }
        }
    }
}


void    ServerManager::acceptNewConnection(ServerConfig &serv)
{
    struct sockaddr_in client_address;
    long  client_address_size = sizeof(client_address);
    int client_sock;
    Client  new_client(serv);
    
    if ( (client_sock = accept(serv.getFd(), (struct sockaddr *)&client_address,
     (socklen_t*)&client_address_size)) == -1)
    {
        std::cout << "fd = " << serv.getFd() << std::endl;
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
    _clients.push_back(new_client);
    _clients_map.insert(std::make_pair(client_sock, new_client));
    _biggest_fd = (--_clients_map.end())->first;
    std::cout << "New Connection with: " << inet_ntoa(new_client.getAddress().sin_addr) << std::endl;
    // new_client.setAddress(client_address);
}

void    ServerManager::setupSelect()
{
    FD_ZERO(&_recv_fd_pool);
    
    // Add all servers sockets to _recv_fd_set
    for(server_vec::iterator it = _servers.begin(); it != _servers.end(); ++it)
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
    _biggest_fd = _servers.back().getFd();

}

void    ServerManager::handleRequest(int &i)
{
    char    buffer[8192];
    int     bytes_read = 0;

    std::cout << "Message from: " << inet_ntoa(_clients_map[i].getAddress().sin_addr) << " Socket no : " <<
    i << std::endl;


    // if(send(i, "", 0, 0) < 0)
    //     std::cerr << "ERROR WRITEING TO SOCKET\n";
    // int error = 0;
    // socklen_t len = sizeof (error);
    // int retval = getsockopt (i, SOL_SOCKET, SO_ERROR, &error, &len);
    // if (retval != 0) {
    // /* there was a problem getting the error code */
    // std::cerr << "error getting socket error code: \n";
    // FD_CLR(i, &_recv_fd_pool);
    // close(i);
    // _clients_map.erase(i);
    // if(_clients.empty())
    //     _biggest_fd = (--_servers_map.end())->first;
    // return;
    // }

    // if (error != 0) {
    //     /* socket has a non zero error status */
    //     fprintf(stderr, "socket error: %s\n", strerror(error));
    // FD_CLR(i, &_recv_fd_pool);
    // close(i);
    // _clients_map.erase(i);
    // if(_clients.empty())
    //     _biggest_fd = (--_servers_map.end())->first;
    // }


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
    if (_clients_map[i].requestError())
    {
        std::cout << "Bad Request, Connection Closed !" << std::endl; // send bad request response here.
        FD_CLR(i, &_recv_fd_pool);
        close(i);
        _clients_map.erase(i);
        if(_clients.empty())
            _biggest_fd = (--_servers_map.end())->first;
    }
    else if(_clients_map[i].requestState()) // 1 = parsing completed so we can work on the response.
    {
        _clients_map[i].buildResponse();
        send(i, _clients_map[i].getResponse().c_str(), _clients_map[i].getResponseLength(), 0);
        send(i, _clients_map[i].getResponseBody(), _clients_map[i].getResponseBodyLength(), 0);
        if(_clients_map[i].keepAlive() == false || _clients_map[i].responseCode() == 404)
        {
            FD_CLR(i, &_recv_fd_pool);
            close(i);
            _clients_map.erase(i);
            if(_clients.empty())
                _biggest_fd = (--_servers_map.end())->first;
        }
        _clients_map[i].clearForNextRequest();
        _clients_map[i].clearResponse();
    }

}

// void            ServerManager::sendResponse(int i, HttpRequest& req)
// {

// }