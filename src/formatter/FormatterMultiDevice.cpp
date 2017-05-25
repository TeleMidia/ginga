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
  this->activeBaseUri = "";
  this->presContext = NULL;
  this->focusManager = NULL;

  if (defaultWidth == 0)
    Ginga_Display->getSize (&defaultWidth, NULL);

  if (defaultHeight == 0)
    Ginga_Display->getSize (NULL, &defaultHeight);

  int tmpw, tmph;
  Ginga_Display->getSize (&tmpw, &tmph);

  this->layoutManager= new NclFormatterLayout (w, h);

  Thread::mutexInit (&mutex, false);
}

FormatterMultiDevice::~FormatterMultiDevice ()
{
  set<IPlayer *>::iterator i;

  presContext = NULL;

  Thread::mutexDestroy (&mutex);

  clog << "FormatterMultiDevice::~FormatterMultiDevice ";
  clog << "all done";
  clog << endl;
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

NclFormatterLayout *
FormatterMultiDevice::getFormatterLayout ()
{
  return this->layoutManager;
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

  descriptor = executionObject->getDescriptor ();
  if (descriptor != NULL)
    {
      layout = getFormatterLayout ();
      if (layout != NULL)
        {
          if (descriptor->getFormatterRegion () != NULL)
            {
              plan = descriptor->getFormatterRegion ()->getPlan ();
            }

          windowId = layout->prepareFormatterRegion (executionObject,plan);

          layout = this->layoutManager;
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

  descriptor = executionObject->getDescriptor ();
  if (descriptor != NULL)
    {
      region = descriptor->getRegion ();
      layout = getFormatterLayout ();
      if (region != NULL && layout != NULL)
        {
          fRegion = descriptor->getFormatterRegion ();
          if (fRegion != NULL)
            {
              fRegion->setGhostRegion (true);
            }
          layout->showObject (executionObject);
        }
    }
}

void
FormatterMultiDevice::hideObject (NclExecutionObject *executionObject)
{
  NclFormatterLayout *layout;
  NclCascadingDescriptor *descriptor;
  LayoutRegion *region;
  string fileUri;

  clog << "FormatterMultiDevice::hideObject '";
  clog << executionObject->getId () << "'" << endl;

  descriptor = executionObject->getDescriptor ();
  if (descriptor != NULL)
    {
      region = descriptor->getRegion ();
      layout = getFormatterLayout ();
      if (region != NULL && layout != NULL)
        {
              layout->hideObject (executionObject);
        }
    }
}

GINGA_FORMATTER_END
