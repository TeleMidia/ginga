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
#include "NclApplicationExecutionObject.h"
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
  this->_isAppPlayer = false;
  this->_currentEvent = nullptr;
  g_assert (Ginga_Display->registerEventListener (this));
}

PlayerAdapter::~PlayerAdapter ()
{
  if (_player)
    {
      _player->removeListener (this);
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

  NclApplicationExecutionObject *appObject;
  string ifId;

  appObject = dynamic_cast <NclApplicationExecutionObject *> (_object);
  g_assert_nonnull (appObject);

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
      appObject->setCurrentEvent (_currentEvent);
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
      appObject->setCurrentEvent (_currentEvent);

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

double
PlayerAdapter::getMediaTime ()
{
  g_assert_nonnull (_player);
  return (double)(GINGA_TIME_AS_MSECONDS (_player->getMediaTime ()));
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
  NclFormatterEvent *evt;

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

  if (_isAppPlayer)
    return true;                // nothing to do

  if (_player->isForcedNaturalEnd ())
    {
      TRACE ("failed, a natural end was forced");
      return false;
    }

  evt = _object->getMainEvent ();
  if (evt == nullptr)
    {
      TRACE ("failed, main event is null");
      return false;
    }

  if (evt->getCurrentState () == EventUtil::ST_SLEEPING)
    {
      TRACE ("failed, main event is sleeping");
      return false;
    }

  return true;
}

GingaTime
PlayerAdapter::prepareProperties (NclExecutionObject *obj)
{
  NclCascadingDescriptor *descriptor;
  LayoutRegion *region = nullptr;
  vector<PropertyAnchor *> *anchors;
  string name, value;
  NclFormatterRegion *fRegion = nullptr;
  Node *ncmNode;
  double transpValue = -1;
  double parentOpacity = -1;
  bool isPercent = false;
  GingaTime explicitDur = GINGA_TIME_NONE;

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
          value = property->getValue ();
          explicitDur = ginga_parse_time (value);
        }

      return explicitDur;
    }

  map <string, string> properties;

  // Get the properties from the descriptor.
  for (Parameter &param : descriptor->getParameters())
    {
      properties[param.getName()] = param.getValue();
    }

  // Get the properties from the object.
  ncmNode = obj->getDataObject ();
  anchors = ((Node *)ncmNode)->getOriginalPropertyAnchors ();
  g_assert_nonnull (anchors);

  for (PropertyAnchor *property : *anchors)
    properties[property->getName()] = property->getValue();

  for (auto it: properties)
    {
      name = it.first;
      value = it.second;

      TRACE ( "preparing name='%s', value='%s'", name.c_str(), value.c_str());
      if (value != "")
        {
          if (name == "explicitDur")
            {
              explicitDur = ginga_parse_time (value);
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
              //zindex = value;
            }
          else if (name == "bounds")
            {
              vector<string> v;

              if (unlikely (!_ginga_parse_list (value, ',', 4, 4, &v)))
                {
                  ERROR_SYNTAX ("property 'bounds': bad value '%s'",
                                value.c_str ());
                }
              left = v[0];
              top = v[1];
              width = v[2];
              height = v[3];
            }
          else if (name == "location")
            {
              vector<string> v;

              if (unlikely (!_ginga_parse_list (value, ',', 2, 2, &v)))
                {
                  ERROR_SYNTAX ("property 'location': bad value '%s'",
                                value.c_str ());
                }
              left = v[0];
              top = v[1];
            }
          else if (name == "size")
            {
              vector<string> v;

              if (unlikely (!_ginga_parse_list (value, ',', 2, 2, &v)))
                {
                  ERROR_SYNTAX ("property 'location': bad value '%s'",
                                value.c_str ());
                }
              width = v[0];
              height = v[1];
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
                  fRegion->setBackgroundColor (ginga_parse_color (value));
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
                  fRegion->setFocusBorderColor (ginga_parse_color (value));
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
                  fRegion->setSelBorderColor (ginga_parse_color (value));
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

  if (transpValue < 0.
      && descriptor->getParameterValue ("transparency") == "")
    {
      transpValue = 1.;

      if (fRegion != nullptr)
        {
          fRegion->setTransparency (transpValue);
        }
    }

  if (descriptor != nullptr)
    explicitDur = descriptor->getExplicitDuration ();

  return explicitDur;
}

bool
PlayerAdapter::prepare (NclExecutionObject *object,
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

  if (object->instanceOf("NclApplicationExecutionObject"))
    {
      if (this->_object != object)
        {
          _preparedEvents.clear ();
          this->_object = object;

          g_assert_nonnull (_object);
          g_assert_nonnull (_object->getDataObject());
          g_assert_nonnull (_object->getDataObject()->getDataEntity());

          NodeEntity *nodeEntity
              = dynamic_cast <NodeEntity *> (object->getDataObject()->getDataEntity());
          g_assert_nonnull (nodeEntity);

          content = nodeEntity->getContent ();
          ReferenceContent *referContent
              = dynamic_cast <ReferenceContent *> (content);
          if (content && referContent)
            {
              mrl = referContent->getCompleteReferenceUrl ();
            }
          else
            {
              mrl = "";
            }

          if (_player != NULL)
            {
              delete _player;
              _player = NULL;
            }

          explicitDur = prepareProperties (object);
          createPlayer (mrl);
        }
      else
        {
          explicitDur = prepareProperties (object);
        }

      NclPresentationEvent *presentationEvt =
          dynamic_cast <NclPresentationEvent *>(event);
      if (presentationEvt)
        {
          GingaTime duration = presentationEvt->getDuration ();

          if (duration == 0 && explicitDur == 0)
            return false;

          // explicit duration overwrites implicit duration
          if (GINGA_TIME_IS_VALID (explicitDur))
            {
              _object->removeEvent (event);
              presentationEvt->setEnd (explicitDur);
              _object->addEvent (event);
            }
        }

      if (event->getCurrentState () == EventUtil::ST_SLEEPING)
        {
          if (!this->_object->prepare (event, 0))
            {
              return false;
            }

          prepare (event);
          return true;
        }
      else
        {
          return false;
        }
    }
  else
    {
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

      explicitDur = prepareProperties (object);

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
      NclCascadingDescriptor *descriptor = _object->getDescriptor ();
      if (descriptor != nullptr)
        {
          string value = descriptor->getParameterValue ("soundLevel");
          if (value == "")
            {
              value = "1.0";
            }

          _player->setProperty ("soundLevel", value);
        }

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
}

void
PlayerAdapter::prepare ()
{
  g_assert_nonnull (_object);
  g_assert_nonnull (_player);

  prepareScope ();
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
  NclCascadingDescriptor *descriptor;
  string paramValue;

  g_assert_nonnull (_object);
  g_assert_nonnull (_player);

  if (!_object->isSleeping ())
    {
      WARNING ("trying to start '%s', but it is not sleeping",
               _object->getId ().c_str ());

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
    }

  return startSuccess;
}

bool
PlayerAdapter::stop ()
{
  g_assert_nonnull (_object);
  g_assert_nonnull (_player);

  if(_isAppPlayer)
    {
      map<string, NclFormatterEvent *>::iterator i;
      NclFormatterEvent *event;
      bool stopLambda = false;

      if (_currentEvent != nullptr)
        {
          NclAnchorEvent *anchorEvt
              = dynamic_cast <NclAnchorEvent *>(_currentEvent);
          if(anchorEvt
             && anchorEvt->getAnchor() != NULL
             && anchorEvt->getAnchor()->instanceOf("LambdaAnchor"))
            {
              stopLambda = true;
            }
        }

      if (stopLambda)
        {
          TRACE ("stop lambda");

          if (_currentEvent->getCurrentState () != EventUtil::ST_SLEEPING)
            {
              _player->stop ();
            }

          i = _preparedEvents.begin ();
          while (i != _preparedEvents.end ())
            {
              event = i->second;
              if (event != _currentEvent
                  && event->getCurrentState () != EventUtil::ST_SLEEPING)
                {
                  _preparedEvents.erase (i);
                  i = _preparedEvents.begin ();

                  TRACE ("forcing '%s' to stop", event->getId().c_str());
                  event->stop ();
                }
              else
                {
                  ++i;
                }
            }
        }
      else if (!_player->isForcedNaturalEnd ())
        {
          _player->stop ();
        }

      if (_object->stop ())
        {
          unprepare ();
          return true;
        }

      if (stopLambda && !_currentEvent->stop ())
        {
          WARNING ("trying to stop '%s', but it is already sleeping",
                   _currentEvent->getId ().c_str ());
        }
      else
        {
          WARNING ("failed to stop '%s'",
                   _currentEvent->getId ().c_str ());
        }
      return false;
    }
  else
    {
      NclFormatterEvent *mainEvent = nullptr;

      mainEvent = _object->getMainEvent ();

      if (mainEvent != nullptr)
        {
          NclPresentationEvent *presentationEvt
              = dynamic_cast <NclPresentationEvent *> (mainEvent);

          if ( presentationEvt && checkRepeat(presentationEvt) )
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

      //_player->removeListener (this);
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

  if (_isAppPlayer)
    {
      map<string, NclFormatterEvent *>::iterator i;
      NclFormatterEvent *event;
      bool abortLambda = false;

      g_assert_nonnull (_player);
      g_assert_nonnull (_object);

      if (_currentEvent != nullptr)
        {
          NclAnchorEvent *anchorEvt
              = dynamic_cast <NclAnchorEvent *>(_currentEvent);
          if(anchorEvt
             && anchorEvt->getAnchor() != NULL
             && anchorEvt->getAnchor()->instanceOf("LambdaAnchor"))
            {
              abortLambda = true;
            }
        }

      if (abortLambda)
        {
          _player->stop ();

          for (i = _preparedEvents.begin (); i != _preparedEvents.end (); )
            {
              event = i->second;
              if (event != _currentEvent
                  && event->getCurrentState () != EventUtil::ST_SLEEPING)
                {
                  i = _preparedEvents.erase (i);
                  TRACE ("forcing '%s' to abort", event->getId().c_str());
                  event->abort ();
                }
              else
                {
                  ++i;
                }
            }
        }
      else if (!_player->isForcedNaturalEnd ())
        {
          _player->stop ();
        }

      if (_object->abort ())
        {
          unprepare ();
          return true;
        }

      if (abortLambda && !_currentEvent->abort ())
        {
          TRACE ("failed to abort '%s', event is sleeping",
                 _currentEvent->getId ().c_str());
        }
      else
        {
          TRACE ("failed to abort, object='%p' mrl='%s'",
                 _object, _currentEvent->getId ().c_str());
        }
      return false;
    }
  else
    {
      _player->stop ();
      if (!_object->isSleeping ())
        {
          _object->abort ();
          unprepare ();
          return true;
        }

      return false;
    }
}

void
PlayerAdapter::naturalEnd ()
{
  g_assert_nonnull (_object);
  g_assert_nonnull (_player);

  if (_isAppPlayer)
    {
      map<string, NclFormatterEvent *>::iterator i;
      NclFormatterEvent *event;

      g_assert_nonnull (_object);
      g_assert_nonnull (_player);

      i = _preparedEvents.begin ();
      while (i != _preparedEvents.end ())
        {
          event = i->second;
          if (event != NULL
              && event->instanceOf ("NclAnchorEvent")
              && ((NclAnchorEvent *)event)->getAnchor () != NULL
              && ((NclAnchorEvent *)event)
                     ->getAnchor ()
                     ->instanceOf ("LambdaAnchor"))
            {
              event->stop ();
              unprepare ();
              return;
            }
          ++i;
        }

      if (_object->stop ())
        {
          unprepare ();
        }
    }
  else
    {
      stop ();
    }
}

bool
PlayerAdapter::checkRepeat (NclPresentationEvent *event)
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
PlayerAdapter::unprepare ()
{
  g_assert_nonnull (_object);
  g_assert_nonnull (_player);

  if(_isAppPlayer)
    {
      g_assert_nonnull (_object);

      if (_currentEvent == nullptr)
        {
          _scheduler->removePlayer (_object);
          _object->unprepare ();

          return true;
        }

      if (_currentEvent->getCurrentState () == EventUtil::ST_OCCURRING
          || _currentEvent->getCurrentState () == EventUtil::ST_PAUSED)
        {
          _currentEvent->stop ();
        }

      if (_preparedEvents.count (_currentEvent->getId ()) != 0
          && _preparedEvents.size () == 1)
        {
          _object->unprepare ();
          _scheduler->removePlayer (_object);
          _preparedEvents.clear ();

          _object = NULL;
        }
      else
        {
          _object->unprepare ();

          map<string, NclFormatterEvent *>::iterator i
              = _preparedEvents.find (_currentEvent->getId ());
          if (i != _preparedEvents.end ())
            {
              _preparedEvents.erase (i);
            }
        }

      return true;

    }
  else
    {
      if (_object->getMainEvent () != nullptr
          && (_object->getMainEvent ()->getCurrentState ()
                  == EventUtil::ST_OCCURRING
              || _object->getMainEvent ()->getCurrentState ()
                     == EventUtil::ST_PAUSED))
        {
          return stop ();
        }

      _scheduler->removePlayer (_object);

      if (NclExecutionObject::hasInstance (_object, false))
        _object->unprepare ();

      _object = nullptr;

      return true;
    }
}

bool
PlayerAdapter::setProperty (NclAttributionEvent *event,
                            const string &v)
{
  string propName;
  string value = v;

  g_assert_nonnull (_player);
  g_assert_nonnull (_object);

  propName = event->getAnchor ()->getName ();
  if (propName == "visible")
    {
      setVisible (value == "true");
    }
  else
    {
      if(_object->instanceOf("NclApplicationExecutionObject"))
        {
          if (!setCurrentEvent (event))
            {
              return false;
            }
        }

      if (_object->setProperty (event, value))
        {
          _player->setProperty (propName, _object->getProperty (propName));
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
          _player->setProperty (propName, value);
        }
    }

  return true;
}

void
PlayerAdapter::setProperty (const string &name,
                            const string &value)
{
  g_assert_nonnull (_player);
  TRACE ("setting property name='%s' to value='%s' (player='%p')",
         name.c_str (), value.c_str (), _player);
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
  if (value == "")
    value = _object->getProperty (name);

  TRACE ("getting property with name='%s', value='%s'",
         name.c_str (), value.c_str ());

  return value;
}

void
PlayerAdapter::updateObjectExpectedDuration ()
{
  NclPresentationEvent *wholeContentEvent;
  GingaTime duration;
  GingaTime implicitDur;

  wholeContentEvent = _object->getWholeContentPresentationEvent ();
  duration = wholeContentEvent->getDuration ();

  if ((_object->getDescriptor () == nullptr)
      || (!GINGA_TIME_IS_VALID (_object->getDescriptor ()
                                ->getExplicitDuration ()))
      || (!GINGA_TIME_IS_VALID (duration)))
    {
      implicitDur = GINGA_TIME_NONE;

      IntervalAnchor *intervalAnchor
          = dynamic_cast <IntervalAnchor *> (wholeContentEvent->getAnchor ());
      g_assert_nonnull(intervalAnchor);
      intervalAnchor->setEnd (implicitDur);
    }
}

void
PlayerAdapter::updateStatus (short code,
                             const string &parameter,
                             short type,
                             arg_unused (const string &value))
{
  g_assert_nonnull (_object);

  switch (code)
    {
    case Player::PL_NOTIFY_STOP:
      if (type == Player::PL_TYPE_PRESENTATION)
        {
          if (parameter == "")
            {
              naturalEnd ();
            }
        }
      break;

    default:
      break;
    }
}

void
PlayerAdapter::handleTickEvent (arg_unused (GingaTime total),
                                GingaTime diff,
                                arg_unused (int frame))
{
  NclEventTransition *next;
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

  if (_player->isVisible ())
    {
      GingaTime time = _player->getMediaTime ();

      TRACE ("key '%d' received for '%s' (time=%" GINGA_TIME_FORMAT ")",
             key, _player->getProperty ("mrl").c_str (),
             GINGA_TIME_ARGS (time));

      _object->selectionEvent (key, time);
    }
}

void
PlayerAdapter::setVisible (bool visible)
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


// Private.

void
PlayerAdapter::createPlayer (const string &mrl)
{
  NclCascadingDescriptor *descriptor;
  NodeEntity *dataObject;
  PropertyAnchor *property;
  NodeEntity *entity;
  Content *content;

  string buf;
  const char *mime;

  g_assert_nonnull (_object);
  dataObject = dynamic_cast <NodeEntity *>(_object->getDataObject ());
  g_assert_nonnull (dataObject);

  entity = dynamic_cast <NodeEntity *>(dataObject->getDataEntity ());
  g_assert_nonnull (entity);
  g_assert (entity->instanceOf ("ContentNode"));

  content = entity->getContent ();
  g_assert_nonnull (content);

  buf = ((ContentNode *) entity)->getNodeType ();
  mime = buf.c_str ();
  g_assert_nonnull (mime);

  if (_player == nullptr)
    {
      if (g_str_has_prefix (mime, "audio")
          || g_str_has_prefix (mime, "video"))
        {
          _player = new VideoPlayer (mrl);
        }
#if WITH_LIBRSVG && WITH_LIBRSVG
      else if (g_str_has_prefix (mime, "image/svg"))
        {
          _player = new SvgPlayer (mrl);
        }
#endif
      else if (g_str_has_prefix (mime, "image"))
        {
          _player = new ImagePlayer (mrl);
        }
#if defined WITH_CEF &&  WITH_CEF
      else if (g_str_has_prefix (mime, "text/html"))
        {
          _player = new HTMLPlayer (mrl);
        }
#endif
#if defined WITH_PANGO && WITH_PANGO
      else if (streq (mime, "text/plain"))
        {
          _player = new TextPlayer (mrl);
        }
#endif
      else if (g_strcmp0 (mime, "application/x-ginga-NCLua") == 0)
        {
          _player = new LuaPlayer (mrl);
          _isAppPlayer = true;
        }
      else
        {
          _player = new Player (mrl);
          WARNING ("unknown mime-type '%s'", mime);
        }
    }

  _player->addListener (this);
  descriptor = _object->getDescriptor ();
  if (descriptor != nullptr)
    {
      for (Parameter &param: descriptor->getParameters ())
        {
          _player->setProperty (param.getName (), param.getValue ());
        }
    }

  ContentNode *contentNode = dynamic_cast <ContentNode *> (dataObject);
  if (contentNode)
    {
      for (Anchor *anchor: contentNode->getAnchors ())
        {
          property = dynamic_cast <PropertyAnchor *> (anchor);
          if (property)
            {
              TRACE ("setting property property name='%s' to '%s' for %s",
                     property->getName ().c_str (),
                     property->getValue ().c_str (),
                     mrl.c_str ());

              _player->setProperty (property->getName(),
                                    property->getValue());
            }
        }
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
         mrl.c_str (), _object->getId ().c_str ());
}

GINGA_FORMATTER_END
