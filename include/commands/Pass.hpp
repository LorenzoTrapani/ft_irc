#ifndef PASS_HPP
#define PASS_HPP

#include "ICommand.hpp"

class Pass : public ICommand {
private:
    Server* _server;

public:
    Pass(Server* server);
    ~Pass();
    
    void execute(Client* client, const std::vector<std::string>& params);
    bool hasPermission(Client* client);
    std::string getCmdName() const;
};

#endif
