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

#ifndef PrivateBaseContext_H_
#define PrivateBaseContext_H_

#include "ncl/PrivateBase.h"
#include "ncl/ContextNode.h"
#include "ncl/Node.h"
#include "ncl/ReferenceContent.h"
using namespace ::ginga::ncl;

#include "ncl/Rule.h"
#include "ncl/SwitchNode.h"
#include "ncl/RuleBase.h"
using namespace ::ginga::ncl;

#include "ncl/SwitchPort.h"
#include "ncl/Anchor.h"
#include "ncl/InterfacePoint.h"
#include "ncl/Port.h"
using namespace ::ginga::ncl;

#include "ncl/Connector.h"
#include "ncl/ConnectorBase.h"
using namespace ::ginga::ncl;

#include "ncl/DeviceLayout.h"
#include "ncl/LayoutRegion.h"
#include "ncl/RegionBase.h"
using namespace ::ginga::ncl;

#include "ncl/GenericDescriptor.h"
#include "ncl/DescriptorBase.h"
using namespace ::ginga::ncl;

#include "ncl/Link.h"
using namespace ::ginga::ncl;

#include "ncl/Transition.h"
#include "ncl/TransitionBase.h"
using namespace ::ginga::ncl;

#include "ncl/Base.h"
#include "ncl/NclDocument.h"
#include "ncl/IPrivateBaseContext.h"
using namespace ::ginga::ncl;

typedef struct
{
  string embeddedNclNodeId;
  string embeddedNclNodeLocation;
  NclDocument *embeddedDocument;
} EmbeddedNclData;

GINGA_FORMATTER_BEGIN

class PrivateBaseContext : public IPrivateBaseContext
{
private:
  map<string, NclDocument *> baseDocuments;
  map<string, NclDocument *> visibleDocuments;
  map<string, EmbeddedNclData *> embeddedDocuments;
  map<NclDocument *, DeviceLayout *> layouts;
  set<DeviceLayout *> layoutsGB; // layouts garbage collector
  PrivateBase *privateBase;

public:
  PrivateBaseContext ();
  virtual ~PrivateBaseContext ();

  void createPrivateBase (const string &id);

private:
  NclDocument *compileDocument (const string &location,
                                DeviceLayout *deviceLayout);

public:
  NclDocument *addDocument (const string &location, DeviceLayout *deviceLayout);

  NclDocument *embedDocument (const string &docId, const string &nodeId,
                              const string &location,
                              DeviceLayout *deviceLayout);

  void *addVisibleDocument (const string &location, DeviceLayout *deviceLayout);
  NclDocument *getVisibleDocument (const string &docId);

private:
  bool eraseVisibleDocument (const string &docLocation);

public:
  string getDocumentLocation (const string &docId);
  string getEmbeddedDocumentLocation (const string &parentDocId, const string &nodeId);
  NclDocument *getDocument (const string &id);
  NclDocument *getEmbeddedDocument (const string &parendDocId, const string &nodeId);
  vector<NclDocument *> *getDocuments ();

private:
  void removeDocumentBase (NclDocument *document, Base *base);
  void removeDocumentBases (NclDocument *document);

public:
  NclDocument *removeDocument (const string &id);
  NclDocument *removeEmbeddedDocument (const string &parentDocId, const string &nodeId);

private:
  NclDocument *getBaseDocument (const string &documentId);
  void *compileEntity (const string &location, NclDocument *document,
                       void *parentObject);

public:
  LayoutRegion *addRegion (const string &documentId, const string &regionBaseId,
                           const string &regionId, const string &xmlRegion);

  LayoutRegion *removeRegion (const string &documentId, const string &regionBaseId,
                              const string &regionId);

  RegionBase *addRegionBase (const string &documentId, const string &xmlRegionBase);

  RegionBase *removeRegionBase (const string &documentId, const string &regionBaseId);

  Rule *addRule (const string &documentId, const string &xmlRule);
  Rule *removeRule (const string &documentId, const string &ruleId);
  RuleBase *addRuleBase (const string &documentId, const string &xmlRuleBase);
  RuleBase *removeRuleBase (const string &documentId, const string &ruleBaseId);
  Transition *addTransition (const string &documentId, const string &xmlTransition);
  Transition *removeTransition (const string &documentId, const string &transitionId);
  TransitionBase *addTransitionBase (const string &documentId,
                                     const string &xmlTransitionBase);

  TransitionBase *removeTransitionBase (const string &documentId,
                                        const string &transitionBaseId);

  Connector *addConnector (const string &documentId, const string &xmlConnector);
  Connector *removeConnector (const string &documentId, const string &connectorId);
  ConnectorBase *addConnectorBase (const string &documentId,
                                   const string &xmlConnectorBase);

  ConnectorBase *removeConnectorBase (const string &documentId,
                                      const string &connectorBaseId);

  GenericDescriptor *addDescriptor (const string &documentId,
                                    const string &xmlDescriptor);

  GenericDescriptor *removeDescriptor (const string &documentId,
                                       const string &descriptorId);

  DescriptorBase *addDescriptorBase (const string &documentId,
                                     const string &xmlDescriptorBase);

  DescriptorBase *removeDescriptorBase (const string &documentId,
                                        const string &descriptorBaseId);

private:
  Base *getBase (NclDocument *document, const string &baseId);

public:
  Base *addImportBase (const string &documentId, const string &docBaseId,
                       const string &xmlImportBase);

  Base *removeImportBase (const string &documentId, const string &docBaseId,
                          const string &documentURI);

  NclDocument *addImportedDocumentBase (const string &documentId,
                                        const string &xmlImportedDocumentBase);

  NclDocument *removeImportedDocumentBase (const string &documentId,
                                           const string &importedDocumentBaseId);

  NclDocument *addImportNCL (const string &documentId, const string &xmlImportNCL);
  NclDocument *removeImportNCL (const string &documentId, const string &documentURI);
  Node *addNode (const string &documentId, const string &compositeId,
                 const string &xmlNode);
  InterfacePoint *addInterface (const string &documentId, const string &nodeId,
                                const string &xmlInterface);

  Link *addLink (const string &documentId, const string &compositeId, const string &xmlLink);
  PrivateBase *getPrivateBase ();
};

GINGA_FORMATTER_END

#endif /*PrivateBaseContext_H_*/
