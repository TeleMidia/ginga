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
using namespace ::ginga::mb;

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
using namespace ::ginga::player;

#include "PresentationContext.h"

GINGA_FORMATTER_BEGIN

class FormatterMultiDevice
{
protected:
  pthread_mutex_t mutex;
  DeviceLayout *deviceLayout;
  map<int, NclFormatterLayout *> layoutManager;
  vector<string> *activeUris;
  string activeBaseUri;
  NclFormatterLayout *mainLayout;
  int xOffset;
  int yOffset;
  int defaultWidth;
  int defaultHeight;
  int deviceClass;
  PresentationContext *presContext;
  void *focusManager;
  FormatterMultiDevice *parent;
  set<IPlayer *> listening;
  pthread_mutex_t lMutex;
  static const int DV_QVGA_WIDTH = 480;
  static const int DV_QVGA_HEIGHT = 320;

public:
  FormatterMultiDevice (DeviceLayout *deviceLayout, int w, int h);
  virtual ~FormatterMultiDevice ();
  void setParent (FormatterMultiDevice *parent);
  void setPresentationContex (PresentationContext *presContext);
  void setFocusManager (void *focusManager);
  void *getMainLayout ();
  void *getFormatterLayout (int devClass);

public:
  NclFormatterLayout *
  getFormatterLayout (NclCascadingDescriptor *descriptor,
                      NclExecutionObject *object);
  SDLWindow* prepareFormatterRegion (NclExecutionObject *object);
  void showObject (NclExecutionObject *executionObject);
  void hideObject (NclExecutionObject *executionObject);
};

GINGA_FORMATTER_END

#endif /* FORMATTER_MULTI_DEVICE_H */
