#include "../inc/Webserv.hpp"
#include "../inc/ServerManager.hpp"

int main(int argc, char **argv) {
	if (argc == 1 || argc == 2) {
		try {
			std::string		config;
			ConfigParser	cluster;
        	ServerManager 	master;
			

			/* configuration file as argument or default path */
			config = (argc == 1 ? "default.conf" : argv[1]);
			cluster.createCluster(config);
			// cluster.print(); // for checking
			master.setupServers(cluster.getServers());
        	master.runServers();

		}
		catch (std::exception &e) {
			std::cerr << e.what() << std::endl;
			return (1);
		}
    }
    else {
		std::cout << "error: wrong arguments" << std::endl;
		return (1);
	}
    return (0);
}