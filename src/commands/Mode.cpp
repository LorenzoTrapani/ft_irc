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
	processModes(client, channel, params);
}

void Mode::showChannelModes(Client* client, Channel* channel)
{
    std::string modes = channel->getModes();
    std::vector<std::string> params = channel->getParams();
    
    // Costruisci la stringa dei parametri
    std::string paramsStr;
    for (size_t i = 0; i < params.size(); i++) {
        paramsStr += params[i] + " ";
    }
    
    // Rimuovi lo spazio finale se necessario
    if (!paramsStr.empty())
        paramsStr = paramsStr.substr(0, paramsStr.size() - 1);
    
    ResponseMessage::sendNumeric(client, RPL_CHANNELMODEIS, channel->getName() + " " + modes + 
                              (paramsStr.empty() ? "" : " " + paramsStr));
}

void Mode::processModes(Client* client, Channel* channel, const std::vector<std::string>& params)
{
    bool addMode = true;//true = +, false = -
	size_t paramIndex = 2;
	std::string modes = params[1];
	int limit = 0;

    for (size_t i = 0; i < modes.length(); i++)
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
    	        handleOperatorMode(client, channel, params[paramIndex++], addMode);
    	        break;
    	    case 'k':  // password
            	if (addMode && paramIndex >= params.size()) {
            	    ResponseMessage::sendError(client, ERR_NEEDMOREPARAMS, "MODE :Not enough parameters");
            	    return;
            	}
            	handleKeyMode(client, channel, addMode ? params[paramIndex++] : "", addMode);
            	break;
			case 'i': //invite only
				handleInviteOnlyMode(client, channel, addMode);
				break;
			case 't': //topic
				handleTopicMode(client, channel, addMode);
				break;
			case 'l': //channel limit
				if (addMode && paramIndex >= params.size()) {
        			ResponseMessage::sendError(client, ERR_NEEDMOREPARAMS, "MODE :Not enough parameters");
        			return;
				}
				if (addMode) {
					limit = atoi(params[paramIndex++].c_str());
					if (limit < 0) {
						ResponseMessage::sendError(client, ERR_INVALIDMODEPARAM, "MODE :Invalid mode parameter");
						return;
					}
				}
				handleChannelLimitMode(client, channel, limit, addMode);
				break;
			default:
    	        ResponseMessage::sendError(client, ERR_UNKNOWNMODE, ":Unknown MODE");
    	        break;
		}
	}
}

void Mode::handleOperatorMode(Client* client, Channel* channel, const std::string& nick, bool adding)
{
	Client *target = _server->getClientByNick(nick);
	if (!target) {
		ResponseMessage::sendError(client, ERR_NOSUCHNICK, nick + " :No such nick");
		return;
	}

	if(!channel->isInChannel(target->getSocketFd())) {
		ResponseMessage::sendError(client, ERR_USERNOTINCHANNEL, nick + " " + channel->getName() + " :They aren't on that channel");
		return;
	}

	if(adding) {
		channel->promoteToOperator(target->getSocketFd(), client->getSocketFd());
	} else {
		channel->demoteOperator(target->getSocketFd(), client->getSocketFd());
	}
	
	std::string modeMsg = ":" + client->getNickname() + "!" + client->getUsername() + "@" + client->getIpAddr() + 
                         " MODE " + channel->getName() + " " + (adding ? "+" : "-") + "o " + nick;
    channel->sendServerMessage(modeMsg);
	Logger::debug("Operator mode set");
}

void Mode::handleKeyMode(Client* client, Channel* channel, const std::string& key, bool adding)
{
	if(adding) {
		channel->setPassword(key, client->getSocketFd());
	} else {
		channel->setPassword("", client->getSocketFd());
	}	
	
	std::string modeMsg = ":" + client->getNickname() + "!" + client->getUsername() + "@" + client->getIpAddr() + 
                         " MODE " + channel->getName() + " " + (adding ? "+" : "-") + "k" + 
                         (adding ? " " + key : "");
    channel->sendServerMessage(modeMsg);
	Logger::debug("Key mode set");
}

void Mode::handleInviteOnlyMode(Client* client, Channel* channel, bool adding)
{
	if(adding) {
		channel->setInviteOnly(true, client->getSocketFd());
	} else {
		channel->setInviteOnly(false, client->getSocketFd());
	}
	
	std::string modeMsg = ":" + client->getNickname() + "!" + client->getUsername() + "@" + client->getIpAddr() + 
                         " MODE " + channel->getName() + " " + (adding ? "+" : "-") + "i";
    channel->sendServerMessage(modeMsg);
	Logger::debug("Invite only mode set");
}

void Mode::handleTopicMode(Client* client, Channel* channel, bool adding)
{
	if(adding) {
		channel->setTopicRestricted(true, client->getSocketFd());
	} else {
		channel->setTopicRestricted(false, client->getSocketFd());
	}
	
	std::string modeMsg = ":" + client->getNickname() + "!" + client->getUsername() + "@" + client->getIpAddr() + 
                         " MODE " + channel->getName() + " " + (adding ? "+" : "-") + "t";
    channel->sendServerMessage(modeMsg);
	Logger::debug("Topic mode set");
}

void Mode::handleChannelLimitMode(Client* client, Channel* channel, int limit, bool adding)
{
	if(adding) {
		channel->setUserLimit(limit, client->getSocketFd());
	} else {
		channel->setUserLimit(0, client->getSocketFd());
	}
	
	std::string modeMsg = ":" + client->getNickname() + "!" + client->getUsername() + "@" + client->getIpAddr() + 
                         " MODE " + channel->getName() + " " + (adding ? "+" : "-") + "l" + 
                         (adding ? " " + intToStr(limit) : "");
    channel->sendServerMessage(modeMsg);
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