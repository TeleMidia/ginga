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

#include "aux-ginga.h"
#include "ExecutionObject.h"

#include "ExecutionObjectContext.h"
#include "ExecutionObjectSettings.h"
#include "ExecutionObjectSwitch.h"
#include "NclEvents.h"
#include "Scheduler.h"

#include "player/Player.h"
using namespace ::ginga::player;

GINGA_FORMATTER_BEGIN

ExecutionObject::ExecutionObject (GingaInternal *ginga,
                                  const string &id,
                                  Node *node,
                                  INclActionListener *seListener)
{
  g_assert_nonnull (ginga);
  _ginga = ginga;

  _scheduler = ginga->getScheduler ();
  g_assert_nonnull (_scheduler);

  _seListener = seListener;
  _node = node;
  _wholeContent = nullptr;
  _isCompiled = false;
  _mainEvent = nullptr;

  _id = id;
  _player = nullptr;
  _time = GINGA_TIME_NONE;
  _destroying = false;

  TRACE ("%s", _id.c_str ());
  _scheduler->addObject (this);
}

ExecutionObject::~ExecutionObject ()
{
  _destroying = true;
  this->stop ();
  TRACE ("%s", _id.c_str ());
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
ExecutionObject::getNode ()
{
  return _node;
}

string
ExecutionObject::getId ()
{
  return _id;
}

ExecutionObjectContext *
ExecutionObject::getParentObject ()
{
  return getParentObject (_node);
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
  addParentObject (_node, parentObject, parentNode);
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
    _parentTable.erase (i);
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

  if (instanceof (AreaLambda *, event->getAnchor ()))
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
  _transMan.addPresentationEvent (event);
}

bool
ExecutionObject::containsEvent (NclEvent *event)
{
  return (_events.count (event->getId ()) != 0);
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

      if (_nodeParentTable.count (_node) == 0)
        {
          nodes.push_back (_node);
        }
    }

  return nodes;
}

Property *
ExecutionObject::getNCMProperty (const string &name)
{
  g_assert_nonnull (_node);
  for (auto anchor: *_node->getAnchors ())
    if (instanceof (Property *, anchor) && anchor->getId () == name)
      return cast (Property *, anchor);
  return nullptr;
}

NclNodeNesting *
ExecutionObject::getNodePerspective ()
{
  return getNodePerspective (_node);
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
      if (_node == node)
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
  Property *attributeAnchor;
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
  Media *media;
  Descriptor *desc;

  string src;
  string mime;

  g_assert_nonnull (_mainEvent);
  g_assert_nonnull (_wholeContent);

  if (this->isOccurring ())
    return true;              // nothing to do

  TRACE ("%s", _id.c_str ());

  if (instanceof (ExecutionObjectContext *, this))
    goto done;

  media = cast (Media *, _node);
  g_assert_nonnull (media);

  // Allocate player.
  src = media->getSrc ();
  mime = media->getMimeType ();
  _player = Player::createPlayer (_ginga, _id, src, mime);

  // Initialize player properties.
  desc = media->getDescriptor ();
  if (desc != nullptr)
    {
      Region *region = desc->getRegion ();
      if (region != nullptr)
        {
          string bounds;
          int z, zorder;

          bounds = xstrbuild ("%s,%s,%s,%s",
                              region->getLeft ().c_str (),
                              region->getTop ().c_str (),
                              region->getWidth ().c_str (),
                              region->getHeight ().c_str ());
          _player->setProperty ("bounds", bounds);
          region->getZ (&z, &zorder);
          _player->setZ (z, zorder);
        }

      for (auto param: *desc->getParameters ())
        _player->setProperty (param->getName (), param->getValue ());
    }

  for (auto anchor: *media->getAnchors ())
    {
      Property *prop = cast (Property *, anchor);
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
  g_assert (_ginga->registerEventListener (this));

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
    return false;               // nothing to do

  TRACE ("%s", _id.c_str ());

  // Stop and destroy player.
  if (_player != nullptr)
    {
      if (_player->getState () != Player::SLEEPING)
        _player->stop ();
      delete _player;
      _player = nullptr;
      _time = GINGA_TIME_NONE;
      g_assert (_ginga->unregisterEventListener (this));
    }

  // Uninstall attribution events.
  for (NclEvent *evt: this->getEvents ())
    {
      AttributionEvent *attevt = cast (AttributionEvent *, evt);
      if (attevt)
        attevt->setPlayer (nullptr);
    }

  if (_destroying)
    return true;                // done

  // Stop main event.
  event = cast (PresentationEvent* , _mainEvent);
  if (event != nullptr)
    {
      _mainEvent->stop ();
      _transMan.stop (0);
    }

  _transMan.resetTimeIndex ();

  return true;
}

bool G_GNUC_NORETURN
ExecutionObject::abort ()
{
  ERROR_NOT_IMPLEMENTED ("action 'abort' is not supported");
}


// -----------------------------------

/**
 * @brief Tests whether object is focused.
 * @return True if successful, or false otherwise.
 */
bool
ExecutionObject::isFocused ()
{
  if (instanceof (ExecutionObjectContext *, this)
      || instanceof (ExecutionObjectSettings *, this)
      || instanceof (ExecutionObjectSwitch *, this)
      || _player == nullptr)
    {
      return false;
    }
  return _player->isFocused ();
}

/**
 * @brief Gets property.
 * @param name Property name.
 * @return Property value.
 */
string
ExecutionObject::getProperty (const string &name)
{
  return (_player) ? _player->getProperty (name) : "";
}

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
  TRACE ("%s.%s:='%s' (previous '%s')",
         _id.c_str (), name.c_str (), from.c_str (), to.c_str ());

  if (dur > 0)
    {
      _player->schedulePropertyAnimation (name, from, to, dur);
    }
  else
    {
      _player->setProperty (name, to);
    }

  if (instanceof (ExecutionObjectSettings *, this))
    cast (ExecutionObjectSettings *, this)
      ->setProperty (name, from, to, dur);
}

void
ExecutionObject::handleTickEvent (unused (GingaTime total),
                                  GingaTime diff,
                                  unused (int frame))
{
  EventTransition *next;
  NclEvent *evt;
  GingaTime waited;
  GingaTime now;
  GingaTime dur;

  if (_player == nullptr)
    return;                     // nothing to do

  if (this->isPaused ())
    return;                     // nothing to do

  if (_player->getEOS ())
    {
      this->stop ();            // done
      return;
    }

  g_assert (GINGA_TIME_IS_VALID (_time));
  _time += diff;
  _player->incTime (diff);

  if (GINGA_TIME_IS_VALID (dur = _player->getDuration ()) && _time > dur)
    {
      this->stop ();            // done
      return;
    }

  g_assert (this->isOccurring ());
  g_assert (instanceof (PresentationEvent *, _mainEvent));

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

  TRACE ("%s.%s timed-out at %" GINGA_TIME_FORMAT,
         _id.c_str(), evt->getId ().c_str (), GINGA_TIME_ARGS (now));

  _transMan.updateTransitionTable (now, _player, _mainEvent);
}

void
ExecutionObject::handleKeyEvent (const string &key, bool press)
{
  ExecutionObjectSettings *settings;
  list<SelectionEvent *> buf;

  if (!press)
    return;                     // nothing to do

  if (this->isPaused ())
    return;                     // nothing to do

  g_assert (this->isOccurring ());
  g_assert (_player);
  g_assert (instanceof (PresentationEvent *, _mainEvent));

  if (xstrhasprefix (key, "CURSOR_") && _player->isFocused ())
    {
      string next;
      if ((key == "CURSOR_UP"
           && (next = _player->getProperty ("moveUp")) != "")
          || ((key == "CURSOR_DOWN"
               && (next = _player->getProperty ("moveDown")) != ""))
          || ((key == "CURSOR_LEFT"
               && (next = _player->getProperty ("moveLeft")) != ""))
          || ((key == "CURSOR_RIGHT"
               && (next = _player->getProperty ("moveRight")) != "")))
        {
          settings = (ExecutionObjectSettings *) _ginga->getData ("settings");
          g_assert_nonnull (settings);
          settings->scheduleFocusUpdate (next);
        }
    }

  if (_selectionEvents.empty ())
    return;                     // nothing to do

  for (SelectionEvent *evt: _selectionEvents)
    {
      Area *anchor;
      string expected;

      expected = evt->getSelectionCode ();

      if (!((expected == "NO_CODE" && key == "ENTER"
             && _player->isFocused ())
            || (expected != "NO_CODE" && key == expected)))
        {
          continue;
        }
      anchor = evt->getAnchor ();
      g_assert_nonnull (anchor);

      if (instanceof (AreaLambda *, anchor))
        {
          buf.push_back (evt);
        }
      else if (instanceof (Area *, anchor))
        {
          ERROR_NOT_IMPLEMENTED
            ("selection of temporal anchors is no supported");
        }
      else
        {
          ERROR_NOT_IMPLEMENTED
            ("selection of property anchors is not supported");
        }
    }

  for (SelectionEvent *evt: buf)
    {
      if (_seListener != nullptr)
        {
          NclSimpleAction *fakeAct =
            new NclSimpleAction (evt, EventStateTransition::START);
          _seListener->scheduleAction (fakeAct);
        }
    }

  if (buf.size () == 0)
    return;

  TRACE ("%s selected via '%s'",
         _id.c_str (), key.c_str ());
}

GINGA_FORMATTER_END
