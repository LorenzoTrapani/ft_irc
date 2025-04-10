#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <set>
#include "Client.hpp"
#include "Logger.hpp"
#include "Utils.hpp"
#include <sys/types.h>
#include <sys/socket.h>

class Channel
{
private:
    std::string     _name;                        // Nome del canale (deve iniziare con #)
    std::string     _topic;                       // Topic/descrizione del canale
    std::string     _password;                    // Password del canale (se protetto)
    
    std::set<int>   _members;                     // Set degli FD dei client nel canale
    std::set<int>   _operators;                   // Set degli operatori (admin) del canale
    std::set<int>   _invited;                     // Set dei client invitati al canale

    bool            _inviteOnly;                  // Modalità: solo su invito
    bool            _topicRestricted;             // Modalità: topic modificabile solo dagli operatori
    unsigned int    _userLimit;                   // Limite massimo di utenti (0 = nessun limite)

public:
    Channel(const std::string& name, Client* creator);
    ~Channel();

    // Getters
    const std::string&      getName() const;
    const std::string&      getTopic() const;
    unsigned int            getUserCount() const;
    unsigned int            getUserLimit() const;
    std::string             getModes() const;
    const std::set<int>&    getMembers() const;
    
    // Setters
    void                    setTopic(const std::string& topic, int clientFd);
    void                    setPassword(const std::string& password, int clientFd);
    void                    setInviteOnly(bool inviteOnly, int clientFd);
    void                    setTopicRestricted(bool restricted, int clientFd);
    void                    setUserLimit(unsigned int limit, int clientFd);

    // Controlli
    bool                    isInviteOnly() const;
    bool                    isTopicRestricted() const;
    bool                    hasPassword() const;
    bool                    isOperator(int clientFd) const;
    bool                    isInChannel(int clientFd) const;
    bool                    isInvited(int clientFd) const;

    // Gestione utenti
    bool                    addClientToChannel(Client* client, const std::string &password);
    bool                    removeClientFromChannel(int clientTargetFd, int clientOperatorFd);
    void                    promoteToOperator(int clientTargetFd, int clientOperatorFd);
    void                    demoteOperator(int clientTargetFd, int clientOperatorFd);
    void                    invite(int clientTargetFd, int clientOperatorFd);
    
    // Messaggi
    void                    sendMessage(const std::string& message, Client* sender);
    void                    sendServerMessage(const std::string& message);

    class ChannelError : public std::runtime_error {
        public:
            ChannelError(const std::string& message) : std::runtime_error(message) {}
    };
};

#endif
