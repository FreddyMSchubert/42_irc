#include "Bot.hpp"
#include <iostream>
#include <stdexcept>
#include <string>
#include <unistd.h>
#include <sstream>
#include <csignal>
#include <cstring>
#include <regex>

volatile bool Socket::_running = false;

void Socket::signalHandler(int signum)
{
	std::cerr << "Caught signal: " << signum << std::endl;
	_running = false;
}

Socket::Socket()
{
	_socket_ip = "";
	_socket_port = -1;
	std::signal(SIGINT, signalHandler);
	std::signal(SIGTERM, signalHandler);
	std::signal(SIGQUIT, signalHandler);
	std::signal(SIGKILL, signalHandler);
	std::signal(SIGSTOP, signalHandler);
}

Socket::~Socket()
{
	std::cout << "Closing socket: " << _socket_fd << std::endl;
	close(_socket_fd);
}

void Socket::_setNonBlocking()
{
	int flags = fcntl(_socket_fd, F_GETFL, 0);
	if (flags == -1)
		throw std::runtime_error("Failed to get socket " + std::to_string(_socket_fd) + " flags");
	if (fcntl(_socket_fd, F_SETFL, flags | O_NONBLOCK) == -1)
		throw std::runtime_error("Failed to set non-blocking mode on socket " + std::to_string(_socket_fd));
}

void Socket::connectToServer(std::string ip, int port)
{
	if (ip.empty() || port <= 0)
		throw std::runtime_error("IP or port not set");

	_socket_ip = ip;
	_socket_port = port;

	_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_socket_fd == -1)
		throw std::runtime_error("Socket creation failed");

	try
	{
		std::memset(&_socket, 0, sizeof(_socket));
		_socket.sin_family = AF_INET;
		_socket.sin_port = htons(this->_socket_port);

		if (inet_pton(AF_INET, this->_socket_ip.c_str(), &_socket.sin_addr) <= 0)
		{
			close(_socket_fd);
			throw std::runtime_error("Invalid IP address");
		}

		if (connect(_socket_fd, (struct sockaddr *)&_socket, sizeof(_socket)) < 0)
		{
			if (errno != EINPROGRESS)
			{
				close(_socket_fd);
				_onErrorCallback("Failed to connect to server: " + std::string(strerror(errno)));
				throw std::runtime_error("Failed to connect to server: " + std::string(strerror(errno)));
			}
		}
		else
		{
			_setNonBlocking();
			std::cout << "Connected to server: " << ip << ":" << port << std::endl;
			_running = true;
		}

	}
	catch(const std::exception &e)
	{
		throw std::runtime_error(e.what());
	}
	_onConnectCallback();
}

void Socket::queueMessage(std::string msg)
{
	std::cout << "Queuing message: " << msg << std::endl;
	_messages.push(msg);
}

void Socket::_sendMessage(std::string msg)
{
	if (msg.empty())
	{
		_onErrorCallback("Message is empty");
		return;
	}
	msg = msg.append("\r\n");
	ssize_t sent = send(_socket_fd, msg.c_str(), msg.length(), 0); //TODO: for some reason it sends everything except the last char
	if (sent == -1)
	{
		if (errno == EPIPE)
		{
			_onErrorCallback("Server closed the connection already. Unable to send message");
			_running = false;
		}
		else
			_onErrorCallback("Failed to send message");
	}
	else if (sent < static_cast<ssize_t>(msg.length()))
		std::cerr << "Warning: Partial message sent: " << sent << " of " << msg.length() << " bytes" << std::endl;
	else
		std::cout << "Message sent successfully: " << msg << std::endl;
}

void Socket::Run()
{
	// Poll for events
	struct pollfd fds[1];
	fds[0].fd = _socket_fd;
	fds[0].events = POLLIN | POLLOUT;

	std::cout << "Starting to poll for events" << std::endl;

	while (_running)
	{
		int ret = poll(fds, 1, 50);
		if (ret == -1)
			_onErrorCallback("Poll error: " + std::string(strerror(errno)));
		else if (ret > 0 && _running)
		{
			if (fds[0].revents & POLLIN && _running)
			{
				std::cout << "Message received from Socket:" << std::endl;
				char buffer[1024] = {0};
				std::memset(buffer, 0, sizeof(buffer));
				int valread = read(_socket_fd, buffer, sizeof(buffer) - 1);
				if (valread == 0)
				{
					std::cerr << "-> Server closed the connection" << std::endl;
					_onDisconnectCallback();
					_running = false;
				}
				else if (valread < 0)
				{
					std::cerr << "-> Read error: " << strerror(errno) << std::endl;
					_onErrorCallback("Read error: " + std::string(strerror(errno)));
					_running = false;
				}
				else
				{
					std::cout << "-> Data read from server: " << buffer << std::endl;
					_parseBuffer(std::string(buffer));
				}
			}
			if (fds[0].revents & POLLOUT && _running)
			{
				if (_messages.empty()) continue;

				std::cout << "Sending next message in queue!" << std::endl;
				_sendMessage(_messages.front());
				_messages.pop();
			}
		}
	}
}

static std::string trim(const std::string &str)
{
    size_t start = str.find_first_not_of("\r\n");
    size_t end = str.find_last_not_of("\r\n");

    return (start == std::string::npos || end == std::string::npos) ? "" : str.substr(start, end - start + 1);
}

// TODO: cant really do this with find
void Socket::_parseBuffer(std::string buffer)
{
	if (buffer.empty())
		return;

	std::cout << "Parsing buffer: " << buffer << std::endl;

	std::regex joinRegex(":([^!]+)![^ ]+ JOIN ([^ ]+)");
	std::smatch matchJoin;
	if (buffer.find("JOIN") != std::string::npos && std::regex_search(buffer, matchJoin, joinRegex))
    {
        if (matchJoin.size() == 3)
        {
            std::string user = matchJoin[1];
            std::string channel = matchJoin[2];
            _onUserChannelJoinCallback(trim(user), trim(channel));
        }
    }

	std::regex partRegex(":([^!]+)!([^@]+)@([^ ]+) PART ([^ ]+)");
    std::smatch matchPart;
    if (buffer.find("PART") != std::string::npos && std::regex_search(buffer, matchPart, partRegex))
    {
        if (matchPart.size() == 5)
        {
            std::string nick = matchPart[1];
            std::string user = matchPart[2];
            std::string host = matchPart[3];
            std::string channel = matchPart[4];
            _onUserChannelLeaveCallback(trim(nick), trim(channel));
        }
    }

	std::regex privmsgRegex(":([^!]+)![^ ]+ PRIVMSG ([^ ]+) :(.+)");
	std::smatch matchPriv;
	if (buffer.find("PRIVMSG") != std::string::npos && std::regex_search(buffer, matchPriv, privmsgRegex))
	{
		if (matchPriv.size() == 4)
		{
			std::string user = matchPriv[1];
			std::string channel = matchPriv[2];
			std::string message = matchPriv[3];
			_onMessageCallback(trim(user), trim(channel), trim(message));
		}
	}
}
