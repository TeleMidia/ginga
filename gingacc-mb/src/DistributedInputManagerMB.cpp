
#include "mb/remote/MessageHandler.h"
#include "mb/LocalScreenManager.h"

#include "mb/DistributedInputManagerMB.h"
#include "mb/interface/CodeMap.h"

#include <iostream>
#include <string.h>

using namespace br::pucrio::telemidia::ginga::core::mb;

DistributedInputManagerMB::DistributedInputManagerMB(const GingaScreenID &screenId)
	: InputManager (screenId)
{
	_IMServer = new InputManagerServer("127.0.0.1", 123457);
}

bool DistributedInputManagerMB::dispatchEvent(IInputEvent* inputEvent)
{
	map<IInputEventListener*, set<int>*>::iterator i;
	int keyCode;

	if (!running) {
		clog << "InputManger::dispatchEvent !running" << endl;
		return false;
	}

	notifying = true;
	lock();
	notifying = true;

	performInputLockedActions();

	if (inputEvent == NULL || !running) 
	{
		unlock();
		notifying = false;

		clog << "InputManger::dispatchEvent no listeners to notify '";
		clog << inputEvent << "'" << endl;
		return true;
	}

	keyCode = inputEvent->getKeyCode(myScreen);

	clog << "InputManger::dispatchEvent ";
	if (keyCode == CodeMap::KEY_TAP) {
		clog << "TAP ";

	} else {
		clog << "keyCode = '" << keyCode << "' ";
	}
	clog << "on screen id = '";
	clog << myScreen << "'";
	clog << endl;

	int x;
	int y;
	int z;

	inputEvent->getAxisValue(&x, &y, &z);

	vector <pair <string, string> >args;
	args.push_back (make_pair("keyCode", to_string(inputEvent->getKeyCode(myScreen))));
	args.push_back (make_pair("type",  to_string(ScreenManagerFactory::getInstance()->fromMBToGinga(myScreen, inputEvent->getType()))) );
	args.push_back (make_pair("isButtonPress", to_string(inputEvent->isButtonPressType())));
	args.push_back (make_pair("isMotion", to_string(inputEvent->isMotionType())));
	args.push_back (make_pair("isPressed", to_string(inputEvent->isPressedType())));
	args.push_back (make_pair("isKey", to_string(inputEvent->isKeyType())));
	args.push_back (make_pair("isApplication", to_string(inputEvent->isApplicationType())));
	args.push_back (make_pair("axisX", to_string(x)));
	args.push_back (make_pair("axisY", to_string(y)));
	args.push_back (make_pair("axisZ", to_string(z)));

	string eventMessage = MessageHandler::createEventMessage(EventType::INPUT_EVENT, args);
	cout << eventMessage << endl;

	std::vector <socket_ptr> listeners = _IMServer->getClientSockets();
	
	boost::system::error_code code;
	for (size_t i = 0; i < listeners.size(); i++)
	{
		socket_ptr socket = listeners.at (i);		
		try 
		{
			size_t len = boost::asio::write(*socket, boost::asio::buffer(eventMessage),
				boost::asio::transfer_all(), code);
			cout << "DistributedInputManagerMB::dispatchEvent::";
			cout << "bytes written: " << len << endl;
		}
		catch (const std::exception& e)
		{
			cout << e.what () << endl;
		}
	}

	unlock();
	notifying = false;
	return true;
}

void DistributedInputManagerMB::notifyMotionListeners (int x, int y, int z)
{
	vector <socket_ptr> clients = _IMServer->getClientSockets ();
}

DistributedInputManagerMB::~DistributedInputManagerMB()
{
}


InputManagerServer::InputManagerServer(const std::string &address, const int port)
{
	_address = address;
	_port = port;

	_io_service = new boost::asio::io_service();
	try
	{
		_serverSocket = new tcp::acceptor (*_io_service, 
			tcp::endpoint(boost::asio::ip::address::from_string(_address), _port));

		Thread::startThread();
	}
	catch (std::exception& e)
	{
		_serverSocket = NULL;
		std::cout << e.what () << std::endl;
	}
}

void InputManagerServer::run ()
{
	while (true) 
	{
		socket_ptr socket (new tcp::socket (*_io_service));
		_serverSocket->accept(*socket); // Accept incoming connection

		_clientSockets.push_back(socket);
	}
}
