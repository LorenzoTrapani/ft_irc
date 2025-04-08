#include "Server.hpp"

Server::Server(const std::string &port, const std::string &password) : _port(port), _password(password) {}

Server::~Server() {}

void Server::run()
{
    std::cout << "Server running on port " << _port << " with password " << _password << std::endl;
}