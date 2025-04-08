#include "Server.hpp"

Server::Server(uint16_t port, const std::string &password) : _port(port), _password(password)
{
    if (!isValidPort(port))
    {
        throw std::invalid_argument("Invalid port number. Must be between 1024 and 65535");
    }
}

Server::~Server() {}

bool Server::isValidPort(uint16_t port)
{
    // Le porte riservate sono 0-1023
    // Le porte utente sono 1024-49151
    // Le porte dinamiche/private sono 49152-65535
    return port >= 1024 && port <= 65535;
}

void Server::run()
{
    Logger::info("Server running on port " + intToStr(_port) + " with password " + _password);
}