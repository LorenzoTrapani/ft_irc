#include "commands/Join.hpp"
#include "ResponseMessage.hpp"
#include "Server.hpp"
#include "Logger.hpp"

Join::Join(Server* server) : _server(server) {}

Join::~Join() {}

void Join::execute(Client* client, const std::vector<std::string>& params)
{
    if (params.empty() || (params.size() != 2 && params.size() != 1))
    {
        ResponseMessage::sendError(client, ERR_NEEDMOREPARAMS, "JOIN :Wrong number of parameters");
        return;
    }

	std::string channelName = params[0];
    std::string password = (params.size() > 1) ? params[1] : "";

    // Verifica che il nome del canale inizi con #
    if (channelName[0] != '#') {
        ResponseMessage::sendError(client, ERR_NOSUCHCHANNEL, channelName + " :No such channel");
		Logger::error("Client " + client->getNickname() + " tried to join channel " + channelName + " but it doesn't start with #");
        return;
    }
	
	Channel* channel = _server->getChannel(channelName);
	// Se il canale non esiste lo creo
    if (!channel) {
        try {
            channel = new Channel(channelName, client, _server);
            _server->addChannel(channelName, channel);

			//check password
			if (!password.empty()) {
				channel->setPassword(password, client->getSocketFd());
			}
            
            // Invia messaggio di join al client
            std::string joinMsg = ":" + client->getNickname() + "!" + client->getUsername() + "@" + client->getIpAddr() + " JOIN " + channelName + "\n";
            channel->sendServerMessage(joinMsg);
            return;
        } catch (const Channel::ChannelError& e) {
            ResponseMessage::sendError(client, ERR_NOSUCHCHANNEL, channelName + " :" + e.what());
            return;
        }
    }

	if (channel->isInChannel(client->getSocketFd())) {
		ResponseMessage::sendError(client, ERR_USERONCHANNEL, client->getNickname() + " " + channelName + " :is already on channel");
		return;
	}

    Channel::JoinError result = channel->addClientToChannel(client, password);
    if (result == Channel::JOIN_SUCCESS) {
        // Invia messaggio di join a tutti i membri del canale
		std::string joinMsg = ":" + client->getNickname() + "!" + client->getUsername() + "@" + client->getIpAddr() + " JOIN :" + channelName + "\n";
        channel->sendServerMessage(joinMsg);
        
        // Invia il topic se esiste
        if (!channel->getTopic().empty()) {
            ResponseMessage::sendNumeric(client, RPL_TOPIC, channelName + " :" + channel->getTopic());
        }
    } else {
        switch(result) {
            case Channel::JOIN_ERR_INVITE_ONLY:
                ResponseMessage::sendError(client, ERR_INVITEONLYCHAN, channelName + " :Cannot join channel (+i)");
                break;
            case Channel::JOIN_ERR_BAD_PASSWORD:
                ResponseMessage::sendError(client, ERR_BADCHANNELKEY, channelName + " :Cannot join channel (+k)");
                break;
            case Channel::JOIN_ERR_CHANNEL_FULL:
                ResponseMessage::sendError(client, ERR_CHANNELISFULL, channelName + " :Cannot join channel (+l)");
                break;
			case Channel::JOIN_SUCCESS: //aggiunto per evitare warning
				break;
        }
    }
}

bool Join::hasPermission(Client* client)
{
	if (!client->isAuthenticated()) {
		ResponseMessage::sendError(client, ERR_NOTREGISTERED, ":You have not registered");
		return false;
	}
    return true;
}

std::string Join::getCmdName() const {return "JOIN";}
