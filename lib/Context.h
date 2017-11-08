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

#ifndef CONTEXT_H
#define CONTEXT_H

#include "Composition.h"
#include "Event.h"
#include "Link.h"

GINGA_NAMESPACE_BEGIN

class Context: public Composition
{
public:
  Context (const string &);
  virtual ~Context ();

  // Object:
  string getProperty (const string &) override;
  void setProperty (const string &, const string &,
                    GingaTime dur=0) override;
  void sendKeyEvent (const string &, bool) override;
  void sendTickEvent (GingaTime, GingaTime, GingaTime) override;
  bool startTransition (Event *, Event::Transition) override;
  void endTransition (Event *, Event::Transition) override;

  // Context:
  const list<Event *> *getPorts ();
  void addPort (Event *);

  const list<Link *> *getLinks ();
  void addLink (Link *);

private:
  list<Event *> _ports;
  list<Link *> _links;

  void toggleLinks (bool);
};

GINGA_NAMESPACE_END

#endif // CONTEXT_H
