#ifndef PRIVMSG_HPP
#define PRIVMSG_HPP

#include "ICommand.hpp"

class Privmsg : public ICommand {
private:
    Server* _server;
	
public:
    Privmsg(Server* server);
    ~Privmsg();

    void execute(Client* sender, const std::vector<std::string>& params);
    std::string getCmdName() const;
    bool hasPermission(Client* client);
};

#endif