# include "../inc/Server.hpp"
# include "../inc/RequestHandler.hpp"

Server::Server(): _port(TEST_PORT)
{
    _port = TEST_PORT;

    if((_listen_fd = socket(AF_INET, SOCK_STREAM, 0) )  == -1 )
    {
        std::cerr << " webserv: socket error" << std::endl;
        exit(EXIT_FAILURE);
    }

    int option_value = 1;
    setsockopt(_listen_fd, SOL_SOCKET, SO_REUSEADDR, &option_value, sizeof(int));
    
    memset(&_server_address, 0, sizeof(_server_address));
    memset(&_client_address, 0, sizeof(_client_address));

    _server_address.sin_family = AF_INET;
    _server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    _server_address.sin_port = htons(_port);


    if (bind(_listen_fd, (struct sockaddr *) &_server_address, sizeof(_server_address)) == -1)
    {
        std::cerr << " webserv: bind error" << std::endl;
        exit(EXIT_FAILURE);
    }
}

Server::~Server(){}

// void    Server::_acceptConnection(fd_set& sockets_pool, int& set_size)
// {
//     long          _client_address_size = sizeof(_client_address);;
    
//     if ( (_connection_fd = accept(_listen_fd, (struct sockaddr *)&_client_address, (socklen_t*)&_client_address_size)) == -1)
//     {
//         std::cerr << " webserv: accept error" << strerror(errno) <<std::endl;
//         exit(EXIT_FAILURE);
//     }
//     FD_SET(_connection_fd, &sockets_pool);
//     set_size++;
// }


// void    Server::_handle_request(fd_set& sockets_pool, int& set_size, int& socket_fd)
// {
//     char    buffer[8192];
//     int     bytes_read = 0;
//     std::string        response_content;

//     bytes_read = read(socket_fd, buffer, sizeof(buffer));
//     buffer[bytes_read] = '\0';
//     _request_content.append(buffer);
//     memset(buffer, 0, sizeof(buffer));
//     if(bytes_read < 0)
//     {
//         std::cerr << " webserv: read error CS99" << strerror(errno) << std::endl;
//         exit(EXIT_FAILURE);
//     }
//     RequestHandler request(_request_content);
//     std::cout << _request_content << std::endl;
//     _request_content.clear();
//     request.buildResponse();
//     response_content = request.getContent();
//     send(socket_fd, response_content.c_str(), strlen(response_content.c_str()), 0);
//     send(socket_fd, request.getBody(), request.getBodyLength(), 0);

//     FD_CLR(socket_fd, &sockets_pool);
//     set_size--;
//     close(socket_fd);
// }

// void    Server::run()
// {
//     int     set_size = 1;
//     fd_set  sockets_pool, sockets_copy;

//     if (listen(_listen_fd, 10) == -1) 
//     { 
//         std::cerr << " webserv: listen error: " << strerror(errno) << std::endl;
//         exit(EXIT_FAILURE);
//     }

//     FD_ZERO(&sockets_pool);
//     FD_SET(_listen_fd, &sockets_pool);

//     while(true)
//     {
//         sockets_copy = sockets_pool;

//         if(select(set_size +4, &sockets_copy, NULL, NULL, NULL) < 0)
//         {
//             std::cerr << " webserv: select error" << strerror(errno) <<std::endl;
//             exit(EXIT_FAILURE);
//         }
//         for(int i = 0; i < set_size + 4; ++i)
//         {
//             if(FD_ISSET(i, &sockets_copy))
//             {
//                 if(i == _listen_fd)
//                     _acceptConnection(sockets_pool, set_size);
//                 else
//                     _handle_request(sockets_pool, set_size, i);
//             }
//         }
//     }
// }

int   Server::getFd() { return _listen_fd; }

