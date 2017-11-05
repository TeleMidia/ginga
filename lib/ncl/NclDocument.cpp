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

#include "NclMedia.h"

GINGA_NAMESPACE_BEGIN

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
NclContext *
NclDocument::getRoot ()
{
  return _root;
}

/**
 * @brief Gets entity.
 * @param id NclEntity id.
 * @return NclEntity.
 */
NclEntity *
NclDocument::getEntityById (const string &id)
{
  map<string, NclEntity *>::iterator it;
  return ((it = _entities.find (id)) == _entities.end ())
    ? nullptr : it->second;
}

/**
 * @brief Registers entity.
 * @param entity NclEntity.
 * @return True if successful, or false otherwise.
 */
bool
NclDocument::registerEntity (NclEntity *entity)
{
  string id = entity->getId ();
  if (this->getEntityById (id))
    return false;
  _entities[id] = entity;
  return true;
}

/**
 * @brief Unregisters entity.
 * @param entity NclEntity.
 * @return True if successful, or false otherwise.
 */
bool
NclDocument::unregisterEntity (NclEntity *entity)
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
  _root = new NclContext (this, id);

  _connectorBase = nullptr;
  _parentDocument = nullptr;
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


void
NclDocument::addDocument (NclDocument *document, const string &alias,
                          const string &location)
{
  g_assert_nonnull (document);
  if (_documentAliases.find (alias) != _documentAliases.end ()
      || _documentLocations.find (location) != _documentLocations.end ())
    {
      return;
    }

  _documentBase.push_back (document);
  _documentAliases[alias] = document;
  _documentLocations[location] = document;
}

NclConnector *
NclDocument::getConnector (const string &connectorId)
{
  NclConnector *connector;
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

NclConnectorBase *
NclDocument::getConnectorBase ()
{
  return _connectorBase;
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


NclNode *
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

NclNode *
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

vector<NclNode *> *
NclDocument::getSettingsNodes ()
{
  NclContext *body;
  list<NclNode *> compositions;

  const vector<NclNode *> *nodes;
  vector<NclNode *> *settings;

  body = this->getRoot ();
  g_assert_nonnull (body);

  settings = new vector<NclNode *>;
  compositions.push_back (body);

 next:
  g_assert (compositions.size () > 0);
  nodes = ((NclComposition *)(compositions.front ()))->getNodes ();
  g_assert_nonnull (nodes);
  compositions.pop_front ();

  for (guint i = 0; i < nodes->size (); i++)
    {
      NclNode *node = cast (NclNode *, nodes->at (i)->derefer ());
      g_assert_nonnull (node);

      if (instanceof (NclMedia *, node)
          && ((NclMedia *) node)->isSettings ())
        {
          //
          // WARNING: For some obscure reason, we have to store the NclNode,
          // not the EntityNode.
          //
          settings->push_back (nodes->at (i)); // found
        }
      else if (instanceof (NclComposition *, node))
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
NclDocument::setConnectorBase (NclConnectorBase *connectorBase)
{
  this->_connectorBase = connectorBase;
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

GINGA_NAMESPACE_END
