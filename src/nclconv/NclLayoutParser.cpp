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

#include "NclDocumentParser.h"

GINGA_PRAGMA_DIAG_IGNORE (-Wsign-conversion)

GINGA_NCLCONV_BEGIN

NclLayoutParser::NclLayoutParser (NclDocumentParser *documentParser,
                                  DeviceLayout *deviceLayout)
    : ModuleParser (documentParser)
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
          string tagname = _documentParser->getTagname(element);

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
          string tagname = _documentParser->getTagname(element);

          if (XMLString::compareIString (tagname.c_str (), "importBase") == 0)
            {
              DOMElement *elementObject =
                  _documentParser->getImportParser ()->
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
  NclDocumentParser *compiler;
  NclDocument *importedDocument;

  // get the external base alias and location
  baseAlias = _documentParser->getAttribute((DOMElement *)childObject, "alias");

  baseLocation = _documentParser->getAttribute((DOMElement *)childObject,
                                               "documentURI");

  compiler = getDocumentParser ();
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

  regionBase = new RegionBase (_documentParser->getAttribute(parentElement, "id"),
                           deviceLayout);

  // device attribute
  if (_documentParser->hasAttribute(parentElement, "device"))
    {
      string mapId = "";
      // region for output bit map attribute
      if (_documentParser->hasAttribute(parentElement, "region"))
        {
          mapId = _documentParser->getAttribute(parentElement, "region");
        }

      regionBase->setDevice (
            _documentParser->getAttribute(parentElement, "device"), mapId);
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
  string attribute = _documentParser->getAttribute(parentElement, "id");
  LayoutRegion *ncmRegion = new LayoutRegion (attribute);

  // title
  if (_documentParser->hasAttribute(parentElement, "title"))
    {
      ncmRegion->setTitle (
            _documentParser->getAttribute(parentElement, "title"));
    }

  // left
  if (_documentParser->hasAttribute(parentElement, "left"))
    {
      attribute = _documentParser->getAttribute(parentElement, "left");

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
  if (_documentParser->hasAttribute(parentElement, "right"))
    {
      attribute = _documentParser->getAttribute(parentElement, "right");

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
  if (_documentParser->hasAttribute(parentElement, "top"))
    {
      attribute = _documentParser->getAttribute(parentElement, "top");

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
  if (_documentParser->hasAttribute(parentElement, "bottom"))
    {
      attribute = _documentParser->getAttribute(parentElement, "bottom");

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
  if (_documentParser->hasAttribute(parentElement, "width"))
    {
      attribute = _documentParser->getAttribute(parentElement,"width");

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
  if (_documentParser->hasAttribute(parentElement, "height"))
    {
      attribute = _documentParser->getAttribute(parentElement, "height");

      if (xstrispercent (attribute))
        {
          ncmRegion->setHeight (getPercentValue (attribute), true);
        }
      else
        {
          ncmRegion->setHeight (getPixelValue (attribute), false);
        }
    }

  if (_documentParser->hasAttribute(parentElement, "zIndex"))
    {
      attribute = _documentParser->getAttribute(parentElement, "zIndex");

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
