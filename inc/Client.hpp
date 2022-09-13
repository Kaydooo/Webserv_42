#ifndef CLIENT_HPP
# define CLIENT_HPP

#include "Webserv.hpp"

class Client
{
    public:

        Client();
        ~Client();
        void setSocket(int &);
        void setAddress(sockaddr_in &);

        int getSocket();
        struct sockaddr_in getAddress();
        
    private:
        int _client_socket;
        struct sockaddr_in _client_address;
        
};


#endif // CLIENT_HPP