//server for networking coursework
//will use UDP
//last edit 15-11-2017

//still to add - make non-blocking for reading - use lab5 and own client for ref. 
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <vector>
#include <iostream>
#include <sstream>
#include<string>
#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <chrono>

#pragma comment(lib, "ws2_32.lib")

#include "Connections.h"
#include "playerData.h"
using std::string;
using std::stringstream;

using std::chrono::duration_cast;
using std::chrono::milliseconds;
typedef std::chrono::steady_clock the_clock;
using std::vector;

//global variables
the_clock::time_point start;
int messageCounter;
bool addPlayer;
vector <playerData> allPlayers;
vector <Connection*> clientConns;
vector <int> disconnectedClients;

playerData recievedData;

SOCKET sock;

//IP server address
#define SERVERIP "127.0.0.1"

//server UDP port no
#define SERVERPORT 4444

//fixed message size - this might need changed
#define MESSAGESIZE 40

void die(const char *message); //used for error reporting

bool operator==(const sockaddr_in& left, const sockaddr_in& right)
{
	return (left.sin_port == right.sin_port)
		&& (memcmp(&left.sin_addr, &right.sin_addr, sizeof(left.sin_addr)) == 0);
}

bool operator<(const sockaddr_in& left, const sockaddr_in& right)
{
	int c = memcmp(&left.sin_addr, &right.sin_addr, sizeof(left.sin_addr));
	if (c < 0)
		return true;
	else if (c == 0)
		return left.sin_port < right.sin_port;
	else
		return false;
} 


void initLibrary()
{
	//init winsock library
	WSADATA w;
	int error = WSAStartup(0x0202, &w);
	if (error != 0)
	{
		die("WSAStartup failed");
	}
	if (w.wVersion != 0x0202)
	{
		die("Wrong Winsock version");
	}
}


void checkClientList(sockaddr_in addr_in_)
{
	addPlayer = false;
	for (int i = 0; i < clientConns.size(); ++i)
	{
		if (clientConns[i]-> getAddr_in() == addr_in_)
		{
			printf("socket allready in list");
			return;
		}
	}

	clientConns.push_back(new Connection(addr_in_));
	addPlayer = true;
	printf("socket added to list");

}

void sendToClient(char buffer[MESSAGESIZE], int n)
{
	if (sendto(sock, buffer, MESSAGESIZE, 0,
		(const sockaddr *)&clientConns[n - 1]->getAddr_in(), sizeof(clientConns[n - 1])->getAddr_in()) != MESSAGESIZE)
	{
		die("sendto failed");
	}

}

int getTimeNow()
{
	auto t = the_clock::now().time_since_epoch();
	int ms = duration_cast<milliseconds>(t).count();
	return ms;
}

void sendPlayerNo(int n)
{
	//first connection message to a new client
	//will set the player number and send the server time to start the client clock
	char buffer[MESSAGESIZE];
	int x = allPlayers[n - 1].getPosX(0);
	int y = allPlayers[n - 1].getPosY(0);
	int startTime = int(the_clock::now);
	int len = sprintf_s(buffer, "%02d#%03d,%03d#%03d#%d&", n, x, y, messageCounter,startTime);  //this will not increase the message counter as only communicates to 1 player

	memset(&buffer[len], '-', MESSAGESIZE - len);
	
	sendToClient(buffer, n);
}

void createUpdates()
{
	char buffer[MESSAGESIZE];
	int x;
	int y;
	messageCounter += 1;
	int msgTime = getTimeNow();
	bool ignore;
	//send an update to everyplayer 
	for (int i = 0; i < allPlayers.size(); i++)
	{
		ignore = false;
		for (int d = 0; d < disconnectedClients.size(); d++)
		{
			if (i == disconnectedClients[d])
			{
				ignore = true;
			}
		}

		//about everyplayer
		if (ignore == false)
		{
			for (int j = 0; j < allPlayers.size(); j++)
			{
				for (int d = 0; d < disconnectedClients.size(); d++)
				{
					if (j == disconnectedClients[d])
					{
						ignore = true;
					}
				}
				if (ignore == false)
				{
					x = allPlayers[j].getPosX(0);
					y = allPlayers[j].getPosY(0);

					int len = sprintf_s(buffer, "%02d#%03d,%03d#%03d#%d&", (j + 1), x, y, messageCounter, msgTime);
					memset(&buffer[len], '-', MESSAGESIZE - len);
					sendToClient(buffer, (i + 1)); // (i+1) used for client address - send an update about each
				}
			}
		}
	}


}


void readMessage(char buffer[MESSAGESIZE])
{
	//RESEARCH FSCAN for this - tidy up the code - job for later
	
	/*MESSAGE STRUCTURE
	player no = char 0,1
	xpos = char 3,4,5 (int)
	ypos = char 7,8,9 (int)
	msgNo = 11,12,13 (ints) allows 1000 messages to be sent
	msgTime = 15 onwards;
	*/
	bool messageEnd = false;
	int messEndpt = 0;
	int msgTime;

	//check for end point of message
	while ((messageEnd == false) && (messEndpt < MESSAGESIZE))
	{
		if (buffer[messEndpt] == '&')
		{
			messageEnd = true;
		}
		else
		{
			messEndpt++;
		}
	}
	//set message time
	string timeRead(buffer + 15, buffer + messEndpt);
	stringstream thing(timeRead);
	thing >> msgTime;

	//get rest of data from message recived
	int x = (100*int(buffer[3]-48)) +(10 * int(buffer[4]-48)) + int(buffer[5]-48);
	int y = (100 * int(buffer[7]-48)) + (10 * int(buffer[8]-48)) + int(buffer[9]-48);
	recievedData.setPos(0, x, y);
	recievedData.setMsgNo((100 * int(buffer[11] - 48)) + (10 * int(buffer[12] - 48) + int(buffer[13] - 48)));
	recievedData.setRecieved(getTimeNow());

	//get playerNo which will be vector pos(+1)
	//do this after as it is the only bit which varies for new connections
	if (addPlayer)
	{
		recievedData.setPlayerNo(clientConns.size());
		recievedData.setLastMsgTime(0, msgTime);
		allPlayers.push_back(playerData(recievedData));
		//send player no to client
		addPlayer = false;
		sendPlayerNo(recievedData.getNo());
	}
	else
	{
		recievedData.setPlayerNo((10 * int(buffer[0] - 48)) + int(buffer[1] - 48));
		int no = recievedData.getNo() -1;
		if (recievedData.getMsgNo() > allPlayers[recievedData.getNo() - 1].getMsgNo())
		{
			int tempTimes[3];
			for (int t = 0; t < 3; t++)
			{
				tempTimes[0] = allPlayers[no].getlastMsgTime(0);
			}
		// check if results need moved
		for (int i = 2; i >=0; i--)
		{
			if (i != 0)
			{
				recievedData.setLastMsgTime(i, tempTimes[i - 1]);             //////////// why is 2 not being set correctly
			}
			else
			{
				recievedData.setLastMsgTime(i, msgTime);
			}
		}
		//update record
		allPlayers[recievedData.getNo() - 1] = recievedData;
		}
	}
	
}

void listen()
{
	char buffer[MESSAGESIZE];

	printf("Waiting for a message...\n");

	sockaddr_in fromAddr;
	int fromAddrSize = sizeof(fromAddr);
	int count = recvfrom(sock, buffer, MESSAGESIZE, 0,
		(sockaddr *)&fromAddr, &fromAddrSize);
	if (count < 0)
	{
		//die("recvfrom failed");
		return;
	}
	if (count != MESSAGESIZE)
	{
		die("received odd-sized message");
	}

	checkClientList(fromAddr);
	readMessage(buffer);

	printf("Received %d bytes from address %s port %d: '",
		count, inet_ntoa(fromAddr.sin_addr), ntohs(fromAddr.sin_port));
	fwrite(buffer, 1, count, stdout);
	printf("'\n");
}

void checkForDead()
{
	//check for clients with no recent updates
	for (int i = 0; i < clientConns.size(); i++)
	{
		bool ignore = false;
		for (int d = 0; d < disconnectedClients.size(); d++)
		{
			if (i == disconnectedClients[d])
			{
				ignore = true;
			}
		}
		if (ignore == false)
		{

			
			int timeSinceLast = getTimeNow() - allPlayers[i].getRecieved();  //using time message recieved to counteract issues with different timing systems
			if (timeSinceLast >= 50000)
			{
			printf("will remove missing client from list");
			//stop updating?
			disconnectedClients.push_back(i);
			}
			
		}
	}
}

void runPredictions(int i)
{
		if (allPlayers[i].getlastMsgTime(1) != 0)  //can only predict if more than one message has been created

		{
			allPlayers[i].linerPrediction(getTimeNow());
			printf("prediction x%3dy%3d", allPlayers[i].getXpre(), allPlayers[i].getYpre());
		}
}

int main()
{
	// Start timing
	start = the_clock::now(); //timer to control updates and gametime
	timeval timeout;   //select time out
	timeout.tv_sec = 0;
	timeout.tv_usec = 0;

	the_clock::time_point lastMsgTime;
	the_clock::time_point lastPrediction;

	printf("game server running \n\n\n");
	initLibrary();
	//create UDP socket
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock == INVALID_SOCKET)
	{
		die("socket failed");
	}

	//fill out and describe socket address
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = inet_addr(SERVERIP);
	
	//convert to bigendian
	serverAddr.sin_port = htons(SERVERPORT);

	//bind socket to address
	if (bind(sock, (const sockaddr*)&serverAddr, sizeof(serverAddr)) != 0)
	{
		die("bind failed");
	}
	//display binding message
	printf("Server socket bound to address %s, port %d\n\n", inet_ntoa(serverAddr.sin_addr), ntohs(serverAddr.sin_port));

	char buffer[MESSAGESIZE];

	while (true)
	{
		//printf("Waiting for a message...\n");
		
		fd_set readable;
		FD_ZERO(&readable);

		if (true)
		{
			FD_SET(sock, &readable);
		}

		int count = select(0, &readable, NULL, NULL, &timeout);
		if (count == SOCKET_ERROR)
		{
			die("select failed");
		}

		if (count == 1)
		{
			listen();
		}
		
		auto timeSince = duration_cast<milliseconds>(the_clock::now() - lastMsgTime).count();

		if ((timeSince > 3000) && clientConns.size()!=0)
		{
			createUpdates();
			lastMsgTime = the_clock::now();
		}
		checkForDead();

		for (int i = 0; i < clientConns.size(); i++)
		{
			
			if ((getTimeNow() - allPlayers[i].getRecieved())>2000)
			{
				int timeBetween = getTimeNow() - allPlayers[i].getLastPred();
				if (timeBetween >= 1000)
				{
					printf("will run prediction\n");
					runPredictions(i);
					allPlayers[i].setLastPred(getTimeNow());
				}
			}
		}
	}

	// We won't actually get here, but if we did then we'd want to clean up...
	printf("Quitting\n");
	closesocket(sock);
	WSACleanup();
	return 0;
}

void die(const char *message)
{
	fprintf(stderr, "Error: %s (WSAGetLastError() = %d)\n", message, WSAGetLastError());

#ifdef _DEBUG
	// Debug build -- drop the program into the debugger.
	abort();
#else
	exit(1);
#endif
}