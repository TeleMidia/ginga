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

#ifndef FORMATTER_SCHEDULER_H
#define FORMATTER_SCHEDULER_H

#include "Converter.h"
#include "NclActions.h"
#include "RuleAdapter.h"

#include "ncl/NclDocument.h"
using namespace ::ginga::ncl;

GINGA_FORMATTER_BEGIN

class Converter;
class FocusManager;

class Scheduler : public INclActionListener
{
private:
  Converter *compiler;
  Settings *settings;

  string file;                        // NCL file path
  NclDocument *doc;                   // NCL document tree
  vector<NclEvent *> events; // document events
  set<NclSimpleAction *> actions; // document actions

public:
  Scheduler ();
  virtual ~Scheduler ();

  void addAction (NclSimpleAction *) override;
  void removeAction (NclSimpleAction *) override;
  void scheduleAction (NclSimpleAction *) override;

  void startDocument (const string &);

private:

  void runAction (NclEvent *event, NclSimpleAction *action);

  void
  runActionOverComposition (ExecutionObjectContext *compositeObject,
                            NclSimpleAction *action);

  void runActionOverSwitch (ExecutionObjectSwitch *switchObject,
                            SwitchEvent *event,
                            NclSimpleAction *action);

  void runSwitchEvent (ExecutionObjectSwitch *switchObject,
                       SwitchEvent *switchEvent,
                       ExecutionObject *selectedObject,
                       NclSimpleAction *action);
};

GINGA_FORMATTER_END

#endif // FORMATTER_SCHEDULER_H
