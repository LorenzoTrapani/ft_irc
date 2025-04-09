#include "Server.hpp"


Server::Server(const std::string &portRaw, const std::string &password)
{
    uint16_t port = static_cast<uint16_t>(std::atoi(portRaw.c_str()));
	if (port == 0)
		throw InvalidArgument("Invalid port number. Must be between 1 and 65535");
	_port = port;
	if (password.empty())
		throw InvalidArgument("Password cannot be empty");
	_password = password;
    _socket = -1;
	initIpAddress();

	Logger::info("Server initialized on port:" + intToStr(_port) + " with password " + _password);
}

Server::~Server() 
{
    if (_socket != -1)
        close(_socket);
}


void Server::initIpAddress()
{
    // Crea un socket temporaneo per determinare l'IP del server
    int tempSocketFd = socket(PF_INET, SOCK_STREAM, 0);
    if (tempSocketFd <= 0) {
        throw ServerException("Failed to create socket for IP");
    }
    
    // Configura una connessione fittizia al server DNS di Google (8.8.8.8)
    // La soluzione della "connessione fittizia" funziona perché:
    // - Chiede al sistema operativo di preparare una connessione esterna reale
    // - Il sistema operativo deve selezionare l'interfaccia di rete appropriata
    // - Il sistema di routing decide quale percorso usare per raggiungere Internet
    // - Non è necessario inviare dati: La connessione viene stabilita ma non utilizzata per trasferimenti
    struct sockaddr_in tempSocketAddr;
    memset(&tempSocketAddr, 0, sizeof(tempSocketAddr));
    tempSocketAddr.sin_family = AF_INET;
    tempSocketAddr.sin_addr.s_addr = inet_addr("8.8.8.8");
    // Si utilizza Google DNS (8.8.8.8) perché:
    // - È un server sempre disponibile
    // - È un indirizzo IP ben noto e stabile
    // - La porta 53 è il servizio DNS standard
    tempSocketAddr.sin_port = htons(53);
    
    // Prova a connettersi al server DNS (non importa se fallisce)
    if (connect(tempSocketFd, (struct sockaddr*)&tempSocketAddr, sizeof(tempSocketAddr)) < 0) {
        close(tempSocketFd);
        throw ServerException("Server IP retrieval: failed to connect to 8.8.8.8");
    }
    
    // Ottiene l'indirizzo IP locale del socket
    struct sockaddr_in localAddr;
    memset(&localAddr, 0, sizeof(localAddr));
    socklen_t localAddrLen = sizeof(localAddr);
    
    if (getsockname(tempSocketFd, (struct sockaddr*)&localAddr, &localAddrLen) < 0) {
        close(tempSocketFd);
        throw ServerException("Server IP retrieval: failed to get socket name");
    }
    
    // Converte l'indirizzo IP da formato binario a stringa
    char* ipStr = inet_ntoa(localAddr.sin_addr);
    if (!ipStr) {
        close(tempSocketFd);
        throw ServerException("Failed to convert IP to string");
    }
    
    std::string host(ipStr);
    close(tempSocketFd);
	Logger::debug("Server IP: " + host);
    //ResponseMsg::setHostname(result);
}

void Server::initSocket()
{
	_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (_socket == -1)
		throw ServerException("Failed to create socket");
    
    int flags = fcntl(_socket, F_GETFL, 0);
    if (flags == -1)
        throw ServerException("Failed to get socket flags");
    if (fcntl(_socket, F_SETFL, flags | O_NONBLOCK) == -1)
        throw ServerException("Failed to set socket to non-blocking mode");
}

void Server::run()
{
    Logger::info("Server RUNNING");
    /*
    while (true)
    {
        acceptNewConnection();
        // TODO: Add select() for handling multiple clients
        // TODO: Add message handling
    }*/
}


// Getters
uint16_t Server::getPort() const { return _port; }
const std::string& Server::getPassword() const { return _password; }
