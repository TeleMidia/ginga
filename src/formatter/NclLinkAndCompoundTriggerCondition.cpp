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
#include "NclLinkAndCompoundTriggerCondition.h"

GINGA_FORMATTER_BEGIN

NclLinkAndCompoundTriggerCondition::NclLinkAndCompoundTriggerCondition ()
    : NclLinkCompoundTriggerCondition ()
{
  typeSet.insert ("NclLinkAndCompoundTriggerCondition");
}

NclLinkAndCompoundTriggerCondition::~NclLinkAndCompoundTriggerCondition ()
{
  vector<NclLinkCondition *>::iterator i;
  NclLinkCondition *l;

  isDeleting = true;
  unsatisfiedConditions.clear ();

  i = statements.begin ();
  while (i != statements.end ())
    {
      l = *i;
      if (l != NULL)
        {
          delete l;
          l = NULL;
        }
      ++i;
    }

  statements.clear ();
}

void
NclLinkAndCompoundTriggerCondition::addCondition (
    NclLinkCondition *condition)
{
  if (condition == NULL)
    {
      return;
    }

  if (condition->instanceOf ("NclLinkTriggerCondition"))
    {
      unsatisfiedConditions.push_back (condition);
      NclLinkCompoundTriggerCondition::addCondition (condition);
    }
  else if (condition->instanceOf ("NclLinkStatement"))
    {
      statements.push_back (condition);
    }
  else
    {
      clog << "NclLinkAndCompoundTriggerCondition::";
      clog << "addCondition Warning! statements ";
      clog << "trying to add a condition !instanceOf(NclLinkStatement)";
      clog << " and !instanceOf(NclLinkTriggerCondition)";
      clog << endl;
    }
}

void
NclLinkAndCompoundTriggerCondition::conditionSatisfied (void *condition)
{
  vector<NclLinkCondition *>::iterator i;
  i = unsatisfiedConditions.begin ();
  while (i != unsatisfiedConditions.end ())
    {
      if ((*i) == (NclLinkCondition *)condition)
        {
          unsatisfiedConditions.erase (i);
          if (unsatisfiedConditions.empty ())
            {
              break;
            }
          else
            {
              i = unsatisfiedConditions.begin ();
            }
        }
      else
        {
          ++i;
        }
    }

  if (unsatisfiedConditions.empty ())
    {
      for (i = conditions.begin (); i != conditions.end (); ++i)
        {
          unsatisfiedConditions.push_back (*i);
        }

      for (i = statements.begin (); i != statements.end (); ++i)
        {
          if (!(((NclLinkStatement *)(*i))->evaluate ()))
            {
              notifyConditionObservers (
                  NclLinkTriggerListener::EVALUATION_ENDED);

              return;
            }
        }

      NclLinkTriggerCondition::conditionSatisfied (condition);
    }
  else
    {
      notifyConditionObservers (NclLinkTriggerListener::EVALUATION_ENDED);
    }
}

vector<NclFormatterEvent *> *
NclLinkAndCompoundTriggerCondition::getEvents ()
{
  vector<NclFormatterEvent *> *events;
  vector<NclFormatterEvent *> *eventsToAdd;

  vector<NclLinkCondition *>::iterator i;
  vector<NclFormatterEvent *>::iterator j;

  events = NclLinkCompoundTriggerCondition::getEvents ();
  if (events != NULL)
    {
      for (i = statements.begin (); i != statements.end (); ++i)
        {
          eventsToAdd = (*i)->getEvents ();
          if (eventsToAdd != NULL)
            {
              for (j = eventsToAdd->begin (); j != eventsToAdd->end (); ++j)
                {
                  events->push_back (*j);
                }
              delete eventsToAdd;
              eventsToAdd = NULL;
            }
        }
    }

  return events;
}

GINGA_FORMATTER_END
