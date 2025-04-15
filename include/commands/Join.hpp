#ifndef JOIN_HPP
#define JOIN_HPP

#include "ICommand.hpp"

class Join : public ICommand
{
    private:
        Server* _server;

    public:
        Join(Server* server);
        ~Join();

        void execute(Client* client, const std::vector<std::string>& params);
		std::string getCmdName() const;
		bool hasPermission(Client* client);

};

#endif
