#ifndef CLIENT_HPP
# define CLIENT_HPP

#include "Webserv.hpp"
#include "HttpRequest.hpp"

class Client
{
    public:
        Client();
        ~Client();

        int                 getSocket();
        struct sockaddr_in  getAddress();
        HttpRequest         &getRequest();

        void                setSocket(int &);
        void                setAddress(sockaddr_in &);

        void                feedData(char *, size_t);
        bool                requestState();
        bool                requestError();
        bool                keepAlive();
        void                clearForNextRequest();


    private:
        int                 _client_socket;
        struct sockaddr_in  _client_address;
        int                 _request_length;
        HttpRequest         _request;
};


#endif // CLIENT_HPP