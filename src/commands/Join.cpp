#include "commands/Join.hpp"
#include "ResponseMessage.hpp"
#include "Server.hpp"
#include "Logger.hpp"

Join::Join(Server* server) : _server(server) {}

Join::~Join() {}

void Join::execute(Client* client, const std::vector<std::string>& params)
{
    if (params.empty())
    {
        ResponseMessage::sendError(client, ERR_NEEDMOREPARAMS, ":Not enough parameters");
        return;
    }

	std::string channelName = params[0];
    std::string password = (params.size() > 1) ? params[1] : "";

    // Verifica che il nome del canale inizi con #
    if (channelName[0] != '#') {
        ResponseMessage::sendError(client, ERR_NOSUCHCHANNEL, channelName + " :No such channel");
        return;
    }
	
	Channel* channel = _server->getChannel(channelName);
	// Se il canale non esistesi crea
    if (!channel) {
        try {
            channel = new Channel(channelName, client, _server);
            _server->addChannel(channelName, channel);
            
            // Invia messaggio di join al client
            std::string joinMsg = ":" + client->getNickname() + "!" + client->getUsername() + "@" + client->getIpAddr() + " JOIN " + channelName;
            // channel->sendServerMessage(joinMsg);
            return;
        } catch (const Channel::ChannelError& e) {
            ResponseMessage::sendError(client, ERR_NOSUCHCHANNEL, channelName + " :" + e.what());
            return;
        }
    }

    if (channel->addClientToChannel(client, password)) {
        // Invia messaggio di join a tutti i membri del canale
        std::string joinMsg = ":" + client->getNickname() + "!" + client->getUsername() + "@" + client->getIpAddr() + " JOIN " + channelName;
        channel->sendServerMessage(joinMsg);
        
        // Invia il topic se esiste
        if (!channel->getTopic().empty()) {
            ResponseMessage::sendNumeric(client, RPL_TOPIC, channelName + " :" + channel->getTopic());
        }
    } else {
        // Gestisci i vari casi di errore
        if (channel->isInviteOnly() && !channel->isInvited(client->getSocketFd())) {
            ResponseMessage::sendError(client, ERR_INVITEONLYCHAN, channelName + " :Cannot join channel (+i)");
        } else if (channel->hasPassword() && password != channel->getPassword()) {
            ResponseMessage::sendError(client, ERR_BADCHANNELKEY, channelName + " :Cannot join channel (+k)");
        } else if (channel->getUserLimit() > 0 && channel->getUserCount() >= channel->getUserLimit()) {
            ResponseMessage::sendError(client, ERR_CHANNELISFULL, channelName + " :Cannot join channel (+l)");
        }
    }
}

std::string Join::getCmdName() const {
    return "JOIN";
}

bool Join::hasPermission(Client* client)
{
	if (!client->isAuthenticated()) {
		ResponseMessage::sendError(client, ERR_NOTREGISTERED, ":You have not registered");
		return false;
	}
    return true;
}
