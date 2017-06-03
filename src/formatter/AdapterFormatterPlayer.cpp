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
#include "AdapterFormatterPlayer.h"

#include "AdapterPlayerManager.h"
#include "NclLinkTransitionTriggerCondition.h"

#include "NclApplicationExecutionObject.h"

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

  this->_isAppPlayer = false;
  this-> _currentEvent = nullptr;

  Ginga_Display->registerEventListener (this);
}

AdapterFormatterPlayer::~AdapterFormatterPlayer ()
{
  if (_player != nullptr)
    {
      _player->removeListener (this);
      _player->stop ();

      delete _player;
    }

  _preparedEvents.clear ();
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
          _isAppPlayer = true;
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
      for (Anchor *anchor: contentNode->getAnchors ())
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
  else if (_isAppPlayer)
    {
      return true;
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

  return false;
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
              //zindex = value;
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
                  SDL_Color bg = {0, 0, 0, 255};
                  ginga_color_parse (value, &bg);
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
                  SDL_Color color;
                  g_assert (ginga_color_parse (value, &color));
                  fRegion->setFocusBorderColor (color);
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
                  SDL_Color color;
                  g_assert (ginga_color_parse (value, &color));
                  fRegion->setSelBorderColor (color);
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

  if (descriptor != nullptr && explicitDur < 0)
    {
      explicitDur = descriptor->getExplicitDuration ();
    }

  return explicitDur;
}

void
AdapterFormatterPlayer::updatePlayerProperties ()
{
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

      _player->setPropertyValue ("soundLevel", value);
    }
}

bool
AdapterFormatterPlayer::prepare (NclExecutionObject *object,
                                 NclPresentationEvent *event)
{
  Content *content;
  double explicitDur = -1;

  g_assert_nonnull (object);

  if (hasPrepared ())
    {
      g_debug ("AdapterFormatterPlayer::prepare returns false, because the"
               "player is already prepared.");
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
              this->_mrl = referContent->getCompleteReferenceUrl ();
            }
          else
            {
              this->_mrl = "";
            }

          if (_player != NULL)
            {
              delete _player;
              _player = NULL;
            }

          explicitDur = prepareProperties (object);
          createPlayer ();
        }
      else
        {
          explicitDur = prepareProperties (object);
        }

      NclPresentationEvent *presentationEvt =
          dynamic_cast <NclPresentationEvent *>(event);
      if (presentationEvt)
        {
          double duration = presentationEvt->getDuration ();
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
              _object->removeEvent (event);

              // the explicit duration is a property of
              // the object. Which means: start an interface with
              // begin = 4s an explicit duration = 5s => new duration
              // will be 1s
              presentationEvt->setEnd (explicitDur);

              /*
               * Adding event in object even though it is added inside
               * application execution object prepare (we have to consider
               * that the event could be already prepared
               */
              _object->addEvent (event);

              g_debug ("Object '%s' with explicitDur = '%f' object duration was"
                       "'%f'. Updated info: event begin=%f, event end=%f.",
                       _object->getId ().c_str(),
                       explicitDur,
                       duration,
                       presentationEvt->getBegin(),
                       presentationEvt->getEnd());
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
              _object->removeEvent (event);

              // The explicit duration is a property of
              // the object. For instance: start an interface with
              // begin = 4s and explicit duration = 5s => new duration
              // will be 1s
              presentationEvent->setEnd (explicitDur);

              _object->addEvent (event);

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
}

void
AdapterFormatterPlayer::prepare ()
{
  g_assert_nonnull (_object);
  g_assert_nonnull (_player);

  prepareScope ();
}

void
AdapterFormatterPlayer::prepare (NclFormatterEvent *event)
{
  double duration;

  NclAnchorEvent *anchorEvent = dynamic_cast <NclAnchorEvent *> (event);
  if (anchorEvent)
    {
      if (anchorEvent->getAnchor ()->instanceOf ("LambdaAnchor"))
        {
          NclPresentationEvent *presentationEvt
              = dynamic_cast <NclPresentationEvent*> (event);
          g_assert_nonnull (presentationEvt);

          duration = presentationEvt->getDuration ();

          if (duration < IntervalAnchor::OBJECT_DURATION)
            _player->setScope ("", Player::PL_TYPE_PRESENTATION, 0.,
                               duration/1000);
        }
      else if (anchorEvent->getAnchor ()->instanceOf ("IntervalAnchor"))
        {
          IntervalAnchor *intervalAnchor
              = dynamic_cast<IntervalAnchor *>(anchorEvent->getAnchor ());
          g_assert_nonnull (intervalAnchor);

          _player->setScope (
              anchorEvent->getAnchor ()->getId (),
              Player::PL_TYPE_PRESENTATION,
              (intervalAnchor->getBegin () / 1000.0),
              (intervalAnchor->getEnd () / 1000.0));
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
                  Player::PL_TYPE_PRESENTATION, 0.0, duration / 1000.0);
            }
        }
    }

  _preparedEvents[event->getId ()] = event;
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
          intervalAnchor
              = dynamic_cast<IntervalAnchor *>(mainEvent->getAnchor ());
          g_assert_nonnull (intervalAnchor);

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
      g_warning ("Trying to start %s, while it is occurring or paused.",
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
          _object->setPlayer (this->_player);
        }
    }

  return startSuccess;
}

bool
AdapterFormatterPlayer::stop ()
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
          g_debug ("AdapterApplicationPlayer::stop ALL");

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

                  g_debug ("AdapterApplicationPlayer::stop ALL forcing '%s' to "
                           "stop",
                           event->getId().c_str());

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
          g_warning ("AdapterApplicationPlayer::stop '%s' is already sleeping",
                     _currentEvent->getId ().c_str());
        }
      else
        {
          g_warning ("Can't stop an already stopped object. mrl = '%s' ",
                     _mrl.c_str());
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
          g_debug ("AdapterApplicationPlayer::abort ALL");

          _player->stop ();

          i = _preparedEvents.begin ();
          while (i != _preparedEvents.end ())
            {
              event = i->second;
              if (event != _currentEvent
                  && event->getCurrentState () != EventUtil::ST_SLEEPING)
                {
                  _preparedEvents.erase (i);
                  i = _preparedEvents.begin ();

                  g_debug ("AdapterApplicationPlayer::abort ALL forcing '%s' to "
                           "abort",
                           event->getId().c_str());
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
          g_debug ("Trying to abort '%s', but it is already sleeping",
                   _currentEvent->getId ().c_str());
        }
      else
        {
          g_debug ("Can't abort an already sleeping object = '%p' mrl = '%s'",
                   _object, _mrl.c_str());
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
AdapterFormatterPlayer::naturalEnd ()
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
  g_assert_nonnull (_player);

  if(_isAppPlayer)
    {
      map<string, NclFormatterEvent *>::iterator i;

      g_assert_nonnull (_object);

      if (_currentEvent == nullptr)
        {
          _manager->removePlayer (_object);
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
          _manager->removePlayer (_object);
          _preparedEvents.clear ();

          _object = NULL;
        }
      else
        {
          _object->unprepare ();

          i = _preparedEvents.find (_currentEvent->getId ());
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

      _manager->removePlayer (_object);
      Ginga_Display->unregisterEventListener (this);

      if (NclExecutionObject::hasInstance (_object, false))
        {
          _object->unprepare ();
        }

      _object = nullptr;

      return true;
    }
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
      if(_object->instanceOf("NclApplicationExecutionObject"))
        {
          if (!setCurrentEvent (event))
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
  g_assert_nonnull (_player);
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
AdapterFormatterPlayer::handleKeyEvent (SDL_EventType evtType,
                                        SDL_Keycode key)
{
  if(evtType == SDL_KEYDOWN)
    return;

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
AdapterFormatterPlayer::setCurrentEvent (NclFormatterEvent *event)
{
  string interfaceId;

  NclApplicationExecutionObject *appObject =
      dynamic_cast <NclApplicationExecutionObject *> (_object);
  g_assert_nonnull (appObject);

  if (_preparedEvents.count (event->getId ()) != 0
      && !event->instanceOf ("NclSelectionEvent")
      && event->instanceOf ("NclAnchorEvent"))
    {
      NclAnchorEvent *anchorEvent = dynamic_cast <NclAnchorEvent *> (event);
      g_assert_nonnull (anchorEvent);
      interfaceId = anchorEvent->getAnchor ()->getId ();

      LabeledAnchor *labeledAnchor
          = dynamic_cast<LabeledAnchor *> (anchorEvent->getAnchor());
      LambdaAnchor *lambdaAnchor
          = dynamic_cast<LambdaAnchor *> (anchorEvent->getAnchor());

      if (labeledAnchor)
        {
          interfaceId = labeledAnchor->getLabel();
        }
      else if(lambdaAnchor)
        {
          interfaceId = "";
        }

      _currentEvent = event;

      appObject->setCurrentEvent (_currentEvent);
      _player->setCurrentScope (interfaceId);
    }
  else if (event->instanceOf ("NclAttributionEvent"))
    {
      interfaceId = ((NclAttributionEvent *)event)
                        ->getAnchor ()
                        ->getPropertyName ();

      _player->setScope (interfaceId, Player::PL_TYPE_ATTRIBUTION);

      _currentEvent = event;
      appObject->setCurrentEvent (_currentEvent);

      _player->setCurrentScope (interfaceId);
    }
  else
    {
      g_warning ("Event '%s' isn't prepared",
                 event->getId ().c_str() );

      return false;
    }

  return true;
}

GINGA_FORMATTER_END
