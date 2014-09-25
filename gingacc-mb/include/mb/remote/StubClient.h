

#ifndef STUBCLIENT_H
#define STUBCLIENT_H

#include <boost/asio.hpp>

using boost::asio::ip::tcp;


class StubClient
{
public:
	StubClient(const std::string& address, const int& port);
	~StubClient(void);

	std::string sendRequest (const std::string& request);
	std::string readMessage ();

private:
	static StubClient* _instance; 
	std::string _address;
	int _port;

	boost::asio::io_service* _ioService;
	tcp::socket* _socket;

	bool connect ();
};


#endif //STUBCLIENT_H