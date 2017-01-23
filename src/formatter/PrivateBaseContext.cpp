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
#include "nclconv/IDocumentConverter.h"
using namespace ::ginga::nclconv;

#include "nclconv/NclDocumentConverter.h"
using namespace ::ginga::nclconv;

#include "PrivateBaseContext.h"

#include "system/SystemCompat.h"
using namespace ::ginga::system;

GINGA_FORMATTER_BEGIN

PrivateBaseContext::PrivateBaseContext () { this->privateBase = NULL; }

PrivateBaseContext::~PrivateBaseContext ()
{
  map<string, NclDocument *>::iterator i;
  map<string, EmbeddedNclData *>::iterator j;
  set<DeviceLayout *>::iterator k;

  NclDocument *doc;

  i = baseDocuments.begin ();
  while (i != baseDocuments.end ())
    {
      doc = removeDocument (i->first);
      if (doc != NULL)
        {
          delete doc;
          i = baseDocuments.begin ();
        }
      else
        {
          ++i;
        }
    }
  baseDocuments.clear ();

  i = visibleDocuments.begin ();
  while (i != visibleDocuments.end ())
    {
      delete i->second;
      ++i;
    }
  visibleDocuments.clear ();

  j = embeddedDocuments.begin ();
  while (j != embeddedDocuments.end ())
    {
      delete j->second->embeddedDocument;
      delete j->second;
      ++j;
    }
  embeddedDocuments.clear ();

  k = layoutsGB.begin ();
  while (k != layoutsGB.end ())
    {
      delete *k;
      ++k;
    }
  layoutsGB.clear ();

  layouts.clear ();

  if (privateBase != NULL)
    {
      delete privateBase;
      privateBase = NULL;
    }
}

void
PrivateBaseContext::createPrivateBase (string id)
{
  if (privateBase != NULL)
    {
      clog << "PrivateBaseContext::createPrivateBase Warning!";
      clog << " trying to overwrite private base context with a ";
      clog << " new private base '" << id << "'" << endl;
      return;
    }

  clog << "PrivateBaseContext::createPrivateBase '" << id << "'";
  clog << endl;
  privateBase = new PrivateBase (id);
}

NclDocument *
PrivateBaseContext::compileDocument (string location,
                                     DeviceLayout *deviceLayout)
{
  IDocumentConverter *compiler = NULL;
  NclDocument *document = NULL;
  string docUrl;

  if (SystemCompat::isAbsolutePath (location))
    {
      docUrl = location;
    }
  else
    {
      docUrl = SystemCompat::getUserCurrentPath () + location;
    }

  compiler = new NclDocumentConverter ();

  compiler->setConverterInfo (this, deviceLayout);
  compiler->parse (docUrl, SystemCompat::getIUriD (),
                   SystemCompat::getFUriD ());

  document = (NclDocument *)(compiler->getObject ("return", "document"));

  delete compiler;
  compiler = NULL;

  layoutsGB.insert (deviceLayout);

  if (document != NULL)
    {
      layouts[document] = deviceLayout;
    }
  else
    {
      clog << "PrivateBaseContext::compileDocument Warning! ";
      clog << "NULL document '" << docUrl << "'" << endl;
    }

  return document;
}

NclDocument *
PrivateBaseContext::addDocument (string location,
                                 DeviceLayout *deviceLayout)
{
  NclDocument *newDocument;
  NclDocument *document;
  string docUrl;

  if (SystemCompat::isAbsolutePath (location))
    {
      docUrl = location;
    }
  else
    {
      docUrl = SystemCompat::getUserCurrentPath () + location;
    }

  docUrl = SystemCompat::updatePath (docUrl);

  if (baseDocuments.count (docUrl) != 0)
    {
      clog << "PrivateBaseContext::addDocument Warning! ";
      clog << "Trying to add the same doc twice '" << docUrl;
      clog << "'";
      clog << endl;
      return baseDocuments[docUrl];
    }
  else if (visibleDocuments.count (docUrl) != 0)
    {
      document = visibleDocuments[docUrl];
      eraseVisibleDocument (docUrl);

      if (document->getBody () != NULL)
        {
          privateBase->addNode (document->getBody ());
        }

      baseDocuments[docUrl] = document;
      return document;
    }
  else
    {
      newDocument = compileDocument (docUrl, deviceLayout);
      if (newDocument != NULL)
        {
          if (newDocument->getBody () != NULL)
            {
              privateBase->addNode (newDocument->getBody ());
            }

          baseDocuments[newDocument->getDocumentLocation ()] = newDocument;
        }
      else
        {
          clog << "PrivateBaseContext::addDocument Warning! ";
          clog << "Can't compile document '" << docUrl << "'";
          clog << endl;
        }

      if (newDocument != NULL)
        {
          clog << "PrivateBaseContext::addDocument ";
          clog << "id = '" << newDocument->getId () << "', URL = '";
          clog << newDocument->getDocumentLocation () << "' all done";
          clog << endl;
        }
      return newDocument;
    }
}

NclDocument *
PrivateBaseContext::embedDocument (string docId, string nodeId,
                                   string location,
                                   DeviceLayout *deviceLayout)
{
  map<string, EmbeddedNclData *>::iterator i;
  EmbeddedNclData *data;
  string docUrl;
  NclDocument *newDocument;

  i = embeddedDocuments.find (docId + "::" + nodeId);
  if (i != embeddedDocuments.end ())
    {
      data = i->second;
      if (data->embeddedNclNodeId == nodeId)
        {
          clog << "PrivateBaseContext::embedDocument Warning! ";
          clog << "Trying to embed the same nodeId twice '";
          clog << nodeId << "' for parent '" << docId << "'";
          clog << endl;
          return data->embeddedDocument;
        }
    }

  if (SystemCompat::isAbsolutePath (location))
    {
      docUrl = location;
    }
  else
    {
      docUrl = SystemCompat::getUserCurrentPath () + location;
    }

  docUrl = SystemCompat::updatePath (docUrl);

  // TODO: insert new embedded document body inside its node
  newDocument = compileDocument (docUrl, deviceLayout);
  if (newDocument == NULL)
    {
      return NULL;
    }

  clog << "PrivateBaseContext::embedDocument(" << this << ")";
  clog << " nodeId = '" << nodeId << "'";
  clog << " docId = '" << newDocument->getId () << "'" << endl;

  data = new EmbeddedNclData;
  data->embeddedDocument = newDocument;
  data->embeddedNclNodeId = nodeId;
  data->embeddedNclNodeLocation = docUrl;

  embeddedDocuments[docId + "::" + nodeId] = data;

  return newDocument;
}

void *
PrivateBaseContext::addVisibleDocument (string location,
                                        DeviceLayout *deviceLayout)
{
  NclDocument *newDocument;
  string id;

  location = SystemCompat::updatePath (location);
  if (baseDocuments.count (location) != 0)
    {
      return baseDocuments[location];
    }
  else if (visibleDocuments.count (location) != 0)
    {
      return visibleDocuments[location];
    }
  else
    {
      newDocument = compileDocument (location, deviceLayout);
      if (newDocument != NULL)
        {
          visibleDocuments[location] = newDocument;
          return newDocument;
        }
    }

  return NULL;
}

NclDocument *
PrivateBaseContext::getVisibleDocument (string id)
{
  map<string, NclDocument *>::iterator i;

  i = visibleDocuments.begin ();
  while (i != visibleDocuments.end ())
    {
      if (i->second->getId () == id)
        {
          return i->second;
        }

      ++i;
    }

  return NULL;
}

bool
PrivateBaseContext::eraseVisibleDocument (string docLocation)
{
  map<string, NclDocument *>::iterator i;

  i = visibleDocuments.find (docLocation);
  if (i != visibleDocuments.end ())
    {
      visibleDocuments.erase (i);
      return true;
    }
  return false;
}

string
PrivateBaseContext::getDocumentLocation (string docId)
{
  map<string, NclDocument *>::iterator i;
  NclDocument *nclDoc = NULL;

  nclDoc = getDocument (docId);

  if (nclDoc != NULL)
    {
      return nclDoc->getDocumentLocation ();
    }

  clog << "PrivateBaseContext::getDocumentLocation Warning! Can't find '";
  clog << docId << "' location" << endl;
  return "";
}

string
PrivateBaseContext::getEmbeddedDocumentLocation (string parentDocId,
                                                 string nodeId)
{
  map<string, EmbeddedNclData *>::iterator i;

  i = embeddedDocuments.find (parentDocId + "::" + nodeId);
  if (i != embeddedDocuments.end ())
    {
      return i->second->embeddedNclNodeLocation;
    }

  return "";
}

NclDocument *
PrivateBaseContext::getDocument (string id)
{
  NclDocument *doc = NULL;
  map<string, NclDocument *>::iterator i;

  doc = getBaseDocument (id);
  if (doc == NULL)
    {
      doc = getVisibleDocument (id);
    }

  return doc;
}

NclDocument *
PrivateBaseContext::getEmbeddedDocument (string parentDocId, string nodeId)
{
  map<string, EmbeddedNclData *>::iterator i;
  NclDocument *document = NULL;

  i = embeddedDocuments.find (parentDocId + "::" + nodeId);
  if (i != embeddedDocuments.end ())
    {
      document = i->second->embeddedDocument;
    }

  return document;
}

vector<NclDocument *> *
PrivateBaseContext::getDocuments ()
{
  map<string, NclDocument *>::iterator i;
  vector<NclDocument *> *documents;
  documents = new vector<NclDocument *>;

  for (i = baseDocuments.begin (); i != baseDocuments.end (); ++i)
    {
      documents->push_back (i->second);
    }

  return documents;
}

void
PrivateBaseContext::removeDocumentBase (arg_unused (NclDocument *document), Base *base)
{
  vector<Base *> *bases;
  vector<Base *>::iterator i;
  string baseLocation;

  if (base != NULL)
    {
      bases = base->getBases ();
      if (bases != NULL)
        {
          i = bases->begin ();
          while (i != bases->end ())
            {
              baseLocation = base->getBaseLocation (*i);
              if (baseLocation != "")
                {
                  eraseVisibleDocument (baseLocation);
                }
              ++i;
            }

          delete bases;
        }
    }
}

void
PrivateBaseContext::removeDocumentBases (NclDocument *document)
{
  Base *base;
  map<int, RegionBase *> *regionBases;
  map<int, RegionBase *>::iterator i;

  base = document->getConnectorBase ();
  removeDocumentBase (document, base);

  base = document->getDescriptorBase ();
  removeDocumentBase (document, base);

  regionBases = document->getRegionBases ();
  i = regionBases->begin ();
  while (i != regionBases->end ())
    {
      base = i->second;
      removeDocumentBase (document, base);
      ++i;
    }

  base = document->getRuleBase ();
  removeDocumentBase (document, base);

  base = document->getTransitionBase ();
  removeDocumentBase (document, base);
}

NclDocument *
PrivateBaseContext::removeDocument (string id)
{
  NclDocument *document = NULL;
  string docLocation;
  map<string, NclDocument *>::iterator i;

  document = getBaseDocument (id);
  if (document != NULL)
    {
      docLocation = document->getDocumentLocation ();
      i = baseDocuments.find (docLocation);
      if (i != baseDocuments.end ())
        {
          baseDocuments.erase (i);
        }
      else
        {
          clog << "PrivateBaseContext::removeDocument Warning! ";
          clog << "Can't find '" << docLocation << "'" << endl;
        }

      if (document->getBody () != NULL)
        {
          privateBase->removeNode (document->getBody ());
        }
      else
        {
          clog << "PrivateBaseContext::removeDocument Warning! ";
          clog << "Can't find BODY of '" << docLocation << "'" << endl;
        }

      eraseVisibleDocument (docLocation);
      removeDocumentBases (document);
    }

  return document;
}

NclDocument *
PrivateBaseContext::removeEmbeddedDocument (string parentDocId,
                                            string nodeId)
{
  map<string, EmbeddedNclData *>::iterator i;
  NclDocument *document = NULL;

  i = embeddedDocuments.find (parentDocId + "::" + nodeId);
  if (i != embeddedDocuments.end ())
    {
      document = i->second->embeddedDocument;
      delete i->second;
      embeddedDocuments.erase (i);
    }
  return document;
}

NclDocument *
PrivateBaseContext::getBaseDocument (string documentId)
{
  map<string, NclDocument *>::iterator i;

  i = baseDocuments.begin ();
  while (i != baseDocuments.end ())
    {
      if (i->second->getId () == documentId)
        {
          return i->second;
        }

      ++i;
    }

  return NULL;
}

void *
PrivateBaseContext::compileEntity (string location, NclDocument *document,
                                   void *parentObject)
{
  IDocumentConverter *compiler;
  string entityUrl;
  void *entity;
  DeviceLayout *deviceLayout;

  if (SystemCompat::isAbsolutePath (location))
    {
      entityUrl = location;
    }
  else
    {
      entityUrl = SystemCompat::getUserCurrentPath () + location;
    }

  if (layouts.count (document) == 0)
    {
      clog << "PrivateBaseContext::compileEntity Warning! Can't ";
      clog << "find layout for '" << location << "'" << endl;
      return NULL;
    }

  deviceLayout = layouts[document];

  compiler = new NclDocumentConverter ();
  compiler->setConverterInfo (this, deviceLayout);

  clog << "PrivateBaseContext::compileEntity entityURL = '" << entityUrl;
  clog << "'" << endl;

  entity = compiler->parseEntity (entityUrl, document, parentObject);

  delete compiler;

  return entity;
}

LayoutRegion *
PrivateBaseContext::addRegion (string documentId, string regionBaseId,
                               string regionId, string xmlRegion)
{
  NclDocument *document;
  RegionBase *base;
  LayoutRegion *region;
  LayoutRegion *parentRegion;

  document = getBaseDocument (documentId);
  if (document == NULL)
    {
      return NULL;
    }

  if (regionBaseId == "")
    {
      base = document->getRegionBase ("systemScreen(0)");
    }
  else
    {
      base = document->getRegionBase (regionBaseId);
    }

  if (base == NULL)
    {
      return NULL;
    }

  if (xstrchomp (regionId) == "")
    {
      parentRegion = NULL;
      region = (LayoutRegion *)compileEntity (xmlRegion, document, base);
    }
  else
    {
      parentRegion = document->getRegion (regionId);
      if (parentRegion == NULL)
        {
          return NULL;
        }
      region = (LayoutRegion *)compileEntity (xmlRegion, document, base);
    }

  if (region == NULL)
    {
      return NULL;
    }

  if (parentRegion == NULL)
    {
      base->addRegion (region);
    }
  else
    {
      parentRegion->addRegion (region);
    }

  return region;
}

LayoutRegion *
PrivateBaseContext::removeRegion (string documentId, string regionBaseId,
                                  string regionId)
{
  NclDocument *document;
  RegionBase *base;
  LayoutRegion *region;

  document = getBaseDocument (documentId);
  if (document == NULL)
    {
      return NULL;
    }

  if (regionBaseId == "")
    {
      base = document->getRegionBase ("systemScreen(0)");
    }
  else
    {
      base = document->getRegionBase (regionBaseId);
    }

  if (base == NULL)
    {
      return NULL;
    }

  region = base->getRegion (regionId);
  if (region == NULL)
    {
      return NULL;
    }

  if (base->removeRegion (region))
    {
      return region;
    }

  return NULL;
}

RegionBase *
PrivateBaseContext::addRegionBase (string documentId, string xmlRegionBase)
{
  NclDocument *document;
  RegionBase *base;

  document = getBaseDocument (documentId);
  if (document == NULL)
    {
      return NULL;
    }

  base = (RegionBase *)compileEntity (xmlRegionBase, document, NULL);
  if (base == NULL)
    {
      return NULL;
    }

  document->addRegionBase (base);
  return base;
}

RegionBase *
PrivateBaseContext::removeRegionBase (string documentId,
                                      string regionBaseId)
{
  NclDocument *document;
  RegionBase *base;

  document = getBaseDocument (documentId);
  if (document == NULL)
    {
      return NULL;
    }

  base = document->getRegionBase (regionBaseId);
  if (base == NULL)
    {
      return NULL;
    }

  document->removeRegionBase (regionBaseId);
  return base;
}

Rule *
PrivateBaseContext::addRule (string documentId, string xmlRule)
{
  NclDocument *document;
  RuleBase *base;
  Rule *rule;

  document = getBaseDocument (documentId);
  if (document == NULL)
    {
      return NULL;
    }

  base = document->getRuleBase ();
  if (base == NULL)
    {
      return NULL;
    }

  rule = (Rule *)compileEntity (xmlRule, document, base);
  if (rule == NULL)
    {
      return NULL;
    }

  base->addRule (rule);
  return rule;
}

Rule *
PrivateBaseContext::removeRule (string documentId, string ruleId)
{
  NclDocument *document;
  RuleBase *base;
  Rule *rule;

  document = getBaseDocument (documentId);
  if (document == NULL)
    {
      return NULL;
    }

  base = document->getRuleBase ();
  if (base == NULL)
    {
      return NULL;
    }

  rule = base->getRule (ruleId);
  if (rule == NULL)
    {
      return NULL;
    }

  if (base->removeRule (rule))
    {
      return rule;
    }
  else
    {
      return NULL;
    }
}

RuleBase *
PrivateBaseContext::addRuleBase (string documentId, string xmlRuleBase)
{
  NclDocument *document;
  RuleBase *base;

  document = getBaseDocument (documentId);
  if (document == NULL)
    {
      return NULL;
    }

  base = (RuleBase *)compileEntity (xmlRuleBase, document, NULL);
  if (base == NULL)
    {
      return NULL;
    }

  document->setRuleBase (base);
  return base;
}

RuleBase *
PrivateBaseContext::removeRuleBase (string documentId, arg_unused (string ruleBaseId))
{
  NclDocument *document;
  RuleBase *base;

  document = getBaseDocument (documentId);
  if (document == NULL)
    {
      return NULL;
    }

  base = document->getRuleBase ();
  if (base == NULL)
    {
      return NULL;
    }

  document->setRuleBase (NULL);
  return base;
}

Transition *
PrivateBaseContext::addTransition (string documentId, string xmlTransition)
{
  NclDocument *document;
  TransitionBase *base;
  Transition *transition;

  document = getBaseDocument (documentId);
  if (document == NULL)
    {
      return NULL;
    }

  base = document->getTransitionBase ();
  if (base == NULL)
    {
      return NULL;
    }

  transition = (Transition *)compileEntity (xmlTransition, document, base);

  if (transition == NULL)
    {
      return NULL;
    }

  base->addTransition (transition);
  return transition;
}

Transition *
PrivateBaseContext::removeTransition (string documentId,
                                      string transitionId)
{
  NclDocument *document;
  TransitionBase *base;
  Transition *transition;

  document = getBaseDocument (documentId);
  if (document == NULL)
    {
      return NULL;
    }

  base = document->getTransitionBase ();
  if (base == NULL)
    {
      return NULL;
    }

  transition = base->getTransition (transitionId);
  if (transition == NULL)
    {
      return NULL;
    }

  if (base->removeTransition (transition))
    {
      return transition;
    }
  else
    {
      return NULL;
    }
}

TransitionBase *
PrivateBaseContext::addTransitionBase (string documentId,
                                       string xmlTransitionBase)
{
  NclDocument *document;
  TransitionBase *base;

  document = getBaseDocument (documentId);
  if (document == NULL)
    {
      return NULL;
    }

  base
      = (TransitionBase *)compileEntity (xmlTransitionBase, document, NULL);

  if (base == NULL)
    {
      return NULL;
    }

  document->setTransitionBase (base);
  return base;
}

TransitionBase *
PrivateBaseContext::removeTransitionBase (string documentId,
                                          arg_unused (string transitionBaseId))
{
  NclDocument *document;
  TransitionBase *base;

  document = getBaseDocument (documentId);
  if (document == NULL)
    {
      return NULL;
    }

  base = document->getTransitionBase ();
  if (base == NULL)
    {
      return NULL;
    }

  document->setTransitionBase (NULL);
  return base;
}

Connector *
PrivateBaseContext::addConnector (string documentId, string xmlConnector)
{
  NclDocument *document;
  ConnectorBase *base;
  Connector *connector;

  document = getBaseDocument (documentId);
  if (document == NULL)
    {
      return NULL;
    }

  base = document->getConnectorBase ();
  if (base == NULL)
    {
      return NULL;
    }

  connector
      = (Connector *)(compileEntity (xmlConnector, document, (void *)base));

  if (connector == NULL)
    {
      return NULL;
    }

  base->addConnector (connector);
  return connector;
}

Connector *
PrivateBaseContext::removeConnector (string documentId, string connectorId)
{
  NclDocument *document;
  ConnectorBase *base;
  Connector *connector;

  document = getBaseDocument (documentId);
  if (document == NULL)
    {
      return NULL;
    }

  base = document->getConnectorBase ();
  if (base == NULL)
    {
      return NULL;
    }

  connector = base->getConnector (connectorId);
  if (connector == NULL)
    {
      return NULL;
    }

  if (base->removeConnector (connector))
    {
      return connector;
    }
  else
    {
      return NULL;
    }
}

ConnectorBase *
PrivateBaseContext::addConnectorBase (string documentId,
                                      string xmlConnectorBase)
{
  NclDocument *document;
  ConnectorBase *base;

  document = getBaseDocument (documentId);
  if (document == NULL)
    {
      return NULL;
    }

  base = (ConnectorBase *)compileEntity (xmlConnectorBase, document, NULL);
  if (base == NULL)
    {
      return NULL;
    }

  document->setConnectorBase (base);
  return base;
}

ConnectorBase *
PrivateBaseContext::removeConnectorBase (string documentId,
                                         arg_unused (string connectorBaseId))
{
  NclDocument *document;
  ConnectorBase *base;

  document = getBaseDocument (documentId);
  if (document == NULL)
    {
      return NULL;
    }

  base = document->getConnectorBase ();
  if (base == NULL)
    {
      return NULL;
    }

  document->setConnectorBase (NULL);
  return base;
}

GenericDescriptor *
PrivateBaseContext::addDescriptor (string documentId, string xmlDescriptor)
{
  NclDocument *document;
  DescriptorBase *base;
  GenericDescriptor *descriptor;

  document = getBaseDocument (documentId);
  if (document == NULL)
    {
      return NULL;
    }

  base = document->getDescriptorBase ();
  if (base == NULL)
    {
      return NULL;
    }

  descriptor = (GenericDescriptor *)(compileEntity (xmlDescriptor, document,
                                                    base));

  if (descriptor == NULL)
    {
      return NULL;
    }

  base->addDescriptor (descriptor);
  return descriptor;
}

GenericDescriptor *
PrivateBaseContext::removeDescriptor (string documentId,
                                      string descriptorId)
{
  NclDocument *document;
  DescriptorBase *base;
  GenericDescriptor *descriptor;

  document = getBaseDocument (documentId);
  if (document == NULL)
    {
      return NULL;
    }

  base = document->getDescriptorBase ();
  if (base == NULL)
    {
      return NULL;
    }

  descriptor = base->getDescriptor (descriptorId);
  if (descriptor == NULL)
    {
      return NULL;
    }

  if (base->removeDescriptor (descriptor))
    {
      return descriptor;
    }

  return NULL;
}

DescriptorBase *
PrivateBaseContext::addDescriptorBase (string documentId,
                                       string xmlDescriptorBase)
{
  NclDocument *document;
  DescriptorBase *base;

  document = getBaseDocument (documentId);
  if (document == NULL)
    {
      return NULL;
    }

  base = (DescriptorBase *)(compileEntity (xmlDescriptorBase, document,
                                           NULL));

  if (base == NULL)
    {
      return NULL;
    }

  document->setDescriptorBase (base);
  return base;
}

DescriptorBase *
PrivateBaseContext::removeDescriptorBase (string documentId,
                                          arg_unused (string descriptorBaseId))
{
  NclDocument *document;
  DescriptorBase *base;

  document = getBaseDocument (documentId);
  if (document == NULL)
    {
      return NULL;
    }

  base = document->getDescriptorBase ();
  if (base == NULL)
    {
      return NULL;
    }

  document->setDescriptorBase (NULL);
  return base;
}

Base *
PrivateBaseContext::getBase (NclDocument *document, string baseId)
{
  Base *base;

  base = document->getConnectorBase ();
  if (base->getId () != "" && base->getId () == baseId)
    {
      return base;
    }

  base = document->getRegionBase (baseId);
  if (base != NULL)
    {
      return base;
    }

  base = document->getRuleBase ();
  if (base->getId () != "" && base->getId () == baseId)
    {
      return base;
    }

  base = document->getDescriptorBase ();
  if (base->getId () != "" && base->getId () == baseId)
    {
      return base;
    }

  return NULL;
}

Base *
PrivateBaseContext::addImportBase (string documentId, string docBaseId,
                                   string xmlImportBase)
{
  IDocumentConverter *compiler;
  NclDocument *document;
  Base *parentBase;
  void *importElement;
  string location, alias;
  NclDocument *importedDocument;
  Base *base = NULL;
  DeviceLayout *deviceLayout;

  document = getBaseDocument (documentId);
  if (document == NULL)
    {
      return NULL;
    }

  parentBase = getBase (document, docBaseId);
  if (parentBase == NULL)
    {
      return NULL;
    }

  importElement = (void *)(compileEntity (xmlImportBase, document, NULL));

  if (importElement == NULL)
    {
      return NULL;
    }

  if (layouts.count (document) == 0)
    {
      clog << "PrivateBaseContext::addImportBase Warning! Can't ";
      clog << "find layout for '" << location << "'" << endl;
      return NULL;
    }

  deviceLayout = layouts[document];

  compiler = new NclDocumentConverter ();
  compiler->setConverterInfo (this, deviceLayout);

  location = compiler->getAttribute (importElement, "documentURI");
  if (location != "")
    {
      importedDocument
          = (NclDocument *)(addVisibleDocument (location, deviceLayout));

      if (importedDocument != NULL)
        {
          if (parentBase->instanceOf ("ConnectorBase"))
            {
              base = importedDocument->getConnectorBase ();
            }
          else if (parentBase->instanceOf ("RegionBase"))
            {
              base = importedDocument->getRegionBase (docBaseId);
            }
          else if (parentBase->instanceOf ("DescriptorBase"))
            {
              base = importedDocument->getDescriptorBase ();
            }
          else if (parentBase->instanceOf ("RuleBase"))
            {
              base = importedDocument->getRuleBase ();
            }

          if (base != NULL)
            {
              alias = compiler->getAttribute (importElement, "alias");
              parentBase->addBase (base, alias, location);
            }
        }
    }

  delete compiler;
  return base;
}

Base *
PrivateBaseContext::removeImportBase (string documentId, string docBaseId,
                                      string documentURI)
{
  NclDocument *document;
  Base *parentBase;
  vector<Base *> *bases;
  vector<Base *>::iterator i;
  string location;

  document = getBaseDocument (documentId);
  if (document == NULL)
    {
      return NULL;
    }

  parentBase = getBase (document, docBaseId);
  if (parentBase == NULL)
    {
      return NULL;
    }

  bases = parentBase->getBases ();
  if (bases != NULL)
    {
      i = bases->begin ();
      while (i != bases->end ())
        {
          location = parentBase->getBaseLocation (*i);
          if (location != "" && location == documentURI)
            {
              parentBase->removeBase (*i);
              delete bases;
              return *i;
            }
          ++i;
        }

      delete bases;
    }

  return NULL;
}

NclDocument *
PrivateBaseContext::addImportedDocumentBase (string documentId,
                                             string xmlImportedDocumentBase)
{
  NclDocument *document;
  void *baseElement;

  document = getBaseDocument (documentId);
  if (document == NULL)
    {
      return NULL;
    }

  baseElement
      = (void *)(compileEntity (xmlImportedDocumentBase, document, NULL));

  if (baseElement == NULL)
    {
      return NULL;
    }

  return document;
}

NclDocument *
PrivateBaseContext::removeImportedDocumentBase (
    string documentId, arg_unused (string importedDocumentBaseId))
{
  NclDocument *document;

  document = getBaseDocument (documentId);
  if (document == NULL)
    {
      return NULL;
    }

  vector<NclDocument *> *documents;
  vector<NclDocument *>::iterator i;

  documents = document->getDocuments ();
  if (documents != NULL)
    {
      i = documents->begin ();
      while (i != documents->end ())
        {
          document->removeDocument (*i);
          documents = document->getDocuments ();
          ++i;
        }
    }

  return document;
}

NclDocument *
PrivateBaseContext::addImportNCL (string documentId, string xmlImportNCL)
{
  IDocumentConverter *compiler;
  NclDocument *document;
  void *importElement;
  string location, alias;
  NclDocument *importedDocument = NULL;
  DeviceLayout *deviceLayout;

  document = getBaseDocument (documentId);
  if (document == NULL)
    {
      clog << "PrivateBaseContext::addImportNCL Warning! ";
      clog << "Can't find document '" << documentId << "'";
      clog << ": returning NULL" << endl;
      return NULL;
    }

  importElement = (void *)(compileEntity (xmlImportNCL, document, NULL));

  if (importElement == NULL)
    {
      clog << "PrivateBaseContext::addImportNCL Warning! ";
      clog << "Can't compile importNCL '" << xmlImportNCL << "'";
      clog << ": returning NULL" << endl;
      return NULL;
    }

  if (layouts.count (document) == 0)
    {
      clog << "PrivateBaseContext::addImportNcl Warning! Can't ";
      clog << "find layout for '" << location << "'" << endl;
      return NULL;
    }

  deviceLayout = layouts[document];

  compiler = new NclDocumentConverter ();
  compiler->setConverterInfo (this, deviceLayout);

  location = compiler->getAttribute (importElement, "documentURI");
  if (location != "")
    {
      importedDocument
          = (NclDocument *)addVisibleDocument (location, deviceLayout);

      if (importedDocument != NULL)
        {
          alias = compiler->getAttribute (importElement, "alias");
          document->addDocument (importedDocument, alias, location);
        }
    }

  delete compiler;
  return importedDocument;
}

NclDocument *
PrivateBaseContext::removeImportNCL (string documentId, string documentURI)
{
  NclDocument *document;

  document = getBaseDocument (documentId);
  if (document == NULL)
    {
      return NULL;
    }

  vector<NclDocument *> *documents;
  vector<NclDocument *>::iterator i;
  NclDocument *importedDocument = NULL;
  string location;

  documents = document->getDocuments ();
  if (documents != NULL)
    {
      i = documents->begin ();
      while (i != documents->end ())
        {
          importedDocument = *i;
          location = document->getDocumentLocation (*i);
          if (location != "" && location == documentURI)
            {
              document->removeDocument (importedDocument);
              return importedDocument;
            }

          ++i;
        }
    }

  return NULL;
}

/*
addTransition (baseId, documentId, xmlTransition)
removeTransition (baseId, documentId, transitionId)
addTransitionBase (baseId, documentId, xmlTransitionBase)
removeTransitionBase (baseId, documentId, transitionBaseId)
*/

Node *
PrivateBaseContext::addNode (string documentId, string compositeId,
                             string xmlNode)
{
  NclDocument *document;
  Node *node = NULL;
  ContextNode *contextNode;

  document = getBaseDocument (documentId);
  if (document == NULL)
    {
      clog << "PrivateBaseContext::addNode Warning! cant add node: ";
      clog << "document '" << documentId << "' not found";
      clog << endl;
      return NULL;
    }

  node = document->getNode (compositeId);
  if (node == NULL || !(node->instanceOf ("ContextNode")))
    {
      clog << "PrivateBaseContext::addNode Warning! cant add node: ";
      clog << "parent composite '" << compositeId << "' not found";
      clog << endl;
      return NULL;
    }

  contextNode = (ContextNode *)node;

  try
    {
      clog << "PrivateBaseContext::addNode Trying to add node: '";
      clog << xmlNode << "'" << endl;
      node = (Node *)(compileEntity (xmlNode, document, node));
    }
  catch (exception *exc)
    {
      return NULL;
    }

  if (node == NULL)
    {
      return NULL;
    }

  contextNode->addNode (node);
  return node;
}

InterfacePoint *
PrivateBaseContext::addInterface (string documentId, string nodeId,
                                  string xmlInterface)
{
  NclDocument *document;
  Node *node;
  InterfacePoint *interfacePoint;

  document = getBaseDocument (documentId);
  if (document == NULL)
    {
      return NULL;
    }

  node = document->getNode (nodeId);
  if (node == NULL)
    {
      return NULL;
    }

  try
    {
      interfacePoint = (InterfacePoint *)(compileEntity (xmlInterface,
                                                         document, node));
    }
  catch (exception *ex)
    {
      return NULL;
    }

  if (interfacePoint == NULL)
    {
      return NULL;
    }

  if (interfacePoint->instanceOf ("Anchor"))
    {
      node->addAnchor ((Anchor *)interfacePoint);
      return interfacePoint;
    }
  else if (interfacePoint->instanceOf ("SwitchPort"))
    {
      if (node->instanceOf ("SwitchNode"))
        {
          ((SwitchNode *)node)->addPort ((SwitchPort *)interfacePoint);
          return interfacePoint;
        }
    }
  else if (interfacePoint->instanceOf ("Port"))
    {
      if (node->instanceOf ("ContextNode"))
        {
          ((ContextNode *)node)->addPort ((Port *)interfacePoint);
          return interfacePoint;
        }
    }

  return NULL;
}

Link *
PrivateBaseContext::addLink (string documentId, string compositeId,
                             string xmlLink)
{
  NclDocument *document;
  Node *node;
  ContextNode *contextNode;
  Link *ncmLink;

  document = getBaseDocument (documentId);
  if (document == NULL)
    {
      clog << "PrivateBaseContext::addLink Warning! Can't find ";
      clog << "document '" << documentId << "'" << endl;
      return NULL;
    }

  node = document->getNode (compositeId);
  if (node == NULL)
    {
      clog << "PrivateBaseContext::addLink Warning! Can't find ";
      clog << "composite node '" << compositeId << "'" << endl;
      return NULL;
    }

  if (!node->instanceOf ("ContextNode"))
    {
      clog << "PrivateBaseContext::addLink Warning! Can't add ";
      clog << "link inside a non composite node '" << compositeId;
      clog << "'" << endl;
      return NULL;
    }

  contextNode = (ContextNode *)node;

  try
    {
      ncmLink = (Link *)compileEntity (xmlLink, document, node);
    }
  catch (exception *exc)
    {
      clog << "PrivateBaseContext::addLink Warning! Catch an exception ";
      clog << "when compiling '" << xmlLink << "'" << endl;
      return NULL;
    }

  if (ncmLink == NULL)
    {
      clog << "PrivateBaseContext::addLink Warning! Can't compile link '";
      clog << xmlLink << "'" << endl;
      return NULL;
    }

  contextNode->addLink (ncmLink);
  return ncmLink;
}

PrivateBase *
PrivateBaseContext::getPrivateBase ()
{
  return privateBase;
}

GINGA_FORMATTER_END
