#include "commands/Mode.hpp"
#include "ResponseMessage.hpp"
#include "Server.hpp"
#include "Logger.hpp"

Mode::Mode(Server* server) : _server(server) {}

Mode::~Mode() {}

void Mode::execute(Client* client, const std::vector<std::string>& params)
{
    if (params.empty()) {
        ResponseMessage::sendError(client, ERR_NEEDMOREPARAMS, "MODE :Not enough parameters");
        return;
    }

    std::string target = params[0];
    
    //il target è un canale (inizia con #)
    if (target[0] != '#') {
        ResponseMessage::sendError(client, ERR_UNKNOWNMODE, target + " :is unknown, only channels are supported");
        return;
    }

    Channel* channel = _server->getChannel(target);
    if (!channel) {
        ResponseMessage::sendError(client, ERR_NOSUCHCHANNEL, target + " :No such channel");
        return;
    }
        
    // Se non ci sono altri parametri, mostra i modi attuali
    if (params.size() == 1) {
        showChannelModes(client, channel);
        return;
    }
    
    // Verifica che il client sia un operatore
    if (!channel->isOperator(client->getSocketFd())) {
        ResponseMessage::sendError(client, ERR_CHANOPRIVSNEEDED, target + " :You're not channel operator");
        return;
    }
    
    // Processa i modi
    std::string modes = params[1];
	processModes(client, channel, modes);
}

void Mode::showChannelModes(Client* client, Channel* channel)
{
    std::string modes = channel->getModes();
    std::string params = channel->getParams();
    ResponseMessage::sendNumeric(client, RPL_CHANNELMODEIS, channel->getName() + " " + modes + " " + params);
}

void Mode::processModes(Client* client, Channel* channel, const std::string& modes)
{
    bool addMode = true;//true = +, false = -
	size_t paramIdx = 2;

    for (size_t i = 0; i < modes.size(); i++)
	{
        char type = modes[i];
        if (type == '+') {
            addMode = true;
            continue;
        }
        else if (type == '-') {
            addMode = false;
            continue;
    	} else {
			ResponseMessage::sendError(client, ERR_UMODEUNKNOWNFLAG, ":Unknown MODE flag");
			return;
		}

		switch(type) {
			case 'o': //operator
				if (paramIndex >= params.size()) {
    	            ResponseMessage::sendError(client, ERR_NEEDMOREPARAMS, "MODE :Not enough parameters");
    	            return;
    	        }
    	        handleOperatorMode(channel, params[paramIndex++], adding);
    	        break;
    	    case 'k':  // password
            	if (adding && paramIndex >= params.size()) {
            	    ResponseMessage::sendError(client, ERR_NEEDMOREPARAMS, "MODE :Not enough parameters");
            	    return;
            	}
            	handleKeyMode(channel, adding ? params[paramIndex++] : "", adding);
            	break;
			case 'i': //invite only
				handleInviteOnlyMode(channel, adding);
				break;
			case 't': //topic
				handleTopicMode(channel, adding);
				break;
			case 'l': //channel limit
				if (adding && paramIndex >= params.size()) {
        			ResponseMessage::sendError(client, ERR_NEEDMOREPARAMS, "MODE :Not enough parameters");
        			return;
				}
				int limit = adding ? std::atoi(params[paramIndex++].c_str()) : 0;
				handleChannelLimitMode(channel, adding, limit);
				break;
			default:
    	        ResponseMessage::sendError(client, ERR_UMODEUNKNOWNFLAG, ":Unknown MODE flag");
    	        break;
		}
	}
}

void Mode::handleOperatorMode(Channel* channel, const std::string& nick, bool adding)
{
	Client *target = _server->getClient(nick);
	if (!target) {
		ResponseMessage::sendError(client, ERR_NOSUCHNICK, nick + " :No such nick");
		return;
	}

	if(!channel->isInChannel(target->getSocketFd())) {
		ResponseMessage::sendError(client, ERR_USERNOTINCHANNEL, nick + " " + channel->getName() + " :They aren't on that channel");
		return;
	}

	if(adding) {
		channel->addOperator(target->getSocketFd());
	} else {
		channel->removeOperator(target->getSocketFd());
	}
	Logger::debug("Operator mode set");
	// std::string modeMsg = ":" + client->getNickname() + "!" + client->getUsername() + "@" + client->getIpAddr() + " MODE " + target + " " + (adding ? "+" : "-") + "o " + targetNick;
    // channel->sendServerMessage(modeMsg);
}

void Mode::handleKeyMode(Channel* channel, const std::string& key, bool adding)
{
	if(adding) {
		channel->setPassword(key, client->getSocketFd());
	} else {
		channel->setPassword("", client->getSocketFd());
	}	
	Logger::debug("Key mode set");
}

void Mode::handleInviteOnlyMode(Channel* channel, bool adding)
{
	if(adding) {
		channel->setInviteOnly(true, client->getSocketFd());
	} else {
		channel->setInviteOnly(false, client->getSocketFd());
	}
	Logger::debug("Invite only mode set");
}

void Mode::handleTopicMode(Channel* channel, bool adding)
{
	if(adding) {
		channel->setTopicRestricted(true, client->getSocketFd());
	} else {
		channel->setTopicRestricted(false, client->getSocketFd());
	}
	Logger::debug("Topic mode set");
}

void Mode::handleChannelLimitMode(Channel* channel, int limit, bool adding)
{
	if(adding) {
		channel->setUserLimit(limit, client->getSocketFd());
	} else {
		channel->setUserLimit(0, client->getSocketFd());
	}
	Logger::debug("Channel limit mode set");
}

bool Mode::hasPermission(Client* client)
{
    if (!client->isAuthenticated()) {
		ResponseMessage::sendError(client, ERR_NOTREGISTERED, ":You have not registered");
		return false;
	}
    return true;
}

std::string Mode::getCmdName() const {return "MODE";}