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

#ifndef FORMATTER_CONTEXT_H
#define FORMATTER_CONTEXT_H

#include "FormatterComposition.h"
#include "FormatterEvent.h"
#include "FormatterLink.h"

GINGA_NAMESPACE_BEGIN

class FormatterContext: public FormatterComposition
{
public:
  FormatterContext (Formatter *, const string &);
  virtual ~FormatterContext ();

  // FormatterObject:
  string getProperty (const string &) override;
  void setProperty (const string &, const string &,
                    GingaTime dur=0) override;
  void sendKeyEvent (const string &, bool) override;
  void sendTickEvent (GingaTime, GingaTime, GingaTime) override;
  bool startTransition (FormatterEvent *, NclEventStateTransition) override;
  void endTransition (FormatterEvent *, NclEventStateTransition) override;

  // FormatterContext:
  const list<FormatterEvent *> *getPorts ();
  void addPort (FormatterEvent *);

  const list<FormatterLink *> *getLinks ();
  void addLink (FormatterLink *);

private:
  list<FormatterEvent *> _ports;
  list<FormatterLink *> _links;

  void toggleLinks (bool);
};

GINGA_NAMESPACE_END

#endif // FORMATTER_CONTEXT_H
