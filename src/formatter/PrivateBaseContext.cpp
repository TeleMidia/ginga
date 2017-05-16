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

#include "nclconv/NclDocumentParser.h"
using namespace ::ginga::nclconv;

#include "PrivateBaseContext.h"

GINGA_FORMATTER_BEGIN

PrivateBaseContext::PrivateBaseContext () { this->privateBase = NULL; }

PrivateBaseContext::~PrivateBaseContext ()
{
  map<string, NclDocument *>::iterator i;
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
PrivateBaseContext::createPrivateBase (const string &id)
{
  g_assert_null (privateBase);
  privateBase = new PrivateBase (id);
}

NclDocument *
PrivateBaseContext::compileDocument (const string &location,
                                     DeviceLayout *deviceLayout)
{
  NclDocumentParser compiler(this, deviceLayout);
  string docUrl (location);
  NclDocument *document = compiler.parse (docUrl);

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
PrivateBaseContext::addDocument (const string &path,
                                 DeviceLayout *deviceLayout)
{
  NclDocument *newDocument;

  newDocument = compileDocument (path, deviceLayout);
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
      clog << "Can't compile document '" << path << "'";
      clog << endl;
    }
  return newDocument;
}

void *
PrivateBaseContext::addVisibleDocument (const string &loc,
                                        DeviceLayout *deviceLayout)
{
  NclDocument *newDocument;
  string id;

  string location = loc;
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
PrivateBaseContext::getVisibleDocument (const string &id)
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
PrivateBaseContext::eraseVisibleDocument (const string &docLocation)
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
PrivateBaseContext::getDocumentLocation (const string &docId)
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

NclDocument *
PrivateBaseContext::getDocument (const string &id)
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
PrivateBaseContext::removeDocumentBase (arg_unused (NclDocument *document),
                                        Base *base)
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
PrivateBaseContext::removeDocument (const string &id)
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
PrivateBaseContext::getBaseDocument (const string &documentId)
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

PrivateBase *
PrivateBaseContext::getPrivateBase ()
{
  return privateBase;
}

GINGA_FORMATTER_END
