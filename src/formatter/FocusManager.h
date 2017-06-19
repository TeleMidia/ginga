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

#ifndef FORMATTER_FOCUS_MANAGER_H
#define FORMATTER_FOCUS_MANAGER_H

#include "ginga.h"

#include "NclCascadingDescriptor.h"
#include "ExecutionObject.h"

#include "NclSelectionEvent.h"

#include "INclLinkActionListener.h"
#include "NclLinkSimpleAction.h"

#include "NclFormatterRegion.h"

#include "PlayerAdapter.h"

GINGA_FORMATTER_BEGIN

class Converter;

class FocusManager : public IEventListener
{
public:
  FocusManager (FormatterScheduler *,
                Settings *,
                INclLinkActionListener *,
                Converter *);

  virtual ~FocusManager ();

  bool isKeyHandler ();
  bool setKeyHandler (bool _isHandler);

  void setKeyMaster (const string &mediaId);
  void setFocus (const string &focusIndex);

  void showObject (ExecutionObject *object);
  void hideObject (ExecutionObject *object);

  void setDefaultFocusBorderColor (SDL_Color);
  void setDefaultFocusBorderWidth (int);
  void setDefaultSelBorderColor (SDL_Color);

  void handleTickEvent (GingaTime, GingaTime, int) {}
  void handleKeyEvent (SDL_EventType evtType, SDL_Keycode key);

private:
  Settings *_settings;
  map<string, set<ExecutionObject *> *> *_focusTable;
  bool _isHandler;
  string _currentFocus;
  string _objectToSelect;
  ExecutionObject *_selectedObject;

  int _defaultFocusBorderWidth;  // pixels
  SDL_Color _defaultFocusBorderColor;
  SDL_Color _defaultSelBorderColor;

  FormatterScheduler *_scheduler;
  FocusManager *_parentManager;
  INclLinkActionListener *_settingActions;

  int _width;
  int _height;

  Converter *_converter;

  static bool _init;
  static set<FocusManager *> _instances;

  static void checkInit ();
  static bool hasInstance (FocusManager *instance, bool remove);

  void resetKeyMaster ();

  void setFocus (NclCascadingDescriptor *descriptor);
  void recoveryDefaultState (ExecutionObject *object);

  ExecutionObject *getObjectFromFocusIndex (const string &focusIndex);
  void insertObject (ExecutionObject *object, const string &focusIndex);
  void removeObject (ExecutionObject *object, const string &focusIndex);

  void changeSettingState (const string &name, const string &act);
};

GINGA_FORMATTER_END

#endif /* FORMATTER_FOCUS_MANAGER_H */
