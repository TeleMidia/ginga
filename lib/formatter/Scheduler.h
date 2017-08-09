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

#include "ncl/Ncl.h"
using namespace ::ginga::ncl;

GINGA_FORMATTER_BEGIN

class Converter;
class Scheduler : public INclActionListener
{
public:
  Scheduler ();
  virtual ~Scheduler ();
  void scheduleAction (NclSimpleAction *) override;
  void startDocument (const string &);

private:
  Converter *_converter;
  string _file;                 // path to document file
  NclDocument *_doc;            // document tree
  vector<NclEvent *> _events;   // document events

  void runAction (NclEvent *, NclSimpleAction *);
  void runActionOverComposition (ExecutionObjectContext *,
                                 NclSimpleAction *);
  void runActionOverSwitch (ExecutionObjectSwitch *, SwitchEvent *,
                            NclSimpleAction *);
  void runSwitchEvent (ExecutionObjectSwitch *, SwitchEvent *,
                       ExecutionObject *, NclSimpleAction *);
};

GINGA_FORMATTER_END

#endif // FORMATTER_SCHEDULER_H
