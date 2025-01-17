#include "../../inc/CommandHandler.hpp"

void CommandHandler::HandleTOPIC(const std::vector<std::string> &parts, Client & client, Server &server)
{
	if (parts.size() != 3)
		return client.sendCodeResponse(461, "Not enough parameters", "TOPIC");

	Channel *channel = server.getChannelByName(parts[1]);
	if (!channel)
		return client.sendCodeResponse(403, parts[1], "No such channel");
	if (!client.isOperatorIn(channel) && !channel->anyoneCanChangeTopic)
		return client.sendCodeResponse(482, "You're not channel operator", channel->name);
	
	channel->topic = parts[2];
	std::string userPrefix = ":" + client.nickname + "!" + client.username + "@" + "irctic.com";
	channel->broadcast(userPrefix + " TOPIC " + channel->name + " :" + channel->topic, server, &client); // TOPIC
	client.sendCodeResponse(332, channel->topic, channel->name); // RPL_TOPIC
}
