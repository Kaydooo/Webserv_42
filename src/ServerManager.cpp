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
            std::cerr << " webserv: select error " << strerror(errno) <<std::endl;
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i <= _biggest_fd + 3; ++i)
        {

            // std::cout << "BIGGETS FD is " << _biggest_fd << std::endl;
            if(FD_ISSET(i, &pool_cpy))
            {
                if(_servers_map.find(i) != _servers_map.end())
                    acceptNewConnection(_servers_map.find(i)->second);
                else
                    handle_request(i);

            }
        }
    }
    // std::vector<Server>::iterator it;
    // for(it = _servers.begin(); it != _servers.end(); ++it)
    //     it->run();
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

void    ServerManager::handle_request(int &i)
{
    char    buffer[8192];
    int     bytes_read = 0;
    int     client_index = 0;
    std::string request_content;
    std::string response_content;

    std::cout << "Message from: " << inet_ntoa(_clients_map[i].getAddress().sin_addr) << std::endl;

    bytes_read = read(i, buffer, sizeof(buffer));
    request_content.append(buffer);
    memset(buffer, 0, sizeof(buffer));
    if(bytes_read < 0)
    {
        std::cerr << " webserv: read error CS99" << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }
    RequestHandler request(request_content);
    request_content.clear();
    request.buildResponse();
    response_content = request.getContent();
    send(i, response_content.c_str(), strlen(response_content.c_str()), 0);
    send(i, request.getBody(), request.getBodyLength(), 0);
    FD_CLR(i, &_recv_fd_pool);
    close(i);
    _clients_map.erase(i);
    if(_clients.empty())
        _biggest_fd = (--_servers_map.end())->first;
}