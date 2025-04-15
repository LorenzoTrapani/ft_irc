#ifndef KICK_HPP
#define KICK_HPP

#include "ICommand.hpp"


class Kick : public ICommand
{
    private:
        Server* _server;

    public:
        Kick(Server* server);
        ~Kick();

	
        void execute(Client* client, const std::vector<std::string>& params);
		std::string getCmdName() const;
		bool hasPermission(Client* client);

};

#endif