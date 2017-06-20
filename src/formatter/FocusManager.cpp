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

#include "ginga.h"
#include "FocusManager.h"

#include "Converter.h"
#include "FormatterScheduler.h"

#include "mb/Display.h"
using namespace ::ginga::mb;

GINGA_PRAGMA_DIAG_IGNORE (-Wfloat-conversion)

GINGA_FORMATTER_BEGIN

bool FocusManager::_init = false;
set<FocusManager *> FocusManager::_instances;

FocusManager::FocusManager (FormatterScheduler *scheduler,
                            Settings *settings,
                            INclLinkActionListener *settingActions,
                            Converter *converter)
{
  string str;

  _focusTable = new map<string, set<ExecutionObject *> *>;
  _currentFocus = "";
  _objectToSelect = "";
  _isHandler = false;
  _parentManager = NULL;
  _width = 0;
  _height = 0;

  this->_defaultFocusBorderWidth = 1;
  this->_defaultFocusBorderColor = {0, 0, 255, 255};
  this->_defaultSelBorderColor = {0, 255, 0, 255};

  str = settings->get ("system.defaultFocusBorderWidth");
  if (str != "")
    this->_defaultFocusBorderWidth = xstrtoint (str, 10);

  str = settings->get ("system.focusBorderColor");
  _ginga_parse_color (str, &this->_defaultFocusBorderColor);

  str = settings->get ("system.defaultSelBorderColor");
  _ginga_parse_color (str, &this->_defaultSelBorderColor);

  this->_converter = converter;
  this->_scheduler = scheduler;
  this->_settings = settings;
  this->_selectedObject = NULL;
  this->_settingActions = settingActions;

  checkInit ();
  _instances.insert (this);
  g_assert (Ginga_Display->registerEventListener (this));
}

FocusManager::~FocusManager ()
{
  hasInstance (this, true);
  _selectedObject = NULL;

  if (_focusTable != NULL)
    {
      delete _focusTable;
      _focusTable = NULL;
    }

  _scheduler = NULL;
  _settings = NULL;
  Ginga_Display->unregisterEventListener (this);
}

void
FocusManager::checkInit ()
{
  if (!_init)
    _init = true;
}

bool
FocusManager::hasInstance (FocusManager *instance,
                           bool remove)
{
  set<FocusManager *>::iterator i;
  bool find = false;

  if (!_init)
    {
      return false;
    }

  i = _instances.find (instance);
  if (i != _instances.end ())
    {
      find = true;
      if (remove)
        {
          _instances.erase (i);
        }
    }

  return find;
}

bool
FocusManager::isKeyHandler ()
{
  return _isHandler;
}

bool
FocusManager::setKeyHandler (bool isHandler)
{
  ExecutionObject *focusedObj;
  NclCascadingDescriptor *dc;
  NclFormatterRegion *fr;
  string ix;

  if (this->_isHandler == isHandler)
    {
      return false;
    }

  if (isHandler && _parentManager != NULL && !_parentManager->isKeyHandler ())
    {
      clog << "FormatterFocusManager::setKeyHandler(" << this << ")";
      clog << " can't set handler because parent manager is not ";
      clog << "handling";
      clog << endl;
      return false;
    }

  focusedObj = getObjectFromFocusIndex (_currentFocus);

  this->_isHandler = isHandler;
  this->_converter->setHandlingStatus (isHandler);
  if (isHandler)
    {
      if (focusedObj == NULL)
        {
          if (!_focusTable->empty ())
            {
              ix = _focusTable->begin ()->first;
              _currentFocus = "";
              setFocus (ix);
            }
        }
      else
        {
          ix = _currentFocus;
          _currentFocus = "";
          setFocus (ix);
        }
    }
  else
    {
      if (focusedObj != NULL)
        {
          dc = focusedObj->getDescriptor ();
          if (dc != NULL)
            {
              fr = dc->getFormatterRegion ();
              if (fr != NULL)
                {
                  fr->setFocus (false);
                }
            }
          recoveryDefaultState (focusedObj);
        }

    }

  return isHandler;
}

ExecutionObject *
FocusManager::getObjectFromFocusIndex (const string &focusIndex)
{
  map<string, set<ExecutionObject *> *>::iterator i;
  set<ExecutionObject *>::iterator j;
  NclCascadingDescriptor *desc;
  bool visible;

  i = _focusTable->find (focusIndex);
  if (i == _focusTable->end ())
    {
      return NULL;
    }

  j = i->second->begin ();
  while (j != i->second->end ())
    {
      desc = (*j)->getDescriptor ();
      if (desc != NULL && desc->getFormatterRegion () != NULL)
        {
          visible = desc->getFormatterRegion ()->isVisible ();
          if (visible)
            {
              return *j;
            }
        }
      ++j;
    }

  delete i->second;
  _focusTable->erase (i);

  return NULL;
}

void
FocusManager::insertObject (ExecutionObject *obj,
                            const string &focusIndex)
{
  string auxIndex;
  map<string, set<ExecutionObject *> *>::iterator i;
  vector<string>::iterator j;
  set<ExecutionObject *> *objs;

  i = _focusTable->find (focusIndex);
  if (i != _focusTable->end ())
    {
      objs = i->second;
    }
  else
    {
      objs = new set<ExecutionObject *>;
      (*_focusTable)[focusIndex] = objs;
    }

  objs->insert (obj);
}

void
FocusManager::removeObject (ExecutionObject *obj,
                            const string &focusIndex)
{
  map<string, set<ExecutionObject *> *>::iterator i;
  set<ExecutionObject *>::iterator j;
  vector<string>::iterator k;
  set<ExecutionObject *> *objs;

  i = _focusTable->find (focusIndex);
  if (i != _focusTable->end ())
    {
      objs = i->second;
      j = objs->find (obj);
      if (j != objs->end ())
        {
          objs->erase (j);
        }
      if (objs->empty ())
        {
          delete objs;
          _focusTable->erase (i);
        }
    }
}

void
FocusManager::resetKeyMaster ()
{
  NclCascadingDescriptor *desc;

  if (_selectedObject != NULL)
    {
      _objectToSelect = "";
      _selectedObject->setHandler (false);
      desc = _selectedObject->getDescriptor ();
      if (desc != NULL)
        {
          desc->getFormatterRegion ()->setSelection (false);
        }

      recoveryDefaultState (_selectedObject);
    }
}

void
FocusManager::setKeyMaster (const string &mediaId)
{
  ExecutionObject *nextObject = NULL;
  NclCascadingDescriptor *nextDescriptor = NULL;
  NclFormatterRegion *fr = NULL;
  bool isFRVisible = false;
  bool abortKeyMaster = false;
  string lastFocus = "";

  if (mediaId == "" && _selectedObject != NULL)
    {
      resetKeyMaster ();
      return;
    }

  nextObject = _converter->getObjectFromNodeId (mediaId);

  if (nextObject == NULL)
    {
      clog << "FormatterFocusManager::setKeyMaster can't set '";
      clog << mediaId << "' as master: object is not available.";
      clog << endl;

      _objectToSelect = mediaId;
      return;
    }

  if (_selectedObject != NULL && _selectedObject != nextObject)
    {
      resetKeyMaster ();
    }

  nextDescriptor = nextObject->getDescriptor ();
  if (nextDescriptor != NULL)
    {
      fr = nextDescriptor->getFormatterRegion ();
    }

  if (fr != NULL && fr->getFocusIndex () != "")
    {
      lastFocus = _currentFocus;
      _currentFocus = fr->getFocusIndex ();
    }

  if (fr != NULL)
    {
      isFRVisible = fr->isVisible ();
      if (isFRVisible)
        {
          fr->setSelection (true);
        }
      else
        {
          abortKeyMaster = true;
        }
    }
  else
    {
      abortKeyMaster = true;
    }

  if (abortKeyMaster && fr != NULL && fr->getFocusIndex () != "")
    {
      _currentFocus = lastFocus;
      return;
    }

  // selecting new object
  _selectedObject = nextObject;
  _selectedObject->setHandler (true);
  nextObject->selectionEvent (0, 0);
}

void
FocusManager::setFocus (const string &focusIndex)
{
  ExecutionObject *nextObject = NULL;
  ExecutionObject *currentObject = NULL;
  NclCascadingDescriptor *currentDescriptor = NULL;
  NclCascadingDescriptor *nextDescriptor = NULL;

  if ((focusIndex == _currentFocus && _currentFocus != "") || !_isHandler)
    {
      if (!_isHandler)
        {
          clog << "FormatterFocusManager::setFocus(" << this << ")";
          clog << " can't set focus index because I'm not ";
          clog << "handling";
          clog << endl;
        }
      return;
    }

  nextObject = getObjectFromFocusIndex (focusIndex);
  if (nextObject == NULL)
    {
      return;
    }

  currentObject = getObjectFromFocusIndex (_currentFocus);
  if (currentObject != NULL)
    {
      currentDescriptor = currentObject->getDescriptor ();
    }
  else
    {
      currentDescriptor = NULL;
      clog << "FormatterFocusManager::setFocus index '";
      clog << focusIndex << "' is not in focus Table." << endl;
    }

  _currentFocus = focusIndex;
  _settings->set ("service.currentFocus", _currentFocus);

  if (currentDescriptor != NULL)
    {
      recoveryDefaultState (currentObject);
      currentDescriptor->getFormatterRegion ()->setFocus (false);
    }
  else if (currentObject != NULL)
    {
      clog << "FormatterFocusManager::setFocus Warning can't recover '";
      clog << currentObject->getId () << "'" << endl;
    }

  nextDescriptor = nextObject->getDescriptor ();
  if (nextDescriptor != NULL)
    {
      setFocus (nextDescriptor);
    }
}

void
FocusManager::setFocus (NclCascadingDescriptor *descriptor)
{
  SDL_Color focusColor;;
  SDL_Color selColor;;
  int borderWidth = 1;
  int width;
  NclFormatterRegion *fr = NULL;

  if (!_isHandler)
    {
      clog << "FormatterFocusManager::setFocus(" << this << ")";
      clog << " can't set focus because I'm not ";
      clog << "handling";
      clog << endl;
      return;
    }

  fr = descriptor->getFormatterRegion ();
  if (fr == NULL)
    {
      clog << "FormatterFocusManager::setFocus(" << this << ")";
      clog << " can't set focus because FR is NULL";
      clog << endl;
      return;
    }

  focusColor = fr->getFocusBorderColor ();
  borderWidth = fr->getFocusBorderWidth ();
  selColor = fr->getSelBorderColor ();

  width = borderWidth;

  if (fr != NULL)
    {
      fr->setFocusInfo (focusColor, width, descriptor->getFocusSrc (),
                        selColor, width, descriptor->getSelectionSrc ());

      fr->setFocus (true);
    }
}

void
FocusManager::recoveryDefaultState (ExecutionObject *object)
{
  if (object == NULL || object->getDescriptor () == NULL
      || object->getDescriptor ()->getFormatterRegion () == NULL)
    {
      return;
    }
}

void
FocusManager::showObject (ExecutionObject *object)
{
  NclCascadingDescriptor *descriptor;
  NclFormatterRegion *fr = NULL;
  string focusIndex;
  string paramValue, mediaId;
  Node *ncmNode;
  vector<string>::iterator i;

  if (object == NULL)
    {
      clog << "FormatterFocusManager::showObject Warning! object ";
      clog << "is null." << endl;
      return;
    }

  descriptor = object->getDescriptor ();
  if (descriptor == NULL)
    {
      clog << "FormatterFocusManager::showObject Warning! ";
      clog << " descriptor is null." << endl;
      return;
    }

  fr = descriptor->getFormatterRegion ();
  if (fr == NULL)
    {
      clog << "FormatterFocusManager::showObject Warning! ";
      clog << " FR is null." << endl;
      return;
    }

  focusIndex = fr->getFocusIndex ();

  ncmNode = object->getDataObject ();
  mediaId = ncmNode->getDataEntity ()->getId ();

  if (ncmNode->instanceOf ("ReferNode"))
    {
      if (((ReferNode *)ncmNode)->getInstanceType () == "new")
        {
          mediaId = ncmNode->getId ();
        }
    }

  clog << "FormatterFocusManager::showObject '" << mediaId << "'";
  clog << " with focus index = '" << focusIndex << "'";
  clog << " current focus = '" << _currentFocus << "'";
  clog << endl;

  if (focusIndex != "")
    {
      insertObject (object, focusIndex);
    }

  if (_currentFocus == "")
    {
      paramValue = _settings->get ("service.currentKeyMaster");
      if (paramValue == mediaId || _objectToSelect == mediaId)
        {
          _objectToSelect = "";
          setKeyMaster (mediaId);
        }
      else if (focusIndex != "")
        {
          setFocus (focusIndex);
        }
    }
  else
    {
      paramValue = _settings->get ("service.currentFocus");
      if (paramValue != "" && paramValue == focusIndex && fr->isVisible ())
        setFocus (focusIndex);

      paramValue = _settings->get ("service.currentKeyMaster");
      if ((paramValue == mediaId || _objectToSelect == mediaId)
          && fr->isVisible ())
        {
          _objectToSelect = "";

          if (focusIndex != "")
            {
              setFocus (focusIndex);
            }

          // then set as selected
          if (fr->setSelection (true))
            {
              // unselect the previous selected object, if exists
              if (_selectedObject != NULL)
                {
                  _selectedObject->setHandler (false);
                  _selectedObject->getDescriptor ()
                      ->getFormatterRegion ()
                      ->setSelection (false);

                  recoveryDefaultState (_selectedObject);
                }

              _selectedObject = object;
              _selectedObject->setHandler (true);
            }
        }
    }
}

void
FocusManager::hideObject (ExecutionObject *object)
{
  string focusIndex = "", ix;
  NclFormatterRegion *fr;
  map<string, set<ExecutionObject *> *>::iterator i;

  if (object == NULL || object->getDescriptor () == NULL)
    {
      return;
    }

  fr = object->getDescriptor ()->getFormatterRegion ();
  if (fr != NULL)
    {
      focusIndex = fr->getFocusIndex ();
    }

  if (focusIndex != "")
    {
      removeObject (object, focusIndex);

      if (fr != NULL && fr->getFocusState () == NclFormatterRegion::SELECTED
          && _selectedObject == object)
        {
          // region->setSelection(false);
          // recoveryDefaultState(selectedObject);
          _selectedObject = NULL;
        }

      if (_currentFocus == focusIndex)
        {
          // region->setFocus(false);
          // recoveryDefaultState(object);

          if (_focusTable->empty ())
            {
              _currentFocus = "";
            }
          else
            {
              ix = _focusTable->begin ()->first;
              setFocus (ix);
            }
        }
    }
}


void
FocusManager::setDefaultFocusBorderColor (SDL_Color color)
{
  this->_defaultFocusBorderColor = color;
}

void
FocusManager::setDefaultFocusBorderWidth (int width)
{
  this->_defaultFocusBorderWidth = width;
}

void
FocusManager::setDefaultSelBorderColor (SDL_Color color)
{
  this->_defaultSelBorderColor = color;
}

void
FocusManager::changeSettingState (const string &name, const string &act)
{
  set<ExecutionObject *> *settingObjects;
  set<ExecutionObject *>::iterator i;
  NclFormatterEvent *event;
  string keyM;

  settingObjects = _converter->getSettingNodeObjects ();

  i = settingObjects->begin ();
  while (i != settingObjects->end ())
    {
      event = (*i)->getEventFromAnchorId (name);
      if (event != NULL && event->instanceOf ("NclAttributionEvent"))
        {
          if (act == "start")
            {
              event->start ();
            }
          else if (act == "stop")
            {
              if (name == "service.currentFocus")
                {
                  ((NclAttributionEvent *)(event))->setValue (_currentFocus);
                }
              else if (name == "service.currentKeyMaster")
                {
                  if (_selectedObject != NULL)
                    {
                      keyM = (_selectedObject->getDataObject ()
                              ->getDataEntity ()
                              ->getId ());

                      ((NclAttributionEvent *)event)->setValue (keyM);
                    }
                }

              event->stop ();
            }
        }
      ++i;
    }

  delete settingObjects;
}

void
FocusManager::handleKeyEvent (SDL_EventType evtType,
                              SDL_Keycode key)
{
  ExecutionObject *currentObject;
  NclCascadingDescriptor *currentDescriptor;
  NclFormatterRegion *fr;
  string nextIndex;
  map<string, set<ExecutionObject *> *>::iterator i;

  if (key == SDLK_ESCAPE || evtType == SDL_KEYDOWN)
    return;

  if (!_isHandler)
    {
      return;
    }

  i = _focusTable->find (_currentFocus);
  if (i == _focusTable->end ())
    {
      if (_currentFocus != "")
        {
          clog << "FormatterFocusManager::keyEventReceived ";
          clog << "currentFocus not found which is '" << _currentFocus;
          clog << "'" << endl;
        }
      if (_selectedObject != NULL && key == SDLK_BACKSPACE)
        {
          return;
        }
      if (!_focusTable->empty ())
        {
          nextIndex = _focusTable->begin ()->first;
          setFocus (nextIndex);
        }
      return;
    }

  currentObject = getObjectFromFocusIndex (_currentFocus);
  if (currentObject == NULL)
    {
      return;
    }

  currentDescriptor = currentObject->getDescriptor ();
  if (currentDescriptor == NULL)
    {
      return;
    }

  fr = currentDescriptor->getFormatterRegion ();
  nextIndex = "";
  if (_selectedObject != NULL)
    {
      if (key == SDLK_BACKSPACE)
        {
          return;
        }
    }
  else if (key == SDLK_UP)
    {
      if (fr != NULL)
        {
          nextIndex = fr->getMoveUp ();
        }
    }
  else if (key == SDLK_DOWN)
    {
      if (fr != NULL)
        {
          nextIndex = fr->getMoveDown ();
        }
    }
  else if (key == SDLK_LEFT)
    {
      if (fr != NULL)
        {
          nextIndex = fr->getMoveLeft ();
        }
    }
  else if (key == SDLK_RIGHT)
    {
      if (fr != NULL)
        {
          nextIndex = fr->getMoveRight ();
        }
    }
  else if (key == SDLK_RETURN)
    {
      return;
    }

  if (nextIndex != "")
    {
      changeSettingState ("service.currentFocus", "start");
      setFocus (nextIndex);
      changeSettingState ("service.currentFocus", "stop");
    }
}


GINGA_FORMATTER_END
