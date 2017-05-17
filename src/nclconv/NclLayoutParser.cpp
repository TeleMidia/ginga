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
#include "NclLayoutParser.h"

#include "NclParser.h"

GINGA_PRAGMA_DIAG_IGNORE (-Wsign-conversion)

GINGA_NCLCONV_BEGIN

#define CHECK_REGION_ATTR_AND_SET(DOMELEMENT,DOMATRR,REGIONATTRFUNC) \
  if (dom_element_has_attr(DOMELEMENT, DOMATRR)) \
    { \
      string attr = dom_element_get_attr(DOMELEMENT, DOMATRR); \
      if (xstrispercent (attr)) \
        { \
          REGIONATTRFUNC (xstrtodorpercent (attr) * 100., true); \
        } \
      else \
        { \
          REGIONATTRFUNC (xstrto_int (attr), false); \
        } \
    }

NclLayoutParser::NclLayoutParser (NclParser *nclParser,
                                  DeviceLayout *deviceLayout)
    : ModuleParser (nclParser)
{
  this->deviceLayout = deviceLayout;
}

LayoutRegion *
NclLayoutParser::parseRegion (DOMElement *region_element)
{
  LayoutRegion *region = createRegion (region_element);
  g_assert_nonnull (region);

  for (DOMElement *child:
       dom_element_children_by_tagname(region_element, "region"))
    {
      LayoutRegion *child_region = parseRegion (child);
      if (child_region)
        {
          region->addRegion (child_region);
        }
    }

  return region;
}

RegionBase *
NclLayoutParser::parseRegionBase (DOMElement *regionBase_element)
{
  RegionBase *regionBase = createRegionBase (regionBase_element);
  g_assert_nonnull (regionBase);

  for (DOMElement *child:
       dom_element_children(regionBase_element) )
    {
      string tagname = dom_element_tagname(child);
      if (tagname == "importBase")
        {
          DOMElement *importBase_element
              = _nclParser->getImportParser ()->parseImportBase (child);
          if (importBase_element)
            {
              addImportBaseToRegionBase (regionBase, importBase_element);
            }
        }
      else if (tagname == "region")
        {
          LayoutRegion *region = parseRegion (child);
          if (region)
            {
              regionBase->addRegion (region);
            }
        }
      else
        {
          syntax_warning( "'%s' is not known as child of 'regionBase'."
                          " It will be ignored.",
                          tagname.c_str() );
        }
    }

  return regionBase;
}

void
NclLayoutParser::addImportBaseToRegionBase (RegionBase *regionBase,
                                            DOMElement *importBase_element)
{
  map<int, RegionBase *> *bases;
  map<int, RegionBase *>::iterator i;
  string baseAlias, baseLocation;
  NclDocument *importedDocument;

  // get the external base alias and location
  baseAlias = dom_element_get_attr(importBase_element, "alias");
  baseLocation = dom_element_get_attr(importBase_element, "documentURI");

  importedDocument = getNclParser ()->importDocument (baseLocation);
  if (importedDocument == NULL)
    {
      return;
    }

  bases = importedDocument->getRegionBases ();
  if (bases == NULL || bases->empty ())
    {
      return;
    }

  // insert the imported base into the document region base
  i = bases->begin ();
  while (i != bases->end ())
    {
      regionBase->addBase (i->second, baseAlias, baseLocation);
      ++i;
    }
}

RegionBase *
NclLayoutParser::createRegionBase (DOMElement *parentElement)
{
  RegionBase *regionBase;

  regionBase = new RegionBase (dom_element_get_attr(parentElement, "id"),
                           deviceLayout);

  // device attribute
  if (dom_element_has_attr(parentElement, "device"))
    {
      string mapId = "";
      // region for output bit map attribute
      if (dom_element_has_attr(parentElement, "region"))
        {
          mapId = dom_element_get_attr(parentElement, "region");
        }

      regionBase->setDevice (
            dom_element_get_attr(parentElement, "device"), mapId);
    }
  else
    {
      regionBase->setDevice (deviceLayout->getLayoutName (), "");
    }

  return regionBase;
}

LayoutRegion *
NclLayoutParser::createRegion (DOMElement *region_element)
{
  string attr = dom_element_get_attr(region_element, "id");
  LayoutRegion *ncmRegion = new LayoutRegion (attr);

  if(dom_element_try_get_attr(attr, region_element, "title"))
    ncmRegion->setTitle(attr);

  CHECK_REGION_ATTR_AND_SET (region_element, "left", ncmRegion->setLeft)
  CHECK_REGION_ATTR_AND_SET (region_element, "right", ncmRegion->setRight)
  CHECK_REGION_ATTR_AND_SET (region_element, "top", ncmRegion->setTop)
  CHECK_REGION_ATTR_AND_SET (region_element, "bottom", ncmRegion->setBottom)
  CHECK_REGION_ATTR_AND_SET (region_element, "width", ncmRegion->setWidth)
  CHECK_REGION_ATTR_AND_SET (region_element, "height", ncmRegion->setHeight)

  if(dom_element_try_get_attr(attr, region_element, "zIndex"))
      ncmRegion->setZIndex(xstrto_int(attr));

  return ncmRegion;
}

GINGA_NCLCONV_END
