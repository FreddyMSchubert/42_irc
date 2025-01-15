#include "bot_dadjokes.hpp"

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
	size_t totalSize = size * nmemb;
	std::string* response = static_cast<std::string*>(userp);
	response->append(static_cast<char*>(contents), totalSize);
	return totalSize;
}

std::string Bot_DadJokes::ApiCall()
{
	CURL* curl;
	CURLcode res;
	std::string response;

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
	if (argc != 5)
	{
		std::cerr << "Usage: <./bot_binary> <host> <port> <password> <user>" << std::endl;
		return 1;
	}

	try
	{
		Bot_DadJokes bot;
		bot.setIp(argv[1]);
		bot.setPort(std::stoi(argv[2]));
		bot.setPassword(argv[3]);
		bot.setUser(argv[4]);

		bot.setCallbacks(
			[]() {std::cout << "Connected to server" << std::endl;},
			[](std::string err) {std::cerr << "Error: " << err << std::endl;},
			[](std::string user, std::string channel, std::string msg) {std::cout << "Message from " << user << " in " << channel << ": " << msg << std::endl;},
			[]() {std::cout << "Disconnected from server" << std::endl;},
			[](std::string user, std::string channel) {std::cout << "User " << user << " joined channel " << channel << std::endl;},
			[](std::string user, std::string channel) {std::cout << "User " << user << " left channel " << channel << std::endl;}
		);

		bot.connectToServer();
		bot.authenticate();
		std::string response = bot.ApiCall();
		bot.sendMessage("#gpt", response); //is this right? if not just put the correct function there -> goal is to send it back to the user
		
		return 0;
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}
}
