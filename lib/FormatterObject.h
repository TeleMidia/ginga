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
  void addAlias (const string &);

  FormatterComposition *getParent ();
  void initParent (FormatterComposition *);

  GingaTime getTime ();

  FormatterEvent *obtainEvent (NclEventType, NclAnchor *, const string &);

  FormatterEvent *getEvent (NclEventType, const string &);
  FormatterEvent *getAttributionEvent (const string &);
  void addAttributionEvent (const string &);
  FormatterEvent *getPresentationEvent (const string &);
  void addPresentationEvent (const string &, GingaTime, GingaTime);
  FormatterEvent *getSelectionEvent (const string &);
  void addSelectionEvent (const string &);

  FormatterEvent *getLambda ();
  bool isOccurring ();
  bool isPaused ();
  bool isSleeping ();

  virtual string getProperty (const string &);
  virtual void setProperty (const string &, const string &,
                            GingaTime dur=0);


  list<pair<FormatterAction *, GingaTime>> *getDelayedActions ();
  void addDelayedAction (FormatterEvent *, NclEventStateTransition,
                         const string &value="", GingaTime delay=0);
  void clearDelayedActions ();

  virtual void sendKeyEvent (const string &, bool);
  virtual void sendTickEvent (GingaTime, GingaTime, GingaTime);

  virtual bool exec (FormatterEvent *, NclEventStateTransition) = 0;

protected:
  Formatter *_formatter;           // formatter handle
  string _id;                      // id
  vector<string> _aliases;         // aliases
  FormatterComposition *_parent;   // parent object

  GingaTime _time;                 // playback time
  map<string, string> _properties; // property map

  FormatterEvent *_lambda;       // lambda event
  set<FormatterEvent *> _events; // all events

  list<pair<FormatterAction *, GingaTime>> _delayed; // delayed actions

  virtual void doStart ();
  virtual void doStop ();
};

GINGA_NAMESPACE_END

#endif // FORMATTER_OBJECT_H
