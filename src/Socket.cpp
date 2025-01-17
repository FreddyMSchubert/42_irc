#include "../inc/Socket.hpp"

// Listening socket constructor
Socket::Socket(int port) : _port(port)
{
	_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_socket_fd == -1)
		throw std::runtime_error("Socket creation failed");

	Logger::Log(LogLevel::INFO, "Running Listening Socket on fd " + std::to_string(_socket_fd) + "...");
	try
	{
		std::memset(&_socket, 0, sizeof(_socket));
		_socket.sin_family = AF_INET;
		_socket.sin_port = htons(_port);
		_socket.sin_addr.s_addr = INADDR_ANY; // accept incoming connections to any ip

		int opt = 1;
		if (setsockopt(_socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
		{
			perror("setsockopt");
			exit(EXIT_FAILURE);
		}

		if (bind(_socket_fd, (struct sockaddr *)&_socket, sizeof(_socket)) < 0)
		{
			close(_socket_fd);
			throw std::runtime_error("Failed to bind socket" + std::to_string(_socket_fd));
		}

		if (listen(_socket_fd, 10) == -1)
		{
			close(_socket_fd);
			throw std::runtime_error("Failed to listen on socket");
		}

		setNonBlocking();
	}
	catch(const std::exception &e)
	{
		throw std::runtime_error(e.what());
	}
}
// Client socket constructor
Socket::Socket(int fd, int port) : _port(port)
{
	_socket_fd = fd;
	try {
	    setNonBlocking();
    }
    catch(const std::exception &e)
    {
        throw std::runtime_error("Failed to set socket with to non blocking" + std::string(e.what()));
	}
}

Socket::Socket(Socket&& other) noexcept
	: _socket_fd(other._socket_fd),
	_socket(other._socket),
	_port(other._port)
{
	other._socket_fd = -1; // Mark as moved; prevents double close
}
Socket& Socket::operator=(Socket&& other) noexcept
{
	if (this == &other)
		return *this;
	if (_socket_fd >= 0)
		close(_socket_fd);
	_socket_fd = other._socket_fd;
	_socket = other._socket;
	_port = other._port;
	other._socket_fd = -1; // Mark as moved; prevents double close
	return *this;
}
Socket::~Socket()
{
	if (_socket_fd >= 0)
	{
		close(_socket_fd);
	}
}

void Socket::sendData(const std::string & data)
{
	ssize_t sent = send(_socket_fd, data.c_str(), data.length(), 0);
	if (sent < 0)
		throw std::runtime_error("Failed to send data");
}

std::string Socket::receiveData()
{
	std::string data;
	char buffer[512];
	ssize_t received;

	Logger::Log(LogLevel::INFO, "Receiving data...");
	received = recv(_socket_fd, buffer, sizeof(buffer), 0);
	if (received > 0)
		data.append(buffer, received);
	else if (received <= 0)
	{
		Logger::Log(LogLevel::WARNING, "Failed to receive data");
		return "";
	}

	std::cout << "DATA: " << data << std::endl;
	return data;
}

void Socket::setNonBlocking()
{
	if (fcntl(_socket_fd, F_SETFL, O_NONBLOCK) == -1)
		throw std::runtime_error("Failed to set non-blocking mode on socket " + std::to_string(_socket_fd));
}
