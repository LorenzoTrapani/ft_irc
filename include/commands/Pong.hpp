#ifndef PONG_HPP
#define PONG_HPP

#include "ICommand.hpp"

class Pong : public ICommand {
private:
    Server* _server;

public:
    Pong(Server* server);
    ~Pong();
    
    void execute(Client* client, const std::vector<std::string>& params);
    bool hasPermission(Client* client);
    std::string getCmdName() const;
};

#endif 
