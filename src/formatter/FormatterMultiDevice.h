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

#ifndef FORMATTER_MULTI_DEVICE_H
#define FORMATTER_MULTI_DEVICE_H

#include "ginga.h"

#include "mb/Display.h"
#include "mb/IInputEventListener.h"
using namespace ::ginga::mb;

#if defined WITH_MULTIDEVICE && WITH_MULTIDEVICE
#include "multidev/DeviceDomain.h"
#include "multidev/RemoteDeviceManager.h"
#include "multidev/IRemoteDeviceListener.h"
using namespace ::ginga::multidev;
#else
class DeviceDomain
{
public:
  // CT: class types
  static const int CT_BASE = 0;
  static const int CT_ACTIVE = 2;

  // FT: frame types
  static const int FT_CONNECTIONREQUEST = 0;
  static const int FT_ANSWERTOREQUEST = 1;
  static const int FT_KEEPALIVE = 2;
  static const int FT_MEDIACONTENT = 3;
  static const int FT_SELECTIONEVENT = 4;
  static const int FT_ATTRIBUTIONEVENT = 5;
  static const int FT_PRESENTATIONEVENT = 6;
};
#endif // WITH_MULTIDEVICE

#include "ncl/LayoutRegion.h"
using namespace ::ginga::ncl;

#include "ncl/Content.h"
#include "ncl/NodeEntity.h"
#include "ncl/ReferenceContent.h"
using namespace ::ginga::ncl;

#include "NclCascadingDescriptor.h"

#include "NclExecutionObject.h"

#include "NclFormatterLayout.h"

#include "ContextBase.h"

#include "ncl/DeviceLayout.h"
using namespace ::ginga::ncl;

#include "player/IPlayer.h"
#include "player/IPlayerListener.h"
using namespace ::ginga::player;

#include "PresentationContext.h"

GINGA_FORMATTER_BEGIN

#if defined WITH_MULTIDEVICE && WITH_MULTIDEVICE
class FormatterMultiDevice : public IPlayerListener,
                             public IInputEventListener,
                             public IRemoteDeviceListener
{
#else
class FormatterMultiDevice : public IPlayerListener,
                             public IInputEventListener
{
#endif
protected:
  InputManager *im;
  pthread_mutex_t mutex;

#if defined WITH_MULTIDEVICE && WITH_MULTIDEVICE
  static RemoteDeviceManager *rdm;
#else
  static void *rdm;
#endif

  DeviceLayout *deviceLayout;
  map<int, NclFormatterLayout *> layoutManager;
  vector<string> *activeUris;
  string activeBaseUri;
  NclFormatterLayout *mainLayout;
  SDLWindow* serialized;
  SDLWindow* printScreen;
  SDLWindow* bitMapScreen;
  int xOffset;
  int yOffset;
  int defaultWidth;
  int defaultHeight;
  int deviceClass;
  bool hasRemoteDevices;
  bool enableMulticast;
  PresentationContext *presContext;
  void *focusManager;
  FormatterMultiDevice *parent;
  set<IPlayer *> listening;
  pthread_mutex_t lMutex;
  static const int DV_QVGA_WIDTH = 480;
  static const int DV_QVGA_HEIGHT = 320;

public:
  FormatterMultiDevice (DeviceLayout *deviceLayout,
                        int x, int y, int w, int h, bool useMulticast,
                        int srvPort);
  virtual ~FormatterMultiDevice ();
  void printGingaWindows ();
  void listenPlayer (IPlayer *player);
  void stopListenPlayer (IPlayer *player);
  void setParent (FormatterMultiDevice *parent);
  void setPresentationContex (PresentationContext *presContext);
  void setFocusManager (void *focusManager);
  void *getMainLayout ();
  void *getFormatterLayout (int devClass);
  string getScreenShot ();
  string serializeScreen (int devClass, SDLWindow* mapWindow);

protected:
  virtual void postMediaContent (int destDevClass);

public:
  NclFormatterLayout *
  getFormatterLayout (NclCascadingDescriptor *descriptor,
                      NclExecutionObject *object);
  SDLWindow* prepareFormatterRegion (NclExecutionObject *object,
                                        SDLSurface* renderedSurface);
  void showObject (NclExecutionObject *executionObject);
  void hideObject (NclExecutionObject *executionObject);

protected:
  virtual bool newDeviceConnected (int newDevClass, int w, int h);
  virtual void connectedToBaseDevice (unsigned int domainAddr) = 0;
  virtual bool receiveRemoteEvent (int remoteDevClass, int eventType,
                                   const string &eventContent);
  virtual bool
  receiveRemoteContent (arg_unused (int remoteDevClass),
                        arg_unused (char *stream),
                        arg_unused (int streamSize))
  {
    return false;
  };
  virtual bool
  receiveRemoteContentInfo (arg_unused (const string &contentId),
                            arg_unused (const string &contentUri))
  {
    return false;
  };
  void renderFromUri (SDLWindow* win, const string &uri);
  void tapObject (int devClass, int x, int y);
  virtual bool
  receiveRemoteContent (arg_unused (int remoteDevClass),
                        arg_unused (const string &contentUri))
  {
    return false;
  };

public:
  void addActiveUris (const string &baseUri, vector<string> *uris);

protected:
  void updateStatus (short code,
                     const string &parameter,
                     short type,
                     const string &value);
  virtual bool userEventReceived (InputEvent *ev) = 0;
};

GINGA_FORMATTER_END

#endif /* FORMATTER_MULTI_DEVICE_H */
