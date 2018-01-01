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
#include "DsmccServiceDomain.h"

GINGA_PRAGMA_DIAG_IGNORE (-Wconversion)

GINGA_DATAPROC_BEGIN

DsmccServiceDomain::DsmccServiceDomain (DsmccDownloadServerInitiate *dsi,
                                        DsmccDownloadInfoIndication *dii,
                                        unsigned short pid)
    : Thread ()
{
  Thread::mutexInit (&stlMutex, true);

  this->serviceGatewayIor = dsi->getServiceGatewayIor ();
  this->carouselId = dii->getDonwloadId ();
  blockSize = dii->getBlockSize ();

  dii->getInfo (&this->info);

  mounted = false;
  hasServiceGateway = false;
  mountingServiceDomain = true;
  xstrassign (mountPoint,
              "%s/ginga/carousel/%u.%u/",
              g_get_tmp_dir (), pid, carouselId);

  remove (mountPoint.c_str ());
  g_mkdir (mountPoint.c_str (), 0777);

  processor = new DsmccObjectProcessor (pid);
  sdl = NULL;

  startThread ();
}

DsmccServiceDomain::~DsmccServiceDomain ()
{
  map<unsigned int, DsmccModule *>::iterator i;
  set<DsmccModule *>::iterator j;

  Thread::mutexLock (&stlMutex);

  if (processor != NULL)
    {
      delete processor;
      processor = NULL;
    }

  // modules are deleted in DII destructor
  info.clear ();

  Thread::mutexUnlock (&stlMutex);
  Thread::mutexDestroy (&stlMutex);
}

void
DsmccServiceDomain::setServiceDomainListener (
    IDsmccServiceDomainListener *sdl)
{
  this->sdl = sdl;
}

void
DsmccServiceDomain::setObjectsListeners (set<IDsmccObjectListener *> *l)
{
  processor->setObjectsListeners (l);
}

int
DsmccServiceDomain::receiveDDB (DsmccDownloadDataBlock *ddb)
{
  if (ddb->processDataBlock (&info) < 0)
    {
      clog << "DsmccServiceDomain::receiveDDB - error." << endl;
      return -1;
    }
  return 0;
}

DsmccModule *
DsmccServiceDomain::getModuleById (unsigned int id)
{
  DsmccModule *mod = NULL;

  Thread::mutexLock (&stlMutex);
  if (info.count (id) != 0)
    {
      mod = info[id];
    }
  Thread::mutexUnlock (&stlMutex);

  return mod;
}

bool
DsmccServiceDomain::isMounted ()
{
  return mounted;
}

DsmccModule *
DsmccServiceDomain::getModule (int pos)
{
  DsmccModule *module = NULL;
  int i;
  map<unsigned int, DsmccModule *>::iterator j;

  Thread::mutexLock (&stlMutex);

  j = info.begin ();
  for (i = 0; i < pos; i++)
    {
      j++;

      if (j == info.end ())
        {
          Thread::mutexUnlock (&stlMutex);
          return NULL;
        }
    }

  module = j->second;

  Thread::mutexUnlock (&stlMutex);

  return module;
}

map<unsigned int, DsmccModule *> *
DsmccServiceDomain::getInfo ()
{
  return &info;
}

void
DsmccServiceDomain::eraseModule (DsmccModule *module)
{
  map<unsigned int, DsmccModule *>::iterator i;

  Thread::mutexLock (&stlMutex);

  i = info.begin ();
  while (i != info.end ())
    {
      if (i->second == module)
        {
          info.erase (i);
          if (remove (module->getModuleFileName ().c_str ()) == -1)
            {
              clog << errno << endl;
            }
          Thread::mutexUnlock (&stlMutex);
          return;
        }
      ++i;
    }

  Thread::mutexUnlock (&stlMutex);
}

bool
DsmccServiceDomain::hasModules ()
{
  Thread::mutexLock (&stlMutex);

  if (info.empty ())
    {
      if (!processor->hasObjects ())
        {
          mountingServiceDomain = false;
        }

      Thread::mutexUnlock (&stlMutex);
      return false;
    }

  map<unsigned int, DsmccModule *>::iterator i;
  for (i = info.begin (); i != info.end (); ++i)
    {
      if ((i->second)->isConsolidated ())
        {
          Thread::mutexUnlock (&stlMutex);
          return true;
        }
    }

  Thread::mutexUnlock (&stlMutex);

  return false;
}

unsigned short
DsmccServiceDomain::getBlockSize ()
{
  return blockSize;
}

void
DsmccServiceDomain::run ()
{
  DsmccModule *module = NULL;
  DsmccBiop *biop;
  map<unsigned int, DsmccModule *>::iterator i;
  unsigned int modId;
  int j = 0;

  while (mountingServiceDomain)
    {
      if (hasModules ())
        {
          if (!hasServiceGateway)
            {
              modId = serviceGatewayIor->getModuleId ();

              Thread::mutexLock (&stlMutex);
              if (info.count (modId) == 0)
                {
                  Thread::mutexUnlock (&stlMutex);
                  break;
                }
              else
                {
                  module = info[modId];
                }

              while (!module->isConsolidated ())
                {
                  g_usleep (1000);
                }
              clog << endl;

              try
                {
                  biop = new DsmccBiop (module, processor);
                }
              catch (...)
                {
                  clog << "DsmccServiceDomain::run - error: BIOP - SRG not "
                          "processed."
                       << endl;
                  return;
                }

              biop->processServiceGateway (
                  serviceGatewayIor->getObjectKey ());

              hasServiceGateway = true;

              biop->process ();
              delete biop;
              biop = NULL;

              Thread::mutexUnlock (&stlMutex);

              j = 0;
              continue;
            }
          else
            {
              module = getModule (j);
              if (module == NULL)
                {
                  j = 0;
                  continue;
                }

              if (module->isConsolidated ())
                {
                  try
                    {
                      biop = new DsmccBiop (module, processor);
                    }
                  catch (...)
                    {
                      clog << "DsmccServiceDomain::run BIOP->process (init "
                              "error)"
                           << endl;
                      return;
                    }

                  biop->process ();

                  eraseModule (module);

                  delete biop;
                  biop = NULL;
                }
              else
                {
                  g_usleep (1000);
                  j++;
                }
            }
        }
      else
        {
          g_usleep (1000);
        }
    }
  mounted = true;
  clog << "DsmccServiceDomain::run ";
  clog << "CAROUSEL " << carouselId << " MOUNTED!" << endl;
  if (sdl != NULL)
    {
      sdl->serviceDomainMounted (mountPoint, processor->getSDNames (),
                                 processor->getSDPaths ());
    }
}

GINGA_DATAPROC_END
