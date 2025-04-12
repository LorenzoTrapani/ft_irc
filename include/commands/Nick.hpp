#ifndef NICK_HPP
#define NICK_HPP

#include "ICommand.hpp"

class Nick : public ICommand {
private:
    Server* _server;

public:
    Nick(Server* server);
    ~Nick();
    
    void execute(Client* client, const std::vector<std::string>& params);
    bool hasPermission(Client* client);
    std::string getCmdName() const;
    
    bool isNicknameInUse(const std::string& nickname);
};

#endif
