#include "Bot.hpp"
#include <exception>
#include <iostream>
#include <stdexcept>
#include <unistd.h>

void onMessage(std::string user, std::string channel, std::string message);
void onDisconnect(std::string reason);
void onConnect(std::string server);
void onError(std::string message);
void onAuthenticate();

Bot::Bot()
{
	std::cout << "Creating bot but dont forget to set ip and everything!" << std::endl;
}

Bot::~Bot()
{
	std::cout << "Destroying bot!" << std::endl;
}

void Bot::setIp(std::string ip)
{
	std::regex ipRegex(R"((\b25[0-5]\b|\b2[0-4][0-9]\b|\b1[0-9]{2}\b|\b[1-9]?[0-9]\b)\.(\b25[0-5]\b|\b2[0-4][0-9]\b|\b1[0-9]{2}\b|\b[1-9]?[0-9]\b)\.(\b25[0-5]\b|\b2[0-4][0-9]\b|\b1[0-9]{2}\b|\b[1-9]?[0-9]\b)\.(\b25[0-5]\b|\b2[0-4][0-9]\b|\b1[0-9]{2}\b|\b[1-9]?[0-9]\b))");
	if (std::regex_match(ip, ipRegex))
		_ip = ip;
	else
		throw std::runtime_error("Invalid IP address");
}

void Bot::setPort(int port)
{
	if (port > 0)
		_port = port;
	else
		throw std::runtime_error("Port must be greater than 0");
}

void Bot::setNick(std::string nick)
{
	if (nick.empty())
		throw std::runtime_error("Nick cannot be empty");
	if (nick.find(" ") != std::string::npos)
		throw std::runtime_error("Nick cannot contain spaces");
	_nick = nick;
}

void Bot::setUser(std::string user)
{
	if (user.empty())
		throw std::runtime_error("User cannot be empty");
	if (user.find(" ") != std::string::npos)
		throw std::runtime_error("User cannot contain spaces");
	_user = user;
}

void Bot::setPassword(std::string password)
{
	_password = password;
}

void Bot::connectToServer()
{
	std::cout << "Connecting to server: " << _ip << ":" << _port << std::endl;
	try {
		socket.connectToServer(this->_ip, this->_port);
	} catch (std::exception& e) {
		throw std::runtime_error(std::string(e.what()));
	}
}

void Bot::authenticate()
{
	socket.queueMessage("PASS " + _password);
	socket.queueMessage("NICK " + _nick);
	socket.queueMessage("USER bot_" + _user);
}

void Bot::directMessage(std::string user, std::string msg)
{
	socket.queueMessage("PRIVMSG " + user + " " + msg);
}

void Bot::sendRawMessage(std::string msg)
{
	socket.queueMessage(msg);
}

void Bot::sendMessage(std::string msg)
{
	socket.queueMessage("PRIVMSG " + _current_channel + " " + msg);

}

void Bot::sendMessage(std::string channelname, std::string msg)
{
	socket.queueMessage("PRIVMSG " + channelname + " " + msg);
}

void Bot::changeChannel(std::string channel)
{
	socket.queueMessage("JOIN " + channel);
	_current_channel = channel;
}

void Bot::changeChannel(std::string channel, std::string password)
{
	socket.queueMessage("JOIN " + channel + " " + password);
	_current_channel = channel;
}

void Bot::setCallbacks(onConnectCallback onConnect,
					onErrorCallback onError,
					onMessageCallback onMessage,
					onDisconnectCallback onDisconnect,
					onUserChannelJoinCallback onUserChannelJoin,
					onUserChannelJoinCallback onUserChannelLeave)
{
	socket.setOnMessageCallback(onMessage);
	socket.setOnDisconnectCallback(onDisconnect);
	socket.setOnConnectCallback(onConnect);
	socket.setOnErrorCallback(onError);
	socket.setOnUserChannelJoinCallback(onUserChannelJoin);
	socket.setOnUserChannelLeaveCallback(onUserChannelLeave);
	std::cout << "Callbacks set!" << std::endl;
}

void Bot::startPollingForEvents()
{
	socket.Run();
}
