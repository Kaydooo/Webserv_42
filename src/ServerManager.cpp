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
    std::vector<Server>::iterator it;
    for(it = _servers.begin(); it != _servers.end(); ++it)
        it->run();
}
