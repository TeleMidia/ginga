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

#ifndef NCL_DOCUMENT_H
#define NCL_DOCUMENT_H

#include "ConnectorBase.h"
#include "Context.h"

GINGA_NCL_BEGIN

class NclDocument
{
public:
  NclDocument (const string &, const string &);
  ~NclDocument ();

  string getId ();
  string getURI ();
  Context *getRoot ();

  Entity *getEntityById (const string &);
  bool registerEntity (Entity *);
  bool unregisterEntity (Entity *);

private:
  string _id;                       // document id
  string _uri;                      // document uri
  Context *_root;                   // document root
  map<string, Entity *> _entities;  // entity map

  // -----------------------------------------------------------------------
public:
  NclDocument *getParentDocument ();
  void setParentDocument (NclDocument *);
  string getDocumentPerspective ();

  bool addDocument (NclDocument *document, const string &alias,
                    const string &location);
  Connector *getConnector (const string &connectorId);
  ConnectorBase *getConnectorBase ();

  NclDocument *getDocument (const string &documentId);
  string getDocumentAlias (NclDocument *document);
  string getDocumentLocation (NclDocument *document);
  vector<NclDocument *> *getDocuments ();


  Node *getNode (const string &nodeId);

  vector<Node *> *getSettingsNodes ();
  bool removeDocument (NclDocument *document);
  void setConnectorBase (ConnectorBase *_connectorBase);
  void setDocumentAlias (NclDocument *document, const string &alias);

  void setDocumentLocation (NclDocument *document, const string &location);
  void setId (const string &_id);

private:
  ConnectorBase *_connectorBase;
  map<string, NclDocument *> _documentAliases;
  vector<NclDocument *> _documentBase;
  map<string, NclDocument *> _documentLocations;

  NclDocument *_parentDocument;

  Node *getNodeLocally (const string &nodeId);
};

GINGA_NCL_END

#endif // NCL_DOCUMENT_H
