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

#include "aux-ginga.h"
#include "NclDocument.h"

#include "Media.h"

GINGA_NCL_BEGIN

/**
 * @brief Gets document id.
 */
string
NclDocument::getId ()
{
  return _id;
}

/**
 * @brief Gets document location.
 */
string
NclDocument::getURI ()
{
  return _uri;
}

/**
 * @brief Gets document body.
 */
Context *
NclDocument::getRoot ()
{
  return _root;
}

/**
 * @brief Gets entity.
 * @param id Entity id.
 * @return Entity.
 */
Entity *
NclDocument::getEntityById (const string &id)
{
  map<string, Entity *>::iterator it;
  return ((it = _entities.find (id)) == _entities.end ())
    ? nullptr : it->second;
}

/**
 * @brief Registers entity.
 * @param entity Entity.
 * @return True if successful, or false otherwise.
 */
bool
NclDocument::registerEntity (Entity *entity)
{
  string id = entity->getId ();
  if (this->getEntityById (id))
    return false;
  _entities[id] = entity;
  return true;
}

/**
 * @brief Unregisters entity.
 * @param entity Entity.
 * @return True if successful, or false otherwise.
 */
bool
NclDocument::unregisterEntity (Entity *entity)
{
  string id = entity->getId ();
  if (!this->getEntityById (id))
    return false;
  _entities[id] = nullptr;
  return true;
}


// INSANITY BEGINS HERE ----------------------------------------------------

/**
 * @brief Creates a new document.
 * @param id Document id.
 * @param uri Document URI.
 */
NclDocument::NclDocument (const string &id, const string &uri)
{
  _id = id;
  _uri = uri;
  _root = new Context (this, id);

  _connectorBase = nullptr;
  _descriptorBase = nullptr;
  _parentDocument = nullptr;
  _ruleBase = nullptr;
  _transitionBase = nullptr;
}

/**
 * @brief Destroys document.
 */
NclDocument::~NclDocument ()
{
  delete _root;
}

NclDocument *
NclDocument::getParentDocument ()
{
  return _parentDocument;
}

string
NclDocument::getDocumentPerspective ()
{
  string docPerspective;
  NclDocument *parent;

  docPerspective = getId ();

  parent = _parentDocument;
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
  this->_parentDocument = parentDocument;
}


bool
NclDocument::addDocument (NclDocument *document, const string &alias,
                          const string &location)
{
  g_assert (document != NULL);

  if (_documentAliases.find (alias) != _documentAliases.end ()
      || _documentLocations.find (location) != _documentLocations.end ())
    {
      return false;
    }

  _documentBase.push_back (document);
  _documentAliases[alias] = document;
  _documentLocations[location] = document;

  return true;
}

Connector *
NclDocument::getConnector (const string &connectorId)
{
  Connector *connector;
  vector<NclDocument *>::iterator i;

  if (_connectorBase != NULL)
    {
      connector = _connectorBase->getConnector (connectorId);
      if (connector != NULL)
        {
          return connector;
        }
    }

  for (i = _documentBase.begin (); i != _documentBase.end (); ++i)
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
  return _connectorBase;
}

Transition *
NclDocument::getTransition (const string &transitionId)
{
  Transition *transition;
  size_t i, size;
  NclDocument *document;

  if (_transitionBase != NULL)
    {
      transition = _transitionBase->getTransition (transitionId);
      if (transition != NULL)
        {
          return transition;
        }
    }

  size = _documentBase.size ();
  for (i = 0; i < size; i++)
    {
      document = _documentBase[i];
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
  return _transitionBase;
}

Descriptor *
NclDocument::getDescriptor (const string &descriptorId)
{
  Descriptor *descriptor;
  vector<NclDocument *>::iterator i;

  if (_descriptorBase != NULL)
    {
      descriptor = _descriptorBase->getDescriptor (descriptorId);
      if (descriptor != NULL)
        {
          return descriptor;
        }
    }

  for (i = _documentBase.begin (); i != _documentBase.end (); ++i)
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
  return _descriptorBase;
}

NclDocument *
NclDocument::getDocument (const string &documentId)
{
  vector<NclDocument *>::iterator i;

  for (i = _documentBase.begin (); i != _documentBase.end (); ++i)
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

  for (i = _documentAliases.begin (); i != _documentAliases.end (); ++i)
    {
      if (i->second == document)
        {
          return i->first;
        }
    }

  return "";
}

string
NclDocument::getDocumentLocation (NclDocument *document)
{
  map<string, NclDocument *>::iterator i;

  for (i = _documentLocations.begin (); i != _documentLocations.end (); ++i)
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
  return &_documentBase;
}


Node *
NclDocument::getNodeLocally (const string &nodeId)
{
  if (_root != NULL)
    {
      if (_root->getId () == nodeId)
        {
          return _root;
        }
      else
        {
          return _root->getNestedNode (nodeId);
        }
    }
  else
    {
      return NULL;
    }
}

Node *
NclDocument::getNode (const string &nodeId)
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
  if (_documentAliases.find (prefix) != _documentAliases.end ())
    {
      document = _documentAliases[prefix];
      return document->getNode (suffix);

    }
  else
    {
      return NULL;
    }

  return NULL;
}

Region *
NclDocument::getRegion (const string &regionId)
{
  Region *region;
  map<int, RegionBase *>::iterator i;

  i = _regionBases.begin ();
  while (i != _regionBases.end ())
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

Region *
NclDocument::getRegion (const string &regionId, RegionBase *regionBase)
{
  Region *region;
  vector<NclDocument *>::iterator i;

  if (regionBase != NULL)
    {
      region = regionBase->getRegion (regionId);
      if (region != NULL)
        {
          return region;
        }
    }

  for (i = _documentBase.begin (); i != _documentBase.end (); ++i)
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

  i = _regionBases.find (devClass);
  if (i == _regionBases.end ())
    {
      return NULL;
    }

  return i->second;
}

RegionBase *
NclDocument::getRegionBase (const string &regionBaseId)
{
  map<int, RegionBase *>::iterator i;

  i = _regionBases.begin ();
  while (i != _regionBases.end ())
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
  return &_regionBases;
}

Rule *
NclDocument::getRule (const string &ruleId)
{
  Rule *rule;
  vector<NclDocument *>::iterator i;

  if (_ruleBase != NULL)
    {
      rule = _ruleBase->getRule (ruleId);
      if (rule != NULL)
        {
          return rule;
        }
    }

  for (i = _documentBase.begin (); i != _documentBase.end (); ++i)
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
  return _ruleBase;
}

/**
 * @return A vector containing all settings nodes in document.
 */
vector<Node *> *
NclDocument::getSettingsNodes ()
{
  Context *body;
  list<Node *> compositions;

  const vector<Node *> *nodes;
  vector<Node *> *settings;

  body = this->getRoot ();
  g_assert_nonnull (body);

  settings = new vector<Node *>;
  compositions.push_back (body);

 next:
  g_assert (compositions.size () > 0);
  nodes = ((Composition *)(compositions.front ()))->getNodes ();
  g_assert_nonnull (nodes);
  compositions.pop_front ();

  for (guint i = 0; i < nodes->size (); i++)
    {
      Node *node = cast (Node *, nodes->at (i)->derefer ());
      g_assert_nonnull (node);

      if (instanceof (Media *, node)
          && ((Media *) node)->isSettings ())
        {
          //
          // WARNING: For some obscure reason, we have to store the Node,
          // not the EntityNode.
          //
          settings->push_back (nodes->at (i)); // found
        }
      else if (instanceof (Composition *, node))
        {
          compositions.push_back (node);
        }
    }
  if (compositions.size () > 0)
    goto next;

  return settings;
}

bool
NclDocument::removeDocument (NclDocument *document)
{
  string alias, location;
  vector<NclDocument *>::iterator i;

  alias = getDocumentAlias (document);
  location = getDocumentLocation (document);
  for (i = _documentBase.begin (); i != _documentBase.end (); ++i)
    {
      if (*i == document)
        {
          _documentBase.erase (i);
          _documentAliases.erase (alias);
          _documentLocations.erase (location);
          return true;
        }
    }
  return false;
}

void
NclDocument::setConnectorBase (ConnectorBase *connectorBase)
{
  this->_connectorBase = connectorBase;
}

void
NclDocument::setTransitionBase (TransitionBase *transitionBase)
{
  this->_transitionBase = transitionBase;
}

void
NclDocument::setDescriptorBase (DescriptorBase *descriptorBase)
{
  this->_descriptorBase = descriptorBase;
}

void
NclDocument::setDocumentAlias (NclDocument *document, const string &alias)
{
  string oldAlias;

  oldAlias = getDocumentAlias (document);
  _documentAliases.erase (oldAlias);
  _documentAliases[alias] = document;
}

void
NclDocument::setDocumentLocation (NclDocument *document, const string &location)
{
  string oldLocation;

  oldLocation = getDocumentLocation (document);
  _documentLocations.erase (oldLocation);
  _documentLocations[location] = document;
}

void
NclDocument::setId (const string &id)
{
  this->_id = id;
}

void
NclDocument::addRegionBase (RegionBase *regionBase)
{
  g_assert (regionBase != NULL);
  _regionBases[0] = regionBase;
}

void
NclDocument::setRuleBase (RuleBase *ruleBase)
{
  this->_ruleBase = ruleBase;
}

void
NclDocument::removeRegionBase (const string &regionBaseId)
{
  map<int, RegionBase *>::iterator i;

  i = _regionBases.begin ();
  while (i != _regionBases.end ())
    {
      if (i->second->getId () == regionBaseId)
        {
          _regionBases.erase (i);
          return;
        }
      ++i;
    }
}

GINGA_NCL_END
