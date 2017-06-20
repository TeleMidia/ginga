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
#include "FormatterEvents.h"

#include "ExecutionObjectContext.h"
#include "ncl/ContentNode.h"

GINGA_FORMATTER_BEGIN

set<NclFormatterEvent *> NclFormatterEvent::_instances;

NclFormatterEvent::NclFormatterEvent (const string &id,
                                      ExecutionObject *exeObj)
{
  _typeSet.insert ("NclFormatterEvent");

  this->_id = id;
  _state = EventState::SLEEPING;
  _occurrences = 0;
  _exeObj = exeObj;
  _type = EventType::UNKNOWN;

  _instances.insert (this);
}

NclFormatterEvent::~NclFormatterEvent ()
{
  _instances.erase (this);
  _listeners.clear ();
}

bool
NclFormatterEvent::hasInstance (NclFormatterEvent *evt, bool remove)
{
  bool has = _instances.find(evt) != _instances.end();

  if (has && remove)
    {
      _instances.erase (evt);
    }
  return has;
}

bool
NclFormatterEvent::instanceOf (const string &s)
{
  if (_typeSet.empty ())
    {
      return false;
    }
  else
    {
      return (_typeSet.find (s) != _typeSet.end ());
    }
}

bool
NclFormatterEvent::hasNcmId (NclFormatterEvent *evt, const string &anchorId)
{
  Anchor *anchor;
  string anchorName = " ";

  if (auto anchorEvt = dynamic_cast<NclAnchorEvent *> (evt))
    {
      anchor = anchorEvt->getAnchor ();
      if (anchor != nullptr)
        {
          if (dynamic_cast<IntervalAnchor *> (anchor))
            {
              anchorName = anchor->getId ();
            }
          else if (auto labeledAnchor = dynamic_cast<LabeledAnchor *> (anchor))
            {
              anchorName = labeledAnchor->getLabel ();
            }
          else if (dynamic_cast<LambdaAnchor *> (anchor))
            {
              anchorName = "";
            }

          if (anchorName == anchorId
              && !(dynamic_cast<NclSelectionEvent *> (evt)))
            {
              return true;
            }
        }
    }
  else if (auto attrEvt = dynamic_cast<NclAttributionEvent *> (evt))
    {
      anchor = attrEvt->getAnchor ();
      if (anchor != nullptr)
        {
          auto propAnchor = dynamic_cast<PropertyAnchor *> (anchor);
          g_assert_nonnull (propAnchor);
          anchorName = propAnchor->getName ();
          if (anchorName == anchorId)
            {
              return true;
            }
        }
    }

  return false;
}

void
NclFormatterEvent::addListener (INclEventListener *listener)
{
  this->_listeners.insert (listener);
}

void
NclFormatterEvent::removeListener (INclEventListener *listener)
{
  _listeners.erase (listener);
}

EventStateTransition
NclFormatterEvent::getTransition (EventState newState)
{
  return EventUtil::getTransition (_state, newState);
}

bool
NclFormatterEvent::abort ()
{
  if (_state == EventState::OCCURRING || _state == EventState::PAUSED)
    return changeState (EventState::SLEEPING, EventStateTransition::ABORTS);
  else
    return false;
}

bool
NclFormatterEvent::start ()
{
  if (_state == EventState::SLEEPING)
    return changeState (EventState::OCCURRING, EventStateTransition::STARTS);
  else
    return false;
}

bool
NclFormatterEvent::stop ()
{
  if (_state == EventState::OCCURRING || _state == EventState::PAUSED)
    return changeState (EventState::SLEEPING, EventStateTransition::STOPS);
  else
    return false;
}

bool
NclFormatterEvent::pause ()
{
  if (_state == EventState::OCCURRING)
    return changeState (EventState::PAUSED, EventStateTransition::PAUSES);
  else
    return false;
}

bool
NclFormatterEvent::resume ()
{
  if (_state == EventState::PAUSED)
    return changeState (EventState::OCCURRING, EventStateTransition::RESUMES);
  else
    return false;
}

void
NclFormatterEvent::setState (EventState newState)
{
  _previousState = _state;
  _state = newState;
}

bool
NclFormatterEvent::changeState (EventState newState,
                                EventStateTransition transition)
{
  if (transition == EventStateTransition::STOPS)
    {
      _occurrences++;
    }

  _previousState = _state;
  _state = newState;

  set<INclEventListener *> clone (_listeners);

  for (INclEventListener *listener: clone)
    {
      listener->eventStateChanged (this, transition, _previousState);
    }

  return true;
}

// NclAnchorEvent
NclAnchorEvent::NclAnchorEvent (const string &id,
                                ExecutionObject *executionObject,
                                ContentAnchor *anchor)
  : NclFormatterEvent (id, executionObject)
{
  this->_anchor = anchor;
  _typeSet.insert ("NclAnchorEvent");
}

// NclPresentationEvent
NclPresentationEvent::NclPresentationEvent (const string &id,
                                            ExecutionObject *exeObj,
                                            ContentAnchor *anchor)
  : NclAnchorEvent (id, exeObj, anchor)
{
  _typeSet.insert ("NclPresentationEvent");

  _numPresentations = 1;
  _repetitionInterval = 0;

  auto intervalAnchor = dynamic_cast<IntervalAnchor *> (anchor);
  if (intervalAnchor)
    {
      _begin = intervalAnchor->getBegin ();
      _end = intervalAnchor->getEnd ();
    }
  else
    {
      _begin = 0;
      _end = GINGA_TIME_NONE;
    }
}

bool
NclPresentationEvent::stop ()
{
  if (_state == EventState::OCCURRING && _numPresentations > 1)
    {
      _numPresentations--;
    }

  return NclFormatterEvent::stop ();
}

GingaTime
NclPresentationEvent::getDuration ()
{
  if (!GINGA_TIME_IS_VALID (this->_end))
    return GINGA_TIME_NONE;
  return this->_end - this->_begin;
}

int
NclPresentationEvent::getRepetitions ()
{
  return (_numPresentations - 1);
}

void
NclPresentationEvent::setRepetitionSettings (int repetitions,
                                             GingaTime repetitionInterval)
{
  if (repetitions >= 0)
    {
      this->_numPresentations = repetitions + 1;
    }
  else
    {
      this->_numPresentations = 1;
    }

  this->_repetitionInterval = repetitionInterval;
}

void
NclPresentationEvent::incOccurrences ()
{
  _occurrences++;
}

// NclSelectionEvent
NclSelectionEvent::NclSelectionEvent (const string &id,
                                      ExecutionObject *exeObj,
                                      ContentAnchor *anchor)
  : NclAnchorEvent (id, exeObj, anchor)
{
  _selCode.assign("NO_CODE");

  _typeSet.insert ("NclSelectionEvent");
}

bool
NclSelectionEvent::start ()
{
  if (NclAnchorEvent::start ())
    return NclAnchorEvent::stop ();
  else
    return false;
}

// NclAttributionEvent
NclAttributionEvent::NclAttributionEvent (const string &id,
                                          ExecutionObject *exeObj,
                                          PropertyAnchor *anchor,
                                          Settings *settings)
  : NclFormatterEvent (id, exeObj)
{
  Entity *entity;
  NodeEntity *dataObject;

  _typeSet.insert ("NclAttributionEvent");

  this->_anchor = anchor;
  this->_valueMaintainer = nullptr;
  this->_settingsNode = false;
  this->_settings = settings;

  dataObject = dynamic_cast<NodeEntity *> (exeObj->getDataObject ());

  auto contentNode = dynamic_cast<ContentNode *> (dataObject);
  if (contentNode
      && contentNode->isSettingNode ())
    {
      _settingsNode = true;
    }

  auto referNode = dynamic_cast<ReferNode *> (dataObject);
  if (referNode)
    {
      if (referNode->getInstanceType () == "instSame")
        {
          entity = referNode->getDataEntity ();
          auto contentNode = dynamic_cast <ContentNode *> (entity);
          if (contentNode
              && contentNode->isSettingNode ())
            {
              _settingsNode = true;
            }
        }
    }
}

NclAttributionEvent::~NclAttributionEvent ()
{
  _assessments.clear ();
}

string
NclAttributionEvent::getCurrentValue ()
{
  string propName;
  string maintainerValue = "";

  if (unlikely (_anchor == nullptr))
    {
      ERROR ("Trying to set a null property anchor of object '%s'.",
             _id.c_str ());
    }

  if (_settingsNode)
    {
      propName = _anchor->getName ();
      if (propName != "")
        {
          maintainerValue = _settings->get (propName);
        }
    }
  else
    {
      if (_valueMaintainer != nullptr)
        {
          maintainerValue = _valueMaintainer->getProperty (this);
        }

      if (maintainerValue == "")
        {
          maintainerValue = _anchor->getValue ();
        }
    }

  return maintainerValue;
}

bool
NclAttributionEvent::setValue (const string &newValue)
{
  if (_anchor->getValue () != newValue)
    {
      _anchor->setValue (newValue);
      return true;
    }
  return false;
}

void
NclAttributionEvent::setImplicitRefAssessmentEvent (
    const string &roleId, NclFormatterEvent *event)
{
  _assessments[roleId] = event;
}

NclFormatterEvent *
NclAttributionEvent::getImplicitRefAssessmentEvent (const string &roleId)
{
  if (_assessments.count (roleId) == 0)
    {
      return nullptr;
    }

  return _assessments[roleId];
}

// NclSwitchEvent
NclSwitchEvent::NclSwitchEvent (const string &id,
                                ExecutionObject *exeObjSwitch,
                                InterfacePoint *interface,
                                EventType type, const string &key)
  : NclFormatterEvent (id, exeObjSwitch)
{
  this->_interface = interface;
  this->_type = type;
  this->_key = key;
  this->_mappedEvent = nullptr;

  _typeSet.insert ("NclSwitchEvent");
}

NclSwitchEvent::~NclSwitchEvent ()
{
  if (NclFormatterEvent::hasInstance (_mappedEvent, false))
    {
      _mappedEvent->removeListener (this);
    }
}

void
NclSwitchEvent::setMappedEvent (NclFormatterEvent *evt)
{
  if (_mappedEvent != nullptr)
    {
      _mappedEvent->removeListener (this);
    }

  _mappedEvent = evt;
  if (_mappedEvent != nullptr)
    {
      _mappedEvent->addListener (this);
    }
}

void
NclSwitchEvent::eventStateChanged (
    arg_unused (NclFormatterEvent *evt),
    EventStateTransition trans,
    arg_unused (EventState prevState))
{
  changeState (EventUtil::getNextState (trans), trans);
}

GINGA_FORMATTER_END
