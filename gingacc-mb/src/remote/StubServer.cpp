
#include "mb/remote/StubServer.h"

#include "mb/DSMDefs.h"

#include <iostream>
#include <vector>

#include <cstdlib>
#include <boost/bind.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/thread/thread.hpp>

StubServer* StubServer::_instance = NULL;

StubServer::StubServer(const std::string& address, const int& port)
{
	this->_address			= address;
	this->_port				= port;
	this->_requestHandler   = NULL;
	this->_serverSocket		= NULL;

	_io_service = new boost::asio::io_service();
}

bool StubServer::initConnection ()
{
	if (_serverSocket != NULL)
	{
		_serverSocket->close();
		free (_serverSocket);
		_serverSocket = NULL;
	}
	try 
	{
		_serverSocket = new tcp::acceptor (*_io_service, 
			tcp::endpoint(boost::asio::ip::address::from_string(_address), _port));
	}
	catch (std::exception& e)
	{
		_serverSocket = NULL;
		std::cout << e.what () << std::endl;
		
		return false;
	}

	return true;
}

StubServer* StubServer::getInstance ()
{
	if (_instance == NULL)
		_instance = new StubServer("127.0.0.1", 123456);

	return _instance;
}

StubServer* StubServer::getInstance (const std::string& address, const int& port)
{
	if (_instance == NULL)
		_instance = new StubServer(address, port);

	return _instance;
}

void StubServer::setRequestHandler (IRequestHandler* handler)
{
	if (handler != NULL)
		_requestHandler = handler;
}

void StubServer::removeRequestHandler ()
{
	_requestHandler = NULL;
}

void StubServer::session(socket_ptr socket)
{
  try
  {
    while (true)
    {
		char data[MAX_BUFFER_SIZE];

		boost::system::error_code error;
		size_t length = socket->read_some(boost::asio::buffer(data), error);

		if (error == boost::asio::error::eof)
			break; // Connection closed cleanly by peer.
		else if (error)
			throw boost::system::system_error(error); // Some other error.

		std::cout << "Bytes read: " << length << std::endl;
		std::cout.write(data, length);
		std::cout << std::endl;

		if (_requestHandler != NULL)
		{
			std::string response = _requestHandler->processRequest(std::string(data).substr(0, length));
			boost::system::error_code code;

			size_t len = boost::asio::write(*socket, boost::asio::buffer(response),
				boost::asio::transfer_all(), code);

			std::cout << "StubServer::sendResponse::bytes written: " << len << std::endl;
		}
	}
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception in thread: " << e.what() << "\n";
  }
}

int StubServer::waitIncoming ()
{
	if (initConnection())
	{
		//std::vector <tcp::socket*> sockets;

		while (true) 
		{
			socket_ptr socket (new tcp::socket (*_io_service));
			_serverSocket->accept(*socket); // Accept incoming connection.
			session(socket);
		}
		return 0;
	}

	return -1;
}
