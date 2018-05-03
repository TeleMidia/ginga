/* Copyright (C) 2006-2018 PUC-Rio/Laboratorio TeleMidia

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
along with Ginga.  If not, see <https://www.gnu.org/licenses/>.  */

#include "aux-ginga.h"
#include "Switch.h"

#include "Document.h"

GINGA_NAMESPACE_BEGIN

// Public.

Switch::Switch (const string &id) : Composition (id)
{
  _selected = nullptr;
}

Switch::~Switch ()
{
  for (auto item : _rules)
    delete item.second;
}

// Public: Object.

string
Switch::getObjectTypeAsString ()
{
  return "Switch";
}

string
Switch::toString ()
{
  string str;

  str = Object::toString ();
  if (_rules.size () > 0)
    {
      int i = 1;
      str += "  rules:\n";
      for (auto rule : _rules)
        {
          str += xstrbuild ("    #%d %s if %s\n", i++,
                            rule.first->getId ().c_str (),
                            rule.second->toString ().c_str ());
        }
    }

  return str;
}

bool
Switch::beforeTransition (Event *evt, Event::Transition transition)
{
  set<Event *> switchPort_evts; // The possible selected events.
  if (evt->isLambda ())
    {
      // If we are acting on the switch's @lambda all the @lambda children
      // events can be selected.
      for (Object *obj : *(getChildren ()))
        switchPort_evts.insert (obj->getLambda ());
    }
  else
    {
      // If we are action on a switchPort we should select only the events
      // being mapped by the switchPort (and the defaultComponent?).
      auto it = _switchPorts.find (evt->getId ());
      g_assert (it != _switchPorts.end ());
      for (Event *e : it->second)
        switchPort_evts.insert (e);
    }

  switch (evt->getType ())
    {
    case Event::PRESENTATION:
      TRACE ("Presentation event on switchPort: %s.",
             evt->getId ().c_str ());

      switch (transition)
        {
        case Event::START:
          g_assert_null (_selected);
          for (auto item : _rules)
            {
              Object *obj;
              Predicate *pred;
              Event *selected_evt;

              obj = item.first;
              g_assert_nonnull (obj);
              pred = item.second;
              g_assert_nonnull (pred);

              // Check if the (possible) selected object is in the
              // switchPort's
              // list of possible events.
              selected_evt = nullptr;
              for (Event *e : switchPort_evts)
                {
                  if (obj == e->getObject ())
                    selected_evt = e;
                }
              if (selected_evt == nullptr)
                continue;

              if (_doc->evalPredicate (pred))
                {
                  g_assert_nonnull (selected_evt);
                  // Found one valid predicate.
                  if (selected_evt->transition (transition))
                    {
                      _selected = obj;
                      return true;
                    } // Found one valid predicate, but its transition
                      // doesn't work.
                  else
                    return false;
                }
            }
          // Not found valid predicate, then transition failed.
          return false;
          break;

        case Event::PAUSE:
        case Event::RESUME:
        case Event::ABORT:
        case Event::STOP:
          if (_selected != nullptr)
            {
              Event *lambda = _selected->getLambda ();
              g_assert_nonnull (lambda);
              lambda->transition (transition);
              _selected = nullptr;
            }
          break;

        default:
          g_assert_not_reached ();
        }
      break;

    case Event::ATTRIBUTION:
    case Event::SELECTION:
    default:
      g_assert_not_reached ();
    }
  return true;
}

bool
Switch::afterTransition (Event *evt, Event::Transition transition)
{
  switch (evt->getType ())
    {
    case Event::PRESENTATION:
      TRACE ("Presentation event on switchPort: %s.",
             evt->getId ().c_str ());
      switch (transition)
        {
        case Event::START:
          Object::doStart ();
          TRACE ("start %s at %" GINGA_TIME_FORMAT,
                 evt->getFullId ().c_str (), GINGA_TIME_ARGS (_time));
          if (_selected == nullptr)
            _doc->evalAction (evt, Event::STOP);
          break;
        case Event::PAUSE:
          TRACE ("pause %s at %" GINGA_TIME_FORMAT,
                 evt->getFullId ().c_str (), GINGA_TIME_ARGS (_time));
          break;
        case Event::RESUME:
          TRACE ("resume %s at %" GINGA_TIME_FORMAT,
                 evt->getFullId ().c_str (), GINGA_TIME_ARGS (_time));
          break;
        case Event::STOP:
          TRACE ("stop %s at %" GINGA_TIME_FORMAT,
                 evt->getFullId ().c_str (), GINGA_TIME_ARGS (_time));
          Object::doStop ();
          break;
        case Event::ABORT:
          TRACE ("abort %s at %" GINGA_TIME_FORMAT,
                 evt->getFullId ().c_str (), GINGA_TIME_ARGS (_time));
          Object::doStop ();
          break;

        default:
          g_assert_not_reached ();
        }
      break;

    case Event::ATTRIBUTION:
    case Event::SELECTION:
    default:
      g_assert_not_reached ();
    }
  return true;
}

// Public.

const list<pair<Object *, Predicate *> > *
Switch::getRules ()
{
  return &_rules;
}

void
Switch::addRule (Object *obj, Predicate *pred)
{
  g_assert_nonnull (obj);
  g_assert_nonnull (pred);
  _rules.push_back (std::make_pair (obj, pred));
}

const map<string, list<Event *> > *
Switch::getSwitchPorts ()
{
  return &_switchPorts;
}

void
Switch::addSwitchPort (const string &id, const list<Event *> &evts)
{
  addPresentationEvent (id, id);

  TRACE ("Adding switchPort %s to %s mapping %u evts.", id.c_str (),
         getId ().c_str (), (guint) evts.size ());

  _switchPorts[id] = evts;
}

GINGA_NAMESPACE_END
