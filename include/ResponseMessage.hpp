#ifndef RESPONSEMESSAGE_HPP
#define RESPONSEMESSAGE_HPP

#include <string>
#include "Client.hpp"

// Codici di risposta IRC
enum ResponseCode {
    // Risposte informative (1xx)
    RPL_WELCOME = 001,               // :Welcome to the Internet Relay Network <nick>!<user>@<host>
    
    // Comandi riusciti (2xx)
    RPL_YOURHOST = 002,              // :Your host is <servername>, running version <ver>
    RPL_CREATED = 003,               // :This server was created <date>
    
    // Risposte dei comandi (3xx)
    RPL_ISON = 303,                  // :<nick1> <nick2> ...
    
    // Errori (4xx, 5xx)
    ERR_NOSUCHNICK = 401,            // <nickname> :No such nick/channel
    ERR_NOSUCHCHANNEL = 403,         // <channel name> :No such channel
    ERR_NOORIGIN = 409,              // :No origin specified
    ERR_NONICKNAMEGIVEN = 431,       // :No nickname given
    ERR_NICKNAMEINUSE = 433,         // <nick> :Nickname is already in use
    ERR_USERNOTINCHANNEL = 441,      // <nick> <channel> :They aren't on that channel
    ERR_NOTONCHANNEL = 442,          // <channel> :You're not on that channel
    ERR_NEEDMOREPARAMS = 461,        // <command> :Not enough parameters
    ERR_ALREADYREGISTERED = 462,     // :You may not reregister
    ERR_PASSWDMISMATCH = 464,        // :Password incorrect
    ERR_UNKNOWNCOMMAND = 421,        // <command> :Unknown command
    ERR_NOPRIVILEGES = 481,          // :Permission Denied- You're not an IRC operator
    ERR_NOTREGISTERED = 451,         // :You have not registered

    // Risposte PING/PONG
    RPL_PONG = 0                     // PONG è un comando speciale, non un codice numerico
};

class ResponseMessage {
private:
    static std::string _serverName;
    static std::string _serverVersion;
    static std::string _serverCreationDate;
    static std::string _hostname;
    
public:
    // Imposta il nome host del server
    static void setHostname(const std::string& hostname);
    
	static void sendWelcomeMsg(Client* client);
    
    // Invia una risposta numerica a un client
    static void sendNumeric(Client* client, ResponseCode code, const std::string& message);
    
    // Invia un messaggio personalizzato a un client
    static void sendCustom(Client* client, const std::string& message);
    
    // Invia un messaggio di errore
    static void sendError(Client* client, ResponseCode code, const std::string& message);
    
    // Crea un messaggio completo con prefisso server
    static std::string formatMessage(Client* client, ResponseCode code, const std::string& message);
    
    // Forma la parte iniziale di un messaggio (prefisso server)
    static std::string formatPrefix();
    
    // Invia un PING al client
    static void sendPing(Client* client, const std::string& token);
    
    // Invia un PONG in risposta a un PING
    static void sendPong(Client* client, const std::string& token);
};

#endif 
