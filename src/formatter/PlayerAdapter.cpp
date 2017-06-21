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
#include "PlayerAdapter.h"

#include "FormatterScheduler.h"
#include "NclLinkTransitionTriggerCondition.h"

#include "mb/Display.h"
using namespace ::ginga::mb;

#include "player/Player.h"
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
using namespace ::ginga::player;

GINGA_FORMATTER_BEGIN


// Public.

PlayerAdapter::PlayerAdapter (FormatterScheduler *scheduler)
{
  this->_scheduler = scheduler;
  this->_object = nullptr;
  this->_player = nullptr;
  this->_currentEvent = nullptr;
  g_assert (Ginga_Display->registerEventListener (this));
}

PlayerAdapter::~PlayerAdapter ()
{
  if (_player)
    {
      if(_player->getMediaStatus () != Player::PL_SLEEPING)
        _player->stop ();
      delete _player;
    }

  _preparedEvents.clear ();
  g_assert (Ginga_Display->unregisterEventListener (this));
}

bool
PlayerAdapter::setCurrentEvent (NclFormatterEvent *event)
{

  string ifId;

  if (_preparedEvents.count (event->getId ()) != 0
      && !event->instanceOf ("NclSelectionEvent")
      && event->instanceOf ("NclAnchorEvent"))
    {
      NclAnchorEvent *anchorEvent = dynamic_cast <NclAnchorEvent *> (event);
      g_assert_nonnull (anchorEvent);

      LabeledAnchor *labeledAnchor
          = dynamic_cast<LabeledAnchor *> (anchorEvent->getAnchor());
      LambdaAnchor *lambdaAnchor
          = dynamic_cast<LambdaAnchor *> (anchorEvent->getAnchor());

      ifId = anchorEvent->getAnchor ()->getId ();

      if (labeledAnchor)
        {
          ifId = labeledAnchor->getLabel();
        }
      else if(lambdaAnchor)
        {
          ifId = "";
        }
      _currentEvent = event;
      _player->setCurrentScope (ifId);
    }
  else if (event->instanceOf ("NclAttributionEvent"))
    {
      NclAttributionEvent *attributionEvt
          = dynamic_cast <NclAttributionEvent *> (event);
      g_assert_nonnull (attributionEvt);

      ifId = attributionEvt->getAnchor ()->getName ();
      _player->setScope (ifId, Player::PL_TYPE_ATTRIBUTION);

      _currentEvent = event;
      _player->setCurrentScope (ifId);
    }
  else
    {
      WARNING ("event '%s' isn't prepared",
               event->getId ().c_str());

      return false;
    }
  return true;
}

Player *
PlayerAdapter::getPlayer ()
{
  return _player;
}

void
PlayerAdapter::setOutputWindow (SDLWindow *win)
{
  g_assert_nonnull (_player);
  _player->setOutWindow (win);
}

bool
PlayerAdapter::hasPrepared ()
{
  if (_object == nullptr)
    {
      TRACE ("failed, object is null");
      return false;
    }

  if (_player == nullptr)
    {
      TRACE ("failed, player is null");
      return false;
    }

  return true;
}

bool
PlayerAdapter::prepare (ExecutionObject *object,
                        NclPresentationEvent *event)
{
  Content *content;
  GingaTime explicitDur = GINGA_TIME_NONE;
  string mrl = "";

  g_assert_nonnull (object);

  if (hasPrepared ())
    {
      TRACE ("failed, player already prepared");
      return false;
    }

  NodeEntity *dataObject;

  this->_object = object;
  dataObject = dynamic_cast<NodeEntity *>(object->getDataObject ());

  if (dataObject && dataObject->getDataEntity () != nullptr)
    {
      content
        = dynamic_cast<NodeEntity *> (dataObject->getDataEntity ())->getContent();

      if (content)
        {
          ReferenceContent *referContent
            = dynamic_cast <ReferenceContent *>(content);
          if (referContent)
            {
              mrl = referContent->getCompleteReferenceUrl ();
            }
        }
      else
        {
          mrl = "";
        }
    }

  NclPresentationEvent *presentationEvent =
    dynamic_cast <NclPresentationEvent *> (event);
  if (presentationEvent)
    {
      GingaTime duration = presentationEvent->getDuration ();
      if (duration == 0 && explicitDur == 0)
        return false;

      // explicit duration overwrites implicit duration
      if (GINGA_TIME_IS_VALID (explicitDur))
        {
          _object->removeEvent (event);
          presentationEvent->setEnd (explicitDur);
          _object->addEvent (event);
        }
    }

  createPlayer (mrl);
  g_assert_nonnull (_object);
  g_assert_nonnull (_player);

  if (event->getCurrentState () == EventState::SLEEPING)
    {
      object->prepare (event, 0);
      prepareScope ();
      return true;
    }
  else
    {
      return false;
    }
}

void
PlayerAdapter::prepare (NclFormatterEvent *event)
{
  GingaTime duration;

  NclAnchorEvent *anchorEvent = dynamic_cast <NclAnchorEvent *> (event);
  if (anchorEvent)
    {
      if (anchorEvent->getAnchor ()->instanceOf ("LambdaAnchor"))
        {
          NclPresentationEvent *presentationEvt
              = dynamic_cast <NclPresentationEvent*> (event);
          g_assert_nonnull (presentationEvt);

          duration = presentationEvt->getDuration ();

          if (GINGA_TIME_IS_VALID (duration))
            _player->setScope ("", Player::PL_TYPE_PRESENTATION, 0.,
                               duration);
        }
      else if (anchorEvent->getAnchor ()->instanceOf ("IntervalAnchor"))
        {
          IntervalAnchor *intervalAnchor
              = dynamic_cast<IntervalAnchor *>(anchorEvent->getAnchor ());
          g_assert_nonnull (intervalAnchor);

          _player->setScope (
              anchorEvent->getAnchor ()->getId (),
              Player::PL_TYPE_PRESENTATION,
              intervalAnchor->getBegin (),
              intervalAnchor->getEnd ());
        }
      else if (anchorEvent->getAnchor ()->instanceOf ("LabeledAnchor"))
        {
          NclPresentationEvent *presentationEvt
              = dynamic_cast <NclPresentationEvent*> (event);
          g_assert_nonnull (presentationEvt);
          duration = presentationEvt->getDuration ();

          LabeledAnchor *labeledAnchor
              = dynamic_cast <LabeledAnchor *> (anchorEvent->getAnchor());
          g_assert_nonnull (labeledAnchor);

          if (isnan (duration))
            {
              _player->setScope (
                  labeledAnchor->getLabel (),
                  Player::PL_TYPE_PRESENTATION);
            }
          else
            {
              _player->setScope (
                  labeledAnchor->getLabel (),
                  Player::PL_TYPE_PRESENTATION, 0, duration);
            }
        }
    }

  _preparedEvents[event->getId ()] = event;
}

void
PlayerAdapter::prepareScope (GingaTime offset)
{
  NclPresentationEvent *mainEvent;
  GingaTime duration;
  GingaTime initTime = 0;
  IntervalAnchor *intervalAnchor;

  mainEvent = dynamic_cast <NclPresentationEvent *>(_object->getMainEvent ());
  if (mainEvent)
    {
      if (mainEvent->getAnchor ()->instanceOf ("LambdaAnchor"))
        {
          duration = mainEvent->getDuration ();

          if (offset > 0)
            {
              initTime = offset;
            }

          if (GINGA_TIME_IS_VALID (duration))
            {
              _player->setScope (mainEvent->getAnchor ()->getId (),
                                Player::PL_TYPE_PRESENTATION, initTime,
                                duration);
            }
          else
            {
              _player->setScope (mainEvent->getAnchor ()->getId (),
                                Player::PL_TYPE_PRESENTATION, initTime);
            }
        }
      else if (mainEvent->getAnchor ()->instanceOf ("IntervalAnchor"))
        {
          intervalAnchor
              = dynamic_cast<IntervalAnchor *>(mainEvent->getAnchor ());
          g_assert_nonnull (intervalAnchor);

          initTime = (intervalAnchor->getBegin ());
          if (offset > 0)
            initTime = offset;

          duration = intervalAnchor->getEnd ();
          if (GINGA_TIME_IS_VALID (duration))
            {
              _player->setScope (mainEvent->getAnchor ()->getId (),
                                Player::PL_TYPE_PRESENTATION, initTime,
                                (intervalAnchor->getEnd ()));
            }
          else
            {
              _player->setScope (mainEvent->getAnchor ()->getId (),
                                 Player::PL_TYPE_PRESENTATION);
            }
        }
    }

  if (offset > 0)
    _player->setMediaTime (offset);
}

bool
PlayerAdapter::start ()
{
  string paramValue;

  g_assert_nonnull (_object);
  g_assert_nonnull (_player);

  if (!_object->isSleeping ())
    {
      WARNING ("trying to start '%s', but it is not sleeping",
               _object->getId ().c_str ());

      return false;
    }

  bool startSuccess = _player->play ();

  if (startSuccess)
    {
      if (!_object->start ())
        {
          _player->stop ();
          startSuccess = false;
        }
    }

  return startSuccess;
}

bool
PlayerAdapter::stop ()
{
  g_assert_nonnull (_object);
  g_assert_nonnull (_player);

  NclFormatterEvent *mainEvent = nullptr;

  mainEvent = _object->getMainEvent ();

  if (mainEvent != nullptr)
    {
      NclPresentationEvent *presentationEvt
        = dynamic_cast <NclPresentationEvent *> (mainEvent);

      if (presentationEvt && 0)
        {
          return true;
        }
    }
  for (NclFormatterEvent *evt: _object->getEvents ())
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
  _object->stop ();
  unprepare ();
  return true;
}

bool
PlayerAdapter::pause ()
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
PlayerAdapter::resume ()
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
PlayerAdapter::abort ()
{
  g_assert_nonnull (_object);
  g_assert_nonnull (_player);

  _player->stop ();
  if (!_object->isSleeping ())
    {
      _object->abort ();
      unprepare ();
    }
  return true;
}

bool
PlayerAdapter::unprepare ()
{
  g_assert_nonnull (_object);
  g_assert_nonnull (_player);

  if (_object->getMainEvent () != nullptr
      && (_object->getMainEvent ()->getCurrentState ()
          == EventState::OCCURRING
          || _object->getMainEvent ()->getCurrentState ()
          == EventState::PAUSED))
    {
      return stop ();
    }

  _scheduler->removePlayer (_object);

  if (ExecutionObject::hasInstance (_object, false))
    _object->unprepare ();

  _object = nullptr;

  return true;
}

void
PlayerAdapter::setProperty (NclAttributionEvent *event,
                            const string &value)
{
  string name;
  g_assert_nonnull (_player);
  g_assert_nonnull (event);
  name = event->getAnchor ()->getName ();
  _player->setProperty (name, value);
}

void
PlayerAdapter::setProperty (const string &name,
                            const string &value)
{
  g_assert_nonnull (_player);
  _player->setProperty (name, value);
}

string
PlayerAdapter::getProperty (NclAttributionEvent *event)
{
  PropertyAnchor *anchor;
  string name;
  string value;

  g_assert_nonnull (_object);
  g_assert_nonnull (_player);
  g_assert_nonnull (event);

  anchor = event->getAnchor ();
  g_assert_nonnull (anchor);

  name = anchor->getName ();
  value = _player->getProperty (name);

  TRACE ("getting property with name='%s', value='%s'",
         name.c_str (), value.c_str ());

  return value;
}

void
PlayerAdapter::handleTickEvent (arg_unused (GingaTime total),
                                GingaTime diff,
                                arg_unused (int frame))
{
  EventTransition *next;
  NclFormatterEvent *evt;
  GingaTime waited;
  GingaTime now;

  if (unlikely (_object == nullptr || _player == nullptr))
    return;

  if (_player->getMediaStatus() != Player::PL_OCCURRING)
    return;

  // Update player time.
  _player->incMediaTime (diff);

  next = _object->getNextTransition ();
  if (next == nullptr)
    return;

  waited = next->getTime ();
  now = _player->getMediaTime ();

  // TRACE ("now=%" GINGA_TIME_FORMAT " waited=%" GINGA_TIME_FORMAT,
  //        GINGA_TIME_ARGS (now), GINGA_TIME_ARGS (waited));

  if (now < waited)
    return;

  evt = dynamic_cast <NclFormatterEvent *> (next->getEvent ());
  g_assert_nonnull (evt);

  TRACE ("anchor '%s' timed out at %" GINGA_TIME_FORMAT
         ", updating transition table",
         evt->getId ().c_str(), GINGA_TIME_ARGS (now));

  _object->updateTransitionTable (now, _player);
};

void
PlayerAdapter::handleKeyEvent (SDL_EventType evtType,
                               SDL_Keycode key)
{
  if (unlikely (_object == nullptr || _player == nullptr))
    return;

  if (evtType == SDL_KEYDOWN)
    return;

  GingaTime time = _player->getMediaTime ();
  _object->selectionEvent (key, time);
}


// Private.

void
PlayerAdapter::createPlayer (const string &uri)
{
  NodeEntity *dataObject;
  NodeEntity *entity;

  NclCascadingDescriptor *descriptor;
  PropertyAnchor *property;

  string buf;
  const char *mime;

  g_assert_nonnull (_object);
  dataObject = dynamic_cast <NodeEntity *>(_object->getDataObject ());
  g_assert_nonnull (dataObject);

  entity = dynamic_cast <NodeEntity *>(dataObject->getDataEntity ());
  g_assert_nonnull (entity);
  g_assert (entity->instanceOf ("ContentNode"));

  buf = ((ContentNode *) entity)->getNodeType ();
  mime = buf.c_str ();
  g_assert_nonnull (mime);

  if (_player == nullptr)
    {
      if (g_str_has_prefix (mime, "audio")
          || g_str_has_prefix (mime, "video"))
        {
          _player = new VideoPlayer (uri);
        }
#if WITH_LIBRSVG && WITH_LIBRSVG
      else if (g_str_has_prefix (mime, "image/svg"))
        {
          _player = new SvgPlayer (uri);
        }
#endif
      else if (g_str_has_prefix (mime, "image"))
        {
          _player = new ImagePlayer (uri);
        }
#if defined WITH_CEF &&  WITH_CEF
      else if (g_str_has_prefix (mime, "text/html"))
        {
          _player = new HTMLPlayer (uri);
        }
#endif
#if defined WITH_PANGO && WITH_PANGO
      else if (streq (mime, "text/plain"))
        {
          _player = new TextPlayer (uri);
        }
#endif
      else if (g_strcmp0 (mime, "application/x-ginga-NCLua") == 0)
        {
          _player = new LuaPlayer (uri);
        }
      else
        {
          _player = new Player (uri);
          WARNING ("unknown mime-type '%s': creating empty player", mime);
        }
    }

  descriptor = _object->getDescriptor ();
  if (descriptor != nullptr)
    {
      NclFormatterRegion *fregion = descriptor->getFormatterRegion ();
      if (fregion != nullptr)
        {
          LayoutRegion *region;
          SDL_Rect rect;
          int z, zorder;

          region = fregion->getLayoutRegion ();
          g_assert_nonnull (region);

          rect = region->getRect ();
          region->getZ (&z, &zorder);

          _player->setRect (rect);
          _player->setZ (z, zorder);
        }

      for (Parameter &param: descriptor->getParameters ())
        _player->setProperty (param.getName (), param.getValue ());
    }

  ContentNode *contentNode = dynamic_cast <ContentNode *> (dataObject);
  if (contentNode)
    {
      string pos_x_name = "";
      string pos_x_value = "";
      string pos_y_name = "";
      string pos_y_value = "";
      string width = "";
      string height = "";
      for (Anchor *anchor: contentNode->getAnchors ())
        {
          string name, value;

          property = dynamic_cast <PropertyAnchor *> (anchor);
          if (!property)
            continue;

          name = property->getName ();
          value = property->getValue ();

          if (name == "left" || name == "right")
            {
              pos_x_name = name;
              pos_x_value = value;
              continue;
            }

          if (name == "top" || name == "bottom")
            {
              pos_y_name = name;
              pos_y_value = value;
              continue;
            }

          if (name == "width")
            {
              width = value;
              continue;
            }

          if (name == "height")
            {
              height = value;
              continue;
            }

          _player->setProperty (name, value);
        }

      if (width != "")
        _player->setProperty ("width", width);
      if (height != "")
        _player->setProperty ("height", height);
      if (pos_x_name != "")
        _player->setProperty (pos_x_name, pos_x_value);
      if (pos_y_name != "")
        _player->setProperty (pos_y_name, pos_y_value);
    }

  for (NclFormatterEvent *evt: _object->getEvents ())
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

  TRACE ("created player for '%s' object='%s'",
         uri.c_str (), _object->getId ().c_str ());
}

GINGA_FORMATTER_END
