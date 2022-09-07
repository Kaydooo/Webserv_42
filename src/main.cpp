#include "../inc/Webserv.hpp"
#include "../inc/ServerManager.hpp"

int main(int argc, char **argv) {
    
    if (argc == 2) {
        //parser(argv[1]);
        //start;
        (void)argv;
	//Currently we are using TEST_PORT to setup the server with no special configuration. Later, ServerManger class should take details from parser.
        ServerManager master;
	
        master.setupServers();
        master.runServers();
    }
    else {
		std::cout << "error: wrong arguments" << std::endl;
		return (1);
	}
    return (0);
}
