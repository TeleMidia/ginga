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

#include "player/ImagePlayer.h"
#include "player/LuaPlayer.h"
#include "player/VideoPlayer.h"
#if defined WITH_LIBRSVG && WITH_LIBRSVG
# include "player/SvgPlayer.h"
#endif
#if defined WITH_PANGO && WITH_PANGO
# include "player/TextPlayer.h"
#endif
#if defined WITH_CEF && WITH_CEF
# include "player/HTMLPlayer.h"
#endif

GINGA_PRAGMA_DIAG_IGNORE (-Wfloat-conversion)

GINGA_FORMATTER_BEGIN

AdapterFormatterPlayer::AdapterFormatterPlayer (AdapterPlayerManager *manager)
{
  this->_manager = manager;
  this->_object = nullptr;
  this->_player = nullptr;
  this->_mrl = "";
  this->_isLocked = false;

  Thread::mutexInit (&_objectMutex, false);

  Ginga_Display->registerKeyEventListener(this);
}

AdapterFormatterPlayer::~AdapterFormatterPlayer ()
{
  lockObject ();

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
AdapterFormatterPlayer::setOutputWindow (SDLWindow* windowId)
{
  g_assert_nonnull (_player);

  _player->setOutWindow (windowId);
}

void
AdapterFormatterPlayer::createPlayer ()
{
  vector<NclFormatterEvent *> *events;
  NclCascadingDescriptor *descriptor;
  NodeEntity *dataObject;
  PropertyAnchor *property;
  g_assert_nonnull (_object);

  NodeEntity *entity
      = dynamic_cast<NodeEntity *>(_object->getDataObject ()->getDataEntity ());
  g_assert_nonnull (entity);
  g_assert (entity->instanceOf ("ContentNode"));

  Content *content = entity->getContent ();
  g_assert_nonnull (content);

  string buf = ((ContentNode *)entity)->getNodeType ();
  const char *mime = buf.c_str ();
  g_assert_nonnull (mime);

  if (_player == nullptr)
    {
#if defined WITH_GSTREAMER && WITH_GSTREAMER
      if (g_str_has_prefix (mime, "audio")
          || g_str_has_prefix (mime, "video"))
        {
          _player = new VideoPlayer (_mrl);
        }
#endif
#if WITH_LIBRSVG && WITH_LIBRSVG
      else if (g_str_has_prefix (mime, "image/svg"))
        {
          _player = new SvgPlayer (_mrl);
        }
#endif
      else if (g_str_has_prefix (mime, "image"))
        {
          _player = new ImagePlayer (_mrl);
        }
#if defined WITH_CEF &&  WITH_CEF
      else if (g_str_has_prefix (mime, "text/test-html"))
        {
          _player = new HTMLPlayer (_mrl);
        }
#endif
#if defined WITH_PANGO && WITH_PANGO
      else if (streq (mime, "text/plain"))
        {
          _player = new TextPlayer (_mrl);
        }
#endif
      else if (g_strcmp0 (mime, "application/x-ginga-NCLua") == 0)
        {
          _player = new LuaPlayer (_mrl);
        }
      else
        {
          _player = new Player (_mrl);
          g_warning ("adapter: unknown mime-type '%s'", mime);
        }
    }

  _player->addListener (this);

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

  ContentNode *contentNode = dynamic_cast <ContentNode *> (dataObject);
  if (contentNode)
    {
      for (Anchor *anchor: *(contentNode->getAnchors ()))
        {
          property = dynamic_cast <PropertyAnchor *> (anchor);
          if (property)
            {
              g_debug ("set property %s=%s for %s",
                       property->getPropertyName().c_str(),
                       property->getPropertyValue().c_str(),
                       _mrl.c_str());

              _player->setPropertyValue (property->getPropertyName(),
                                         property->getPropertyValue());
            }
        }
    }

  events = _object->getEvents ();
  for (NclFormatterEvent *evt: *events)
    {
      g_assert_nonnull (evt);
      NclAttributionEvent *attributionEvt
          = dynamic_cast <NclAttributionEvent *> (evt);
      if (attributionEvt)
        {
          property = attributionEvt->getAnchor ();
          attributionEvt->setValueMaintainer (this);
        }
    }
  delete events;

  g_debug ("Create player for '%s' object = '%s'",
           _mrl.c_str(),
           _object->getId().c_str());
}

bool
AdapterFormatterPlayer::hasPrepared ()
{
  bool presented;
  NclFormatterEvent *mEv;
  short st;

  if (_object == nullptr || _player == nullptr)
    {
      g_debug ("AdapterFormatterPlayer::hasPrepared returns false because"
               " object = '%p' and player = '%p'",
               _object, _player);
      return false;
    }

  presented = _player->isForcedNaturalEnd ();
  if (presented)
    {
      g_debug ("AdapterFormatterPlayer::hasPrepared return false because"
               " a natural end was forced");
      return false;
    }

  mEv = _object->getMainEvent ();
  NclApplicationExecutionObject *appExeObj =
      dynamic_cast <NclApplicationExecutionObject *> (_object);
  if (mEv != nullptr && (appExeObj == nullptr))
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
              parentOpacity = 1;
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
      double x = xstrtodorpercent (value, &isPercent);
      if (isPercent)
        x *= 100;
      region->setLeft (x, isPercent);
    }

  if (top != "")
    {
      double x = xstrtodorpercent (value, &isPercent);
      if (isPercent)
        x *= 100;
      region->setTop (x, isPercent);
    }

  if (width != "")
    {
      double x = xstrtodorpercent (value, &isPercent);
      if (isPercent)
        x *= 100;
      region->setWidth (x, isPercent);
    }

  if (height != "")
    {
      double x = xstrtodorpercent (value, &isPercent);
      if (isPercent)
        x *= 100;
      region->setHeight (x, isPercent);
    }

  if (bottom != "")
    {
      double x = xstrtodorpercent (value, &isPercent);
      if (isPercent)
        x *= 100;
      region->setBottom (x, isPercent);
    }

  if (right != "")
    {
      double x = xstrtodorpercent (value, &isPercent);
      if (isPercent)
        x *= 100;
      region->setRight (x, isPercent);
    }

  if (transpValue < 0.
      && descriptor->getParameterValue ("transparency") == "")
    {
      transpValue = 1.;

      if (fRegion != nullptr)
        {
          fRegion->setTransparency (transpValue);
        }
    }

  if (descriptor != nullptr && explicitDur < 0)
    {
      explicitDur = descriptor->getExplicitDuration ();
    }

  return explicitDur;
}

void
AdapterFormatterPlayer::updatePlayerProperties ()
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
                                 NclPresentationEvent *event)
{
  Content *content;
  double explicitDur = -1;
  NodeEntity *dataObject;

  g_assert_nonnull(object);

  if (hasPrepared ())
    {
      g_debug ("AdapterFormatterPlayer::prepare returns false, because the"
               "player is already prepared.");
      return false;
    }

  if (!lockObject ())
    {
      g_assert_nonnull (_player);
      if (_player->isForcedNaturalEnd ())
        {
          while (!lockObject ())
            ;
        }
      else
        {
          g_debug ("AdapterFormatterPlayer::prepare returns false because the"
                   "object is locked");
          return false;
        }
    }

  this->_object = object;
  dataObject = dynamic_cast<NodeEntity *>(object->getDataObject ());

  if (dataObject
      &&  dataObject->getDataEntity () != nullptr)
    {
      content
          = dynamic_cast<NodeEntity *> (dataObject->getDataEntity ())->getContent();

      if (content)
        {
          ReferenceContent *referContent
              = dynamic_cast <ReferenceContent *>(content);
          if (referContent)
            {
              this->_mrl = referContent->getCompleteReferenceUrl ();
            }
        }
      else
        {
          this->_mrl = "";
        }
    }

  explicitDur = prepareProperties (object);

  NclPresentationEvent *presentationEvent =
      dynamic_cast <NclPresentationEvent *> (event);
  if (presentationEvent)
    {
      double duration = presentationEvent->getDuration ();
      bool infDur = (isnan (duration) || isinf (duration));

      if (!infDur && duration <= 0 && explicitDur <= 0)
        {
          g_warning ("Can't prepare an object (%s) with an event duration <= 0",
                     object->getId().c_str());
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
          presentationEvent->setEnd (explicitDur);
          object->addEvent (event);

          g_debug ("Object '%s' with explicitDur = '%f' object duration was"
                   "'%f'. Updated info: event begin=%f, event end=%f.",
                   object->getId ().c_str(),
                   explicitDur,
                   duration,
                   presentationEvent->getBegin(),
                   presentationEvent->getEnd());
        }
    }

  createPlayer ();
  updatePlayerProperties ();

  if (event->getCurrentState () == EventUtil::ST_SLEEPING)
    {
      object->prepare (event, 0);
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
  g_assert_nonnull (_object);
  g_assert_nonnull (_player);

  prepareScope ();
}

void
AdapterFormatterPlayer::prepareScope (double offset)
{
  NclPresentationEvent *mainEvent;
  double duration;
  double initTime = 0;
  IntervalAnchor *intervalAnchor;

  mainEvent = dynamic_cast <NclPresentationEvent *>(_object->getMainEvent ());
  if (mainEvent)
    {
      if (mainEvent->getAnchor ()->instanceOf ("LambdaAnchor"))
        {
          duration = mainEvent->getDuration ();

          if (offset > 0.0)
            {
              initTime = offset;
            }

          if (duration < IntervalAnchor::OBJECT_DURATION)
            {
              _player->setScope (mainEvent->getAnchor ()->getId (),
                                Player::PL_TYPE_PRESENTATION, initTime,
                                duration / 1000);
            }
          else
            {
              _player->setScope (mainEvent->getAnchor ()->getId (),
                                Player::PL_TYPE_PRESENTATION, initTime);
            }
        }
      else if (mainEvent->getAnchor ()->instanceOf ("IntervalAnchor"))
        {
          intervalAnchor = (IntervalAnchor *)(mainEvent->getAnchor ());
          initTime = (intervalAnchor->getBegin () / 1000);
          if (offset > 0)
            {
              initTime = offset;
            }

          duration = intervalAnchor->getEnd ();
          if (duration < IntervalAnchor::OBJECT_DURATION)
            {
              _player->setScope (mainEvent->getAnchor ()->getId (),
                                Player::PL_TYPE_PRESENTATION, initTime,
                                (intervalAnchor->getEnd () / 1000));
            }
          else
            {
              _player->setScope (mainEvent->getAnchor ()->getId (),
                                Player::PL_TYPE_PRESENTATION);
            }
        }
    }

  if (offset > 0)
    {
      _player->setMediaTime ( (guint32)offset);
    }
}

bool
AdapterFormatterPlayer::start ()
{
  NclCascadingDescriptor *descriptor;
  string paramValue;

  g_assert_nonnull (_object);
  g_assert_nonnull (_player);

  if (!_object->isSleeping ())
    {
      g_debug ("AdapterFormatterPlayer::start(%s) is occurring or paused.",
               _object->getId ().c_str());

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
    }

  bool startSuccess = _player->play ();

  if (startSuccess)
    {
      if (!_object->start ())
        {
          _player->stop ();
          startSuccess = false;
        }
      else
        {
          _object->setPlayer ((Player *) this->_player);
        }
    }

  return startSuccess;
}

bool
AdapterFormatterPlayer::stop ()
{
  g_assert_nonnull (_object);
  g_assert_nonnull (_player);

  NclFormatterEvent *mainEvent = nullptr;
  vector<NclFormatterEvent *> *events = nullptr;

  mainEvent = _object->getMainEvent ();
  events = _object->getEvents ();

  if (mainEvent != nullptr)
    {
      NclPresentationEvent *presentationEvt
          = dynamic_cast <NclPresentationEvent *> (mainEvent);

      if ( presentationEvt && checkRepeat(presentationEvt) )
        {
            return true;
        }
    }

  g_assert_nonnull (events);
  for (NclFormatterEvent *evt: *events)
    {
      g_assert_nonnull(evt);
      NclAttributionEvent *attributionEvt
          = dynamic_cast <NclAttributionEvent *> (evt);

      if (attributionEvt)
        {
          attributionEvt->setValueMaintainer (nullptr);
        }
    }

  _player->stop ();

  if (_player->isForcedNaturalEnd ())
    {
      _player->forceNaturalEnd (false);
      _object->stop ();
      return unprepare ();
    }
  else
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
  g_assert_nonnull (_object);
  g_assert_nonnull (_player);

  if (_object->pause ())
    {
      _player->pause ();
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
  g_assert_nonnull (_object);
  g_assert_nonnull (_player);
  if (_object->resume ())
    {
      _player->resume ();
      return true;
    }
  return false;
}

bool
AdapterFormatterPlayer::abort ()
{
  g_assert_nonnull (_object);
  g_assert_nonnull (_player);

  _player->stop ();
  if (!_object->isSleeping ())
    {
      _object->abort ();
      unprepare ();
      return true;
    }

  unlockObject ();
  return false;
}

void
AdapterFormatterPlayer::naturalEnd ()
{
  g_assert_nonnull (_object);
  g_assert_nonnull (_player);

  // if freeze is true the natural end is not performed
  if (_object->getDescriptor () != nullptr)
    {
      bool freeze = _object->getDescriptor ()->getFreeze ();
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
  g_assert_nonnull (_player);

  if (event->getRepetitions () > 1)
    {
      _player->stop ();
      if (_object != nullptr)
        _object->stop ();

      prepare ();
      return true;
    }

  return false;
}

bool
AdapterFormatterPlayer::unprepare ()
{
  g_assert_nonnull (_object);
  if (_object->getMainEvent () != nullptr
      && (_object->getMainEvent ()->getCurrentState ()
              == EventUtil::ST_OCCURRING
          || _object->getMainEvent ()->getCurrentState ()
                 == EventUtil::ST_PAUSED))
    {
      return stop ();
    }

  _manager->removePlayer (_object);
  Ginga_Display->unregisterKeyEventListener(this);

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

  g_assert_nonnull (_player);
  g_assert_nonnull (_object);

  propName = event->getAnchor ()->getPropertyName ();
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
              parentOpacity = 1;
              transpValue
                  = (1 - (parentOpacity - (parentOpacity * transpValue)));

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
AdapterFormatterPlayer::setPropertyValue (const string &name,
                                          const string &value)
{
  g_assert_nonnull (_player);
  g_debug ("AdapterFormatterPlayer::setPropertyValue name = '%s' value='%s' "
           "address=%p.",
           name.c_str(),
           value.c_str(),
           _player);

  _player->setPropertyValue (name, value);
}

string
AdapterFormatterPlayer::getPropertyValue (NclAttributionEvent *event)
{
  g_assert_nonnull (event);

  string name = event->getAnchor ()->getPropertyName ();
  string value = getPropertyValue (name);

  return value;
}

string
AdapterFormatterPlayer::getPropertyValue (const string &name)
{
  string value = "";

  g_assert_nonnull (_player);
  g_assert_nonnull (_object);

  value = _player->getPropertyValue (name);
  if (value == "")
    {
      value = _object->getPropertyValue (name);
    }

  g_debug ("getPropertyValue name = '%s', value = '%s'",
           name.c_str(), value.c_str());

  return value;
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
      implicitDur = IntervalAnchor::OBJECT_DURATION;

      IntervalAnchor *intervalAnchor
          = dynamic_cast <IntervalAnchor *> (wholeContentEvent->getAnchor ());
      g_assert_nonnull(intervalAnchor);
      intervalAnchor->setEnd (implicitDur);

      wholeContentEvent->setDuration (implicitDur);
    }
}

double
AdapterFormatterPlayer::getMediaTime ()
{
  g_assert_nonnull(_player);
  return _player->getMediaTime ();
}

Player *
AdapterFormatterPlayer::getPlayer ()
{
  return _player;
}

void
AdapterFormatterPlayer::updateStatus (short code,
                                      const string &parameter,
                                      short type,
                                      arg_unused (const string &value))
{
  switch (code)
    {
    case Player::PL_NOTIFY_STOP:
      if (_object != nullptr)
        {
          if (type == Player::PL_TYPE_PRESENTATION)
            {
              if (parameter == "")
                {
                  naturalEnd ();
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
AdapterFormatterPlayer::keyInputCallback (SDL_EventType evtType,
                                          SDL_Keycode key)
{
  if(evtType == SDL_KEYDOWN)
    return;

  cout << "keyEventReceived for '%s'" << _mrl.c_str() << endl;

  g_assert_nonnull (_object);
  g_assert_nonnull (_player);

  g_debug ("keyEventReceived for '%s' player visibility = '%d' "
           "event keycode='%d'",
           _mrl.c_str(),
           _player->isVisible(),
           key);

  if (_player->isVisible ())
    _object->selectionEvent (key, _player->getMediaTime () * 1000);
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
