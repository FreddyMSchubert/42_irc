#include "../../inc/CommandHandler.hpp"

void CommandHandler::HandlePART(const std::vector<std::string> &parts, Client & client, Server &server)
{
	if (!client.isAuthenticated)
		return client.sendCodeResponse(451, "You have not registered", "PART"); // ERR_NOTREGISTERED
	if (!client.channel)
		return client.sendCodeResponse(442, "You're not on any channel", "PART"); // ERR_NOTONCHANNEL
	if (parts.size() < 2)
		return client.sendCodeResponse(461, "Not enough parameters", "PART"); // ERR_NEEDMOREPARAMS

	std::string partMessage = "PART " + client.channel->name;
	if (parts.size() > 1)
		partMessage += " 300 :" + parts[1];
	partMessage += "\r\n";

	client.channel->removeMember(client.id, server);
	client.channel = nullptr;

	client.sendMessage(partMessage);
}
