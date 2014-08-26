/******************************************************************************
Este arquivo eh parte da implementacao do ambiente declarativo do middleware
Ginga (Ginga-NCL).

Direitos Autorais Reservados (c) 1989-2007 PUC-Rio/Laboratorio TeleMidia

Este programa eh software livre; voce pode redistribui-lo e/ou modificah-lo sob
os termos da Licenca Publica Geral GNU versao 2 conforme publicada pela Free
Software Foundation.

Este programa eh distribuido na expectativa de que seja util, porem, SEM
NENHUMA GARANTIA; nem mesmo a garantia implicita de COMERCIABILIDADE OU
ADEQUACAO A UMA FINALIDADE ESPECIFICA. Consulte a Licenca Publica Geral do
GNU versao 2 para mais detalhes.

Voce deve ter recebido uma copia da Licenca Publica Geral do GNU versao 2 junto
com este programa; se nao, escreva para a Free Software Foundation, Inc., no
endereco 59 Temple Street, Suite 330, Boston, MA 02111-1307 USA.

Para maiores informacoes:
ncl @ telemidia.puc-rio.br
http://www.ncl.org.br
http://www.ginga.org.br
http://www.telemidia.puc-rio.br
******************************************************************************
This file is part of the declarative environment of middleware Ginga (Ginga-NCL)

Copyright: 1989-2007 PUC-RIO/LABORATORIO TELEMIDIA, All Rights Reserved.

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License version 2 as published by
the Free Software Foundation.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License version 2 for more
details.

You should have received a copy of the GNU General Public License version 2
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA

For further information contact:
ncl @ telemidia.puc-rio.br
http://www.ncl.org.br
http://www.ginga.org.br
http://www.telemidia.puc-rio.br
*******************************************************************************/

#include "player/UDPClient.h"
#include <unistd.h>
#include <string.h>

UDPClient::UDPClient() {
	sd = 0;
}

UDPClient::~UDPClient() {
	closeSocket();
}

bool UDPClient::connectSocket(string host, int port) {
#if defined _WIN32 || defined __CYGWIN__
	WORD L_Ver;
	WSADATA wsaData;

	L_Ver = MAKEWORD(2, 2);

	int L_Err = WSAStartup(L_Ver, &wsaData);
	if (L_Err != 0) {
		cout << " Can not find winsock dll!" << endl;
		return false;
	}

	if (LOBYTE(wsaData.wVersion) < 2 || HIBYTE(wsaData.wVersion) < 2) {
		cout << "UDPClient::createSocket Warning!";
		cout << " Winsock dll is too old!" << endl;
		return false;
	}
#endif
	struct protoent *proto;
	proto=getprotobyname("udp");
	sd = socket(AF_INET, SOCK_DGRAM, proto->p_proto);
	if (sd < 0) {
		perror("UDPClient::createSocket error");
		return false;
	}

	memset((char *)&myaddr, 0, sizeof(myaddr));
	myaddr.sin_family = AF_INET;
	myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	myaddr.sin_port = htons(port);

	if (bind(sd, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0) {
		perror("bind failed");
		return 0;
	}

	memset(&remaddr, 0, sizeof(remaddr));
	remaddr.sin_family = AF_INET;
	remaddr.sin_port = htons(port);
	remaddr.sin_addr.s_addr = inet_addr(host.c_str());

	return true;
}

int UDPClient::send(char* buff, unsigned int size) {
	if (sd <= 0) return -1;
	return sendto(sd, buff, size, 0, (struct sockaddr*)&remaddr, sizeof(remaddr));
}

int UDPClient::receive(char** buff) {
	socklen_t size = sizeof(remaddr);

	if (sd <= 0) return -1;

	return recvfrom(sd, *buff, 8191, 0, (struct sockaddr*)&remaddr, &size);
}

void UDPClient::closeSocket() {
	if (sd > 0) {
#if defined _WIN32 || defined __CYGWIN__
		closesocket(sd);
#else
		close(sd);
#endif

	}
}

int UDPClient::dataAvailable(int timeout) {
	timeval tvtimeout;
	fd_set fds;

	if (sd <= 0) return false;

	tvtimeout.tv_sec = timeout / 1000;
	tvtimeout.tv_usec = timeout % 1000;
	FD_ZERO(&fds);
	FD_SET(sd, &fds);

	int nStatus = select(0, &fds, NULL, NULL, &tvtimeout);

	if (nStatus < 0) {
		perror ("UDPClient::select");
		return false;
	} else if (nStatus > 0){
		return true;
	}

	return false;
}
