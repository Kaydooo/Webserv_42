#ifndef SERVERMANAGER_HPP
# define SERVERMANAGER_HPP

#include "Webserv.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include  "RequestHandler.hpp"
typedef std::vector<Server> server_vec;
typedef std::vector<Client> client_vec;


class ServerManager
{
    public:
        ServerManager();
        ~ServerManager();
        void    setupServers(); // Later this function should take a list of servers to be established.
        void    runServers();
        
    private:
        /* 
        _server: store servers parased from input config file, if no config file was spefcied. default configruation is used.
        _clients: store info about all conncted clients. Socket number, address port, connected server ..
        */
        server_vec _servers;
        client_vec _clients;
        fd_set     _recv_fd_pool;

        void acceptNewConnection(Server &);
        void setupSelect(int &);
        void handle_request(int &, int &set_size);
        int  checkServer(int &);

};


#endif // SERVERMANAGER_HPP