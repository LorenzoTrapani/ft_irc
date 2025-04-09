#include "Client.hpp"

Client::Client()
{
	_socketFd = 0;
	_ipAddr = "";
}

Client::~Client() {}

void Client::setSocketFd(int socketFd) {_socketFd = socketFd;}

int Client::getSocketFd() {return _socketFd;}

void Client::setIpAddr(std::string ipAddr) {_ipAddr = ipAddr;}

std::string Client::getIpAddr() {return _ipAddr;}





