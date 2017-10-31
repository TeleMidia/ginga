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

#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "GingaInternal.h"

#include "ExecutionObject.h"
#include "ExecutionObjectContext.h"
#include "ExecutionObjectSwitch.h"
#include "Converter.h"
#include "NclAction.h"

#include "ncl/Ncl.h"
using namespace ::ginga::ncl;

class GingaInternal;

GINGA_FORMATTER_BEGIN

class Converter;
class Scheduler
{
public:
  Scheduler (GingaInternal *);
  virtual ~Scheduler ();
  bool run (NclDocument *);

  ExecutionObjectSettings *getSettings ();
  const set<ExecutionObject *> *getObjects ();
  ExecutionObject *getObjectById (const string &);
  ExecutionObject *getObjectByIdOrAlias (const string &);
  bool getObjectPropertyByRef (const string &, string *);
  bool addObject (ExecutionObject *);

  void redraw (cairo_t *);
  void resize (int, int);
  void sendKeyEvent (const string &, bool);
  void sendTickEvent (GingaTime, GingaTime, GingaTime);

  void scheduleAction (NclAction *);

private:
  GingaInternal *_ginga;              // ginga handle
  Converter *_converter;              // converter object
  NclDocument *_doc;                  // document tree
  ExecutionObjectSettings *_settings; // settings object
  set<ExecutionObject *> _objects;    // document objects

  void runAction (NclEvent *, NclAction *);
  void runActionOverComposition (ExecutionObjectContext *,
                                 NclAction *);
  void runActionOverSwitch (ExecutionObjectSwitch *, SwitchEvent *,
                            NclAction *);
  void runSwitchEvent (ExecutionObjectSwitch *, SwitchEvent *,
                       ExecutionObject *, NclAction *);
};

GINGA_FORMATTER_END

#endif // SCHEDULER_H
