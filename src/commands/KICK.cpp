#include "../../inc/CommandHandler.hpp"

void CommandHandler::HandleKICK(const std::vector<std::string> &parts, Client & client, Server &server)
{
	if (parts.size() < 3)
		return client.sendCodeResponse(461, "Not enough parameters", "KICK");
	Channel *channel = server.getChannelByName(parts[1]);
	if (!channel)
		return client.sendCodeResponse(403, "No such channel", parts[1]);
	if (!client.isOperatorIn(channel))
		return client.sendCodeResponse(482, "You're not channel operator", parts[1]);

	Client *clientToKick = server.getClientByName(parts[2]);
	if (!clientToKick)
		return client.sendCodeResponse(401, "No such nick/channel", parts[2]);
	channel->kick(clientToKick->id, server);
	clientToKick->sendMessage(":" + client.nickname + "!" + client.username + "@irctic.com KICK " + channel->name + " " + clientToKick->getName() + " :Kicked");
	clientToKick->sendMessage(":" + client.nickname + "!" + client.username + "@irctic.com PART " + channel->name + " " + clientToKick->getName() + " :Kicked");
	channel->broadcast(":" + client.nickname + "!" + client.username + "@irctic.com KICK " + channel->name + " " + clientToKick->getName() + " :Kicked", server, clientToKick);
	clientToKick->channelId.reset();

	if (parts.size() < 4)
		return clientToKick->sendCodeResponse(403, "No reason given", "KICK");
	else
		return clientToKick->sendCodeResponse(403, "Kicked (" + parts[3] + ")", "KICK");
	client.sendCodeResponse(200, "KICK", channel->name + " " + parts[2] + " :Kicked");
}
