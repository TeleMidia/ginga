/* Copyright (C) 2006-2017 PUC-Rio/Laboratorio TeleMidia

This file is part of Ginga (Ginga-NCL).

Ginga is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

Ginga is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
License for more details.

You should have received a copy of the GNU General Public License
along with Ginga.  If not, see <http://www.gnu.org/licenses/>.  */

#ifndef PRESENTATION_ENGINE_MANAGER_H
#define PRESENTATION_ENGINE_MANAGER_H

#include "system/ITimeBaseProvider.h"
using namespace ::ginga::system;

#include "player/INCLPlayer.h"
using namespace ::ginga::player;

#include "system/Thread.h"
using namespace ::ginga::system;

#include "mb/InputManager.h"
#include "mb/LocalScreenManager.h"
#include "mb/IInputEventListener.h"
using namespace ::ginga::mb;

#include "player/ShowButton.h"
#include "player/IPlayerListener.h"
using namespace ::ginga::player;

#include "gingancl/PrivateBaseManager.h"
using namespace ::br::pucrio::telemidia::ginga::ncl;

GINGA_LSSM_BEGIN

class PresentationEngineManager :  public IPlayerListener,
                                   public IInputEventListener,
                                   public ICmdEventListener,
                                   public Thread
{
private:
  static const short UC_BACKGROUND  = 0;
  static const short UC_PRINTSCREEN = 1;
  static const short UC_STOP        = 2;
  static const short UC_PAUSE       = 3;
  static const short UC_RESUME      = 4;
  static const short UC_SHIFT       = 5;
  int devClass;
  int x;
  int y;
  int w;
  int h;
  bool enableGfx;
  GingaScreenID myScreen;
  PrivateBaseManager* privateBaseManager;
  map<string, INCLPlayer*> formatters;
  set<INCLPlayer*> formattersToRelease;
  bool paused;
  string iconPath;
  bool isEmbedded;
  bool standAloneApp;
  bool isLocalNcl;
  void* dsmccListener;
  void* tuner;
  bool closed;
  bool hasTMPNotification;
  bool hasInteractivity;
  bool enableMulticast;
  bool exitOnEnd;
  bool disableFKeys;
  ITimeBaseProvider* timeBaseProvider;
  int currentPrivateBaseId;
  static bool autoProcess;
  vector<string> commands;
  static LocalScreenManager* dm;
  InputManager* im;
  ShowButton* sb;
  bool debugWindow;

public:
  PresentationEngineManager(int devClass, int xOffset, int yOffset,
                            int width, int height, bool disableGfx,
                            bool useMulticast, GingaScreenID screenId);
  virtual ~PresentationEngineManager();
  void setDebugWindow(bool debugWindow);
  void setExitOnEnd(bool exitOnEnd);
  void setDisableFKeys(bool disableFKeys);
  set<string>* createPortIdList(string nclFile);
  short getMappedInterfaceType(string nclFile, string portId);
  void autoMountOC(bool autoMountIt);
  void setCurrentPrivateBaseId(unsigned int baseId);
  void setTimeBaseProvider(ITimeBaseProvider* tmp);

private:
  void printGingaWindows();
  bool nclEdit(string nclEditApi);
  bool editingCommand(string commandTag, string commandPayload);

public:
  bool editingCommand(string editingCmd);
  void setBackgroundImage(string uri);

private:
  void close();
  void registerKeys();
  void setTimeBaseInfo(INCLPlayer* nclPlayer);

public:
  void getScreenShot();
  bool getIsLocalNcl();
  void setEmbedApp(bool isEmbedded);
  void setIsLocalNcl(bool isLocal, void* tuner=NULL);
  void setInteractivityInfo(bool hasInt);

private:
  INCLPlayer* createNclPlayer(string baseId, string fname);

public:
  NclPlayerData* createNclPlayerData();
  void addPlayerListener(string nclFile, IPlayerListener* listener);
  void removePlayerListener(string nclFile, IPlayerListener* listener);
  bool openNclFile(string nclFile);
  bool startPresentation(string nclFile, string interfId);
  bool pausePresentation(string nclFile);
  bool resumePresentation(string nclFile);
  bool stopPresentation(string nclFile);
  bool setPropertyValue(string nclFile, string interfaceId, string value);
  string getPropertyValue(string nclFile, string interfaceId);
  bool stopAllPresentations();
  bool abortPresentation(string nclFile);

private:
  void openNclDocument(string docUri, int x, int y, int w, int h);
  void pausePressed();

public:
  void* getDsmccListener();
  void setCmdFile(string cmdFile);
  static void* processAutoCmd(void* ptr);
  void waitUnlockCondition();

private:
  void presentationCompleted(string formatterId);
  void releaseFormatter(string formatterId);
  bool checkStatus();
  void updateStatus(short code, string parameter, short type, string value);
  bool userEventReceived(SDLInputEvent* ev);
  bool cmdEventReceived(string command, string args);
  static void* eventReceived(void* ptr);
  void readCommand(string command);
  bool getNclPlayer(string docLocation, INCLPlayer** player);
  bool getNclPlayer(string baseId, string docId, INCLPlayer** p);
  void updateFormatters(short command, string parameter="");
  void run();
};

GINGA_LSSM_END

#endif /* PRESENTATION_ENGINE_MANAGER_H */
