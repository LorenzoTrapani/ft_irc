#include "commands/Pong.hpp"
#include "ResponseMessage.hpp"
#include "Server.hpp"
#include "Logger.hpp"

Pong::Pong(Server* server) : _server(server) {}

Pong::~Pong() {}

void Pong::execute(Client* client, const std::vector<std::string>& params) {
    // Anche se PONG non richiede necessariamente una risposta, registriamo che l'abbiamo ricevuto
    if (params.empty()) {
        Logger::debug("Received empty PONG from " + client->getNickname());
        return;
    }
    
    std::string token = params[0];

    if (!token.empty() && token[0] == ':') {
        token = token.substr(1);
    }
    
    // Registra il PONG ricevuto (potrebbe essere usato per verificare la connessione)
    Logger::debug("Received PONG from " + client->getNickname() + " with token: " + token);
    
    // Per utilizzare _server (evitare l'errore di campo non utilizzato)
    if (_server) {
        // In una implementazione più completa, qui si controlla se questo PONG
        // corrisponde a un PING che abbiamo inviato in precedenza
        Logger::debug("Server port: " + intToStr(_server->getPort()));
    }
}

bool Pong::hasPermission(Client* /* client */) {
    // Chiunque può inviare un PONG
    return true;
}

std::string Pong::getCmdName() const {
    return "PONG";
} 
