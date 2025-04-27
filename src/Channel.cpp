#include "../include/Channel.hpp"
#include "../include/Server.hpp"

Channel::Channel(const std::string& name, Client* creator, Server* server)
    : _server(server),
      _name(name), 
      _topic(""), 
      _password(""), 
      _userLimit(0),
      _inviteOnly(false), 
      _topicRestricted(false)
{

    if (name.empty() || !isValidChannelName(name)) {
        throw ChannelError("Invalid channel name");
    }
    if (!creator) {
        throw ChannelError("Cannot create a channel without an initial user");
    }
    _operators.insert(creator->getSocketFd());
    _members.insert(creator->getSocketFd());
    Logger::info("Channel " + _name + " created by " + creator->getNickname());
}

Channel::~Channel()
{
    Logger::info("Channel " + _name + " has been destroyed");
    _members.clear();
    _operators.clear();
    _invited.clear();
}

bool Channel::isValidChannelName(const std::string& name) {
    if (name.empty() || name[0] != '#')
        return false;
        
    // Controlla lunghezza
    if (name.length() > 50)
        return false;
        
    // Controlla caratteri validi
    for (size_t i = 1; i < name.length(); ++i) {
        char c = name[i];
        if (c <= 32 || c == ',' || c == ':' || c == ' ')
            return false;
    }
    return true;
}

// Getters
const std::string& Channel::getName() const { return _name; }
const std::string& Channel::getTopic() const { return _topic; }
unsigned int Channel::getUserCount() const { return _members.size(); }
unsigned int Channel::getUserLimit() const { return _userLimit; }
std::string Channel::getPassword() const { return _password; }
std::set<int> Channel::getClients() const { return _members; }
std::string Channel::getModes() const {
	std::string modes;
	if (hasPassword()) modes += "k";
	if (isInviteOnly()) modes += "i";
	if (isTopicRestricted()) modes += "t";
	if (isLimited()) modes += "l";
	return modes;
}

std::vector<std::string> Channel::getParams() const {
    std::vector<std::string> paramsVector;
    
    // Aggiungere i parametri nell'ordine in cui appaiono nei flags
    std::string modes = getModes();
    
    for (size_t i = 0; i < modes.length(); i++) {
        char mode = modes[i];
        if (mode == 'k' && hasPassword()) {
            paramsVector.push_back(_password);
        } 
        else if (mode == 'l' && isLimited()) {
            paramsVector.push_back(intToStr(_userLimit));
        }
    }
    
    return paramsVector;
}


// Controlli
bool Channel::isInviteOnly() const { return _inviteOnly; }
bool Channel::isTopicRestricted() const { return _topicRestricted; }
bool Channel::isLimited() const { return _userLimit > 0; }
bool Channel::hasPassword() const { return !_password.empty(); }
bool Channel::isOperator(int clientFd) const { return _operators.find(clientFd) != _operators.end(); }
bool Channel::isInChannel(int clientFd) const { return _members.find(clientFd) != _members.end(); }
bool Channel::isInvited(int clientFd) const { return _invited.find(clientFd) != _invited.end(); }

// Setters
void Channel::setTopic(const std::string& topic, int clientFd)
{
	static const size_t MAX_TOPIC_LENGTH = 307; //307 è standard IRC
    // Solo gli operatori possono modificare il topic se è ristretto
    if (_topicRestricted && !isOperator(clientFd))
    {
        Logger::warning("Non-operator tried to change topic in channel " + _name);
        return;
    }
	if (topic.length() > MAX_TOPIC_LENGTH)
	{
		Logger::warning("Topic is too long in channel " + _name);
		return;
	}
    
    _topic = topic;
    Logger::info("Topic changed in channel " + _name + ": " + _topic);
}

void Channel::setPassword(const std::string& password, int clientFd)
{
    if (!isOperator(clientFd))
    {
        Logger::warning("Non-operator tried to change password in channel " + _name);
        return;
    }
    
    _password = password;
    Logger::info("Password " + std::string(_password.empty() ? "removed from" : "set for") + " channel " + _name);
}

void Channel::setInviteOnly(bool inviteOnly, int clientFd)
{
    if (!isOperator(clientFd))
    {
        Logger::warning("Non-operator tried to change invite-only mode in channel " + _name);
        return;
    }
    
    _inviteOnly = inviteOnly;
    Logger::info("Channel " + _name + " is now " + std::string(_inviteOnly ? "invite-only" : "open to all"));
}

void Channel::setTopicRestricted(bool restricted, int clientFd)
{
    if (!isOperator(clientFd))
    {
        Logger::warning("Non-operator tried to change topic restriction in channel " + _name);
        return;
    }
    
    _topicRestricted = restricted;
    Logger::info("Topic in channel " + _name + " can now be changed by " + 
                std::string(_topicRestricted ? "operators only" : "all members"));
}

void Channel::setUserLimit(unsigned int limit, int clientFd)
{
    if (!isOperator(clientFd))
    {
        Logger::warning("Non-operator tried to change user limit in channel " + _name);
        return;
    }

    if (limit < getUserCount())
    {
        Logger::warning("User limit cannot be set lower than the current number of users in channel " + _name);
        return;
    }
    
    _userLimit = limit;
    if (_userLimit > 0)
        Logger::info("User limit for channel " + _name + " set to " + intToStr(_userLimit));
    else
        Logger::info("User limit removed from channel " + _name);
}

Channel::JoinError Channel::addClientToChannel(Client* client, const std::string &password)
{

    if (isInviteOnly() && !isInvited(client->getSocketFd())) {
        Logger::warning("Client " + client->getIpAddr() + " tried to join invite-only channel " + _name + " but is not invited");
        return JOIN_ERR_INVITE_ONLY;
    }
    if ((hasPassword() && password != _password) && !isInvited(client->getSocketFd())) {
        Logger::warning("Client " + client->getIpAddr() + " tried to join password-protected channel " + _name + " with incorrect password");
        return JOIN_ERR_BAD_PASSWORD;
    }
    if (_userLimit > 0 && getUserCount() >= _userLimit) {
        Logger::warning("Channel " + _name + " is full (limit: " + intToStr(_userLimit) + ")");
        return JOIN_ERR_CHANNEL_FULL;
    }
    _members.insert(client->getSocketFd());
    Logger::info("Client " + client->getIpAddr() + " added to channel " + _name);
    return JOIN_SUCCESS;
}

bool Channel::removeClientFromChannel(int clientTargetFd, int clientOperatorFd, bool isKick)
{
    if (isKick && !isOperator(clientOperatorFd)) {
        Logger::warning("Non-operator tried to remove client from channel " + _name);
        return false;
    }
    _members.erase(clientTargetFd);
	if (isOperator(clientTargetFd))
        _operators.erase(clientTargetFd);
	if (_members.empty()) {
		_server->removeChannel(_name);
		return true;
	}
	if (_operators.empty() && !_members.empty()) {
		int newOperatorFd = *_members.begin();
		Client* newOperator = _server->getClient(newOperatorFd);
		if (newOperator) {
            _operators.insert(newOperator->getSocketFd());
			Logger::info("Channel " + _name + " has new operator: " + newOperator->getNickname());
        }

	}
    Client* targetClient = _server->getClient(clientTargetFd);
    if (targetClient)
        Logger::info("Client " + targetClient->getNickname() + " removed from channel " + _name);
    return true;
}

void Channel::promoteToOperator(int clientTargetFd, int clientOperatorFd)
{
    if (!isOperator(clientOperatorFd)) {
        Logger::warning("Non-operator tried to promote client to operator in channel " + _name);
        return;
    }
    Client* targetClient = _server->getClient(clientTargetFd);
    if (!targetClient) {
        Logger::warning("Client " + intToStr(clientTargetFd) + " not found in channel " + _name);
        return;
    }
    _operators.insert(clientTargetFd);
    Logger::info("Client " + targetClient->getNickname() + " promoted to operator in channel " + _name);
}

void Channel::demoteOperator(int clientTargetFd, int clientOperatorFd) 
{
    if (!isOperator(clientOperatorFd)) {
        Logger::warning("Non-operator tried to demote client from operator in channel " + _name);
        return;
    }
    Client* targetClient = _server->getClient(clientTargetFd);
    if (!targetClient) {
        Logger::warning("Client " + intToStr(clientTargetFd) + " not found in channel " + _name);
        return;
    }
    _operators.erase(clientTargetFd);
	if (_operators.empty()) {
		int newOperatorFd = *_members.begin();
		Client* newOperator = _server->getClient(newOperatorFd);
		if (newOperator) {
            _operators.insert(newOperator->getSocketFd());
			Logger::info("Channel " + _name + " has new operator: " + newOperator->getNickname());
        }

	}
    Logger::info("Client " + targetClient->getNickname() + " demoted from operator in channel " + _name);
}

void Channel::invite(int clientTargetFd, int clientOperatorFd)
{
    if (!isOperator(clientOperatorFd)) {
        Logger::warning("Non-operator tried to invite client to channel " + _name);
        return;
    }

    Client* targetClient = _server->getClient(clientTargetFd);
    if (!targetClient) {
        Logger::warning("Client " + intToStr(clientTargetFd) + " not found in channel " + _name);
        return;
    }

	if (!isInvited(clientTargetFd)) {
		_invited.insert(clientTargetFd);
	}

    Logger::info("Client " + targetClient->getNickname() + " invited to channel " + _name);
}

void Channel::sendMessage(const std::string& message, Client* sender)
{
    // Invio del messaggio da parte di un altro CLIENT a tutti i client nel canale tranne il mittente
    for (std::set<int>::iterator it = _members.begin(); it != _members.end(); ++it) {
        int clientFd = *it;
        if (clientFd != sender->getSocketFd())
            _server->sendMessageToClient(clientFd, message);
    }
}

void Channel::sendServerMessage(const std::string& message)
{
    // Invio del messaggio da parte del SERVER a tutti i client nel canale
    for (std::set<int>::iterator it = _members.begin(); it != _members.end(); ++it)
        _server->sendMessageToClient(*it, message);
}