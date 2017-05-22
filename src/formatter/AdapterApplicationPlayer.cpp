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

GINGA_FORMATTER_BEGIN

AdapterApplicationPlayer::AdapterApplicationPlayer ()
    : AdapterFormatterPlayer ()
{
  Thread::mutexInit (&eventMutex, false);
  Thread::mutexInit (&eventsMutex, false);

  currentEvent = NULL;
  running = false;
  isDeleting = false;

  clog << "AdapterApplicationPlayer::AdapterApplicationPlayer(" << this;
  clog << ")" << endl;
}

AdapterApplicationPlayer::~AdapterApplicationPlayer ()
{
  vector<ApplicationStatus *>::iterator i;

  clog << "AdapterApplicationPlayer::AdapterApplicationPlayer(" << this;
  clog << ")" << endl;

  isDeleting = true;
  running = false;
  unlockConditionSatisfied ();

  lock ();
  i = notes.begin ();
  while (i != notes.end ())
    {
      delete (*i);
      ++i;
    }
  notes.clear ();
  unlock ();

  lockPreparedEvents ();
  preparedEvents.clear ();
  unlockPreparedEvents ();
  currentEvent = NULL;
  _object = NULL;
  Thread::mutexDestroy (&eventMutex);
  Thread::mutexDestroy (&eventsMutex);
}

void
AdapterApplicationPlayer::createPlayer ()
{
  AdapterFormatterPlayer::createPlayer ();
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
  NclCascadingDescriptor *descriptor;
  double explicitDur;

  if (object == NULL)
    {
      clog << "AdapterApplicationPlayer::prepare(";
      clog << this << ") Warning! Can't prepare NULL object" << endl;

      return false;
    }

  if (this->_object != object)
    {
      lockPreparedEvents ();
      preparedEvents.clear ();
      unlockPreparedEvents ();

      lockObject ();
      this->_object = object;
      unlockObject ();

      if (this->_object->getDataObject () != NULL
          && this->_object->getDataObject ()->getDataEntity () != NULL)
        {
          content
              = ((NodeEntity *)(object->getDataObject ()->getDataEntity ()))
                    ->getContent ();

          if (content != NULL && content->instanceOf ("ReferenceContent"))
            {
              this->_mrl = ((ReferenceContent *)content)
                              ->getCompleteReferenceUrl ();
            }
          else
            {
              this->_mrl = "";
            }
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

  if (event->instanceOf ("NclPresentationEvent"))
    {
      double duration = ((NclPresentationEvent *)event)->getDuration ();
      bool infDur = (isnan (duration) || isinf (duration));

      if (explicitDur < 0)
        {
          descriptor = object->getDescriptor ();
          if (descriptor != NULL)
            {
              explicitDur = descriptor->getExplicitDuration ();
            }
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

          // the explicit duration is a property of
          // the object. Which means: start an interface with
          // begin = 4s an explicit duration = 5s => new duration
          // will be 1s
          ((NclPresentationEvent *)event)->setEnd (explicitDur);

          /*
           * Adding event in object even though the it is added inside
           * application execution object prepare (we have to consider
           * that the event could be already prepared
           */
          object->addEvent (event);

          clog << "AdapterApplicationPlayer::prepare '";
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
      clog << "AdapterApplicationPlayer::prepare event '";
      clog << event->getId () << "' is not sleeping!" << endl;

      return false;
    }
}

void
AdapterApplicationPlayer::prepare (NclFormatterEvent *event)
{
  double duration;
  IntervalAnchor *intervalAnchor;
  NclCascadingDescriptor *descriptor;
  LayoutRegion *region;

  descriptor = _object->getDescriptor ();
  if (descriptor != NULL)
    {
      region = descriptor->getRegion ();
      // the player has NULL address if it is a remote one
      if (region != NULL && _player != NULL)
        {
          _player->setNotifyContentUpdate (region->getDeviceClass () == 1);
        }
    }

  if (_player != NULL && event->instanceOf ("NclAnchorEvent"))
    {
      if ((((NclAnchorEvent *)event)->getAnchor ())
              ->instanceOf ("LambdaAnchor"))
        {
          duration = ((NclPresentationEvent *)event)->getDuration ();

          if (duration < IntervalAnchor::OBJECT_DURATION)
            {
              _player->setScope ("", IPlayer::TYPE_PRESENTATION, 0.0,
                                duration / 1000);
            }
        }
      else if (((((NclAnchorEvent *)event)->getAnchor ()))
                   ->instanceOf ("IntervalAnchor"))
        {
          intervalAnchor
              = (IntervalAnchor *)(((NclAnchorEvent *)event)->getAnchor ());

          _player->setScope (
              ((NclAnchorEvent *)event)->getAnchor ()->getId (),
              IPlayer::TYPE_PRESENTATION,
              (intervalAnchor->getBegin () / 1000),
              (intervalAnchor->getEnd () / 1000));
        }
      else if (((((NclAnchorEvent *)event)->getAnchor ()))
                   ->instanceOf ("LabeledAnchor"))
        {
          duration = ((NclPresentationEvent *)event)->getDuration ();

          clog << "AdapterApplicationPlayer::prepare '" << _object->getId ();
          clog << "' with dur = '" << duration << "'" << endl;

          if (isnan (duration))
            {
              _player->setScope (
                  ((LabeledAnchor *)((NclAnchorEvent *)event)->getAnchor ())
                      ->getLabel (),
                  IPlayer::TYPE_PRESENTATION);
            }
          else
            {
              _player->setScope (
                  ((LabeledAnchor *)((NclAnchorEvent *)event)->getAnchor ())
                      ->getLabel (),
                  IPlayer::TYPE_PRESENTATION, 0.0, duration / 1000);
            }
        }
    }

  lockPreparedEvents ();
  preparedEvents[event->getId ()] = event;
  unlockPreparedEvents ();
}

bool
AdapterApplicationPlayer::start ()
{
  bool startSuccess = false;

  clog << "AdapterApplicationPlayer::start ";
  clog << endl;

  if (_player != NULL)
    {
      startSuccess = _player->play ();
    }
  if ((startSuccess) || (_objectDevice == 2))
    { // DeviceDomain::CT_ACTIVE
      // clog << "AdapterApplicationPlayer::play objectDevice" <<
      // objectDevice
      // << endl;
      if (_object != NULL && !_object->start ())
        {
          if (_player != NULL)
            {
              _player->stop ();
            }
          startSuccess = false;
        }
      else
        {
          checkAnchorMonitor ();
        }

      return startSuccess;
    }
 
  return false;
}

bool
AdapterApplicationPlayer::stop ()
{
  map<string, NclFormatterEvent *>::iterator i;
  NclFormatterEvent *event;
  bool stopLambda = false;

  if (currentEvent != NULL && currentEvent->instanceOf ("NclAnchorEvent")
      && ((NclAnchorEvent *)currentEvent)->getAnchor () != NULL
      && ((NclAnchorEvent *)currentEvent)
             ->getAnchor ()
             ->instanceOf ("LambdaAnchor"))
    {
      stopLambda = true;
    }

  if (stopLambda)
    {
      clog << "AdapterApplicationPlayer::stop ALL" << endl;

      lockPreparedEvents ();
      if (currentEvent->getCurrentState () != EventUtil::ST_SLEEPING
          && _player != NULL)
        {
          _player->stop ();
          _player->notifyReferPlayers (EventUtil::TR_STOPS);
        }

      i = preparedEvents.begin ();
      while (i != preparedEvents.end ())
        {
          event = i->second;
          if (event != currentEvent
              && event->getCurrentState () != EventUtil::ST_SLEEPING)
            {
              preparedEvents.erase (i);
              i = preparedEvents.begin ();

              clog << "AdapterApplicationPlayer::stop ALL forcing '";
              clog << event->getId () << "' to stop" << endl;
              event->stop ();
            }
          else
            {
              ++i;
            }
        }

      unlockPreparedEvents ();
    }
  else if (_player != NULL && !_player->isForcedNaturalEnd ())
    {
      clog << "AdapterApplicationPlayer::stop calling stop player";
      clog << endl;

      _player->stop ();
      _player->notifyReferPlayers (EventUtil::TR_STOPS);
    }

  if (_object != NULL && _object->stop ())
    {
      clog << "AdapterApplicationPlayer::stop calling unprepare";
      clog << endl;

      unprepare ();
      return true;
    }

  if (stopLambda && !currentEvent->stop ())
    {
      clog << "AdapterApplicationPlayer::stop '";
      clog << currentEvent->getId () << "' is already sleeping";
      clog << endl;
    }
  else
    {
      clog << "AdapterApplicationPlayer::stop(" << this;
      clog << ") Can't stop an already stopped object = '";
      clog << _object << "'. mrl = '" << _mrl << "' device class = '";
      clog << getObjectDevice () << "'" << endl;
    }
  return false;
}

bool
AdapterApplicationPlayer::pause ()
{
  if (_object != NULL && _object->pause ())
    {
      if (_player != NULL)
        {
          _player->pause ();
          _player->notifyReferPlayers (EventUtil::TR_PAUSES);
        }
      return true;
    }
  else
    {
      return false;
    }
}

bool
AdapterApplicationPlayer::resume ()
{
  if (_object != NULL && _object->resume ())
    {
      if (_player != NULL)
        {
          _player->resume ();
          _player->notifyReferPlayers (EventUtil::TR_RESUMES);
        }
      return true;
    }
  return false;
}

bool
AdapterApplicationPlayer::abort ()
{
  map<string, NclFormatterEvent *>::iterator i;
  NclFormatterEvent *event;
  bool abortLambda = false;

  if (currentEvent != NULL && currentEvent->instanceOf ("NclAnchorEvent")
      && ((NclAnchorEvent *)currentEvent)->getAnchor () != NULL
      && ((NclAnchorEvent *)currentEvent)
             ->getAnchor ()
             ->instanceOf ("LambdaAnchor"))
    {
      abortLambda = true;
    }

  if (abortLambda)
    {
      clog << "AdapterApplicationPlayer::abort ALL" << endl;

      _player->stop ();
      _player->notifyReferPlayers (EventUtil::TR_ABORTS);

      lockPreparedEvents ();
      i = preparedEvents.begin ();
      while (i != preparedEvents.end ())
        {
          event = i->second;
          if (event != currentEvent
              && event->getCurrentState () != EventUtil::ST_SLEEPING)
            {
              preparedEvents.erase (i);
              i = preparedEvents.begin ();

              clog << "AdapterApplicationPlayer::abort ALL forcing '";
              clog << event->getId () << "' to abort" << endl;
              event->abort ();
            }
          else
            {
              ++i;
            }
        }

      unlockPreparedEvents ();
    }
  else if (_player != NULL && !_player->isForcedNaturalEnd ())
    {
      clog << "AdapterApplicationPlayer::abort calling stop player";
      clog << endl;

      _player->stop ();
      _player->notifyReferPlayers (EventUtil::TR_ABORTS);
    }

  if (_object != NULL && _object->abort ())
    {
      clog << "AdapterApplicationPlayer::abort calling unprepare";
      clog << endl;

      unprepare ();
      return true;
    }

  if (abortLambda && !currentEvent->abort ())
    {
      clog << "AdapterApplicationPlayer::abort '";
      clog << currentEvent->getId () << "' is already sleeping";
      clog << endl;
    }
  else
    {
      clog << "AdapterApplicationPlayer::abort(" << this;
      clog << ") Can't abort an already sleeping object = '";
      clog << _object << "'. mrl = '" << _mrl << "' device class = '";
      clog << getObjectDevice () << "'" << endl;
    }
  return false;
}

bool
AdapterApplicationPlayer::unprepare ()
{
  map<string, NclFormatterEvent *>::iterator i;

  clog << "AdapterApplicationPlayer::unprepare ";
  clog << endl;

  if (currentEvent == NULL)
    {
    

      if (_object != NULL)
        {
          _manager->removePlayer (_object);
          _object->unprepare ();
        }

      return true;
    }

  if (currentEvent->getCurrentState () == EventUtil::ST_OCCURRING
      || currentEvent->getCurrentState () == EventUtil::ST_PAUSED)
    {
      clog << "AdapterApplicationPlayer::unprepare stopping ";
      clog << "current event '" << currentEvent->getId () << "'";
      clog << endl;

      currentEvent->stop ();
    }

  lockPreparedEvents ();
  if (preparedEvents.count (currentEvent->getId ()) != 0
      && preparedEvents.size () == 1)
    {
      if (_object != NULL)
        {
          _object->unprepare ();
          _manager->removePlayer (_object);
        }

      preparedEvents.clear ();

      _object = NULL;
    }
  else
    {
      if (_object != NULL)
        {
          _object->unprepare ();
        }

      i = preparedEvents.find (currentEvent->getId ());
      if (i != preparedEvents.end ())
        {
          preparedEvents.erase (i);
        }

      clog << "AdapterApplicationPlayer::unprepare I still have '";
      clog << preparedEvents.size () << "' prepared events" << endl;
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

  if (_player != NULL)
    {
      _player->notifyReferPlayers (EventUtil::TR_STOPS);
    }

  lockPreparedEvents ();
  i = preparedEvents.begin ();
  while (i != preparedEvents.end ())
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

  if (_object != NULL && _object->stop ())
    {
      clog << "AdapterApplicationPlayer::naturalEnd call unprepare";
      clog << endl;
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

  lock ();
  data = new ApplicationStatus;
  data->code = code;
  data->param = param;
  data->type = type;
  data->value = value;

  if (!running)
    {
      running = true;
      Thread::startThread ();
    }

  notes.push_back (data);

  unlock ();

  unlockConditionSatisfied ();
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

  while (running)
    {
      lock ();
      if (!notes.empty ())
        {
          data = *notes.begin ();

          code = data->code;
          param = data->param;
          type = data->type;
          value = data->value;

          delete data;
          data = NULL;
          notes.erase (notes.begin ());
        }
      else
        {
          code = -1;
        }
      unlock ();

      if (_object == NULL)
        {
          break;
        }

      if (code >= 0)
        {
          notificationHandler (code, param, type, value);
        }

      code = -1;

      lock ();
      if (notes.empty () && running && !isDeleting)
        {
          unlock ();
          waitForUnlockCondition ();
        }
      else
        {
          unlock ();
        }
    }
}

bool
AdapterApplicationPlayer::checkEvent (NclFormatterEvent *event, short type)
{
  bool isPresentation;
  bool isAttribution;

  if (event != NULL)
    {
      isPresentation = event->instanceOf ("NclPresentationEvent")
                       && type == IPlayer::TYPE_PRESENTATION;

      isAttribution = event->instanceOf ("NclAttributionEvent")
                      && type == IPlayer::TYPE_ATTRIBUTION;

      if (isPresentation || isAttribution)
        {
          return true;
        }
    }

  return false;
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
          /*clog << "AdapterApplicationPlayer::startEvent '";
          clog << event->getId() << "' with anchorId = '";
          clog << anchorId << "'" << endl;*/

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
          clog << "AdapterApplicationPlayer::startEvent can't ";
          clog << "prepare '" << event->getId () << "' from ";
          clog << "anchor ID = '" << anchorId << "'" << endl;
        }
    }
  else
    {
      clog << "AdapterApplicationPlayer::startEvent event not found '";
      clog << anchorId;
      clog << "' in object '" << _object->getId ();
      clog << endl;
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
      clog << "AdapterApplicationPlayer::stopEvent '";
      clog << event->getId ();
      clog << "' from anchorId = '";
      clog << anchorId << "' calling setAndLockCurrentEvent";
      clog << endl;

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
      clog << "AdapterApplicationPlayer::stopEvent can't stop event";
      clog << "'" << anchorId << "'";
      clog << endl;
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
      clog << "AdapterApplicationPlayer::abortEvent '";
      clog << event->getId ();
      clog << "' from anchorId = '";
      clog << anchorId << "' calling setAndLockCurrentEvent";
      clog << endl;

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
      clog << "AdapterApplicationPlayer::abortEvent can't abort event";
      clog << "'" << anchorId << "'";
      clog << endl;
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
      clog << "AdapterApplicationPlayer::pauseEvent '";
      clog << event->getId ();
      clog << "' from anchorId = '";
      clog << anchorId << "' calling setAndLockCurrentEvent";
      clog << endl;

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
      clog << "AdapterApplicationPlayer::pauseEvent can't pause event";
      clog << "'" << anchorId << "'";
      clog << endl;
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
      clog << "AdapterApplicationPlayer::resumeEvent '";
      clog << event->getId ();
      clog << "' from anchorId = '";
      clog << anchorId << "' calling setAndLockCurrentEvent";
      clog << endl;

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
      clog << "AdapterApplicationPlayer::resumeEvent can't resume event";
      clog << "'" << anchorId << "'";
      clog << endl;
    }

  return false;
}

void
AdapterApplicationPlayer::lockEvent ()
{
  Thread::mutexLock (&eventMutex);
}

void
AdapterApplicationPlayer::unlockEvent ()
{
  Thread::mutexUnlock (&eventMutex);
}

void
AdapterApplicationPlayer::lockPreparedEvents ()
{
  Thread::mutexLock (&eventsMutex);
}

void
AdapterApplicationPlayer::unlockPreparedEvents ()
{
  Thread::mutexUnlock (&eventsMutex);
}

GINGA_FORMATTER_END
