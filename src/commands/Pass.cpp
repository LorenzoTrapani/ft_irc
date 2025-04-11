#include "commands/Pass.hpp"
#include "ResponseMessage.hpp"
#include "Server.hpp"
#include "Logger.hpp"

Pass::Pass(Server* server) : _server(server) {}

Pass::~Pass() {}

void Pass::execute(Client* client, const std::vector<std::string>& params) {
    if (client->isAuthenticated()) {
        ResponseMessage::sendError(client, ERR_ALREADYREGISTERED, ":You may not reregister");
        return;
    }
    
    if (params.empty()) {
        ResponseMessage::sendError(client, ERR_NEEDMOREPARAMS, "PASS :Not enough parameters");
        return;
    }
    
    std::string password = params[0];
    
    if (password == _server->getPassword()) {
        // pwd OK ma il client non è ancora completamente autenticato
        // L'autenticazione completa avviene dopo i comandi NICK e USER
        client->setAuthenticated(true);
        Logger::info("Client " + client->getIpAddr() + " has entered the correct password");
    } else {
        ResponseMessage::sendError(client, ERR_PASSWDMISMATCH, ":Password incorrect");
        Logger::warning("Client " + client->getIpAddr() + " has entered an incorrect password");
    }
}

bool Pass::hasPermission(Client*) {
    // PASS può essere usato solo prima dell'autenticazione completa
    // ma non controlliamo qui perché lo faccio in execute
    return true;
}

std::string Pass::getCmdName() const {
    return "PASS";
}
