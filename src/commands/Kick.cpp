#include "Kick.hpp"

Kick::Kick(Server* server) : _server(server) {}

Kick::~Kick() {}

void Kick::execute(Client* client, const std::vector<std::string>& params) {} //TODO: implementare

std::string Kick::getCmdName() const {
    return "KICK";
}

bool Kick::hasPermission(Client* client) {
	if (!client->isAuthenticated()) {
        ResponseMessage::sendError(client, ERR_NOTREGISTERED, ":You have not registered");
        return false;
    }
    return true;
}