

#ifndef REQUESTMANAGER_H
#define REQUESTMANAGER_H

#include "mb/LocalScreenManager.h"

#include "mb/DSMDefs.h"
#include "mb/remote/StubServer.h"
#include "mb/remote/MessageHandler.h"

#include <vector>
#include <string>

using namespace br::pucrio::telemidia::ginga::core::mb;

class RequestManager : public IRequestHandler
{
public:
	RequestManager ();

	std::string processRequest (std::string& request);

	std::string addIEListener (
		const MethodRequested&, const std::vector <std::string>&)
		 { return ""; }

	std::string removeIEListener (
		const MethodRequested&, const std::vector <std::string>&)
		 { return ""; }

	std::string hasIEListener (
		const MethodRequested&, const std::vector <std::string>&)
		 { return ""; }
	
	std::string addMEListener (
		const MethodRequested&, const std::vector <std::string>&)
		 { return ""; }

	std::string removeMEListener (
		const MethodRequested&, const std::vector <std::string>&)
		 { return ""; }

	std::string hasMEListener(
		const MethodRequested&, const std::vector <std::string>&)
		 { return ""; } 

	std::string setBackgroundImage(
		const MethodRequested&, const std::vector <std::string>&);		 

	std::string getDeviceWidth(
		const MethodRequested&, const std::vector <std::string>&);

	std::string getDeviceHeight(
		const MethodRequested&, const std::vector <std::string>&);

	std::string getGfxRoot(
		const MethodRequested&, const std::vector <std::string>&);

	std::string releaseScreen(
		const MethodRequested&, const std::vector <std::string>&);		 

	std::string releaseMB(
		const MethodRequested&, const std::vector <std::string>&);
	
	std::string clearWidgetPools(
		const MethodRequested&, const std::vector <std::string>&);

	std::string createScreen(
		const MethodRequested&, const std::vector <std::string>&); 

	std::string getScreenName(
		const MethodRequested&, const std::vector <std::string>&);	

	std::string getScreenUnderlyingWindow(
		const MethodRequested&, const std::vector <std::string>&);

	std::string getIMediaProviderFromId (
		const MethodRequested&, const std::vector <std::string>&)
		 { return ""; }

	std::string getISurfaceFromId (
		const MethodRequested&, const std::vector <std::string>&)
		 { return ""; }
	
	std::string getIWindowFromId (
		const MethodRequested&, const std::vector <std::string>&)
		 { return ""; }
	
	std::string mergeIds (
		const MethodRequested&, const std::vector <std::string>&)
		 { return ""; }
	
	std::string blitScreen_screenId_isurface (
		const MethodRequested&, const std::vector <std::string>&)
		 { return ""; }

	std::string blitScreen_screenId_string (
		const MethodRequested&, const std::vector <std::string>&)
		 { return ""; }
	
	std::string refreshScreen (
		const MethodRequested&, const std::vector <std::string>&)
		 { return ""; }

	std::string createWindow_screenID_4int_float (
		const MethodRequested&, const std::vector <std::string>&);

	std::string createUnderlyingSubWindow (
		const MethodRequested&, const std::vector <std::string>&)
		 { return ""; }
	
	std::string createWindowFrom (
		const MethodRequested&, const std::vector <std::string>&)
		 { return ""; }

	std::string hasWindow (
		const MethodRequested&, const std::vector <std::string>&);		 

	std::string releaseWindow (
		const MethodRequested&, const std::vector <std::string>&)
		 { return ""; }

	std::string registerSurface (
		const MethodRequested&, const std::vector <std::string>&)
		 { return ""; }

	std::string createSurface_screenId (
		const MethodRequested&, const std::vector <std::string>&);

	std::string createSurface_screenId_2int (
		const MethodRequested&, const std::vector <std::string>&)
		 { return ""; }
	
	std::string createSurfaceFrom (
		const MethodRequested&, const std::vector <std::string>&);

	std::string hasSurface (
		const MethodRequested&, const std::vector <std::string>&);		 
	
	std::string releaseSurface (
		const MethodRequested&, const std::vector <std::string>&)
		 { return ""; }

	std::string lowerWindowToBottom (
		const MethodRequested&, const std::vector <std::string>&);
	
	std::string createContinuousMediaProvider (
		const MethodRequested&, const std::vector <std::string>&);		 

	std::string releaseContinuousMediaProvider (
		const MethodRequested&, const std::vector <std::string>&);

	std::string createFontProvider (
		const MethodRequested&, const std::vector <std::string>&);

	std::string releaseFontProvider (
		const MethodRequested&, const std::vector <std::string>&);

	std::string createImageProvider (
		const MethodRequested&, const std::vector <std::string>&);

	std::string releaseImageProvider (
		const MethodRequested&, const std::vector <std::string>&);

	std::string createRenderedSurfaceFromImageFile (
		const MethodRequested&, const std::vector <std::string>&);		 
	
	std::string getInputManager (
		const MethodRequested&, const std::vector <std::string>&);

	std::string createEventBuffer (
		const MethodRequested&, const std::vector <std::string>&);

	std::string createInputEvent (
		const MethodRequested&, const std::vector <std::string>&)
		 { return ""; }

	std::string createApplicationEvent (
		const MethodRequested&, const std::vector <std::string>&)
		 { return ""; }
	
	std::string fromMBToGinga (
		const MethodRequested&, const std::vector <std::string>&)
		 { return ""; }

	std::string fromGingaToMB (
		const MethodRequested&, const std::vector <std::string>&);

	std::string addWindowCaps (
		const MethodRequested&, const std::vector <std::string>&);		 

	std::string setWindowCaps (
		const MethodRequested&, const std::vector <std::string>&);

	std::string getWindowCap (
		const MethodRequested&, const std::vector <std::string>&);

	std::string drawWindow (
		const MethodRequested&, const std::vector <std::string>&);
	
	std::string setWindowBounds (
		const MethodRequested&, const std::vector <std::string>&);		 
	
	std::string showWindow (
		const MethodRequested&, const std::vector <std::string>&);

	std::string hideWindow (
		const MethodRequested&, const std::vector <std::string>&);
	
	std::string raiseWindowToTop (
		const MethodRequested&, const std::vector <std::string>&);

	std::string renderWindowFrom (
		const MethodRequested&, const std::vector <std::string>&);

	std::string setWindowBgColor (
		const MethodRequested&, const std::vector <std::string>&)
		 { return ""; }

	std::string setWindowBorder (
		const MethodRequested&, const std::vector <std::string>&);

	std::string setWindowCurrentTransparency (
		const MethodRequested&, const std::vector <std::string>&);		 

	std::string setWindowColorKey (
		const MethodRequested&, const std::vector <std::string>&)
		 { return ""; }

	std::string setWindowX (
		const MethodRequested&, const std::vector <std::string>&)
		 { return "";}

	std::string setWindowY (
		const MethodRequested&, const std::vector <std::string>&)
		 { return "";}

	std::string setWindowW (
		const MethodRequested&, const std::vector <std::string>&)
		 { return "";}

	std::string setWindowH (
		const MethodRequested&, const std::vector <std::string>&)
		 { return "";}

	std::string setWindowZ (
		const MethodRequested&, const std::vector <std::string>&);

	std::string disposeWindow (
		const MethodRequested&, const std::vector <std::string>&);

	std::string setGhostWindow (
		const MethodRequested&, const std::vector <std::string>&)
		 { return "";}

	std::string validateWindow (
		const MethodRequested&, const std::vector <std::string>&);
	
	std::string getWindowX (
		const MethodRequested&, const std::vector <std::string>&);

	std::string getWindowY (
		const MethodRequested&, const std::vector <std::string>&);

	std::string getWindowW (
		const MethodRequested&, const std::vector <std::string>&);		 

	std::string getWindowH (
		const MethodRequested&, const std::vector <std::string>&);

	std::string getWindowZ (
		const MethodRequested&, const std::vector <std::string>&);		 

	std::string getWindowTransparencyValue (
		const MethodRequested&, const std::vector <std::string>&);

	std::string resizeWindow (
		const MethodRequested&, const std::vector <std::string>&);
	
	std::string getWindowDumpFileUri (
		const MethodRequested&, const std::vector <std::string>&)
		 { return "";}

	std::string clearWindowContent (
		const MethodRequested&, const std::vector <std::string>&);
	
	std::string revertWindowContent (
		const MethodRequested&, const std::vector <std::string>&)
		 { return "";}

	std::string deleteWindow (
		const MethodRequested&, const std::vector <std::string>&);

	std::string moveWindowTo (
		const MethodRequested&, const std::vector <std::string>&)
		 { return "";}

	std::string setWindowMirrorSrc (
		const MethodRequested&, const std::vector <std::string>&)
		 { return "";}

	std::string getSurfaceContent (
		const MethodRequested&, const std::vector <std::string>&)
		 { return "";}

	std::string getSurfaceParentWindow (
		const MethodRequested&, const std::vector <std::string>&);

	std::string deleteSurface (
		const MethodRequested&, const std::vector <std::string>&);

	std::string setSurfaceParentWindow (
		const MethodRequested&, const std::vector <std::string>&);		 

	std::string clearSurfaceContent (
		const MethodRequested&, const std::vector <std::string>&)
		 { return "";}

	std::string getSurfaceSize (
		const MethodRequested&, const std::vector <std::string>&)
		 { return "";}

	std::string addSurfaceCaps (
		const MethodRequested&, const std::vector <std::string>&)
		 { return "";}

	std::string setSurfaceCaps (
		const MethodRequested&, const std::vector <std::string>&)
		 { return "";}

	std::string getSurfaceCap (
		const MethodRequested&, const std::vector <std::string>&)
		 { return "";}

	std::string getSurfaceCaps (
		const MethodRequested&, const std::vector <std::string>&)
		 { return "";}

	std::string setSurfaceBgColor (
		const MethodRequested&, const std::vector <std::string>&)
		 { return "";}

	std::string setSurfaceFont (
		const MethodRequested&, const std::vector <std::string>&)
		 { return "";}

	std::string setColor (
		const MethodRequested&, const std::vector <std::string>&)
		 { return "";}

	std::string setSurfaceExternalHandler (
		const MethodRequested&, const std::vector <std::string>&)
		 { return "";}

	std::string blitSurface (
		const MethodRequested&, const std::vector <std::string>&)
		 { return "";}

	std::string flipSurface (
		const MethodRequested&, const std::vector <std::string>&)
		 { return "";}

	std::string setSurfaceContent (
		const MethodRequested&, const std::vector <std::string>&)
		 { return "";}

	std::string getSurfaceColor (
		const MethodRequested&, const std::vector <std::string>&)
		 { return "";}

	std::string hasSurfaceExternalHandler (
		const MethodRequested&, const std::vector <std::string>&)
		 { return "";}

	std::string setSurfaceColor (
		const MethodRequested&, const std::vector <std::string>&)
		 { return "";}

	std::string setProviderSoundLevel (
		const MethodRequested&, const std::vector <std::string>&);

	std::string getProviderOriginalResolution (
		const MethodRequested&, const std::vector <std::string>&)
		 { return "";}

	std::string getProviderTotalMediaTime (
		const MethodRequested&, const std::vector <std::string>&);

	std::string getProviderVPts (
		const MethodRequested&, const std::vector <std::string>&)
		 { return "";}

	std::string setProviderMediaTime (
		const MethodRequested&, const std::vector <std::string>&)
		 { return "";}

	std::string getProviderMediaTime (
		const MethodRequested&, const std::vector <std::string>&);

	std::string pauseProvider (
		const MethodRequested&, const std::vector <std::string>&);

	std::string stopProvider (
		const MethodRequested&, const std::vector <std::string>&);

	std::string resumeProvider (
		const MethodRequested&, const std::vector <std::string>&);
	
	std::string setProviderAVPid (
		const MethodRequested&, const std::vector <std::string>&)
		 { return "";}

	std::string feedProviderBuffers (
		const MethodRequested&, const std::vector <std::string>&)
		 { return "";}

	std::string checkProviderVideoResizeEvent (
		const MethodRequested&, const std::vector <std::string>&)
		 { return "";}

	std::string getProviderStringWidth (
		const MethodRequested&, const std::vector <std::string>&)
		 { return "";}
	
	std::string playProviderOver_provId_surId (
		const MethodRequested&, const std::vector <std::string>&);

	std::string playProviderOver_provId_surId_char_2int_short (
		const MethodRequested&, const std::vector <std::string>&)
		 { return "";}

	std::string playProviderOver_provId_surId_bool (
		const MethodRequested&, const std::vector <std::string>&);
	
	std::string getProviderHeight (
		const MethodRequested&, const std::vector <std::string>&)
		 { return ""; }

private:
	std::string (RequestManager::*_ptMethods[MethodRequested::LENGHT]) (const MethodRequested&, 
		const std::vector<std::string>& args) ;

	LocalScreenManager* _screenManager;
};

#endif //REQUESTMANAGER_H

