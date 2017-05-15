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
#include "NclLayoutConverter.h"

GINGA_NCLCONV_BEGIN

NclLayoutConverter::NclLayoutConverter (NclDocumentParser *documentParser,
                                        DeviceLayout *deviceLayout)
    : NclLayoutParser (documentParser)
{
  this->deviceLayout = deviceLayout;
}

void
NclLayoutConverter::addImportBaseToRegionBase (void *parentObject,
                                               void *childObject)
{
  map<int, RegionBase *> *bases;
  map<int, RegionBase *>::iterator i;
  string baseAlias, baseLocation;
  NclDocumentConverter *compiler;
  NclDocument *importedDocument;

  // get the external base alias and location
  baseAlias = XMLString::transcode (
      ((DOMElement *)childObject)
          ->getAttribute (XMLString::transcode ("alias")));

  baseLocation = XMLString::transcode (
      ((DOMElement *)childObject)
          ->getAttribute (XMLString::transcode ("documentURI")));

  compiler = (NclDocumentConverter *)getDocumentParser ();
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
      ((RegionBase *)parentObject)
          ->addBase (i->second, baseAlias, baseLocation);
      ++i;
    }
}

void
NclLayoutConverter::addRegionToRegion (void *parentObject,
                                       void *childObject)
{
  ((LayoutRegion *)parentObject)->addRegion ((LayoutRegion *)childObject);
}

void
NclLayoutConverter::addRegionToRegionBase (void *parentObject,
                                           void *childObject)
{
  RegionBase *layout = (RegionBase *)parentObject;
  layout->addRegion ((LayoutRegion *)childObject);
}

void *
NclLayoutConverter::createRegionBase (DOMElement *parentElement,
                                      arg_unused (void *objGrandParent))
{
  RegionBase *layout;
  string mapId = "";

  layout
      = new RegionBase (XMLString::transcode (parentElement->getAttribute (
                            XMLString::transcode ("id"))),

                        deviceLayout);

  // device attribute
  if (parentElement->hasAttribute (XMLString::transcode ("device")))
    {
      // region for output bit map attribute
      if (parentElement->hasAttribute (XMLString::transcode ("region")))
        {
          mapId = XMLString::transcode (parentElement->getAttribute (
              XMLString::transcode ("region")));
        }

      layout->setDevice (XMLString::transcode (parentElement->getAttribute (
                             XMLString::transcode ("device"))),
                         mapId);
    }
  else
    {
      layout->setDevice (deviceLayout->getLayoutName (), "");
    }

  return layout;
}

void *
NclLayoutConverter::createRegion (DOMElement *parentElement,
                                  arg_unused (void *objGrandParent))
{
  LayoutRegion *ncmRegion;
  string attribute;
  double percentValue;

  attribute = XMLString::transcode (
      parentElement->getAttribute (XMLString::transcode ("id")));

  // cria nova regiao
  ncmRegion = new LayoutRegion (attribute);

  // atributo title
  if (parentElement->hasAttribute (XMLString::transcode ("title")))
    {
      ncmRegion->setTitle (XMLString::transcode (
          parentElement->getAttribute (XMLString::transcode ("title"))));
    }

  // atributo: left
  if (parentElement->hasAttribute (XMLString::transcode ("left")))
    {
      attribute = XMLString::transcode (
          parentElement->getAttribute (XMLString::transcode ("left")));

      if (attribute != "")
        {
          if (xstrispercent (attribute))
            {
              percentValue = getPercentValue (attribute);
              ncmRegion->setLeft (percentValue, true);
            }
          else
            {
              double pixelValue;
              pixelValue = getPixelValue (attribute);
              ncmRegion->setLeft (pixelValue, false);
            }
        }
    }

  // atributo: right
  if (parentElement->hasAttribute (XMLString::transcode ("right")))
    {
      attribute = XMLString::transcode (
          parentElement->getAttribute (XMLString::transcode ("right")));

      if (xstrispercent (attribute))
        {
          percentValue = getPercentValue (attribute);
          ncmRegion->setRight (percentValue, true);
        }
      else
        {
          ncmRegion->setRight (getPixelValue (attribute), false);
        }
    }

  // atributo: top
  if (parentElement->hasAttribute (XMLString::transcode ("top")))
    {
      attribute = XMLString::transcode (
          parentElement->getAttribute (XMLString::transcode ("top")));

      if (xstrispercent (attribute))
        {
          percentValue = getPercentValue (attribute);
          ncmRegion->setTop (percentValue, true);
        }
      else
        {
          ncmRegion->setTop (getPixelValue (attribute), false);
        }
    }

  // atributo: bottom
  if (parentElement->hasAttribute (XMLString::transcode ("bottom")))
    {
      attribute = XMLString::transcode (
          parentElement->getAttribute (XMLString::transcode ("bottom")));

      if (xstrispercent (attribute))
        {
          percentValue = getPercentValue (attribute);
          ncmRegion->setBottom (percentValue, true);
        }
      else
        {
          ncmRegion->setBottom (getPixelValue (attribute), false);
        }
    }

  // atributo: width
  if (parentElement->hasAttribute (XMLString::transcode ("width")))
    {
      attribute = XMLString::transcode (
          parentElement->getAttribute (XMLString::transcode ("width")));

      if (xstrispercent (attribute))
        {
          percentValue = getPercentValue (attribute);
          ncmRegion->setWidth (percentValue, true);
        }
      else
        {
          ncmRegion->setWidth (getPixelValue (attribute), false);
        }
    }

  // atributo: height
  if (parentElement->hasAttribute (XMLString::transcode ("height")))
    {
      attribute = XMLString::transcode (
          parentElement->getAttribute (XMLString::transcode ("height")));

      if (xstrispercent (attribute))
        {
          percentValue = getPercentValue (attribute);
          ncmRegion->setHeight (percentValue, true);
        }
      else
        {
          ncmRegion->setHeight (getPixelValue (attribute), false);
        }
    }

  if (parentElement->hasAttribute (XMLString::transcode ("zIndex")))
    {
      attribute = XMLString::transcode (
          parentElement->getAttribute (XMLString::transcode ("zIndex")));

      ncmRegion->setZIndex (xstrto_int (attribute));
    }

  return ncmRegion;
}

double
NclLayoutConverter::getPercentValue (const string &value)
{
  return xstrtodorpercent (value) * 100.;
}

int
NclLayoutConverter::getPixelValue (const string &value)
{
  return xstrto_int (value);
}

GINGA_NCLCONV_END
