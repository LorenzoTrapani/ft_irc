#include "commands/PrivMsg.hpp"
#include "ResponseMessage.hpp"
#include "Server.hpp"
#include "Logger.hpp"

Privmsg::Privmsg(Server* server) : _server(server) {}

Privmsg::~Privmsg() {}

void Privmsg::execute(Client* sender, const std::vector<std::string>& params)
{
    if (params.empty() || params.size() != 2) {
        ResponseMessage::sendError(sender, ERR_NEEDMOREPARAMS, "PRIVMSG :Wrong number of parameters");
        return;
    }

    std::string target = params[0];
    std::string message = params[1];

	// Controlli base
    if (target.empty()) {
        ResponseMessage::sendError(sender, ERR_NORECIPIENT, ":No recipient given (PRIVMSG)");
        return;
    }

    if (message.empty()) {
        ResponseMessage::sendError(sender, ERR_NOTEXTTOSEND, ":No text to send");
        return;
    }

	// Formatta il messaggio completo con i dettagli del mittente
    std::string prefix = ":" + sender->getNickname() + "!" + 
                        sender->getUsername() + "@" + 
                        sender->getIpAddr();
    std::string fullMsg = prefix + " PRIVMSG " + target + " :" + message + "\n";


    // Se il target inizia con #, è un messaggio al canale
    if (target[0] == '#') {
        Channel* channel = _server->getChannel(target);
        if (!channel) {
            ResponseMessage::sendError(sender, ERR_NOSUCHCHANNEL, target + " :No such channel");
            return;
        }
        
        // Verifica che il sender sia nel canale
        if (!channel->isInChannel(sender->getSocketFd())) {
            ResponseMessage::sendError(sender, ERR_CANNOTSENDTOCHAN, target + " :Cannot send to channel");
            return;
        }
        // Invia il messaggio al canale
        channel->sendMessage(fullMsg, sender);
    }
    else {
        // Invia il messaggio privato all'utente
        Client* privateUser = _server->getClientByNick(target);
        if (!privateUser) {
            ResponseMessage::sendError(sender, ERR_NOSUCHNICK, target + " :No such nick");
            return;
        }
        _server->sendMessageToClient(privateUser->getSocketFd(), fullMsg);
    }
}

bool Privmsg::hasPermission(Client* client) {
    if (!client->isAuthenticated()) {
        ResponseMessage::sendError(client, ERR_NOTREGISTERED, ":You have not registered");
        return false;
    }
    return true;
}

std::string Privmsg::getCmdName() const {return "PRIVMSG";}
