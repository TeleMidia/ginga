/* Copyright (C) 2006-2017 PUC-Rio/Laboratorio TeleMidia

This file is part of Ginga (Ginga-NCL).

Ginga is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

Ginga is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
License for more details.

You should have received a copy of the GNU General Public License
along with Ginga.  If not, see <http://www.gnu.org/licenses/>.  */

#ifndef UDPCLIENT_H_
#define UDPCLIENT_H_

#if defined _MSC_VER || defined __CYGWIN__
# include <winsock2.h>
# include <ws2tcpip.h>
#else
# include <sys/socket.h>
# include <netinet/in.h>
# include <sys/ioctl.h>
# include <arpa/inet.h>
# include <netdb.h>
# include <net/if.h>
# include <netdb.h>
#endif

class UDPClient {

protected:

#if defined _MSC_VER || defined __CYGWIN__
	SOCKET sd;
#else
	int sd;
#endif
	struct sockaddr_in myaddr, remaddr;

public:
	UDPClient();
	~UDPClient();

	bool connectSocket(string host, int port);
	int send(char* buff, unsigned int size);
	int receive(char** buff);
	void closeSocket();
	int dataAvailable(int timeout);

};


#endif /* UDPCLIENT_H_ */
