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
#include "ginga-color-table.h"
#include "AdapterFormatterPlayer.h"

#include "AdapterApplicationPlayer.h"
#include "AdapterPlayerManager.h"
#include "NclLinkTransitionTriggerCondition.h"

#include "mb/Display.h"
using namespace ::ginga::mb;

#include "player/Player.h"
using namespace ::ginga::player;

GINGA_FORMATTER_BEGIN

double AdapterFormatterPlayer::_eventTS = 0;

AdapterFormatterPlayer::AdapterFormatterPlayer ()
{
  this->_manager = nullptr;
  this->_object = nullptr;
  this->_player = nullptr;
  this->_mrl = "";
  this->_objectDevice = -1;
  this->_outTransDur = 0;
  this->_outTransTime = -1.0;
  this->_isLocked = false;
  Thread::mutexInit (&_objectMutex, false);
}

AdapterFormatterPlayer::~AdapterFormatterPlayer ()
{
  int objDevice;

  Ginga_Display->unregisterKeyEventListener(this);

  lockObject ();

  objDevice = getObjectDevice ();

  if (_object != nullptr)
    {
      _object = nullptr;
    }

  if (_player != nullptr)
    {
      _player->removeListener (this);
      _player->stop ();
      delete _player;
      _player = nullptr;
    }

  unlockObject ();

  Thread::mutexDestroy (&_objectMutex);
}

void
AdapterFormatterPlayer::setAdapterManager (AdapterPlayerManager *manager)
{
  this->_manager = manager;
  Ginga_Display->registerKeyEventListener(this);
}

void
AdapterFormatterPlayer::setOutputWindow (SDLWindow* windowId)
{
  if (_player != nullptr)
    {
      _player->setOutWindow (windowId);
    }
  else
    {
      clog << "AdapterFormatterPlayer::setOutputWindow Warning!";
      clog << " Player is nullptr" << endl;
    }
}

void
AdapterFormatterPlayer::createPlayer ()
{
  vector<Anchor *> *anchors;

  vector<NclFormatterEvent *> *events;

  NclCascadingDescriptor *descriptor;

  NodeEntity *dataObject;
  PropertyAnchor *property;

  if (_player == nullptr)
    {
      _player = new Player (_mrl);
    }

  _player->addListener (this);

  g_assert_nonnull (_object);

  descriptor = _object->getDescriptor ();
  if (descriptor)
    {
      for (Parameter &param: descriptor->getParameters ())
        {
          _player->setPropertyValue (param.getName (), param.getValue ());
        }
    }

  dataObject = dynamic_cast <NodeEntity *> (_object->getDataObject ());
  g_assert_nonnull (dataObject);
  if (dataObject->instanceOf ("ContentNode"))
    {
      anchors = ((ContentNode *)dataObject)->getAnchors ();
      if (anchors != nullptr)
        {
          for (Anchor *anchor: *anchors)
            {
              property = dynamic_cast <PropertyAnchor *> (anchor);
              if (property)
                {
                  clog << "AdapterFormatterPlayer::createPlayer for '";
                  clog << _mrl;
                  clog << "' set property '";
                  clog << property->getPropertyName ();
                  clog << "' value '" << property->getPropertyValue ();
                  clog << "'" << endl;

                  _player->setPropertyValue (property->getPropertyName (),
                                            property->getPropertyValue ());
                }
            }
        }
    }

  events = _object->getEvents ();
  if (events != nullptr)
    {
      for (NclFormatterEvent *evt: *events)
        {
          if (evt != nullptr && evt->instanceOf ("NclAttributionEvent"))
            {
              property = ((NclAttributionEvent *)evt)->getAnchor ();
              ((NclAttributionEvent *)evt)->setValueMaintainer (this);
            }
        }
      delete events;
      events = nullptr;
    }

  _objectDevice = getObjectDevice ();

  clog << "AdapterFormatterPlayer::createPlayer for '" << _mrl;
  clog << "' object = '" << _object->getId () << "'";
  clog << " objectDevice = '" << _objectDevice << "'" << endl;
}

int
AdapterFormatterPlayer::getObjectDevice ()
{
  NclCascadingDescriptor *descriptor;
  LayoutRegion *ncmRegion = nullptr;

  if (_objectDevice > -1)
    {
      return _objectDevice;
    }

  if (_object != nullptr)
    {
      descriptor = _object->getDescriptor ();
      if (descriptor != nullptr)
        {
          ncmRegion = descriptor->getRegion ();
          if (ncmRegion != nullptr)
            {
              _objectDevice = ncmRegion->getDeviceClass ();
              return _objectDevice;
            }
        }
    }

  return 0;
}

bool
AdapterFormatterPlayer::hasPrepared ()
{
  bool presented;
  NclFormatterEvent *mEv;
  short st;

  if (_object == nullptr || _player == nullptr)
    {
      clog << "AdapterFormatterPlayer::hasPrepared return false because";
      clog << " object = '" << _object << "' and player = '";
      clog << _player << "'" << endl;
      return false;
    }

  presented = _player->isForcedNaturalEnd ();
  if (presented)
    {
      clog << "AdapterFormatterPlayer::hasPrepared return false because";
      clog << " a natural end was forced" << endl;
      return false;
    }

  mEv = _object->getMainEvent ();
  if (mEv != nullptr && !_object->instanceOf ("NclApplicationExecutionObject"))
    {
      st = mEv->getCurrentState ();
      if (st != EventUtil::ST_SLEEPING)
        {
          return true;
        }
    }

  return _isLocked;
}


double
AdapterFormatterPlayer::prepareProperties (NclExecutionObject *obj)
{
  NclCascadingDescriptor *descriptor;
  LayoutRegion *region = nullptr;
  vector<string> params;
  vector<PropertyAnchor *> *anchors;
  string name, value;
  NclFormatterRegion *fRegion = nullptr;
  Node *ncmNode;
  double transpValue = -1;
  double parentOpacity = -1;
  bool isPercent = false;
  double explicitDur = -1;

  string left = "", top = "", width = "", height = "", bottom = "", right = "";
  string plan = "";

  descriptor = obj->getDescriptor ();
  if (descriptor != nullptr)
    {
      fRegion = descriptor->getFormatterRegion ();
      if (fRegion != nullptr)
        {
          region = fRegion->getLayoutRegion ();
        }
      else
        {
          region = nullptr;
        }
    }

  if (region == nullptr)
    {
      PropertyAnchor *property = obj->getNCMProperty ("explicitDur");
      if (property != nullptr)
        {
          value = property->getPropertyValue ();
          explicitDur = xstrtimetod (value) * 1000;
        }

      return explicitDur;
    }

  map <string, string> properties;
  // get the properties from the descriptor
  for (Parameter &param : descriptor->getParameters())
    {
      properties[param.getName()] = param.getValue();
    }

  //get the properties from the object
  ncmNode = obj->getDataObject ();
  anchors = ((Node *)ncmNode)->getOriginalPropertyAnchors ();
  g_assert_nonnull (anchors);
  for (PropertyAnchor *property : *anchors)
    {
      properties[property->getPropertyName()] = property->getPropertyValue();
    }

  for (auto it: properties)
    {
      name = it.first;
      value = it.second;

      g_debug ( "Prepare property: name=%s, value=%s.",
                name.c_str(), value.c_str());

      if (value != "")
        {
          if (name == "explicitDur")
            {
              explicitDur = xstrtimetod (value) * 1000;
            }
          else if (name == "left")
            {
              left = value;
            }
          else if (name == "top")
            {
              top = value;
            }
          else if (name == "width")
            {
              width = value;
            }
          else if (name == "height")
            {
              height = value;
            }
          else if (name == "bottom")
            {
              bottom = value;
            }
          else if (name == "right")
            {
              right = value;
            }
          else if (name == "zIndex")
            {
              region->setZIndex (xstrtoint (value, 10));
            }
          else if (name == "bounds")
            {
              params = xstrsplit (xstrchomp (value), ',');
              if (params.size () == 4)
                {
                  left = xstrchomp (params[0]);
                  top = xstrchomp (params[1]);
                  width = xstrchomp (params[2]);
                  height = xstrchomp (params[3]);
                }
            }
          else if (name == "location")
            {
              params = xstrsplit (xstrchomp (value), ',');
              if (params.size () == 2)
                {
                  left = xstrchomp (params[0]);
                  top = xstrchomp (params[1]);
                }
            }
          else if (name == "size")
            {
              params = xstrsplit (xstrchomp (value), ',');
              if (params.size () == 2)
                {
                  width = xstrchomp (params[0]);
                  height = xstrchomp (params[1]);
                }
            }
          else if (name == "transparency")
            {
              transpValue = xstrtodorpercent (value, &isPercent);
              parentOpacity = (1
                               - _manager
                                     ->getNclPlayerData ()
                                     ->transparency);

              transpValue = (1 - (parentOpacity
                                  - (parentOpacity * transpValue)));

              if (fRegion != nullptr)
                {
                  fRegion->setTransparency (transpValue);
                }
            }
          else if (name == "background")
            {
              if (fRegion != nullptr)
                {
                  SDL_Color *bg = new SDL_Color();
                  ginga_color_input_to_sdl_color(value,bg);
                  fRegion->setBackgroundColor (bg);
                }
            }
          else if (name == "focusIndex")
            {
              if (fRegion != nullptr)
                {
                  fRegion->setFocusIndex (value);
                }
            }
          else if (name == "focusBorderColor")
            {
              if (fRegion != nullptr)
                {
                  SDL_Color *c = new SDL_Color();
                  ginga_color_input_to_sdl_color(value,c);
                  fRegion->setFocusBorderColor(c);
                }
            }
          else if (name == "focusBorderWidth")
            {
              if (fRegion != nullptr)
                {
                  fRegion->setFocusBorderWidth (xstrtoint (value, 10));
                }
            }
          else if (name == "focusComponentSrc")
            {
              if (fRegion != nullptr)
                {
                  fRegion->setFocusComponentSrc (value);
                }
            }
          else if (name == "selBorderColor")
            {
              if (fRegion != nullptr)
                {
                  SDL_Color *c = new SDL_Color();
                  ginga_color_input_to_sdl_color(value,c);
                  fRegion->setSelBorderColor (c);
                }
            }
          else if (name == "selBorderWidth")
            {
              if (fRegion != nullptr)
                {
                  fRegion->setSelBorderWidth (xstrtoint (value, 10));
                }
            }
          else if (name == "selComponentSrc")
            {
              if (fRegion != nullptr)
                {
                  fRegion->setSelComponentSrc (value);
                }
            }
          else if (name == "moveUp")
            {
              if (fRegion != nullptr)
                {
                  fRegion->setMoveUp (value);
                }
            }
          else if (name == "moveDown")
            {
              if (fRegion != nullptr)
                {
                  fRegion->setMoveDown (value);
                }
            }
          else if (name == "moveLeft")
            {
              if (fRegion != nullptr)
                {
                  fRegion->setMoveLeft (value);
                }
            }
          else if (name == "moveRight")
            {
              if (fRegion != nullptr)
                {
                  fRegion->setMoveRight (value);
                }
            }
          else if (name == "plan")
            {
              plan = value;
            }
        }
    }

  if (left != "")
    {
      region->resetLeft ();
    }

  if (top != "")
    {
      region->resetTop ();
    }

  if (width != "")
    {
      region->resetWidth ();
    }

  if (height != "")
    {
      region->resetHeight ();
    }

  if (bottom != "")
    {
      if (top != "")
        {
          region->resetHeight ();
        }
      region->resetBottom ();
    }

  if (right != "")
    {
      if (left != "")
        {
          region->resetWidth ();
        }
      region->resetRight ();
    }

  if (left != "")
    {
      region->setLeft (xstrtodorpercent (value, &isPercent), isPercent);
    }

  if (top != "")
    {
      region->setTop (xstrtodorpercent (value, &isPercent), isPercent);
    }

  if (width != "")
    {
      region->setWidth (xstrtodorpercent (value, &isPercent), isPercent);
    }

  if (height != "")
    {
      region->setHeight (xstrtodorpercent (value, &isPercent), isPercent);
    }

  if (bottom != "")
    {
      region->setBottom (xstrtodorpercent (value, &isPercent), isPercent);
    }

  if (right != "")
    {
      region->setRight (xstrtodorpercent (value, &isPercent),
                        isPercent);
    }

  if ((plan == "") && (_mrl.find ("sbtvd-ts://") != std::string::npos))
    {
      plan = "video";
    }

  if (plan == "")
    {
      plan = "graphic";
    }

  if (fRegion != nullptr)
    {
      fRegion->setPlan (plan);
    }

  if (transpValue < 0.
      && descriptor->getParameterValue ("transparency") == "")
    {
      transpValue = _manager->getNclPlayerData ()->transparency;

      if (fRegion != nullptr)
        {
          fRegion->setTransparency (transpValue);
        }
    }

  return explicitDur;
}

void
AdapterFormatterPlayer::updatePlayerProperties (
    arg_unused (NclExecutionObject *obj))
{
  NclCascadingDescriptor *descriptor;
  string value;

  if (_object != nullptr)
    {
      descriptor = _object->getDescriptor ();
      if (descriptor != nullptr)
        {
          value = descriptor->getParameterValue ("soundLevel");
          if (value == "")
            {
              value = "1.0";
            }

          if (_player != nullptr)
            {
              _player->setPropertyValue ("soundLevel", value);
            }
        }
    }
}

bool
AdapterFormatterPlayer::prepare (NclExecutionObject *object,
                                 NclFormatterEvent *event)
{
  Content *content;
  NclCascadingDescriptor *descriptor;
  double explicitDur = -1;
  NodeEntity *dataObject;

  if (hasPrepared ())
    {
      clog << "AdapterFormatterPlayer::prepare returns false, ";
      clog << "because player is already prepared" << endl;
      return false;
    }

  if (!lockObject ())
    {
      if (_player != nullptr)
        {
          // if (player->hasPresented () ||

          if (_player->isForcedNaturalEnd ())
            {
              while (!lockObject ())
                ;
            }
          else
            {
              clog << "AdapterFormatterPlayer::prepare returns false, ";
              clog << "because object is locked" << endl;
              return false;
            }
        }
    }

  if (object == nullptr)
    {
      clog << "AdapterFormatterPlayer::prepare Warning! ";
      clog << "Trying to prepare a nullptr object." << endl;
      unlockObject ();
      return false;
    }

  this->_object = object;
  descriptor = object->getDescriptor ();
  dataObject = dynamic_cast<NodeEntity *>(object->getDataObject ());

  if (dataObject
      &&  dataObject->getDataEntity () != nullptr)
    {
      content = dynamic_cast<NodeEntity *> (dataObject->getDataEntity ())->getContent();

      if (content
          && content->instanceOf ("ReferenceContent"))
        {
          this->_mrl
              = ((ReferenceContent *)content)->getCompleteReferenceUrl ();

          // this->mrl = SystemCompat::updatePath (this->mrl);
        }
      else
        {
          this->_mrl = "";
        }
    }

  explicitDur = prepareProperties (object);

  if (event->instanceOf ("NclPresentationEvent"))
    {
      double duration = ((NclPresentationEvent *)event)->getDuration ();
      bool infDur = (isnan (duration) || isinf (duration));

      if (descriptor != nullptr && explicitDur < 0)
        {
          explicitDur = descriptor->getExplicitDuration ();
        }

      if (!infDur && duration <= 0 && explicitDur <= 0)
        {
          clog << "AdapterFormatterPlayer::prepare '";
          clog << object->getId () << "' Warning! Can't prepare an ";
          clog << "object with an event duration <= 0" << endl;
          return false;
        }

      // explicit duration overwrites implicit duration
      if (!isnan (explicitDur) && explicitDur > 0)
        {
          object->removeEvent (event);

          // The explicit duration is a property of
          // the object. For instance: start an interface with
          // begin = 4s and explicit duration = 5s => new duration
          // will be 1s
          ((NclPresentationEvent *)event)->setEnd (explicitDur);
          object->addEvent (event);

          clog << "AdapterFormatterPlayer::prepare '";
          clog << object->getId () << "' ";
          clog << "with explicitDur = '";
          clog << explicitDur << "' object duration was '";
          clog << duration << "'. Updated info: event begin = '";
          clog << ((NclPresentationEvent *)event)->getBegin () << "'";
          clog << " event end = '";
          clog << ((NclPresentationEvent *)event)->getEnd () << "'";
          clog << endl;
        }
    }

  createPlayer ();
  updatePlayerProperties (object);
  if (event->getCurrentState () == EventUtil::ST_SLEEPING)
    {
      object->prepare ((NclPresentationEvent *)event, 0);
      prepare ();
      return true;
    }
  else
    {
      return false;
    }
}

void
AdapterFormatterPlayer::prepare ()
{
  NclCascadingDescriptor *descriptor;
  LayoutRegion *region;

  g_assert_nonnull (_object);
  g_assert_nonnull (_player);
  if (_object == nullptr || _player == nullptr)
    {
      return;
    }

  descriptor = _object->getDescriptor ();
  if (descriptor != nullptr)
    {
      region = descriptor->getRegion ();
      if (region != nullptr)
        {
          _player->setNotifyContentUpdate (region->getDeviceClass () == 1);
        }
    }

  prepareScope ();

  if (_player->immediatelyStart ())
    {
      _player->setImmediatelyStart (false);
      start ();
    }
}

void
AdapterFormatterPlayer::prepareScope (double offset)
{
  NclPresentationEvent *mainEvent;
  double duration;
  double playerDur;
  double initTime = 0;
  IntervalAnchor *intervalAnchor;

  mainEvent = (NclPresentationEvent *)(_object->getMainEvent ());
  if (mainEvent->instanceOf ("NclPresentationEvent"))
    {
      if ((mainEvent->getAnchor ())->instanceOf ("LambdaAnchor"))
        {
          duration = mainEvent->getDuration ();

          if (offset > 0)
            {
              initTime = offset;
            }

          _outTransDur = getOutTransDur ();
          if (_outTransDur > 0.0)
            {
              playerDur = _player->getTotalMediaTime ();
              if (isinf (duration) && playerDur > 0.0)
                {
                  duration = playerDur * 1000;
                }

              _outTransTime = duration - _outTransDur;
              if (_outTransTime <= 0.0)
                {
                  _outTransTime = 0.1;
                }
            }

          if (duration < IntervalAnchor::OBJECT_DURATION)
            {
              _player->setScope (mainEvent->getAnchor ()->getId (),
                                IPlayer::TYPE_PRESENTATION, initTime,
                                duration / 1000, _outTransTime);
            }
          else
            {
              _outTransDur = -1.0;
              _player->setScope (mainEvent->getAnchor ()->getId (),
                                IPlayer::TYPE_PRESENTATION, initTime);
            }
        }
      else if (((mainEvent->getAnchor ()))->instanceOf ("IntervalAnchor"))
        {
          intervalAnchor = (IntervalAnchor *)(mainEvent->getAnchor ());
          initTime = (intervalAnchor->getBegin () / 1000);
          if (offset > 0)
            {
              initTime = offset;
            }

          duration = intervalAnchor->getEnd ();

          _outTransDur = getOutTransDur ();
          if (_outTransDur > 0.0)
            {
              playerDur = _player->getTotalMediaTime ();
              if (isinf (duration) && playerDur > 0.0)
                {
                  duration = playerDur * 1000;
                }

              _outTransTime = duration - _outTransDur;
              if (_outTransTime <= 0.0)
                {
                  _outTransTime = 0.1;
                }
            }

          if (duration < IntervalAnchor::OBJECT_DURATION)
            {
              _player->setScope (mainEvent->getAnchor ()->getId (),
                                IPlayer::TYPE_PRESENTATION, initTime,
                                (intervalAnchor->getEnd () / 1000),
                                _outTransTime);
            }
          else
            {
              _outTransDur = -1.0;
              _player->setScope (mainEvent->getAnchor ()->getId (),
                                IPlayer::TYPE_PRESENTATION, initTime);
            }
        }
    }

  if (offset > 0)
    {
      _player->setMediaTime ( (guint32)offset);
    }
}

double
AdapterFormatterPlayer::getOutTransDur ()
{
  NclCascadingDescriptor *descriptor;
  NclFormatterRegion *fRegion;
  double outTransDur = 0.0;

  descriptor = _object->getDescriptor ();
  if (descriptor != nullptr)
    {
      fRegion = descriptor->getFormatterRegion ();
      if (fRegion != nullptr)
        {
          outTransDur = fRegion->getOutTransDur ();
        }
    }

  return outTransDur;
}

void
AdapterFormatterPlayer::checkAnchorMonitor ()
{
   if (_object == nullptr || this->_player == nullptr)
      return;

   _object->setPlayer( (Player*)this->_player );
}

bool
AdapterFormatterPlayer::start ()
{
  NclCascadingDescriptor *descriptor;
  LayoutRegion *ncmRegion = nullptr;
  string paramValue;
  NclFormatterEvent *objEv;

  assert (_object != nullptr);

  if (!_object->isSleeping ())
    {
      clog << "AdapterFormatterPlayer::start(" << _object->getId ();
      clog << ") is occurring or paused" << endl;
      return false;
    }

  descriptor = _object->getDescriptor ();
  if (descriptor != nullptr)
    {
      paramValue = descriptor->getParameterValue ("visible");
      if (paramValue == "false")
        {
          setVisible (false);
        }
      else if (paramValue == "true")
        {
          setVisible (true);
        }

      ncmRegion = descriptor->getRegion ();
      if (ncmRegion != nullptr && ncmRegion->getDeviceClass () == 2)
        {
          objEv = _object->getMainEvent ();
          if (objEv != nullptr)
            {
              clog << "AdapterFormatterPlayer::start(";
              clog << _object->getId ();
              clog << ") ACTIVE CLASS" << endl;
              objEv->start ();
            }
          return true;
        }
    }

  if (_object != nullptr)
    {
      bool startSuccess = false;

      if (_player != nullptr)
        {
          startSuccess = _player->play ();
        }

      if (startSuccess)
        {
          if (!_object->start ())
            {
              if (_player != nullptr)
                {
                  _player->stop ();
                }
              startSuccess = false;
            }
          else
            {
              checkAnchorMonitor ();
            }
        }

      return startSuccess;
    }
  return false;
}

bool
AdapterFormatterPlayer::stop ()
{
  NclFormatterEvent *mainEvent = nullptr;
  vector<NclFormatterEvent *> *events = nullptr;

  if (_player == nullptr && _object == nullptr)
    {
      unlockObject ();
      return false;
    }
  else if (_object != nullptr)
    {
      mainEvent = _object->getMainEvent ();
      events = _object->getEvents ();
    }

  if (mainEvent != nullptr && mainEvent->instanceOf ("NclPresentationEvent"))
    {
      if (checkRepeat ((NclPresentationEvent *)mainEvent))
        {
          return true;
        }
    }

  if (events != nullptr)
    {
      vector<NclFormatterEvent *>::iterator i;
      i = events->begin ();
      while (i != events->end ())
        {
          if (*i != nullptr && (*i)->instanceOf ("NclAttributionEvent"))
            {
              ((NclAttributionEvent *)(*i))->setValueMaintainer (nullptr);
            }

          if (i != events->end ())
            {
              ++i;
            }
        }
      delete events;
      events = nullptr;
    }

  if (_player != nullptr)
    {
      _player->stop ();
      _player->notifyReferPlayers (EventUtil::TR_STOPS);
    }

  if (_player != nullptr && _player->isForcedNaturalEnd ())
    {
      _player->forceNaturalEnd (false);
      _object->stop ();
      return unprepare ();
    }
  else if (_object != nullptr)
    {
      _object->stop ();
      unprepare ();
      return true;
    }

  return false;
}

bool
AdapterFormatterPlayer::pause ()
{
  if (_object != nullptr && _player != nullptr && _object->pause ())
    {
      _player->pause ();

      _player->notifyReferPlayers (EventUtil::TR_PAUSES);
      return true;
    }
  else
    {
      return false;
    }
}

bool
AdapterFormatterPlayer::resume ()
{
  if (_object != nullptr && _player != nullptr && _object->resume ())
    {
      _player->resume ();
      _player->notifyReferPlayers (EventUtil::TR_RESUMES);
      return true;
    }
  return false;
}

bool
AdapterFormatterPlayer::abort ()
{
  if (_player != nullptr)
    {
      _player->stop ();
      _player->notifyReferPlayers (EventUtil::TR_ABORTS);
    }

  if (_object != nullptr)
    {
      if (!_object->isSleeping ())
        {
          _object->abort ();
          unprepare ();
          return true;
        }
    }

  unlockObject ();
  return false;
}

void
AdapterFormatterPlayer::naturalEnd ()
{
  bool freeze;

  if (_object == nullptr || _player == nullptr)
    {
      unlockObject ();
      return;
    }

  // if freeze is true the natural end is not performed
  if (_object->getDescriptor () != nullptr)
    {
      freeze = _object->getDescriptor ()->getFreeze ();
      if (freeze)
        {
          return;
        }
    }

  stop ();
}

bool
AdapterFormatterPlayer::checkRepeat (NclPresentationEvent *event)
{
  if (event->getRepetitions () > 1)
    {
      _player->stop ();
      _player->notifyReferPlayers (EventUtil::TR_STOPS);

      if (_object != nullptr)
        {
          // clog << "'" << object->getId() << "'";
          _object->stop ();
        }
      // clog << endl << endl;

      _player->setImmediatelyStart (true);
      prepare ();
      return true;
    }

  return false;
}

bool
AdapterFormatterPlayer::unprepare ()
{
  if (_object != nullptr && _object->getMainEvent () != nullptr
      && (_object->getMainEvent ()->getCurrentState ()
              == EventUtil::ST_OCCURRING
          || _object->getMainEvent ()->getCurrentState ()
                 == EventUtil::ST_PAUSED))
    {
      return stop ();
    }

  _manager->removePlayer (_object);

  if (NclExecutionObject::hasInstance (_object, false))
    {
      _object->unprepare ();
    }

  _object = nullptr;
  unlockObject ();

  return true;
}

bool
AdapterFormatterPlayer::setPropertyValue (NclAttributionEvent *event,
                                          const string &v)
{
  string propName;
  string value = v;

  if (_player == nullptr || _object == nullptr)
    {
      clog << "AdapterFormatterPlayer::setPropertyValue Warning!";
      clog << " cant set property '" << event->getId ();
      clog << "' value = '" << value << "' object = '";

      if (_object != nullptr)
        {
          clog << _object->getId ();
        }
      else
        {
          clog << _object;
        }
      clog << "' player = '";
      clog << _player << "' for '" << _mrl << "'" << endl;

      return false;
    }

  propName = (event->getAnchor ())->getPropertyName ();
  if (propName == "visible")
    {
      setVisible (value == "true");
    }
  else
    {
      AdapterApplicationPlayer *adapterAppPlayer =
          dynamic_cast <AdapterApplicationPlayer *> (this);
      if(adapterAppPlayer)
        {
          if (!adapterAppPlayer->setAndLockCurrentEvent (event))
            {
              return false;
            }
        }

      if (_object->setPropertyValue (event, value))
        {
          _player->setPropertyValue (propName,
                                    _object->getPropertyValue (propName));
        }
      else
        {
          if (propName == "transparency")
            {
              bool isPercent;
              double transpValue, parentOpacity;
              NclFormatterRegion *fRegion;
              NclCascadingDescriptor *descriptor;

              transpValue = xstrtodorpercent (value, &isPercent);
              parentOpacity = (1
                               - _manager
                                     ->getNclPlayerData ()
                                     ->transparency);

              transpValue
                  = (1 - (parentOpacity - (parentOpacity * transpValue)));

              clog << "AdapterFormatterPlayer::setPropertyValue ";
              clog << "parent opacity is '" << parentOpacity;
              clog << "' original transparency property is '";
              clog << value << "' new value is '";
              clog << transpValue << "'" << endl;

              descriptor = _object->getDescriptor ();
              if (descriptor != nullptr)
                {
                  fRegion = descriptor->getFormatterRegion ();
                  if (fRegion != nullptr)
                    {
                      fRegion->setTransparency (transpValue);
                    }
                }
            }
          _player->setPropertyValue (propName, value);
        }

      if (adapterAppPlayer)
        {
          adapterAppPlayer->unlockCurrentEvent (event);
        }
    }

  return true;
}

void
AdapterFormatterPlayer::setPropertyValue (const string &name, const string &value)
{
  if (_player != nullptr)
    {
      clog << "AdapterFormatterPlayer::setPropertyValue name = '";
      clog << name << "' value = '" << value << "' to player ";
      clog << " address '" << _player << "'";
      clog << endl;

      _player->setPropertyValue (name, value);
    }
}

string
AdapterFormatterPlayer::getPropertyValue (NclAttributionEvent *event)
{
  string value = "";
  string name;

  if (event == nullptr)
    {
      return "";
    }

  name = event->getAnchor ()->getPropertyName ();
  value = getPropertyValue (name);

  return value;
}

string
AdapterFormatterPlayer::getPropertyValue (const string &name)
{
  string value = "";

  if (_player != nullptr)
    {
      value = _player->getPropertyValue (name);
    }

  if (value == "" && _object != nullptr)
    {
      value = _object->getPropertyValue (name);
    }

  clog << "AdapterFormatterPlayer::getPropertyValue name = '";
  clog << name << "' value = '" << value << "'";
  clog << endl;

  return value;
}

double
AdapterFormatterPlayer::getObjectExpectedDuration ()
{
  return IntervalAnchor::OBJECT_DURATION;
}

void
AdapterFormatterPlayer::updateObjectExpectedDuration ()
{
  NclPresentationEvent *wholeContentEvent;
  double duration;
  double implicitDur;

  wholeContentEvent = _object->getWholeContentPresentationEvent ();
  duration = wholeContentEvent->getDuration ();
  if ((_object->getDescriptor () == nullptr)
      || (isnan ((_object->getDescriptor ())->getExplicitDuration ()))
      || (duration < 0) || (isnan (duration)))
    {
      implicitDur = getObjectExpectedDuration ();
      ((IntervalAnchor *)wholeContentEvent->getAnchor ())
          ->setEnd (implicitDur);

      wholeContentEvent->setDuration (implicitDur);
    }
}

double
AdapterFormatterPlayer::getMediaTime ()
{
  if (_player != nullptr)
    {
      return _player->getMediaTime ();
    }
  return 0;
}

IPlayer *
AdapterFormatterPlayer::getPlayer ()
{
  return _player;
}

void
AdapterFormatterPlayer::setTimeBasePlayer (
    AdapterFormatterPlayer *timeBasePlayerAdapter)
{
  IPlayer *timePlayer;
  timePlayer = timeBasePlayerAdapter->getPlayer ();
  if (timePlayer != nullptr)
    {
      _player->setReferenceTimePlayer (timePlayer);
      _player->setTimeBasePlayer (timePlayer);
    }
}

void
AdapterFormatterPlayer::updateStatus (short code,
                                      const string &parameter,
                                      short type,
                                      arg_unused (const string &value))
{
  NclFormatterEvent *mainEvent;

  switch (code)
    {
    case IPlayer::PL_NOTIFY_OUTTRANS:
      if (_outTransDur > 0.0)
        {
          NclCascadingDescriptor *descriptor;
          NclFormatterRegion *fRegion;

          _outTransDur = -1.0;
          _outTransTime = -1.0;
          descriptor = _object->getDescriptor ();
          if (descriptor != nullptr)
            {
              fRegion = descriptor->getFormatterRegion ();
              if (fRegion != nullptr)
                {
                  clog << "AdapterFormatterPlayer::";
                  clog << "updateStatus transition" << endl;
                  fRegion->performOutTrans ();
                }
            }
        }
      break;

    case IPlayer::PL_NOTIFY_STOP:
      if (_object != nullptr)
        {
          if (type == IPlayer::TYPE_PRESENTATION)
            {
              if (parameter == "")
                {
                  naturalEnd ();
                }
            }
          else if (type == IPlayer::TYPE_SIGNAL)
            {
              mainEvent = _object->getMainEvent ();
              if (mainEvent != nullptr
                  && mainEvent->getCurrentState ()
                         != EventUtil::ST_SLEEPING)
                {
                  clog << "AdapterFormatterPlayer::updateStatus ";
                  clog << "process recovered";
                  clog << endl;
                }
            }
        }
      break;

    default:
      break;
    }
}

//dragon head
void
AdapterFormatterPlayer::keyInputCallback (SDL_EventType evtType, SDL_Keycode key){

   if(evtType == SDL_KEYDOWN)
      return;
  /*
  if (xruntime_ms () - eventTS < 300){
      return;
  } */

  if (_object != nullptr && _player != nullptr)
    {
      clog << "AdapterFormatterPlayer::keyEventReceived for '";
      clog << _mrl << "' player visibility = '" << _player->isVisible ();
      clog << "' event key code = '" << key ;
      clog << "'";
      clog << endl;

      if (_player->isVisible ())
        {
          _eventTS = (double) xruntime_ms ();
          _object->selectionEvent (key, _player->getMediaTime () * 1000);
        }
    }

}

void
AdapterFormatterPlayer::setVisible (bool visible)
{
  NclCascadingDescriptor *descriptor;
  NclFormatterRegion *region;

  descriptor = _object->getDescriptor ();
  if (descriptor != nullptr)
    {
      region = descriptor->getFormatterRegion ();
      if (region != nullptr)
        {
          region->setRegionVisibility (visible);
          _player->setVisible (visible);
        }
    }
}

bool
AdapterFormatterPlayer::lockObject ()
{
  if (_isLocked)
    {
      return false;
    }
  _isLocked = true;
  Thread::mutexLock (&_objectMutex);
  return true;
}

bool
AdapterFormatterPlayer::unlockObject ()
{
  if (!_isLocked)
    {
      return false;
    }
  Thread::mutexUnlock (&_objectMutex);
  _isLocked = false;
  return true;
}

GINGA_FORMATTER_END
