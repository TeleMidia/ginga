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

GINGA_NAMESPACE_BEGIN

FormatterSwitch::FormatterSwitch (Formatter *formatter, const string &id)
  :FormatterComposition (formatter, id)
{
  _selected = nullptr;
}

FormatterSwitch::~FormatterSwitch ()
{
  for (auto item: _rules)
    delete item.second;
}

bool
FormatterSwitch::exec (FormatterEvent *evt,
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
          g_assert_null (_selected);
          for (auto item: _rules)
            {
              FormatterObject *obj;
              FormatterPredicate *pred;
              FormatterEvent *e;

              obj = item.first;
              g_assert_nonnull (obj);
              pred = item.second;
              g_assert_nonnull (pred);

              if (_formatter->evalPredicate (pred))
                {
                  _selected = obj;
                  e = _selected->getLambda ();
                  g_assert_nonnull (e);
                  this->addDelayedAction (e, transition);
                  break;
                }
            }

          if (_selected == nullptr) // schedule stop
            this->addDelayedAction (evt, NclEventStateTransition::STOP);

          TRACE ("start %s@lambda", _id.c_str ());
          FormatterObject::doStart ();
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
          g_assert_nonnull (_selected);
          {
            FormatterEvent *e = _selected->getLambda ();
            g_assert_nonnull (e);
            _selected = nullptr;
            this->addDelayedAction (e, NclEventStateTransition::STOP);
            this->addDelayedAction (evt, NclEventStateTransition::STOP);
          }
          TRACE ("stop %s@lambda", _id.c_str ());
          FormatterObject::doStop ();
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

const list<pair<FormatterObject *, FormatterPredicate *>> *
FormatterSwitch::getRules ()
{
  return &_rules;
}

void
FormatterSwitch::addRule (FormatterObject *obj, FormatterPredicate *pred)
{
  g_assert_nonnull (obj);
  g_assert_nonnull (pred);
  _rules.push_back (std::make_pair (obj, pred));
}

GINGA_NAMESPACE_END
