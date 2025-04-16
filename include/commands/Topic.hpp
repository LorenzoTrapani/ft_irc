#ifndef TOPIC_HPP
#define TOPIC_HPP

#include "ICommand.hpp"
#include "../ResponseMessage.hpp"
#include "../Server.hpp"
#include "../Logger.hpp"

class Topic : public ICommand 
{
    private:
        Server* _server;

    public:
        Topic(Server* server);
        ~Topic();
        
        void execute(Client* client, const std::vector<std::string>& params);
        bool hasPermission(Client* client);
        std::string getCmdName() const;
};

#endif