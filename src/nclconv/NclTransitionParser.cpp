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
#include "NclParser.h"

GINGA_PRAGMA_DIAG_IGNORE (-Wsign-conversion)

GINGA_NCLCONV_BEGIN

NclTransitionParser::NclTransitionParser (NclParser *nclParser)
    : ModuleParser (nclParser)
{
}

TransitionBase *
NclTransitionParser::parseTransitionBase (DOMElement *parentElement)
{
  TransitionBase *transBase = new TransitionBase (
        _nclParser->getAttribute(parentElement, "id") );
  g_assert_nonnull (transBase);

  DOMNodeList *elementNodeList = parentElement->getChildNodes ();
  for (int i = 0; i < (int)elementNodeList->getLength (); i++)
    {
      DOMNode *node = elementNodeList->item (i);
      if (node->getNodeType () == DOMNode::ELEMENT_NODE)
        {
          DOMElement *element = (DOMElement *)node;
          string tagname = _nclParser->getTagname(element);

          if (XMLString::compareIString (tagname.c_str(), "importBase") == 0)
            {
              DOMElement *elementObject = _nclParser->getImportParser ()
                      ->parseImportBase (element);

              if (elementObject != NULL)
                {
                  addImportBaseToTransitionBase (transBase, elementObject);
                }
            }
          else if (XMLString::compareIString (tagname.c_str(), "transition")
                   == 0)
            {
              Transition *trans = parseTransition (element);
              if (trans)
                {
                  transBase->addTransition (trans);
                }
            }
        }
    }

  return transBase;
}

Transition *
NclTransitionParser::parseTransition (DOMElement *parentElement)
{
  Transition *transition;
  string id, attValue;
  int type, subtype;
  short direction;
  double dur;
  SDL_Color *color;

  if (unlikely (!_nclParser->hasAttribute(parentElement, "id")))
    syntax_error ("transition: missing id");

  id = _nclParser->getAttribute(parentElement, "id");

  if (unlikely (!_nclParser->hasAttribute(parentElement, "type")))
    {
      syntax_error ("transition '%s': missing type", id.c_str ());
    }

  attValue = _nclParser->getAttribute(parentElement, "type");
  type = TransitionUtil::getTypeCode (attValue);

  if (unlikely (type < 0))
    syntax_error ("transition '%s': bad type '%d'", id.c_str (), type);

  transition = new Transition (id, type);

  if (_nclParser->hasAttribute(parentElement, "subtype"))
    {
      attValue = _nclParser->getAttribute(parentElement, "subtype");

      subtype = TransitionUtil::getSubtypeCode (type, attValue);
      if (subtype >= 0)
        {
          transition->setSubtype (subtype);
        }
    }

  if (_nclParser->hasAttribute(parentElement, "dur"))
    {
      attValue = _nclParser->getAttribute(parentElement, "dur");

      dur = xstrtod (attValue.substr (0, attValue.length () - 1));
      transition->setDur (dur * 1000);
    }

  if (_nclParser->hasAttribute(parentElement, "startProgress"))
    {
      attValue = _nclParser->getAttribute(parentElement, "startProgress");

      transition->setStartProgress (xstrtod (attValue));
    }

  if (_nclParser->hasAttribute(parentElement, "endProgress"))
    {
      attValue = _nclParser->getAttribute(parentElement, "endProgress");

      transition->setEndProgress (xstrtod (attValue));
    }

  if (_nclParser->hasAttribute(parentElement, "direction"))
    {
      attValue = _nclParser->getAttribute(parentElement, "direction");

      direction = TransitionUtil::getDirectionCode (attValue);
      if (direction >= 0)
        {
          transition->setDirection (direction);
        }
    }

  if (_nclParser->hasAttribute(parentElement, "fadeColor"))
    {
      color = new SDL_Color ();
      ginga_color_input_to_sdl_color(
            _nclParser->getAttribute(parentElement, "fadeColor"), color);
      transition->setFadeColor (color);
    }

  if (_nclParser->hasAttribute(parentElement, "horzRepeat"))
    {
      attValue = _nclParser->getAttribute(parentElement, "horzRepeat");

      transition->setHorzRepeat (xstrto_int (attValue));
    }

  if (_nclParser->hasAttribute(parentElement, "vertRepeat"))
    {
      attValue = _nclParser->getAttribute(parentElement, "vertRepeat");

      transition->setVertRepeat (xstrto_int (attValue));
    }

  if (_nclParser->hasAttribute(parentElement, "borderWidth"))
    {
      attValue = _nclParser->getAttribute(parentElement, "borderWidth");

      transition->setBorderWidth (xstrto_int (attValue));
    }

  if (_nclParser->hasAttribute(parentElement, "borderColor"))
    {
      color = new SDL_Color ();
      ginga_color_input_to_sdl_color(
            _nclParser->getAttribute(parentElement, "borderColor"),
                  color);
      transition->setBorderColor (color);
    }

  return transition;
}

void
NclTransitionParser::addImportBaseToTransitionBase (TransitionBase *transBase,
                                                    DOMElement *element)
{
  string baseAlias, baseLocation;
  NclParser *compiler;
  NclDocument *importedDocument;
  TransitionBase *createdBase;

  // get the external base alias and location
  baseAlias = _nclParser->getAttribute(element, "alias");

  baseLocation = _nclParser->getAttribute(element, "documentURI");

  compiler = getNclParser ();
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
      transBase->addBase (createdBase, baseAlias, baseLocation);
    }
  catch (std::exception *exc)
    {
      syntax_error ("importBase: bad transition base");
    }
}

GINGA_NCLCONV_END
