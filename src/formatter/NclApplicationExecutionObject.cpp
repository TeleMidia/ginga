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
#include "NclApplicationExecutionObject.h"

GINGA_PRAGMA_DIAG_IGNORE (-Wsign-conversion)

GINGA_FORMATTER_BEGIN

NclApplicationExecutionObject::NclApplicationExecutionObject (
    const string &id, Node *node, NclCascadingDescriptor *descriptor,
    bool handling, INclLinkActionListener *seListener)
    : NclExecutionObject (id, node, descriptor, handling, seListener)
{
  _typeSet.insert ("NclApplicationExecutionObject");
  _currentEvent = NULL;
}

NclApplicationExecutionObject::~NclApplicationExecutionObject ()
{
  map<string, NclFormatterEvent *>::iterator i;

  NclExecutionObject::removeInstance (this);

  i = _preparedEvents.begin ();
  while (i != _preparedEvents.end ())
    {
      removeParentListenersFromEvent (i->second);
      ++i;
    }

  unprepare ();
  unprepareEvents ();
  removeEventListeners ();

  destroyEvents ();
}

bool
NclApplicationExecutionObject::isSleeping ()
{
  map<string, NclFormatterEvent *>::iterator i;

  lockEvents ();
  i = _preparedEvents.begin ();
  while (i != _preparedEvents.end ())
    {
      if (i->second->getCurrentState () != EventUtil::ST_SLEEPING)
        {
          unlockEvents ();
          return false;
        }
      ++i;
    }
  unlockEvents ();
  return true;
}

bool
NclApplicationExecutionObject::isPaused ()
{
  map<string, NclFormatterEvent *>::iterator i;
  NclFormatterEvent *event;
  bool hasPaused = false;

  if (_currentEvent != NULL)
    {
      if (_currentEvent->getCurrentState () == EventUtil::ST_OCCURRING)
        {
          return false;
        }

      if (_currentEvent->getCurrentState () == EventUtil::ST_PAUSED)
        {
          hasPaused = true;
        }
    }

  lockEvents ();
  i = _preparedEvents.begin ();
  while (i != _preparedEvents.end ())
    {
      event = i->second;
      if (event->getCurrentState () == EventUtil::ST_OCCURRING)
        {
          unlockEvents ();
          return false;
        }

      if (event->getCurrentState () == EventUtil::ST_PAUSED)
        {
          hasPaused = true;
        }
      ++i;
    }

  unlockEvents ();
  return hasPaused;
}

NclFormatterEvent *
NclApplicationExecutionObject::getCurrentEvent ()
{
  return _currentEvent;
}

bool
NclApplicationExecutionObject::hasPreparedEvent (NclFormatterEvent *event)
{
  map<string, NclFormatterEvent *>::iterator i;

  lockEvents ();
  i = _preparedEvents.begin ();
  while (i != _preparedEvents.end ())
    {
      if (i->second != event)
        {
          unlockEvents ();
          return true;
        }
      ++i;
    }

  unlockEvents ();
  return false;
}

void
NclApplicationExecutionObject::setCurrentEvent (NclFormatterEvent *event)
{
  if (!containsEvent (event))
    {
      _currentEvent = NULL;
    }
  else
    {
      _currentEvent = event;
    }
}

bool
NclApplicationExecutionObject::prepare (NclFormatterEvent *event,
                                        GingaTime offsetTime)
{
  int size;
  map<Node *, NclCompositeExecutionObject *>::iterator i;
  GingaTime startTime = 0;
  ContentAnchor *contentAnchor;
  NclFormatterEvent *auxEvent;
  NclAttributionEvent *attributeEvent;
  PropertyAnchor *attributeAnchor;
  int j;

  if (event->getCurrentState () != EventUtil::ST_SLEEPING)
    {
      clog << "NclApplicationExecutionObject::prepare can't prepare '";
      clog << event->getId () << "': event isn't sleeping" << endl;
      return false;
    }

  if (event->instanceOf ("NclPresentationEvent"))
    {
      GingaTime duration = ((NclPresentationEvent *)event)->getDuration ();
      if (duration <= 0)
        {
          clog << "NclApplicationExecutionObject::prepare can't prepare '";
          clog << event->getId () << "': event duration <= 0" << endl;
          return false;
        }
    }

  addEvent (event);
  if (event->instanceOf ("NclAnchorEvent"))
    {
      contentAnchor = ((NclAnchorEvent *)event)->getAnchor ();
      if (contentAnchor != NULL
          && contentAnchor->instanceOf ("LabeledAnchor"))
        {
          i = _parentTable.begin ();
          while (i != _parentTable.end ())
            {
              // register parent as a mainEvent listener
              event->addEventListener (i->second);

              ++i;
            }

          lockEvents ();
          _preparedEvents[event->getId ()] = event;
          unlockEvents ();
          return true;
        }
    }

  if (event->instanceOf ("NclPresentationEvent"))
    {
      startTime = ((NclPresentationEvent *)event)->getBegin () + offsetTime;
      if (startTime > ((NclPresentationEvent *)event)->getEnd ())
        {
          clog << "NclApplicationExecutionObject::prepare skipping '";
          clog << event->getId () << "': past event (start = '";
          clog << startTime << "'; end = '";
          clog << ((NclPresentationEvent *)event)->getEnd () << "'";
          clog << endl;
          return false;
        }
    }

  i = _parentTable.begin ();
  while (i != _parentTable.end ())
    {
      // register parent as a currentEvent listener
      event->addEventListener (i->second);
      ++i;
    }

  _transMan->prepare (event == _wholeContent, startTime);

  size = (int) _otherEvents.size ();
  for (j = 0; j < size; j++)
    {
      auxEvent = _otherEvents[j];
      if (auxEvent->instanceOf ("NclAttributionEvent"))
        {
          attributeEvent = (NclAttributionEvent *)auxEvent;
          attributeAnchor = attributeEvent->getAnchor ();
          if (attributeAnchor->getValue () != "")
            {
              attributeEvent->setValue (attributeAnchor->getValue ());
            }
        }
    }

  this->_offsetTime = startTime;

  lockEvents ();
  _preparedEvents[event->getId ()] = event;
  unlockEvents ();

  return true;
}

bool
NclApplicationExecutionObject::start ()
{
  ContentAnchor *contentAnchor;

  lockEvents ();
  if (_currentEvent == NULL
      || _preparedEvents.count (_currentEvent->getId ()) == 0)
    {
      clog << "NclApplicationExecutionObject::start nothing to do!" << endl;
      unlockEvents ();
      return false;
    }

  unlockEvents ();

  /*
   * TODO: follow the event state machine or start instruction behavior
   * if (currentEvent->getCurrentState() == EventUtil::ST_PAUSED) {
          return resume();
  }*/

  if (_currentEvent->getCurrentState () != EventUtil::ST_SLEEPING)
    {
      clog << "NclApplicationExecutionObject::start current event '";
      clog << _currentEvent->getId () << "' is already running!" << endl;
      return false;
    }

  if (_currentEvent->instanceOf ("NclAnchorEvent"))
    {
      contentAnchor = ((NclAnchorEvent *)_currentEvent)->getAnchor ();
      if (contentAnchor != NULL
          && (contentAnchor->instanceOf ("LabeledAnchor")))
        {
          _transMan->start (_offsetTime);
          _currentEvent->start ();

          clog << "NclApplicationExecutionObject::start current event '";
          clog << _currentEvent->getId () << "' started!" << endl;
          return true;
        }
    }

  clog << "NclApplicationExecutionObject::start starting transition ";
  clog << "manager!" << endl;
  _transMan->start (_offsetTime);

  if (_currentEvent->getCurrentState () != EventUtil::ST_SLEEPING)
    {
      clog << "NclApplicationExecutionObject::start YEAP! ";
      clog << "Current event is running!" << endl;
    }
  return true;
}

NclEventTransition *
NclApplicationExecutionObject::getNextTransition ()
{
  if (_currentEvent == NULL
      || _currentEvent->getCurrentState () == EventUtil::ST_SLEEPING
      || !_currentEvent->instanceOf ("NclPresentationEvent"))
    {
      return NULL;
    }

  return _transMan->getNextTransition (_currentEvent);
}

bool
NclApplicationExecutionObject::stop ()
{
  ContentAnchor *contentAnchor;
  GingaTime endTime;
  bool isLabeled = false;

  if (isSleeping ())
    {
      if (_wholeContent != NULL
          && _wholeContent->getCurrentState () != EventUtil::ST_SLEEPING)
        {
          clog << "NclApplicationExecutionObject::stop WHOLECONTENT"
               << endl;
          _wholeContent->stop ();
        }
      return false;
    }

  if (_currentEvent->instanceOf ("NclAnchorEvent"))
    {
      contentAnchor = ((NclAnchorEvent *)_currentEvent)->getAnchor ();
      if (contentAnchor != NULL
          && contentAnchor->instanceOf ("LabeledAnchor"))
        {
          isLabeled = true;

          clog << "NclApplicationExecutionObject::stop stopping event '";
          clog << contentAnchor->getId () << "'" << endl;
          _currentEvent->stop ();
        }
    }

  if (!isLabeled && _currentEvent->instanceOf ("NclPresentationEvent"))
    {
      endTime = ((NclPresentationEvent *)_currentEvent)->getEnd ();
      _currentEvent->stop ();
      if (endTime > 0)
        {
          _transMan->stop (endTime, true);
        }
    }

  _transMan->resetTimeIndex ();
  _pauseCount = 0;

  return true;
}

bool
NclApplicationExecutionObject::abort ()
{
  vector<NclFormatterEvent *>::iterator i;
  NclFormatterEvent *ev;
  ContentAnchor *contentAnchor;
  GingaTime endTime;
  bool isLabeled = false;

  if (isSleeping ())
    {
      if (_wholeContent != NULL
          && _wholeContent->getCurrentState () != EventUtil::ST_SLEEPING)
        {
          _wholeContent->abort ();
        }
      return false;
    }

  if (_currentEvent == _wholeContent)
    {
      vector<NclFormatterEvent *> evs = getEvents ();
      i = evs.begin ();
      while (i != evs.end ())
        {
          ev = (*i);

          if (ev->instanceOf ("NclAnchorEvent"))
            {
              contentAnchor = ((NclAnchorEvent *)ev)->getAnchor ();
              if (contentAnchor != NULL
                  && contentAnchor->instanceOf ("LabeledAnchor"))
                {
                  isLabeled = true;

                  clog
                      << "NclApplicationExecutionObject::abort event '";
                  clog << contentAnchor->getId () << "'" << endl;
                  ev->abort ();
                }
            }

          if (!isLabeled && ev->instanceOf ("NclPresentationEvent"))
            {
              endTime = ((NclPresentationEvent *)ev)->getEnd ();
              ev->abort ();
              if (endTime > 0)
                {
                  _transMan->abort (endTime, true);
                }
            }

          ++i;
        }
      _transMan->resetTimeIndex ();
      _pauseCount = 0;
    }
  else
    {
      if (_currentEvent->instanceOf ("NclAnchorEvent"))
        {
          contentAnchor = ((NclAnchorEvent *)_currentEvent)->getAnchor ();
          if (contentAnchor != NULL
              && contentAnchor->instanceOf ("LabeledAnchor"))
            {
              isLabeled = true;

              clog << "NclApplicationExecutionObject::abort event '";
              clog << contentAnchor->getId () << "'" << endl;
              _currentEvent->abort ();
            }
        }

      if (!isLabeled && _currentEvent->instanceOf ("NclPresentationEvent"))
        {
          endTime = ((NclPresentationEvent *)_currentEvent)->getEnd ();
          _currentEvent->abort ();
          if (endTime > 0)
            {
              _transMan->abort (endTime, true);
            }
        }
    }

  return true;
}

bool
NclApplicationExecutionObject::pause ()
{
  NclFormatterEvent *ev;
  vector<NclFormatterEvent *>::iterator i;

  lockEvents ();
  if (_currentEvent == NULL
      || _currentEvent->getCurrentState () != EventUtil::ST_OCCURRING
      || _preparedEvents.count (_currentEvent->getId ()) == 0)
    {
      unlockEvents ();
      return false;
    }
  unlockEvents ();

  if (_currentEvent == _wholeContent)
    {
      vector<NclFormatterEvent *> evs = getEvents ();
      if (_pauseCount == 0)
        {
          i = evs.begin ();
          while (i != evs.end ())
            {
              ev = *i;
              if (ev->getCurrentState () == EventUtil::ST_OCCURRING)
                {
                  ev->pause ();
                }
              ++i;
            }
        }

      _pauseCount++;
    }
  else if (_currentEvent->getCurrentState () == EventUtil::ST_OCCURRING)
    {
      _currentEvent->pause ();
    }

  return true;
}

bool
NclApplicationExecutionObject::resume ()
{
  NclFormatterEvent *event;
  vector<NclFormatterEvent *>::iterator i;

  if (_currentEvent == _wholeContent)
    {
      if (_pauseCount == 0)
        {
          return false;
        }
      else
        {
          _pauseCount--;
          if (_pauseCount > 0)
            {
              return false;
            }
        }

      vector<NclFormatterEvent *> evs = getEvents ();
      if (_pauseCount == 0)
        {
          i = evs.begin ();
          while (i != evs.end ())
            {
              event = *i;
              if (event->getCurrentState () == EventUtil::ST_PAUSED)
                {
                  event->resume ();
                }
              ++i;
            }
        }
    }
  else if (_currentEvent->getCurrentState () == EventUtil::ST_PAUSED)
    {
      _currentEvent->resume ();
    }

  return true;
}

bool
NclApplicationExecutionObject::unprepare ()
{
  map<Node *, NclCompositeExecutionObject *>::iterator i;
  map<string, NclFormatterEvent *>::iterator j;

  // clog << "NclApplicationExecutionObject::unprepare(" << id << ")" <<
  // endl;

  lockEvents ();
  if (_currentEvent == NULL
      || _currentEvent->getCurrentState () != EventUtil::ST_SLEEPING
      || _preparedEvents.count (_currentEvent->getId ()) == 0)
    {
      unlockEvents ();
      return false;
    }
  unlockEvents ();

  if (_currentEvent->instanceOf ("NclAnchorEvent")
      && ((NclAnchorEvent *)_currentEvent)->getAnchor () != NULL
      && ((NclAnchorEvent *)_currentEvent)
             ->getAnchor ()
             ->instanceOf ("LambdaAnchor"))
    {
      unprepareEvents ();
    }

  removeParentListenersFromEvent (_currentEvent);

  lockEvents ();
  j = _preparedEvents.find (_currentEvent->getId ());
  if (j != _preparedEvents.end ())
    {
      _preparedEvents.erase (j);
    }
  unlockEvents ();

  return true;
}

void
NclApplicationExecutionObject::unprepareEvents ()
{
  NclFormatterEvent *event;

  vector<NclFormatterEvent *>::iterator i;

  vector<NclFormatterEvent *> evs = getEvents ();
  i = evs.begin ();
  while (i != evs.end ())
    {
      event = *i;
      if (event->getCurrentState () != EventUtil::ST_SLEEPING)
        {
          event->stop ();
        }
      ++i;
    }
}

void
NclApplicationExecutionObject::removeEventListeners ()
{
  NclFormatterEvent *event;

  vector<NclFormatterEvent *>::iterator i;

  vector<NclFormatterEvent *> evs = getEvents ();
  i = evs.begin ();
  while (i != evs.end ())
    {
      event = *i;
      removeParentListenersFromEvent (event);
      ++i;
    }
}

void
NclApplicationExecutionObject::removeParentObject (Node *parentNode,
                                                   NclCompositeExecutionObject *parentObject)
{
  map<string, NclFormatterEvent *>::iterator j;

  lockEvents ();
  if (_mainEvent != NULL)
    {
      _mainEvent->removeEventListener (parentObject);
    }

  j = _preparedEvents.begin ();
  while (j != _preparedEvents.end ())
    {
      j->second->removeEventListener (parentObject);

      ++j;
    }
  unlockEvents ();

  NclExecutionObject::removeParentObject (parentNode, parentObject);
}

void
NclApplicationExecutionObject::removeParentListenersFromEvent (
    NclFormatterEvent *event)
{
  map<Node *, NclCompositeExecutionObject *>::iterator i;
  NclCompositeExecutionObject *parentObject;

  i = _parentTable.begin ();
  while (i != _parentTable.end ())
    {
      parentObject = i->second;
      // unregister parent as a currentEvent listener
      event->removeEventListener (parentObject);
      ++i;
    }
}

void
NclApplicationExecutionObject::lockEvents ()
{
}

void
NclApplicationExecutionObject::unlockEvents ()
{
}

GINGA_FORMATTER_END
