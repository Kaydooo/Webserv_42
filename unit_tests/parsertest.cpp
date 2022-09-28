#include "../inc/HttpRequest.hpp"

#include <sys/socket.h> // socket, listen
#include <netinet/in.h> // sockaddr  struct
#include <arpa/inet.h> // htonl
#include <cstring>
#include <string>  
#include <fcntl.h>
#include <unistd.h>
#include <iostream>

#define PORT 9221
int main(int argc, char **argv)
{
    struct sockaddr_in serverAddress;
    int webserv_fd;
    int accept_socket;
    size_t read_size;
    long    address_size;
    char    result_buffer[1024];

    int option_value = 1;
    if((webserv_fd = socket(AF_INET, SOCK_STREAM, 0) )  == -1 )
    {
        std::cerr << " SCOCKET ERROR ! " << std::endl;
        return (1);
    }

    setsockopt(webserv_fd, SOL_SOCKET, SO_REUSEADDR, &option_value, sizeof(int));

    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddress.sin_port = htons(PORT);

    if (bind(webserv_fd, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) == -1)
    {
        std::cerr << " BIND ERROR22 ! " << strerror(errno) << std::endl;
        return (1);
    }

    if (listen(webserv_fd, 3) < 0) 
    { 
        std::cerr << " LISTEN ERROR ! " << std::endl;
        return (1); 
    }

    HttpRequest request;
    struct sockaddr_in client_address;
    long  client_address_size = sizeof(client_address);
    if ((accept_socket = accept(webserv_fd, (struct sockaddr *)&client_address, (socklen_t*)&client_address_size)) == -1)
    {
        std::cerr << " ACCEPT ERROR ! " << strerror(errno) << std::endl;
        return (1);      
    }
    while(1)
    {
        read_size = read(accept_socket, result_buffer, sizeof(result_buffer));
        // std::cout << result_buffer << std:: endl;
        // std::cout << read_size << std::endl;
        if(read_size > 0)
        {   
            request.feed(result_buffer, read_size);
            memset(result_buffer, 0, sizeof(result_buffer));
            request.printMessage();
        }
        if(request.parsingCompleted())
        {
            close(accept_socket);
            std::cout << "Parsing completed !" << std::endl;
            sleep(100);
        }
    }
}