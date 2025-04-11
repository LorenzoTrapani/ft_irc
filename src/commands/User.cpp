#include "commands/User.hpp"
#include "ResponseMessage.hpp"
#include "Server.hpp"
#include "Logger.hpp"

User::User(Server* server) : _server(server) {}

User::~User() {}

void User::execute(Client* client, const std::vector<std::string>& params) {
    if (!client->getUsername().empty()) {
        ResponseMessage::sendError(client, ERR_ALREADYREGISTERED, ":You may not reregister");
        return;
    }
    
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
    // Il comando USER può essere usato solo prima di aver impostato uno username
    return client->getUsername().empty();
}

std::string User::getCmdName() const {
    return "USER";
}
