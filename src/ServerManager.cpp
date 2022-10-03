# include "../inc/ServerManager.hpp"

ServerManager::ServerManager() {}

ServerManager::~ServerManager(){}

void    ServerManager::setupServers()
{
    Server testServer; // later create servers here using info provided in config file.
    _servers.push_back(testServer);
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
                    acceptNewConnection(_servers_map.find(i)->second);
                else
                    handleRequest(i);

            }
        }
    }
}


void    ServerManager::acceptNewConnection(Server &serv)
{
    struct sockaddr_in client_address;
    long  client_address_size = sizeof(client_address);
    int client_sock;
    Client  new_client;
    
    if ( (client_sock = accept(serv.getFd(), (struct sockaddr *)&client_address,
     (socklen_t*)&client_address_size)) == -1)
    {
        std::cerr << " webserv: accept error" << strerror(errno) <<std::endl;
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

    std::cout << "Message from: " << inet_ntoa(_clients_map[i].getAddress().sin_addr) << std::endl;

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
        RequestHandler response(_clients_map[i].getRequest()); // change class name to something else later.
        response.buildResponse();
        send(i, response.getContent().c_str(), strlen(response.getContent().c_str()), 0);
        send(i, response.getBody(), response.getBodyLength(), 0);
        if(_clients_map[i].keepAlive() == 0)
        {
            FD_CLR(i, &_recv_fd_pool);
            close(i);
            _clients_map.erase(i);
            if(_clients.empty())
                _biggest_fd = (--_servers_map.end())->first;
        }
        else
            _clients_map[i].clear();
    }

}

// void            ServerManager::sendResponse(int i, HttpRequest& req)
// {

// }