#ifndef COMMANDHANDLER_HPP
#define COMMANDHANDLER_HPP

#include <map>
#include <string>
#include <memory>
#include "commands/ICommand.hpp"
#include "Server.hpp"

class CommandHandler {
private:
    std::map<std::string, ICommand*> _commands;
    Server*	server;

public:
    CommandHandler(Server* server);
    ~CommandHandler();
    
    void registerCommand(ICommand* command);
    
    bool executeCommand(Client* client, const std::string& rawInput);
    
    Server* getServer() const;
};

#endif 
