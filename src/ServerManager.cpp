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
    int    set_size = 0;
    setupSelect(set_size);
    while(true)
    {
        pool_cpy = _recv_fd_pool;
        if( select(FD_SETSIZE, &pool_cpy, NULL, NULL, NULL) < 0 )
        {
            std::cerr << " webserv: select error " << strerror(errno) <<std::endl;
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < FD_SETSIZE; ++i)
        {
            if(FD_ISSET(i, &pool_cpy))
            {
                if(!checkServer(i))
                    handle_request(i, set_size);
                else
                    set_size++;
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
    std::cout << "New Connection with: " << inet_ntoa(new_client.getAddress().sin_addr) << std::endl;
    // new_client.setAddress(client_address);
}

void    ServerManager::setupSelect(int &set_size)
{
    set_size += _servers.size();
    FD_ZERO(&_recv_fd_pool);
    
    // Add all servers sockets to recv fd_set
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
    }
}

int     ServerManager::checkServer(int &i)
{
    for(server_vec::iterator it = _servers.begin(); it != _servers.end(); ++it)
    {
        if(i == it->getFd())
        {
            acceptNewConnection(*it);
            return (1);
        }
    }
    return (0);
}

void    ServerManager::handle_request(int &i, int &set_size)
{
    char    buffer[8192];
    int     bytes_read = 0;
    int     client_index = 0;
    std::string request_content;
    std::string        response_content;

    if(client_index)
    {}

    for(size_t j = 0; j < _clients.size(); ++j)
    {
        if(_clients[j].getSocket() == i)
            client_index = j;
    }

    std::cout << "Message from: " << inet_ntoa(_clients[client_index].getAddress().sin_addr) << std::endl;

    bytes_read = read(i, buffer, sizeof(buffer));
    buffer[bytes_read] = '\0';
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
    set_size--;
    close(i);
}