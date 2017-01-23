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
#include "NclDocument.h"

GINGA_NCL_BEGIN

NclDocument::NclDocument (string id, string docLocation)
{
  this->id = id;

  ruleBase = NULL;
  transitionBase = NULL;
  descriptorBase = NULL;
  connectorBase = NULL;
  body = NULL;

  parentDocument = NULL;
  this->docLocation = docLocation;
}

NclDocument::~NclDocument () { clearDocument (); }

NclDocument *
NclDocument::getParentDocument ()
{
  return parentDocument;
}

string
NclDocument::getDocumentPerspective ()
{
  string docPerspective;
  NclDocument *parent;

  docPerspective = getId ();

  parent = parentDocument;
  while (parent != NULL)
    {
      docPerspective = parent->getId () + "/" + docPerspective;
      parent = parent->getParentDocument ();
    }

  return docPerspective;
}

void
NclDocument::setParentDocument (NclDocument *parentDocument)
{
  this->parentDocument = parentDocument;
}

string
NclDocument::getDocumentLocation ()
{
  return docLocation;
}

bool
NclDocument::addDocument (NclDocument *document, string alias,
                          string location)
{
  assert (document != NULL);

  if (documentAliases.find (alias) != documentAliases.end ()
      || documentLocations.find (location) != documentLocations.end ())
    {
      return false;
    }

  documentBase.push_back (document);
  documentAliases[alias] = document;
  documentLocations[location] = document;

  return true;
}

void
NclDocument::clearDocument ()
{
  vector<NclDocument *>::iterator i;
  map<int, RegionBase *>::iterator j;
  vector<Meta *>::iterator k;
  vector<Metadata *>::iterator l;

  id = "";

  i = documentBase.begin ();
  while (i != documentBase.end ())
    {
      delete *i;
      ++i;
    }

  documentBase.clear ();
  documentLocations.clear ();
  documentAliases.clear ();

  j = regionBases.begin ();
  while (j != regionBases.end ())
    {
      delete j->second;
      ++j;
    }
  regionBases.clear ();

  if (ruleBase != NULL)
    {
      delete ruleBase;
      ruleBase = NULL;
    }

  if (transitionBase != NULL)
    {
      delete transitionBase;
      transitionBase = NULL;
    }

  if (descriptorBase != NULL)
    {
      delete descriptorBase;
      descriptorBase = NULL;
    }

  if (connectorBase != NULL)
    {
      delete connectorBase;
      connectorBase = NULL;
    }

  if (body != NULL)
    {
      if (Entity::hasInstance (body, true))
        {
          delete body;
        }
      body = NULL;
    }

  k = metainformationList.begin ();
  while (k != metainformationList.end ())
    {
      delete *k;
      ++k;
    }
  metainformationList.clear ();

  l = metadataList.begin ();
  while (l != metadataList.end ())
    {
      delete *l;
      ++l;
    }
  metadataList.clear ();
}

Connector *
NclDocument::getConnector (string connectorId)
{
  Connector *connector;
  vector<NclDocument *>::iterator i;

  if (connectorBase != NULL)
    {
      connector = connectorBase->getConnector (connectorId);
      if (connector != NULL)
        {
          return connector;
        }
    }

  for (i = documentBase.begin (); i != documentBase.end (); ++i)
    {
      connector = (*i)->getConnector (connectorId);
      if (connector != NULL)
        {
          return connector;
        }
    }

  return NULL;
}

ConnectorBase *
NclDocument::getConnectorBase ()
{
  return connectorBase;
}

Transition *
NclDocument::getTransition (string transitionId)
{
  Transition *transition;
  int i, size;
  NclDocument *document;

  if (transitionBase != NULL)
    {
      transition = transitionBase->getTransition (transitionId);
      if (transition != NULL)
        {
          return transition;
        }
    }

  size = (int) documentBase.size ();
  for (i = 0; i < size; i++)
    {
      document = documentBase[i];
      transition = document->getTransition (transitionId);
      if (transition != NULL)
        {
          return transition;
        }
    }

  return NULL;
}

TransitionBase *
NclDocument::getTransitionBase ()
{
  return transitionBase;
}

GenericDescriptor *
NclDocument::getDescriptor (string descriptorId)
{
  GenericDescriptor *descriptor;
  vector<NclDocument *>::iterator i;

  if (descriptorBase != NULL)
    {
      descriptor = descriptorBase->getDescriptor (descriptorId);
      if (descriptor != NULL)
        {
          return descriptor;
        }
    }

  for (i = documentBase.begin (); i != documentBase.end (); ++i)
    {
      descriptor = (*i)->getDescriptor (descriptorId);
      if (descriptor != NULL)
        {
          return descriptor;
        }
    }

  return NULL;
}

DescriptorBase *
NclDocument::getDescriptorBase ()
{
  return descriptorBase;
}

NclDocument *
NclDocument::getDocument (string documentId)
{
  vector<NclDocument *>::iterator i;

  for (i = documentBase.begin (); i != documentBase.end (); ++i)
    {
      if ((*i)->getId () != "" && (*i)->getId () == documentId)
        {
          return (*i);
        }
    }

  return NULL;
}

string
NclDocument::getDocumentAlias (NclDocument *document)
{
  map<string, NclDocument *>::iterator i;

  for (i = documentAliases.begin (); i != documentAliases.end (); ++i)
    {
      if (i->second == document)
        {
          return i->first;
        }
    }

  return "";
}

ContextNode *
NclDocument::getBody ()
{
  return body;
}

string
NclDocument::getDocumentLocation (NclDocument *document)
{
  map<string, NclDocument *>::iterator i;

  for (i = documentLocations.begin (); i != documentLocations.end (); ++i)
    {
      if (i->second == document)
        {
          return i->first;
        }
    }

  return "";
}

vector<NclDocument *> *
NclDocument::getDocuments ()
{
  return &documentBase;
}

string
NclDocument::getId ()
{
  return id;
}

Node *
NclDocument::getNodeLocally (string nodeId)
{
  if (body != NULL)
    {
      if (body->getId () == nodeId)
        {
          return body;
        }
      else
        {
          return body->recursivelyGetNode (nodeId);
        }
    }
  else
    {
      return NULL;
    }
}

Node *
NclDocument::getNode (string nodeId)
{
  string::size_type index;
  string prefix, suffix;
  NclDocument *document;

  index = nodeId.find_first_of ("#");
  if (index == string::npos)
    {
      return getNodeLocally (nodeId);
    }
  else if (index == 0)
    {
      return getNodeLocally (nodeId.substr (1, nodeId.length () - 1));
    }

  prefix = nodeId.substr (0, index);
  index++;
  suffix = nodeId.substr (index, nodeId.length () - index);
  if (documentAliases.find (prefix) != documentAliases.end ())
    {
      document = documentAliases[prefix];
      return document->getNode (suffix);

    } /* else if (documentLocations.find(prefix) !=
                     documentLocations.end()) {
             document = documentLocations[prefix];
             return document->getNode(suffix);
     }*/
  else
    {
      return NULL;
    }

  return NULL;
}

LayoutRegion *
NclDocument::getRegion (string regionId)
{
  LayoutRegion *region;
  map<int, RegionBase *>::iterator i;

  i = regionBases.begin ();
  while (i != regionBases.end ())
    {
      region = getRegion (regionId, i->second);
      if (region != NULL)
        {
          return region;
        }
      ++i;
    }

  return NULL;
}

LayoutRegion *
NclDocument::getRegion (string regionId, RegionBase *regionBase)
{
  LayoutRegion *region;
  vector<NclDocument *>::iterator i;

  if (regionBase != NULL)
    {
      region = regionBase->getRegion (regionId);
      if (region != NULL)
        {
          return region;
        }
    }

  for (i = documentBase.begin (); i != documentBase.end (); ++i)
    {
      region = (*i)->getRegion (regionId);
      if (region != NULL)
        {
          return region;
        }
    }

  return NULL;
}

RegionBase *
NclDocument::getRegionBase (int devClass)
{
  map<int, RegionBase *>::iterator i;

  i = regionBases.find (devClass);
  if (i == regionBases.end ())
    {
      return NULL;
    }

  return i->second;
}

RegionBase *
NclDocument::getRegionBase (string regionBaseId)
{
  map<int, RegionBase *>::iterator i;

  i = regionBases.begin ();
  while (i != regionBases.end ())
    {
      if (i->second->getId () == regionBaseId)
        {
          return i->second;
        }
      ++i;
    }

  return NULL;
}

map<int, RegionBase *> *
NclDocument::getRegionBases ()
{
  return &regionBases;
}

Rule *
NclDocument::getRule (string ruleId)
{
  Rule *rule;
  vector<NclDocument *>::iterator i;

  if (ruleBase != NULL)
    {
      rule = ruleBase->getRule (ruleId);
      if (rule != NULL)
        {
          return rule;
        }
    }

  for (i = documentBase.begin (); i != documentBase.end (); ++i)
    {
      rule = (*i)->getRule (ruleId);
      if (rule != NULL)
        {
          return rule;
        }
    }

  return NULL;
}

RuleBase *
NclDocument::getRuleBase ()
{
  return ruleBase;
}

bool
NclDocument::removeDocument (NclDocument *document)
{
  string alias, location;
  vector<NclDocument *>::iterator i;

  alias = getDocumentAlias (document);
  location = getDocumentLocation (document);
  for (i = documentBase.begin (); i != documentBase.end (); ++i)
    {
      if (*i == document)
        {
          documentBase.erase (i);
          documentAliases.erase (alias);
          documentLocations.erase (location);
          return true;
        }
    }
  return false;
}

void
NclDocument::setConnectorBase (ConnectorBase *connectorBase)
{
  this->connectorBase = connectorBase;
}

void
NclDocument::setTransitionBase (TransitionBase *transitionBase)
{
  this->transitionBase = transitionBase;
}

void
NclDocument::setDescriptorBase (DescriptorBase *descriptorBase)
{
  this->descriptorBase = descriptorBase;
}

void
NclDocument::setDocumentAlias (NclDocument *document, string alias)
{
  string oldAlias;

  oldAlias = getDocumentAlias (document);
  documentAliases.erase (oldAlias);
  documentAliases[alias] = document;
}

void
NclDocument::setBody (ContextNode *node)
{
  body = node;
}

void
NclDocument::setDocumentLocation (NclDocument *document, string location)
{
  string oldLocation;

  oldLocation = getDocumentLocation (document);
  documentLocations.erase (oldLocation);
  documentLocations[location] = document;
}

void
NclDocument::setId (string id)
{
  this->id = id;
}

void
NclDocument::addRegionBase (RegionBase *regionBase)
{
  map<int, RegionBase *>::iterator i;
  int dClass;

  assert (regionBase != NULL);

  dClass = regionBase->getDeviceClass ();
  i = regionBases.find (dClass);
  if (i != regionBases.end ())
    {
      clog << "NclDocument::addRegionBase Warning! Trying to add the ";
      clog << "same regionBase deviceClass = '" << dClass << "'";
      clog << " regionBase twice." << endl;
    }
  else
    {
      regionBases[dClass] = regionBase;
    }

  /*
   * checking if passive region base has an output map region
   * if it does create map region reference on base device region base.
   */
  if (regionBases.find (0) != regionBases.end ()
      && regionBases.find (1) != regionBases.end ())
    {
      RegionBase *baseRegionBase = regionBases[0];
      RegionBase *passiveRegionBase = regionBases[1];
      LayoutRegion *outputMapRegion;
      string regionId;

      regionId = passiveRegionBase->getOutputMapRegionId ();
      if (regionId != "")
        {
          outputMapRegion = baseRegionBase->getRegion (regionId);
          /*
           *  ignoring if the NCL author have specified a
           *  non-existent region id
           */
          if (outputMapRegion != NULL)
            {
              passiveRegionBase->setOutputMapRegion (outputMapRegion);
            }
        }
    }
}

void
NclDocument::setRuleBase (RuleBase *ruleBase)
{
  this->ruleBase = ruleBase;
}

void
NclDocument::addMetainformation (Meta *meta)
{
  assert (meta != NULL);

  metainformationList.push_back (meta);
}

void
NclDocument::addMetadata (Metadata *metadata)
{
  assert (metadata != NULL);

  metadataList.push_back (metadata);
}

vector<Meta *> *
NclDocument::getMetainformation ()
{
  return &metainformationList;
}

vector<Metadata *> *
NclDocument::getMetadata ()
{
  return &metadataList;
}

void
NclDocument::removeRegionBase (string regionBaseId)
{
  map<int, RegionBase *>::iterator i;

  i = regionBases.begin ();
  while (i != regionBases.end ())
    {
      if (i->second->getId () == regionBaseId)
        {
          regionBases.erase (i);
          return;
        }
      ++i;
    }
}

void
NclDocument::removeMetainformation (Meta *meta)
{
  vector<Meta *>::iterator i;

  assert (meta != NULL);

  i = metainformationList.begin ();
  while (i != metainformationList.end ())
    {
      if (*i == meta)
        {
          metainformationList.erase (i);
          break;
        }

      if (i != metainformationList.end ())
        {
          ++i;
        }
    }
}

void
NclDocument::removeMetadata (Metadata *metadata)
{
  vector<Metadata *>::iterator i;

  assert (metadata != NULL);

  i = metadataList.begin ();
  while (i != metadataList.end ())
    {
      if (*i == metadata)
        {
          metadataList.erase (i);
          break;
        }

      if (i != metadataList.end ())
        {
          ++i;
        }
    }
}

void
NclDocument::removeAllMetainformation ()
{
  metainformationList.clear ();
}

void
NclDocument::removeAllMetadata ()
{
  metadataList.clear ();
}

GINGA_NCL_END
