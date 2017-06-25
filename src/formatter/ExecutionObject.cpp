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

#include "ExecutionObject.h"
#include "ExecutionObjectContext.h"
#include "NclEvents.h"

#include "ncl/ContentNode.h"
using namespace ::ginga::ncl;

#include "player/Player.h"
using namespace ::ginga::player;

#include "mb/Display.h"
using namespace ::ginga::mb;

GINGA_FORMATTER_BEGIN

/**
 * @brief Set containing all execution objects.
 */
set<ExecutionObject *> ExecutionObject::_objects;

ExecutionObject::ExecutionObject (const string &id,
                                  Node *node,
                                  NclCascadingDescriptor *descriptor,
                                  bool handling,
                                  INclActionListener *seListener)
{
  this->_seListener = seListener;
  this->_dataObject = node;
  this->_wholeContent = nullptr;
  this->_descriptor = nullptr;
  this->_isCompiled = false;
  this->_mainEvent = nullptr;
  this->_descriptor = descriptor;
  this->_isHandler = false;
  this->_isHandling = handling;

  _id = id;
  _player = nullptr;
  _time = GINGA_TIME_NONE;

  _objects.insert (this);
  TRACE ("creating exec object '%s' (%p)", _id.c_str (), this);
}

ExecutionObject::~ExecutionObject ()
{
  map<Node *, ExecutionObjectContext *>::iterator j;

  Node *parentNode;
  ExecutionObjectContext *parentObject;

  unsetParentsAsListeners ();

  _seListener = nullptr;
  _dataObject = nullptr;
  _wholeContent = nullptr;
  _mainEvent = nullptr;

  destroyEvents ();

  for (auto i : _nodeParentTable)
    {
      parentNode = i.second;
      j = _parentTable.find (parentNode);
      if (j != _parentTable.end ())
        {
          parentObject = j->second;

          parentObject->removeExecutionObject (this);
        }
    }

  _nodeParentTable.clear ();

  _parentTable.clear ();
  if (_descriptor != nullptr)
    {
      delete _descriptor;
      _descriptor = nullptr;
    }

  _objects.insert (this);
  TRACE ("destroying exec object '%s' (%p)", _id.c_str (), this);
}

void
ExecutionObject::destroyEvents ()
{
  for (auto i : _events)
    {
      NclEvent *event = i.second;
      if (NclEvent::hasInstance (event, true))
        {
          delete event;
        }
    }
  _events.clear ();

  _presEvents.clear ();
  _selectionEvents.clear ();
  _otherEvents.clear ();
}

void
ExecutionObject::unsetParentsAsListeners ()
{
  removeParentListenersFromEvent (_mainEvent);
  removeParentListenersFromEvent (_wholeContent);
}

void
ExecutionObject::removeParentListenersFromEvent (
    NclEvent *event)
{
  map<Node *, ExecutionObjectContext *>::iterator i;
  ExecutionObjectContext *parentObject;

  if (NclEvent::hasInstance (event, false))
    {
      i = _parentTable.begin ();
      while (i != _parentTable.end ())
        {
          parentObject = (ExecutionObjectContext *)(i->second);

          if (NclEvent::hasInstance (_mainEvent, false))
            {
              // register parent as a mainEvent listener
              _mainEvent->removeListener (parentObject);
            }
          else
            {
              break;
            }
          ++i;
        }
    }
}

bool
ExecutionObject::isSleeping ()
{
  return  _mainEvent
    && _mainEvent->getCurrentState () == EventState::SLEEPING;
}

bool
ExecutionObject::isPaused ()
{
  return _mainEvent
    && _mainEvent->getCurrentState () == EventState::PAUSED;
}

bool
ExecutionObject::isOccurring ()
{
  return _mainEvent
    && _mainEvent->getCurrentState () == EventState::OCCURRING;
}

Node *
ExecutionObject::getDataObject ()
{
  return _dataObject;
}

NclCascadingDescriptor *
ExecutionObject::getDescriptor ()
{
  return _descriptor;
}

string
ExecutionObject::getId ()
{
  return _id;
}

ExecutionObjectContext *
ExecutionObject::getParentObject ()
{
  return getParentObject (_dataObject);
}

ExecutionObjectContext *
ExecutionObject::getParentObject (Node *node)
{
  ExecutionObjectContext *parentObj = nullptr;

  auto i = _nodeParentTable.find (node);
  if (i != _nodeParentTable.end ())
    {
      Node *parentNode = i->second;
      auto j = _parentTable.find (parentNode);
      if (j != _parentTable.end ())
        {
          parentObj = j->second;
        }
    }

  return parentObj;
}

void
ExecutionObject::addParentObject (ExecutionObjectContext *parentObject,
                                  Node *parentNode)
{
  addParentObject (_dataObject, parentObject, parentNode);
}

void
ExecutionObject::addParentObject (Node *node,
                                  ExecutionObjectContext *parentObject,
                                  Node *parentNode)
{
  _nodeParentTable[node] = parentNode;
  _parentTable[parentNode] = parentObject;
}

void
ExecutionObject::removeParentObject (Node *parentNode,
                                     ExecutionObjectContext *parentObject)
{
  map<Node *, ExecutionObjectContext *>::iterator i;

  i = _parentTable.find (parentNode);
  if (i != _parentTable.end () && i->second == parentObject)
    {
      if (_wholeContent != nullptr)
        {
          _wholeContent->removeListener (parentObject);
        }
      _parentTable.erase (i);
    }
}

void
ExecutionObject::setDescriptor (GenericDescriptor *descriptor)
{
  NclCascadingDescriptor *cascade;
  cascade = new NclCascadingDescriptor (descriptor);

  if (this->_descriptor != nullptr)
    {
      delete this->_descriptor;
    }

  this->_descriptor = cascade;
}

bool
ExecutionObject::addEvent (NclEvent *event)
{
  map<string, NclEvent *>::iterator i;

  g_assert (_events.find (event->getId ()) == _events.end ());

  _events[event->getId ()] = event;
  if (instanceof (PresentationEvent *, event))
    {
      addPresentationEvent ((PresentationEvent *) event);
    }
  else if (instanceof (SelectionEvent *, event))
    {
      _selectionEvents.insert (((SelectionEvent *) event));
    }
  else
    {
      _otherEvents.push_back (event);
    }
  return true;
}

void
ExecutionObject::addPresentationEvent (PresentationEvent *event)
{
  PresentationEvent *auxEvent;
  GingaTime begin, auxBegin;
  int posBeg = -1;
  int posEnd, posMid;

  if (instanceof (LambdaAnchor *, event->getAnchor ()))
    {
      _presEvents.insert (_presEvents.begin (), event);
      _wholeContent = (PresentationEvent *)event;
    }
  else
    {
      begin = event->getBegin ();
      posBeg = 0;
      posEnd = (int)(_presEvents.size () - 1);
      while (posBeg <= posEnd)
        {
          posMid = (posBeg + posEnd) / 2;
          auxEvent = (PresentationEvent *)(_presEvents[(size_t)posMid]);
          auxBegin = auxEvent->getBegin ();
          if (begin < auxBegin)
            {
              posEnd = posMid - 1;
            }
          else if (begin > auxBegin)
            {
              posBeg = posMid + 1;
            }
          else
            {
              posBeg = posMid + 1;
              break;
            }
        }

      _presEvents.insert ((_presEvents.begin () + posBeg), event);
    }

  clog << "NclExecutionObject::addPresentationEvent in '";
  clog << getId () << "'. Call transition ";
  clog << "manager addPresentationEvent for '" << event->getId ();
  clog << "' begin = '" << event->getBegin () << "'; end = '";
  clog << event->getEnd () << "' position = '" << posBeg << "'" << endl;
  _transMan.addPresentationEvent (event);
}

bool
ExecutionObject::containsEvent (NclEvent *event)
{
  return (_events.count (event->getId ()) != 0);
}

NclEvent *
ExecutionObject::getEventFromAnchorId (const string &anchorId)
{
  map<string, NclEvent *>::iterator i;
  NclEvent *event;

  if (anchorId == "")
    {
      if (_wholeContent != nullptr)
        {
          return _wholeContent;
        }
    }
  else
    {
      if (_wholeContent != nullptr)
        {
          if (NclEvent::hasNcmId (_wholeContent, anchorId))
            {
              return _wholeContent;
            }
        }

      i = _events.begin ();
      clog << "NclExecutionObject::getEventFromAnchorId searching '";
      clog << anchorId << "' for '" << _id;
      clog << "' with following events = ";
      while (i != _events.end ())
        {
          event = i->second;
          if (event != nullptr)
            {
              if (NclEvent::hasNcmId (event, anchorId))
                {
                  return event;
                }
            }
          ++i;
        }
      clog << endl;
    }

  return nullptr;
}

NclEvent *
ExecutionObject::getEvent (const string &id)
{
  NclEvent *ev;
  if (_events.count (id) != 0)
    {
      ev = _events[id];
      return ev;
    }
  return nullptr;
}

vector<NclEvent *>
ExecutionObject::getEvents ()
{
  vector<NclEvent *> eventsVector;
  for (const auto &i : _events)
    {
      eventsVector.push_back (i.second);
    }
  return eventsVector;
}

PresentationEvent *
ExecutionObject::getWholeContentPresentationEvent ()
{
  return _wholeContent;
}

bool
ExecutionObject::removeEvent (NclEvent *event)
{
  vector<PresentationEvent *>::iterator i;
  set<SelectionEvent *>::iterator j;
  vector<NclEvent *>::iterator k;
  map<string, NclEvent *>::iterator l;

  if (!containsEvent (event))
    {
      return false;
    }

  clog << "NclExecutionObject::removeEvent '" << event->getId () << "'";
  clog << "from '" << getId () << "'" << endl;

  if (instanceof (PresentationEvent *, event))
    {
      for (i = _presEvents.begin (); i != _presEvents.end (); ++i)
        {
          if (*i == (PresentationEvent *)event)
            {
              _presEvents.erase (i);
              break;
            }
        }
      _transMan.removeEventTransition ((PresentationEvent *)event);
    }
  else if (instanceof (SelectionEvent *, event))
    {
      j = _selectionEvents.find (((SelectionEvent *)event));
      if (j != _selectionEvents.end ())
        {
          _selectionEvents.erase (j);
        }
    }
  else
    {
      for (k = _otherEvents.begin (); k != _otherEvents.end (); ++k)
        {
          if (*k == event)
            {
              _otherEvents.erase (k);
              break;
            }
        }
    }

  l = _events.find (event->getId ());
  if (l != _events.end ())
    _events.erase (l);

  return true;
}

bool
ExecutionObject::isCompiled ()
{
  return _isCompiled;
}

void
ExecutionObject::setCompiled (bool status)
{
  _isCompiled = status;
}

vector<Node *>
ExecutionObject::getNodes ()
{
  vector<Node *> nodes;
  if (!_nodeParentTable.empty())
    {
      for (auto &i: _nodeParentTable)
        {
          nodes.push_back (i.first);
        }

      if (_nodeParentTable.count (_dataObject) == 0)
        {
          nodes.push_back (_dataObject);
        }
    }

  return nodes;
}

PropertyAnchor *
ExecutionObject::getNCMProperty (const string &propertyName)
{
  PropertyAnchor *property = nullptr;

  if (_dataObject != nullptr)
    {
      property = _dataObject->getPropertyAnchor (propertyName);
    }

  return property;
}

NclNodeNesting *
ExecutionObject::getNodePerspective ()
{
  return getNodePerspective (_dataObject);
}

NclNodeNesting *
ExecutionObject::getNodePerspective (Node *node)
{
  Node *parentNode;
  NclNodeNesting *perspective;
  ExecutionObjectContext *parentObject;
  map<Node *, ExecutionObjectContext *>::iterator i;

  if (_nodeParentTable.count (node) == 0)
    {
      if (_dataObject == node)
        {
          perspective = new NclNodeNesting ();
        }
      else
        {
          return nullptr;
        }
    }
  else
    {
      parentNode = _nodeParentTable[node];

      i = _parentTable.find (parentNode);
      if (i != _parentTable.end ())
        {
          parentObject = (ExecutionObjectContext *)(i->second);

          perspective = parentObject->getNodePerspective (parentNode);
        }
      else
        {
          return nullptr;
        }
    }
  perspective->insertAnchorNode (node);
  return perspective;
}

NclEvent *
ExecutionObject::getMainEvent ()
{
  return _mainEvent;
}

bool
ExecutionObject::prepare (NclEvent *event)
{
  size_t size;
  map<Node *, ExecutionObjectContext *>::iterator i;
  NclEvent *auxEvent;
  AttributionEvent *attributeEvent;
  PropertyAnchor *attributeAnchor;
  size_t j;
  string value;

  g_assert_nonnull (event);
  g_assert (this->containsEvent (event));
  if (event->getCurrentState () != EventState::SLEEPING)
    return false;

  _mainEvent = event;

  i = _parentTable.begin ();
  while (i != _parentTable.end ())
    {
      // register parent as a mainEvent listener
     _mainEvent->addListener (i->second);
      ++i;
    }

  _transMan.prepare (_mainEvent == _wholeContent, 0);

  size = _otherEvents.size ();
  for (j = 0; j < size; j++)
    {
      auxEvent = _otherEvents[j];
      if (instanceof (AttributionEvent *, auxEvent))
        {
          attributeEvent = (AttributionEvent *)auxEvent;
          attributeAnchor = attributeEvent->getAnchor ();
          value = attributeAnchor->getValue ();
          if (value != "")
            {
              attributeEvent->setValue (value);
            }
        }
    }

  return true;
}

bool
ExecutionObject::start ()
{
  NodeEntity *entity;
  ContentNode *contentNode;
  Content *content;

  string src;
  string mime;

  g_assert_nonnull (_mainEvent);
  g_assert_nonnull (_wholeContent);

  if (this->isOccurring ())
    return true;              // nothing to do

  TRACE ("starting");

  if (instanceof (ExecutionObjectContext *, this))
    goto done;

  entity = cast (NodeEntity *, _dataObject);
  g_assert_nonnull (entity);

  contentNode = cast (ContentNode *, entity);
  g_assert_nonnull (contentNode);

  content = contentNode->getContent ();
  if (content != nullptr)
    {
      ReferenceContent *ref = cast (ReferenceContent *, content);
      g_assert_nonnull (ref);
      src = ref->getCompleteReferenceUrl ();
    }
  else
    {
      src = "";                 // empty source
    }

  // Allocate player.
  mime = contentNode->getNodeType ();
  _player = Player::createPlayer (src, mime);

  // Initialize player properties.
  if (_descriptor != nullptr)
    {
      NclFormatterRegion *formreg = _descriptor->getFormatterRegion ();
      if (formreg != nullptr)
        {
          LayoutRegion *region;
          int z, zorder;

          region = formreg->getLayoutRegion ();
          g_assert_nonnull (region);

          _player->setRect (region->getRect ());

          region->getZ (&z, &zorder);
          _player->setZ (z, zorder);
        }
      for (Parameter &p: _descriptor->getParameters ())
        _player->setProperty (p.getName (), p.getValue ());
    }

  for (Anchor *anchor: contentNode->getAnchors ())
    {
      PropertyAnchor *prop = cast (PropertyAnchor *, anchor);
      if (prop != nullptr)
        _player->setProperty (prop->getName (), prop->getValue ());
    }

  // Install attribution events.
  for (NclEvent *evt: this->getEvents ())
    {
      AttributionEvent *attevt = cast (AttributionEvent *, evt);
      if (attevt)
        attevt->setPlayer (_player);
    }

  _time = 0;
  _player->start ();
  g_assert (Ginga_Display->registerEventListener (this));

 done:
  // Start main event.
  if (instanceof (PresentationEvent *, _mainEvent))
    _mainEvent->start ();
  _transMan.start (0);
  return true;
}

bool
ExecutionObject::pause ()
{
  if (!this->isOccurring ())
    return true;

  for (NclEvent *event: this->getEvents ())
    event->pause ();

  g_assert_nonnull (_player);
  _player->pause ();

  return true;
}

bool
ExecutionObject::resume ()
{
  if (!this->isPaused ())
    return true;

  for (NclEvent *event: this->getEvents ())
    event->resume ();

  g_assert_nonnull (_player);
  _player->resume ();

  return true;
}

bool
ExecutionObject::stop ()
{
  PresentationEvent *event;

  if (this->isSleeping ())
    return true;                // nothing to do

  TRACE ("stopping");

  // Stop and destroy player.
  if (_player != nullptr)
    {
      _player->stop ();
      delete _player;
      _player = nullptr;
      _time = GINGA_TIME_NONE;
      g_assert (Ginga_Display->unregisterEventListener (this));
    }

  // Uninstall attribution events.
  for (NclEvent *evt: this->getEvents ())
    {
      AttributionEvent *attevt = cast (AttributionEvent *, evt);
      if (attevt)
        attevt->setPlayer (nullptr);
    }

  // Stop main event.
  event = cast (PresentationEvent* , _mainEvent);
  if (event != nullptr)
    {
      _mainEvent->stop ();
      _transMan.stop (0);
    }

  _transMan.resetTimeIndex ();
  removeParentListenersFromEvent (_mainEvent);

  return true;
}

bool G_GNUC_NORETURN
ExecutionObject::abort ()
{
  ERROR_NOT_IMPLEMENTED ("action 'abort' is not supported");
}

void
ExecutionObject::setHandling (bool isHandling)
{
  this->_isHandling = isHandling;
}

void
ExecutionObject::setHandler (bool isHandler)
{
  this->_isHandler = isHandler;
}

bool
ExecutionObject::selectionEvent (SDL_Keycode key, GingaTime currentTime)
{
  string selCode;
  string keyString;
  SelectionEvent *selectionEvent;
  IntervalAnchor *intervalAnchor;
  NclEvent *expectedEvent;
  Anchor *expectedAnchor;
  string anchorId = "";
  set<SelectionEvent *> *selectedEvents;
  set<SelectionEvent *>::iterator i;
  bool sleeping = isSleeping ();
  bool paused = isPaused ();
  bool selected = false;
  GingaTime intervalBegin;
  GingaTime intervalEnd;

  keyString = "UNKNOWN";
  ginga_key_table_index (key, &keyString);

  if ((!_isHandling && !_isHandler) || sleeping /*|| paused*/)
    {
      clog << "NclExecutionObject::selectionEvent Can't receive event on '";
      clog << getId () << "': isHandling = '" << _isHandling << "' ";
      clog << "isHandler = '" << _isHandler << "' ";
      clog << "isSleeping() = '" << sleeping << "' ";
      clog << "isPaused() = '" << paused << "' ";
      clog << endl;
      return false;
    }

  if (_selectionEvents.empty ())
    {
      clog << "NclExecutionObject::selectionEvent Can't receive event on '";
      clog << getId () << "': selection events is empty";
      clog << endl;
      return false;
    }

  selectedEvents = new set<SelectionEvent *>;
  i = _selectionEvents.begin ();
  while (i != _selectionEvents.end ())
    {
      selectionEvent = (SelectionEvent *)(*i);
      selCode = selectionEvent->getSelectionCode ();

      clog << "NclExecutionObject::selectionEvent(" << _id << ") event '";
      clog << selectionEvent->getId () << "' has selCode = '" << selCode;
      clog << "' (looking for key code '" << keyString << "'" << endl;

      if (keyString == selCode)
        {
          ContentAnchor *anchor = selectionEvent->getAnchor ();
          if (instanceof (LambdaAnchor *, anchor))
            {
              selectedEvents->insert (selectionEvent);
            }
          else if (instanceof (IntervalAnchor *, anchor))
            {
              intervalAnchor
                  = (IntervalAnchor *)(selectionEvent->getAnchor ());

              intervalBegin = intervalAnchor->getBegin ();
              intervalEnd = intervalAnchor->getEnd ();

              clog << "NclExecutionObject::selectionEvent(" << _id << ") ";
              clog << "interval anchor '" << intervalAnchor->getId ();
              clog << "' begins at '" << intervalBegin << "', ends at ";
              clog << intervalEnd << "' (current time is '";
              clog << currentTime << "'";
              clog << endl;

              if (intervalBegin <= currentTime
                  && intervalEnd >= currentTime)
                {
                  selectedEvents->insert (selectionEvent);
                }
            }
          else
            {
              expectedAnchor = selectionEvent->getAnchor ();
              anchorId = expectedAnchor->getId ();
              expectedEvent = getEventFromAnchorId (anchorId);
              if (expectedEvent != nullptr)
                {
                  clog << "NclExecutionObject::selectionEvent(";
                  clog << _id << ")";
                  clog << " analyzing event '";
                  clog << expectedEvent->getId ();

                  if (expectedEvent->getCurrentState ()
                      == EventState::OCCURRING)
                    {
                      selectedEvents->insert (selectionEvent);
                    }
                  else
                    {
                      clog << "' not occurring";
                    }

                  clog << "'" << endl;
                }
              else
                {
                  clog << "NclExecutionObject::selectionEvent(" << _id
                       << ")";
                  clog << " can't find event for anchorid = '";
                  clog << anchorId << "'" << endl;
                }
            }
        }
      ++i;
    }

  NclSimpleAction *fakeAct;

  i = selectedEvents->begin ();
  while (i != selectedEvents->end ())
    {
      selected = true;
      selectionEvent = (*i);

      if (_seListener != nullptr)
        {
          clog << "NclExecutionObject::selectionEvent(" << _id << ")";
          clog << " calling scheduler to execute fake action";
          clog << endl;

          fakeAct = new NclSimpleAction (selectionEvent, ACT_START);

          _seListener->scheduleAction (fakeAct);
        }

      ++i;
    }

  delete selectedEvents;
  selectedEvents = nullptr;

  return selected;
}


// -----------------------------------

/**
 * @brief Sets property.
 * @param name Property name.
 * @param from Current value.
 * @param to Updated value.
 * @param dur Duration of the attribution.
 */
void
ExecutionObject::setProperty (const string &name,
                              const string &from,
                              const string &to,
                              GingaTime dur)
{
  if (_player == nullptr)
    return;                     // nothing to do

  g_assert (GINGA_TIME_IS_VALID (dur));
  TRACE ("updating '%s.%s' from '%s' to '%s'",
         _id.c_str (), name.c_str (), from.c_str (), to.c_str ());

  if (dur > 0)
    {
      _player->schedulePropertyAnimation (name, from, to, dur);
    }
  else
    {
      _player->setProperty (name, to);
    }
}

void
ExecutionObject::handleTickEvent (arg_unused (GingaTime total),
                                  GingaTime diff,
                                  arg_unused (int frame))
{
  EventTransition *next;
  NclEvent *evt;
  GingaTime waited;
  GingaTime now;

  if (_player == nullptr)
    return;                     // nothing to do

  if (_player->getEOS ())
    {
      this->stop ();
      return;
    }

  g_assert (GINGA_TIME_IS_VALID (_time));
  _time += diff;

  g_assert (this->isOccurring ());
  g_assert_nonnull (instanceof (PresentationEvent *, _mainEvent));

  next = _transMan.nextTransition (_mainEvent);
  if (next == nullptr)
    return;

  waited = next->getTime ();
  now = _time;

  // TRACE ("now=%" GINGA_TIME_FORMAT " waited=%" GINGA_TIME_FORMAT,
  //        GINGA_TIME_ARGS (now), GINGA_TIME_ARGS (waited));

  if (now < waited)
    return;

  evt = cast (NclEvent *, next->getEvent ());
  g_assert_nonnull (evt);

  TRACE ("anchor '%s' timed out at %" GINGA_TIME_FORMAT
         ", updating transition table",
         evt->getId ().c_str(), GINGA_TIME_ARGS (now));

  _transMan.updateTransitionTable (now, _player, _mainEvent);
}

void
ExecutionObject::handleKeyEvent (SDL_EventType type, SDL_Keycode key)
{
  if (type == SDL_KEYDOWN || _player == nullptr)
    return;                     // nothing to do

  g_assert (this->isOccurring ());
  g_assert_nonnull (instanceof (PresentationEvent *, _mainEvent));

  this->selectionEvent (key, _time);
}

GINGA_FORMATTER_END
