#ifndef ICOMMAND_HPP
#define ICOMMAND_HPP

#include <string>
#include <vector>
#include "../Client.hpp"
#include "../Server.hpp"

class ICommand {
public:
    virtual ~ICommand() {}

    virtual void execute(Client* client, const std::vector<std::string>& params) = 0;
    
    virtual bool hasPermission(Client* client) = 0;
    
    virtual std::string getCmdName() const = 0;
};

#endif 
