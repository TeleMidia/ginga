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

NclLayoutParser::NclLayoutParser (NclParser *nclParser,
                                  DeviceLayout *deviceLayout)
    : ModuleParser (nclParser)
{
  this->deviceLayout = deviceLayout;
}

LayoutRegion *
NclLayoutParser::parseRegion (DOMElement *parentElement)
{
  LayoutRegion *region = createRegion (parentElement);
  g_assert_nonnull (region);

  DOMNodeList *elementNodeList = parentElement->getChildNodes ();
  for (int i = 0; i < (int)elementNodeList->getLength (); i++)
    {
      DOMNode *node = elementNodeList->item (i);
      if (node->getNodeType () == DOMNode::ELEMENT_NODE)
        {
          DOMElement *element = (DOMElement *)node;
          string tagname = dom_element_tagname(element);

          if (XMLString::compareIString (tagname.c_str (), "region")
              == 0)
            {
              LayoutRegion *child_region = parseRegion (element);
              if (child_region)
                {
                  addRegionToRegion (region, child_region);
                }
            }
        }
    }

  return region;
}

RegionBase *
NclLayoutParser::parseRegionBase (DOMElement *parentElement)
{
  RegionBase *regionBase = createRegionBase (parentElement);
  g_assert_nonnull (regionBase);

  DOMNodeList *elementNodeList = parentElement->getChildNodes ();
  for (int i = 0; i < (int)elementNodeList->getLength (); i++)
    {
      DOMNode *node = elementNodeList->item (i);
      if (node->getNodeType () == DOMNode::ELEMENT_NODE)
        {
          DOMElement *element = (DOMElement *)node;
          string tagname = dom_element_tagname(element);

          if (XMLString::compareIString (tagname.c_str (), "importBase") == 0)
            {
              DOMElement *elementObject =
                  _nclParser->getImportParser ()->
                    parseImportBase (element);

              if (elementObject)
                {
                  addImportBaseToRegionBase (regionBase, elementObject);
                }
            }
          else if (XMLString::compareIString (tagname.c_str (), "region") == 0)
            {
              LayoutRegion *region = parseRegion (element);
              if (region)
                {
                  addRegionToRegionBase (regionBase, region);
                }
            }
        }
    }

  return regionBase;
}

void
NclLayoutParser::addImportBaseToRegionBase (RegionBase *regionBase,
                                            DOMElement *childObject)
{
  map<int, RegionBase *> *bases;
  map<int, RegionBase *>::iterator i;
  string baseAlias, baseLocation;
  NclParser *compiler;
  NclDocument *importedDocument;

  // get the external base alias and location
  baseAlias = dom_element_get_attr((DOMElement *)childObject, "alias");

  baseLocation = dom_element_get_attr((DOMElement *)childObject,
                                               "documentURI");

  compiler = getNclParser ();
  importedDocument = compiler->importDocument (baseLocation);
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

void
NclLayoutParser::addRegionToRegion (LayoutRegion *parentRegion,
                                    LayoutRegion *childRegion)
{
  parentRegion->addRegion (childRegion);
}

void
NclLayoutParser::addRegionToRegionBase (RegionBase *parentRegion,
                                        LayoutRegion *childRegion)
{
  parentRegion->addRegion (childRegion);
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
NclLayoutParser::createRegion (DOMElement *parentElement)
{
  string attribute = dom_element_get_attr(parentElement, "id");
  LayoutRegion *ncmRegion = new LayoutRegion (attribute);

  // title
  if (dom_element_has_attr(parentElement, "title"))
    {
      ncmRegion->setTitle (
            dom_element_get_attr(parentElement, "title"));
    }

  // left
  if (dom_element_has_attr(parentElement, "left"))
    {
      attribute = dom_element_get_attr(parentElement, "left");

      if (attribute != "")
        {
          if (xstrispercent (attribute))
            {
              ncmRegion->setLeft (getPercentValue (attribute), true);
            }
          else
            {
              ncmRegion->setLeft (getPixelValue (attribute), false);
            }
        }
    }

  // right
  if (dom_element_has_attr(parentElement, "right"))
    {
      attribute = dom_element_get_attr(parentElement, "right");

      if (xstrispercent (attribute))
        {
          ncmRegion->setRight (getPercentValue (attribute), true);
        }
      else
        {
          ncmRegion->setRight (getPixelValue (attribute), false);
        }
    }

  // top
  if (dom_element_has_attr(parentElement, "top"))
    {
      attribute = dom_element_get_attr(parentElement, "top");

      if (xstrispercent (attribute))
        {
          ncmRegion->setTop (getPercentValue (attribute), true);
        }
      else
        {
          ncmRegion->setTop (getPixelValue (attribute), false);
        }
    }

  // bottom
  if (dom_element_has_attr(parentElement, "bottom"))
    {
      attribute = dom_element_get_attr(parentElement, "bottom");

      if (xstrispercent (attribute))
        {
          ncmRegion->setBottom (getPercentValue (attribute), true);
        }
      else
        {
          ncmRegion->setBottom (getPixelValue (attribute), false);
        }
    }

  // width
  if (dom_element_has_attr(parentElement, "width"))
    {
      attribute = dom_element_get_attr(parentElement,"width");

      if (xstrispercent (attribute))
        {
          ncmRegion->setWidth (getPercentValue (attribute), true);
        }
      else
        {
          ncmRegion->setWidth (getPixelValue (attribute), false);
        }
    }

  // height
  if (dom_element_has_attr(parentElement, "height"))
    {
      attribute = dom_element_get_attr(parentElement, "height");

      if (xstrispercent (attribute))
        {
          ncmRegion->setHeight (getPercentValue (attribute), true);
        }
      else
        {
          ncmRegion->setHeight (getPixelValue (attribute), false);
        }
    }

  if (dom_element_has_attr(parentElement, "zIndex"))
    {
      attribute = dom_element_get_attr(parentElement, "zIndex");

      ncmRegion->setZIndex (xstrto_int (attribute));
    }

  return ncmRegion;
}

double
NclLayoutParser::getPercentValue (const string &value)
{
  return xstrtodorpercent (value) * 100.;
}

int
NclLayoutParser::getPixelValue (const string &value)
{
  return xstrto_int (value);
}

GINGA_NCLCONV_END
