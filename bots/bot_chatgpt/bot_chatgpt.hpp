#pragma once

#include "../../bot_template/Bot.hpp"
#include <string>
#include <curl/curl.h>

class Bot_ChatGPT : public Bot
{
	public:
		Bot_ChatGPT(
			const std::string& host = "127.0.0.1",
			int port = 6667,
			const std::string& password = "password",
			const std::string& nick = "chatgpt",
			const std::string& user = "chatgpt"
		) {
			setIp(host);
			setPort(port);
			setPassword(password);
			setNick(nick);
			setUser(user);
		}
		std::string ApiCall(const std::string &prompt);
};
