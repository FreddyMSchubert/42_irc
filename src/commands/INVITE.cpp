#include "../../inc/CommandHandler.hpp"

void CommandHandler::HandleINVITE(const std::vector<std::string> &parts, Client & client, Server &server)
{
	if (parts.size() != 3)
		return client.sendCodeResponse(461, "Not enough parameters", "INVITE");
	Channel *channel = server.getChannelByName(parts[2]);
	if (!channel)
		return client.sendCodeResponse(403, "No such channel", parts[2]);
	if (!client.isOperatorIn(channel))
		return client.sendCodeResponse(482, "You're not channel operator", parts[2]);

	std::string userToInvite = parts[1];
	Client *clientToInvite = server.getClientByName(userToInvite);
	if (!clientToInvite)
		return client.sendCodeResponse(401, "No such nick/channel", userToInvite);
	channel->inviteMember(clientToInvite->id, server);

	clientToInvite->sendMessage(":irctic.com INVITE " + userToInvite + " " + channel->name + " :You've been invited to the channel"); // RPL_INVITE
	client.sendCodeResponse(341, "Invited", "INVITE " + userToInvite + " " + channel->name);
}
