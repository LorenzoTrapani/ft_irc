#include "Client.hpp"

Client::Client() : _socketFd(-1), _ipAddr(""), _nickname(""), _username(""), _inputBuffer(""), _authenticated(false) {}

Client::~Client() {}

// Getters
int Client::getSocketFd() const { return _socketFd; }
const std::string& Client::getIpAddr() const { return _ipAddr; }
const std::string& Client::getNickname() const { return _nickname; }
const std::string& Client::getUsername() const { return _username; }
const std::string& Client::getRealname() const { return _realname; }
bool Client::isAuthenticated() const { return _authenticated; }

// Setters
void Client::setSocketFd(int socketFd) { _socketFd = socketFd; }
void Client::setIpAddr(const std::string& ipAddr) { _ipAddr = ipAddr; }
void Client::setNickname(const std::string& nickname) { _nickname = nickname; }
void Client::setUsername(const std::string& username) { _username = username; }
void Client::setAuthenticated(bool authenticated) { _authenticated = authenticated; }
void Client::setRealname(const std::string& realname) { _realname = realname; }
// Buffer management
void Client::appendToBuffer(const std::string& data) {
    _inputBuffer += data;
}

std::vector<std::string> Client::extractCommands() {
    std::vector<std::string> commands;
    
    size_t pos;
    // Cerca sia \r\n (standard IRC) che \n (per client come netcat)
    while ((pos = _inputBuffer.find("\r\n")) != std::string::npos || 
           (pos = _inputBuffer.find("\n")) != std::string::npos) {
        
        // Estrai il comando fino al delimitatore
        std::string cmd = _inputBuffer.substr(0, pos);
        
        // Rimuovi il comando e il delimitatore dal buffer
        if (_inputBuffer[pos] == '\r')
            _inputBuffer.erase(0, pos + 2); // Rimuovi \r\n
        else
            _inputBuffer.erase(0, pos + 1); // Rimuovi \n
            
        commands.push_back(cmd);
    }

    return commands;
}





