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

#include "config.h"
#include "FormatterLink.h"

BR_PUCRIO_TELEMIDIA_GINGA_NCL_MODEL_LINK_BEGIN

FormatterLink::FormatterLink (Link *ncmLink, void *parentObject)
{

  typeSet.insert ("FormatterLink");

  this->parentObject = parentObject; // CompositeExecutionObject
  this->ncmLink = ncmLink;
  this->suspend = false;
}

FormatterLink::~FormatterLink ()
{
  parentObject = NULL;
  if (ncmLink != NULL)
    {
      ncmLink = NULL;
    }
}

void
FormatterLink::suspendLinkEvaluation (bool suspend)
{
  this->suspend = suspend;
}

Link *
FormatterLink::getNcmLink ()
{
  return ncmLink;
}

bool
FormatterLink::instanceOf (string s)
{
  if (typeSet.empty ())
    {
      return false;
    }
  else
    {
      return (typeSet.find (s) != typeSet.end ());
    }
}

BR_PUCRIO_TELEMIDIA_GINGA_NCL_MODEL_LINK_END
