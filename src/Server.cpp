#include "../inc/Server.hpp"
#include "../inc/Socket.hpp"

//Setup the listening socket and push it to the vector of sockets
Server::Server(int port, std::string password, std::string op_password) : _port(port), _listening_socket(port), _password(password), _op_password(op_password), current_id(1)
{
	Logger::Log(LogLevel::INFO, "Server initialized on port " + std::to_string(port));
}

void Server::Run()
{
	while (running) // to exit program, please send sigint
	{
		acceptNewConnections();
		if (!running)
			break;
		updatePoll();
		if (!running)
			break;
		handleExistingConnections();
	}

	// clean up sockets
	_sockets.clear();
}

/* ----------------------- */
/* ----- CONNECTIONS ----- */
/* ----------------------- */

// POLLIN    // Ready to read (incoming requests/data)
// POLLOUT   // Ready to write (sending responses)
// POLLHUP   // Client disconnected
// POLLERR   // Error occurred
void Server::updatePoll()
{
	// Create pollfds for poll call
	std::vector<struct pollfd> fds;
	for (size_t i = 0; i < _sockets.size(); ++i)
	{
		struct pollfd pfd;
		pfd.fd = _sockets[i].socket.getSocketFd();
		pfd.events = POLLIN | POLLOUT | POLLHUP | POLLERR;
		fds.push_back(pfd);
	}
	struct pollfd listen_pfd;
	listen_pfd.fd = _listening_socket.socket.getSocketFd();
	listen_pfd.events = POLLIN | POLLERR | POLLHUP;
	fds.push_back(listen_pfd);

	// poll
	int ret = poll(fds.data(), fds.size(), 10);
	if (ret < 0)
	{
		Logger::Log(LogLevel::ERROR, "Poll failed.");
		return;
	}

	// Read out poll data
	_listening_socket.states.read = fds[fds.size() - 1].revents & POLLIN;
	_listening_socket.states.disconnect = fds[fds.size() - 1].revents & POLLHUP;
	_listening_socket.states.error = fds[fds.size() - 1].revents & POLLERR;
	for (size_t i = 0; i < fds.size() - 1; i++)
	{
		_sockets[i].states.read = fds[i].revents & POLLIN;
		_sockets[i].states.write = fds[i].revents & POLLOUT;
		_sockets[i].states.disconnect = fds[i].revents & POLLHUP;
		_sockets[i].states.error = fds[i].revents & POLLERR;
	}
}

void Server::acceptNewConnections()
{
	if (!_listening_socket.states.read)
		return;
	int max_iterations = 10;
	while (max_iterations > 0)
	{
		struct sockaddr_in client_addr;
		socklen_t addrlen = sizeof(struct sockaddr_in);
		int client_fd = accept(_listening_socket.socket.getSocketFd(), (struct sockaddr*)&client_addr, &addrlen);
		if (client_fd >= 0)
		{
			Logger::Log(LogLevel::INFO, "New client connected");
			try {
				_sockets.emplace_back(client_fd, Socket(client_fd, _port), current_id++);
			}
			catch (std::exception &e)
			{
				Logger::Log(LogLevel::ERROR, "Failed to create new client: " + std::string(e.what()));
			}
			max_iterations--;
		}
		else
			break; // no more clients to accept
	}
}

void Server::handleExistingConnections()
{
	for (int i = (int)_sockets.size() - 1; i >= 0; i--)
	{
		if (_sockets[i].shouldDisconnect)
		{
			Logger::Log(LogLevel::INFO, "Client disconnected");
			if (_sockets[i].channelId && getChannelById(_sockets[i].channelId.value()))
				getChannelById(_sockets[i].channelId.value())->removeMember(_sockets[i].id, *this);
			_sockets.erase(_sockets.begin() + i);
			continue;
		}
		if (_sockets[i].states.disconnect || _sockets[i].states.error)
		{
			if (_sockets[i].states.disconnect)
				Logger::Log(LogLevel::INFO, "Client disconnected");
			else
				Logger::Log(LogLevel::ERROR, "Client encountered an error and disconnected.");
			if (_sockets[i].channelId && getChannelById(_sockets[i].channelId.value()))
				getChannelById(_sockets[i].channelId.value())->removeMember(_sockets[i].id, *this);
			_sockets.erase(_sockets.begin() + i);
			continue;
		}
		if (_sockets[i].states.read)
		{
			try
			{
				std::string data = _sockets[i].socket.receiveData();
				_sockets[i].inbuffer += data;
			}
			catch(const std::runtime_error &e)
			{
				Logger::Log(LogLevel::WARNING, "Erroneous message data: " + std::string(e.what()));
				_sockets.erase(_sockets.begin() + i);
				continue;
			}
			HandleClientData(_sockets[i].id);
		}
		if (_sockets[i].states.write && !_sockets[i].outbuffer.empty())
		{
			try
			{
				_sockets[i].socket.sendData(_sockets[i].outbuffer);
				_sockets[i].outbuffer.clear();
			}
			catch (const std::exception &e)
			{
				Logger::Log(LogLevel::ERROR, "Failed to send response: " + std::string(e.what()));
			}
			continue;
		}
	}
}

/* ----------------------- */
/* ------ IRC LOGIC ------ */
/* ----------------------- */

void Server::HandleClientData(unsigned int clientId)
{
	Client *client = getClientById(clientId);
	if (!client)
		return;
	while (true)
	{
		size_t newLinePos = client->inbuffer.find("\n");
		if (newLinePos == std::string::npos)
			return;
		size_t lineEnd = newLinePos;
		if (newLinePos > 0 && client->inbuffer[newLinePos - 1] == '\r')
			lineEnd = newLinePos - 1;
		std::string line = client->inbuffer.substr(0, lineEnd);
		client->inbuffer.erase(0, newLinePos + 1);
		CommandHandler::HandleCommand(line, clientId, *this);
	}
}

Client * Server::getClientById(unsigned int id)
{
	for (auto& client_ptr : _sockets)
		if (client_ptr.id == id)
			return &client_ptr;
	return nullptr;
}
std::vector<Client> &Server::getClients()
{
	return _sockets;
}
unsigned int Server::getClientIdByName(std::string name)
{
	for (auto& client : _sockets)
		if (client.nickname == name)
			return client.id;
	return -1;
}
Client * Server::getClientByName(std::string name)
{
	unsigned int id = getClientIdByName(name);
	if (id <= 0)
		return nullptr;
	return getClientById(id);
}

bool Server::isCorrectPassword(std::string passwordAttempt)
{
	return passwordAttempt == _password;
}
bool Server::isCorrectOperatorPassword(std::string passwordAttempt)
{
	return passwordAttempt == _op_password;
}

void Server::createChannel(std::string channelName)
{
	_channels.emplace_back(channelName, current_id++);
	Logger::Log(LogLevel::INFO, "Created new channel: " + channelName);
}
std::vector<Channel> &Server::getChannels()
{
	return _channels;
}
Channel *Server::getChannelById(unsigned int id)
{
	for (auto& channel : _channels)
		if (channel.id == id)
			return &channel;
	return nullptr;
}
Channel *Server::getChannelByName(std::string name)
{
	for (auto& channel : _channels)
		if (channel.name == name)
			return &channel;
	return nullptr;
}
