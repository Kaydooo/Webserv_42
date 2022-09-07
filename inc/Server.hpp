#ifndef SERVER_HPP
# define SERVER_HPP

#include "Webserv.hpp"

class Server
{
    public:
        Server();
        ~Server();

        void    run();

    private:
        int     _port;
        int     _listen_fd;
        int     _connection_fd;
        struct sockaddr_in _server_address;
        struct sockaddr_in _client_address;
        std::string        _request_content;
        void    _acceptConnection(fd_set&, int&);
        void    _handle_request(fd_set&, int&, int&);

};


#endif // SERVER_HPP