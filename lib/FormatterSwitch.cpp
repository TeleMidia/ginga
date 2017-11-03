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
#include "FormatterSwitch.h"
#include "FormatterScheduler.h"

GINGA_NAMESPACE_BEGIN

FormatterSwitch::FormatterSwitch (Formatter *ginga, const string &id,
                                  NclNode *node)
  :FormatterContext (ginga, id, node)
{
  g_assert_nonnull (node);
  _switch = cast (NclSwitch *, node);
  g_assert_nonnull (_switch);
  _selected = nullptr;
}

FormatterSwitch::~FormatterSwitch ()
{
}

bool
FormatterSwitch::exec (FormatterEvent *evt,
                       unused (NclEventState from),
                       unused (NclEventState to),
                       NclEventStateTransition transition)
{
  switch (evt->getType ())
    {
    // ---------------------------------------------------------------------
    // Presentation event.
    // ---------------------------------------------------------------------
    case NclEventType::PRESENTATION:
      switch (transition)
        {
        case NclEventStateTransition::START:
          //
          // Start lambda.
          //
          TRACE ("start %s@lambda", _id.c_str ());
          g_assert_null (_selected);
          for (auto item: *_switch->getRules ())
            {
              NclNode *node;
              FormatterPredicate *pred;
              FormatterEvent *e;

              node = item.first;
              g_assert_nonnull (node);
              pred = item.second;
              g_assert_nonnull (pred);

              if (_scheduler->eval (pred))
                {
                  _selected = _scheduler->obtainExecutionObject (node);
                  g_assert_nonnull (_selected);
                  e = _selected->obtainEvent (NclEventType::PRESENTATION,
                                              node->getLambda (), "");
                  g_assert_nonnull (e);
                  e->transition (transition);
                  break;
                }
            }
          if (_selected == nullptr) // schedule stop
            {
              FormatterAction *act = new FormatterAction
                (evt, NclEventStateTransition::STOP);
              _delayed_new.push_back (std::make_pair (act, _time));
            }
          break;
        case NclEventStateTransition::PAUSE:
          g_assert_not_reached ();
          break;
        case NclEventStateTransition::RESUME:
          g_assert_not_reached ();
          break;
        case NclEventStateTransition::STOP:
          //
          // Stop lambda.
          //
          TRACE ("stop %s@lambda", _id.c_str ());
          g_assert_nonnull (_selected);
          {
            FormatterEvent *e = _selected->obtainLambda ();
            g_assert_nonnull (e);
            e->transition (NclEventStateTransition::STOP);
            _selected = nullptr;
            FormatterAction *act = new FormatterAction
              (evt, NclEventStateTransition::STOP);
            _delayed_new.push_back (std::make_pair (act, _time));
          }
          break;
        case NclEventStateTransition::ABORT:
          g_assert_not_reached ();
          break;
        default:
          g_assert_not_reached ();
        }
      break;

    // ---------------------------------------------------------------------
    // Attribution event.
    // ---------------------------------------------------------------------
    case NclEventType::ATTRIBUTION:
      g_assert_not_reached ();
      break;

    //----------------------------------------------------------------------
    // Selection event.
    // ---------------------------------------------------------------------
    case NclEventType::SELECTION:
      g_assert_not_reached ();
      break;
    default:
      g_assert_not_reached ();
    }
  return true;
}

GINGA_NAMESPACE_END
