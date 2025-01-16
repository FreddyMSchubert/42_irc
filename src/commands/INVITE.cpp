#include "../../inc/CommandHandler.hpp"

std::string CommandHandler::HandleINVITE(const std::vector<std::string> &parts, Client & client, Server &server)
{
	if (parts.size() != 3)
		return ":irctic.com 461 INVITE :Not enough parameters"; // ERR_NEEDMOREPARAMS
	Channel *channel = server.getChannel(parts[2]);
	if (!channel)
		return ":irctic.com 403 " + parts[2] + " :No such channel"; // ERR_NOSUCHCHANNEL
	if (!client.isOperatorIn(channel))
		return ":irctic.com 482 " + parts[2] + " :You're not channel operator"; // ERR_CHANOPRIVSNEEDED

	std::string userToInvite = parts[1];
	Client *clientToInvite = server.getClientByName(userToInvite);
	if (!clientToInvite)
		return ":irctic.com 401 " + userToInvite + " :No such nick/channel"; // ERR_NOSUCHNICK
	channel->inviteMember(clientToInvite->id, server);

	clientToInvite->sendMessage(":irctic.com INVITE " + userToInvite + " " + channel->name + " :You've been invited to the channel"); // RPL_INVITE
	return ":irctic.com 341 INVITE " + userToInvite + " " + channel->name + " :Invited";
}
