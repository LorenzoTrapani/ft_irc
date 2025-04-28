#include "commands/Invite.hpp"
#include "ResponseMessage.hpp"
#include "Server.hpp"
#include "Logger.hpp"

Invite::Invite(Server* server) : _server(server) {}

Invite::~Invite() {}

// INVITE <nickname> <channel>

void Invite::execute(Client* client, const std::vector<std::string>& params)
{
    if (params.empty() || params.size() != 2) {
        ResponseMessage::sendError(client, ERR_NEEDMOREPARAMS, "INVITE :Wrong number of parameters");
        return;
    }

    std::string targetNick = params[0];
    std::string channelName = params[1];

    // Verifica che il client di destinazione esista
    Client* targetClient = _server->getClientByNick(targetNick);
    if (!targetClient) {
        ResponseMessage::sendError(client, ERR_NOSUCHNICK, targetNick + " :No such nick");
        return;
    }

    // Verifica che il canale esista
    Channel* channel = _server->getChannel(channelName);
    if (!channel) {
        ResponseMessage::sendError(client, ERR_NOSUCHCHANNEL, channelName + " :No such channel");
        return;
    }

    // Verifica che l'invitante sia nel canale
    if (!channel->isInChannel(client->getSocketFd())) {
        ResponseMessage::sendError(client, ERR_NOTONCHANNEL, channelName + " :You're not on that channel");
        return;
    }

    // Verifica che l'utente di destinazione non sia già nel canale
    if (channel->isInChannel(targetClient->getSocketFd())) {
        ResponseMessage::sendError(client, ERR_USERNOTINCHANNEL, targetNick + " " + channelName + " :User is already on that channel");
        return;
    }

    // Invita l'utente al canale
    channel->invite(targetClient->getSocketFd(), client->getSocketFd());

    // Invia conferma all'invitante
    ResponseMessage::sendNumeric(client, RPL_INVITING, targetNick + " " + channelName);

    // Invia notifica all'invitato
    std::string inviteMsg = ":" + client->getNickname() + "!" + client->getUsername() + "@" + client->getIpAddr() + 
                          " INVITE " + targetNick + " :" + channelName + "\n";
    _server->sendMessageToClient(targetClient->getSocketFd(), inviteMsg);
}

bool Invite::hasPermission(Client* client) {
    if (!client->isAuthenticated()) {
        ResponseMessage::sendError(client, ERR_NOTREGISTERED, ":You have not registered");
        return false;
    }
    return true;
}

std::string Invite::getCmdName() const {return "INVITE";}