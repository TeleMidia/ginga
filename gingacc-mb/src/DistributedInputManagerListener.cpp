

#include "mb/DistributedInputManagerListener.h"
#include "mb/remote/MessageHandler.h"

using namespace br::pucrio::telemidia::ginga::core::mb;

DistributedInputManagerListener::DistributedInputManagerListener 
	(const GingaScreenID &screenId, const std::string &address, int port) : InputManager(screenId)
{
	_stubClient = new StubClient(address, port);
}

void DistributedInputManagerListener::run()
{
	IInputEvent* inputEvent;

	int pLastCode     = -1;
	int lastCode      = -1;
	double pTimeStamp = 0;
	double timeStamp  = 0;

	int mouseX, mouseY;

	clog << "InputManager::run main loop" << endl;

#if HAVE_KINECTSUPPORT
	if (running) {
		ief->createFactory(InputEventFactory::FT_KINECT, this);
	}
#endif

	while (running && _stubClient != NULL) 
	{
		std::string inputMessage = _stubClient->readMessage();
		std::cout << inputMessage << std::endl;

		if (!running) {
			break;
		}

		map <string, string> args;
		if (MessageHandler::extractEventMessage(inputMessage, args))
		{
			inputEvent = new RemoteInputEvent (args);
			handleInputEvent(inputEvent, pLastCode, lastCode, pTimeStamp, timeStamp, mouseX, mouseY);
		}

		delete inputEvent;
		
		if (running && _stubClient == NULL) 
		{
			clog << "DistributedInputManagerListener::run Warning! Can't receive events: ";
			clog << "event buffer is NULL" << endl;
		}
	}

	currentXAxis = 0;
	currentYAxis = 0;
	currentZAxis = 0;
	maxX         = 0;
	maxY         = 0;

	clog << "DistributedInputManagerListener::run all done" << endl;
}