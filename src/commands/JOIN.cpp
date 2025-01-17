#include "../../inc/CommandHandler.hpp"

void CommandHandler::HandleJOIN(const std::vector<std::string> &parts, Client & client, Server &server)
{
	if (!client.isAuthenticated)
		return client.sendCodeResponse(451, "You have not registered", "JOIN"); // ERR_NOTREGISTERED
	if (parts.size() < 2 || parts.size() > 3)
	{
		std::string response = ":irctic.com 461 JOIN :Not enough parameters"; // ERR_NEEDMOREPARAMS
		if (client.channelId && server.getChannelById(client.channelId.value()))
			response += ":irctic.com 442 " + server.getChannelById(client.channelId.value())->name + " :You are already on channel"; // ERR_USERONCHANNEL
		if (server.getChannels().size() > 0)
		{
			response += ":irctic.com 353 " + client.nickname + " = :"; // RPL_NAMREPLY
			for (auto &channel : server.getChannels())
				response += channel.name + " ";
			response += "\r\n";
			response += ":irctic.com 366 " + client.nickname + " :End of NAMES list"; // RPL_ENDOFNAMES
		}
		client.sendMessage(response);
		return ;
	}

	std::string channelName = parts[1];
	Channel *channel = server.getChannelByName(channelName);
	if (!channel)
	{
		if (channelName[0] != '#')
			return client.sendCodeResponse(476, "Invalid channel name", channelName); // ERR_BADCHANNELKEY
		server.createChannel(channelName);
		channel = server.getChannelByName(channelName);
	}

	if (channel->password != "" && (parts.size() < 3 || parts[2] != channel->password))
		return client.sendCodeResponse(475, "Cannot join channel (+k)", channelName); // ERR_BADCHANNELKEY
	
	if (channel->isKicked(client.id))
		return client.sendCodeResponse(474, "Cannot join channel (+b)", channelName); // ERR_BANNEDFROMCHAN

	channel->addMember(client.id, server);
	client.channelId = channel->id;
}
