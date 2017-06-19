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

#ifndef _FORMATTERLINK_H_
#define _FORMATTERLINK_H_

#include "ncl/Link.h"
using namespace ::ginga::ncl;

GINGA_FORMATTER_BEGIN

class ExecutionObjectContext;

class NclFormatterLink
{
public:
  NclFormatterLink (Link *ncmLink, ExecutionObjectContext *parentObject);
  virtual ~NclFormatterLink ();

  void suspendLinkEvaluation (bool suspend);
  Link *getNcmLink ();
  bool instanceOf (const string &s);

protected:
  Link *ncmLink;
  bool suspend;
  ExecutionObjectContext *parentObject;
  set<string> typeSet;
};

GINGA_FORMATTER_END

#endif //_FORMATTERLINK_H_
