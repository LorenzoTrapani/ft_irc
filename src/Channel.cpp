#include "../include/Channel.hpp"
#include "../include/Server.hpp"

Channel::Channel(const std::string& name, Client* creator, Server* server)
    : _server(server),
      _name(name), 
      _topic(""), 
      _password(""), 
      _inviteOnly(false), 
      _topicRestricted(true), 
      _userLimit(0)
{
     if (name.empty() || name[0] != '#')
        throw ChannelError("Channel name must start with #");
    if (!creator)
        throw ChannelError("Cannot create a channel without an initial user");
    _operators.insert(creator->getSocketFd());
    _members.insert(creator->getSocketFd());
    Logger::info("Channel " + _name + " created by " + creator->getNickname());
}

Channel::~Channel()
{
    Logger::info("Channel " + _name + " destroyed");
    _members.clear();
    _operators.clear();
    _invited.clear();
}

// Getters
const std::string& Channel::getName() const { return _name; }
const std::string& Channel::getTopic() const { return _topic; }
unsigned int Channel::getUserCount() const { return _members.size(); }
unsigned int Channel::getUserLimit() const { return _userLimit; }
std::string Channel::getPassword() const { return _password; }
const std::set<int>& Channel::getMembers() const {return _members;}

bool Channel::isInviteOnly() const { return _inviteOnly; }
bool Channel::isTopicRestricted() const { return _topicRestricted; }
bool Channel::hasPassword() const { return !_password.empty(); }
bool Channel::isOperator(int clientFd) const { return _operators.find(clientFd) != _operators.end(); }
bool Channel::isInChannel(int clientFd) const { return _members.find(clientFd) != _members.end(); }
bool Channel::isInvited(int clientFd) const { return _invited.find(clientFd) != _invited.end(); }

// Setters
void Channel::setTopic(const std::string& topic, int clientFd)
{
    // Solo gli operatori possono modificare il topic se è ristretto
    if (_topicRestricted && !isOperator(clientFd))
    {
        Logger::warning("Non-operator tried to change topic in channel " + _name);
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

bool Channel::addClientToChannel(Client* client, const std::string &password)
{
    if (isInviteOnly() && !isInvited(client->getSocketFd())) {
        Logger::warning("Client " + client->getIpAddr() + " tried to join invite-only channel " + _name + " but is not invited");
        return false;
    }
    if (hasPassword() && password != _password) {
        Logger::warning("Client " + client->getIpAddr() + " tried to join password-protected channel " + _name + " with incorrect password");
        return false;
    }
    if (_userLimit > 0 && getUserCount() >= _userLimit) {
        Logger::warning("Channel " + _name + " is full (limit: " + intToStr(_userLimit) + ")");
        return false;
    }
    _members.insert(client->getSocketFd());
    Logger::info("Client " + client->getIpAddr() + " added to channel " + _name);
    return true;
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
		_operators.insert(newOperatorFd);
		Logger::info("Channel " + _name + " has new operator: " + _server->getClient(newOperatorFd)->getNickname());
	}
    Logger::info("Client " + _server->getClient(clientTargetFd)->getNickname() + " removed from channel " + _name);
    return true;
}

void Channel::promoteToOperator(int clientTargetFd, int clientOperatorFd)
{
    if (!isOperator(clientOperatorFd)) {
        Logger::warning("Non-operator tried to promote client to operator in channel " + _name);
        return;
    }
    _operators.insert(clientTargetFd);
    Logger::info("Client " + _server->getClient(clientTargetFd)->getNickname() + " promoted to operator in channel " + _name);
}

void Channel::demoteOperator(int clientTargetFd, int clientOperatorFd) 
{
    if (!isOperator(clientOperatorFd)) {
        Logger::warning("Non-operator tried to demote client from operator in channel " + _name);
        return;
    }
    _operators.erase(clientTargetFd);
    Logger::info("Client " + _server->getClient(clientTargetFd)->getNickname() + " demoted from operator in channel " + _name);
}

void Channel::invite(int clientTargetFd, int clientOperatorFd)
{
    if (!isOperator(clientOperatorFd)) {
        Logger::warning("Non-operator tried to invite client to channel " + _name);
        return;
    }
    _invited.insert(clientTargetFd);
    Logger::info("Client " + _server->getClient(clientTargetFd)->getNickname() + " invited to channel " + _name);
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

std::string Channel::getModes() const
{
    std::string modes;
    if (_inviteOnly) modes += "i";
    if (_topicRestricted) modes += "t";
    if (_userLimit > 0) modes += "l";
    return modes;
}