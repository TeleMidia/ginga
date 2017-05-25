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
#include "FormatterMultiDevice.h"

#include "FormatterMediator.h"

#include "mb/Display.h"
using namespace ::ginga::mb;

GINGA_FORMATTER_BEGIN

FormatterMultiDevice::FormatterMultiDevice (int w, int h)
{
  this->defaultWidth = w;
  this->defaultHeight = h;
  this->deviceClass = -1;
  this->activeBaseUri = "";
  this->activeUris = NULL;
  this->presContext = NULL;
  this->focusManager = NULL;
  this->parent = NULL;

  if (defaultWidth == 0)
    Ginga_Display->getSize (&defaultWidth, NULL);

  if (defaultHeight == 0)
    Ginga_Display->getSize (NULL, &defaultHeight);

  int tmpw, tmph;
  Ginga_Display->getSize (&tmpw, &tmph);

  Thread::mutexInit (&mutex, false);
  Thread::mutexInit (&lMutex, false);
}

FormatterMultiDevice::~FormatterMultiDevice ()
{
  set<IPlayer *>::iterator i;

  presContext = NULL;

  clog << "FormatterMultiDevice::~FormatterMultiDevice ";
  clog << "checking listening list";
  clog << endl;

  if (!listening.empty ())
    {
      Thread::mutexLock (&lMutex);
      /*			i = listening.begin();
                              while (i != listening.end()) {
                                      (*i)->removeListener(this);
                                      ++i;
                              }*/
      listening.clear ();
      Thread::mutexUnlock (&lMutex);
    }

  clog << "FormatterMultiDevice::~FormatterMultiDevice ";
  clog << "destroying mutexes";
  clog << endl;

  Thread::mutexDestroy (&mutex);
  Thread::mutexDestroy (&lMutex);


  clog << "FormatterMultiDevice::~FormatterMultiDevice ";
  clog << "all done";
  clog << endl;
}


void
FormatterMultiDevice::setParent (FormatterMultiDevice *parent)
{
  this->parent = (FormatterMultiDevice *)parent;
}

void
FormatterMultiDevice::setPresentationContex (
    PresentationContext *presContext)
{
  this->presContext = presContext;
}

void
FormatterMultiDevice::setFocusManager (void *focusManager)
{
  this->focusManager = focusManager;
}

void *
FormatterMultiDevice::getMainLayout ()
{
  return mainLayout;
}

void *
FormatterMultiDevice::getFormatterLayout (int devClass)
{
  map<int, NclFormatterLayout *>::iterator i;

  i = layoutManager.find (devClass);
  if (i != layoutManager.end ())
    {
      return i->second;
    }

  return NULL;
}

NclFormatterLayout *
FormatterMultiDevice::getFormatterLayout (
    NclCascadingDescriptor *descriptor, NclExecutionObject *object)
{
  map<int, NclFormatterLayout *>::iterator i;
  LayoutRegion *region;

  region = descriptor->getRegion ();
  if (region == NULL)
    {
      if (layoutManager.count (this->deviceClass) != 0)
        {
          region = descriptor->getRegion (layoutManager[this->deviceClass],
                                          object);
        }

      if (region == NULL)
        {
          clog << "FormatterMultiDevice::getFormatterLayout ";
          clog << "region is NULL";
          clog << endl;
          return NULL;
        }
    }

  i = layoutManager.find (0);
  return i->second;
}

SDLWindow*
FormatterMultiDevice::prepareFormatterRegion (
    NclExecutionObject *executionObject)
{
  NclFormatterLayout *layout;
  NclCascadingDescriptor *descriptor;
  string regionId, plan = "";
  SDLWindow* windowId = 0;

  map<int, NclFormatterLayout *>::iterator i;
  LayoutRegion *bitMapRegion;
  LayoutRegion *ncmRegion;

  descriptor = executionObject->getDescriptor ();
  if (descriptor != NULL)
    {
      layout = getFormatterLayout (descriptor, executionObject);
      if (layout != NULL)
        {
          if (descriptor->getFormatterRegion () != NULL)
            {
              plan = descriptor->getFormatterRegion ()->getPlan ();
            }

          windowId = layout->prepareFormatterRegion (executionObject,plan);

          regionId = layout->getBitMapRegionId ();
          /*clog << endl;
          clog << "FormatterMultiDevice::prepareFormatterRegion map '";
          clog << regionId << "'" << endl;*/

          if (regionId == "")
            {
              return windowId;
            }

          i = layoutManager.find (1);
          if (i == layoutManager.end ())
            {
              /*clog << endl;
              clog << "FormatterMultiDevice::prepareFormatterRegion ";
              clog << "CANT FIND devClass '" << deviceClass << "'";
              clog << endl;*/
              return windowId;
            }

          layout = i->second;

          ncmRegion = layout->getDeviceRegion ();

          bitMapRegion = ncmRegion->getOutputMapRegion ();
          if (bitMapRegion == NULL)
            {
              clog << endl;
              clog << "FormatterMultiDevice::prepareFormatterRegion(";
              clog << this << ") ";
              clog << "CANT FIND bitMapRegion";
              clog << " for id '" << regionId << "' devClass = '";
              clog << deviceClass << "'" << endl;
              clog << endl;
              return windowId;
            }
        }
    }

  return windowId;
}

void
FormatterMultiDevice::showObject (NclExecutionObject *executionObject)
{
  NclFormatterLayout *layout;
  NclCascadingDescriptor *descriptor;
  NclFormatterRegion *fRegion;
  LayoutRegion *region;
  int devClass;
  string fileUri;
  string url;
  string relativePath;
  string tempRelPath;
  string value;

  descriptor = executionObject->getDescriptor ();
  if (descriptor != NULL)
    {
      region = descriptor->getRegion ();
      layout = getFormatterLayout (descriptor, executionObject);
      if (region != NULL && layout != NULL)
        {
          devClass = region->getDeviceClass ();

          clog << "FormatterMultiDevice::showObject '";
          clog << executionObject->getId () << "' class '";
          clog << devClass << "'" << endl;

          fRegion = descriptor->getFormatterRegion ();
          if (devClass != DeviceDomain::CT_BASE)
            {
              clog << "FormatterMultiDevice::showObject as base" << endl;

              if (fRegion != NULL)
                {
                  fRegion->setGhostRegion (true);
                }
            }

          if (devClass != DeviceDomain::CT_ACTIVE)
            {
              layout->showObject (executionObject);
            }
        }
    }
}

void
FormatterMultiDevice::hideObject (NclExecutionObject *executionObject)
{
  NclFormatterLayout *layout;
  NclCascadingDescriptor *descriptor;
  LayoutRegion *region;
  int devClass;
  string fileUri;

  clog << "FormatterMultiDevice::hideObject '";
  clog << executionObject->getId () << "'" << endl;

  descriptor = executionObject->getDescriptor ();
  if (descriptor != NULL)
    {
      region = descriptor->getRegion ();
      layout = getFormatterLayout (descriptor, executionObject);
      if (region != NULL && layout != NULL)
        {
          devClass = region->getDeviceClass ();
          if (devClass != DeviceDomain::CT_ACTIVE)
            {
              layout->hideObject (executionObject);
            }
        }
    }
}

GINGA_FORMATTER_END
