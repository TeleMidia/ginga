
#include "mb/remote/MessageHandler.h"
#include "mb/DSMDefs.h"
#include "mb/DistributedScreenManager.h"
#include "mb/interface/sdl/input/SDLEventBuffer.h"
#include "mb/DistributedInputManagerListener.h"

#include "mb/InputManager.h"

using namespace br::pucrio::telemidia::ginga::core::mb;

DistributedScreenManager::DistributedScreenManager() : LocalScreenManager ()
{
	_im = nullptr;
	_eb = nullptr;

	_stubMutex = new pthread_mutex_t();

	_stubClient = new StubClient ("127.0.0.1", 123456);
	Thread::mutexInit(_stubMutex);
}

DistributedScreenManager::~DistributedScreenManager() 
{
	Thread::mutexUnlock(_stubMutex);
	Thread::mutexDestroy(_stubMutex);

	LocalScreenManager::~LocalScreenManager ();
}

string DistributedScreenManager::sendMessage (const string& message)
{
	string response = "";

	if (_stubClient != NULL)
	{
		Thread::mutexLock(_stubMutex);
		response = _stubClient->sendRequest(message);
		Thread::mutexUnlock(_stubMutex);
	}

	return response;
}

void DistributedScreenManager::releaseHandler()
{
}

void DistributedScreenManager::addIEListenerInstance(
	IInputEventListener* listener)
{
	LocalScreenManager::addIEListenerInstance(listener);
}

void DistributedScreenManager::removeIEListenerInstance(
			IInputEventListener* listener)
{
	LocalScreenManager::removeIEListenerInstance(listener);
}

bool DistributedScreenManager::hasIEListenerInstance(
			IInputEventListener* listener, bool removeInstance)
{
	return LocalScreenManager::hasIEListenerInstance(listener, removeInstance);
}

void DistributedScreenManager::addMEListenerInstance(
			IMotionEventListener* listener)
{
	LocalScreenManager::addMEListenerInstance(listener);
}

void DistributedScreenManager::removeMEListenerInstance(
			IMotionEventListener* listener)
{
	LocalScreenManager::removeMEListenerInstance(listener);
}

bool DistributedScreenManager::hasMEListenerInstance(
			IMotionEventListener* listener, bool removeInstance)
{
	return LocalScreenManager::hasMEListenerInstance(listener, removeInstance);
}

void DistributedScreenManager::setBackgroundImage(GingaScreenID screenId, string uri)
{
	vector <string> args;
	args.push_back(to_string(screenId));
	args.push_back(uri);

	string request = MessageHandler::createRequestMessage(MethodRequested::t_setBackgroundImage, args);
	sendMessage (request);
}

int DistributedScreenManager::getDeviceWidth(GingaScreenID screenId)
{
	ResponseCode code;
	vector <string> args;
	int width = 0;

	args.push_back(to_string(screenId));

	string request = MessageHandler::createRequestMessage(MethodRequested::t_getDeviceWidth, args);
	
	string response = sendMessage (request);

	args.clear();
	if (MessageHandler::extractResponseMessage(response, code, args))
	{
		width = atoi (args.at(0).c_str());
	}

	return width;
}

int DistributedScreenManager::getDeviceHeight(GingaScreenID screenId)
{
	ResponseCode code;
	vector <string> args;
	int height = -1;
	args.push_back(to_string(screenId));

	string request = MessageHandler::createRequestMessage(MethodRequested::t_getDeviceHeight, args);
	string response = sendMessage (request);

	args.clear();
	if (MessageHandler::extractResponseMessage(response, code, args))
	{
		height = atoi (args.at(0).c_str());
	}

	return height;
}

void* DistributedScreenManager::getGfxRoot(GingaScreenID screenId)
{
	vector <string> args;
	args.push_back(to_string(screenId));

	string request = MessageHandler::createRequestMessage(MethodRequested::t_getGfxRoot, args);
	sendMessage (request);
	return LocalScreenManager::getGfxRoot(screenId);
}

void DistributedScreenManager::releaseScreen(GingaScreenID screenId)
{
	vector <string> args;
	args.push_back(to_string(screenId));

	string request = MessageHandler::createRequestMessage(MethodRequested::t_releaseScreen, args);
	sendMessage (request);
}

void DistributedScreenManager::releaseMB(GingaScreenID screenId)
{
	vector <string> args;
	args.push_back(to_string(screenId));

	string request = MessageHandler::createRequestMessage(MethodRequested::t_releaseMB, args);
	sendMessage (request);
}

void DistributedScreenManager::clearWidgetPools(GingaScreenID screenId)
{
	vector <string> args;
	args.push_back(to_string(screenId));

	string request = MessageHandler::createRequestMessage(MethodRequested::t_clearWidgetPools, args);
	sendMessage (request);
}

GingaScreenID DistributedScreenManager::createScreen(int argc, char** args)
{
	GingaScreenID screenId = -1;
	vector <string> arguments;
	ResponseCode code;

	arguments.push_back(to_string(argc));

	for (int i = 0; i < argc; i++)
	{
		arguments.push_back(std::string (args[i]));
	}

	std::string request = 
		MessageHandler::createRequestMessage (MethodRequested::t_createScreen, arguments);

	string response = sendMessage(request);
	arguments.clear();
	
	if (MessageHandler::extractResponseMessage(response, code, arguments))
	{
		screenId = (GingaScreenID) atoi (arguments.at(0).c_str());
	}

	return screenId;
}

string DistributedScreenManager::getScreenName(GingaScreenID screenId)
{
	ResponseCode code;
	vector <string> args;
	std::string screenName = "";
	args.push_back(to_string(screenId));

	string request = MessageHandler::createRequestMessage(MethodRequested::t_getScreenName, args);
	
	string response = sendMessage(request);
	args.clear();
	
	if (MessageHandler::extractResponseMessage(response, code, args))
	{
		screenName =  args.at(0);
	}

	return screenName;
}

UnderlyingWindowID DistributedScreenManager::getScreenUnderlyingWindow(GingaScreenID screenId)
{
	ResponseCode code;
	vector <string> args;
	UnderlyingWindowID underlyingWindowId = "";
	args.push_back(to_string(screenId));

	string request = MessageHandler::createRequestMessage(MethodRequested::t_getScreenUnderlyingWindow, args);
	
	string response = sendMessage(request);
	args.clear();
	
	if (MessageHandler::extractResponseMessage(response, code, args))
	{
		underlyingWindowId = (UnderlyingWindowID) args.at(0).c_str(); //FIXME
	}

	return underlyingWindowId;
}

IMediaProvider* DistributedScreenManager::getIMediaProviderFromId (const GingaProviderID& provId)
{
	vector <string> args;
	args.push_back(to_string(provId));

	string request = MessageHandler::createRequestMessage(MethodRequested::t_getIMediaProviderFromId, args);
	sendMessage (request);
	return LocalScreenManager::getIMediaProviderFromId(provId);
}
	
ISurface* DistributedScreenManager::getISurfaceFromId(const GingaSurfaceID &surfaceId)
{
	vector <string> args;
	args.push_back(to_string(surfaceId));

	string request = MessageHandler::createRequestMessage(MethodRequested::t_getISurfaceFromId, args);
	sendMessage (request);

	return LocalScreenManager::getISurfaceFromId(surfaceId);
}

IWindow* DistributedScreenManager::getIWindowFromId(
			GingaScreenID screenId, GingaWindowID winId)
{
	vector <string> args;
	args.push_back(to_string(screenId));
	args.push_back(to_string(winId));

	string request = MessageHandler::createRequestMessage(MethodRequested::t_getIWindowFromId, args);
	sendMessage (request);

	return LocalScreenManager::getIWindowFromId(screenId, winId);
}

bool DistributedScreenManager::mergeIds(
	GingaScreenID screenId, GingaWindowID destId, vector<GingaWindowID>* srcIds)
{
	vector <string> args;
	args.push_back(to_string(screenId));
	args.push_back(to_string(destId));

	string request = MessageHandler::createRequestMessage(MethodRequested::t_mergeIds, args);
	sendMessage (request);


	return LocalScreenManager::mergeIds(screenId, destId, srcIds);
}

void DistributedScreenManager::blitScreen(GingaScreenID screenId, ISurface* destination)
{
	vector <string> args;
	args.push_back(to_string(screenId));
	args.push_back(to_string(destination->getId()));

	string request = MessageHandler::createRequestMessage(
		MethodRequested::t_blitScreen_screenId_isurface, args);
	
	sendMessage (request);
}

void DistributedScreenManager::blitScreen(GingaScreenID screenId, string fileUri)
{
	vector <string> args;
	args.push_back(to_string(screenId));
	args.push_back(fileUri);

	string request = MessageHandler::createRequestMessage(
		MethodRequested::t_blitScreen_screenId_string, args);
	sendMessage (request);
}

void DistributedScreenManager::refreshScreen(GingaScreenID screenId)
{
	vector <string> args;
	args.push_back(to_string(screenId));

	string request = MessageHandler::createRequestMessage(
		MethodRequested::t_refreshScreen, args);
	sendMessage (request);
}

	/* Interfacing output */
GingaWindowID DistributedScreenManager::createWindow(
	GingaScreenID screenId, int x, int y, int w, int h, float z)
{
	ResponseCode code;
	vector <string> args;
	GingaWindowID winId = NULL;

	args.push_back(to_string(screenId));
	args.push_back(to_string(x));
	args.push_back(to_string(y));
	args.push_back(to_string(w));
	args.push_back(to_string(h));
	args.push_back(to_string(z));

	string request = MessageHandler::createRequestMessage(
		MethodRequested::t_createWindow_screenID_4int_float, args);

	string response = sendMessage(request);
	args.clear();
	
	if (MessageHandler::extractResponseMessage(response, code, args))
	{
		winId = (GingaWindowID) atoi (args.at(0).c_str());
	}

	return winId;
}

UnderlyingWindowID DistributedScreenManager::createUnderlyingSubWindow(
	GingaScreenID screenId, int x, int y, int w, int h, float z)
{
	vector <string> args;
	args.push_back(to_string(screenId));
	args.push_back(to_string(x));
	args.push_back(to_string(y));
	args.push_back(to_string(w));
	args.push_back(to_string(h));
	args.push_back(to_string(z));

	string request = MessageHandler::createRequestMessage(
		MethodRequested::t_createUnderlyingSubWindow, args);

	sendMessage (request);

	return LocalScreenManager::createUnderlyingSubWindow(screenId, x, y, w, h, z);
}

bool DistributedScreenManager::hasWindow(GingaScreenID screenId, GingaWindowID window)
{
	ResponseCode code;
	vector <string> args;
	bool hasWin = false;

	args.push_back(to_string(screenId));	
	args.push_back(to_string(window));

	string request = MessageHandler::createRequestMessage(
		MethodRequested::t_hasWindow, args);

	std::string response = sendMessage (request);
	args.clear();

	if (MessageHandler::extractResponseMessage(response, code, args))
	{
		hasWin = (bool) atoi (args[0].c_str());
	}

	return hasWin;
}

void DistributedScreenManager::releaseWindow(GingaScreenID screenId, IWindow* window)
{
	vector <string> args;
	args.push_back(to_string(screenId));
	
	
	args.push_back(to_string(window->getId()));

	string request = MessageHandler::createRequestMessage(
		MethodRequested::t_releaseWindow, args);

	sendMessage (request);
}

void DistributedScreenManager::registerSurface (ISurface* sur)
{
	LocalScreenManager::registerSurface(sur);
}

GingaSurfaceID DistributedScreenManager::createSurface(GingaScreenID screenId)
{
	ResponseCode code;
	vector <string> args;
	GingaSurfaceID surfaceId = -1;

	args.push_back(to_string(screenId));

	string request = MessageHandler::createRequestMessage(
		MethodRequested::t_createSurface_screenId, args);

	std::string response = sendMessage (request);
	args.clear ();

	if (MessageHandler::extractResponseMessage(response, code, args))
	{
		surfaceId = (GingaSurfaceID) atoi(args[0].c_str());
	}

	return surfaceId;
}

GingaSurfaceID DistributedScreenManager::createSurface(GingaScreenID screenId, int w, int h)
{
	vector <string> args;
	args.push_back(to_string(screenId));
	args.push_back(to_string(w));
	args.push_back(to_string(h));

	string request = MessageHandler::createRequestMessage(
		MethodRequested::t_createSurface_screenId_2int, args);

	sendMessage (request);

	return LocalScreenManager::createSurface(screenId, w, h);
}

GingaSurfaceID DistributedScreenManager::createSurfaceFrom(
			GingaScreenID screenId, GingaSurfaceID underlyingSurface)
{
	ResponseCode code;
	vector <string> args;
	GingaSurfaceID surfaceId = 0;

	args.push_back(to_string(screenId));
	args.push_back(to_string(underlyingSurface));

	string request = MessageHandler::createRequestMessage(
		MethodRequested::t_createSurfaceFrom, args);

	std::string response = sendMessage (request);
	args.clear();

	if (MessageHandler::extractResponseMessage(response, code, args))
	{
		surfaceId = (GingaSurfaceID) atoi (args.at(0).c_str());
	}

	return surfaceId;
}

bool DistributedScreenManager::hasSurface(
			const GingaScreenID &screenId, const GingaSurfaceID &surId)
{
	ResponseCode code;
	vector <string> args;
	bool hasSur = false;

	args.push_back(to_string(screenId));	
	args.push_back(to_string(surId));

	string request = MessageHandler::createRequestMessage(
		MethodRequested::t_hasSurface, args);

	std::string response = sendMessage (request);
	args.clear();

	if (MessageHandler::extractResponseMessage(response, code, args))
	{
		hasSur = (bool) atoi (args[0].c_str());
	}

	return hasSur;
}

bool DistributedScreenManager::releaseSurface(GingaScreenID screenId, ISurface* surface)
{
	return LocalScreenManager::releaseSurface(screenId, surface);
}

void DistributedScreenManager::lowerWindowToBottom (
			const GingaScreenID &screenId, const GingaWindowID &winId)
{
	vector <string> args;
	
	args.push_back(to_string(screenId));
	args.push_back(to_string(winId));

	string request = MessageHandler::createRequestMessage(
		MethodRequested::t_lowerWindowToBottom, args);

	sendMessage (request);
}

	/* Interfacing content */
GingaProviderID DistributedScreenManager::createContinuousMediaProvider(
			GingaScreenID screenId, const char* mrl, bool* hasVisual, bool isRemote)
{
	ResponseCode code;
	GingaSurfaceID surfaceId = -1;
	vector <string> args;
	args.push_back(to_string(screenId));
	args.push_back(mrl);
	args.push_back(to_string(*hasVisual));
	args.push_back(to_string(isRemote));
	
	string request = MessageHandler::createRequestMessage(
		MethodRequested::t_createContinuousMediaProvider, args);
	
	std::string response = sendMessage (request);
	args.clear();

	if (MessageHandler::extractResponseMessage(response, code, args))
	{
		surfaceId = atoi (args.at(0).c_str());
		*hasVisual = atoi (args.at(1).c_str());
	}

	return surfaceId;
}

void DistributedScreenManager::releaseContinuousMediaProvider(
	GingaScreenID screenId, GingaProviderID provider)
{
	vector <string> args;
	args.push_back(to_string(screenId));
	args.push_back(to_string(provider));

	string request = MessageHandler::createRequestMessage(
		MethodRequested::t_releaseContinuousMediaProvider, args);

	sendMessage (request);
}

GingaProviderID DistributedScreenManager::createFontProvider(
	GingaScreenID screenId, const char* mrl, int fontSize)
{
	vector <string> args;
	args.push_back(to_string(screenId));
	args.push_back(mrl);
	args.push_back(to_string(fontSize));

	string request = MessageHandler::createRequestMessage(
		MethodRequested::t_createFontProvider, args);

	sendMessage (request);


	return LocalScreenManager::createFontProvider(screenId, mrl, fontSize);
}

void DistributedScreenManager::releaseFontProvider(
			GingaScreenID screenId, GingaProviderID provider)
{
	vector <string> args;
	args.push_back(to_string(screenId));
	args.push_back(to_string(provider));

	string request = MessageHandler::createRequestMessage(
		MethodRequested::t_releaseFontProvider, args);

	sendMessage (request);
}

GingaProviderID DistributedScreenManager::createImageProvider(
			GingaScreenID screenId, const char* mrl)
{
	ResponseCode code;
	vector <string> args;
	GingaProviderID providerId = 0;

	args.push_back(to_string(screenId));
	args.push_back(mrl);

	string request = MessageHandler::createRequestMessage(
		MethodRequested::t_createImageProvider, args);

	std::string response = sendMessage (request);
	args.clear();

	if (MessageHandler::extractResponseMessage(response, code, args))
	{
		providerId = atoi (args.at(0).c_str());
	}
	
	return providerId;
}

void DistributedScreenManager::releaseImageProvider(
			GingaScreenID screenId, GingaProviderID provider)
{
	vector <string> args;
	args.push_back(to_string(screenId));
	args.push_back(to_string(provider));
	
	string request = MessageHandler::createRequestMessage(
		MethodRequested::t_releaseImageProvider, args);

	sendMessage (request);
}

GingaSurfaceID DistributedScreenManager::createRenderedSurfaceFromImageFile(
			GingaScreenID screenId, const char* mrl)
{
	ResponseCode code;
	vector <string> args;
	GingaSurfaceID surfaceId = 0;

	args.push_back(to_string(screenId));
	args.push_back(mrl);

	string request = MessageHandler::createRequestMessage(
		MethodRequested::t_createRenderedSurfaceFromImageFile, args);

	std::string response = sendMessage (request);
	args.clear();

	if (MessageHandler::extractResponseMessage(response, code, args))
	{
		surfaceId = atoi (args.at(0).c_str());
	}

	return surfaceId;
}


	/* interfacing input */
IInputManager* DistributedScreenManager::getInputManager(GingaScreenID screenId)
{
	vector <string> args;
	args.push_back(to_string(screenId));

	string request = MessageHandler::createRequestMessage(
		MethodRequested::t_getInputManager, args);

	sendMessage (request);

	if (_im == nullptr)
		_im = new DistributedInputManagerListener(screenId, "127.0.0.1", 123457);

	return _im;
}

IEventBuffer* DistributedScreenManager::createEventBuffer(GingaScreenID screenId)
{
	vector <string> args;
	args.push_back(to_string(screenId));

	string request = MessageHandler::createRequestMessage(
		MethodRequested::t_createEventBuffer, args);

	sendMessage (request);

	if (_eb == nullptr)
		_eb = new SDLEventBuffer(screenId);

	return _eb;
}

IInputEvent* DistributedScreenManager::createInputEvent(
			GingaScreenID screenId, void* event, const int symbol)
{
	//TODO

	return NULL;
}

IInputEvent* DistributedScreenManager::createApplicationEvent(
			GingaScreenID screenId, int type, void* data)
{
	//TODO

	return 0;
}

int DistributedScreenManager::fromMBToGinga(GingaScreenID screenId, int keyCode)
{
	vector <string> args;
	args.push_back(to_string(screenId));
	args.push_back(to_string(keyCode));

	string request = MessageHandler::createRequestMessage(
		MethodRequested::t_fromMBToGinga, args);

	sendMessage (request);

	return LocalScreenManager::fromMBToGinga(screenId, keyCode);
}

int DistributedScreenManager::fromGingaToMB(GingaScreenID screenId, int keyCode)
{
	ResponseCode code;
	vector <string> args;
	int value = 0;

	args.push_back(to_string(screenId));
	args.push_back(to_string(keyCode));	

	string request = MessageHandler::createRequestMessage(
		MethodRequested::t_fromGingaToMB, args);

	std::string response = sendMessage (request);
	args.clear();

	if (MessageHandler::extractResponseMessage(response, code, args))
	{
		value = atoi (args[0].c_str());
	}

	return value;
}

	/* Methods created to isolate gingacc-mb */
	//windows
void DistributedScreenManager::addWindowCaps (
	const GingaScreenID &screenId, const GingaWindowID &winId, int caps)
{
	vector <string> args;
	args.push_back(to_string(screenId));
	args.push_back(to_string(winId));
	args.push_back(to_string(caps));

	string request = MessageHandler::createRequestMessage(
		MethodRequested::t_addWindowCaps, args);

	sendMessage (request);
}

void DistributedScreenManager::setWindowCaps (
	const GingaScreenID &screenId, const GingaWindowID &winId, int caps)
{
	vector <string> args;
	args.push_back(to_string(screenId));
	args.push_back(to_string(winId));
	args.push_back(to_string(caps));

	string request = MessageHandler::createRequestMessage(
		MethodRequested::t_setWindowCaps, args);

	sendMessage (request);
}

int DistributedScreenManager::getWindowCap (
	const GingaScreenID &screenId, const GingaWindowID &winId, const string &capName)
{
	ResponseCode code;
	vector <string> args;
	int cap = -1;

	args.push_back(to_string(screenId));
	args.push_back(to_string(winId));
	args.push_back(capName);

	string request = MessageHandler::createRequestMessage(
		MethodRequested::t_getWindowCap, args);

	std::string response = sendMessage (request);
	args.clear();

	if (MessageHandler::extractResponseMessage(response, code, args))
	{
		cap = atoi (args.at(0).c_str());
	}

	return cap;
}

void DistributedScreenManager::drawWindow (
	const GingaScreenID &screenId, const GingaWindowID &winId)
{
		vector <string> args;
	args.push_back(to_string(screenId));

	
	
	 args.push_back(to_string(winId));

	string request = MessageHandler::createRequestMessage(
		MethodRequested::t_drawWindow, args);

	sendMessage (request);
}

void DistributedScreenManager::setWindowBounds (
	const GingaScreenID &screenId, const GingaWindowID &winId, int x, 
	int y, int w, int h)
{
	vector <string> args;
	args.push_back(to_string(screenId));

	
	
	 args.push_back(to_string(winId));

	args.push_back(to_string(x));
	args.push_back(to_string(y));
	args.push_back(to_string(w));
	args.push_back(to_string(h));

	string request = MessageHandler::createRequestMessage(
		MethodRequested::t_setWindowBounds, args);

	sendMessage (request);
}

void DistributedScreenManager::showWindow (
	const GingaScreenID &screenId, const GingaWindowID &winId)
{
	vector <string> args;
	args.push_back(to_string(screenId));

	
	
	 args.push_back(to_string(winId));


	string request = MessageHandler::createRequestMessage(
		MethodRequested::t_showWindow, args);

	sendMessage (request);
}

void DistributedScreenManager::hideWindow (
	const GingaScreenID &screenId, const GingaWindowID &winId)
{
	vector <string> args;
	args.push_back(to_string(screenId));

	
	
	 args.push_back(to_string(winId));


	string request = MessageHandler::createRequestMessage(
		MethodRequested::t_hideWindow, args);

	sendMessage (request);
}

void DistributedScreenManager::raiseWindowToTop (
	const GingaScreenID &screenId, const GingaWindowID &winId)
{
	vector <string> args;
	args.push_back(to_string(screenId));

	
	
	 args.push_back(to_string(winId));

	string request = MessageHandler::createRequestMessage(
		MethodRequested::t_raiseWindowToTop, args);

	sendMessage (request);
}

void DistributedScreenManager::renderWindowFrom (
	const GingaScreenID &screenId, const GingaWindowID &winId,
			const GingaSurfaceID &surId)
{
	vector <string> args;
	args.push_back(to_string(screenId));
	args.push_back(to_string(winId));
	args.push_back(to_string(surId));

	string request = MessageHandler::createRequestMessage(
		MethodRequested::t_renderWindowFrom, args);

	sendMessage (request);
}

void DistributedScreenManager::setWindowBgColor (
	const GingaScreenID &screenId, const GingaWindowID &winId, int r,
	int g, int b, int alpha)
{
	vector <string> args;
	args.push_back(to_string(screenId));

	args.push_back(to_string(winId));

	args.push_back(to_string(r));
	args.push_back(to_string(g));
	args.push_back(to_string(b));
	args.push_back(to_string(alpha));

	string request = MessageHandler::createRequestMessage(
		MethodRequested::t_setWindowBgColor, args);

	sendMessage (request);
}

void DistributedScreenManager::setWindowBorder (
	const GingaScreenID &screenId, const GingaWindowID &winId, int r,
	int g, int b, int alpha, int width)
{
	vector <string> args;
	args.push_back(to_string(screenId));

	
	

	 args.push_back(to_string(winId));
	args.push_back(to_string(r));
	args.push_back(to_string(g));
	args.push_back(to_string(b));
	args.push_back(to_string(alpha));
	args.push_back(to_string(width));

	string request = MessageHandler::createRequestMessage(
		MethodRequested::t_setWindowBorder, args);

	sendMessage (request);
}

void DistributedScreenManager::setWindowCurrentTransparency (
	const GingaScreenID &screenId, const GingaWindowID &winId,
	int transparency)
{
	vector <string> args;
	args.push_back(to_string(screenId));

	
	
	 args.push_back(to_string(winId));
	args.push_back(to_string(transparency));

	string request = MessageHandler::createRequestMessage(
		MethodRequested::t_setWindowCurrentTransparency, args);

	sendMessage (request);
}

void DistributedScreenManager::setWindowColorKey (
	const GingaScreenID &screenId, const GingaWindowID &winId, int r,
	int g, int b)
{
	vector <string> args;
	args.push_back(to_string(screenId));

	
	
	 args.push_back(to_string(winId));

	args.push_back(to_string(r));
	args.push_back(to_string(g));
	args.push_back(to_string(b));

	string request = MessageHandler::createRequestMessage(
		MethodRequested::t_setWindowColorKey, args);

	sendMessage (request);
}

void DistributedScreenManager::setWindowX(
	const GingaScreenID &screenId, const GingaWindowID &winId, int x)
{
	vector <string> args;
	args.push_back(to_string(screenId));

	
	
	 args.push_back(to_string(winId));

	args.push_back(to_string(x));

	string request = MessageHandler::createRequestMessage(
		MethodRequested::t_setWindowX, args);

	sendMessage (request);
}

void DistributedScreenManager::setWindowY(
	const GingaScreenID &screenId, const GingaWindowID &winId, int y)
{
	vector <string> args;
	args.push_back(to_string(screenId));

	
	
	 args.push_back(to_string(winId));

	args.push_back(to_string(y));

	string request = MessageHandler::createRequestMessage(
		MethodRequested::t_setWindowY, args);

	sendMessage (request);
}

void DistributedScreenManager::setWindowW(
	const GingaScreenID &screenId, const GingaWindowID &winId, int w)
{
	vector <string> args;
	args.push_back(to_string(screenId));

	
	
	 args.push_back(to_string(winId));

	args.push_back(to_string(w));

	string request = MessageHandler::createRequestMessage(
		MethodRequested::t_setWindowW, args);

	sendMessage (request);
}

void DistributedScreenManager::setWindowH(
	const GingaScreenID &screenId, const GingaWindowID &winId, int h)
{
	vector <string> args;
	args.push_back(to_string(screenId));

	
	
	 args.push_back(to_string(winId));

	args.push_back(to_string(h));

	string request = MessageHandler::createRequestMessage(
		MethodRequested::t_setWindowH, args);

	sendMessage (request);
}

void DistributedScreenManager::setWindowZ(
	const GingaScreenID &screenId, const GingaWindowID &winId, float z)
{
	vector <string> args;
	args.push_back(to_string(screenId));

	
	
	 args.push_back(to_string(winId));
	
	args.push_back(to_string(z));

	string request = MessageHandler::createRequestMessage(
		MethodRequested::t_setWindowZ, args);

	sendMessage (request);
}

void DistributedScreenManager::disposeWindow (
	const GingaScreenID &screenId, const GingaWindowID &winId)
{
	vector <string> args;
	args.push_back(to_string(screenId));

	
	
	 args.push_back(to_string(winId));

	string request = MessageHandler::createRequestMessage(
		MethodRequested::t_disposeWindow, args);

	sendMessage (request);
}

void DistributedScreenManager::setGhostWindow(
	const GingaScreenID &screenId, const GingaWindowID &winId, bool ghost)
{
	vector <string> args;
	args.push_back(to_string(screenId));

	
	
	 args.push_back(to_string(winId));

	args.push_back(to_string(ghost));

	string request = MessageHandler::createRequestMessage(
		MethodRequested::t_setGhostWindow, args);

	sendMessage (request);
}

void DistributedScreenManager::validateWindow (
	const GingaScreenID &screenId, const GingaWindowID &winId)
{
	vector <string> args;
	args.push_back(to_string(screenId));

	
	
	 args.push_back(to_string(winId));

	string request = MessageHandler::createRequestMessage(
		MethodRequested::t_validateWindow, args);

	sendMessage (request);
}

int DistributedScreenManager::getWindowX (
	const GingaScreenID &screenId, const GingaWindowID &winId)
{
	ResponseCode code;
	vector <string> args;
	int value = 0;

	args.push_back(to_string(screenId));
	args.push_back(to_string(winId));

	string request = MessageHandler::createRequestMessage(
		MethodRequested::t_getWindowX, args);

	std::string response = sendMessage (request);
	args.clear();

	if (MessageHandler::extractResponseMessage(response, code, args))
	{
		value = atoi (args[0].c_str());
	}

	return value;
}

int DistributedScreenManager::getWindowY (
	const GingaScreenID &screenId, const GingaWindowID &winId)
{
	ResponseCode code;
	vector <string> args;
	int value = 0;

	args.push_back(to_string(screenId));
	args.push_back(to_string(winId));

	string request = MessageHandler::createRequestMessage(
		MethodRequested::t_getWindowY, args);

	std::string response = sendMessage (request);
	args.clear();

	if (MessageHandler::extractResponseMessage(response, code, args))
	{
		value = atoi (args[0].c_str());
	}

	return value;
}

int DistributedScreenManager::getWindowW (
	const GingaScreenID &screenId, const GingaWindowID &winId)
{
	ResponseCode code;
	vector <string> args;
	int value = 0;

	args.push_back(to_string(screenId));
	args.push_back(to_string(winId));

	string request = MessageHandler::createRequestMessage(
		MethodRequested::t_getWindowW, args);

	std::string response = sendMessage (request);
	args.clear();

	if (MessageHandler::extractResponseMessage(response, code, args))
	{
		value = atoi (args[0].c_str());
	}

	return value;
}

int DistributedScreenManager::getWindowH (
	const GingaScreenID &screenId, const GingaWindowID &winId)
{
	ResponseCode code;
	vector <string> args;
	int value = 0;

	args.push_back(to_string(screenId));
	args.push_back(to_string(winId));

	string request = MessageHandler::createRequestMessage(
		MethodRequested::t_getWindowH, args);

	std::string response = sendMessage (request);
	args.clear();

	if (MessageHandler::extractResponseMessage(response, code, args))
	{
		value = atoi (args[0].c_str());
	}

	return value;
}

float DistributedScreenManager::getWindowZ (
	const GingaScreenID &screenId, const GingaWindowID &winId)
{
	ResponseCode code;
	vector <string> args;
	float value = 0;

	args.push_back(to_string(screenId));
	args.push_back(to_string(winId));

	string request = MessageHandler::createRequestMessage(
		MethodRequested::t_getWindowZ, args);

	std::string response = sendMessage (request);
	args.clear();

	if (MessageHandler::extractResponseMessage(response, code, args))
	{
		value = atoi (args[0].c_str());
	}

	return value;
}

int DistributedScreenManager::getWindowTransparencyValue(
	const GingaScreenID &screenId, const GingaWindowID &winId)
{
	ResponseCode code;
	vector <string> args;
	int value = 0;

	args.push_back(to_string(screenId));
	args.push_back(to_string(winId));

	string request = MessageHandler::createRequestMessage(
		MethodRequested::t_getWindowTransparencyValue, args);

	std::string response = sendMessage (request);
	args.clear();

	if (MessageHandler::extractResponseMessage(response, code, args))
	{
		value = atoi (args[0].c_str());
	}

	return value;
}

void DistributedScreenManager::resizeWindow(
	const GingaScreenID &screenId, const GingaWindowID &winId, int width,
	int height)
{
	vector <string> args;
	args.push_back(to_string(screenId));
	args.push_back(to_string(winId));
	args.push_back(to_string(width));
	args.push_back(to_string(height));

	string request = MessageHandler::createRequestMessage(
		MethodRequested::t_resizeWindow, args);

	sendMessage (request);
}

string DistributedScreenManager::getWindowDumpFileUri (
	const GingaScreenID &screenId, const GingaWindowID &winId, 
	int quality, int dumpW, int dumpH)
{
	vector <string> args;
	args.push_back(to_string(screenId));

	
	
	 args.push_back(to_string(winId));

	string request = MessageHandler::createRequestMessage(
		MethodRequested::t_getWindowDumpFileUri, args);

	sendMessage (request);

	return LocalScreenManager::getWindowDumpFileUri(screenId, winId, quality, dumpW, dumpH);
}

void DistributedScreenManager::clearWindowContent (
	const GingaScreenID &screenId, const GingaWindowID &winId)
{
	vector <string> args;
	args.push_back(to_string(screenId));

	
	
	 args.push_back(to_string(winId));

	string request = MessageHandler::createRequestMessage(
		MethodRequested::t_clearWindowContent, args);

	sendMessage (request);
}


void DistributedScreenManager::revertWindowContent(
	const GingaScreenID &screenId, const GingaWindowID &winId)
{
	vector <string> args;
	args.push_back(to_string(screenId));

	
	
	 args.push_back(to_string(winId));

	string request = MessageHandler::createRequestMessage(
		MethodRequested::t_revertWindowContent, args);

	sendMessage (request);
}

void DistributedScreenManager::deleteWindow(
	const GingaScreenID &screenId, const GingaWindowID &winId)
{
	vector <string> args;
	args.push_back(to_string(screenId));

	
	
	 args.push_back(to_string(winId));

	string request = MessageHandler::createRequestMessage(
		MethodRequested::t_deleteWindow, args);

	sendMessage (request);
}

void DistributedScreenManager::moveWindowTo(
	const GingaScreenID &screenId, const GingaWindowID &winId, int x, int y)
{
	vector <string> args;
	args.push_back(to_string(screenId));

	
	
	 args.push_back(to_string(winId));

	string request = MessageHandler::createRequestMessage(
		MethodRequested::t_moveWindowTo, args);

	sendMessage (request);
}

void DistributedScreenManager::setWindowMirrorSrc (
	const GingaScreenID &screenId, const GingaWindowID &winId,
	const GingaWindowID &mirrorSrc)
{
	vector <string> args;
	args.push_back(to_string(screenId));

	
	
	 args.push_back(to_string(winId));

	string request = MessageHandler::createRequestMessage(
		MethodRequested::t_setWindowMirrorSrc, args);

	sendMessage (request);
}

	//surfaces
void* DistributedScreenManager::getSurfaceContent(const GingaSurfaceID &surId)
{
	vector <string> args;
	args.push_back(to_string(surId));

	string request = MessageHandler::createRequestMessage(
		MethodRequested::t_getSurfaceContent, args);

	sendMessage (request);

	return LocalScreenManager::getSurfaceContent (surId);
}

GingaWindowID DistributedScreenManager::getSurfaceParentWindow(const GingaSurfaceID& surId)
{
	ResponseCode code;
	vector <string> args;
	GingaWindowID winId = -1;

	args.push_back(to_string(surId));

	string request = MessageHandler::createRequestMessage(
		MethodRequested::t_getSurfaceParentWindow, args);

	sendMessage (request);
	std::string response = sendMessage (request);
	args.clear();

	if (MessageHandler::extractResponseMessage(response, code, args))
	{
		winId = (GingaWindowID) atoi (args.at(0).c_str());
	}


	return winId;
}

void DistributedScreenManager::deleteSurface (const GingaSurfaceID &surId)
{
	vector <string> args;
	args.push_back(to_string(surId));

	string request = MessageHandler::createRequestMessage(
		MethodRequested::t_deleteSurface, args);

	sendMessage (request);
}

bool DistributedScreenManager::setSurfaceParentWindow (
	const GingaScreenID &screenId,  const GingaSurfaceID &surId,
	const GingaWindowID &winId)
{
	ResponseCode code;
	vector <string> args;
	bool result = false;

	args.push_back(to_string(screenId));
	args.push_back(to_string(surId));
	args.push_back (to_string(winId));

	string request = MessageHandler::createRequestMessage(
		MethodRequested::t_setSurfaceParentWindow, args);

	std::string response = sendMessage (request);
	args.clear ();

	if (MessageHandler::extractResponseMessage(response, code, args))
	{
		result = (bool) atoi (args[0].c_str());
	}

	return result;
}

void DistributedScreenManager::clearSurfaceContent (const GingaSurfaceID &surId)
{
	vector <string> args;
	args.push_back(to_string(surId));

	string request = MessageHandler::createRequestMessage(
		MethodRequested::t_clearSurfaceContent, args);

	sendMessage (request);
}

void DistributedScreenManager::getSurfaceSize(const GingaSurfaceID &surId, int* width, int* height)
{
	vector <string> args;
	args.push_back(to_string(surId));

	string request = MessageHandler::createRequestMessage(
		MethodRequested::t_getSurfaceSize, args);

	sendMessage (request);
}

void DistributedScreenManager::addSurfaceCaps(const GingaSurfaceID &surId, const int caps)
{
	vector <string> args;
	args.push_back(to_string(surId));
	args.push_back(to_string(caps));

	string request = MessageHandler::createRequestMessage(
		MethodRequested::t_addSurfaceCaps, args);

	sendMessage (request);
}

void DistributedScreenManager::setSurfaceCaps (const GingaSurfaceID &surId, const int caps)
{
	vector <string> args;
	args.push_back(to_string(surId));
	args.push_back(to_string(caps));

	string request = MessageHandler::createRequestMessage(
		MethodRequested::t_setSurfaceCaps, args);

	sendMessage (request);
}

int DistributedScreenManager::getSurfaceCap(const GingaSurfaceID &surId, const string &cap)
{
	vector <string> args;
	args.push_back(to_string(surId));
	args.push_back(cap);

	string request = MessageHandler::createRequestMessage(
		MethodRequested::t_getSurfaceCap, args);

	sendMessage (request);

	return LocalScreenManager::getSurfaceCap(surId, cap);
}

int DistributedScreenManager::getSurfaceCaps(const GingaSurfaceID &surId)
{
	vector <string> args;
	args.push_back(to_string(surId));

	string request = MessageHandler::createRequestMessage(
		MethodRequested::t_getSurfaceCaps, args);

	sendMessage (request);

	return LocalScreenManager::getSurfaceCaps(surId);
}

void DistributedScreenManager::setSurfaceBgColor(
	const GingaSurfaceID &surId, int r, int g, int b, int alpha)
{
	vector <string> args;
	args.push_back(to_string(surId));
	args.push_back(to_string(r));
	args.push_back(to_string(g));
	args.push_back(to_string(b));
	args.push_back(to_string(alpha));

	string request = MessageHandler::createRequestMessage(
		MethodRequested::t_setSurfaceBgColor, args);

	sendMessage (request);
}

void DistributedScreenManager::setSurfaceFont(const GingaSurfaceID &surId, GingaSurfaceID font)
{
	vector <string> args;
	args.push_back(to_string(surId));
	args.push_back(to_string(font));

	string request = MessageHandler::createRequestMessage(
		MethodRequested::t_setSurfaceFont, args);

	sendMessage (request);
}

void DistributedScreenManager::setColor(
	const GingaSurfaceID &surId, int r, int g, int b,int alpha)
{
	vector <string> args;
	args.push_back(to_string(surId));
	args.push_back(to_string(r));
	args.push_back(to_string(g));
	args.push_back(to_string(b));
	args.push_back(to_string(alpha));

	string request = MessageHandler::createRequestMessage(
		MethodRequested::t_setColor, args);

	sendMessage (request);
}

void DistributedScreenManager::setExternalHandler(
	const GingaSurfaceID &surId, bool extHandler)
{
	vector <string> args;
	args.push_back(to_string(surId));
	args.push_back(to_string(extHandler));

	string request = MessageHandler::createRequestMessage(
		MethodRequested::t_setSurfaceExternalHandler, args);

	sendMessage (request);
}

void DistributedScreenManager::blitSurface (
	const GingaSurfaceID &surId,int x, int y, GingaSurfaceID src, int srcX, int srcY, int srcW, int srcH)
{
	vector <string> args;
	args.push_back(to_string(surId));
	args.push_back(to_string(x));
	args.push_back(to_string(y));
	args.push_back(to_string(src));
	args.push_back(to_string(srcX));
	args.push_back(to_string(srcY));
	args.push_back(to_string(srcW));
	args.push_back(to_string(srcH));

	string request = MessageHandler::createRequestMessage(
		MethodRequested::t_blitSurface, args);

	sendMessage (request);
}

void DistributedScreenManager::flipSurface (const GingaSurfaceID &surId)
{
	vector <string> args;
	args.push_back(to_string(surId));

	string request = MessageHandler::createRequestMessage(
		MethodRequested::t_flipSurface, args);

	sendMessage (request);
}

void DistributedScreenManager::setSurfaceContent(const GingaSurfaceID &surId, void* surface)
{
	//TODO
}

IColor* DistributedScreenManager::getSurfaceColor(const GingaSurfaceID &surId)
{
	vector <string> args;
	args.push_back(to_string(surId));

	string request = MessageHandler::createRequestMessage(
		MethodRequested::t_getSurfaceColor, args);

	sendMessage (request);

	return LocalScreenManager::getSurfaceColor(surId);
}

bool DistributedScreenManager::hasSurfaceExternalHandler(const GingaSurfaceID &surId)
{
	vector <string> args;
	args.push_back(to_string(surId));

	string request = MessageHandler::createRequestMessage(
		MethodRequested::t_hasSurfaceExternalHandler, args);

	sendMessage (request);
	return LocalScreenManager::hasSurfaceExternalHandler(surId);
}

void DistributedScreenManager::setSurfaceColor(
	const GingaSurfaceID &surId, int r, int g, int b, int alpha)
{
	vector <string> args;
	args.push_back(to_string(surId));
	args.push_back(to_string(r));
	args.push_back(to_string(g));
	args.push_back(to_string(b));
	args.push_back(to_string(alpha));

	string request = MessageHandler::createRequestMessage(
		MethodRequested::t_setSurfaceColor, args);

	sendMessage (request);
}

	//providers
void DistributedScreenManager::setProviderSoundLevel (
	const GingaProviderID &provId, float level)
{
	vector <string> args;
	args.push_back(to_string(provId));
	args.push_back(to_string(level));

	string request = MessageHandler::createRequestMessage(
		MethodRequested::t_setProviderSoundLevel, args);

	sendMessage (request);
}

void DistributedScreenManager::getProviderOriginalResolution(
	const GingaProviderID &provId, int* width, int* height)
{
	vector <string> args;
	args.push_back(to_string(provId));

	string request = MessageHandler::createRequestMessage(
		MethodRequested::t_getProviderOriginalResolution, args);

	sendMessage (request);
}

double DistributedScreenManager::getProviderTotalMediaTime(const GingaProviderID &provId)
{
	ResponseCode code;
	vector <string> args;
	double totalTime = 0.0;

	args.push_back(to_string(provId));

	string request = MessageHandler::createRequestMessage(
		MethodRequested::t_getProviderTotalMediaTime, args);

	std::string response = sendMessage (request);
	args.clear ();

	if (MessageHandler::extractResponseMessage(response, code, args))
	{
		totalTime = atof (args[0].c_str());
	}

	return totalTime;
}

int64_t DistributedScreenManager::getProviderVPts(const GingaProviderID &provId)
{
	vector <string> args;
	args.push_back(to_string(provId));

	string request = MessageHandler::createRequestMessage(
		MethodRequested::t_getProviderVPts, args);

	sendMessage (request);

	return LocalScreenManager::getProviderVPts(provId);
}

void DistributedScreenManager::setProviderMediaTime(const GingaProviderID &provId, double pos)
{
	vector <string> args;
	args.push_back(to_string(provId));
	args.push_back(to_string(pos));

	string request = MessageHandler::createRequestMessage(
		MethodRequested::t_setProviderMediaTime, args);

	sendMessage (request);
}

double DistributedScreenManager::getProviderMediaTime(const GingaProviderID &provId)
{
	ResponseCode code;
	vector <string> args;
	double mediaTime = 0;
	args.push_back(to_string(provId));

	string request = MessageHandler::createRequestMessage(
		MethodRequested::t_getProviderMediaTime, args);

	string response = sendMessage (request);

	args.clear();
	if (MessageHandler::extractResponseMessage(response, code, args))
	{
		mediaTime = atoi (args.at(0).c_str());
	}

	return mediaTime;
}

void DistributedScreenManager::pauseProvider (const GingaProviderID &provId)
{
	vector <string> args;
	args.push_back(to_string(provId));

	string request = MessageHandler::createRequestMessage(
		MethodRequested::t_pauseProvider, args);

	sendMessage (request);
}

void DistributedScreenManager::stopProvider (const GingaProviderID &provId)
{
	vector <string> args;
	args.push_back(to_string(provId));

	string request = MessageHandler::createRequestMessage(
		MethodRequested::t_stopProvider, args);

	sendMessage (request);
}

void DistributedScreenManager::resumeProvider (
	const GingaProviderID &provId, GingaSurfaceID surface,bool hasVisual)
{
	vector <string> args;
	args.push_back(to_string(provId));

	string request = MessageHandler::createRequestMessage(
		MethodRequested::t_resumeProvider, args);

	sendMessage (request);
}

void DistributedScreenManager::setProviderAVPid(const GingaProviderID &provId, int aPid, int vPid)
{
	vector <string> args;
	args.push_back(to_string(provId));
	args.push_back(to_string(aPid));
	args.push_back(to_string(vPid));

	string request = MessageHandler::createRequestMessage(
		MethodRequested::t_setProviderAVPid, args);

	sendMessage (request);
}

void DistributedScreenManager::feedProviderBuffers(const GingaProviderID &provId)
{
	vector <string> args;
	args.push_back(to_string(provId));

	string request = MessageHandler::createRequestMessage(
		MethodRequested::t_feedProviderBuffers, args);

	sendMessage (request);
}

bool DistributedScreenManager::checkProviderVideoResizeEvent(
	const GingaProviderID &provId, const GingaSurfaceID &frame)
{
	vector <string> args;
	args.push_back(to_string(provId));
	args.push_back(to_string(frame));

	string request = MessageHandler::createRequestMessage(
		MethodRequested::t_checkProviderVideoResizeEvent, args);

	sendMessage (request);

	return LocalScreenManager::checkProviderVideoResizeEvent(provId, frame);
}

int DistributedScreenManager::getProviderStringWidth(
	const GingaProviderID &provId, const char* text, int textLength)
{
	vector <string> args;
	args.push_back(to_string(provId));
	args.push_back(string(text));
	args.push_back(to_string(textLength));

	string request = MessageHandler::createRequestMessage(
		MethodRequested::t_getProviderStringWidth, args);

	sendMessage (request);

	return LocalScreenManager::getProviderStringWidth(provId, text, textLength);
}

void DistributedScreenManager::playProviderOver(
	const GingaProviderID &provId, const GingaSurfaceID &surface)
{
	vector <string> args;
	args.push_back(to_string(provId));
	args.push_back(to_string(surface));

	string request = MessageHandler::createRequestMessage(
		MethodRequested::t_playProviderOver_provId_surId, args);

	sendMessage (request);
}

void DistributedScreenManager::playProviderOver(
	const GingaProviderID &provId, const GingaSurfaceID &surface,
	const char* text, int x, int y, short align)
{
	vector <string> args;
	args.push_back(to_string(provId));
	args.push_back(to_string(surface));
	args.push_back(string(text));
	args.push_back(to_string(x));
	args.push_back(to_string(y));
	args.push_back(to_string(align));

	string request = MessageHandler::createRequestMessage(
		MethodRequested::t_playProviderOver_provId_surId_char_2int_short, args);

	sendMessage (request);
}

void DistributedScreenManager::playProviderOver(
	const GingaProviderID &provId, GingaSurfaceID surface,
	bool hasVisual)
{
	vector <string> args;
	args.push_back(to_string(provId));
	args.push_back(to_string(surface));
	args.push_back(to_string(hasVisual));
	
	string request = MessageHandler::createRequestMessage(
		MethodRequested::t_playProviderOver_provId_surId_bool, args);

	sendMessage (request);
}

int DistributedScreenManager::getProviderHeight(const GingaProviderID &provId)
{
	vector <string> args;
	args.push_back(to_string(provId));

	string request = MessageHandler::createRequestMessage(
		MethodRequested::t_getProviderHeight, args);

	sendMessage (request);

	return LocalScreenManager::getProviderHeight(provId);
}