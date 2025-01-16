#pragma once

#include "../../bot_template/Bot.hpp"
#include <string>
#include <curl/curl.h>

class Bot_ChatGPT : public Bot
{
	public:
		std::string ApiCall(const std::string &prompt);
};
