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
#include "ginga-color-table.h"

#include "NclPresentationSpecificationParser.h"
#include "NclPresentationControlParser.h"

#include "NclParser.h"

GINGA_PRAGMA_DIAG_IGNORE (-Wsign-conversion)

GINGA_NCLCONV_BEGIN

NclPresentationSpecificationParser::NclPresentationSpecificationParser (
    NclParser *nclParser, DeviceLayout *deviceLayout)
    : ModuleParser (nclParser)
{
  this->deviceLayout = deviceLayout;
}

Descriptor *
NclPresentationSpecificationParser::parseDescriptor (
    DOMElement *descriptor_element)
{
  Descriptor *descriptor = createDescriptor (descriptor_element);
  g_assert_nonnull (descriptor);

  for(DOMElement *child: dom_element_children(descriptor_element))
    {
      string tagname = dom_element_tagname(child);
      if (tagname == "descriptorParam")
        {
          DOMElement *descParam = parseDescriptorParam (child);
          if (descParam)
            {
              string pName = dom_element_get_attr(descParam, "name");
              string pValue = dom_element_get_attr(descParam, "value");

              descriptor->addParameter (new Parameter (pName, pValue));
            }
        }
      else
        {
          syntax_warning( "'%s' is not known as child of 'descriptor'."
                          " It will be ignored.",
                          tagname.c_str() );
        }
    }

  return descriptor;
}

DescriptorBase *
NclPresentationSpecificationParser::parseDescriptorBase (
    DOMElement *descriptorBase_element)
{
  DescriptorBase *descBase = createDescriptorBase (descriptorBase_element);
  g_assert_nonnull (descBase);

  for (DOMElement *child: dom_element_children(descriptorBase_element))
    {
      string tagname = dom_element_tagname(child);
      if (tagname == "importBase")
        {
          DOMElement *elementObject = _nclParser->getImportParser ()
              ->parseImportBase (child);
          if (elementObject)
            {
              addImportBaseToDescriptorBase (descBase, elementObject);
            }
        }
      else if (tagname == "descriptorSwitch")
        {
          DescriptorSwitch *descSwitch =
              _nclParser->getPresentationControlParser ()
              ->parseDescriptorSwitch (child);
          if (descSwitch)
            {
              descBase->addDescriptor (descSwitch);
            }
        }
      else if (tagname == "descriptor")
        {
          Descriptor *desc = parseDescriptor (child);
          if (desc)
            {
              descBase->addDescriptor (desc);
            }
        }
      else
        {
          syntax_warning( "'%s' is not known as child of 'descriptorBase'."
                          " It will be ignored.",
                          tagname.c_str() );
        }
    }

  return descBase;
}

DOMElement *
NclPresentationSpecificationParser::parseDescriptorBind (
    DOMElement *parentElement)
{
  return parentElement;
}

DOMElement *
NclPresentationSpecificationParser::parseDescriptorParam (
    DOMElement *parentElement)
{
  return parentElement;
}

void
NclPresentationSpecificationParser::addImportBaseToDescriptorBase (
    DescriptorBase *descriptorBase, DOMElement *childObject)
{
  string baseAlias, baseLocation;
  NclParser *compiler;
  NclDocument *importedDocument, *thisDocument;
  DescriptorBase *importedDescriptorBase;
  RegionBase *regionBase;

  map<int, RegionBase *> *regionBases;
  map<int, RegionBase *>::iterator i;

  RuleBase *ruleBase;

  // get the external base alias and location
  baseAlias = dom_element_get_attr(childObject, "alias");
  baseLocation = dom_element_get_attr(childObject, "documentURI");

  compiler = getNclParser ();
  importedDocument = compiler->importDocument (baseLocation);
  if (importedDocument == NULL)
    {
      return;
    }

  importedDescriptorBase = importedDocument->getDescriptorBase ();
  if (importedDescriptorBase == NULL)
    {
      return;
    }

  // insert the imported base into the document descriptor base
  try
    {
      descriptorBase->addBase (importedDescriptorBase, baseAlias, baseLocation);
    }
  catch (...)
    {
    }

  // importing descriptor bases implies importing region, rule, and cost
  // function bases in order to maintain reference consistency
  thisDocument = getNclParser ()->getNclDocument ();
  regionBase = thisDocument->getRegionBase (0);
  if (regionBase == NULL)
    {
      regionBase = new RegionBase ("dummy", deviceLayout);
      thisDocument->addRegionBase (regionBase);
    }

  regionBases = importedDocument->getRegionBases ();
  if (regionBases != NULL && !regionBases->empty ())
    {
      i = regionBases->begin ();
      while (i != regionBases->end ())
        {
          regionBase->addBase (i->second, baseAlias, baseLocation);
          ++i;
        }
    }

  ruleBase = importedDocument->getRuleBase ();
  if (ruleBase != NULL)
    {
      try
        {
          thisDocument->getRuleBase ()->addBase (ruleBase, baseAlias, baseLocation);
        }
      catch (...)
        {
        }
    }
}

DescriptorBase *
NclPresentationSpecificationParser::createDescriptorBase (
        DOMElement *descriptorBase_element)
{
  return new DescriptorBase (
        dom_element_get_attr(descriptorBase_element ,"id") );
}

Descriptor *
NclPresentationSpecificationParser::createDescriptor (
    DOMElement *descriptor_element)
{
  Descriptor *descriptor;
  NclDocument *document;
  LayoutRegion *region;
  KeyNavigation *keyNavigation;
  string src;
  FocusDecoration *focusDecoration;
  SDL_Color *color;
  string attValue;
  vector<string> *transIds;
  unsigned int i;
  TransitionBase *transitionBase;
  Transition *transition;

  // cria descritor
  descriptor = new Descriptor (dom_element_get_attr(descriptor_element, "id"));

  document = getNclParser ()->getNclDocument ();

  // region
  if (dom_element_has_attr(descriptor_element, "region"))
    {
      region = document->getRegion (
            dom_element_get_attr(descriptor_element, "region") );

      if (region)
        {
          descriptor->setRegion (region);
        }
    }

  // explicitDur
  if (dom_element_has_attr(descriptor_element, "explicitDur"))
    {
      string durStr =
          dom_element_get_attr(descriptor_element, "explicitDur");

      descriptor->setExplicitDuration (::ginga::util::strUTCToSec (durStr)
                                       * 1000);
    }

  if (dom_element_has_attr(descriptor_element,"freeze"))
    {
      string freeze = dom_element_get_attr(descriptor_element,"freeze");

      if (freeze == "true")
        {
          descriptor->setFreeze (true);
        }
      else
        {
          descriptor->setFreeze (false);
        }
    }

  // atributo player
  if (dom_element_has_attr(descriptor_element, "player"))
    {
      descriptor->setPlayerName (
            dom_element_get_attr(descriptor_element, "player") );
    }

  // key navigation attributes
  keyNavigation = new KeyNavigation ();
  descriptor->setKeyNavigation (keyNavigation);
  if (dom_element_has_attr(descriptor_element, "focusIndex"))
    {
      keyNavigation->setFocusIndex (
          dom_element_get_attr(descriptor_element,"focusIndex"));
    }

  if (dom_element_has_attr(descriptor_element, "moveUp"))
    {
      keyNavigation->setMoveUp (
            dom_element_get_attr(descriptor_element,"moveUp") );
    }

  if (dom_element_has_attr(descriptor_element, "moveDown"))
    {
      keyNavigation->setMoveDown (
            dom_element_get_attr(descriptor_element,"moveDown") );
    }

  if (dom_element_has_attr(descriptor_element, "moveLeft"))
    {
      keyNavigation->setMoveLeft (
            dom_element_get_attr(descriptor_element, "moveLeft") );
    }

  if (dom_element_has_attr(descriptor_element, "moveRight"))
    {
      keyNavigation->setMoveRight (
          dom_element_get_attr(descriptor_element, "moveRight") );
    }

  focusDecoration = new FocusDecoration ();
  descriptor->setFocusDecoration (focusDecoration);
  if (dom_element_has_attr (descriptor_element, "focusSrc"))
    {
      src = dom_element_get_attr (descriptor_element, "focusSrc");

      if (!xpathisuri (src) && !xpathisabs (src))
        src = xpathbuildabs (getNclParser ()->getDirName (), src);

      focusDecoration->setFocusSrc (src);
    }

  if (dom_element_has_attr (descriptor_element, "focusBorderColor"))
    {
      color = new SDL_Color ();
      ginga_color_input_to_sdl_color(
            dom_element_get_attr (descriptor_element, "focusBorderColor"),
            color);

      focusDecoration->setFocusBorderColor ( color );
    }

  if (dom_element_has_attr (descriptor_element, "focusBorderWidth"))
    {
      int w;
      w = xstrto_int (
          dom_element_get_attr (descriptor_element, "focusBorderWidth") );
      focusDecoration->setFocusBorderWidth (w);
    }

  if (dom_element_has_attr (descriptor_element, "focusBorderTransparency"))
    {
      double alpha;
      alpha = xstrtod (
          dom_element_get_attr (descriptor_element,
                                         "focusBorderTransparency"));

      focusDecoration->setFocusBorderTransparency (alpha);
    }

  if (dom_element_has_attr (descriptor_element, "focusSelSrc"))
    {
      src = dom_element_get_attr (descriptor_element, "focusSelSrc");

      if (!xpathisuri (src) && !xpathisabs (src))
        src = xpathbuildabs (getNclParser ()->getDirName (), src);

      focusDecoration->setFocusSelSrc (src);
    }

  if (dom_element_has_attr (descriptor_element, "selBorderColor"))
    {
      color = new SDL_Color ();
      ginga_color_input_to_sdl_color (
            dom_element_get_attr (descriptor_element, "selBorderColor"),
            color );
      focusDecoration->setSelBorderColor ( color );
    }

  if (dom_element_has_attr (descriptor_element, "transIn"))
    {
      transitionBase = document->getTransitionBase ();
      if (transitionBase != NULL)
        {
          string trimValue, value;
          attValue = dom_element_get_attr (descriptor_element, "transIn");

          transIds = split (attValue, ";");
          if (!transIds->empty ())
            {
              vector<string>::iterator it;
              it = transIds->begin ();
              int j = 0;
              while (it != transIds->end ())
                {
                  value = (*it);
                  trimValue = xstrchomp (value);
                  *it = trimValue;

                  transition = transitionBase->getTransition (trimValue);
                  if (transition != NULL)
                    {
                      descriptor->addInputTransition (transition, j);
                    }
                  ++it;
                  j++;
                }
            }

          delete transIds;
          transIds = NULL;
        }
    }

  if (dom_element_has_attr (descriptor_element, "transOut"))
    {
      transitionBase = document->getTransitionBase ();
      if (transitionBase != NULL)
        {
          string trimValue, value;
          attValue = dom_element_get_attr(descriptor_element, "transOut");

          transIds = split (attValue, ";");
          if (!transIds->empty ())
            {
              for (i = 0; i < transIds->size (); i++)
                {
                  value = (*transIds)[i];
                  trimValue = xstrchomp (value);
                  (*transIds)[i] = trimValue;
                  transition = transitionBase->getTransition (trimValue);
                  if (transition != NULL)
                    {
                      descriptor->addOutputTransition (transition, i);
                    }
                }
            }

          delete transIds;
          transIds = NULL;
        }
    }

  return descriptor;
}


GINGA_NCLCONV_END
