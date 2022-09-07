#ifndef SERVERMANAGER_HPP
# define SERVERMANAGER_HPP

#include "Webserv.hpp"
#include "Server.hpp"

class ServerManager
{
    public:
        ServerManager();
        ~ServerManager();
        void    setupServers(); // Later this function should take a list of servers to be established.
        void    runServers();
        
    private:
        std::vector<Server> _servers;
};


#endif // SERVERMANAGER_HPP