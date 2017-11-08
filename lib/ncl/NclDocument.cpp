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

NclDocument::NclDocument (const string &id, const string &uri)
{
  _id = id;
  _uri = uri;
  _root = new NclContext (this, id);
}

NclDocument::~NclDocument ()
{
  delete _root;
}

string
NclDocument::getId ()
{
  return _id;
}

string
NclDocument::getURI ()
{
  return _uri;
}

NclContext *
NclDocument::getRoot ()
{
  return _root;
}

NclEntity *
NclDocument::getEntityById (const string &id)
{
  map<string, NclEntity *>::iterator it;
  return ((it = _entities.find (id)) == _entities.end ())
    ? nullptr : it->second;
}

bool
NclDocument::registerEntity (NclEntity *entity)
{
  string id = entity->getId ();
  if (this->getEntityById (id))
    return false;
  _entities[id] = entity;
  return true;
}

bool
NclDocument::unregisterEntity (NclEntity *entity)
{
  string id = entity->getId ();
  if (!this->getEntityById (id))
    return false;
  _entities[id] = nullptr;
  return true;
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


GINGA_NAMESPACE_END
