#include "../inc/Client.hpp"

#include "../inc/Channel.hpp" // avoid circ depend

Client::Client(int fd, Socket socket, int id) // Client socket constructor
	: id(id), socket(std::move(socket)), fd(fd), states({0, 0, 0, 0}) {}
Client::Client(int port) // Listening socket constructor
	: id(-1), socket(port), fd(socket.getSocketFd()), states({0, 0, 0, 0}) {}

Client::Client(Client&& other) noexcept :
	id(other.id),
	socket(std::move(other.socket)),
	fd(other.fd),
	states(other.states),
	inbuffer(std::move(other.inbuffer)),
	outbuffer(std::move(other.outbuffer)),
	channelId(other.channelId),
	isAuthenticated(other.isAuthenticated),
	isOperator(other.isOperator),
	nickname(other.nickname),
	username(other.username)
	{}
Client& Client::operator=(Client&& other) noexcept
{
	if (this == &other)
		return *this;
	id = other.id;
	socket = std::move(other.socket);
	fd = other.fd;
	states = other.states;
	inbuffer = other.inbuffer;
	outbuffer = other.outbuffer;
	channelId = other.channelId;
	isAuthenticated = other.isAuthenticated;
	isOperator = other.isOperator;
	nickname = other.nickname;
	username = other.username;
	return *this;
}

std::string Client::getName()
{
	if (!nickname.empty())
		return nickname;
	if (!username.empty())
		return username;
	return std::string("User_") + std::to_string(id);
}

bool Client::isOperatorIn(Channel * channel)
{
	if (!channel)
		return isOperator;
	return isOperator || channel->isOperator(id);
}

void Client::sendMessage(std::string msg)
{
	if (msg.empty())
		return;
	if (msg[msg.size() - 1] != '\n')
		msg += "\r\n";
	outbuffer += msg;
}

bool Client::updateAuthStatus()
{
	isAuthenticated = !nickname.empty() && !username.empty() && knewPassword;
	return isAuthenticated;
}

std::string Client::getInfoString()
{
	std::string info = "Client " + std::to_string(id) + " (fd " + std::to_string(fd) + ")" + ": ";
	info += "n: " + nickname + ", ";
	info += "u: " + username + ", ";
	info += "auth: " + std::to_string(isAuthenticated) + ", ";
	info += "op: " + std::to_string(isOperator) + ", ";
	info += "knewpw: " + std::to_string(knewPassword) + ", ";
	info += "welcome: " + std::to_string(hasReceivedWelcome);
	return info;
}

void Client::sendCodeResponse(int code, const std::string msg, const std::string arg)
{
	if (msg.empty())
		return;

	std::string formattedMsg = msg;
	if (formattedMsg.back() != '\n')
		formattedMsg += "\r\n";

	std::stringstream ss;
	ss << std::setw(3) << std::setfill('0') << code;
	std::string paddedCode = ss.str();

	std::string fullMessage = ":irctic.com " + paddedCode + " " + this->nickname + " " + arg + " :" + formattedMsg; //check if its ok if we always send the nickname with it
	sendMessage(fullMessage);
}
