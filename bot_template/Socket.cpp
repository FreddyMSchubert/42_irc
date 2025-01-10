#include "Bot.hpp"
#include <iostream>
#include <stdexcept>
#include <readline/readline.h>
#include <string>
#include <unistd.h>
#include <sstream>

Socket::Socket()
{
	_socket_ip = "";
	_socket_port = -1;
}

Socket::~Socket()
{
	if (!error)
		this->_sendMessage("QUIT");
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
	ssize_t sent = send(_socket_fd, msg.append("\r\n").c_str(), msg.length(), 0);
	if (sent == -1)
	{
		if (errno == EPIPE)
		{
			_onErrorCallback("Server closed the connection already. Unable to send message");
			running = false;
		}
		else
			_onErrorCallback("Failed to send message");
	}
	else if (sent < msg.length())
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

	while (42)
	{
		int ret = poll(fds, 1, 50);
		if (ret == -1)
			_onErrorCallback("Poll error: " + std::string(strerror(errno)));
		else if (ret > 0)
		{
			if (fds[0].revents & POLLIN)
			{
				std::cout << "Message received from Socket:" << std::endl;
				char buffer[1024] = {0};
				std::memset(buffer, 0, sizeof(buffer));
				int valread = read(_socket_fd, buffer, sizeof(buffer) - 1);
				if (valread == 0)
				{
					std::cerr << "-> Server closed the connection" << std::endl;
					_onDisconnectCallback();
					break;
				}
				else if (valread < 0)
				{
					std::cerr << "-> Read error: " << strerror(errno) << std::endl;
					_onErrorCallback("Read error: " + std::string(strerror(errno)));
				}
				else
				{
					std::cout << "-> Data read from server: " << buffer << std::endl;
					_parseBuffer(std::string(buffer));
				}
			}
			if (fds[0].revents & POLLOUT)
			{
				if (_messages.empty()) continue;

				std::cout << "Sending next message in queue!" << std::endl;
				_sendMessage(_messages.front());
				_messages.pop();
				usleep(8000);
			}
		}
	}
}

// TODO: cant really do this with find
void Socket::_parseBuffer(std::string buffer)
{
	if (buffer.find("Your username is now ") != std::string::npos)
	{
		std::cout << "Username set!" << std::endl;
		return;
	}
	if (buffer.find("Your nickname is now ") != std::string::npos)
	{
		std::cout << "Nickname set!" << std::endl;
		return;
	}
	if (buffer.find("Authentication failed.") != std::string::npos)
	{
		_onErrorCallback("Authentication failed.");
		error = true;
		return;
	}

	std::string user, channel, message;
	std::istringstream iss(buffer);
	iss >> user >> channel;
	std::getline(iss, message);
	_onMessageCallback(user, channel, message);
}