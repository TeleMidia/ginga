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

#include "NclTransitionParser.h"
#include "NclDocumentParser.h"

GINGA_PRAGMA_DIAG_IGNORE (-Wsign-conversion)

GINGA_NCLCONV_BEGIN

NclTransitionParser::NclTransitionParser (NclDocumentParser *documentParser)
    : ModuleParser (documentParser)
{
}

void *
NclTransitionParser::parseTransitionBase (DOMElement *parentElement,
                                          void *objGrandParent)
{
  void *parentObject;
  DOMNodeList *elementNodeList;
  DOMElement *element;
  DOMNode *node;
  string elementTagName;
  void *elementObject;

  parentObject = new TransitionBase (
              XMLString::transcode (
                    parentElement->getAttribute (XMLString::transcode ("id"))));

  g_assert_nonnull (parentObject);

  elementNodeList = parentElement->getChildNodes ();
  for (int i = 0; i < (int)elementNodeList->getLength (); i++)
    {
      node = elementNodeList->item (i);
      if (node->getNodeType () == DOMNode::ELEMENT_NODE)
        {
          element = (DOMElement *)node;
          elementTagName = XMLString::transcode (element->getTagName ());

          if (XMLString::compareIString (elementTagName.c_str (),
                                         "importBase")
              == 0)
            {
              elementObject = getImportParser ()->parseImportBase (
                  element, parentObject);

              if (elementObject != NULL)
                {
                  addImportBaseToTransitionBase (parentObject,
                                                 elementObject);
                }
            }
          else if (XMLString::compareIString (elementTagName.c_str (),
                                              "transition")
                   == 0)
            {
              elementObject = parseTransition (element, parentObject);
              if (elementObject != NULL)
                {
                  addTransitionToTransitionBase (parentObject,
                                                 elementObject);
                }
            }
        }
    }

  return parentObject;
}

void *
NclTransitionParser::parseTransition (DOMElement *parentElement,
                                      arg_unused (void *objGrandParent))
{
  Transition *transition;
  string id, attValue;
  int type, subtype;
  short direction;
  double dur;
  SDL_Color *color;

  if (unlikely (!parentElement->hasAttribute (XMLString::transcode ("id"))))
    syntax_error ("transition: missing id");

  id = XMLString::transcode
    (parentElement->getAttribute (XMLString::transcode ("id")));

  if (unlikely (!parentElement->hasAttribute
                (XMLString::transcode ("type"))))
    {
      syntax_error ("transition '%s': missing type", id.c_str ());
    }

  attValue = XMLString::transcode
    (parentElement->getAttribute (XMLString::transcode ("type")));

  type = TransitionUtil::getTypeCode (attValue);
  if (unlikely (type < 0))
    syntax_error ("transition '%s': bad type '%d'", id.c_str (), type);

  transition = new Transition (id, type);

  if (parentElement->hasAttribute (XMLString::transcode ("subtype")))
    {
      attValue = XMLString::transcode (
          parentElement->getAttribute (XMLString::transcode ("subtype")));

      subtype = TransitionUtil::getSubtypeCode (type, attValue);
      if (subtype >= 0)
        {
          transition->setSubtype (subtype);
        }
    }

  if (parentElement->hasAttribute (XMLString::transcode ("dur")))
    {
      attValue = XMLString::transcode (
          parentElement->getAttribute (XMLString::transcode ("dur")));

      dur = xstrtod (
          attValue.substr (0, attValue.length () - 1));
      transition->setDur (dur * 1000);
    }

  if (parentElement->hasAttribute (XMLString::transcode ("startProgress")))
    {
      attValue = XMLString::transcode (parentElement->getAttribute (
          XMLString::transcode ("startProgress")));

      transition->setStartProgress (xstrtod (attValue));
    }

  if (parentElement->hasAttribute (XMLString::transcode ("endProgress")))
    {
      attValue = XMLString::transcode (parentElement->getAttribute (
          XMLString::transcode ("endProgress")));

      transition->setEndProgress (xstrtod (attValue));
    }

  if (parentElement->hasAttribute (XMLString::transcode ("direction")))
    {
      attValue = XMLString::transcode (
          parentElement->getAttribute (XMLString::transcode ("direction")));

      direction = TransitionUtil::getDirectionCode (attValue);
      if (direction >= 0)
        {
          transition->setDirection (direction);
        }
    }

  if (parentElement->hasAttribute (XMLString::transcode ("fadeColor")))
    {
      color = new SDL_Color ();
      ginga_color_input_to_sdl_color( XMLString::transcode (parentElement->getAttribute (
          XMLString::transcode ("fadeColor"))), color);
      transition->setFadeColor (color);
    }

  if (parentElement->hasAttribute (XMLString::transcode ("horzRepeat")))
    {
      attValue = XMLString::transcode (parentElement->getAttribute (
          XMLString::transcode ("horzRepeat")));

      transition->setHorzRepeat (xstrto_int (attValue));
    }

  if (parentElement->hasAttribute (XMLString::transcode ("vertRepeat")))
    {
      attValue = XMLString::transcode (parentElement->getAttribute (
          XMLString::transcode ("vertRepeat")));

      transition->setVertRepeat (xstrto_int (attValue));
    }

  if (parentElement->hasAttribute (XMLString::transcode ("borderWidth")))
    {
      attValue = XMLString::transcode (parentElement->getAttribute (
          XMLString::transcode ("borderWidth")));

      transition->setBorderWidth (xstrto_int (attValue));
    }

  if (parentElement->hasAttribute (XMLString::transcode ("borderColor")))
    {
      color = new SDL_Color ();
      ginga_color_input_to_sdl_color( XMLString::transcode (parentElement->getAttribute (
          XMLString::transcode ("borderColor"))), color);
      transition->setBorderColor (color);
    }

  return transition;
}

NclImportParser *
NclTransitionParser::getImportParser ()
{
  return importParser;
}

void
NclTransitionParser::setImportParser (NclImportParser *importParser)
{
  this->importParser = importParser;
}



void
NclTransitionParser::addTransitionToTransitionBase (void *parentObject,
                                                    void *childObject)
{
  ((TransitionBase *)parentObject)->addTransition ((Transition *)childObject);
}

void
NclTransitionParser::addImportBaseToTransitionBase (void *parentObject,
                                                    void *childObject)
{
  string baseAlias, baseLocation;
  NclDocumentParser *compiler;
  NclDocument *importedDocument;
  TransitionBase *createdBase;

  // get the external base alias and location
  baseAlias = XMLString::transcode (
      ((DOMElement *)childObject)
          ->getAttribute (XMLString::transcode ("alias")));

  baseLocation = XMLString::transcode (
      ((DOMElement *)childObject)
          ->getAttribute (XMLString::transcode ("documentURI")));

  compiler = getDocumentParser ();
  importedDocument = compiler->importDocument (baseLocation);
  if (importedDocument == NULL)
    {
      return;
    }

  createdBase = importedDocument->getTransitionBase ();
  if (createdBase == NULL)
    {
      return;
    }

  // insert the imported base into the document region base
  try
    {
      ((TransitionBase *)parentObject)
          ->addBase (createdBase, baseAlias, baseLocation);
    }
  catch (std::exception *exc)
    {
      syntax_error ("importBase: bad transition base");
    }
}

GINGA_NCLCONV_END
