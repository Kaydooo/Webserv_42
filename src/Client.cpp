# include "../inc/Client.hpp"

Client::Client() {}

Client::~Client() {}

void    Client::setSocket(int &sock){ _client_socket = sock; }

void    Client::setAddress(sockaddr_in &addr){ _client_address =  addr;}

int    Client::getSocket(){ return (_client_socket); }

struct sockaddr_in    Client::getAddress(){ return (_client_address); }