

#ifndef STUBSERVER_H
#define STUBSERVER_H

#include <string>
#include "mb/DSMDefs.h"

class IRequestHandler
{
public:
	virtual std::string processRequest (std::string& request) = 0;
};

class StubServer
{
public:
	static StubServer* getInstance ();
	static StubServer* getInstance (const std::string& address, const int& port);

	void setRequestHandler (IRequestHandler* handler);
	void removeRequestHandler ();
	
	int waitIncoming ();

private:
	static StubServer* _instance;
	
	IRequestHandler* _requestHandler;
	std::string _address;
	int _port;
	
	boost::asio::io_service* _io_service;
	tcp::acceptor* _serverSocket;

	bool initConnection ();
	void session (socket_ptr);

	StubServer (const std::string& address, const int& port);
};

#endif //STUBSERVER_H