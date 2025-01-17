#include "../../inc/CommandHandler.hpp"

void CommandHandler::HandleTOPIC(const std::vector<std::string> &parts, Client & client, Server &server)
{
	if (parts.size() != 3)
	{
		if (client.channel && client.channel->topic != "")
			return client.sendCodeResponse(331, client.channel->topic, client.channel->name);
		return client.sendCodeResponse(461, "Not enough parameters", "TOPIC");
	}

	Channel *channel = server.getChannel(parts[1]);
	if (!channel)
		return client.sendCodeResponse(403, parts[1], "No such channel");
	if (!client.isOperatorIn(channel) && !channel->anyoneCanChangeTopic)
		return client.sendCodeResponse(482, "You're not channel operator", channel->name);
	
	channel->topic = parts[2];
	channel->broadcast(":irctic.com 332 " + channel->name + " :" + channel->topic, server); // RPL_TOPIC
	client.sendCodeResponse(332, channel->topic, channel->name); // RPL_TOPIC
}
