#include <exception>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <string>
#include <unistd.h>
#include <arpa/inet.h>
#include "Bot.hpp"

void onMessage(std::string user, std::string channel, std::string message);
void onDisconnect();
void onConnect();
void onError(std::string message);
void onUserChannelJoin(std::string user, std::string channel);
void onUserChannelLeave(std::string user, std::string channel);

Bot &getBot()
{
	static Bot bot = Bot();

	return bot;
}

// TODO: fix that callbacks are not being called correctly
int main(int argc, char *argv[])
{
	if (argc != 6)
	{
		std::cerr << "Usage: " << argv[0] << " <ip> <port> <nick> <user> <password>" << std::endl;
		return 1;
	}

	try
	{
		Bot &bot = getBot();

		bot.setCallbacks(
				onConnect,
				onError,
				onMessage,
				onDisconnect,
				onUserChannelJoin,
				onUserChannelLeave
			);

		bot.setIp(argv[1]);
		bot.setPort(std::stoi(argv[2]));
		bot.setNick(argv[3]);
		bot.setUser(argv[4]);
		bot.setPassword(argv[5]);

		bot.connectToServer();

	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return 1;
	}

	return 0;
}

// custom function to handle connection
void onConnect()
{
	std::cout << "Connected to IRC Server!" << std::endl;
	try {
		std::cout << "Trying to athenticate." << std::endl;
		getBot().authenticate();
		getBot().changeChannel("#bot");
		getBot().startPollingForEvents();
	} catch (std::exception& e) {
		std::cerr << "Failed to authenticate: " << e.what() << std::endl;
	}
}

// custom function to handle errors
void onError(std::string message)
{
	std::cerr << "ERROR: " << message << std::endl;
}

// custom function to handle messages
void onMessage(std::string user, std::string channel, std::string message)
{
	std::cout << "Message received:\n";
	std::cout << "User:\t\t" << user << "\n";
	std::cout << "Channel:\t\t" << channel << "\n";
	std::cout << "Message:\t\t" << message << std::endl;

	Bot &bot = getBot();

	if (channel != "#bot")
	{
		bot.directMessage(user, "Please send messages to #bot channel only.");
		return;
	}

	if (message == "ping")
		bot.directMessage(user, "pong");
	else if (message == "hello")
		bot.directMessage(user, "Hello! How are you?");
	else if (message == "bye")
		bot.directMessage(user, "Goodbye! Have a nice day!");
	else if (message == "help")
		bot.directMessage(user, "Available commands: ping, hello, bye, help");
	else
		bot.directMessage(user, "Hello " + user + "! You said: " + message);

	// bot.directMessage(user, "Hello! You said: " + message);
}

// custom function to handle disconnect
void onDisconnect()
{
	std::cerr << "Disconnected from server!" << std::endl;
}

void onUserChannelJoin(std::string user, std::string channel)
{
	std::cout << "User " << user << " joined channel " << channel << std::endl;

	getBot().directMessage(channel, "Welcome " + user + " to the channel! Type 'help' for available commands.");
}

void onUserChannelLeave(std::string user, std::string channel)
{
	std::cout << "User " << user << " left channel " << channel << std::endl;

	getBot().directMessage(user, "Goodbye " + user + "! Have a nice day!");
}