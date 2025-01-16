#include "../../inc/CommandHandler.hpp"

void CommandHandler::HandleMODE(const std::vector<std::string> &parts, Client & client, Server &server)
{
	if (parts.size() < 3 || parts.size() > 4)
		return client.sendCodeResponse(461, "Not enough parameters", "MODE");
	Channel *channel = server.getChannel(parts[1]);
	if (!client.isOperatorIn(channel))
		return client.sendCodeResponse(403, "No such channel", parts[1]);
	if (!channel)
		return client.sendCodeResponse(482, "You're not channel operator", parts[1]);

	std::string mode = parts[2];
	if (mode == "+i")
	{
		channel->inviteOnly = true;
		channel->broadcast(":irctic.com MODE " + channel->name + " +i", server); // RPL_CHANNELMODEIS
	}
	else if (mode == "-i")
	{
		channel->inviteOnly = false;
		channel->broadcast(":irctic.com MODE " + channel->name + " -i", server); // RPL_CHANNELMODEIS
	}
	else if (mode == "+t")
	{
		channel->anyoneCanChangeTopic = true;
		channel->broadcast(":irctic.com MODE " + channel->name + " +t", server); // RPL_CHANNELMODEIS
	}
	else if (mode == "-t")
	{
		channel->anyoneCanChangeTopic = false;
		channel->broadcast(":irctic.com MODE " + channel->name + " -t", server); // RPL_CHANNELMODEIS
	}
	else if (mode == "+k")
	{
		if (parts.size() != 4)
			return client.sendCodeResponse(461, "Not enough parameters", "MODE +k");
		channel->password = parts[3];
		channel->broadcast(":irctic.com MODE " + channel->name + " +k " + channel->password + "", server); // RPL_CHANNELMODEIS
	}
	else if (mode == "-k")
	{
		channel->password = "";
		channel->broadcast(":irctic.com MODE " + channel->name + " -k", server); // RPL_CHANNELMODEIS
	}
	else if (mode == "+l")
	{
		if (parts.size() != 4)
			return client.sendCodeResponse(461, "Not enough parameters", "MODE +l");
		channel->limit = std::stoi(parts[3]);
		channel->broadcast(":irctic.com MODE " + channel->name + " +l " + std::to_string(channel->limit) + "", server); // RPL_CHANNELMODEIS
	}
	else if (mode == "-l")
	{
		channel->limit = 0;
		channel->broadcast(":irctic.com MODE " + channel->name + " -l", server); // RPL_CHANNELMODEIS
	}
	else if (mode == "+o")
	{
		if (parts.size() != 4)
			return client.sendCodeResponse(461, "Not enough parameters", "MODE +o");
		unsigned int clientIdToOp = server.getClientIdByName(parts[3]);
		if (clientIdToOp > 0)
		{
			channel->addOperator(clientIdToOp);
			channel->broadcast(":irctic.com MODE " + channel->name + " +o " + parts[3] + "", server); // RPL_CHANNELMODEIS
		}
		else
			client.sendCodeResponse(401, "No such nick/channel", parts[3]);
	}
	else if (mode == "-o")
	{
		if (parts.size() != 4)
			return client.sendCodeResponse(461, "Not enough parameters", "MODE -o");
		unsigned int clientIdToDeop = server.getClientIdByName(parts[3]);
		if (clientIdToDeop > 0)
		{
			channel->removeOperator(clientIdToDeop);
			channel->broadcast(":irctic.com MODE " + channel->name + " -o " + parts[3] + "", server); // RPL_CHANNELMODEIS
		}
		else
			return client.sendCodeResponse(401, "No such nick/channel", parts[3]);
	}
	else
		return client.sendCodeResponse(501, "Unknown MODE flag", mode);
}
