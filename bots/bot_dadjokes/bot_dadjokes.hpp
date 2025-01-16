#ifndef BOT_DADJOKES_HPP
#define BOT_DADJOKES_HPP

#include "../../bot_template/Bot.hpp"
#include <string>
#include <curl/curl.h>
#include <iostream>

class Bot_DadJokes : public Bot
{
	public:
		std::string ApiCall();
};

#endif