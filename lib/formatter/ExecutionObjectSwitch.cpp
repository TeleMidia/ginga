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
#include "ExecutionObjectSwitch.h"
#include "Scheduler.h"

GINGA_FORMATTER_BEGIN

ExecutionObjectSwitch::ExecutionObjectSwitch (GingaInternal *ginga,
                                              const string &id,
                                              Node *node)
  : ExecutionObjectContext (ginga, id, node)
{
  g_assert_nonnull (node);
  _switch = cast (Switch *, node);
  g_assert_nonnull (_switch);
  _selected = nullptr;
}

ExecutionObjectSwitch::~ExecutionObjectSwitch ()
{
}

bool
ExecutionObjectSwitch::exec (FormatterEvent *evt,
                             unused (EventState from),
                             unused (EventState to),
                             EventStateTransition transition)
{
  switch (evt->getType ())
    {
    // ---------------------------------------------------------------------
    // Presentation event.
    // ---------------------------------------------------------------------
    case EventType::PRESENTATION:
      switch (transition)
        {
        case EventStateTransition::START:
          //
          // Start lambda.
          //
          TRACE ("start %s@lambda", _id.c_str ());
          g_assert_null (_selected);
          for (auto item: *_switch->getRules ())
            {
              Node *node;
              Predicate *pred;
              FormatterEvent *e;

              node = item.first;
              g_assert_nonnull (node);
              pred = item.second;
              g_assert_nonnull (pred);

              if (_scheduler->eval (pred))
                {
                  _selected = _scheduler->obtainExecutionObject (node);
                  g_assert_nonnull (_selected);
                  e = _selected->obtainEvent (EventType::PRESENTATION,
                                              node->getLambda (), "");
                  g_assert_nonnull (e);
                  e->transition (transition);
                  break;
                }
            }
          if (_selected == nullptr) // schedule stop
            {
              FormatterAction *act = new FormatterAction
                (evt, EventStateTransition::STOP);
              _delayed_new.push_back (std::make_pair (act, _time));
            }
          break;
        case EventStateTransition::PAUSE:
          g_assert_not_reached ();
          break;
        case EventStateTransition::RESUME:
          g_assert_not_reached ();
          break;
        case EventStateTransition::STOP:
          //
          // Stop lambda.
          //
          TRACE ("stop %s@lambda", _id.c_str ());
          g_assert_nonnull (_selected);
          {
            FormatterEvent *e = _selected->obtainLambda ();
            g_assert_nonnull (e);
            e->transition (EventStateTransition::STOP);
            _selected = nullptr;
            FormatterAction *act = new FormatterAction
              (evt, EventStateTransition::STOP);
            _delayed_new.push_back (std::make_pair (act, _time));
          }
          break;
        case EventStateTransition::ABORT:
          g_assert_not_reached ();
          break;
        default:
          g_assert_not_reached ();
        }
      break;

    // ---------------------------------------------------------------------
    // Attribution event.
    // ---------------------------------------------------------------------
    case EventType::ATTRIBUTION:
      g_assert_not_reached ();
      break;

    //----------------------------------------------------------------------
    // Selection event.
    // ---------------------------------------------------------------------
    case EventType::SELECTION:
      g_assert_not_reached ();
      break;
    default:
      g_assert_not_reached ();
    }
  return true;
}

GINGA_FORMATTER_END
