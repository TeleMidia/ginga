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

#include "config.h"
#include "NclLinkSimpleAction.h"
#include "NclLinkAssignmentAction.h"

GINGA_FORMATTER_BEGIN

NclLinkSimpleAction::NclLinkSimpleAction (NclFormatterEvent *event, short type)
    : NclLinkAction ()
{

  this->event = event;
  actionType = type;
  listener = NULL;

  typeSet.insert ("NclLinkSimpleAction");
}

NclLinkSimpleAction::~NclLinkSimpleAction ()
{
  clog << "NclLinkSimpleAction::~NclLinkSimpleAction" << endl;
  if (listener != NULL)
    {
      listener->removeAction (this);
    }

  listener = NULL;
  event = NULL;
}

NclFormatterEvent *
NclLinkSimpleAction::getEvent ()
{
  return event;
}

short
NclLinkSimpleAction::getType ()
{
  return actionType;
}

void
NclLinkSimpleAction::setSimpleActionListener (INclLinkActionListener *listener)
{

  if (listener != NULL)
    {
      listener->addAction (this);
    }

  this->listener = listener;
}

vector<NclFormatterEvent *> *
NclLinkSimpleAction::getEvents ()
{
  if (event == NULL)
    return NULL;

  vector<NclFormatterEvent *> *events;
  events = new vector<NclFormatterEvent *>;

  events->push_back (event);
  return events;
}

vector<NclLinkAction *> *
NclLinkSimpleAction::getImplicitRefRoleActions ()
{
  vector<NclLinkAction *> *actions;
  string attVal = "", durVal = "", byVal = "";
  Animation *anim;

  actions = new vector<NclLinkAction *>;

  if (this->instanceOf ("NclLinkAssignmentAction"))
    {
      attVal = ((NclLinkAssignmentAction *)this)->getValue ();
      anim = ((NclLinkAssignmentAction *)this)->getAnimation ();

      if (anim != NULL)
        {
          durVal = anim->getDuration ();
          byVal = anim->getBy ();
        }

      if ((byVal != "" && byVal.substr (0, 1) == "$")
          || (durVal != "" && durVal.substr (0, 1) == "$")
          || (attVal != "" && attVal.substr (0, 1) == "$"))
        {

          if (event->instanceOf ("NclAttributionEvent"))
            {
              actions->push_back ((NclLinkAction *)this);
            }
        }
    }

  if (actions->empty ())
    {
      delete actions;
      return NULL;
    }

  return actions;
}

void
NclLinkSimpleAction::run ()
{
  NclLinkAction::run ();

  if (listener != NULL)
    {
      listener->scheduleAction (satisfiedCondition, (void *)this);
    }

  if (actionType == SimpleAction::ACT_START)
    {
      /*clog << "NclLinkSimpleAction::run notify action INIT ";
      if (event != NULL) {
              clog << "'" << event->getId() << "'";
      }
      clog << endl;*/
      notifyProgressionListeners (true);
    }
  else
    {
      /*clog << "NclLinkSimpleAction::run notify action END ";
      if (event != NULL) {
              clog << "'" << event->getId() << "'";
      }
      clog << endl;*/
      notifyProgressionListeners (false);
    }
}

GINGA_FORMATTER_END
