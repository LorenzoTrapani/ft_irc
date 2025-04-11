#include "CommandHandler.hpp"
#include "Logger.hpp"
#include <sstream>
#include <algorithm>

CommandHandler::CommandHandler(Server* server) : _server(server) {}

CommandHandler::~CommandHandler() {
    for (std::map<std::string, ICommand*>::iterator it = _commands.begin(); it != _commands.end(); ++it) {
        delete it->second;
    }
    _commands.clear();
}

void CommandHandler::registerCommand(ICommand* command) {
    std::string cmdName = command->getCmdName();
    std::transform(cmdName.begin(), cmdName.end(), cmdName.begin(), ::toupper);
    _commands[cmdName] = command;
}

bool CommandHandler::executeCommand(Client* client, const std::string& rawInput) {
    if (rawInput.empty()) {
        return false;
    }
    
    std::istringstream iss(rawInput);
    std::string commandName;
    
    // Se il comando inizia con ':', salta il prefisso
    if (rawInput[0] == ':') {
        std::string prefix;
        iss >> prefix;
        iss >> commandName;
    } else {
        iss >> commandName;
    }
    
    std::transform(commandName.begin(), commandName.end(), commandName.begin(), ::toupper);
    
    // cerco il comando nella mappa
    std::map<std::string, ICommand*>::iterator cmdIt = _commands.find(commandName);
    if (cmdIt == _commands.end()) {
        Logger::debug("Command not found: " + commandName);
        // TODO: Invia errore 421 (comando sconosciuto) al client
        return false;
    }
    
    // Estraggo i parametri
    std::vector<std::string> params;
    std::string param;
    std::string trailing = "";
    
    // Controllo se c'è un parametro trailing (indicato da ':')
    size_t trailingPos = rawInput.find(" :", iss.tellg());
    if (trailingPos != std::string::npos) {
        // Estraggo i parametri prima del trailing
        while (iss.tellg() < static_cast<std::streampos>(trailingPos) && iss >> param) {
            params.push_back(param);
        }
        
        // Estraggo il parametro trailing
        trailing = rawInput.substr(trailingPos + 2); // +2 per saltare lo spazio e il ':'
        params.push_back(trailing);
    } else {
        // Nessun parametro trailing, estraggo tutti i parametri normali
        while (iss >> param) {
            params.push_back(param);
        }
    }
    
    // Verifico i permessi
    if (!cmdIt->second->hasPermission(client)) {
        Logger::debug("Client does not have permission to execute command: " + commandName);
        // TODO: Invia errore appropriato al client
        return false;
    }
    
    // Eseguo il comando
    Logger::debug("Executing command: " + commandName);
    cmdIt->second->execute(client, params);
    
    return true;
}

Server* CommandHandler::getServer() const {
    return _server;
} 
