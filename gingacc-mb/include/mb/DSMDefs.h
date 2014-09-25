
#ifndef DSMDEFS
#define DSMDEFS

#define MAX_BUFFER_SIZE 1024

#include <boost/asio.hpp>
using boost::asio::ip::tcp;

typedef boost::shared_ptr<tcp::socket> socket_ptr;

enum ResponseCode
{
	SUCCESS = 200,
	INTERNAL_ERROR = 500,
	METHOD_NOT_FOUND = 404
};

enum EventType
{
	INPUT_EVENT,
	MOTION_EVENT
};


enum MethodRequested
{ 
	t_addIEListener, t_removeIEListener, t_hasIEListener,
	t_addMEListener, t_removeMEListener, t_hasMEListener,
	t_setBackgroundImage, t_getDeviceWidth, t_getDeviceHeight,
	t_getGfxRoot, t_releaseScreen, t_releaseMB, t_clearWidgetPools,
	t_createScreen, t_getScreenName, t_getScreenUnderlyingWindow,
	t_getIMediaProviderFromId, t_getISurfaceFromId, t_getIWindowFromId,
	t_mergeIds, t_blitScreen_screenId_isurface, t_blitScreen_screenId_string,
	t_refreshScreen, t_createWindow_screenID_4int_float, t_createUnderlyingSubWindow,
	t_createWindowFrom, t_hasWindow, t_releaseWindow, t_registerSurface,
	t_createSurface_screenId, t_createSurface_screenId_2int, t_createSurfaceFrom,

	t_hasSurface, t_releaseSurface, t_lowerWindowToBottom, 
	t_createContinuousMediaProvider, t_releaseContinuousMediaProvider, 
	t_createFontProvider, t_releaseFontProvider, t_createImageProvider, 
	t_releaseImageProvider, t_createRenderedSurfaceFromImageFile, t_getInputManager, 
	t_createEventBuffer, t_createInputEvent, t_createApplicationEvent,
	t_fromMBToGinga, t_fromGingaToMB, t_addWindowCaps, t_setWindowCaps,
	t_getWindowCap, t_drawWindow, t_setWindowBounds, t_showWindow, t_hideWindow,
	t_raiseWindowToTop, t_renderWindowFrom, t_setWindowBgColor, t_setWindowBorder,
	t_setWindowCurrentTransparency, t_setWindowColorKey, t_setWindowX, 
	t_setWindowY, t_setWindowW, t_setWindowH, t_setWindowZ, t_disposeWindow,
	t_setGhostWindow, t_validateWindow, t_getWindowX, t_getWindowY,
	t_getWindowW, t_getWindowH, t_getWindowZ, t_getWindowTransparencyValue,

	t_resizeWindow, t_getWindowDumpFileUri, t_clearWindowContent, 
	t_revertWindowContent, t_deleteWindow, t_moveWindowTo, t_setWindowMirrorSrc, 
	t_getSurfaceContent, t_getSurfaceParentWindow, t_deleteSurface,
	t_setSurfaceParentWindow, t_clearSurfaceContent, 
	t_getSurfaceSize, t_addSurfaceCaps, t_setSurfaceCaps,
	t_getSurfaceCap, t_getSurfaceCaps, t_setSurfaceBgColor,
	t_setSurfaceFont, t_setColor, t_setSurfaceExternalHandler,
	t_blitSurface, t_flipSurface, t_setSurfaceContent, 
	t_getSurfaceColor, t_hasSurfaceExternalHandler,
	t_setSurfaceColor, t_setProviderSoundLevel, 
	t_getProviderOriginalResolution, t_getProviderTotalMediaTime,
	t_getProviderVPts, t_setProviderMediaTime, t_getProviderMediaTime,
	t_pauseProvider, t_stopProvider, t_resumeProvider,
	t_setProviderAVPid, t_feedProviderBuffers, 
	t_checkProviderVideoResizeEvent, t_getProviderStringWidth,
	t_playProviderOver_provId_surId, 
	t_playProviderOver_provId_surId_char_2int_short,
	t_playProviderOver_provId_surId_bool,
	t_getProviderHeight,

	LENGHT = 118
};

#endif //DSMDEFS