# Irssi Command Reference

> irssi -c localhost -p 6667 -w password --config "~/.irssi/config"

## 1. Basic Irssi Commands

| **Action**                  | **Irssi Command**                            | **Description**                                           |
|-----------------------------|----------------------------------------------|-----------------------------------------------------------|
| **Connect to a Server**     | `/connect irctic.com [port] [password]`                 | Connect to your custom IRC server. Replace `[port]` or `[password]`if needed. |
| **Disconnect from Server** | `/disconnect` or `/disconnect irctic.com`    | Disconnect from the current IRC server.                   |
| **Set Nickname**            | `/nick YourNickname`                         | Change your current nickname to `YourNickname`.           |
| **Set Username**                 | `/user yourusername 0 * :Your Real Name`     | Send the `USER` command to set your username and real name. |
| **Join a Channel**          | `/join #channelname [key]`                    | Join or create a channel named `#channelname`. Include `[key]` if the channel is password-protected. |
| **Leave a Channel**         | `/part #channelname [reason]`                 | Leave the specified channel. Optionally provide a reason. |
| **Send a Private Message**  | `/msg nickname Your message here`             | Send a private message to `nickname`.                     |
| **Broadcast a Message**     | `/say #channelname Your message here`         | Send a message to all members in `#channelname`.          |
| **Become an Operator**           | `/oper yourOperatorPassword`                 | Authenticate as an IRC operator using the `OPER` command. |
| **Kick a User from Channel**     | `/kick #channelname nickname :Reason`        | Use the `KICK` command to remove `nickname` from `#channelname` with an optional reason. |
| **Invite a User to Channel**     | `/invite nickname #channelname`              | Use the `INVITE` command to invite `nickname` to `#channelname`. |
| **Change Channel Topic**         | `/topic #channelname New topic here`          | Use the `TOPIC` command to set a new topic for `#channelname`. |
| **Modify Channel Modes**         | `/mode #channelname +i`<br>`/mode #channelname -k key` | Use the `MODE` command to set or unset channel modes like invite-only (`+i`) or set/remove a channel key (`+k`/`-k`). |
| **Quit Irssi**              | `/quit [message]`                             | Exit Irssi with an optional quit message.                 |
| **View Help**               | `/help` or `/help command`                    | Display help information for commands.                    |

## 2. Commands for Your Custom IRC Server

| **Action**                       | **Irssi Command**                            | **Description**                                           |
|----------------------------------|----------------------------------------------|-----------------------------------------------------------|
| **List Users in Channel**        | `/names #channelname`                        | Send the `NAMES` command to list all users in `#channelname`. *(Note: Irssi handles this automatically when you join a channel.)* |
| **View Your Operators Status**   | `/whois YourNickname`                         | Use the `WHOIS` command to check if you have operator status. |
