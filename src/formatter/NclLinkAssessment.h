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

#ifndef LINKATTRIBUTEASSESSMENT_H_
#define LINKATTRIBUTEASSESSMENT_H_

#include "ncl/EventUtil.h"
using namespace ::ginga::ncl;

#include "NclEvents.h"
#include "NclLinkAssessment.h"

GINGA_FORMATTER_BEGIN

class NclLinkAssessment
{
public:
  NclLinkAssessment () {}
  virtual ~NclLinkAssessment () {}

  virtual string getValue () = 0;
};

class NclLinkAttributeAssessment : public NclLinkAssessment
{
public:
  NclLinkAttributeAssessment (NclEvent *ev, AttributeType attrType);

  NclEvent *getEvent ();
  void setOffset (const string &offset);
  string getOffset ();
  void setEvent (NclEvent *ev);
  AttributeType getAttributeType ();
  void setAttributeType (AttributeType attrType);
  string getValue ();

private:
  NclEvent *_event;
  AttributeType _attrType;
  string _offset;

  string getAssessmentWithOffset (const string &assessmentValue);
};

class NclLinkValueAssessment : public NclLinkAssessment
{
public:
  NclLinkValueAssessment (const string &value);
  virtual ~NclLinkValueAssessment ();

  string getComparableValue ();
  void setComparableValue (const string &value);
  string getValue ();

protected:
  string _value;
};

GINGA_FORMATTER_END

#endif /*LINKATTRIBUTEASSESSMENT_H_*/
