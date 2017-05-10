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

#ifndef PrivateBaseManager_H_
#define PrivateBaseManager_H_

#include "system/Thread.h"
using namespace ::ginga::system;

#include "PrivateBaseContext.h"

GINGA_FORMATTER_BEGIN

class PrivateBaseManager
{
private:
  map<string, PrivateBaseContext *> privateBases;
  pthread_mutex_t mutexTable;

public:
  PrivateBaseManager ();
  virtual ~PrivateBaseManager ();

  void createPrivateBase (const string &id);
  void releasePrivateBases ();

  NclDocument *addDocument (const string &id, const string &location,
                            DeviceLayout *deviceLayout);

  void *addVisibleDocument (const string &id, const string &location,
                            DeviceLayout *deviceLayout);

  string getDocumentLocation (const string &id, const string &docId);

  NclDocument *getDocument (const string &id, const string &docId);

  vector<NclDocument *> *getDocuments (const string &id);
  NclDocument *removeDocument (const string &id, const string &docId);

  LayoutRegion *addRegion (const string &id, const string &documentId,
                           const string &regionBaseId, const string &regionId,
                           const string &xmlRegion);

  LayoutRegion *removeRegion (const string &id, const string &documentId,
                              const string &regionBaseId, const string &regionId);

  RegionBase *addRegionBase (const string &id, const string &documentId,
                             const string &xmlRegionBase);

  RegionBase *removeRegionBase (const string &id, const string &documentId,
                                const string &regionBaseId);

  Rule *addRule (const string &id, const string &documentId, const string &xmlRule);

  Rule *removeRule (const string &id, const string &documentId, const string &ruleId);

  RuleBase *addRuleBase (const string &id, const string &documentId, const string &xmlRuleBase);

  RuleBase *removeRuleBase (const string &id, const string &documentId,
                            const string &ruleBaseId);

  Transition *addTransition (const string &id, const string &documentId,
                             const string &xmlTransition);

  Transition *removeTransition (const string &id, const string &documentId,
                                const string &transitionId);

  TransitionBase *addTransitionBase (const string &id, const string &documentId,
                                     const string &xmlTransitionBase);

  TransitionBase *removeTransitionBase (const string &id, const string &documentId,
                                        const string &transitionBaseId);

  Connector *addConnector (const string &id, const string &documentId,
                           const string &xmlConnector);

  Connector *removeConnector (const string &id, const string &documentId,
                              const string &connectorId);

  ConnectorBase *addConnectorBase (const string &id, const string &documentId,
                                   const string &xmlConnectorBase);

  ConnectorBase *removeConnectorBase (const string &id, const string &documentId,
                                      const string &connectorBaseId);

  GenericDescriptor *addDescriptor (const string &id, const string &documentId,
                                    const string &xmlDescriptor);

  GenericDescriptor *removeDescriptor (const string &id, const string &documentId,
                                       const string &descriptorId);

  DescriptorBase *addDescriptorBase (const string &id, const string &documentId,
                                     const string &xmlDescriptorBase);

  DescriptorBase *removeDescriptorBase (const string &id, const string &documentId,
                                        const string &descriptorBaseId);

  Base *addImportBase (const string &id, const string &documentId, const string &docBaseId,
                       const string &xmlImportBase);

  Base *removeImportBase (const string &id, const string &documentId, const string &docBaseId,
                          const string &documentURI);

  NclDocument *addImportedDocumentBase (const string &id, const string &documentId,
                                        const string &xmlImportedDocumentBase);

  NclDocument *removeImportedDocumentBase (const string &id, const string &documentId,
                                           const string &importedDocumentBaseId);

  NclDocument *addImportNCL (const string &id, const string &documentId,
                             const string &xmlImportNCL);

  NclDocument *removeImportNCL (const string &id, const string &documentId,
                                const string &documentURI);

  Node *addNode (const string &id, const string &documentId, const string &compositeId,
                 const string &xmlNode);

  InterfacePoint *addInterface (const string &id, const string &documentId, const string &nodeId,
                                const string &xmlInterface);

  Link *addLink (const string &id, const string &documentId, const string &compositeId,
                 const string &xmlLink);

private:
  PrivateBaseContext *getPrivateBaseContext (const string &id);

public:
  PrivateBase *getPrivateBase (const string &id);

private:
  void lockTable ();
  void unlockTable ();
};

GINGA_FORMATTER_END

#endif /*PrivateBaseManager_H_*/
