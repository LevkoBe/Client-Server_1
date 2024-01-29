### Transmission protocol documentation

My realization of connection between client and server uses TCP protocol, adding some extra-features.
First of all, each data transmission should follow the next guideline:

Each time we should send, as well as receive the next:

| Type of operation | CHAR | 1 B |
| --- | --- | --- |
| Size of chunk | INT | 4 B |
| Size of message | INT | 4 B |
| Message | CHAR* | x B |

**Type of operation** is the first message that is being sent, and it represent some predefined command, that should be recognizable by the receiver. Alternatively, it can be ignored, if all the receiver needs is to receive some data.

**Size of chunk** is an integer, representing the size of chunks in which the message sent is being divided. May be used by receiver to get data in the same-sized chunks as ones being sent.

**Size of message** is also an integer and defines the size of the whole message that needs to be received. Vital for avoiding errors in receiving partial data, receiving data from another message, or expecting to receive data when it’s no longer being send.

**Message** is any amount of characters stored in char pointer type. That is the message with specified size, which is expected by the receiver.
Here's a simple visualizations related to my program:
![image](https://github.com/LevkoBe/Client-Server_1/assets/118983753/9d76243e-4c13-4f5e-91ae-fab8ad0f3621)
### And use case diagram:
![image](https://github.com/LevkoBe/Client-Server_1/assets/118983753/7f419b92-b8b0-444a-811b-2919ccdb6000)

And in each use case, client sends a message to the server, and receives a response.
