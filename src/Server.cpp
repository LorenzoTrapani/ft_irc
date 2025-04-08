#include "Server.hpp"

Server::Server(const std::string &portRaw, const std::string &password)
{
    uint16_t port = static_cast<uint16_t>(std::atoi(portRaw.c_str()));
	if (port < 1 || port > 65535)
		throw std::invalid_argument("Invalid port number. Must be between 1 and 65535");
	_port = port;
	_password = password;
}

Server::~Server() {}


void Server::run()
{
    Logger::info("Server running on port " + intToStr(_port) + " with password " + _password);
	// Crea un socket TCP
	// socket()
	// Bind(socket, indirizzo, lunghezza)
	// Listen(socket, numero_max_connessioni)
	// Accept(socket, indirizzo_remoto, lunghezza_indirizzo)
	// read(socket, buffer, lunghezza_buffer)
	// Close(socket)
}