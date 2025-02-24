#include <exception>
#include <iostream>
#include <cstring>
#include <cstdlib>
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

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
	size_t totalSize = size * nmemb;
	std::string* response = static_cast<std::string*>(userp);
	response->append(static_cast<char*>(contents), totalSize);
	return totalSize;
}

std::string Bot::ApiCall()
{
	CURL* curl;
	CURLcode res;
	std::string response;

	std::cout << "Making API call" << std::endl;

	curl_global_init(CURL_GLOBAL_DEFAULT);
	curl = curl_easy_init();

	if (curl)
	{
		curl_easy_setopt(curl, CURLOPT_URL, "https://icanhazdadjoke.com/");
		struct curl_slist* headers = NULL;
		headers = curl_slist_append(headers, "Accept: text/plain");
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
		res = curl_easy_perform(curl);
		if (res != CURLE_OK)
			response = "cURL request failed: " + std::string(curl_easy_strerror(res));
		curl_slist_free_all(headers);
		curl_easy_cleanup(curl);
	}
	curl_global_cleanup();
	return response;
}

int main(int argc, char *argv[])
{
	if (argc != 6)
	{
		std::cerr << "Usage: <programname> <ip> <port> <nick> <user> <password>" << std::endl;
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
		getBot().changeChannel("#jokes");
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
	std::cout << "Received message from " << user << " in channel " << channel << "= '" << message << "'" << std::endl;

	if (channel != "#jokes") return;

	Bot &bot = getBot();

	if (message.find("joke") != std::string::npos)
	{
		std::string response = bot.ApiCall();
		bot.directMessage(channel, response);
	}
	else if (message.find("help") != std::string::npos)
	{
		bot.directMessage(channel, "Type 'joke' to get a random dad joke. Yes thats all lol.");
	}
	else
	{
		bot.directMessage(channel, "Type 'help' for available commands.");
	}
}

// custom function to handle disconnect
void onDisconnect()
{
	std::cerr << "Disconnected from server!" << std::endl;
}

void onUserChannelJoin(std::string user, std::string channel)
{
	std::cout << "User " << user << " joined channel " << channel << std::endl;

	getBot().directMessage(user, "Welcome to the jokes channel, type 'joke' to get a random dad joke!");
}

void onUserChannelLeave(std::string user, std::string channel)
{
	std::cout << "User " << user << " left channel " << channel << std::endl;

	getBot().directMessage(user, "Goodbye " + user + "! Have a nice day!");
}
