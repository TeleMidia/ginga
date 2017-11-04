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

#ifndef FORMATTER_EVENT_H
#define FORMATTER_EVENT_H

#include "Formatter.h"
#include "ncl/Ncl.h"

GINGA_NAMESPACE_BEGIN

class FormatterEvent;
class IFormatterEventListener
{
public:
  virtual void eventStateChanged (FormatterEvent *,
                                  NclEventStateTransition) = 0;
};

class FormatterObject;
class FormatterEvent
{
public:
  FormatterEvent (NclEventType, FormatterObject *, NclAnchor *);
  ~FormatterEvent ();

  NclEventType getType ();
  FormatterObject *getObject ();
  NclAnchor *getAnchor ();
  NclEventState getState ();

  const vector<IFormatterEventListener *> *getListeners ();
  void addListener (IFormatterEventListener *);

  bool getParameter (const string &, string *);
  void setParameter (const string &, const string &);

  bool transition (NclEventStateTransition);

private:
  NclEventType _type;                           // event type
  FormatterObject *_object;                     // target object
  NclAnchor *_anchor;                           // target anchor
  NclEventState _state;                         // event state
  vector<IFormatterEventListener *> _listeners; // event listeners
  map<string, string> _parameters;              // parameters
};

GINGA_NAMESPACE_END

#endif // FORMATTER_EVENT_H
