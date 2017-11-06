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

#ifndef FORMATTER_SWITCH_H
#define FORMATTER_SWITCH_H

#include "FormatterContext.h"
#include "FormatterEvent.h"

GINGA_NAMESPACE_BEGIN

class FormatterSwitch: public FormatterComposition
{
public:
  FormatterSwitch (Formatter *, const string &);
  ~FormatterSwitch ();

  bool exec (FormatterEvent *, NclEventStateTransition) override;

  const list<pair<FormatterObject *, FormatterPredicate *>> *getRules ();
  void addRule (FormatterObject *, FormatterPredicate *);

private:
  list<pair<FormatterObject *, FormatterPredicate *>> _rules;
  FormatterObject *_selected;
};

GINGA_NAMESPACE_END

#endif // FORMATTER_SWITCH_H
