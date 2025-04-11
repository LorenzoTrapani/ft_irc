#include "commands/Ping.hpp"
#include "ResponseMessage.hpp"
#include "Server.hpp"
#include "Logger.hpp"

Ping::Ping(Server* server) : _server(server) {}

Ping::~Ping() {}

void Ping::execute(Client* client, const std::vector<std::string>& params) {
    if (params.empty()) {
        ResponseMessage::sendError(client, ERR_NOORIGIN, ":No origin specified");
        return;
    }
    
    std::string token = params[0];
    
    if (!token.empty() && token[0] == ':') {
        token = token.substr(1);
    }
    
    if (_server) {
        Logger::debug("Processing PING from client on server port: " + intToStr(_server->getPort()));
    }
    
    ResponseMessage::sendPong(client, token);
    
    Logger::debug("Received PING from " + client->getNickname() + ", replied with PONG");
}

bool Ping::hasPermission(Client* /* client */) {
    // Chiunque può inviare un PING
    return true;
}

std::string Ping::getCmdName() const {
    return "PING";
}
