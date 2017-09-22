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

#ifndef NCLDOCUMENT_H_
#define NCLDOCUMENT_H_

#include "Base.h"
#include "Connector.h"
#include "ConnectorBase.h"
#include "Context.h"
#include "Descriptor.h"
#include "DescriptorBase.h"
#include "Media.h"
#include "Node.h"
#include "Region.h"
#include "RegionBase.h"
#include "Rule.h"
#include "RuleBase.h"
#include "Transition.h"
#include "TransitionBase.h"

GINGA_NCL_BEGIN

class NclDocument
{
public:
  NclDocument (const string &, const string &);
  ~NclDocument ();
  void setBody (Context *);

  // -----------------------------------------------------------------------

  NclDocument *getParentDocument ();
  void setParentDocument (NclDocument *);
  string getDocumentPerspective ();
  string getDocumentLocation ();
  bool addDocument (NclDocument *document, const string &alias,
                    const string &location);
  Connector *getConnector (const string &connectorId);
  ConnectorBase *getConnectorBase ();
  Transition *getTransition (const string &transitionId);
  TransitionBase *getTransitionBase ();
  Descriptor *getDescriptor (const string &descriptorId);
  DescriptorBase *getDescriptorBase ();
  NclDocument *getDocument (const string &documentId);
  string getDocumentAlias (NclDocument *document);
  Context *getBody ();
  string getDocumentLocation (NclDocument *document);
  vector<NclDocument *> *getDocuments ();
  string getId ();

  Node *getNode (const string &nodeId);
  Region *getRegion (const string &regionId);

  RegionBase *getRegionBase (int devClass);
  RegionBase *getRegionBase (const string &regionBaseId);
  map<int, RegionBase *> *getRegionBases ();
  Rule *getRule (const string &ruleId);
  RuleBase *getRuleBase ();
  vector<Node *> *getSettingsNodes ();
  bool removeDocument (NclDocument *document);
  void setConnectorBase (ConnectorBase *_connectorBase);
  void setTransitionBase (TransitionBase *_transitionBase);
  void setDescriptorBase (DescriptorBase *_descriptorBase);
  void setDocumentAlias (NclDocument *document, const string &alias);

  void setDocumentLocation (NclDocument *document, const string &location);
  void setId (const string &_id);
  void addRegionBase (RegionBase *regionBase);
  void setRuleBase (RuleBase *_ruleBase);
  void removeRegionBase (const string &regionBaseId);

private:
  Context *_body;
  ConnectorBase *_connectorBase;
  DescriptorBase *_descriptorBase;
  map<string, NclDocument *> _documentAliases;
  vector<NclDocument *> _documentBase;
  map<string, NclDocument *> _documentLocations;
  string _id;
  map<int, RegionBase *> _regionBases;
  RuleBase *_ruleBase;
  TransitionBase *_transitionBase;

  string _docLocation;
  NclDocument *_parentDocument;

  Node *getNodeLocally (const string &nodeId);
  Region *getRegion (const string &regionId, RegionBase *regionBase);
};

GINGA_NCL_END

#endif /*NCLDOCUMENT_H_*/
