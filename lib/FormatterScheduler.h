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

#include "FormatterObject.h"
#include "FormatterContext.h"
#include "FormatterSwitch.h"
#include "FormatterMedia.h"
#include "FormatterMediaSettings.h"
#include "FormatterAction.h"
#include "FormatterCondition.h"
#include "FormatterEvent.h"
#include "FormatterLink.h"
#include "ncl/Ncl.h"

GINGA_NAMESPACE_BEGIN

class Formatter;
class FormatterScheduler
{
public:
  FormatterScheduler (Formatter *);
  ~FormatterScheduler ();
  bool run (NclDocument *);

  const set<FormatterObject *> *getObjects ();
  const set<FormatterMedia *> *getMediaObjects ();
  FormatterMediaSettings *getSettings ();

  FormatterObject *getObjectById (const string &);
  FormatterObject *getObjectByIdOrAlias (const string &);
  bool getObjectPropertyByRef (const string &, string *);
  bool addObject (FormatterObject *);

  void redraw (cairo_t *);
  void resize (int, int);
  void sendKeyEvent (const string &, bool);
  void sendTickEvent (GingaTime, GingaTime, GingaTime);

  FormatterObject *obtainExecutionObject (NclNode *);
  bool eval (FormatterPredicate *);

private:
  Formatter *_ginga;                   // formatter handle
  NclDocument *_doc;                   // the document tree
  set<FormatterObject *> _objects;     // all objects
  set<FormatterMedia *> _mediaObjects; // media objects
  FormatterMediaSettings *_settings;   // settings object

  FormatterEvent *obtainFormatterEventFromBind (NclBind *);
  FormatterLink *obtainFormatterLink (NclLink *);
};

GINGA_NAMESPACE_END

#endif // FORMATTER_SCHEDULER_H
