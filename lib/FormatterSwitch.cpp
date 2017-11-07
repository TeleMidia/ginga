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


// Public.

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


// Public: FormatterObject.

bool
FormatterSwitch::startTransition (FormatterEvent *evt,
                                  NclEventStateTransition transition)
{
  switch (evt->getType ())
    {
    case NclEventType::PRESENTATION:
      g_assert (evt->isLambda ());
      switch (transition)
        {
        case NclEventStateTransition::START:
          g_assert_null (_selected);
          for (auto item: _rules)
            {
              FormatterObject *obj;
              FormatterPredicate *pred;
              FormatterEvent *lambda;

              obj = item.first;
              g_assert_nonnull (obj);
              pred = item.second;
              g_assert_nonnull (pred);

              if (_formatter->evalPredicate (pred))
                {
                  lambda = obj->getLambda ();
                  g_assert_nonnull (lambda);
                  if (lambda->transition (transition))
                    _selected = obj;
                  break;
                }
            }
          break;

        case NclEventStateTransition::STOP:
          if (_selected != nullptr)
            {
              FormatterEvent *lambda = _selected->getLambda ();
              g_assert_nonnull (lambda);
              lambda->transition (transition);
              _selected = nullptr;
            }
          break;

        default:
          g_assert_not_reached ();
        }
      break;

    case NclEventType::ATTRIBUTION:
    case NclEventType::SELECTION:
    default:
      g_assert_not_reached ();
    }
  return true;
}

void
FormatterSwitch::endTransition (FormatterEvent *evt,
                                NclEventStateTransition transition)
{
  switch (evt->getType ())
    {
    case NclEventType::PRESENTATION:
      g_assert (evt->isLambda ());
     switch (transition)
        {
        case NclEventStateTransition::START:
          FormatterObject::doStart ();
          TRACE ("start %s@lambda", _id.c_str ());
          if (_selected == nullptr)
            _formatter->evalAction (evt, NclEventStateTransition::STOP);
          break;

        case NclEventStateTransition::STOP:
          FormatterObject::doStop ();
          TRACE ("stop %s@lambda", _id.c_str ());
          break;

        default:
          g_assert_not_reached ();
        }
     break;

    case NclEventType::ATTRIBUTION:
    case NclEventType::SELECTION:
    default:
      g_assert_not_reached ();
    }
}


// Public.

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
