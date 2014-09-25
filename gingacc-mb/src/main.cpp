

#include <SDL.h>
#include "mb/LocalScreenManager.h"
#include "mb/DSMDefs.h"

using namespace br::pucrio::telemidia::ginga::core::mb;

#include "mb/remote/RequestManager.h"
#include "mb/remote/StubServer.h"

#include <vector>
#include <sstream>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>

int main(int argc, char *argv[])
{
	RequestManager* requestManager = new RequestManager;

	StubServer* server = StubServer::getInstance ("127.0.0.1", 123456);
	server->setRequestHandler(requestManager);

	server->waitIncoming();

	return 0;
}
