

#ifndef DISTRIBUTEDINPUTMANAGERMB_H
#define DISTRIBUTEDINPUTMANAGERMB_H

#include "InputManager.h"
#include "mb/DSMDefs.h"


namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace mb {

class InputManagerServer;

class DistributedInputManagerMB : public InputManager
{
public:
	DistributedInputManagerMB (const GingaScreenID &screenId);
	~DistributedInputManagerMB ();

protected:
	void notifyMotionListeners(int x, int y, int z);
	bool dispatchEvent(IInputEvent* keyEvent);

private:
	InputManagerServer* _IMServer;
	
};

class InputManagerServer : public Thread
{
public:
	InputManagerServer (const std::string &address, const int port);

	std::vector <socket_ptr> getClientSockets () const
	{ return _clientSockets; }

protected:
	void run();

private:
	std::string _address;
	int _port;
	boost::asio::io_service* _io_service;
	tcp::acceptor* _serverSocket;

	std::vector <socket_ptr> _clientSockets;
};

}
}
}
}
}
}

#endif //DISTRIBUTEDINPUTMANAGERMB_H