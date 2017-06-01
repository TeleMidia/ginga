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
#include "AdapterApplicationPlayer.h"
#include "AdapterPlayerManager.h"

#include "mb/Display.h"
using namespace ::ginga::mb;

GINGA_PRAGMA_DIAG_IGNORE (-Wfloat-conversion)

GINGA_FORMATTER_BEGIN

AdapterApplicationPlayer::AdapterApplicationPlayer (AdapterPlayerManager *mngr)
    : AdapterFormatterPlayer (mngr)
{
  _currentEvent = nullptr;
}

AdapterApplicationPlayer::~AdapterApplicationPlayer ()
{
  _preparedEvents.clear ();
}

bool
AdapterApplicationPlayer::hasPrepared ()
{
  if (_object == NULL || _player == NULL)
    return false;

  return true;
}

bool
AdapterApplicationPlayer::prepare (NclExecutionObject *object,
                                   NclPresentationEvent *event)
{
  Content *content;
  double explicitDur;

  if (object == NULL)
    {
      g_warning ("Can't prepare a NULL object.");
      return false;
    }

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
          object->removeEvent (event);

          // the explicit duration is a property of
          // the object. Which means: start an interface with
          // begin = 4s an explicit duration = 5s => new duration
          // will be 1s
          presentationEvt->setEnd (explicitDur);

          /*
           * Adding event in object even though the it is added inside
           * application execution object prepare (we have to consider
           * that the event could be already prepared
           */
          object->addEvent (event);

          g_debug ("Object '%s' with explicitDur = '%f' object duration was"
                   "'%f'. Updated info: event begin=%f, event end=%f.",
                   object->getId ().c_str(),
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

void
AdapterApplicationPlayer::prepare (NclFormatterEvent *event)
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

bool
AdapterApplicationPlayer::stop ()
{
  map<string, NclFormatterEvent *>::iterator i;
  NclFormatterEvent *event;
  bool stopLambda = false;

  g_assert_nonnull (_object);
  g_assert_nonnull (_player);

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

bool
AdapterApplicationPlayer::abort ()
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
      g_debug ("an't abort an already sleeping object = '%p' mrl = '%s'",
               _object, _mrl.c_str());
    }
  return false;
}

void
AdapterApplicationPlayer::naturalEnd ()
{
  map<string, NclFormatterEvent *>::iterator i;
  NclFormatterEvent *event;

  clog << "AdapterApplicationPlayer::naturalEnd ";
  clog << endl;

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

bool
AdapterApplicationPlayer::unprepare ()
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

bool
AdapterApplicationPlayer::setCurrentEvent (NclFormatterEvent *event)
{
  string interfaceId;

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

      NclApplicationExecutionObject *appObject =
          dynamic_cast <NclApplicationExecutionObject *> (_object);
      g_assert_nonnull (appObject);

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
      ((NclApplicationExecutionObject *)_object)
          ->setCurrentEvent (_currentEvent);

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
