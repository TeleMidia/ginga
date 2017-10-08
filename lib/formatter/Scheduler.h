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
class Scheduler : public INclActionListener
{
public:
  Scheduler (GingaInternal *);
  virtual ~Scheduler ();
  bool run (const string &, string *);

  set<NclEvent *> *getEvents ();
  bool hasEvent (NclEvent *);
  NclEvent *getEventById (const string &);
  void addEvent (NclEvent *);

  set<ExecutionObject *> *getObjects ();
  bool hasObject (ExecutionObject *);
  ExecutionObject *getObjectById (const string &);
  void addObject (ExecutionObject *);

  void scheduleAction (NclAction *) override;

private:
  GingaInternal *_ginga;        // ginga handle
  Converter *_converter;        // converter object
  string _file;                 // path to document file
  NclDocument *_doc;            // document tree

  set<NclEvent *> _events;         // document events
  set<ExecutionObject *> _objects; // document objects

  void runAction (NclEvent *, NclAction *);
  void runActionOverComposition (ExecutionObjectContext *,
                                 NclAction *);
  void runActionOverSwitch (ExecutionObjectSwitch *, SwitchEvent *,
                            NclAction *);
  void runSwitchEvent (ExecutionObjectSwitch *, SwitchEvent *,
                       ExecutionObject *, NclAction *);
};

GINGA_FORMATTER_END

#endif // FORMATTER_SCHEDULER_H
