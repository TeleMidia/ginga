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

#ifndef FORMATTER_MEDIATOR_H
#define FORMATTER_MEDIATOR_H

#include "FormatterConverter.h"
#include "FormatterScheduler.h"
#include "NclExecutionObject.h"

#include "ncl/NclDocument.h"
using namespace ::ginga::ncl;

GINGA_FORMATTER_BEGIN

class FormatterMediator
{
private:
  NclPlayerData *data;
  string currentFile;
  NclDocument *currentDocument;

  map<string, NclFormatterEvent *> documentEvents;
  map<string, vector<NclFormatterEvent *> *> documentEntryEvents;
  FormatterScheduler *scheduler;
  RuleAdapter *ruleAdapter;
  FormatterConverter *compiler;
  AdapterPlayerManager *playerManager;
  DeviceLayout *deviceLayout;
  FormatterMultiDevice *multiDevice;
  PresentationContext *presContext;

private:
  bool compileDocument (const string &documentId);

public:
  FormatterMediator ();
  ~FormatterMediator ();
  void *addDocument (const string &file);
  bool play ();
  void stop ();
};

GINGA_FORMATTER_END

#endif /* FORMATTER_MEDIATOR_H */
