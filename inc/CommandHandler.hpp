#pragma once

#include <string>
#include <vector>
#include <sstream>
#include "../inc/Server.hpp"

class CommandHandler {
public:
	static void HandlePASS(const std::vector<std::string> &parts, Client & client, Server &server);
	static void HandleCAP(const std::vector<std::string> &parts, Client & client, Server &server);
	static void HandlePING(const std::vector<std::string> &parts, Client & client, Server &server);
	static void HandleOPER(const std::vector<std::string> &parts, Client & client, Server &server);
	static void HandleNICK(const std::vector<std::string> &parts, Client & client, Server &server);
	static void HandleUSER(const std::vector<std::string> &parts, Client & client, Server &server);
	static void HandleJOIN(const std::vector<std::string> &parts, Client & client, Server &server);
	static void HandleQUIT(const std::vector<std::string> &parts, Client & client, Server &server);
	static void HandlePART(const std::vector<std::string> &parts, Client & client, Server &server);
	static void HandlePRIVMSG(const std::vector<std::string> &parts, Client & client, Server &server);
	static void HandleKICK(const std::vector<std::string> &parts, Client & client, Server &server);
	static void HandleINVITE(const std::vector<std::string> &parts, Client & client, Server &server);
	static void HandleTOPIC(const std::vector<std::string> &parts, Client & client, Server &server);
	static void HandleMODE(const std::vector<std::string> &parts, Client & client, Server &server);
	static void HandleDEBUG(const std::vector<std::string> &parts, Client & client, Server &server);

	static void HandleCommand(const std::string &inCommand, unsigned int clientId, Server &server);

private:
	static void HandleDCC(std::string target, std::string msg, Client & client, Server &server);

	// Helper functions
	static std::vector<std::string> split(const std::string &str, char delim);
	static void CompleteHandshake(Client & client);
};

typedef void (*CommandFunc)(const std::vector<std::string> &, Client &, Server &);
struct CommandMapping
{
	std::string command;
	CommandFunc func;
};
