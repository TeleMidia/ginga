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

#include "ginga.h"
#include "PrivateBaseManager.h"

GINGA_FORMATTER_BEGIN

PrivateBaseManager::PrivateBaseManager ()
{
  Thread::mutexInit (&mutexTable, false);
}

PrivateBaseManager::~PrivateBaseManager ()
{
  releasePrivateBases ();

  Thread::mutexDestroy (&mutexTable);
}

void
PrivateBaseManager::releasePrivateBases ()
{
  map<string, PrivateBaseContext *>::iterator i;

  lockTable ();
  i = privateBases.begin ();
  while (i != privateBases.end ())
    {
      delete i->second;
      ++i;
    }
  privateBases.clear ();
  unlockTable ();
}

void
PrivateBaseManager::createPrivateBase (const string &id)
{
  PrivateBaseContext *privateBaseContext;

  lockTable ();
  if (privateBases.count (id) == 0)
    {
      privateBaseContext = new PrivateBaseContext ();
      privateBaseContext->createPrivateBase (id);
      privateBases[id] = privateBaseContext;
    }
  else
    {
      clog << "PrivateBaseManager::createPrivateBase Warning! ";
      clog << "Trying to overwrite the '" << id << "' private base";
      clog << endl;
    }
  unlockTable ();
}

NclDocument *
PrivateBaseManager::addDocument (const string &id, const string &location,
                                 DeviceLayout *deviceLayout)
{
  PrivateBaseContext *privateBaseContext;
  NclDocument *document = NULL;

  privateBaseContext = getPrivateBaseContext (id);
  if (privateBaseContext != NULL)
    {
      document = privateBaseContext->addDocument (location, deviceLayout);
    }

  return document;
}

NclDocument *
PrivateBaseManager::embedDocument (const string &id, const string &docId, const string &nodeId,
                                   const string &location,
                                   DeviceLayout *deviceLayout)
{
  PrivateBaseContext *privateBaseContext;

  privateBaseContext = getPrivateBaseContext (id);
  if (privateBaseContext != NULL)
    {
      return privateBaseContext->embedDocument (docId, nodeId, location,
                                                deviceLayout);
    }

  return NULL;
}

void *
PrivateBaseManager::addVisibleDocument (const string &id, const string &location,
                                        DeviceLayout *deviceLayout)
{
  PrivateBaseContext *privateBaseContext;

  privateBaseContext = getPrivateBaseContext (id);
  if (privateBaseContext != NULL)
    {
      return privateBaseContext->addVisibleDocument (location,
                                                     deviceLayout);
    }

  return NULL;
}

string
PrivateBaseManager::getDocumentLocation (const string &id, const string &docId)
{
  PrivateBaseContext *privateBaseContext;

  privateBaseContext = getPrivateBaseContext (id);
  if (privateBaseContext != NULL)
    {
      return privateBaseContext->getDocumentLocation (docId);
    }

  return "";
}

string
PrivateBaseManager::getEmbeddedDocumentLocation (const string &id,
                                                 const string &parentDocId,
                                                 const string &nodeId)
{
  PrivateBaseContext *privateBaseContext;

  privateBaseContext = getPrivateBaseContext (id);
  if (privateBaseContext != NULL)
    {
      return privateBaseContext->getEmbeddedDocumentLocation (parentDocId,
                                                              nodeId);
    }

  return "";
}

NclDocument *
PrivateBaseManager::getDocument (const string &id, const string &docId)
{
  PrivateBaseContext *privateBaseContext;

  privateBaseContext = getPrivateBaseContext (id);
  if (privateBaseContext != NULL)
    {
      return privateBaseContext->getDocument (docId);
    }

  return NULL;
}

NclDocument *
PrivateBaseManager::getEmbeddedDocument (const string &id, const string &parentDocId,
                                         const string &nodeId)
{
  PrivateBaseContext *privateBaseContext;

  privateBaseContext = getPrivateBaseContext (id);
  if (privateBaseContext != NULL)
    {
      return privateBaseContext->getEmbeddedDocument (parentDocId, nodeId);
    }

  return NULL;
}

vector<NclDocument *> *
PrivateBaseManager::getDocuments (const string &id)
{
  PrivateBaseContext *privateBaseContext;

  privateBaseContext = getPrivateBaseContext (id);
  if (privateBaseContext != NULL)
    {
      return privateBaseContext->getDocuments ();
    }

  return NULL;
}

NclDocument *
PrivateBaseManager::removeDocument (const string &id, const string &docId)
{
  PrivateBaseContext *privateBaseContext;

  privateBaseContext = getPrivateBaseContext (id);
  if (privateBaseContext != NULL)
    {
      return privateBaseContext->removeDocument (docId);
    }

  return NULL;
}

NclDocument *
PrivateBaseManager::removeEmbeddedDocument (const string &baseId,
                                            const string &parentDocId,
                                            const string &nodeId)
{
  PrivateBaseContext *privateBaseContext;

  privateBaseContext = getPrivateBaseContext (baseId);
  if (privateBaseContext != NULL)
    {
      return privateBaseContext->removeEmbeddedDocument (parentDocId,
                                                         nodeId);
    }

  return NULL;
}

LayoutRegion *
PrivateBaseManager::addRegion (const string &id, const string &documentId,
                               const string &regionBaseId, const string &regionId,
                               const string &xmlRegion)
{
  PrivateBaseContext *privateBaseContext;

  privateBaseContext = getPrivateBaseContext (id);
  if (privateBaseContext != NULL)
    {
      return privateBaseContext->addRegion (documentId, regionBaseId,
                                            regionId, xmlRegion);
    }

  return NULL;
}

LayoutRegion *
PrivateBaseManager::removeRegion (const string &id, const string &documentId,
                                  const string &regionBaseId, const string &regionId)
{
  PrivateBaseContext *privateBaseContext;

  privateBaseContext = getPrivateBaseContext (id);
  if (privateBaseContext != NULL)
    {
      return privateBaseContext->removeRegion (documentId, regionBaseId,
                                               regionId);
    }

  return NULL;
}

RegionBase *
PrivateBaseManager::addRegionBase (const string &id, const string &documentId,
                                   const string &xmlRegionBase)
{
  PrivateBaseContext *privateBaseContext;

  privateBaseContext = getPrivateBaseContext (id);
  if (privateBaseContext != NULL)
    {
      return privateBaseContext->addRegionBase (documentId, xmlRegionBase);
    }

  return NULL;
}

RegionBase *
PrivateBaseManager::removeRegionBase (const string &id, const string &documentId,
                                      const string &regionBaseId)
{
  PrivateBaseContext *privateBaseContext;

  privateBaseContext = getPrivateBaseContext (id);
  if (privateBaseContext != NULL)
    {
      return privateBaseContext->removeRegionBase (documentId,
                                                   regionBaseId);
    }

  return NULL;
}

Rule *
PrivateBaseManager::addRule (const string &id, const string &documentId, const string &xmlRule)
{
  PrivateBaseContext *privateBaseContext;

  privateBaseContext = getPrivateBaseContext (id);
  if (privateBaseContext != NULL)
    {
      return privateBaseContext->addRule (documentId, xmlRule);
    }

  return NULL;
}

Rule *
PrivateBaseManager::removeRule (const string &id, const string &documentId, const string &ruleId)
{
  PrivateBaseContext *privateBaseContext;

  privateBaseContext = getPrivateBaseContext (id);
  if (privateBaseContext != NULL)
    {
      return privateBaseContext->removeRule (documentId, ruleId);
    }

  return NULL;
}

RuleBase *
PrivateBaseManager::addRuleBase (const string &id, const string &documentId,
                                 const string &xmlRuleBase)
{
  PrivateBaseContext *privateBaseContext;

  privateBaseContext = getPrivateBaseContext (id);
  if (privateBaseContext != NULL)
    {
      return privateBaseContext->addRuleBase (documentId, xmlRuleBase);
    }

  return NULL;
}

RuleBase *
PrivateBaseManager::removeRuleBase (const string &id, const string &documentId,
                                    const string &ruleBaseId)
{
  PrivateBaseContext *privateBaseContext;

  privateBaseContext = getPrivateBaseContext (id);
  if (privateBaseContext != NULL)
    {
      return privateBaseContext->removeRuleBase (documentId, ruleBaseId);
    }

  return NULL;
}

Transition *
PrivateBaseManager::addTransition (const string &id, const string &documentId,
                                   const string &xmlTransition)
{
  PrivateBaseContext *privateBaseContext;

  privateBaseContext = getPrivateBaseContext (id);
  if (privateBaseContext != NULL)
    {
      return privateBaseContext->addTransition (documentId, xmlTransition);
    }

  return NULL;
}

Transition *
PrivateBaseManager::removeTransition (const string &id, const string &documentId,
                                      const string &transitionId)
{
  PrivateBaseContext *privateBaseContext;

  privateBaseContext = getPrivateBaseContext (id);
  if (privateBaseContext != NULL)
    {
      return privateBaseContext->removeTransition (documentId,
                                                   transitionId);
    }

  return NULL;
}

TransitionBase *
PrivateBaseManager::addTransitionBase (const string &id, const string &documentId,
                                       const string &xmlTransitionBase)
{
  PrivateBaseContext *privateBaseContext;

  privateBaseContext = getPrivateBaseContext (id);
  if (privateBaseContext != NULL)
    {
      return privateBaseContext->addTransitionBase (documentId,
                                                    xmlTransitionBase);
    }

  return NULL;
}

TransitionBase *
PrivateBaseManager::removeTransitionBase (const string &id, const string &documentId,
                                          const string &transitionBaseId)
{
  PrivateBaseContext *privateBaseContext;

  privateBaseContext = getPrivateBaseContext (id);
  if (privateBaseContext != NULL)
    {
      return privateBaseContext->removeTransitionBase (documentId,
                                                       transitionBaseId);
    }

  return NULL;
}

Connector *
PrivateBaseManager::addConnector (const string &id, const string &documentId,
                                  const string &xmlConnector)
{
  PrivateBaseContext *privateBaseContext;

  privateBaseContext = getPrivateBaseContext (id);
  if (privateBaseContext != NULL)
    {
      return privateBaseContext->addConnector (documentId, xmlConnector);
    }

  return NULL;
}

Connector *
PrivateBaseManager::removeConnector (const string &id, const string &documentId,
                                     const string &connectorId)
{
  PrivateBaseContext *privateBaseContext;

  privateBaseContext = getPrivateBaseContext (id);
  if (privateBaseContext != NULL)
    {
      return privateBaseContext->removeConnector (documentId, connectorId);
    }

  return NULL;
}

ConnectorBase *
PrivateBaseManager::addConnectorBase (const string &id, const string &documentId,
                                      const string &xmlConnectorBase)
{
  PrivateBaseContext *privateBaseContext;

  privateBaseContext = getPrivateBaseContext (id);
  if (privateBaseContext != NULL)
    {
      return privateBaseContext->addConnectorBase (documentId,
                                                   xmlConnectorBase);
    }

  return NULL;
}

ConnectorBase *
PrivateBaseManager::removeConnectorBase (const string &id, const string &documentId,
                                         const string &connectorBaseId)
{
  PrivateBaseContext *privateBaseContext;

  privateBaseContext = getPrivateBaseContext (id);
  if (privateBaseContext != NULL)
    {
      return privateBaseContext->removeConnectorBase (documentId,
                                                      connectorBaseId);
    }

  return NULL;
}

GenericDescriptor *
PrivateBaseManager::addDescriptor (const string &id, const string &documentId,
                                   const string &xmlDescriptor)
{
  PrivateBaseContext *privateBaseContext;

  privateBaseContext = getPrivateBaseContext (id);
  if (privateBaseContext != NULL)
    {
      return privateBaseContext->addDescriptor (documentId, xmlDescriptor);
    }

  return NULL;
}

GenericDescriptor *
PrivateBaseManager::removeDescriptor (const string &id, const string &documentId,
                                      const string &descriptorId)
{
  PrivateBaseContext *privateBaseContext;

  privateBaseContext = getPrivateBaseContext (id);
  if (privateBaseContext != NULL)
    {
      return privateBaseContext->removeDescriptor (documentId,
                                                   descriptorId);
    }

  return NULL;
}

DescriptorBase *
PrivateBaseManager::addDescriptorBase (const string &id, const string &documentId,
                                       const string &xmlDescriptorBase)
{
  PrivateBaseContext *privateBaseContext;

  privateBaseContext = getPrivateBaseContext (id);
  if (privateBaseContext != NULL)
    {
      return privateBaseContext->addDescriptorBase (documentId,
                                                    xmlDescriptorBase);
    }

  return NULL;
}

DescriptorBase *
PrivateBaseManager::removeDescriptorBase (const string &id, const string &documentId,
                                          const string &descriptorBaseId)
{
  PrivateBaseContext *privateBaseContext;

  privateBaseContext = getPrivateBaseContext (id);
  if (privateBaseContext != NULL)
    {
      return privateBaseContext->removeDescriptorBase (documentId,
                                                       descriptorBaseId);
    }

  return NULL;
}

Base *
PrivateBaseManager::addImportBase (const string &id,
                                   const string &docId,
                                   const string &docBaseId,
                                   const string &xmlImportBase)
{
  PrivateBaseContext *privateBaseContext;

  privateBaseContext = getPrivateBaseContext (id);
  if (privateBaseContext != NULL)
    {
      return privateBaseContext->addImportBase (docId, docBaseId,
                                                xmlImportBase);
    }

  return NULL;
}

Base *
PrivateBaseManager::removeImportBase (const string &id, const string &documentId,
                                      const string &docBaseId, const string &documentURI)
{
  PrivateBaseContext *privateBaseContext;

  privateBaseContext = getPrivateBaseContext (id);
  if (privateBaseContext != NULL)
    {
      return privateBaseContext->removeImportBase (documentId, docBaseId,
                                                   documentURI);
    }

  return NULL;
}

NclDocument *
PrivateBaseManager::addImportedDocumentBase (const string &id, const string &documentId,
                                             const string &xmlImportedDocumentBase)
{
  PrivateBaseContext *privateBaseContext;

  privateBaseContext = getPrivateBaseContext (id);
  if (privateBaseContext != NULL)
    {
      return privateBaseContext->addImportedDocumentBase (
          documentId, xmlImportedDocumentBase);
    }

  return NULL;
}

NclDocument *
PrivateBaseManager::removeImportedDocumentBase (
    const string &id, const string &documentId, const string &importedDocumentBaseId)
{
  PrivateBaseContext *privateBaseContext;

  privateBaseContext = getPrivateBaseContext (id);
  if (privateBaseContext != NULL)
    {
      return privateBaseContext->removeImportedDocumentBase (
          documentId, importedDocumentBaseId);
    }

  return NULL;
}

NclDocument *
PrivateBaseManager::addImportNCL (const string &id, const string &documentId,
                                  const string &xmlImportNCL)
{
  PrivateBaseContext *privateBaseContext;

  privateBaseContext = getPrivateBaseContext (id);
  if (privateBaseContext != NULL)
    {
      return privateBaseContext->addImportNCL (documentId, xmlImportNCL);
    }

  return NULL;
}

NclDocument *
PrivateBaseManager::removeImportNCL (const string &id, const string &documentId,
                                     const string &documentURI)
{
  PrivateBaseContext *privateBaseContext;

  privateBaseContext = getPrivateBaseContext (id);
  if (privateBaseContext != NULL)
    {
      return privateBaseContext->removeImportNCL (documentId, documentURI);
    }

  return NULL;
}

Node *
PrivateBaseManager::addNode (const string &id, const string &documentId,
                             const string &compositeId, const string &xmlNode)
{
  PrivateBaseContext *privateBaseContext;

  privateBaseContext = getPrivateBaseContext (id);
  if (privateBaseContext != NULL)
    {
      return privateBaseContext->addNode (documentId, compositeId, xmlNode);
    }

  return NULL;
}

InterfacePoint *
PrivateBaseManager::addInterface (const string &id, const string &documentId,
                                  const string &nodeId, const string &xmlInterface)
{
  PrivateBaseContext *privateBaseContext;

  privateBaseContext = getPrivateBaseContext (id);
  if (privateBaseContext != NULL)
    {
      return privateBaseContext->addInterface (documentId, nodeId,
                                               xmlInterface);
    }

  return NULL;
}

Link *
PrivateBaseManager::addLink (const string &id, const string &documentId,
                             const string &compositeId, const string &xmlLink)
{
  PrivateBaseContext *privateBaseContext;

  privateBaseContext = getPrivateBaseContext (id);
  if (privateBaseContext != NULL)
    {
      return privateBaseContext->addLink (documentId, compositeId, xmlLink);
    }

  return NULL;
}

PrivateBaseContext *
PrivateBaseManager::getPrivateBaseContext (const string &id)
{
  map<string, PrivateBaseContext *>::iterator i;
  PrivateBaseContext *privateBaseContext = NULL;

  lockTable ();
  i = privateBases.find (id);
  if (i != privateBases.end ())
    {
      privateBaseContext = i->second;
    }
  unlockTable ();

  return privateBaseContext;
}

PrivateBase *
PrivateBaseManager::getPrivateBase (const string &id)
{
  PrivateBaseContext *privateBaseContext;

  privateBaseContext = getPrivateBaseContext (id);
  if (privateBaseContext != NULL)
    {
      return privateBaseContext->getPrivateBase ();
    }

  return NULL;
}

void
PrivateBaseManager::lockTable ()
{
  Thread::mutexLock (&mutexTable);
}

void
PrivateBaseManager::unlockTable ()
{
  Thread::mutexUnlock (&mutexTable);
}

GINGA_FORMATTER_END
