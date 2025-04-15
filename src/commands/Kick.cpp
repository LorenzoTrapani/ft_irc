#include "commands/Kick.hpp"
#include "ResponseMessage.hpp"
#include "Server.hpp"
#include "Logger.hpp"

Kick::Kick(Server* server) : _server(server) {}

Kick::~Kick() {}

void Kick::execute(Client* client, const std::vector<std::string>& params) {
	if (params.size() < 2) {
		ResponseMessage::sendError(client, ERR_NEEDMOREPARAMS, ":Not enough parameters");
		return;
	}

	std::string channelName = params[0];
	std::string nickname = params[1];

	Channel* channel = _server->getChannel(channelName);
	if (!channel) {
		ResponseMessage::sendError(client, ERR_NOSUCHCHANNEL, channelName + " :No such channel");
		return;
	}

	if (!channel->isInChannel(client->getSocketFd())) {
		ResponseMessage::sendError(client, ERR_NOTONCHANNEL, channelName + " :You're not on that channel");
		return;
	}

	if (!channel->isOperator(client->getSocketFd())) {
		ResponseMessage::sendError(client, ERR_CHANOPRIVSNEEDED, channelName + " :You're not a channel operator");
		return;
	}

	Client* target = _server->getClientByNick(nickname);
	if (!target) {
		ResponseMessage::sendError(client, ERR_NOSUCHNICK, nickname + " :No such nick");
		return;
	}

	if (!channel->isInChannel(target->getSocketFd())) {
		ResponseMessage::sendError(client, ERR_USERNOTINCHANNEL, channelName + " " + nickname + " :They aren't on that channel");
		return;
	}

	std::string reason = (params.size() > 2) ? params[2] : "No reason given";
	std::string kickMsg = ":" + client->getNickname() + "!" + client->getUsername() + "@" + 
                     client->getIpAddr() + " KICK " + channelName + " " + 
                     nickname + " :" + reason;

	channel->sendServerMessage(kickMsg);
	channel->removeClientFromChannel(target->getSocketFd(), client->getSocketFd(), true);	
}

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