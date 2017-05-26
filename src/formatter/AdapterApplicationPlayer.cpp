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
  Thread::mutexInit (&_eventMutex, false);
  Thread::mutexInit (&_eventsMutex, false);

  _currentEvent = NULL;
  _running = false;

}

AdapterApplicationPlayer::~AdapterApplicationPlayer ()
{
  _running = false;
  for(auto status: _notes)
    {
      delete status;
    }
  _notes.clear ();

  lockPreparedEvents ();
  _preparedEvents.clear ();
  unlockPreparedEvents ();
  _currentEvent = NULL;
  _object = NULL;
  Thread::mutexDestroy (&_eventMutex);
  Thread::mutexDestroy (&_eventsMutex);
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
                                   NclFormatterEvent *event)
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
      lockPreparedEvents ();
      _preparedEvents.clear ();
      unlockPreparedEvents ();

      lockObject ();
      this->_object = object;
      unlockObject ();

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
          NclPresentationEvent *presentationEvt = dynamic_cast <NclPresentationEvent*> (event);
          g_assert_nonnull (presentationEvt);
          duration = presentationEvt->getDuration ();

          if (duration < IntervalAnchor::OBJECT_DURATION)
            {
              _player->setScope ("",
                                 IPlayer::TYPE_PRESENTATION,
                                 0.0,
                                 duration / 1000);
            }
        }
      else if (anchorEvent->getAnchor ()->instanceOf ("IntervalAnchor"))
        {
          IntervalAnchor *intervalAnchor
              = dynamic_cast<IntervalAnchor *>(anchorEvent->getAnchor ());
          g_assert_nonnull (intervalAnchor);

          _player->setScope (
              anchorEvent->getAnchor ()->getId (),
              IPlayer::TYPE_PRESENTATION,
              (intervalAnchor->getBegin () / 1000.0),
              (intervalAnchor->getEnd () / 1000.0));
        }
      else if (anchorEvent->getAnchor ()->instanceOf ("LabeledAnchor"))
        {
          NclPresentationEvent *presentationEvt = dynamic_cast <NclPresentationEvent*> (event);
          g_assert_nonnull (presentationEvt);
          duration = presentationEvt->getDuration ();
          LabeledAnchor *labeledAnchor = dynamic_cast <LabeledAnchor *> (anchorEvent->getAnchor());
          g_assert_nonnull (labeledAnchor);

          if (isnan (duration))
            {
              _player->setScope (
                  labeledAnchor->getLabel (),
                  IPlayer::TYPE_PRESENTATION);
            }
          else
            {
              _player->setScope (
                  labeledAnchor->getLabel (),
                  IPlayer::TYPE_PRESENTATION, 0.0, duration / 1000.0);
            }
        }
    }

  lockPreparedEvents ();
  _preparedEvents[event->getId ()] = event;
  unlockPreparedEvents ();
}

bool
AdapterApplicationPlayer::stop ()
{
  map<string, NclFormatterEvent *>::iterator i;
  NclFormatterEvent *event;
  bool stopLambda = false;

  g_assert_nonnull (_player);
  g_assert_nonnull (_object);

  if (_currentEvent != NULL)
    {
      NclAnchorEvent *anchorEvt = dynamic_cast <NclAnchorEvent *>(_currentEvent);
      if(anchorEvt && anchorEvt->getAnchor() != NULL && anchorEvt->getAnchor()->instanceOf("LambdaAnchor"))
        {
          stopLambda = true;
        }
    }

  if (stopLambda)
    {
      g_debug ("AdapterApplicationPlayer::stop ALL");

      lockPreparedEvents ();
      if (_currentEvent->getCurrentState () != EventUtil::ST_SLEEPING)
        {
          _player->stop ();
          _player->notifyReferPlayers (EventUtil::TR_STOPS);
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

      unlockPreparedEvents ();
    }
  else if (!_player->isForcedNaturalEnd ())
    {
      _player->stop ();
      _player->notifyReferPlayers (EventUtil::TR_STOPS);
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

  if (_currentEvent != NULL)
    {
      NclAnchorEvent *anchorEvt = dynamic_cast <NclAnchorEvent *>(_currentEvent);
      if(anchorEvt && anchorEvt->getAnchor() != NULL && anchorEvt->getAnchor()->instanceOf("LambdaAnchor"))
        {
          abortLambda = true;
        }
    }

  if (abortLambda)
    {
      g_debug ("AdapterApplicationPlayer::abort ALL");

      _player->stop ();
      _player->notifyReferPlayers (EventUtil::TR_ABORTS);

      lockPreparedEvents ();
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

      unlockPreparedEvents ();
    }
  else if (!_player->isForcedNaturalEnd ())
    {
      _player->stop ();
      _player->notifyReferPlayers (EventUtil::TR_ABORTS);
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

bool
AdapterApplicationPlayer::unprepare ()
{
  map<string, NclFormatterEvent *>::iterator i;

  if (_currentEvent == NULL)
    {
    

      if (_object != NULL)
        {
          _manager->removePlayer (_object);
          _object->unprepare ();
        }

      return true;
    }

  if (_currentEvent->getCurrentState () == EventUtil::ST_OCCURRING
      || _currentEvent->getCurrentState () == EventUtil::ST_PAUSED)
    {
      clog << "AdapterApplicationPlayer::unprepare stopping ";
      clog << "current event '" << _currentEvent->getId () << "'";
      clog << endl;

      _currentEvent->stop ();
    }

  lockPreparedEvents ();
  if (_preparedEvents.count (_currentEvent->getId ()) != 0
      && _preparedEvents.size () == 1)
    {
      if (_object != NULL)
        {
          _object->unprepare ();
          _manager->removePlayer (_object);
        }

      _preparedEvents.clear ();

      _object = NULL;
    }
  else
    {
      if (_object != NULL)
        {
          _object->unprepare ();
        }

      i = _preparedEvents.find (_currentEvent->getId ());
      if (i != _preparedEvents.end ())
        {
          _preparedEvents.erase (i);
        }

      clog << "AdapterApplicationPlayer::unprepare I still have '";
      clog << _preparedEvents.size () << "' prepared events" << endl;
    }

  unlockPreparedEvents ();

  return true;
}

void
AdapterApplicationPlayer::naturalEnd ()
{
  map<string, NclFormatterEvent *>::iterator i;
  NclFormatterEvent *event;

  clog << "AdapterApplicationPlayer::naturalEnd ";
  clog << endl;

  g_assert_nonnull (_player);
  _player->notifyReferPlayers (EventUtil::TR_STOPS);

  lockPreparedEvents ();
  i = _preparedEvents.begin ();
  while (i != _preparedEvents.end ())
    {
      event = i->second;
      if (event != NULL && event->instanceOf ("NclAnchorEvent")
          && ((NclAnchorEvent *)event)->getAnchor () != NULL
          && ((NclAnchorEvent *)event)
                 ->getAnchor ()
                 ->instanceOf ("LambdaAnchor"))
        {
          unlockPreparedEvents ();
          event->stop ();
          unprepare ();
          return;
        }
      ++i;
    }

  unlockPreparedEvents ();

  g_assert_nonnull (_object);
  if (_object->stop ())
    {
      unprepare ();
    }
}

void
AdapterApplicationPlayer::updateStatus (short code,
                                        const string &param,
                                        short type,
                                        const string &value)
{
  ApplicationStatus *data;

  data = new ApplicationStatus;
  data->code = code;
  data->param = param;
  data->type = type;
  data->value = value;

  if (!_running)
    {
      _running = true;
      //Thread::startThread ();
      run ();
    }

  _notes.push_back (data);
}

void
AdapterApplicationPlayer::notificationHandler (short code, const string &_param,
                                               short type, const string &value)
{
  string param = _param;

  clog << "AdapterApplicationPlayer::notificationHandler";
  clog << " with code = '" << code << "' param = '";
  clog << param << "'";
  clog << " type = '" << type << "' and value = '";
  clog << value << "'";
  clog << endl;

  switch (code)
    {
    case IPlayer::PL_NOTIFY_START:
      clog << "AdapterApplicationPlayer::notificationHandler";
      clog << " call startEvent '" << param << "'";
      clog << " type '" << type << "'";
      clog << endl;

      startEvent (param, type, value);
      break;

    case IPlayer::PL_NOTIFY_PAUSE:
      pauseEvent (param, type);
      break;

    case IPlayer::PL_NOTIFY_RESUME:
      resumeEvent (param, type);
      break;

    case IPlayer::PL_NOTIFY_STOP:
      if (param == "")
        {
          clog << "AdapterApplicationPlayer::notificationHandler";
          clog << " call naturalEnd '" << param << "'";
          clog << " type '" << type << "'";
          clog << endl;
          naturalEnd ();
        }
      else
        {
          clog << "AdapterApplicationPlayer::notificationHandler";
          clog << " call stopEvent '" << param << "'";
          clog << " type '" << type << "'";
          clog << endl;
          stopEvent (param, type, value);
        }
      break;

    case IPlayer::PL_NOTIFY_ABORT:
      abortEvent (param, type);
      break;

    default:
      clog << "AdapterApplicationPlayer::notificationHandler";
      clog << " Warning! Received an unknown notification";
      clog << endl;
      break;
    }
}

void
AdapterApplicationPlayer::run ()
{
  short code = 0;
  string param;
  short type;
  string value;
  ApplicationStatus *data;

  while (_running)
    {
      if (!_notes.empty ())
        {
          data = *_notes.begin ();

          code = data->code;
          param = data->param;
          type = data->type;
          value = data->value;

          delete data;
          data = NULL;
          _notes.erase (_notes.begin ());
        }
      else
        {
          code = -1;
        }

      if (_object == NULL)
        {
          break;
        }

      if (code >= 0)
        {
          notificationHandler (code, param, type, value);
        }

      code = -1;
    }
}

bool
AdapterApplicationPlayer::checkEvent (NclFormatterEvent *event, short type)
{
  bool isPresentation;
  bool isAttribution;

  g_assert_nonnull (event);
  isPresentation = event->instanceOf ("NclPresentationEvent")
      && type == IPlayer::TYPE_PRESENTATION;

  isAttribution = event->instanceOf ("NclAttributionEvent")
      && type == IPlayer::TYPE_ATTRIBUTION;

  return (isPresentation || isAttribution);
}

bool
AdapterApplicationPlayer::startEvent (const string &anchorId, short type,
                                      const string &value)
{
  NclFormatterEvent *event;
  bool fakeStart = false;

  event = _object->getEventFromAnchorId (anchorId);
  if (checkEvent (event, type))
    {
      if (prepare (_object, event))
        {
          if (setAndLockCurrentEvent (event))
            {
              if (type == IPlayer::TYPE_PRESENTATION)
                {
                  fakeStart = _object->start ();
                  unlockCurrentEvent (event);
                }
              else
                {
                  fakeStart = event->start ();
                  ((NclAttributionEvent *)event)->setValue (value);
                  unlockCurrentEvent (event);

                  /*if (hasPrepared()) {
                          setPropertyValue(
                                          (NclAttributionEvent*)event,
                  value);

                          player->setPropertyValue(anchorId, value);

                  } else {
                          object->setPropertyValue(
                                          (NclAttributionEvent*)event,
                  value);
                  }*/
                }
            }
        }
      else
        {
          g_warning ("Can't prepare '%s' form anchor id = '%s'",
                     event->getId().c_str(),
                     anchorId.c_str ());
        }
    }
  else
    {
      g_warning ("Event not found '%s' in object '%s'",
                 anchorId.c_str(),
                 _object->getId().c_str());
    }

  return fakeStart;
}

bool
AdapterApplicationPlayer::stopEvent (const string &anchorId, short type,
                                     const string &value)
{
  NclFormatterEvent *event;

  if (_object->getId () == anchorId)
    {
      clog << "AdapterApplicationPlayer::stopEvent ";
      clog << " considering anchor '";
      clog << anchorId << "' will call naturalEnd" << endl;
      naturalEnd ();
      return false;
    }

  event = _object->getEventFromAnchorId (anchorId);
  if (checkEvent (event, type))
    {
      if (setAndLockCurrentEvent (event))
        {
          if (type == IPlayer::TYPE_PRESENTATION)
            {
              if (_object->stop ())
                {
                  unprepare ();
                  unlockCurrentEvent (event);
                  return true;
                }
            }
          else
            {
              ((NclAttributionEvent *)event)->setValue (value);
              if (event->stop ())
                {
                  unprepare ();
                  unlockCurrentEvent (event);

                  /*if (hasPrepared()) {
                          setPropertyValue(
                                          (NclAttributionEvent*)event,
                  value);

                          player->setPropertyValue(anchorId, value);

                  } else {
                          object->setPropertyValue(
                                          (NclAttributionEvent*)event,
                  value);
                  }*/
                  return true;
                }
            }
          unlockCurrentEvent (event);
        }
    }
  else
    {
      g_warning ("Can't stop event '%s'",
                 anchorId.c_str());
    }

  return false;
}

bool
AdapterApplicationPlayer::abortEvent (const string &anchorId, short type)
{
  NclFormatterEvent *event;
  string cvt_id = anchorId;

  if (_object->getId () == anchorId)
    {
      cvt_id = "";
    }

  event = _object->getEventFromAnchorId (cvt_id);
  if (checkEvent (event, type))
    {
      if (setAndLockCurrentEvent (event))
        {
          if (type == IPlayer::TYPE_PRESENTATION)
            {
              if (_object->abort ())
                {
                  unprepare ();
                  unlockCurrentEvent (event);
                  return true;
                }
            }
          else
            {
              if (event->abort ())
                {
                  unprepare ();
                  unlockCurrentEvent (event);

                  /*if (hasPrepared()) {
                          setPropertyValue(
                                          (NclAttributionEvent*)event,
                  value);

                          player->setPropertyValue(anchorId, value);

                  } else {
                          object->setPropertyValue(
                                          (NclAttributionEvent*)event,
                  value);
                  }*/
                  return true;
                }
            }
          unlockCurrentEvent (event);
        }
    }
  else
    {
      g_warning ("Can't abort event '%s'",
                 anchorId.c_str());
    }

  return false;
}

bool
AdapterApplicationPlayer::pauseEvent (const string &anchorId, short type)
{
  NclFormatterEvent *event;
  string cvt_id = anchorId;

  if (_object->getId () == anchorId)
    {
      cvt_id = "";
    }

  event = _object->getEventFromAnchorId (cvt_id);
  if (checkEvent (event, type))
    {
      if (setAndLockCurrentEvent (event))
        {
          if (type == IPlayer::TYPE_PRESENTATION)
            {
              if (_object->pause ())
                {
                  unlockCurrentEvent (event);
                  return true;
                }
            }
          else
            {
              if (event->pause ())
                {
                  unlockCurrentEvent (event);
                  return true;
                }
            }
          unlockCurrentEvent (event);
        }
    }
  else
    {
      g_warning ("Can't pause event '%s'",
                 anchorId.c_str());
    }

  return false;
}

bool
AdapterApplicationPlayer::resumeEvent (const string &anchorId, short type)
{
  NclFormatterEvent *event;
  string cvt_id = anchorId;

  if (_object->getId () == anchorId)
    {
      cvt_id = "";
    }

  event = _object->getEventFromAnchorId (cvt_id);
  if (checkEvent (event, type))
    {
      if (setAndLockCurrentEvent (event))
        {
          if (type == IPlayer::TYPE_PRESENTATION)
            {
              if (_object->resume ())
                {
                  unlockCurrentEvent (event);
                  return true;
                }
            }
          else
            {
              if (event->resume ())
                {
                  unlockCurrentEvent (event);
                  return true;
                }
            }
          unlockCurrentEvent (event);
        }
    }
  else
    {
      g_warning ("Can't resume event '%s'",
                 anchorId.c_str());
    }

  return false;
}

void
AdapterApplicationPlayer::unlockCurrentEvent (NclFormatterEvent *event)
{
  if (event != _currentEvent)
    {
      string id = "";

      if (_currentEvent != NULL)
        {
          id = _currentEvent->getId ();
        }
      g_warning ("Handling events warning! id = '%s'",
                 id.c_str());
    }
  unlockEvent ();
}

bool
AdapterApplicationPlayer::setAndLockCurrentEvent (NclFormatterEvent *event)
{
  string interfaceId;

  lockEvent ();
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

      _player->setScope (interfaceId, IPlayer::TYPE_ATTRIBUTION);

      _currentEvent = event;
      ((NclApplicationExecutionObject *)_object)
          ->setCurrentEvent (_currentEvent);

      _player->setCurrentScope (interfaceId);
    }
  else
    {
      g_warning ("Event '%s' isn't prepared",
                 event->getId ().c_str() );

      unlockEvent ();
      return false;
    }

  return true;
}

void
AdapterApplicationPlayer::lockEvent ()
{
  Thread::mutexLock (&_eventMutex);
}

void
AdapterApplicationPlayer::unlockEvent ()
{
  Thread::mutexUnlock (&_eventMutex);
}

void
AdapterApplicationPlayer::lockPreparedEvents ()
{
  Thread::mutexLock (&_eventsMutex);
}

void
AdapterApplicationPlayer::unlockPreparedEvents ()
{
  Thread::mutexUnlock (&_eventsMutex);
}

GINGA_FORMATTER_END
