//class to hold connection info for clients
//last edit 08-11-17
#ifndef CONNECTION_H
#define CONNECTION_H

#include <WinSock2.h>
class Connection {
public:
	Connection(sockaddr_in);

	sockaddr_in& getAddr_in()
	{
		return addr_in;
	}

private:
	sockaddr_in addr_in;
	

};
#endif