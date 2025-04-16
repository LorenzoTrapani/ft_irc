#ifndef INVITE_HPP
#define INVITE_HPP

#include "ICommand.hpp"

class Invite : public ICommand 
{
    private:
        Server* _server;

    public:
        Invite(Server* server);
        ~Invite();
    
        void execute(Client* client, const std::vector<std::string>& params);
        bool hasPermission(Client* client);
        std::string getCmdName() const;
};

#endif