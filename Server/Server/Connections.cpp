#include "Connections.h"
#include <iostream>
using std::cout;

//constructor - sock_ clients socket
Connection::Connection(sockaddr_in sockAddr_in ) :addr_in(sockAddr_in)
{
	cout << "new connection\n";
}


