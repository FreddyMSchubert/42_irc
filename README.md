# ***IRC*** by [FreddyMSchubert](https://github.com/FreddyMSchubert) && [Reptudn](https://github.com/Reptudn) && [NoelSabia](https://github.com/NoelSabia)

![Boundary](https://github.com/FreddyMSchubert/42_cub3d/blob/master/assets/readme/boundaries/boundary-2.png)

# Installation

*(The program was tested on Mac & Linux.)*

Simply run this command:

`git clone https://github.com/FreddyMSchubert/42_irc.git irc && cd irc && make run`

Now you have the compiled executable called "ircserv" in your current folder.

![Boundary](https://github.com/FreddyMSchubert/42_cub3d/blob/master/assets/readme/boundaries/boundary-3.png)

# Project introduction

> Internet Relay Chat or IRC is a text-based communication protocol on the Internet. \
> It offers real-time messaging that can be either public or private. Users can exchange direct messages and join group channels. \
> IRC clients connect to IRC servers in order to join channels. IRC servers are connected together to form a network.

That's right, instead of using Discord, we're going full retro and building the tech my dad used in his youth.

![Boundary](https://github.com/FreddyMSchubert/42_cub3d/blob/master/assets/readme/boundaries/boundary-4.png)

# The task

> Here's [the subject](https://github.com/FreddyMSchubert/42_irc/blob/main/en.subject.pdf).

The task is to make a server, not a client, with:
- multiple clients
- no hanging, no blocking
- only 1 poll to handle all I/O operations
- TCP/IP

We chose [`IRSSI`](https://formulae.brew.sh/formula/irssi) as our client.

Required functionalities:
- Nickname & Username setting
- Authentication & Handshake
- Messaging into channels & to users directly
- Global operators, local channel operators, and normal users
- Client kicking / banning
- Client inviting
- Channel Topics
- Channel Settings modification using MODE
	- invite only channel
	- operator only topic modification
	- channel password
	- local channel operator privilege
	- channel user limit

The server should be able to handle receiving partial data, low bandwidth, ...

## The bonus

Add:
- File Transfer
- A bot.

![Boundary](https://github.com/FreddyMSchubert/42_cub3d/blob/master/assets/readme/boundaries/boundary-1.png)

# The Approach / Implementation

Since we started making an HTTP Webserver and only switched after working on this for a while, we already had a fully functional socket networking system. Porting that over was easy.

The primary headache of this project was it then to make sure all of the required features worked well together and that they all returned functional status codes back to IRSSI so the process would work out smoothly.

But since the overall workload was pretty managable after having networking done after an hour, we finished this project within 10 days.

## The Bonus

Since we always like to do a little extra, we focused on cool bots this time. Many other 42 irc projects implement bots and file transfer in very questionable ways.

For the bots, many just added a bot command, that would return something unique. For file transfer, some literally read out the file from the servers file system, which is both a security nightmare and hilarious because it is super wrong but nobody would notice in an evaluation.

For file transfer, we could luckily rely on irssi to handle the heavy lifting, just forwarding the file sending requests. The actual data transfer was done between the clients without the servers involvement.

We implemented the bots as their own standalone programs which connect to the server using their own socket. This was fun, and we were working on bot event handlers, which is absolutely not required by the subject, but the bots turned out cool.

We then made three bots to show them off a bit:

1. The most basic bot, a dad joke bot that returns a random dad joke from a free api.
2. ChatGPT bot, so we had someone to talk to even if nobody joined the server.
3. A TicTacToe bot, to show off session management and more complex interactions between multiple users.

![Boundary](https://github.com/FreddyMSchubert/42_cub3d/blob/master/assets/readme/boundaries/boundary-2.png)

# Supported commands

Finally, here's a list of all supported commands you can try:

| **Command** | **Description**                                                                                                                                                  |
|-------------|------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| `PASS`      | Sets a password to authenticate the client with the IRC server before registering.                                                                               |
| `CAP`       | Handles IRC capabilities negotiation, allowing clients and servers to agree on optional features.                                                                |
| `PING`      | Checks the connectivity between the client and server, ensuring the server is still responsive.                                                                  |
| `OPER`      | Grants operator privileges to a user, allowing them to perform administrative tasks on the server.                                                               |
| `NICK`      | Sets or changes the client's nickname on the IRC network.                                                                                                        |
| `USER`      | Registers the client's username, hostname, server name, and real name with the IRC server during the initial connection.                                         |
| `JOIN`      | Allows a client to join one or more channels on the IRC server.                                                                                                  |
| `QUIT`      | Disconnects the client from the IRC server, optionally providing a quit message.                                                                                 |
| `PART`      | Removes the client from one or more channels they have joined.                                                                                                   |
| `PRIVMSG`   | Sends a private message to another user or a message to a channel. Also handles `file transfer`.                                                                 |
| `KICK`      | Removes a user from a channel, optionally providing a reason for the kick.                                                                                       |
| `INVITE`    | Invites a user to join a specific channel.                                                                                                                       |
| `TOPIC`     | Sets or retrieves the topic of a channel.                                                                                                                        |
| `MODE`      | Changes or queries the mode of a user or a channel, such as setting channel restrictions or user privileges.                                                     |
| `DEBUG`     | Enables or displays debugging information for the server, useful for development and troubleshooting purposes.                                                   |
