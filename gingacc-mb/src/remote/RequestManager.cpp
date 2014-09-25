
#include "mb/remote/RequestManager.h"
#include "mb/IMBDefs.h"

RequestManager::RequestManager()
{
	_ptMethods[MethodRequested::t_addIEListener] = &RequestManager::addIEListener;
	_ptMethods[MethodRequested::t_removeIEListener] = &RequestManager::removeIEListener;
	_ptMethods[MethodRequested::t_hasIEListener] = &RequestManager::hasIEListener;
	_ptMethods[MethodRequested::t_addMEListener] = &RequestManager::addMEListener;
	_ptMethods[MethodRequested::t_removeMEListener] = &RequestManager::removeMEListener;
	_ptMethods[MethodRequested::t_hasMEListener] = &RequestManager::hasMEListener;
	_ptMethods[MethodRequested::t_setBackgroundImage] = &RequestManager::setBackgroundImage;
	_ptMethods[MethodRequested::t_getDeviceWidth] = &RequestManager::getDeviceWidth;
	_ptMethods[MethodRequested::t_getDeviceHeight] = &RequestManager::getDeviceHeight;
	_ptMethods[MethodRequested::t_getGfxRoot] = &RequestManager::getGfxRoot;
	_ptMethods[MethodRequested::t_releaseScreen] = &RequestManager::releaseScreen;
	_ptMethods[MethodRequested::t_releaseMB] = &RequestManager::releaseMB;
	_ptMethods[MethodRequested::t_clearWidgetPools] = &RequestManager::clearWidgetPools;
	_ptMethods[MethodRequested::t_createScreen] = &RequestManager::createScreen;
	_ptMethods[MethodRequested::t_getScreenName] = &RequestManager::getScreenName;
	_ptMethods[MethodRequested::t_getScreenUnderlyingWindow] = &RequestManager::getScreenUnderlyingWindow;
	_ptMethods[MethodRequested::t_getIMediaProviderFromId] = &RequestManager::getIMediaProviderFromId;
	_ptMethods[MethodRequested::t_getISurfaceFromId] = &RequestManager::getISurfaceFromId;
	_ptMethods[MethodRequested::t_getIWindowFromId] = &RequestManager::getIWindowFromId;
	_ptMethods[MethodRequested::t_mergeIds] = &RequestManager::mergeIds;
	_ptMethods[MethodRequested::t_blitScreen_screenId_isurface] = &RequestManager::blitScreen_screenId_isurface;
	_ptMethods[MethodRequested::t_blitScreen_screenId_string] = &RequestManager::blitScreen_screenId_string;
	_ptMethods[MethodRequested::t_refreshScreen] = &RequestManager::refreshScreen;
	_ptMethods[MethodRequested::t_createWindow_screenID_4int_float] = &RequestManager::createWindow_screenID_4int_float;
	_ptMethods[MethodRequested::t_createUnderlyingSubWindow] = &RequestManager::createUnderlyingSubWindow;
	_ptMethods[MethodRequested::t_createWindowFrom] = &RequestManager::createWindowFrom;
	_ptMethods[MethodRequested::t_hasWindow] = &RequestManager::hasWindow;
	_ptMethods[MethodRequested::t_releaseWindow] = &RequestManager::releaseWindow;
	_ptMethods[MethodRequested::t_registerSurface] = &RequestManager::registerSurface;
	_ptMethods[MethodRequested::t_createSurface_screenId] = &RequestManager::createSurface_screenId;
	_ptMethods[MethodRequested::t_createSurface_screenId_2int] = &RequestManager::createSurface_screenId_2int;
	_ptMethods[MethodRequested::t_createSurfaceFrom] = &RequestManager::createSurfaceFrom;
	_ptMethods[MethodRequested::t_hasSurface] = &RequestManager::hasSurface;
	_ptMethods[MethodRequested::t_releaseSurface] = &RequestManager::releaseSurface;
	_ptMethods[MethodRequested::t_lowerWindowToBottom] = &RequestManager::lowerWindowToBottom;
	_ptMethods[MethodRequested::t_createContinuousMediaProvider] = &RequestManager::createContinuousMediaProvider;
	_ptMethods[MethodRequested::t_releaseContinuousMediaProvider] = &RequestManager::releaseContinuousMediaProvider;
	_ptMethods[MethodRequested::t_createFontProvider] = &RequestManager::createFontProvider;
	_ptMethods[MethodRequested::t_releaseFontProvider] = &RequestManager::releaseFontProvider;
	_ptMethods[MethodRequested::t_createImageProvider] = &RequestManager::createImageProvider;
	_ptMethods[MethodRequested::t_releaseImageProvider] = &RequestManager::releaseImageProvider;
	_ptMethods[MethodRequested::t_createRenderedSurfaceFromImageFile] = &RequestManager::createRenderedSurfaceFromImageFile;
	_ptMethods[MethodRequested::t_getInputManager] = &RequestManager::getInputManager;
	_ptMethods[MethodRequested::t_createEventBuffer] = &RequestManager::createEventBuffer;
	_ptMethods[MethodRequested::t_createInputEvent] = &RequestManager::createInputEvent;
	_ptMethods[MethodRequested::t_createApplicationEvent] = &RequestManager::createApplicationEvent;
	_ptMethods[MethodRequested::t_fromMBToGinga] = &RequestManager::fromMBToGinga;
	_ptMethods[MethodRequested::t_fromGingaToMB] = &RequestManager::fromGingaToMB;
	_ptMethods[MethodRequested::t_addWindowCaps] = &RequestManager::addWindowCaps;
	_ptMethods[MethodRequested::t_setWindowCaps] = &RequestManager::setWindowCaps;
	_ptMethods[MethodRequested::t_getWindowCap] = &RequestManager::getWindowCap;
	_ptMethods[MethodRequested::t_drawWindow] = &RequestManager::drawWindow;
	_ptMethods[MethodRequested::t_setWindowBounds] = &RequestManager::setWindowBounds;
	_ptMethods[MethodRequested::t_showWindow] = &RequestManager::showWindow;
	_ptMethods[MethodRequested::t_hideWindow] = &RequestManager::hideWindow;
	_ptMethods[MethodRequested::t_raiseWindowToTop] = &RequestManager::raiseWindowToTop;
	_ptMethods[MethodRequested::t_renderWindowFrom] = &RequestManager::renderWindowFrom;
	_ptMethods[MethodRequested::t_setWindowBgColor] = &RequestManager::setWindowBgColor;
	_ptMethods[MethodRequested::t_setWindowBorder] = &RequestManager::setWindowBorder;
	_ptMethods[MethodRequested::t_setWindowCurrentTransparency] = &RequestManager::setWindowCurrentTransparency;
	_ptMethods[MethodRequested::t_setWindowColorKey] = &RequestManager::setWindowColorKey;
	_ptMethods[MethodRequested::t_setWindowX] = &RequestManager::setWindowX;
	_ptMethods[MethodRequested::t_setWindowY] = &RequestManager::setWindowY;
	_ptMethods[MethodRequested::t_setWindowW] = &RequestManager::setWindowW;
	_ptMethods[MethodRequested::t_setWindowH] = &RequestManager::setWindowH;
	_ptMethods[MethodRequested::t_setWindowZ] = &RequestManager::setWindowZ;
	_ptMethods[MethodRequested::t_disposeWindow] = &RequestManager::disposeWindow;
	_ptMethods[MethodRequested::t_setGhostWindow] = &RequestManager::setGhostWindow;
	_ptMethods[MethodRequested::t_validateWindow] = &RequestManager::validateWindow;
	_ptMethods[MethodRequested::t_getWindowX] = &RequestManager::getWindowX;
	_ptMethods[MethodRequested::t_getWindowY] = &RequestManager::getWindowY;
	_ptMethods[MethodRequested::t_getWindowW] = &RequestManager::getWindowW;
	_ptMethods[MethodRequested::t_getWindowH] = &RequestManager::getWindowH;
	_ptMethods[MethodRequested::t_getWindowZ] = &RequestManager::getWindowZ;
	_ptMethods[MethodRequested::t_getWindowTransparencyValue] = &RequestManager::getWindowTransparencyValue;
	_ptMethods[MethodRequested::t_resizeWindow] = &RequestManager::resizeWindow;
	_ptMethods[MethodRequested::t_getWindowDumpFileUri] = &RequestManager::getWindowDumpFileUri;
	_ptMethods[MethodRequested::t_clearWindowContent] = &RequestManager::clearWindowContent;
	_ptMethods[MethodRequested::t_revertWindowContent] = &RequestManager::revertWindowContent;
	_ptMethods[MethodRequested::t_deleteWindow] = &RequestManager::deleteWindow;
	_ptMethods[MethodRequested::t_moveWindowTo] = &RequestManager::moveWindowTo;
	_ptMethods[MethodRequested::t_setWindowMirrorSrc] = &RequestManager::setWindowMirrorSrc;
	_ptMethods[MethodRequested::t_getSurfaceContent] = &RequestManager::getSurfaceContent;
	_ptMethods[MethodRequested::t_getSurfaceParentWindow] = &RequestManager::getSurfaceParentWindow;
	_ptMethods[MethodRequested::t_deleteSurface] = &RequestManager::deleteSurface;
	_ptMethods[MethodRequested::t_setSurfaceParentWindow] = &RequestManager::setSurfaceParentWindow;
	_ptMethods[MethodRequested::t_clearSurfaceContent] = &RequestManager::clearSurfaceContent;
	_ptMethods[MethodRequested::t_getSurfaceSize] = &RequestManager::getSurfaceSize;
	_ptMethods[MethodRequested::t_addSurfaceCaps] = &RequestManager::addSurfaceCaps;
	_ptMethods[MethodRequested::t_setSurfaceCaps] = &RequestManager::setSurfaceCaps;
	_ptMethods[MethodRequested::t_getSurfaceCap] = &RequestManager::getSurfaceCap;
	_ptMethods[MethodRequested::t_getSurfaceCaps] = &RequestManager::getSurfaceCaps;
	_ptMethods[MethodRequested::t_setSurfaceBgColor] = &RequestManager::setSurfaceBgColor;
	_ptMethods[MethodRequested::t_setSurfaceFont] = &RequestManager::setSurfaceFont;
	_ptMethods[MethodRequested::t_setColor] = &RequestManager::setColor;
	_ptMethods[MethodRequested::t_setSurfaceExternalHandler] = &RequestManager::setSurfaceExternalHandler;
	_ptMethods[MethodRequested::t_blitSurface] = &RequestManager::blitSurface;
	_ptMethods[MethodRequested::t_flipSurface] = &RequestManager::flipSurface;
	_ptMethods[MethodRequested::t_setSurfaceContent] = &RequestManager::setSurfaceContent;
	_ptMethods[MethodRequested::t_getSurfaceColor] = &RequestManager::getSurfaceColor;
	_ptMethods[MethodRequested::t_hasSurfaceExternalHandler] = &RequestManager::hasSurfaceExternalHandler;
	_ptMethods[MethodRequested::t_setSurfaceColor] = &RequestManager::setSurfaceColor;
	_ptMethods[MethodRequested::t_setProviderSoundLevel] = &RequestManager::setProviderSoundLevel;
	_ptMethods[MethodRequested::t_getProviderOriginalResolution] = &RequestManager::getProviderOriginalResolution;
	_ptMethods[MethodRequested::t_getProviderTotalMediaTime] = &RequestManager::getProviderTotalMediaTime;
	_ptMethods[MethodRequested::t_getProviderVPts] = &RequestManager::getProviderVPts;
	_ptMethods[MethodRequested::t_setProviderMediaTime] = &RequestManager::setProviderMediaTime;
	_ptMethods[MethodRequested::t_getProviderMediaTime] = &RequestManager::getProviderMediaTime;
	_ptMethods[MethodRequested::t_pauseProvider] = &RequestManager::pauseProvider;
	_ptMethods[MethodRequested::t_stopProvider] = &RequestManager::stopProvider;
	_ptMethods[MethodRequested::t_resumeProvider] = &RequestManager::resumeProvider;
	_ptMethods[MethodRequested::t_setProviderAVPid] = &RequestManager::setProviderAVPid;
	_ptMethods[MethodRequested::t_feedProviderBuffers] = &RequestManager::feedProviderBuffers;
	_ptMethods[MethodRequested::t_checkProviderVideoResizeEvent] = &RequestManager::checkProviderVideoResizeEvent;
	_ptMethods[MethodRequested::t_getProviderStringWidth] = &RequestManager::getProviderStringWidth;
	_ptMethods[MethodRequested::t_playProviderOver_provId_surId] = &RequestManager::playProviderOver_provId_surId;
	_ptMethods[MethodRequested::t_playProviderOver_provId_surId_char_2int_short] = &RequestManager::playProviderOver_provId_surId_char_2int_short;
	_ptMethods[MethodRequested::t_playProviderOver_provId_surId_bool] = &RequestManager::playProviderOver_provId_surId_bool;
	_ptMethods[MethodRequested::t_getProviderHeight] = &RequestManager::getProviderHeight;

}

std::string RequestManager::processRequest (std::string& request) 
{	
	_screenManager = LocalScreenManager::getInstance();

	std::vector <std::string> args;
	MethodRequested code;
	std::string response;
	if (MessageHandler::extractRequestMessage (request, code, args))
	{
		std::cout << code << std::endl;
		for (int i = 0; i < args.size(); i++)
		{
			std::cout << args[i] << std::endl;
		}

		if (code >=0 && code < MethodRequested::LENGHT)
			response = (this->*_ptMethods[code])(code, args);			
		else
			response = MessageHandler::createResponseMessage(ResponseCode::METHOD_NOT_FOUND);
	}
	else
		response = MessageHandler::createResponseMessage(ResponseCode::INTERNAL_ERROR);

	return response;
}

std::string RequestManager::createScreen(
	const MethodRequested& code, const std::vector <std::string>& args)
{
	if (code != MethodRequested::t_createScreen || args.size () == 0)	
		return MessageHandler::createResponseMessage(ResponseCode::INTERNAL_ERROR);

	int argc = atoi(args[0].c_str());
	char **createScreenArgs;
	createScreenArgs = (char **) malloc (args.size() - 1 * sizeof(char *));
	for (int i = 1; i < args.size (); i++)
	{
		createScreenArgs[i - 1] = (char *) malloc ((args[i].length() + 1) * sizeof (char));
		createScreenArgs[i - 1] = (char *) args[i].c_str();
	}
	
	GingaScreenID screenId = _screenManager->createScreen (argc, createScreenArgs);
	
	std::vector<std::string> respArgs;
	respArgs.push_back(to_string(screenId));

	return MessageHandler::createResponseMessage (ResponseCode::SUCCESS, respArgs);
	
}

std::string RequestManager::getDeviceWidth(
		const MethodRequested& code, const std::vector <std::string>& args)
{
	if (code != MethodRequested::t_getDeviceWidth || args.size () < 1)	
		return MessageHandler::createResponseMessage(ResponseCode::INTERNAL_ERROR);

	GingaScreenID screenId = (GingaScreenID) atoi(args[0].c_str());
	
	int width = _screenManager->getDeviceWidth(screenId);
	
	std::vector<std::string> respArgs;
	respArgs.push_back(to_string(width));

	return MessageHandler::createResponseMessage (ResponseCode::SUCCESS, respArgs);
}

std::string RequestManager::getDeviceHeight(
		const MethodRequested& code, const std::vector <std::string>& args)
{
	if (code != MethodRequested::t_getDeviceHeight || args.size () > 1)	
		return MessageHandler::createResponseMessage(ResponseCode::INTERNAL_ERROR);

	GingaScreenID screenId = (GingaScreenID) atoi(args[0].c_str());
	
	int height = _screenManager->getDeviceHeight(screenId);
	
	std::vector<std::string> respArgs;
	respArgs.push_back(to_string(height));

	return MessageHandler::createResponseMessage (ResponseCode::SUCCESS, respArgs);
}

std::string RequestManager::getInputManager (
		const MethodRequested& code, const std::vector <std::string>& args)
{
	if (code != MethodRequested::t_getInputManager || args.size () < 1)	
		return MessageHandler::createResponseMessage(ResponseCode::INTERNAL_ERROR);

	GingaScreenID screenId = (GingaScreenID) atoi(args[0].c_str());
	
	_screenManager->getInputManager(screenId);
	
	return MessageHandler::createResponseMessage (ResponseCode::SUCCESS);
}


std::string RequestManager::createEventBuffer (
		const MethodRequested& code, const std::vector <std::string>& args)
{
	if (code != MethodRequested::t_createEventBuffer || args.size () < 1)	
		return MessageHandler::createResponseMessage(ResponseCode::INTERNAL_ERROR);

	GingaScreenID screenId = (GingaScreenID) atoi(args[0].c_str());
	
	_screenManager->createEventBuffer(screenId);
	
	return MessageHandler::createResponseMessage (ResponseCode::SUCCESS);
}


std::string RequestManager::createWindow_screenID_4int_float (
		const MethodRequested& code, const std::vector <std::string>& args)
{
	if (code != MethodRequested::t_createWindow_screenID_4int_float || args.size () < 6)	
		return MessageHandler::createResponseMessage(ResponseCode::INTERNAL_ERROR);

	GingaScreenID screenId = (GingaScreenID) atoi(args[0].c_str());
	int x = atoi (args[1].c_str());
	int y = atoi (args[2].c_str());
	int w = atoi (args[3].c_str());
	int h = atoi (args[4].c_str());
	float z = atof (args[5].c_str());
	
	GingaWindowID winId = _screenManager->createWindow(screenId, x, y, w, h, z);
	
	std::vector<std::string> respArgs;
	respArgs.push_back(to_string(winId));
	
	return MessageHandler::createResponseMessage (ResponseCode::SUCCESS, respArgs);
}

std::string RequestManager::getWindowCap (
		const MethodRequested& code, const std::vector <std::string>& args)
{
	if (code != MethodRequested::t_getWindowCap || args.size () < 3)	
		return MessageHandler::createResponseMessage(ResponseCode::INTERNAL_ERROR);

	GingaScreenID screenId = (GingaScreenID) atoi(args[0].c_str());
	GingaWindowID winId = (GingaWindowID) atoi (args[1].c_str());
	string capName = args[2];	
	
	int cap = _screenManager->getWindowCap(screenId, winId, capName);
	
	std::vector<std::string> respArgs;
	respArgs.push_back(to_string(cap));
	
	return MessageHandler::createResponseMessage (ResponseCode::SUCCESS, respArgs);
}

std::string RequestManager::setWindowCaps (
		const MethodRequested& code, const std::vector <std::string>& args)
{
	if (code != MethodRequested::t_setWindowCaps || args.size () < 3)	
		return MessageHandler::createResponseMessage(ResponseCode::INTERNAL_ERROR);

	GingaScreenID screenId = (GingaScreenID) atoi(args[0].c_str());
	GingaWindowID winId = (GingaWindowID) atoi (args[1].c_str());
	int caps = atoi (args[2].c_str());	
	
	_screenManager->setWindowCaps(screenId, winId, caps);
	
	return MessageHandler::createResponseMessage (ResponseCode::SUCCESS);
}

std::string RequestManager::addWindowCaps (
		const MethodRequested& code, const std::vector <std::string>& args)
{
	if (code != MethodRequested::t_addWindowCaps || args.size () < 3)	
		return MessageHandler::createResponseMessage(ResponseCode::INTERNAL_ERROR);

	GingaScreenID screenId = (GingaScreenID) atoi(args[0].c_str());
	GingaWindowID winId = (GingaWindowID) atoi (args[1].c_str());
	int caps = atoi (args[2].c_str());	
	
	_screenManager->addWindowCaps(screenId, winId, caps);
	
	return MessageHandler::createResponseMessage (ResponseCode::SUCCESS);
}

std::string RequestManager::drawWindow (
		const MethodRequested& code, const std::vector <std::string>& args)
{
	if (code != MethodRequested::t_drawWindow || args.size () < 2)	
		return MessageHandler::createResponseMessage(ResponseCode::INTERNAL_ERROR);

	GingaScreenID screenId = (GingaScreenID) atoi(args[0].c_str());
	GingaWindowID winId = (GingaWindowID) atoi (args[1].c_str());
	
	_screenManager->drawWindow(screenId, winId);
	
	return MessageHandler::createResponseMessage (ResponseCode::SUCCESS);
}

std::string RequestManager::setWindowCurrentTransparency (
		const MethodRequested& code, const std::vector <std::string>& args)
{
	if (code != MethodRequested::t_setWindowCurrentTransparency || args.size () < 3)	
		return MessageHandler::createResponseMessage(ResponseCode::INTERNAL_ERROR);

	GingaScreenID screenId = (GingaScreenID) atoi(args[0].c_str());
	GingaWindowID winId = (GingaWindowID) atoi (args[1].c_str());
	int transparency = atoi (args[2].c_str());
	
	_screenManager->setWindowCurrentTransparency(screenId, winId, transparency);
	
	return MessageHandler::createResponseMessage (ResponseCode::SUCCESS);
}

std::string RequestManager::setWindowZ (
		const MethodRequested& code, const std::vector <std::string>& args)
{
	if (code != MethodRequested::t_setWindowZ || args.size () < 3)	
		return MessageHandler::createResponseMessage(ResponseCode::INTERNAL_ERROR);

	GingaScreenID screenId = (GingaScreenID) atoi(args[0].c_str());
	GingaWindowID winId = (GingaWindowID) atoi (args[1].c_str());
	float z = atof (args[2].c_str());
	
	_screenManager->setWindowZ(screenId, winId, z);
	
	return MessageHandler::createResponseMessage (ResponseCode::SUCCESS);
}
std::string RequestManager::raiseWindowToTop (
		const MethodRequested& code, const std::vector <std::string>& args)
{
	if (code != MethodRequested::t_raiseWindowToTop || args.size () < 2)	
		return MessageHandler::createResponseMessage(ResponseCode::INTERNAL_ERROR);

	GingaScreenID screenId = (GingaScreenID) atoi(args[0].c_str());
	GingaWindowID winId = (GingaWindowID) atoi (args[1].c_str());
	
	_screenManager->raiseWindowToTop(screenId, winId);
	
	return MessageHandler::createResponseMessage (ResponseCode::SUCCESS);
}

std::string RequestManager::createSurface_screenId (
		const MethodRequested& code, const std::vector <std::string>& args)
{
	if (code != MethodRequested::t_createSurface_screenId || args.size () < 1)	
		return MessageHandler::createResponseMessage(ResponseCode::INTERNAL_ERROR);

	GingaScreenID screenId = (GingaScreenID) atoi(args[0].c_str());
	
	GingaSurfaceID surfaceId = _screenManager->createSurface(screenId);

	std::vector<std::string> respArgs;
	respArgs.push_back(to_string(surfaceId));
	
	return MessageHandler::createResponseMessage (ResponseCode::SUCCESS, respArgs);
}

std::string RequestManager::createContinuousMediaProvider (
		const MethodRequested& code, const std::vector <std::string>& args)
{
	if (code != MethodRequested::t_createContinuousMediaProvider|| args.size () < 4)	
		return MessageHandler::createResponseMessage(ResponseCode::INTERNAL_ERROR);

	GingaScreenID screenId = (GingaScreenID) atoi(args[0].c_str());
	const char* mrl = args[1].c_str();
	bool hasVisual = (bool) atoi (args[2].c_str());
	int isRemote = atoi (args[3].c_str());

	GingaProviderID providerId = 
		_screenManager->createContinuousMediaProvider(screenId, mrl, &hasVisual, isRemote);

	std::vector<std::string> respArgs;
	respArgs.push_back(to_string(providerId));
	respArgs.push_back(to_string(hasVisual));
	
	return MessageHandler::createResponseMessage (ResponseCode::SUCCESS, respArgs);
}

std::string RequestManager::releaseContinuousMediaProvider (
		const MethodRequested& code, const std::vector <std::string>& args)
{
	if (code != MethodRequested::t_releaseContinuousMediaProvider || args.size () < 2)	
		return MessageHandler::createResponseMessage(ResponseCode::INTERNAL_ERROR);

	GingaScreenID screenId = (GingaScreenID) atoi(args[0].c_str());
	GingaProviderID providerId = (GingaProviderID) atoi(args[1].c_str());

	_screenManager->releaseContinuousMediaProvider(screenId, providerId);
	
	return MessageHandler::createResponseMessage (ResponseCode::SUCCESS);
}

std::string RequestManager::createFontProvider (
		const MethodRequested& code, const std::vector <std::string>& args)
{
	if (code != MethodRequested::t_createFontProvider || args.size () < 3)	
		return MessageHandler::createResponseMessage(ResponseCode::INTERNAL_ERROR);

	GingaScreenID screenId = (GingaScreenID) atoi(args[0].c_str());
	const char* mrl = args[1].c_str();
	int fontSize = atoi (args[2].c_str());

	GingaProviderID providerId = 
		_screenManager->createFontProvider(screenId, mrl, fontSize);

	std::vector<std::string> respArgs;
	respArgs.push_back(to_string(providerId));
	
	return MessageHandler::createResponseMessage (ResponseCode::SUCCESS, respArgs);
}

std::string RequestManager::releaseFontProvider (
		const MethodRequested& code, const std::vector <std::string>& args)
{
	if (code != MethodRequested::t_releaseFontProvider || args.size () < 2)	
		return MessageHandler::createResponseMessage(ResponseCode::INTERNAL_ERROR);

	GingaScreenID screenId = (GingaScreenID) atoi(args[0].c_str());
	GingaProviderID providerId = (GingaProviderID) atoi(args[1].c_str());
	
	_screenManager->releaseFontProvider(screenId, providerId);
	
	return MessageHandler::createResponseMessage (ResponseCode::SUCCESS);
}

std::string RequestManager::createImageProvider (
		const MethodRequested& code, const std::vector <std::string>& args)
{
	if (code != MethodRequested::t_createImageProvider || args.size () < 2)	
		return MessageHandler::createResponseMessage(ResponseCode::INTERNAL_ERROR);

	GingaScreenID screenId = (GingaScreenID) atoi(args[0].c_str());
	const char* mrl = args[1].c_str();
	
	GingaProviderID providerId = 
		_screenManager->createImageProvider (screenId, mrl);

	std::vector<std::string> respArgs;
	respArgs.push_back(to_string(providerId));
	
	return MessageHandler::createResponseMessage (ResponseCode::SUCCESS, respArgs);
}

std::string RequestManager::releaseImageProvider (
		const MethodRequested& code, const std::vector <std::string>& args)
{
	if (code != MethodRequested::t_releaseImageProvider || args.size () < 2)	
		return MessageHandler::createResponseMessage(ResponseCode::INTERNAL_ERROR);

	GingaScreenID screenId = (GingaScreenID) atoi(args[0].c_str());
	GingaProviderID providerId = (GingaProviderID) atoi(args[1].c_str());
	
	_screenManager->releaseImageProvider(screenId, providerId);
	
	return MessageHandler::createResponseMessage (ResponseCode::SUCCESS);
}

std::string RequestManager::getProviderTotalMediaTime (
		const MethodRequested& code, const std::vector <std::string>& args)
{
	if (code != MethodRequested::t_getProviderTotalMediaTime || args.size () < 1)	
		return MessageHandler::createResponseMessage(ResponseCode::INTERNAL_ERROR);

	GingaProviderID providerId = (GingaProviderID) atoi(args[0].c_str());
	
	double totalTime =  _screenManager->getProviderTotalMediaTime(providerId);

	std::vector<std::string> respArgs;
	respArgs.push_back(to_string(totalTime));
	
	return MessageHandler::createResponseMessage (ResponseCode::SUCCESS, respArgs);
}

std::string RequestManager::setProviderSoundLevel (
		const MethodRequested& code, const std::vector <std::string>& args)
{
	if (code != MethodRequested::t_setProviderSoundLevel || args.size () < 2)	
		return MessageHandler::createResponseMessage(ResponseCode::INTERNAL_ERROR);

	GingaProviderID providerId = (GingaProviderID) atoi(args[0].c_str());
	float level = atof (args[1].c_str());
	
	_screenManager->setProviderSoundLevel(providerId, level);

	return MessageHandler::createResponseMessage (ResponseCode::SUCCESS);
}

std::string RequestManager::getSurfaceParentWindow (
		const MethodRequested& code, const std::vector <std::string>& args)
{
	if (code != MethodRequested::t_getSurfaceParentWindow || args.size () < 1)	
		return MessageHandler::createResponseMessage(ResponseCode::INTERNAL_ERROR);

	GingaSurfaceID surfaceId = (GingaSurfaceID) atoi(args[0].c_str());
	
	GingaWindowID winId = _screenManager->getSurfaceParentWindow(surfaceId);
	
	std::vector<std::string> respArgs;
	respArgs.push_back(to_string(winId));

	return MessageHandler::createResponseMessage (ResponseCode::SUCCESS, respArgs);
}

std::string RequestManager::setSurfaceParentWindow (
		const MethodRequested& code, const std::vector <std::string>& args)
{
	if (code != MethodRequested::t_setSurfaceParentWindow || args.size () < 3)	
		return MessageHandler::createResponseMessage(ResponseCode::INTERNAL_ERROR);

	GingaScreenID screenId = (GingaScreenID) atoi(args[0].c_str());
	GingaSurfaceID surfaceId = (GingaSurfaceID) atoi(args[1].c_str());
	GingaWindowID winId = (GingaWindowID) atoi(args[2].c_str());
	
	bool result = _screenManager->setSurfaceParentWindow(screenId, surfaceId, winId);
	
	std::vector<std::string> respArgs;
	respArgs.push_back(to_string(result));

	return MessageHandler::createResponseMessage (ResponseCode::SUCCESS, respArgs);
}

std::string RequestManager::showWindow (
		const MethodRequested& code, const std::vector <std::string>& args)
{
	if (code != MethodRequested::t_showWindow || args.size () < 2)	
		return MessageHandler::createResponseMessage(ResponseCode::INTERNAL_ERROR);

	GingaScreenID screenId = (GingaScreenID) atoi(args[0].c_str());	
	GingaWindowID winId = (GingaWindowID) atoi(args[1].c_str());
	
	_screenManager->showWindow(screenId, winId);	

	return MessageHandler::createResponseMessage (ResponseCode::SUCCESS);
}

std::string RequestManager::hideWindow (
		const MethodRequested& code, const std::vector <std::string>& args)
{
	if (code != MethodRequested::t_hideWindow || args.size () < 2)	
		return MessageHandler::createResponseMessage(ResponseCode::INTERNAL_ERROR);

	GingaScreenID screenId = (GingaScreenID) atoi(args[0].c_str());	
	GingaWindowID winId = (GingaWindowID) atoi(args[1].c_str());
	
	_screenManager->hideWindow(screenId, winId);	

	return MessageHandler::createResponseMessage (ResponseCode::SUCCESS);
}

std::string RequestManager::getProviderMediaTime (
		const MethodRequested& code, const std::vector <std::string>& args)
{
	if (code != MethodRequested::t_getProviderMediaTime || args.size () < 1)	
		return MessageHandler::createResponseMessage(ResponseCode::INTERNAL_ERROR);

	GingaProviderID providerId = (GingaProviderID) atoi(args[0].c_str());	
	
	double mediaTime = _screenManager->getProviderMediaTime(providerId);

	std::vector<std::string> respArgs;
	respArgs.push_back(to_string(mediaTime));

	return MessageHandler::createResponseMessage (ResponseCode::SUCCESS, respArgs);
}

std::string RequestManager::playProviderOver_provId_surId_bool (
		const MethodRequested& code, const std::vector <std::string>& args)
{
	if (code != MethodRequested::t_playProviderOver_provId_surId_bool || args.size () < 3)	
		return MessageHandler::createResponseMessage(ResponseCode::INTERNAL_ERROR);

	GingaProviderID providerId = (GingaProviderID) atoi(args[0].c_str());	
	GingaSurfaceID surfaceId = (GingaSurfaceID) atoi(args[1].c_str());	
	int hasVisual = atoi(args[2].c_str());	
	
	_screenManager->playProviderOver(providerId, surfaceId, hasVisual);

	return MessageHandler::createResponseMessage (ResponseCode::SUCCESS);
}

std::string RequestManager::setBackgroundImage(
		const MethodRequested& code, const std::vector <std::string>& args)
{
	if (code != MethodRequested::t_setBackgroundImage || args.size () < 2)	
		return MessageHandler::createResponseMessage(ResponseCode::INTERNAL_ERROR);

	GingaScreenID screenId = (GingaScreenID) atoi(args[0].c_str());	
	std::string uri = args[1];	

	_screenManager->setBackgroundImage(screenId, uri);

	return MessageHandler::createResponseMessage (ResponseCode::SUCCESS);
}

std::string RequestManager::getScreenName(
		const MethodRequested& code, const std::vector <std::string>& args)
{
	if (code != MethodRequested::t_getScreenName || args.size () < 1)	
		return MessageHandler::createResponseMessage(ResponseCode::INTERNAL_ERROR);

	GingaScreenID screenId = (GingaScreenID) atoi(args[0].c_str());	

	std::string screenName = _screenManager->getScreenName(screenId);
	
	std::vector<std::string> respArgs;
	respArgs.push_back(screenName);

	return MessageHandler::createResponseMessage (ResponseCode::SUCCESS, respArgs);
}

std::string RequestManager::createRenderedSurfaceFromImageFile (
		const MethodRequested& code, const std::vector <std::string>& args)
{
	if (code != MethodRequested::t_createRenderedSurfaceFromImageFile || args.size () < 2)	
		return MessageHandler::createResponseMessage(ResponseCode::INTERNAL_ERROR);

	GingaScreenID screenId = (GingaScreenID) atoi(args[0].c_str());	
	std::string mrl = args[1];	

	GingaSurfaceID surfaceId = _screenManager->createRenderedSurfaceFromImageFile(screenId, mrl.c_str());
	
	std::vector<std::string> respArgs;
	respArgs.push_back(to_string(surfaceId));

	return MessageHandler::createResponseMessage (ResponseCode::SUCCESS, respArgs);
}

std::string RequestManager::setWindowBounds (
		const MethodRequested& code, const std::vector <std::string>& args)
{
	if (code != MethodRequested::t_setWindowBounds || args.size () < 6)	
		return MessageHandler::createResponseMessage(ResponseCode::INTERNAL_ERROR);

	GingaScreenID screenId = (GingaScreenID) atoi(args[0].c_str());	
	GingaWindowID winId = (GingaWindowID) atoi(args[1].c_str());	
	int x = atoi (args[2].c_str());
	int y = atoi (args[3].c_str());
	int w = atoi (args[4].c_str());
	int h = atoi (args[5].c_str());

	_screenManager->setWindowBounds(screenId, winId, x, y, w, h);

	return MessageHandler::createResponseMessage (ResponseCode::SUCCESS);
}

std::string RequestManager::createSurfaceFrom (
		const MethodRequested& code, const std::vector <std::string>& args)
{
	if (code != MethodRequested::t_createSurfaceFrom || args.size () < 2)	
		return MessageHandler::createResponseMessage(ResponseCode::INTERNAL_ERROR);

	GingaScreenID screenId = (GingaScreenID) atoi(args[0].c_str());	
	GingaSurfaceID underlyingSuface = (GingaSurfaceID) atoi(args[1].c_str());	
	
	GingaSurfaceID surfaceId = _screenManager->createSurfaceFrom(screenId, underlyingSuface);

	std::vector<std::string> respArgs;
	respArgs.push_back(to_string(surfaceId));

	return MessageHandler::createResponseMessage (ResponseCode::SUCCESS, respArgs);
}

std::string RequestManager::playProviderOver_provId_surId (
		const MethodRequested& code, const std::vector <std::string>& args)
{
	if (code != MethodRequested::t_playProviderOver_provId_surId || args.size () < 2)	
		return MessageHandler::createResponseMessage(ResponseCode::INTERNAL_ERROR);

	GingaProviderID providerId = (GingaProviderID) atoi(args[0].c_str());	
	GingaSurfaceID surfaceId = (GingaSurfaceID) atoi(args[1].c_str());	
	
	_screenManager->playProviderOver(providerId, surfaceId);

	return MessageHandler::createResponseMessage (ResponseCode::SUCCESS);
}

std::string RequestManager::validateWindow (
		const MethodRequested& code, const std::vector <std::string>& args)
{
	if (code != MethodRequested::t_validateWindow || args.size () < 2)	
		return MessageHandler::createResponseMessage(ResponseCode::INTERNAL_ERROR);

	GingaScreenID screenId = (GingaScreenID) atoi(args[0].c_str());	
	GingaWindowID windowId = (GingaWindowID) atoi(args[1].c_str());	
	
	_screenManager->validateWindow(screenId, windowId);

	return MessageHandler::createResponseMessage (ResponseCode::SUCCESS);
}
std::string RequestManager::setWindowBorder (
		const MethodRequested& code, const std::vector <std::string>& args)
{
	if (code != MethodRequested::t_setWindowBorder || args.size () < 7)	
		return MessageHandler::createResponseMessage(ResponseCode::INTERNAL_ERROR);

	GingaScreenID screenId = (GingaScreenID) atoi(args[0].c_str());	
	GingaWindowID windowId = (GingaWindowID) atoi(args[1].c_str());	
	int r = atoi(args[2].c_str());	
	int g = atoi(args[3].c_str());	
	int b = atoi(args[4].c_str());	
	int alpha = atoi(args[5].c_str());	
	int width = atoi(args[6].c_str());	

	_screenManager->setWindowBorder(screenId, windowId, r, g, b, alpha, width);

	return MessageHandler::createResponseMessage (ResponseCode::SUCCESS);
}

std::string RequestManager::getWindowTransparencyValue (
		const MethodRequested& code, const std::vector <std::string>& args)
{
	if (code != MethodRequested::t_getWindowTransparencyValue || args.size () < 2)	
		return MessageHandler::createResponseMessage(ResponseCode::INTERNAL_ERROR);

	GingaScreenID screenId = (GingaScreenID) atoi(args[0].c_str());	
	GingaWindowID windowId = (GingaWindowID) atoi(args[1].c_str());		

	int value = _screenManager->getWindowTransparencyValue(screenId, windowId);
	std::vector<std::string> respArgs;
	respArgs.push_back(to_string(value));
	return MessageHandler::createResponseMessage (ResponseCode::SUCCESS, respArgs);
}

std::string RequestManager::getWindowX (
		const MethodRequested& code, const std::vector <std::string>& args)
{
	if (code != MethodRequested::t_getWindowX || args.size () < 2)	
		return MessageHandler::createResponseMessage(ResponseCode::INTERNAL_ERROR);

	GingaScreenID screenId = (GingaScreenID) atoi(args[0].c_str());	
	GingaWindowID windowId = (GingaWindowID) atoi(args[1].c_str());		

	int value = _screenManager->getWindowX (screenId, windowId);
	std::vector<std::string> respArgs;
	respArgs.push_back(to_string(value));

	return MessageHandler::createResponseMessage (ResponseCode::SUCCESS, respArgs);
}

std::string RequestManager::getWindowY (
		const MethodRequested& code, const std::vector <std::string>& args)
{
	if (code != MethodRequested::t_getWindowY || args.size () < 2)	
		return MessageHandler::createResponseMessage(ResponseCode::INTERNAL_ERROR);

	GingaScreenID screenId = (GingaScreenID) atoi(args[0].c_str());	
	GingaWindowID windowId = (GingaWindowID) atoi(args[1].c_str());		

	int value = _screenManager->getWindowY (screenId, windowId);
	std::vector<std::string> respArgs;
	respArgs.push_back(to_string(value));

	return MessageHandler::createResponseMessage (ResponseCode::SUCCESS, respArgs);
}

std::string RequestManager::getWindowW (
		const MethodRequested& code, const std::vector <std::string>& args)
{
	if (code != MethodRequested::t_getWindowW || args.size () < 2)	
		return MessageHandler::createResponseMessage(ResponseCode::INTERNAL_ERROR);

	GingaScreenID screenId = (GingaScreenID) atoi(args[0].c_str());	
	GingaWindowID windowId = (GingaWindowID) atoi(args[1].c_str());		

	int value = _screenManager->getWindowW (screenId, windowId);
	std::vector<std::string> respArgs;
	respArgs.push_back(to_string(value));

	return MessageHandler::createResponseMessage (ResponseCode::SUCCESS, respArgs);
}

std::string RequestManager::getWindowH (
		const MethodRequested& code, const std::vector <std::string>& args)
{
	if (code != MethodRequested::t_getWindowH || args.size () < 2)	
		return MessageHandler::createResponseMessage(ResponseCode::INTERNAL_ERROR);

	GingaScreenID screenId = (GingaScreenID) atoi(args[0].c_str());	
	GingaWindowID windowId = (GingaWindowID) atoi(args[1].c_str());		

	int value = _screenManager->getWindowH (screenId, windowId);
	std::vector<std::string> respArgs;
	respArgs.push_back(to_string(value));

	return MessageHandler::createResponseMessage (ResponseCode::SUCCESS, respArgs);
}

std::string RequestManager::getWindowZ (
		const MethodRequested& code, const std::vector <std::string>& args)
{
	if (code != MethodRequested::t_getWindowZ || args.size () < 2)	
		return MessageHandler::createResponseMessage(ResponseCode::INTERNAL_ERROR);

	GingaScreenID screenId = (GingaScreenID) atoi(args[0].c_str());	
	GingaWindowID windowId = (GingaWindowID) atoi(args[1].c_str());		

	float value = _screenManager->getWindowZ (screenId, windowId);
	std::vector<std::string> respArgs;
	respArgs.push_back(to_string(value));

	return MessageHandler::createResponseMessage (ResponseCode::SUCCESS, respArgs);
}

std::string RequestManager::resizeWindow (
	const MethodRequested& code, const std::vector <std::string>& args)
{
	if (code != MethodRequested::t_resizeWindow || args.size () < 4)	
		return MessageHandler::createResponseMessage(ResponseCode::INTERNAL_ERROR);

	GingaScreenID screenId = (GingaScreenID) atoi(args[0].c_str());	
	GingaWindowID windowId = (GingaWindowID) atoi(args[1].c_str());		
	int width = atoi(args[2].c_str());		
	int height = atoi(args[3].c_str());		
	
	_screenManager->resizeWindow(screenId, windowId, width, height);

	return MessageHandler::createResponseMessage (ResponseCode::SUCCESS);
}

std::string RequestManager::disposeWindow (
		const MethodRequested& code, const std::vector <std::string>& args)
{
	if (code != MethodRequested::t_disposeWindow || args.size () < 2)	
		return MessageHandler::createResponseMessage(ResponseCode::INTERNAL_ERROR);

	GingaScreenID screenId = (GingaScreenID) atoi(args[0].c_str());	
	GingaWindowID windowId = (GingaWindowID) atoi(args[1].c_str());		
	
	_screenManager->disposeWindow(screenId, windowId);

	return MessageHandler::createResponseMessage (ResponseCode::SUCCESS);
}

std::string RequestManager::stopProvider (
		const MethodRequested& code, const std::vector <std::string>& args)
{
	if (code != MethodRequested::t_stopProvider || args.size () < 1)	
		return MessageHandler::createResponseMessage(ResponseCode::INTERNAL_ERROR);

	GingaProviderID providerId = (GingaProviderID) atoi(args[0].c_str());	
	
	_screenManager->stopProvider(providerId);

	return MessageHandler::createResponseMessage (ResponseCode::SUCCESS);
}

std::string RequestManager::pauseProvider (
		const MethodRequested& code, const std::vector <std::string>& args)
{
	if (code != MethodRequested::t_pauseProvider || args.size () < 1)	
		return MessageHandler::createResponseMessage(ResponseCode::INTERNAL_ERROR);

	GingaProviderID providerId = (GingaProviderID) atoi(args[0].c_str());	
	
	_screenManager->pauseProvider(providerId);

	return MessageHandler::createResponseMessage (ResponseCode::SUCCESS);
}

std::string RequestManager::resumeProvider (
		const MethodRequested& code, const std::vector <std::string>& args)
{
	if (code != MethodRequested::t_resumeProvider || args.size () < 3)	
		return MessageHandler::createResponseMessage(ResponseCode::INTERNAL_ERROR);

	GingaProviderID providerId = (GingaProviderID) atoi(args[0].c_str());	
	GingaSurfaceID surfaceId = (GingaSurfaceID) atoi(args[1].c_str());	
	int hasVisual = atoi(args[2].c_str());	
	
	_screenManager->resumeProvider(providerId, surfaceId, hasVisual);

	return MessageHandler::createResponseMessage (ResponseCode::SUCCESS);
}

std::string RequestManager::hasWindow (
		const MethodRequested& code, const std::vector <std::string>& args)
{
	if (code != MethodRequested::t_hasWindow || args.size () < 2)	
		return MessageHandler::createResponseMessage(ResponseCode::INTERNAL_ERROR);

	GingaScreenID screenId = (GingaScreenID) atoi(args[0].c_str());	
	GingaWindowID windowId = (GingaWindowID) atoi(args[1].c_str());		
	
	bool hasWin = _screenManager->hasWindow(screenId, windowId);
	std::vector<std::string> respArgs;

	respArgs.push_back(to_string(hasWin));

	return MessageHandler::createResponseMessage (ResponseCode::SUCCESS, respArgs);
}

std::string RequestManager::hasSurface (
		const MethodRequested& code, const std::vector <std::string>& args)
{
	if (code != MethodRequested::t_hasSurface || args.size () < 2)	
		return MessageHandler::createResponseMessage(ResponseCode::INTERNAL_ERROR);

	GingaScreenID screenId = (GingaScreenID) atoi(args[0].c_str());	
	GingaSurfaceID surfaceId = (GingaSurfaceID) atoi(args[1].c_str());		
	
	bool hasSur = _screenManager->hasSurface(screenId, surfaceId);
	std::vector<std::string> respArgs;

	respArgs.push_back(to_string(hasSur));

	return MessageHandler::createResponseMessage (ResponseCode::SUCCESS, respArgs);
}

std::string RequestManager::lowerWindowToBottom (
		const MethodRequested& code, const std::vector <std::string>& args)
{
	if (code != MethodRequested::t_lowerWindowToBottom || args.size () < 2)	
		return MessageHandler::createResponseMessage(ResponseCode::INTERNAL_ERROR);

	GingaScreenID screenId = (GingaScreenID) atoi(args[0].c_str());	
	GingaWindowID windowId = (GingaWindowID) atoi(args[1].c_str());		
	
	_screenManager->lowerWindowToBottom(screenId, windowId);

	return MessageHandler::createResponseMessage (ResponseCode::SUCCESS);
}

std::string RequestManager::deleteSurface (
		const MethodRequested& code, const std::vector <std::string>& args)
{
	if (code != MethodRequested::t_deleteSurface || args.size () < 1)	
		return MessageHandler::createResponseMessage(ResponseCode::INTERNAL_ERROR);

	GingaSurfaceID surfaceId = (GingaWindowID) atoi(args[0].c_str());		
	
	_screenManager->deleteSurface(surfaceId);

	return MessageHandler::createResponseMessage (ResponseCode::SUCCESS);
}

std::string RequestManager::renderWindowFrom (
		const MethodRequested& code, const std::vector <std::string>& args)
{
	if (code != MethodRequested::t_renderWindowFrom || args.size () < 3)	
		return MessageHandler::createResponseMessage(ResponseCode::INTERNAL_ERROR);

	GingaScreenID screenId = (GingaScreenID) atoi(args[0].c_str());		
	GingaWindowID windowId = (GingaWindowID) atoi(args[1].c_str());		
	GingaSurfaceID surfaceId = (GingaWindowID) atoi(args[2].c_str());		
	
	_screenManager->renderWindowFrom(screenId, windowId, surfaceId);

	return MessageHandler::createResponseMessage (ResponseCode::SUCCESS);
}

std::string RequestManager::deleteWindow (
		const MethodRequested& code, const std::vector <std::string>& args)
{
	if (code != MethodRequested::t_deleteWindow || args.size () < 2)	
		return MessageHandler::createResponseMessage(ResponseCode::INTERNAL_ERROR);

	GingaScreenID screenId = (GingaScreenID) atoi(args[0].c_str());		
	GingaWindowID windowId = (GingaWindowID) atoi(args[1].c_str());			
	
	_screenManager->deleteWindow(screenId, windowId);

	return MessageHandler::createResponseMessage (ResponseCode::SUCCESS);
}

std::string RequestManager::fromGingaToMB (
		const MethodRequested& code, const std::vector <std::string>& args)
{
	if (code != MethodRequested::t_fromGingaToMB || args.size () < 2)	
		return MessageHandler::createResponseMessage(ResponseCode::INTERNAL_ERROR);

	GingaScreenID screenId = (GingaScreenID) atoi(args[0].c_str());		
	int keyCode =  atoi(args[1].c_str());			
	
	int value = _screenManager->fromGingaToMB(screenId, keyCode);
	std::vector <std::string> respArgs;
	respArgs.push_back(to_string(value));

	return MessageHandler::createResponseMessage (ResponseCode::SUCCESS, respArgs);
}

std::string RequestManager::releaseScreen (
		const MethodRequested& code, const std::vector <std::string>& args)
{
	if (code != MethodRequested::t_releaseScreen || args.size () < 1)	
		return MessageHandler::createResponseMessage(ResponseCode::INTERNAL_ERROR);

	GingaScreenID screenId = (GingaScreenID) atoi(args[0].c_str());		
	
	_screenManager->releaseScreen(screenId);
	
	return MessageHandler::createResponseMessage (ResponseCode::SUCCESS);
}

std::string RequestManager::getGfxRoot(
		const MethodRequested& code, const std::vector <std::string>& args)
{
	return MessageHandler::createResponseMessage(ResponseCode::INTERNAL_ERROR);
}

std::string RequestManager::releaseMB(
		const MethodRequested& code, const std::vector <std::string>& args)
{
	if (code != MethodRequested::t_releaseMB || args.size () < 1)	
		return MessageHandler::createResponseMessage(ResponseCode::INTERNAL_ERROR);

	GingaScreenID screenId = (GingaScreenID) atoi(args[0].c_str());		
	
	_screenManager->releaseMB(screenId);
	
	return MessageHandler::createResponseMessage (ResponseCode::SUCCESS);
}

std::string RequestManager::clearWidgetPools(
		const MethodRequested& code, const std::vector <std::string>& args)
{
	if (code != MethodRequested::t_clearWidgetPools || args.size () < 1)	
		return MessageHandler::createResponseMessage(ResponseCode::INTERNAL_ERROR);

	GingaScreenID screenId = (GingaScreenID) atoi(args[0].c_str());		
	
	_screenManager->clearWidgetPools(screenId);
	
	return MessageHandler::createResponseMessage (ResponseCode::SUCCESS);
}

std::string RequestManager::getScreenUnderlyingWindow(
		const MethodRequested& code, const std::vector <std::string>& args)
{
	if (code != MethodRequested::t_getScreenUnderlyingWindow || args.size () < 1)	
		return MessageHandler::createResponseMessage(ResponseCode::INTERNAL_ERROR);

	GingaScreenID screenId = (GingaScreenID) atoi(args[0].c_str());		
	
	UnderlyingWindowID id = _screenManager->getScreenUnderlyingWindow(screenId);
	std::stringstream ss;
	ss << id;
	
	std::vector <std::string> respArgs;
	respArgs.push_back(ss.str());
	
	return MessageHandler::createResponseMessage (ResponseCode::SUCCESS, respArgs);
}

std::string RequestManager::clearWindowContent (
		const MethodRequested& code, const std::vector <std::string>& args)
{
	if (code != MethodRequested::t_clearWindowContent || args.size () < 2)	
		return MessageHandler::createResponseMessage(ResponseCode::INTERNAL_ERROR);

	GingaScreenID screenId = (GingaScreenID) atoi(args[0].c_str());		
	GingaWindowID windowId = (GingaWindowID) atoi(args[1].c_str());		
	
	_screenManager->clearWindowContent(screenId, windowId);
	
	return MessageHandler::createResponseMessage (ResponseCode::SUCCESS);
}