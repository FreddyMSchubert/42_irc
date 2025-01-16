#include "../inc/Channel.hpp"

#include "../inc/Server.hpp"

Channel::Channel(std::string name) : name(name) {};

std::string Channel::addMember(unsigned int clientId, Server &server)
{
	Client * client = server.getClientById(clientId);
	if (!client)
		return ":irctic.com 401 * :No such nick/channel";

	if (inviteOnly && !_invites[clientId])
		return ":irctic.com 473 " + client->nickname + " " + name + " :Cannot join channel (+i)";
	if (_kicked[clientId] && !_invites[clientId])
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
	std::string joinMsg = ":" + client->nickname + "!" + client->username + "@" + server.name + " JOIN :" + name + "\r\n";
	broadcast(joinMsg, server, clientId);

	std::string response;
	// 332 RPL_TOPIC
	if (!topic.empty())
		response += ":" + server.name + " 332 " + client->nickname + " " + name + " :" + topic + "\r\n";
	// 353 RPL_NAMREPLY
	std::string namesList;
	for (const auto& member : _members)
	{
		if (member.second)
		{
			Client* m = server.getClientById(member.first);
			if (m)
				namesList += m->nickname + " ";
		}
	}
	if (!namesList.empty())
		namesList.pop_back();
	response += ":" + server.name + " 353 " + client->nickname + " = " + name + " :" + namesList + "\r\n";
	// 366 RPL_ENDOFNAMES
	response += ":" + server.name + " 366 " + client->nickname + " " + name + " :End of /NAMES list\r\n";
	return joinMsg + response;
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
std::string Channel::inviteMember(unsigned int clientId, Server &server)
{
	Client * client = server.getClientById(clientId);
	if (!client)
		return ":irctic.com 401 * :No such nick/channel";
	if (_members[clientId])
		return ":irctic.com 443 " + client->nickname + " " + name + " :is already on that channel";
	if (_invites[clientId])
		return ":irctic.com 443 " + client->nickname + " " + name + " :is already invited to that channel";
	_invites[clientId] = true;
	_kicked[clientId] = false;
	return ":irctic.com 341 " + client->nickname + " " + name + " :Invited to channel";
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
	_invites[clientId] = false;
	_operators[clientId] = false;
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

std::string Channel::getInfoString()
{
	std::string info = "Channel: \"" + name + "\"";
	info += "\nTopic: \"" + topic + "\"";
	info += "\nPassword: \"" + password + "\"";
	info += "\nInvite Only: \"" + std::to_string(inviteOnly) + "\"";
	info += "\nAnyone Can Change Topic: \"" + std::to_string(anyoneCanChangeTopic) + "\"";
	info += "\nUser Limit: \"" + std::to_string(limit) + "\"";
	info += "\nMembers: ";
	for (const auto& member : _members)
	{
		if (member.second)
			info += std::to_string(member.first) + " ";
	}
	info += "\nKicked: ";
	for (const auto& kicked : _kicked)
	{
		if (kicked.second)
			info += std::to_string(kicked.first) + " ";
	}
	info += "\nOperators: ";
	for (const auto& op : _operators)
	{
		if (op.second)
			info += std::to_string(op.first) + " ";
	}
	info += "\nInvites: ";
	for (const auto& invite : _invites)
	{
		if (invite.second)
			info += std::to_string(invite.first) + " ";
	}
	return info;
}
