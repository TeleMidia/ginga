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

#ifndef FORMATTER_OBJECT_H
#define FORMATTER_OBJECT_H

#include "Formatter.h"
#include "FormatterAction.h"
#include "FormatterEvent.h"

GINGA_NAMESPACE_BEGIN

class FormatterComposition;
class FormatterMediaSettings;

class FormatterObject
{
public:
  FormatterObject (Formatter *, const string &);
  virtual ~FormatterObject ();

  string getId ();
  const vector <string> *getAliases ();
  bool hasAlias (const string &);
  bool addAlias (const string &);

  FormatterComposition *getParent ();
  void initParent (FormatterComposition *);

  FormatterEvent *obtainEvent (NclEventType, NclAnchor *, const string &);

  FormatterEvent *getEvent (NclEventType, const string &);
  FormatterEvent *getAttributionEvent (const string &);
  bool addAttributionEvent (const string &);
  FormatterEvent *getPresentationEvent (const string &);
  bool addPresentationEvent (const string &, GingaTime, GingaTime);
  FormatterEvent *getSelectionEvent (const string &);
  bool addSelectionEvent (const string &);

  FormatterEvent *getLambda ();
  bool isOccurring ();
  bool isPaused ();
  bool isSleeping ();

  virtual string getProperty (const string &);
  virtual void setProperty (const string &, const string &,
                            GingaTime dur=0);

  void scheduleAction (FormatterAction *, GingaTime);
  virtual void sendKeyEvent (const string &, bool);
  virtual void sendTickEvent (GingaTime, GingaTime, GingaTime);

  virtual bool exec (FormatterEvent *, NclEventState, NclEventState,
                     NclEventStateTransition) = 0;

protected:
  Formatter *_formatter;         // formatter handle
  string _id;                    // id
  vector<string> _aliases;       // aliases
  FormatterComposition *_parent; // parent object
  GingaTime _time;               // playback time
  map<string, string> _property; // property map

  set<FormatterEvent *> _events; // all events
  FormatterEvent *_lambda;       // lambda event

  // delayed actions
  vector<pair<FormatterAction *, GingaTime>> _delayed;
  vector<pair<FormatterAction *, GingaTime>> _delayed_new;

  virtual void doStart ();
  virtual void doStop ();
};

GINGA_NAMESPACE_END

#endif // FORMATTER_OBJECT_H
