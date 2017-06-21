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
#include "ExecutionObjectApplication.h"

GINGA_PRAGMA_DIAG_IGNORE (-Wsign-conversion)

GINGA_FORMATTER_BEGIN

ExecutionObjectApplication::ExecutionObjectApplication (
    const string &id, Node *node, NclCascadingDescriptor *descriptor,
    bool handling, INclLinkActionListener *seListener)
  : ExecutionObject (id, node, descriptor, handling, seListener)
{
  _typeSet.insert ("ExecutionObjectApplication");
  _currentEvent = nullptr;
}

ExecutionObjectApplication::~ExecutionObjectApplication ()
{
  ExecutionObject::removeInstance (this);

  for (auto i: _preparedEvents)
    {
      removeParentListenersFromEvent (i.second);
    }

  unprepare ();
  unprepareEvents ();
  removeEventListeners ();

  destroyEvents ();
}

bool
ExecutionObjectApplication::isSleeping ()
{
  for (auto i: _preparedEvents)
    {
      if (i.second->getCurrentState () != EventState::SLEEPING)
        {
          return false;
        }
    }
  return true;
}

bool
ExecutionObjectApplication::isPaused ()
{
  bool hasPaused = false;

  if (_currentEvent != nullptr)
    {
      if (_currentEvent->getCurrentState () == EventState::OCCURRING)
        {
          return false;
        }

      if (_currentEvent->getCurrentState () == EventState::PAUSED)
        {
          hasPaused = true;
        }
    }

  for (auto i: _preparedEvents)
    {
      NclFormatterEvent *event = i.second;
      if (event->getCurrentState () == EventState::OCCURRING)
        {
          return false;
        }

      if (event->getCurrentState () == EventState::PAUSED)
        {
          hasPaused = true;
        }
    }

  return hasPaused;
}

NclFormatterEvent *
ExecutionObjectApplication::getCurrentEvent ()
{
  return _currentEvent;
}

bool
ExecutionObjectApplication::hasPreparedEvent (NclFormatterEvent *event)
{
  for (auto i: _preparedEvents)
    {
      if (i.second != event)
        {
          return true;
        }
    }
  return false;
}

void
ExecutionObjectApplication::setCurrentEvent (NclFormatterEvent *event)
{
  if (!containsEvent (event))
    {
      _currentEvent = nullptr;
    }
  else
    {
      _currentEvent = event;
    }
}

bool
ExecutionObjectApplication::prepare (NclFormatterEvent *event,
                                        GingaTime offsetTime)
{
  GingaTime startTime = 0;
  ContentAnchor *contentAnchor;
  NclFormatterEvent *auxEvent;
  PropertyAnchor *attributeAnchor;
  int j;

  if (event->getCurrentState () != EventState::SLEEPING)
    {
      WARNING ("Can't prepare '%s': event isn't sleeping.",
               event->getId ().c_str ());
      return false;
    }

  NclPresentationEvent *presentationEvt
      = dynamic_cast<NclPresentationEvent *> (event);
  if (presentationEvt)
    {
      GingaTime duration = presentationEvt->getDuration ();
      if (duration <= 0)
        {
          WARNING ("Can't prepare '%s': event duration <= 0.",
                   event->getId ().c_str ());
          return false;
        }
    }

  addEvent (event);
  NclAnchorEvent *anchorEvt = dynamic_cast<NclAnchorEvent *> (event);
  if (anchorEvt)
    {
      contentAnchor = anchorEvt->getAnchor ();
      auto labeledAnchor = dynamic_cast<LabeledAnchor *> (contentAnchor);
      if (labeledAnchor)
        {
          for (auto i : _parentTable)
            {
              // register parent as a mainEvent listener
              event->addListener (i.second);
            }

          _preparedEvents[event->getId ()] = event;
          return true;
        }
    }

  if (presentationEvt)
    {
      startTime = presentationEvt->getBegin () + offsetTime;
      if (startTime > presentationEvt->getEnd ())
        {
          TRACE ("Skipping '%s': past event (start = '%lu'; end = '%lu'.",
                 event->getId ().c_str (),
                 startTime,
                 presentationEvt->getEnd ());

          return false;
        }
    }

  for (auto i : _parentTable)
    {
      // register parent as a currentEvent listener
      event->addListener (i.second);
    }

  _transMan.prepare (event == _wholeContent, startTime);

  for (j = 0; j < (int) _otherEvents.size (); j++)
    {
      auxEvent = _otherEvents[j];
      auto attributionEvt = dynamic_cast<NclAttributionEvent *> (auxEvent);
      if (attributionEvt)
        {
          attributeAnchor = attributionEvt->getAnchor ();
          if (attributeAnchor->getValue () != "")
            {
              attributionEvt->setValue (attributeAnchor->getValue ());
            }
        }
    }

  this->_offsetTime = startTime;
  _preparedEvents[event->getId ()] = event;

  return true;
}

bool
ExecutionObjectApplication::start ()
{
  if (_currentEvent == nullptr
      || _preparedEvents.count (_currentEvent->getId ()) == 0)
    {
      return false; // nothing to do
    }

  /*
   * TODO: follow the event state machine or start instruction behavior
   * if (currentEvent->getCurrentState() == EventState::PAUSED) {
          return resume();
  }*/

  if (_currentEvent->getCurrentState () != EventState::SLEEPING)
    {
      TRACE ("Current event '%s' is already running.",
             _currentEvent->getId ().c_str ());

      return false;
    }

  auto anchorEvent = dynamic_cast<NclAnchorEvent *> (_currentEvent);

  if (anchorEvent)
    {
      auto contentAnchor = anchorEvent->getAnchor ();
      auto labeledAnchor = dynamic_cast<LabeledAnchor *> (contentAnchor);
      if (labeledAnchor)
        {
          _transMan.start (_offsetTime);
          _currentEvent->start ();

          TRACE ("Current event '%s' started.",
                 _currentEvent->getId ().c_str ());
          return true;
        }
    }

  _transMan.start (_offsetTime);

  return true;
}

NclEventTransition *
ExecutionObjectApplication::getNextTransition ()
{
  if (_currentEvent == nullptr
      || _currentEvent->getCurrentState () == EventState::SLEEPING
      || !(dynamic_cast <NclPresentationEvent *> (_currentEvent)))
    {
      return nullptr;
    }

  return _transMan.getNextTransition (_currentEvent);
}

bool
ExecutionObjectApplication::stop ()
{
  ContentAnchor *contentAnchor;
  GingaTime endTime;
  bool isLabeled = false;

  if (isSleeping ())
    {
      if (_wholeContent != nullptr
          && _wholeContent->getCurrentState () != EventState::SLEEPING)
        {
          _wholeContent->stop ();
        }
      return false;
    }

  auto anchorEvent = dynamic_cast<NclAnchorEvent*> (_currentEvent);
  if (anchorEvent)
    {
      contentAnchor = anchorEvent->getAnchor ();
      auto labeledAnchor = dynamic_cast<LabeledAnchor *> (contentAnchor);
      if (labeledAnchor)
        {
          isLabeled = true;

          _currentEvent->stop ();
        }
    }

  auto presentationEvt = dynamic_cast<NclPresentationEvent *> (_currentEvent);
  if (!isLabeled && presentationEvt)
    {
      endTime = presentationEvt->getEnd ();
      _currentEvent->stop ();
      if (endTime > 0)
        {
          _transMan.stop (endTime, true);
        }
    }

  _transMan.resetTimeIndex ();
  _pauseCount = 0;

  return true;
}

bool
ExecutionObjectApplication::abort ()
{
  ContentAnchor *contentAnchor;
  GingaTime endTime;
  bool isLabeled = false;

  if (isSleeping ())
    {
      if (_wholeContent != nullptr
          && _wholeContent->getCurrentState () != EventState::SLEEPING)
        {
          _wholeContent->abort ();
        }
      return false;
    }

  if (_currentEvent == _wholeContent)
    {
      for (NclFormatterEvent *ev : getEvents ())
        {
          auto anchorEvt = dynamic_cast <NclAnchorEvent *> (ev);
          if (anchorEvt)
            {
              contentAnchor = anchorEvt->getAnchor ();
              if (dynamic_cast<LabeledAnchor *> (contentAnchor))
                {
                  isLabeled = true;

                  ev->abort ();
                }
            }

          auto presentationEvt = dynamic_cast <NclPresentationEvent *> (ev);
          if (!isLabeled && presentationEvt)
            {
              endTime = presentationEvt->getEnd ();
              ev->abort ();
              if (endTime > 0)
                {
                  _transMan.abort (endTime, true);
                }
            }
        }
      _transMan.resetTimeIndex ();
      _pauseCount = 0;
    }
  else
    {
      auto anchorEvt = dynamic_cast<NclAnchorEvent *> (_currentEvent);
      if (anchorEvt)
        {
          contentAnchor = anchorEvt->getAnchor ();

          if (dynamic_cast<LabeledAnchor *>(contentAnchor))
            {
              isLabeled = true;

              _currentEvent->abort ();
            }
        }

      auto presentationEvt
          = dynamic_cast <NclPresentationEvent *> (_currentEvent);
      if (!isLabeled && presentationEvt)
        {
          endTime = presentationEvt->getEnd ();
          _currentEvent->abort ();
          if (endTime > 0)
            {
              _transMan.abort (endTime, true);
            }
        }
    }

  return true;
}

bool
ExecutionObjectApplication::pause ()
{
  if (_currentEvent == nullptr
      || _currentEvent->getCurrentState () != EventState::OCCURRING
      || _preparedEvents.count (_currentEvent->getId ()) == 0)
    {
      return false;
    }

  if (_currentEvent == _wholeContent)
    {
      if (_pauseCount == 0)
        {
          for (NclFormatterEvent *ev: getEvents ())
            {
              if (ev->getCurrentState () == EventState::OCCURRING)
                {
                  ev->pause ();
                }
            }
        }

      _pauseCount++;
    }
  else if (_currentEvent->getCurrentState () == EventState::OCCURRING)
    {
      _currentEvent->pause ();
    }

  return true;
}

bool
ExecutionObjectApplication::resume ()
{
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

      if (_pauseCount == 0)
        {
          for (NclFormatterEvent *event: getEvents ())
            {
              if (event->getCurrentState () == EventState::PAUSED)
                {
                  event->resume ();
                }
            }
        }
    }
  else if (_currentEvent->getCurrentState () == EventState::PAUSED)
    {
      _currentEvent->resume ();
    }

  return true;
}

bool
ExecutionObjectApplication::unprepare ()
{
  if (_currentEvent == nullptr
      || _currentEvent->getCurrentState () != EventState::SLEEPING
      || _preparedEvents.count (_currentEvent->getId ()) == 0)
    {
      return false;
    }


  if (_currentEvent->instanceOf ("NclAnchorEvent")
      && ((NclAnchorEvent *)_currentEvent)->getAnchor () != nullptr
      && ((NclAnchorEvent *)_currentEvent)
      ->getAnchor ()
      ->instanceOf ("LambdaAnchor"))
    {
      unprepareEvents ();
    }

  removeParentListenersFromEvent (_currentEvent);

  auto j = _preparedEvents.find (_currentEvent->getId ());
  if (j != _preparedEvents.end ())
    {
      _preparedEvents.erase (j);
    }

  return true;
}

void
ExecutionObjectApplication::unprepareEvents ()
{
  vector<NclFormatterEvent *>::iterator i;

  for (NclFormatterEvent *event : getEvents ())
    if (event->getCurrentState () != EventState::SLEEPING)
      event->stop ();
}

void
ExecutionObjectApplication::removeEventListeners ()
{
  for (NclFormatterEvent *event : getEvents ())
    {
      removeParentListenersFromEvent (event);
    }
}

void
ExecutionObjectApplication::removeParentObject (
    Node *parentNode, ExecutionObjectContext *parentObj)
{
  if (_mainEvent != nullptr)
    {
      _mainEvent->removeListener (parentObj);
    }

  for (auto i : _preparedEvents)
    {
      i.second->removeListener (parentObj);
    }

  ExecutionObject::removeParentObject (parentNode, parentObj);
}

void
ExecutionObjectApplication::removeParentListenersFromEvent (
    NclFormatterEvent *event)
{
  ExecutionObjectContext *parentObject;

  for (auto i : _parentTable)
    {
      parentObject = i.second;
      // unregister parent as a currentEvent listener
      event->removeListener (parentObject);
    }
}

GINGA_FORMATTER_END
