#ifndef USER_HPP
#define USER_HPP

#include "ICommand.hpp"

class User : public ICommand {
private:
    Server* _server;

public:
    User(Server* server);
    ~User();
    
    void execute(Client* client, const std::vector<std::string>& params);
    bool hasPermission(Client* client);
    std::string getCmdName() const;
};

#endif
