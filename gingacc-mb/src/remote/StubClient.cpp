#include "mb/remote/StubClient.h"

#include "mb/DSMDefs.h"

StubClient* StubClient::_instance = NULL;

#include <iostream>
#include <string>

#include <boost/array.hpp>


StubClient::StubClient(const std::string& address, const int& port)
{
	this->_address		= address;
	this->_port			= port;

	_ioService = new boost::asio::io_service();
	_socket = NULL;

	connect();
}

bool StubClient::connect ()
{
	if (_socket == NULL)
		_socket = new tcp::socket (*_ioService);

	tcp::endpoint endpoint (boost::asio::ip::address::from_string(_address), _port);
	try
	{
		_socket->connect(endpoint);	
	}
	catch (boost::system::system_error& e)
	{
		free (_socket);
		_socket = NULL;
		std::cout << e.what () << std::endl;
		return false;
	}
	return true;
}

std::string StubClient::readMessage ()
{
	if (_socket == NULL || !_socket->is_open())
	{
		bool succeeded = connect ();
		if (!succeeded)
			return "";
	}
	try
	{
		boost::system::error_code code;

		char data[MAX_BUFFER_SIZE];
		size_t len = _socket->read_some(boost::asio::buffer(data));
		
		std::string response = std::string(data).substr(0, len);
		std::cout << "StubClient::readMessage::bytes read: " << len << std::endl;
		std::cout << response << std::endl;
		
		return response;
	}
	catch (boost::system::system_error& e)
	{
		std::cout << e.what() << std::endl;
	}
	
	return "";
}


std::string StubClient::sendRequest (const std::string& request)
{
	if (_socket == NULL || !_socket->is_open())
	{
		bool succeeded = connect ();
		if (!succeeded)
			return "";
	}
	try
	{
		boost::system::error_code code;

		size_t len = boost::asio::write(*_socket, boost::asio::buffer(request),
			boost::asio::transfer_all(), code);

		std::cout << "StubClient::sendRequest::bytes written: " << len << std::endl;

		char data[MAX_BUFFER_SIZE];
		len = _socket->read_some(boost::asio::buffer(data));
		
		std::string response = std::string(data).substr(0, len);
		std::cout << "StubClient::sendRequest::bytes read: " << len << std::endl;
		std::cout << response << std::endl;
		
		return response;
	}
	catch (boost::system::system_error& e)
	{
		std::cout << e.what() << std::endl;
	}
	
	return "";
}


StubClient::~StubClient(void)
{
	free (_instance);
}
