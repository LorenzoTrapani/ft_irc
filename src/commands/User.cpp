#include "commands/User.hpp"
#include "ResponseMessage.hpp"
#include "Server.hpp"
#include "Logger.hpp"

User::User(Server* server) : _server(server) {}

User::~User() {}

void User::execute(Client* client, const std::vector<std::string>& params) {
    // Verifica che ci siano abbastanza parametri (USER username mode unused :realname)
    if (params.size() < 4) {
        ResponseMessage::sendError(client, ERR_NEEDMOREPARAMS, "USER :Not enough parameters");
        return;
    }
    
    std::string username = params[0];
    std::string realname = params[3];
    
    client->setUsername(username);
    client->setRealname(realname);
    
    Logger::info("Client " + client->getIpAddr() + " set username to " + username);
    
    // Verifica se il client ha completato l'autenticazione (ha sia nickname che username e password corretta)
    if (client->isAuthenticated() && !client->getNickname().empty()) {
        ResponseMessage::sendWelcomeMsg(client);
		Logger::debug("User authenticated on server port: " + intToStr(_server->getPort()));
    }
}

bool User::hasPermission(Client* client) {
    // Verifica che il client sia autenticato (PASS eseguito)
    if (!client->isAuthenticated()) {
        ResponseMessage::sendError(client, ERR_NOTREGISTERED, ":You have not registered");
        return false;
    }

    // Verifica che NICK sia stato impostato prima di USER
    if (client->getNickname().empty()) {
        ResponseMessage::sendError(client, ERR_NONICKNAMEGIVEN, ":No nickname given. Use NICK first");
        return false;
    }
    
    // Verifica che lo username non sia già impostato
    if (!client->getUsername().empty()) {
        ResponseMessage::sendError(client, ERR_ALREADYREGISTERED, ":You may not reregister");
        return false;
    }
    
    return true;
}

std::string User::getCmdName() const {
    return "USER";
}
