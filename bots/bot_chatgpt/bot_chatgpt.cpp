#include "bot_chatgpt.hpp"
#include <iostream>

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
	size_t totalSize = size * nmemb;
	std::string* response = static_cast<std::string*>(userp);
	response->append(static_cast<char*>(contents), totalSize);
	return totalSize;
}

std::string Bot_ChatGPT::ApiCall(const std::string &prompt)
{
	CURL* curl = curl_easy_init();
	if (!curl) 
		return "Error: unable to init cURL";

	std::string response;
	curl_global_init(CURL_GLOBAL_DEFAULT);
	struct curl_slist* headers = NULL;
	
	curl_easy_setopt(curl, CURLOPT_URL, "https://api.openai.com/v1/chat/completions");
	headers = curl_slist_append(headers, "Content-Type: application/json");
	headers = curl_slist_append(headers, "Authorization: Bearer API_KEY");
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

	std::string jsonBody =
		"{"
			"\"model\":\"gpt-3.5-turbo\","
			"\"messages\":[{\"role\":\"user\",\"content\":\"" + prompt + "\"}],"
			"\"max_tokens\":150"
		"}";

	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonBody.c_str());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

	CURLcode res = curl_easy_perform(curl);
	if (res != CURLE_OK)
		return "cURL request failed: " + std::string(curl_easy_strerror(res));

	curl_slist_free_all(headers);
	curl_easy_cleanup(curl);
	curl_global_cleanup();

	if (response.empty())
		return "Error: Empty response from API";

	size_t errorPos = response.find("\"error\":");
	if (errorPos != std::string::npos)
	{
		size_t msgStart = response.find("\"message\":\"", errorPos);
		if (msgStart != std::string::npos)
		{
			msgStart += 11;
			size_t msgEnd = response.find("\"", msgStart);
			if (msgEnd != std::string::npos)
				return "API Error: " + response.substr(msgStart, msgEnd - msgStart);
		}
		return "API Error: Unknown error format in response";
	}

	size_t contentPos = response.find("\"content\": \"");
	if (contentPos == std::string::npos)
		return "Error: Could not find content in message";

	contentPos += 12;
	size_t endPos = response.find("\"", contentPos);
	if (endPos == std::string::npos)
		return "Error: Malformed response - missing closing quote";

	std::string content = response.substr(contentPos, endPos - contentPos);
	
	size_t pos = 0;
	while ((pos = content.find("\\n", pos)) != std::string::npos)
	{
		content.replace(pos, 2, "\n");
		pos += 1;
	}

	return content;
}

int main(int argc, char *argv[])
{
	if (argc != 6)
	{
		std::cerr << "Usage: <./bot_binary> <host> <port> <password> <user> <message>" << std::endl;
		return 1;
	}

	try
	{
		Bot_ChatGPT bot;
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
		std::string response = bot.ApiCall(argv[5]);
		bot.sendMessage(response); //is this right? if not just put the correct function there -> goal is to send it back to the user
		
		return 0;
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}
}
