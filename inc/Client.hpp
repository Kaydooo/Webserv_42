#ifndef CLIENT_HPP
# define CLIENT_HPP

#include "Webserv.hpp"
#include "HttpRequest.hpp"

class Client
{
    public:

        Client();
        ~Client();
        void setSocket(int &);
        void setAddress(sockaddr_in &);

        int getSocket();
        struct sockaddr_in getAddress();
        void    feedData(char *, size_t);
        
    private:
        int _client_socket;
        struct sockaddr_in _client_address;
        int _request_length;
        HttpRequest _request;
        
};


#endif // CLIENT_HPP