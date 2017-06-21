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

#include "PlayerAdapter.h"

GINGA_FORMATTER_BEGIN

set<FormatterEvent *> FormatterEvent::_instances;

FormatterEvent::FormatterEvent (const string &id,
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

FormatterEvent::~FormatterEvent ()
{
  _instances.erase (this);
  _listeners.clear ();
}

bool
FormatterEvent::hasInstance (FormatterEvent *evt, bool remove)
{
  bool has = _instances.find(evt) != _instances.end();

  if (has && remove)
    {
      _instances.erase (evt);
    }
  return has;
}

bool
FormatterEvent::instanceOf (const string &s)
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
FormatterEvent::hasNcmId (FormatterEvent *evt, const string &anchorId)
{
  Anchor *anchor;
  string anchorName = " ";

  if (auto anchorEvt = dynamic_cast<AnchorEvent *> (evt))
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
              && !(dynamic_cast<SelectionEvent *> (evt)))
            {
              return true;
            }
        }
    }
  else if (auto attrEvt = dynamic_cast<AttributionEvent *> (evt))
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
FormatterEvent::addListener (IFormatterEventListener *listener)
{
  this->_listeners.insert (listener);
}

void
FormatterEvent::removeListener (IFormatterEventListener *listener)
{
  _listeners.erase (listener);
}

EventStateTransition
FormatterEvent::getTransition (EventState newState)
{
  return EventUtil::getTransition (_state, newState);
}

bool
FormatterEvent::abort ()
{
  if (_state == EventState::OCCURRING || _state == EventState::PAUSED)
    return changeState (EventState::SLEEPING, EventStateTransition::ABORTS);
  else
    return false;
}

bool
FormatterEvent::start ()
{
  if (_state == EventState::SLEEPING)
    return changeState (EventState::OCCURRING, EventStateTransition::STARTS);
  else
    return false;
}

bool
FormatterEvent::stop ()
{
  if (_state == EventState::OCCURRING || _state == EventState::PAUSED)
    return changeState (EventState::SLEEPING, EventStateTransition::STOPS);
  else
    return false;
}

bool
FormatterEvent::pause ()
{
  if (_state == EventState::OCCURRING)
    return changeState (EventState::PAUSED, EventStateTransition::PAUSES);
  else
    return false;
}

bool
FormatterEvent::resume ()
{
  if (_state == EventState::PAUSED)
    return changeState (EventState::OCCURRING, EventStateTransition::RESUMES);
  else
    return false;
}

void
FormatterEvent::setState (EventState newState)
{
  _previousState = _state;
  _state = newState;
}

bool
FormatterEvent::changeState (EventState newState,
                             EventStateTransition transition)
{
  if (transition == EventStateTransition::STOPS)
    {
      _occurrences++;
    }

  _previousState = _state;
  _state = newState;

  set<IFormatterEventListener *> clone (_listeners);

  for (IFormatterEventListener *listener: clone)
    {
      listener->eventStateChanged (this, transition, _previousState);
    }

  return true;
}

// AnchorEvent
AnchorEvent::AnchorEvent (const string &id,
                          ExecutionObject *executionObject,
                          ContentAnchor *anchor)
  : FormatterEvent (id, executionObject)
{
  this->_anchor = anchor;
  _typeSet.insert ("AnchorEvent");
}

// PresentationEvent
PresentationEvent::PresentationEvent (const string &id,
                                      ExecutionObject *exeObj,
                                      ContentAnchor *anchor)
  : AnchorEvent (id, exeObj, anchor)
{
  _typeSet.insert ("PresentationEvent");

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
PresentationEvent::stop ()
{
  if (_state == EventState::OCCURRING && _numPresentations > 1)
    {
      _numPresentations--;
    }

  return FormatterEvent::stop ();
}

GingaTime
PresentationEvent::getDuration ()
{
  if (!GINGA_TIME_IS_VALID (this->_end))
    return GINGA_TIME_NONE;
  return this->_end - this->_begin;
}

int
PresentationEvent::getRepetitions ()
{
  return (_numPresentations - 1);
}

void
PresentationEvent::setRepetitionSettings (int repetitions,
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
PresentationEvent::incOccurrences ()
{
  _occurrences++;
}

// SelectionEvent
SelectionEvent::SelectionEvent (const string &id,
                                ExecutionObject *exeObj,
                                ContentAnchor *anchor)
  : AnchorEvent (id, exeObj, anchor)
{
  _selCode.assign("NO_CODE");

  _typeSet.insert ("SelectionEvent");
}

bool
SelectionEvent::start ()
{
  if (AnchorEvent::start ())
    return AnchorEvent::stop ();
  else
    return false;
}

// AttributionEvent
AttributionEvent::AttributionEvent (const string &id,
                                    ExecutionObject *exeObj,
                                    PropertyAnchor *anchor,
                                    Settings *settings)
  : FormatterEvent (id, exeObj)
{
  Entity *entity;
  NodeEntity *dataObject;

  _typeSet.insert ("AttributionEvent");

  this->_anchor = anchor;
  this->_player = nullptr;
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

AttributionEvent::~AttributionEvent ()
{
  _assessments.clear ();
}

string
AttributionEvent::getCurrentValue ()
{
  string propName;
  string value = "";

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
          value = _settings->get (propName);
        }
    }
  else
    {
      if (_player)
        {
          value = _player->getProperty (this);
        }

      if (value == "")
        {
          value = _anchor->getValue ();
        }
    }

  return value;
}

bool
AttributionEvent::setValue (const string &newValue)
{
  if (_anchor->getValue () != newValue)
    {
      _anchor->setValue (newValue);
      return true;
    }
  return false;
}

void
AttributionEvent::setImplicitRefAssessmentEvent (
    const string &roleId, FormatterEvent *event)
{
  _assessments[roleId] = event;
}

FormatterEvent *
AttributionEvent::getImplicitRefAssessmentEvent (const string &roleId)
{
  if (_assessments.count (roleId) == 0)
    {
      return nullptr;
    }

  return _assessments[roleId];
}

// SwitchEvent
SwitchEvent::SwitchEvent (const string &id,
                          ExecutionObject *exeObjSwitch,
                          InterfacePoint *interface,
                          EventType type, const string &key)
  : FormatterEvent (id, exeObjSwitch)
{
  this->_interface = interface;
  this->_type = type;
  this->_key = key;
  this->_mappedEvent = nullptr;

  _typeSet.insert ("SwitchEvent");
}

SwitchEvent::~SwitchEvent ()
{
  if (FormatterEvent::hasInstance (_mappedEvent, false))
    {
      _mappedEvent->removeListener (this);
    }
}

void
SwitchEvent::setMappedEvent (FormatterEvent *evt)
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
SwitchEvent::eventStateChanged (
    arg_unused (FormatterEvent *evt),
    EventStateTransition trans,
    arg_unused (EventState prevState))
{
  changeState (EventUtil::getNextState (trans), trans);
}

EventTransition::EventTransition (GingaTime time,
                                  PresentationEvent *evt)
{
  this->_time = time;
  this->_evt = evt;
}

BeginEventTransition::BeginEventTransition (
    GingaTime t, PresentationEvent *evt)
  : EventTransition (t, evt)
{

}

EndEventTransition::EndEventTransition (GingaTime t,
                                        PresentationEvent *evt,
                                        BeginEventTransition *trans)
  : EventTransition (t, evt)
{
  _beginTrans = trans;
  _beginTrans->setEndTransition (this);
}

GINGA_FORMATTER_END
