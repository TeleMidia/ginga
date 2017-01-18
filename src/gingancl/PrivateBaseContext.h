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

BR_PUCRIO_TELEMIDIA_GINGA_NCL_BEGIN

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

  void createPrivateBase (string id);

private:
  NclDocument *compileDocument (string location, DeviceLayout *deviceLayout);

public:
  NclDocument *addDocument (string location, DeviceLayout *deviceLayout);

  NclDocument *embedDocument (string docId, string nodeId, string location,
                              DeviceLayout *deviceLayout);

  void *addVisibleDocument (string location, DeviceLayout *deviceLayout);
  NclDocument *getVisibleDocument (string docId);

private:
  bool eraseVisibleDocument (string docLocation);

public:
  string getDocumentLocation (string docId);
  string getEmbeddedDocumentLocation (string parentDocId, string nodeId);
  NclDocument *getDocument (string id);
  NclDocument *getEmbeddedDocument (string parendDocId, string nodeId);
  vector<NclDocument *> *getDocuments ();

private:
  void removeDocumentBase (NclDocument *document, Base *base);
  void removeDocumentBases (NclDocument *document);

public:
  NclDocument *removeDocument (string id);
  NclDocument *removeEmbeddedDocument (string parentDocId, string nodeId);

private:
  NclDocument *getBaseDocument (string documentId);
  void *compileEntity (string location, NclDocument *document,
                       void *parentObject);

public:
  LayoutRegion *addRegion (string documentId, string regionBaseId,
                           string regionId, string xmlRegion);

  LayoutRegion *removeRegion (string documentId, string regionBaseId,
                              string regionId);

  RegionBase *addRegionBase (string documentId, string xmlRegionBase);

  RegionBase *removeRegionBase (string documentId, string regionBaseId);

  Rule *addRule (string documentId, string xmlRule);
  Rule *removeRule (string documentId, string ruleId);
  RuleBase *addRuleBase (string documentId, string xmlRuleBase);
  RuleBase *removeRuleBase (string documentId, string ruleBaseId);
  Transition *addTransition (string documentId, string xmlTransition);
  Transition *removeTransition (string documentId, string transitionId);
  TransitionBase *addTransitionBase (string documentId,
                                     string xmlTransitionBase);

  TransitionBase *removeTransitionBase (string documentId,
                                        string transitionBaseId);

  Connector *addConnector (string documentId, string xmlConnector);
  Connector *removeConnector (string documentId, string connectorId);
  ConnectorBase *addConnectorBase (string documentId, string xmlConnectorBase);

  ConnectorBase *removeConnectorBase (string documentId,
                                      string connectorBaseId);

  GenericDescriptor *addDescriptor (string documentId, string xmlDescriptor);

  GenericDescriptor *removeDescriptor (string documentId, string descriptorId);

  DescriptorBase *addDescriptorBase (string documentId,
                                     string xmlDescriptorBase);

  DescriptorBase *removeDescriptorBase (string documentId,
                                        string descriptorBaseId);

private:
  Base *getBase (NclDocument *document, string baseId);

public:
  Base *addImportBase (string documentId, string docBaseId,
                       string xmlImportBase);

  Base *removeImportBase (string documentId, string docBaseId,
                          string documentURI);

  NclDocument *addImportedDocumentBase (string documentId,
                                        string xmlImportedDocumentBase);

  NclDocument *removeImportedDocumentBase (string documentId,
                                           string importedDocumentBaseId);

  NclDocument *addImportNCL (string documentId, string xmlImportNCL);
  NclDocument *removeImportNCL (string documentId, string documentURI);
  Node *addNode (string documentId, string compositeId, string xmlNode);
  InterfacePoint *addInterface (string documentId, string nodeId,
                                string xmlInterface);

  Link *addLink (string documentId, string compositeId, string xmlLink);
  PrivateBase *getPrivateBase ();
};

BR_PUCRIO_TELEMIDIA_GINGA_NCL_END
#endif /*PrivateBaseContext_H_*/
