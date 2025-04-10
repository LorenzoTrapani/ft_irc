#include "../include/Channel.hpp"

Channel::Channel(const std::string& name, Client* creator)
    : _name(name), 
      _topic(""), 
      _password(""), 
      _inviteOnly(false), 
      _topicRestricted(true), 
      _userLimit(0)
{
    // Aggiungi il creatore come membro e operatore
    if (!creator)
        throw ChannelError("Cannot create a channel without an initial user");
    _operators.insert(creator->getSocketFd());
    _members.insert(creator->getSocketFd());
    Logger::info("Channel " + _name + " created by " + creator->getIpAddr());
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
    
    _userLimit = limit;
    if (_userLimit > 0)
        Logger::info("User limit for channel " + _name + " set to " + intToStr(_userLimit));
    else
        Logger::info("User limit removed from channel " + _name);
}
