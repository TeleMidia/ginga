

#ifndef DISTRIBUTEDSCREENMANAGER_H
#define DISTRIBUTEDSCREENMANAGER_H

#include "system/thread/Thread.h"

#include <utility> 
#include <sstream>
#include <stdio.h>
#include <stdarg.h>
#include <vector>

#include "mb/LocalScreenManager.h"
#include "mb/remote/StubClient.h"

#include "mb/DSMDefs.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace mb {

class DistributedScreenManager : public LocalScreenManager
{
public:
	DistributedScreenManager();
	~DistributedScreenManager();

	void releaseHandler();

	static void addIEListenerInstance(
			IInputEventListener* listener);

	static void removeIEListenerInstance(
			IInputEventListener* listener);

	static bool hasIEListenerInstance(
			IInputEventListener* listener, bool removeInstance=false);

	static void addMEListenerInstance(
			IMotionEventListener* listener);

	static void removeMEListenerInstance(
			IMotionEventListener* listener);

	static bool hasMEListenerInstance(
			IMotionEventListener* listener, bool removeInstance=false);

	void setBackgroundImage(GingaScreenID screenId, string uri);

	int getDeviceWidth(GingaScreenID screenId);

	int getDeviceHeight(GingaScreenID screenId);

	void* getGfxRoot(GingaScreenID screenId);

	void releaseScreen(GingaScreenID screenId);
	void releaseMB(GingaScreenID screenId);
	void clearWidgetPools(GingaScreenID screenId);

	GingaScreenID createScreen(int argc, char** args);

	string getScreenName(GingaScreenID screenId);
	UnderlyingWindowID getScreenUnderlyingWindow(GingaScreenID screenId);
	
	IMediaProvider* getIMediaProviderFromId (const GingaProviderID& provId);
	ISurface* getISurfaceFromId(const GingaSurfaceID &surfaceId);

	IWindow* getIWindowFromId(
			GingaScreenID screenId, GingaWindowID winId);

	bool mergeIds(
			GingaScreenID screenId,
			GingaWindowID destId,
			vector<GingaWindowID>* srcIds);

	void blitScreen(GingaScreenID screenId, ISurface* destination);
	void blitScreen(GingaScreenID screenId, string fileUri);
	void refreshScreen(GingaScreenID screenId);


	/* Interfacing output */
	GingaWindowID createWindow(
			GingaScreenID screenId,
			int x, int y,
			int w, int h,
			float z);

	UnderlyingWindowID createUnderlyingSubWindow(
			GingaScreenID screenId,
			int x, int y,
			int w, int h,
			float z);

	bool hasWindow(GingaScreenID screenId, GingaWindowID window);

	void releaseWindow(GingaScreenID screenId, IWindow* window);

	void registerSurface (ISurface*);

	GingaSurfaceID createSurface(GingaScreenID screenId);

	GingaSurfaceID createSurface(GingaScreenID screenId, int w, int h);

	GingaSurfaceID createSurfaceFrom(
			GingaScreenID screenId, GingaSurfaceID underlyingSurface);

	bool hasSurface(
			const GingaScreenID &screenId, const GingaSurfaceID &surId);
	bool releaseSurface(GingaScreenID screenId, ISurface* surface);

	void lowerWindowToBottom (
			const GingaScreenID &screenId, const GingaWindowID &winId);

	/* Interfacing content */
	GingaProviderID createContinuousMediaProvider(
			GingaScreenID screenId,
			const char* mrl,
			bool isRemote);

	void releaseContinuousMediaProvider(
			GingaScreenID screenId,
			GingaProviderID provider);

	GingaProviderID createFontProvider(
			GingaScreenID screenId,
			const char* mrl,
			int fontSize);

	void releaseFontProvider(
			GingaScreenID screenId, GingaProviderID provider);

	GingaProviderID createImageProvider(
			GingaScreenID screenId, const char* mrl);

	void releaseImageProvider(
			GingaScreenID screenId, GingaProviderID provider);

	GingaSurfaceID createRenderedSurfaceFromImageFile(
			GingaScreenID screenId, const char* mrl);


	/* interfacing input */
	IInputManager* getInputManager(GingaScreenID screenId);
	IEventBuffer* createEventBuffer(GingaScreenID screenId);
	IInputEvent* createInputEvent(
			GingaScreenID screenId, void* event, const int symbol);

	IInputEvent* createApplicationEvent(
			GingaScreenID screenId, int type, void* data);

	int fromMBToGinga(GingaScreenID screenId, int keyCode);
	int fromGingaToMB(GingaScreenID screenId, int keyCode);

	/* Methods created to isolate gingacc-mb */
	//windows
	void addWindowCaps (const GingaScreenID &screenId,
			            const GingaWindowID &winId, int caps);
	void setWindowCaps (const GingaScreenID &screenId,
			            const GingaWindowID &winId, int caps);
	int getWindowCap (const GingaScreenID &screenId,
			            const GingaWindowID &winId, const string &capName);

	void drawWindow (const GingaScreenID &screenId,
			            const GingaWindowID &winId);
	void setWindowBounds (
			const GingaScreenID &screenId, const GingaWindowID &winId, int x,
			int y, int w, int h);
	void showWindow (const GingaScreenID &screenId,
			            const GingaWindowID &winId);
	void hideWindow (const GingaScreenID &screenId,
			            const GingaWindowID &winId);
	void raiseWindowToTop (const GingaScreenID &screenId,
			                const GingaWindowID &winId);
	void renderWindowFrom (
			const GingaScreenID &screenId, const GingaWindowID &winId,
			const GingaSurfaceID &surId);

	void setWindowBgColor (
			const GingaScreenID &screenId, const GingaWindowID &winId, int r,
			int g, int b, int alpha);

	void setWindowBorder (
			const GingaScreenID &screenId, const GingaWindowID &winId, int r,
			int g, int b, int alpha, int width);

	void setWindowCurrentTransparency (
			const GingaScreenID &screenId, const GingaWindowID &winId,
			int transparency);
	void setWindowColorKey (
			const GingaScreenID &screenId, const GingaWindowID &winId, int r,
			int g, int b);

	void setWindowX(
			const GingaScreenID &screenId, const GingaWindowID &winId, int x);

	void setWindowY(
			const GingaScreenID &screenId, const GingaWindowID &winId, int y);

	void setWindowW(
			const GingaScreenID &screenId, const GingaWindowID &winId, int w);

	void setWindowH(
			const GingaScreenID &screenId, const GingaWindowID &winId, int h);

	void setWindowZ(
			const GingaScreenID &screenId, const GingaWindowID &winId, float z);

	void disposeWindow (const GingaScreenID &screenId,
			            const GingaWindowID &winId);

	void setGhostWindow(const GingaScreenID &screenId,
			            const GingaWindowID &winId, bool ghost);

	void validateWindow (const GingaScreenID &screenId,
			                const GingaWindowID &winId);

	int getWindowX (const GingaScreenID &screenId,
			        const GingaWindowID &winId);
	int getWindowY (const GingaScreenID &screenId,
			        const GingaWindowID &winId);
	int getWindowW (const GingaScreenID &screenId,
			        const GingaWindowID &winId);
	int getWindowH (const GingaScreenID &screenId,
			        const GingaWindowID &winId);
	float getWindowZ (const GingaScreenID &screenId,
			            const GingaWindowID &winId);

	int getWindowTransparencyValue(const GingaScreenID &screenId,
			                        const GingaWindowID &winId);

	void resizeWindow(
			const GingaScreenID &screenId, const GingaWindowID &winId, int width,
			int height);

	string getWindowDumpFileUri (
			const GingaScreenID &screenId, const GingaWindowID &winId,
			int quality, int dumpW, int dumpH);

	void clearWindowContent (const GingaScreenID &screenId,
			                    const GingaWindowID &winId);

	void revertWindowContent(
						const GingaScreenID &screenId, const GingaWindowID &winId);

	void deleteWindow(
						const GingaScreenID &screenId, const GingaWindowID &winId);

	void moveWindowTo(
						const GingaScreenID &screenId, const GingaWindowID &winId,
						int x, int y);

	void setWindowMirrorSrc (
			const GingaScreenID &screenId, const GingaWindowID &winId,
			const GingaWindowID &mirrorSrc);

	//surfaces
	void* getSurfaceContent(const GingaSurfaceID &surId);

	GingaWindowID getSurfaceParentWindow(const GingaSurfaceID& surId);

	void deleteSurface (const GingaSurfaceID &surId);

	bool setSurfaceParentWindow (
			const GingaScreenID &screenId,  const GingaSurfaceID &surId,
			const GingaWindowID &winId);

	void clearSurfaceContent (const GingaSurfaceID &surId);

	void getSurfaceSize(const GingaSurfaceID &surId, int* width, int* height);

	void addSurfaceCaps(const GingaSurfaceID &surId, const int caps);
	void setSurfaceCaps (const GingaSurfaceID &surId, const int caps);
	int getSurfaceCap(const GingaSurfaceID &surId, const string &cap);
	int getSurfaceCaps(const GingaSurfaceID &surId);

	void setSurfaceBgColor(
			const GingaSurfaceID &surId, int r, int g, int b, int alpha);

	void setSurfaceFont(const GingaSurfaceID &surId, GingaSurfaceID font);

	void setColor(const GingaSurfaceID &surId, int r, int g, int b,int alpha);

	void setExternalHandler(const GingaSurfaceID &surId, bool extHandler);

	void blitSurface (
			const GingaSurfaceID &surId,int x, int y, GingaSurfaceID src=0,
			int srcX=-1, int srcY=-1, int srcW=-1, int srcH=-1);

	void flipSurface (const GingaSurfaceID &surId);

	void setSurfaceContent(const GingaSurfaceID &surId, void* surface);

	IColor* getSurfaceColor(const GingaSurfaceID &surId);

	bool hasSurfaceExternalHandler(const GingaSurfaceID &surId);

	void setSurfaceColor(
						const GingaSurfaceID &surId, int r, int g, int b, int alpha);

	//providers
	void setProviderSoundLevel (
						const GingaProviderID &provId, float level);

	void getProviderOriginalResolution(
						const GingaProviderID &provId, int* width, int* height);

	double getProviderTotalMediaTime(const GingaProviderID &provId);

	int64_t getProviderVPts(const GingaProviderID &provId);

	void setProviderMediaTime( const GingaProviderID &provId, double pos);

	double getProviderMediaTime(const GingaProviderID &provId);

	void pauseProvider (const GingaProviderID &provId);

	void stopProvider (const GingaProviderID &provId);

	void resumeProvider (
			const GingaProviderID &provId, GingaSurfaceID surface,bool hasVisual);

	void setProviderAVPid(const GingaProviderID &provId, int aPid, int vPid);

	void feedProviderBuffers(const GingaProviderID &provId);

	bool checkProviderVideoResizeEvent(
						const GingaProviderID &provId, const GingaSurfaceID &frame);

	int getProviderStringWidth(
			const GingaProviderID &provId, const char* text, int textLength=0);

	void playProviderOver(
			const GingaProviderID &provId, const GingaSurfaceID &surface);

	void playProviderOver(
			const GingaProviderID &provId, const GingaSurfaceID &surface,
			const char* text, int x, int y, short align);

	void playProviderOver(
						const GingaProviderID &provId, GingaSurfaceID surface);

	int getProviderHeight(const GingaProviderID &provId);

private:
	StubClient* _stubClient;
	pthread_mutex_t *_stubMutex;
	IInputManager*_im;
	IEventBuffer* _eb;

	string sendMessage (const string& message);

};

}
}
}
}
}
}

#endif //DISTRIBUTEDSCREENMANAGER_H
