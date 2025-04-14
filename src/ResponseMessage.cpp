#include "ResponseMessage.hpp"
#include <sys/socket.h>
#include <sstream>
#include <iomanip>
#include "Logger.hpp"

std::string ResponseMessage::_serverName = "ft_irc";
std::string ResponseMessage::_serverVersion = "1.0";
std::string ResponseMessage::_serverCreationDate = "2025-04-20";
std::string ResponseMessage::_hostname = "localhost";

void ResponseMessage::setHostname(const std::string& hostname) {
    _hostname = hostname;
}

void ResponseMessage::sendWelcomeMsg(Client* client) {
    if (!client)
        return;
    
    if (client->isAuthenticated() && !client->getNickname().empty() && !client->getUsername().empty()) {
        std::string userhost = client->getNickname() + "!" + "-->" + client->getUsername() + "@" + client->getIpAddr();
        sendNumeric(client, RPL_WELCOME, ":Welcome to the Internet Relay Network " + userhost);
        sendNumeric(client, RPL_YOURHOST, ":Your host is " + formatPrefix().substr(1) + ", running version " + _serverVersion);
        sendNumeric(client, RPL_CREATED, ":This server, named " + _serverName + " was created " + _serverCreationDate);
        
        Logger::info("Client " + client->getIpAddr() + " (" + client->getNickname() + ") successfully authenticated");
    }
}

std::string ResponseMessage::formatPrefix() {
    return ":" + _hostname + " ";
}

std::string ResponseMessage::formatMessage(Client* client, ResponseCode code, const std::string& message) {
    std::ostringstream oss;
    
    // Aggiungi il prefisso
    oss << formatPrefix();
    
    // Aggiungi il codice numerico se è diverso da 0 (usato per PING/PONG)
    if (code != 0) {
        // Formatta il codice con zeri iniziali
        std::ostringstream codeFormat;
        codeFormat << std::setw(3) << std::setfill('0') << code;
        oss << codeFormat.str() << " ";
    }
    
    // Aggiungi il nickname se il client è autenticato
    if (client && !client->getNickname().empty()) {
        oss << client->getNickname() << " ";
    } else if (client) {
        oss << "* ";
    }
    
    // Aggiungi il messaggio
    oss << message;
    
    // Aggiungi CRLF per conformità IRC
    oss << "\r\n";
    
    return oss.str();
}

void ResponseMessage::sendNumeric(Client* client, ResponseCode code, const std::string& message) {
    if (!client)
        return;
        
    std::string formattedMessage = formatMessage(client, code, message);
    send(client->getSocketFd(), formattedMessage.c_str(), formattedMessage.length(), 0);
    
    if (code != RPL_WELCOME && code != RPL_YOURHOST && code != RPL_CREATED) {
        Logger::debug(">> " + formattedMessage);
    }
}

void ResponseMessage::sendCustom(Client* client, const std::string& message) {
    if (!client)
        return;
        
    std::string formattedMessage = message + "\r\n";
    send(client->getSocketFd(), formattedMessage.c_str(), formattedMessage.length(), 0);
    
    Logger::debug(">> " + formattedMessage);
}

void ResponseMessage::sendError(Client* client, ResponseCode code, const std::string& message) {
    sendNumeric(client, code, message);
}

void ResponseMessage::sendPong(Client* client, const std::string& token) {
    if (!client)
        return;
        
    std::string pongMessage = "PONG :" + token + "\r\n";
    send(client->getSocketFd(), pongMessage.c_str(), pongMessage.length(), 0);
    
    Logger::info(">> " + pongMessage);
} 
