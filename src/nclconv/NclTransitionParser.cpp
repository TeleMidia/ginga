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
NclTransitionParser::parseTransitionBase (DOMElement *transBase_element)
{
  TransitionBase *transBase = new TransitionBase (
        dom_element_get_attr(transBase_element, "id") );
  g_assert_nonnull (transBase);


  for(DOMElement *child: dom_element_children(transBase_element))
    {
      if (dom_element_tagname(child) == "importBase")
        {
          DOMElement *newel = _nclParser->getImportParser ()
              ->parseImportBase (child);

          if (newel)
            {
              addImportBaseToTransitionBase (transBase, newel);
            }
        }
      else if (dom_element_tagname(child) == "transition")
        {
          Transition *trans = parseTransition (child);
          if (trans)
            {
              transBase->addTransition (trans);
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

  if (unlikely (!dom_element_has_attr(parentElement, "id")))
    syntax_error ("transition: missing id");

  id = dom_element_get_attr(parentElement, "id");

  if (unlikely (!dom_element_has_attr(parentElement, "type")))
    {
      syntax_error ("transition '%s': missing type", id.c_str ());
    }

  attValue = dom_element_get_attr(parentElement, "type");
  type = TransitionUtil::getTypeCode (attValue);

  if (unlikely (type < 0))
    syntax_error ("transition '%s': bad type '%d'", id.c_str (), type);

  transition = new Transition (id, type);

  if (dom_element_has_attr(parentElement, "subtype"))
    {
      attValue = dom_element_get_attr(parentElement, "subtype");

      subtype = TransitionUtil::getSubtypeCode (type, attValue);
      if (subtype >= 0)
        {
          transition->setSubtype (subtype);
        }
    }

  if (dom_element_has_attr(parentElement, "dur"))
    {
      attValue = dom_element_get_attr(parentElement, "dur");

      dur = xstrtod (attValue.substr (0, attValue.length () - 1));
      transition->setDur (dur * 1000);
    }

  if (dom_element_has_attr(parentElement, "startProgress"))
    {
      attValue = dom_element_get_attr(parentElement, "startProgress");

      transition->setStartProgress (xstrtod (attValue));
    }

  if (dom_element_has_attr(parentElement, "endProgress"))
    {
      attValue = dom_element_get_attr(parentElement, "endProgress");

      transition->setEndProgress (xstrtod (attValue));
    }

  if (dom_element_has_attr(parentElement, "direction"))
    {
      attValue = dom_element_get_attr(parentElement, "direction");

      direction = TransitionUtil::getDirectionCode (attValue);
      if (direction >= 0)
        {
          transition->setDirection (direction);
        }
    }

  if (dom_element_has_attr(parentElement, "fadeColor"))
    {
      color = new SDL_Color ();
      ginga_color_input_to_sdl_color(
            dom_element_get_attr(parentElement, "fadeColor"), color);
      transition->setFadeColor (color);
    }

  if (dom_element_has_attr(parentElement, "horzRepeat"))
    {
      attValue = dom_element_get_attr(parentElement, "horzRepeat");

      transition->setHorzRepeat (xstrto_int (attValue));
    }

  if (dom_element_has_attr(parentElement, "vertRepeat"))
    {
      attValue = dom_element_get_attr(parentElement, "vertRepeat");

      transition->setVertRepeat (xstrto_int (attValue));
    }

  if (dom_element_has_attr(parentElement, "borderWidth"))
    {
      attValue = dom_element_get_attr(parentElement, "borderWidth");

      transition->setBorderWidth (xstrto_int (attValue));
    }

  if (dom_element_has_attr(parentElement, "borderColor"))
    {
      color = new SDL_Color ();
      ginga_color_input_to_sdl_color(
            dom_element_get_attr(parentElement, "borderColor"),
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
  baseAlias = dom_element_get_attr(element, "alias");
  baseLocation = dom_element_get_attr(element, "documentURI");

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
