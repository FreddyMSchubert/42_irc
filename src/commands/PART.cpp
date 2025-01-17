#include "../../inc/CommandHandler.hpp"

void CommandHandler::HandlePART(const std::vector<std::string> &parts, Client & client, Server &server)
{
	if (!client.isAuthenticated)
		return client.sendCodeResponse(451, "You have not registered", "PART"); // ERR_NOTREGISTERED
	if (!client.channelId)
		return client.sendCodeResponse(442, "You're not on any channel", "PART"); // ERR_NOTONCHANNEL
	if (parts.size() < 2)
		return client.sendCodeResponse(461, "Not enough parameters", "PART"); // ERR_NEEDMOREPARAMS

	Channel *channel = server.getChannelById(client.channelId.value());
	if (!channel)
		return client.sendCodeResponse(403, "No such channel");

	std::string reason = parts.size() > 1 ? parts[1] : "";
	std::string partMessage = ":" + client.nickname + "!" + client.username + "@irctic.com PART " + channel->name + " ";
	if (!reason.empty())
		partMessage += " :" + reason;
	partMessage += "\r\n";

	channel->removeMember(client.id, server);
	client.channelId.reset();
	channel = nullptr;

	client.sendMessage(partMessage);
}
