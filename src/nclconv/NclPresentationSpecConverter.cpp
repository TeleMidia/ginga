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
#include "NclPresentationSpecConverter.h"
#include "NclImportConverter.h"
#include "NclPresentationControlConverter.h"

GINGA_PRAGMA_DIAG_IGNORE (-Wsign-conversion)

GINGA_NCLCONV_BEGIN

NclPresentationSpecConverter::NclPresentationSpecConverter (
    DocumentParser *documentParser, DeviceLayout *deviceLayout)
    : NclPresentationSpecificationParser (documentParser)
{
  this->deviceLayout = deviceLayout;
}

void
NclPresentationSpecConverter::addDescriptorToDescriptorBase (
    void *parentObject, void *childObject)
{
  ((DescriptorBase *)parentObject)
      ->addDescriptor ((GenericDescriptor *)childObject);
}

void
NclPresentationSpecConverter::addDescriptorSwitchToDescriptorBase (
    void *parentObject, void *childObject)
{
  ((DescriptorBase *)parentObject)
      ->addDescriptor ((GenericDescriptor *)childObject);
}

void
NclPresentationSpecConverter::addDescriptorParamToDescriptor (
    void *parentObject, void *childObject)
{
  // cast para descritor
  Descriptor *descriptor = (Descriptor *)parentObject;
  DOMElement *param = (DOMElement *)childObject;
  // recuperar nome e valor da variavel
  string paramName = XMLString::transcode (
      param->getAttribute (XMLString::transcode ("name")));

  string paramValue = XMLString::transcode (
      param->getAttribute (XMLString::transcode ("value")));

  // adicionar variavel ao descritor
  Parameter *descParam = new Parameter (paramName, paramValue);
  descriptor->addParameter (descParam);
}

void
NclPresentationSpecConverter::addImportBaseToDescriptorBase (
    void *parentObject, void *childObject)
{
  string baseAlias, baseLocation;
  NclDocumentConverter *compiler;
  NclDocument *importedDocument, *thisDocument;
  DescriptorBase *descriptorBase;
  RegionBase *regionBase;

  map<int, RegionBase *> *regionBases;
  map<int, RegionBase *>::iterator i;

  RuleBase *ruleBase;

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
  thisDocument = (NclDocument *)getDocumentParser ()->getObject (
      "return", "document");

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

void *
NclPresentationSpecConverter::createDescriptorParam (
    DOMElement *parentElement, arg_unused (void *objGrandParent))
{
  return parentElement;
}

void *
NclPresentationSpecConverter::createDescriptorBind (
    DOMElement *parentElement, arg_unused (void *objGrandParent))
{
  return parentElement;
}

void *
NclPresentationSpecConverter::createDescriptorBase (
    DOMElement *parentElement, arg_unused (void *objGrandParent))
{
  DescriptorBase *descBase;
  // criar nova base de conectores com id gerado a partir do nome de seu
  // elemento
  descBase = new DescriptorBase (XMLString::transcode (
      parentElement->getAttribute (XMLString::transcode ("id"))));

  return descBase;
}

void *
NclPresentationSpecConverter::createDescriptor (DOMElement *parentElement,
                                                arg_unused (void *objGrandParent))
{
  Descriptor *descriptor;
  NclDocument *document;
  LayoutRegion *region;
  KeyNavigation *keyNavigation;
  string src;
  FocusDecoration *focusDecoration;
  Color *color;
  string attValue;
  vector<string> *transIds;
  unsigned int i;
  TransitionBase *transitionBase;
  Transition *transition;

  // cria descritor
  descriptor = new Descriptor (XMLString::transcode (
      parentElement->getAttribute (XMLString::transcode ("id"))));

  document = (NclDocument *)getDocumentParser ()->getObject ("return",
                                                             "document");

  // atributo region
  if (parentElement->hasAttribute (XMLString::transcode ("region")))
    {
      region = document->getRegion (XMLString::transcode (
          parentElement->getAttribute (XMLString::transcode ("region"))));

      if (region != NULL)
        {
          descriptor->setRegion (region);
        }
    }

  // atributo explicitDur
  if (parentElement->hasAttribute (XMLString::transcode ("explicitDur")))
    {
      string durStr = XMLString::transcode (parentElement->getAttribute (
          XMLString::transcode ("explicitDur")));

      descriptor->setExplicitDuration (::ginga::util::strUTCToSec (durStr)
                                       * 1000);
    }

  if (parentElement->hasAttribute (XMLString::transcode ("freeze")))
    {
      string freeze;
      freeze = XMLString::transcode (
          parentElement->getAttribute (XMLString::transcode ("freeze")));

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
  if (parentElement->hasAttribute (XMLString::transcode ("player")))
    {
      descriptor->setPlayerName (XMLString::transcode (
          parentElement->getAttribute (XMLString::transcode ("player"))));
    }

  // key navigation attributes
  keyNavigation = new KeyNavigation ();
  descriptor->setKeyNavigation (keyNavigation);
  if (parentElement->hasAttribute (XMLString::transcode ("focusIndex")))
    {
      keyNavigation->setFocusIndex (
          XMLString::transcode (parentElement->getAttribute (
              XMLString::transcode ("focusIndex"))));
    }

  if (parentElement->hasAttribute (XMLString::transcode ("moveUp")))
    {
      keyNavigation->setMoveUp (XMLString::transcode (
          parentElement->getAttribute (XMLString::transcode ("moveUp"))));
    }

  if (parentElement->hasAttribute (XMLString::transcode ("moveDown")))
    {
      keyNavigation->setMoveDown (XMLString::transcode (
          parentElement->getAttribute (XMLString::transcode ("moveDown"))));
    }

  if (parentElement->hasAttribute (XMLString::transcode ("moveLeft")))
    {
      keyNavigation->setMoveLeft (XMLString::transcode (
          parentElement->getAttribute (XMLString::transcode ("moveLeft"))));
    }

  if (parentElement->hasAttribute (XMLString::transcode ("moveRight")))
    {
      keyNavigation->setMoveRight (
          XMLString::transcode (parentElement->getAttribute (
              XMLString::transcode ("moveRight"))));
    }

  focusDecoration = new FocusDecoration ();
  descriptor->setFocusDecoration (focusDecoration);
  if (parentElement->hasAttribute (XMLString::transcode ("focusSrc")))
    {
      src = XMLString::transcode (
          parentElement->getAttribute (XMLString::transcode ("focusSrc")));

      if (getDocumentParser ()->isAbsolutePath (src))
        {
          focusDecoration->setFocusSrc (src);
        }
      else
        {
          focusDecoration->setFocusSrc (
              getDocumentParser ()->getDocumentPath () + src);
        }
    }

  if (parentElement->hasAttribute (
          XMLString::transcode ("focusBorderColor")))
    {
      color = new Color (XMLString::transcode (parentElement->getAttribute (
          XMLString::transcode ("focusBorderColor"))));
           SDL_Color c = color->getColor();
      focusDecoration->setFocusBorderColor ( &c );
    }

  if (parentElement->hasAttribute (
          XMLString::transcode ("focusBorderWidth")))
    {
      int w;
      w = xstrto_int (
          XMLString::transcode (parentElement->getAttribute (
              XMLString::transcode ("focusBorderWidth"))));

      focusDecoration->setFocusBorderWidth (w);
    }

  if (parentElement->hasAttribute (
          XMLString::transcode ("focusBorderTransparency")))
    {
      double alpha;
      alpha = xstrtod (
          XMLString::transcode (parentElement->getAttribute (
              XMLString::transcode ("focusBorderTransparency"))));

      focusDecoration->setFocusBorderTransparency (alpha);
    }

  if (parentElement->hasAttribute (XMLString::transcode ("focusSelSrc")))
    {
      src = XMLString::transcode (parentElement->getAttribute (
          XMLString::transcode ("focusSelSrc")));

      if (getDocumentParser ()->isAbsolutePath (src))
        {
          focusDecoration->setFocusSelSrc (src);
        }
      else
        {
          focusDecoration->setFocusSelSrc (
              getDocumentParser ()->getDocumentPath () + src);
        }
    }

  if (parentElement->hasAttribute (XMLString::transcode ("selBorderColor")))
    {
      color = new Color (XMLString::transcode (parentElement->getAttribute (
          XMLString::transcode ("selBorderColor"))));
           SDL_Color c = color->getColor();
      focusDecoration->setSelBorderColor ( &c );
    }

  if (parentElement->hasAttribute (XMLString::transcode ("transIn")))
    {
      transitionBase = document->getTransitionBase ();
      if (transitionBase != NULL)
        {
          string trimValue, value;
          attValue = XMLString::transcode (parentElement->getAttribute (
              XMLString::transcode ("transIn")));

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

  if (parentElement->hasAttribute (XMLString::transcode ("transOut")))
    {
      transitionBase = document->getTransitionBase ();
      if (transitionBase != NULL)
        {
          string trimValue, value;
          attValue = XMLString::transcode (parentElement->getAttribute (
              XMLString::transcode ("transOut")));

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
