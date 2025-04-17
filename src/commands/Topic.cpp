#include "commands/Topic.hpp"
#include "ResponseMessage.hpp"
#include "Server.hpp"
#include "Logger.hpp"

Topic::Topic(Server* server) : _server(server) {}

Topic::~Topic() {}

void Topic::execute(Client* client, const std::vector<std::string>& params)
{
    if (params.empty()) {
        ResponseMessage::sendError(client, ERR_NEEDMOREPARAMS, "TOPIC :Not enough parameters");
        return;
    }

    std::string channelName = params[0];
    
    // Verifico che il canale esista
    Channel* channel = _server->getChannel(channelName);
    if (!channel) {
        ResponseMessage::sendError(client, ERR_NOSUCHCHANNEL, channelName + " :No such channel");
        return;
    }

    // Verifico che il client sia nel canale
    if (!channel->isInChannel(client->getSocketFd())) {
        ResponseMessage::sendError(client, ERR_NOTONCHANNEL, channelName + " :You're not on that channel");
        return;
    }

    // Se non sono forniti altri parametri, mostro il topic corrente
    if (params.size() == 1) {
        std::string currentTopic = channel->getTopic();
        if (currentTopic.empty()) {
            ResponseMessage::sendNumeric(client, RPL_NOTOPIC, channelName + " :No topic is set");
        } else {
            ResponseMessage::sendNumeric(client, RPL_TOPIC, channelName + " :" + currentTopic);
        }
        return;
    }

    // Altrimenti, modifico il topic
    std::string newTopic = params[1];
    
    // Verifico i permessi
    if (channel->isTopicRestricted() && !channel->isOperator(client->getSocketFd())) {
        ResponseMessage::sendError(client, ERR_CHANOPRIVSNEEDED, channelName + " :You're not channel operator");
        return;
    }

    // Imposto il nuovo topic
    channel->setTopic(newTopic, client->getSocketFd());
    
    // Informo tutti i membri del canale (migliorabile i guess)
    std::string topicMsg = ":" + client->getNickname() + "!" + client->getUsername() + "@" + client->getIpAddr() + 
                          " TOPIC " + channelName + " :" + newTopic + "\n";
    channel->sendServerMessage(topicMsg);
}

bool Topic::hasPermission(Client* client) {
    if (!client->isAuthenticated()) {
        ResponseMessage::sendError(client, ERR_NOTREGISTERED, ":You have not registered");
        return false;
    }
    return true;
}

std::string Topic::getCmdName() const {return "TOPIC";}