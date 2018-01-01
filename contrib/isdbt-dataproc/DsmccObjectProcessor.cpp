/* Copyright (C) 2006-2018 PUC-Rio/Laboratorio TeleMidia

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
along with Ginga.  If not, see <https://www.gnu.org/licenses/>.  */

#include "ginga.h"
#include "DsmccObjectProcessor.h"

GINGA_DATAPROC_BEGIN

DsmccObjectProcessor::DsmccObjectProcessor (unsigned short pid)
{
  this->pid = pid;
}

DsmccObjectProcessor::~DsmccObjectProcessor ()
{
  map<string, DsmccObject *>::iterator i;

  i = objects.begin ();
  while (i != objects.end ())
    {
      delete i->second;
      ++i;
    }

  objects.clear ();

  objectNames.clear ();
  objectPaths.clear ();
  listeners.clear ();
}

void
DsmccObjectProcessor::setObjectsListeners (set<IDsmccObjectListener *> *l)
{
  listeners.clear ();
  listeners.insert (l->begin (), l->end ());
}

void
DsmccObjectProcessor::pushObject (DsmccObject *object)
{
  map<string, DsmccObject *>::iterator i;

  objects[object->getObjectId ()] = object;

  i = objects.begin ();
  while (i != objects.end ())
    {
      object = i->second;

      if (mountObject (object))
        {
          notifyObjectListeners (object);

          objects.erase (i);
          i = objects.begin ();

          delete object;
          object = NULL;
        }
      else
        {
          ++i;
        }
    }
}

bool
DsmccObjectProcessor::hasObjects ()
{
  if (objects.empty ())
    {
      return false;
    }

  return true;
}

map<string, string> *
DsmccObjectProcessor::getSDNames ()
{
  return &objectNames;
}

map<string, string> *
DsmccObjectProcessor::getSDPaths ()
{
  return &objectPaths;
}

bool
DsmccObjectProcessor::mountObject (DsmccObject *object)
{
  vector<DsmccBinding *> *bindings;
  vector<DsmccBinding *>::iterator i;
  string objectId, path, strToken;
  FILE *fd;
  char *data;
  char token[6];
  unsigned int j, size;

  if (object->getKind () == "srg"
      || object->getKind () == "DSM::ServiceGateway")
    {
      bindings = object->getBindings ();
      for (i = bindings->begin (); i != bindings->end (); ++i)
        {
          xstrassign (objectId, "%u%u%u",
                      (*i)->getIor ()->getCarouselId (),
                      (*i)->getIor ()->getModuleId (),
                      (*i)->getIor ()->getObjectKey ());

          objectNames[objectId] = (*i)->getId ();
          xstrassign (objectPaths[objectId],
                      "%s/ginga/carousel/%u.%u/",
                      g_get_tmp_dir (), pid, object->getCarouselId ());
        }

      return true;
    }
  else if (object->getKind () == "dir"
           || object->getKind () == "DSM::Directory")
    {
      if (objectPaths.count (object->getObjectId ()) == 0)
        {
          return false;
        }
      else
        {
          path = (objectPaths.find (object->getObjectId ()))->second
                 + (objectNames.find (object->getObjectId ()))->second
                 + "/";

          g_mkdir (path.c_str (), 0777);
        }

      bindings = object->getBindings ();
      for (i = bindings->begin (); i != bindings->end (); ++i)
        {
          xstrassign (objectId, "%u%u%u",
                      (*i)->getIor ()->getCarouselId (),
                      (*i)->getIor ()->getModuleId (),
                      (*i)->getIor ()->getObjectKey ());

          objectNames[objectId] = (*i)->getId ();
          objectPaths[objectId] = path;
        }
      return true;
    }
  else if (object->getKind () == "fil" || object->getKind () == "DSM::File")
    {
      if (objectPaths.count (object->getObjectId ()) == 0)
        {
          clog << "DsmccObjectProcessor::mountObject Warning! ";
          clog << "cant find object id '" << object->getObjectId ();
          clog << "" << endl;
          return false;
        }
      else
        {
          path = (objectPaths.find (object->getObjectId ()))->second
                 + (objectNames.find (object->getObjectId ()))->second;

          fd = fopen (path.c_str (), "w+b");
          size = object->getDataSize ();
          if (fd != NULL)
            {
              // TODO: correct BUG in content provider
              if (path.find (".ncl") != std::string::npos)
                {
                  data = object->getData ();
                  j = 0;
                  while (j < size)
                    {
                      memcpy ((void *)&(token[0]), (void *)&(data[j]), 6);
                      strToken = (string) (char *)token;
                      if (strToken.find ("</ncl>") != std::string::npos)
                        {
                          size = j + 6;
                          break;
                        }
                      j++;
                    }
                }

              fwrite ((void *)(object->getData ()), 1, size, fd);
              fclose (fd);
            }
          else
            {
              clog << "Warning! Cannot mount ";
              clog << path.c_str () << endl;
              return false;
            }
        }
      return true;
    }

  clog << "DsmccObjectProcessor::mountObject Warning! unrecognized type";
  clog << endl;
  return false;
}

void
DsmccObjectProcessor::notifyObjectListeners (DsmccObject *obj)
{
  set<IDsmccObjectListener *>::iterator i;
  string clientUri = "";
  string name = "";
  string objectId;

  objectId = obj->getObjectId ();
  if (objectPaths.count (objectId) != 0)
    {
      clientUri = objectPaths[objectId];
    }

  if (objectNames.count (objectId) != 0)
    {
      name = objectNames[objectId];
    }

  i = listeners.begin ();
  while (i != listeners.end ())
    {
      (*i)->objectMounted (objectId, clientUri, name);
      ++i;
    }
}

GINGA_DATAPROC_END
