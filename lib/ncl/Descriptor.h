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

#ifndef DESCRIPTOR_H
#define DESCRIPTOR_H

#include "Parameter.h"
#include "Region.h"
#include "Transition.h"

GINGA_NCL_BEGIN

class Descriptor : public Entity
{
public:
  Descriptor (NclDocument *, const string &);
  virtual ~Descriptor ();

  Region *getRegion ();
  void initRegion (Region *);

  void addParameter (Parameter *);
  const vector<Parameter *> *getParameters ();
  Parameter *getParameter (const string &);

  void addInputTransition (Transition *);
  const vector<Transition *> *getInputTransitions ();

  void addOutputTransition (Transition *);
  const vector<Transition *> *getOutputTransitions ();

private:
  Region *_region;
  vector<Parameter *> _parameters;
  vector<Transition *> _inputTransitions;
  vector<Transition *> _outputTransitions;
};

GINGA_NCL_END

#endif // DESCRIPTOR_H
