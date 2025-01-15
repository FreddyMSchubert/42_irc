#include "../inc/Channel.hpp"

#include "../inc/Server.hpp"

Channel::Channel(std::string name) : name(name) {};

std::string Channel::addMember(unsigned int clientId, Server &server, bool wasInvited)
{
	Client * client = server.getClientById(clientId);
	if (!client)
		return ":irctic.com 401 * :No such nick/channel";

	if (inviteOnly && !wasInvited)
		return ":irctic.com 473 " + client->nickname + " " + name + " :Cannot join channel (+i)";
	if (_kicked[clientId])
		return ":irctic.com 474 " + client->nickname + " " + name + " :Banned from channel";
	size_t currMemberInChannel = 0;
	for (const auto& member : _members)
		currMemberInChannel += member.second;
	if (limit > 0 && currMemberInChannel >= (size_t)limit)
		return ":irctic.com 471 " + client->nickname + " " + name + " :Cannot join channel (+l)";
	if (_members[clientId])
		return ":irctic.com 443 " + client->nickname + " " + name + " :is already on that channel";

	_members[clientId] = true;
	client->channel = this;
	Logger::Log(LogLevel::INFO, std::string("Added client ") + client->nickname + " to channel " + name + ".");
	broadcast(":" + client->nickname + "!" + server.getClientById(clientId)->username + "@irctic.com JOIN " + name, server, clientId);
	return ":" + client->nickname + "!" + client->username + "@irctic.com JOIN " + name;
}
std::string Channel::removeMember(unsigned int clientId, Server &server)
{
	Logger::Log(LogLevel::INFO, std::string("Removing member ") + std::to_string(clientId) + " from channel " + name);
	_members[clientId] = false;
	Client * client = server.getClientById(clientId); 
	if (!client)
		return ":irctic.com 401 * :No such nick/channel";
	client->channel = nullptr;
	broadcast(":" + client->nickname + "!" + server.getClientById(clientId)->username + "@irctic.com PART " + name, server, clientId);
	return ":" + client->nickname + "!" + client->username + "@irctic.com PART " + name;
}

void Channel::broadcast(std::string msg, Server &server, unsigned int except_id)
{
	for (const auto& member : _members)
	{
		unsigned int clientId = member.first;
		if (clientId != except_id && member.second)
		{
			Client* client = server.getClientById(clientId);
			if (client)
				client->sendMessage(msg);
		}
	}
}

void Channel::kick(unsigned int clientId, Server &server)
{
	removeMember(clientId, server);
	_kicked[clientId] = true;
}
void Channel::unkick(unsigned int clientId)
{
	_kicked[clientId] = false;
}

void Channel::addOperator(unsigned int clientId)
{
	_operators[clientId] = true;
}
void Channel::removeOperator(unsigned int clientId)
{
	_operators[clientId] = false;
}
bool Channel::isOperator(unsigned int clientId)
{
	return _operators[clientId];
}

std::map<unsigned int, bool> & Channel::getMembers()
{
	return _members;
}
