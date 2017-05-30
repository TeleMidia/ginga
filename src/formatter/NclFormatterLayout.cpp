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
#include "NclFormatterLayout.h"

GINGA_FORMATTER_BEGIN

NclFormatterLayout::NclFormatterLayout (int w, int h)
{
  region = new LayoutRegion ("defaultScreenFormatter");
  region->setTop (0, false);
  region->setLeft (0, false);
  region->setWidth (w, false);
  region->setHeight (h, false);

}

NclFormatterLayout::~NclFormatterLayout ()
{
}

LayoutRegion *
NclFormatterLayout::getRegion ()
{
  return this->region;
}

SDLWindow*
NclFormatterLayout::prepareFormatterRegion (NclExecutionObject *object)
{
  NclCascadingDescriptor *descriptor;
  NclFormatterRegion *fregion;

  g_assert_nonnull (object);

  descriptor = object->getDescriptor ();
  g_assert_nonnull (descriptor);

  fregion = descriptor->getFormatterRegion ();
  g_assert_nonnull (fregion);

  return fregion->prepareOutputDisplay ();
}

void
NclFormatterLayout::showObject (NclExecutionObject *object)
{
  NclFormatterRegion *region;
  map<int, set<string> *>::iterator i;

  if (object == NULL || object->getDescriptor () == NULL
      || object->getDescriptor ()->getFormatterRegion () == NULL)
    {
      return;
    }

  region = object->getDescriptor ()->getFormatterRegion ();
  region->showContent ();
}

void
NclFormatterLayout::hideObject (NclExecutionObject *object)
{
  NclFormatterRegion *region = NULL;
  LayoutRegion *layoutRegion;
  NclCascadingDescriptor *descriptor;
  string regionId;

  descriptor = object->getDescriptor ();
  if (descriptor != NULL)
    {
      region = object->getDescriptor ()->getFormatterRegion ();
    }

  if (region == NULL)
    {
      if (descriptor != NULL)
        {
          descriptor->setFormatterLayout (this);
        }
      return;
    }

  region->hideContent ();
  layoutRegion = region->getLayoutRegion ();
  regionId = layoutRegion->getId ();

}

GINGA_FORMATTER_END
