#ifndef SERVERMANAGER_HPP
# define SERVERMANAGER_HPP

#include "Webserv.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include "Response.hpp"
typedef std::vector<ServerConfig> server_vec;
typedef std::vector<Client> client_vec;


class ServerManager
{
    public:
        ServerManager();
        ~ServerManager();
        void    setupServers(std::vector<ServerConfig>); // Later this function should take a list of servers to be established.
        void    runServers();
        
    private:
        /* 
        _server: store servers parased from input config file, if no config file was spefcied. default configruation is used.
        _clients: store info about all conncted clients. Socket number, address port, connected server ..
        */
        std::vector<ServerConfig> _servers;
        std::map<int, ServerConfig> _servers_map;
        client_vec _clients;
        std::map<int, Client> _clients_map;
        fd_set     _recv_fd_pool;
        int        _biggest_fd;

        void acceptNewConnection(ServerConfig &);
        void setupSelect();
        void handleRequest(int &);
        void sendResponse(int &i);
        // int  checkServer(int &);

};


#endif // SERVERMANAGER_HPP