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
NclPresentationSpecificationParser::parseDescriptor (DOMElement *parentElement)
{
  Descriptor *descriptor = createDescriptor (parentElement);
  g_assert_nonnull (descriptor);

  DOMNodeList *elementNodeList = parentElement->getChildNodes ();
  for (int i = 0; i < (int)elementNodeList->getLength (); i++)
    {
      DOMNode *node = elementNodeList->item (i);
      if (node->getNodeType () == DOMNode::ELEMENT_NODE)
        {
          DOMElement *element = (DOMElement *)node;
          string tagname = _nclParser->getTagname(element);
          if (XMLString::compareIString (tagname.c_str (), "descriptorParam")
              == 0)
            {
              DOMElement *elementObject = parseDescriptorParam (element);

              if (elementObject != NULL)
                {
                  addDescriptorParamToDescriptor (descriptor, elementObject);
                }
            }
        }
    }

  return descriptor;
}

DescriptorBase *
NclPresentationSpecificationParser::parseDescriptorBase (
    DOMElement *parentElement)
{
  DOMElement *element;
  void *elementObject;

  DescriptorBase *descBase = createDescriptorBase (parentElement);
  g_assert_nonnull (descBase);

  DOMNodeList *elementNodeList = parentElement->getChildNodes ();
  for (int i = 0; i < (int)elementNodeList->getLength (); i++)
    {
      DOMNode *node = elementNodeList->item (i);
      if (node->getNodeType () == DOMNode::ELEMENT_NODE)
        {
          element = (DOMElement *)node;
          string tagname = _nclParser->getTagname(element);

          if (XMLString::compareIString (tagname.c_str (), "importBase") == 0)
            {
              elementObject = _nclParser->getImportParser ()
                      ->parseImportBase (element);

              if (elementObject != NULL)
                {
                  addImportBaseToDescriptorBase (descBase,
                                                 elementObject);
                }
            }
          else if (XMLString::compareIString (tagname.c_str (),
                                              "descriptorSwitch")
                   == 0)
            {
              DescriptorSwitch *descSwitch
                  = _nclParser->getPresentationControlParser ()
                      ->parseDescriptorSwitch (element);

              if (descSwitch)
                {
                  addDescriptorSwitchToDescriptorBase (descBase, descSwitch);
                }
            }
          else if (XMLString::compareIString (tagname.c_str (), "descriptor")
                   == 0)
            {
              Descriptor *desc = parseDescriptor (element);
              if (desc)
                {
                  addDescriptorToDescriptorBase (descBase, desc);
                }
            }
        }
    }

  return descBase;
}

void *
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
NclPresentationSpecificationParser::addDescriptorToDescriptorBase (
    DescriptorBase *descBase, GenericDescriptor *desc)
{
  descBase->addDescriptor (desc);
}

void
NclPresentationSpecificationParser::addDescriptorSwitchToDescriptorBase (
    DescriptorBase *descBase, GenericDescriptor *desc)
{
  descBase->addDescriptor (desc);
}

void
NclPresentationSpecificationParser::addDescriptorParamToDescriptor (
    Descriptor *descriptor, DOMElement *param)
{
  // recuperar nome e valor da variavel
  string paramName = _nclParser->getAttribute(param, "name");
  string paramValue = _nclParser->getAttribute(param, "value");

  // adicionar variavel ao descritor
  Parameter *descParam = new Parameter (paramName, paramValue);
  descriptor->addParameter (descParam);
}

void
NclPresentationSpecificationParser::addImportBaseToDescriptorBase (
    void *parentObject, void *childObject)
{
  string baseAlias, baseLocation;
  NclParser *compiler;
  NclDocument *importedDocument, *thisDocument;
  DescriptorBase *descriptorBase;
  RegionBase *regionBase;

  map<int, RegionBase *> *regionBases;
  map<int, RegionBase *>::iterator i;

  RuleBase *ruleBase;

  // get the external base alias and location
  baseAlias = _nclParser->getAttribute((DOMElement *)childObject, "alias");

  baseLocation = _nclParser->getAttribute((DOMElement *)childObject,
                                               "documentURI");

  compiler = getNclParser ();
  importedDocument = compiler->importDocument (baseLocation);
  if (importedDocument == NULL)
    {
      return;
    }

  descriptorBase = importedDocument->getDescriptorBase ();
  if (descriptorBase == NULL)
    {
      return;
    }

  // insert the imported base into the document descriptor base
  try
    {
      ((DescriptorBase *)parentObject)
          ->addBase (descriptorBase, baseAlias, baseLocation);
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
          thisDocument->getRuleBase ()->addBase (ruleBase, baseAlias,
                                                 baseLocation);
        }
      catch (...)
        {
        }
    }
}

DescriptorBase *
NclPresentationSpecificationParser::createDescriptorBase (
        DOMElement *parentElement)
{
  DescriptorBase *descBase;

  // criar nova base de conectores com id gerado a partir do nome de seu
  // elemento
  descBase = new DescriptorBase (
        _nclParser->getAttribute(parentElement ,"id") );

  return descBase;
}

Descriptor *
NclPresentationSpecificationParser::createDescriptor (DOMElement *parentElement)
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
  descriptor = new Descriptor (
        _nclParser->getAttribute(parentElement, "id"));

  document = getNclParser ()->getNclDocument ();

  // region
  if (_nclParser->hasAttribute(parentElement, "region"))
    {
      region = document->getRegion (
            _nclParser->getAttribute(parentElement, "region") );

      if (region)
        {
          descriptor->setRegion (region);
        }
    }

  // explicitDur
  if (_nclParser->hasAttribute(parentElement, "explicitDur"))
    {
      string durStr =
          _nclParser->getAttribute(parentElement, "explicitDur");

      descriptor->setExplicitDuration (::ginga::util::strUTCToSec (durStr)
                                       * 1000);
    }

  if (_nclParser->hasAttribute(parentElement,"freeze"))
    {
      string freeze = _nclParser->getAttribute(parentElement,"freeze");

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
  if (_nclParser->hasAttribute(parentElement, "player"))
    {
      descriptor->setPlayerName (
            _nclParser->getAttribute(parentElement, "player") );
    }

  // key navigation attributes
  keyNavigation = new KeyNavigation ();
  descriptor->setKeyNavigation (keyNavigation);
  if (_nclParser->hasAttribute(parentElement, "focusIndex"))
    {
      keyNavigation->setFocusIndex (
          _nclParser->getAttribute(parentElement,"focusIndex"));
    }

  if (_nclParser->hasAttribute(parentElement, "moveUp"))
    {
      keyNavigation->setMoveUp (
            _nclParser->getAttribute(parentElement,"moveUp") );
    }

  if (_nclParser->hasAttribute(parentElement, "moveDown"))
    {
      keyNavigation->setMoveDown (
            _nclParser->getAttribute(parentElement,"moveDown") );
    }

  if (_nclParser->hasAttribute(parentElement, "moveLeft"))
    {
      keyNavigation->setMoveLeft (
            _nclParser->getAttribute(parentElement, "moveLeft") );
    }

  if (_nclParser->hasAttribute(parentElement, "moveRight"))
    {
      keyNavigation->setMoveRight (
          _nclParser->getAttribute(parentElement, "moveRight") );
    }

  focusDecoration = new FocusDecoration ();
  descriptor->setFocusDecoration (focusDecoration);
  if (_nclParser->hasAttribute (parentElement, "focusSrc"))
    {
      src = _nclParser->getAttribute (parentElement, "focusSrc");

      if (!xpathisuri (src) && !xpathisabs (src))
        src = xpathbuildabs (getNclParser ()->getDirName (), src);

      focusDecoration->setFocusSrc (src);
    }

  if (_nclParser->hasAttribute (parentElement, "focusBorderColor"))
    {
      color = new SDL_Color ();
      ginga_color_input_to_sdl_color(
            _nclParser->getAttribute (parentElement, "focusBorderColor"),
            color);

      focusDecoration->setFocusBorderColor ( color );
    }

  if (_nclParser->hasAttribute (parentElement, "focusBorderWidth"))
    {
      int w;
      w = xstrto_int (
          _nclParser->getAttribute (parentElement, "focusBorderWidth") );
      focusDecoration->setFocusBorderWidth (w);
    }

  if (_nclParser->hasAttribute (parentElement, "focusBorderTransparency"))
    {
      double alpha;
      alpha = xstrtod (
          _nclParser->getAttribute (parentElement,
                                         "focusBorderTransparency"));

      focusDecoration->setFocusBorderTransparency (alpha);
    }

  if (_nclParser->hasAttribute (parentElement, "focusSelSrc"))
    {
      src = _nclParser->getAttribute (parentElement, "focusSelSrc");

      if (!xpathisuri (src) && !xpathisabs (src))
        src = xpathbuildabs (getNclParser ()->getDirName (), src);

      focusDecoration->setFocusSelSrc (src);
    }

  if (_nclParser->hasAttribute (parentElement, "selBorderColor"))
    {
      color = new SDL_Color ();
      ginga_color_input_to_sdl_color (
            _nclParser->getAttribute (parentElement, "selBorderColor"),
            color );
      focusDecoration->setSelBorderColor ( color );
    }

  if (_nclParser->hasAttribute (parentElement, "transIn"))
    {
      transitionBase = document->getTransitionBase ();
      if (transitionBase != NULL)
        {
          string trimValue, value;
          attValue = _nclParser->getAttribute (parentElement, "transIn");

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

  if (_nclParser->hasAttribute (parentElement, "transOut"))
    {
      transitionBase = document->getTransitionBase ();
      if (transitionBase != NULL)
        {
          string trimValue, value;
          attValue = _nclParser->getAttribute(parentElement, "transOut");

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
