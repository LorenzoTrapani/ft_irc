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

	Logger::debug("Server initialized on port:" + intToStr(_port) + " with password " + _password);
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
    
	//TODO: Controllare se e' da modificare questo ultimo pezzo o se e' giusto cosi'
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

    int optval = 1;
	if (setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval))!=0) {
		throw ServerException("Failed to set socket option");
	}
    
    int flags = fcntl(_socket, F_GETFL, 0);
    if (flags == -1)
        throw ServerException("Failed to get socket flags");
    if (fcntl(_socket, F_SETFL, flags | O_NONBLOCK) == -1)
        throw ServerException("Failed to set socket to non-blocking mode");
}

void Server::removeClient(int socketFd)
{
    if (!_clients.count(socketFd))
        return;

    // Find and remove the client from the vector
    std::map<int, Client*>::iterator it = _clients.find(socketFd);
    if (it != _clients.end()) {
        _clients.erase(it);
        close(socketFd);
    }
}

void Server::run()
{
    
    initSocket();
	bindSocket();
	listenForConnections();
    Logger::info("Server RUNNING");
	handleConnections();
}

void Server::bindSocket()
{
    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(_port);

    if (bind(_socket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        throw ServerException("Failed to bind socket: Port " + intToStr(_port) + " is already in use");
    }
	Logger::info("Socket bound to port " + intToStr(_port));
}

void Server::listenForConnections()
{
	if (listen(_socket, 5) < 0)
		throw ServerException("Failed to listen for connections");
	Logger::info("Listening for connections on port " + intToStr(_port));
}

void Server::handleConnections()
{
	fd_set readFds, writeFds;
	int maxFd = _socket;

	while (420)
	{
		// Prepara i set per select()
		FD_ZERO(&readFds);
		FD_ZERO(&writeFds);
		
		// Aggiungi il socket principale al set di lettura
		FD_SET(_socket, &readFds);
		
		// Aggiungi tutti i socket client ai set di lettura e scrittura
		for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
		{
			int clientFd = it->first;
			FD_SET(clientFd, &readFds);
			
			// Se ci sono dati da inviare al client, aggiungi al set di scrittura
			// Per ora lo aggiungiamo sempre, successivamente verrà aggiunto solo 
			// quando ci sono effettivamente dati da inviare
			FD_SET(clientFd, &writeFds);
			
			// Aggiorna maxFd se necessario
			if (clientFd > maxFd)
				maxFd = clientFd;
		}
		
		// Imposta timeout a NULL per bloccare indefinitamente
		// select() ritornerà solo quando c'è attività su uno dei socket
		struct timeval timeout;
		timeout.tv_sec = 1;  // 1 secondo di timeout per permettere controlli periodici
		timeout.tv_usec = 0;
		
		int activity = select(maxFd + 1, &readFds, &writeFds, NULL, &timeout);
		
		if (activity < 0)
		{
			// Gestisci errori di select, ma ignora EINTR (interruzione da segnale)
			if (errno != EINTR)
				Logger::error("select() failed: " + std::string(strerror(errno)));
			continue;
		}
		
		// Controlla se ci sono nuove connessioni sul socket principale
		if (FD_ISSET(_socket, &readFds))
			acceptNewConnection();
		
		// Controlla attività sui socket client esistenti
		std::map<int, Client*>::iterator it = _clients.begin();
		while (it != _clients.end())
		{
			int clientFd = it->first;
			Client* client = it->second;
			++it;
			
			// Se c'è attività in lettura su questo socket client
			if (FD_ISSET(clientFd, &readFds))
			{
				if (!handleClientData(clientFd))
				{
					// Se handleClientData ritorna false, il client si è disconnesso
					delete client;
					client = NULL;
					removeClient(clientFd);
					continue;
				}
			}
			
			// Se c'è possibilità di scrittura su questo socket client
			if (client && FD_ISSET(clientFd, &writeFds))
			{
				// TODO: Implementare l'invio di dati in attesa per questo client
			}
		}
	}
}

void Server::acceptNewConnection()
{
	struct sockaddr_in clientAddr;
	socklen_t clientAddrLen = sizeof(clientAddr);
	
	int clientFd = accept(_socket, (struct sockaddr*)&clientAddr, &clientAddrLen);
	
	if (clientFd < 0)
	{
		if (errno != EAGAIN && errno != EWOULDBLOCK)
			Logger::error("Failed to accept connection: " + std::string(strerror(errno)));
		return;
	}
	
	// Imposta il socket client in modalità non bloccante
	int flags = fcntl(clientFd, F_GETFL, 0);
	if (flags == -1 || fcntl(clientFd, F_SETFL, flags | O_NONBLOCK) == -1)
	{
		Logger::error("Failed to set client socket to non-blocking mode");
		close(clientFd);
		return;
	}
	
	// Crea un nuovo client
	Client* newClient = new Client();
	newClient->setSocketFd(clientFd);
	newClient->setIpAddr(inet_ntoa(clientAddr.sin_addr));
	
	// Aggiungi il client alla mappa
	_clients[clientFd] = newClient;
	
	Logger::info("New client connected from " + newClient->getIpAddr());
}

bool Server::handleClientData(int clientFd)
{
	char buffer[1024];
	memset(buffer, 0, sizeof(buffer));
	
	int bytesRead = recv(clientFd, buffer, sizeof(buffer) - 1, 0);
	
	if (bytesRead <= 0)
	{
		if (bytesRead == 0)
		{
			// Connessione chiusa dal client
			Logger::info("Client disconnected gracefully");
		}
		else if (errno != EAGAIN && errno != EWOULDBLOCK)
		{
			// Errore reale
			Logger::error("Error reading from client: " + std::string(strerror(errno)));
		}
		return false;
	}
	
	// Termina il buffer con un null byte per sicurezza
	buffer[bytesRead] = '\0';
	
	// Logga il messaggio ricevuto
	Logger::debug("Received data from client: " + std::string(buffer));
	
	// Qui verrebbe implementato il parsing e la gestione del comando
	// Per ora, echoing semplice per testing
    // TODO: gestione mssaggi incompleti senza /n --> echo -n "TEST WITHOUT NEWLINE" | nc localhost 6667
	std::string response = "Server received: " + std::string(buffer) + "\r\n";
	send(clientFd, response.c_str(), response.length(), 0);
	
	return true;
}

// Getters
uint16_t Server::getPort() const { return _port; }
const std::string& Server::getPassword() const { return _password; }
