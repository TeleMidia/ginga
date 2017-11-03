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

#ifndef FORMATTER_LINK_H
#define FORMATTER_LINK_H

#include "FormatterAction.h"
#include "FormatterCondition.h"
#include "FormatterEvent.h"

GINGA_FORMATTER_BEGIN

class FormatterLink: public IFormatterConditionListener
{
public:
  FormatterLink (GingaInternal *);
  virtual ~FormatterLink ();

  const vector <FormatterCondition *> *getConditions ();
  bool addCondition (FormatterCondition *);

  const vector <FormatterAction *> *getActions ();
  bool addAction (FormatterAction *);

  virtual vector<FormatterEvent *> getEvents ();
  void disable (bool);

  // IFormatterConditionListener
  void conditionSatisfied (FormatterCondition *);

private:
  GingaInternal *_ginga;               // ginga handle
  Scheduler *_scheduler;               // scheduler
  vector <FormatterCondition *> _conditions; // list of conditions
  vector <FormatterAction *> _actions;       // list of actions
  bool _disabled;                      // whether link is disabled
};

GINGA_FORMATTER_END

#endif // FORMATTER_LINK_H
