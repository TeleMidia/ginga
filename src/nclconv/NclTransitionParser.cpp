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
      string tagname = dom_element_tagname(child);
      if (tagname == "importBase")
        {
          DOMElement *newel = _nclParser->getImportParser ()
              ->parseImportBase (child);

          if (newel)
            {
              addImportBaseToTransitionBase (transBase, newel);
            }
        }
      else if (tagname == "transition")
        {
          Transition *trans = parseTransition (child);
          if (trans)
            {
              transBase->addTransition (trans);
            }
        }
      else
        {
          syntax_warning( "'%s' is not known as child of 'transitionBase'. "
                          "It will be ignored.",
                          tagname.c_str() );
        }
    }

  return transBase;
}

Transition *
NclTransitionParser::parseTransition (DOMElement *transition_element)
{
  Transition *transition;
  string id, attValue;
  int type, subtype;
  short direction;
  double dur;
  SDL_Color *color;

  if (unlikely (!dom_element_has_attr(transition_element, "id")))
    syntax_error ("transition: missing id");

  id = dom_element_get_attr(transition_element, "id");
  if (unlikely (!dom_element_has_attr(transition_element, "type")))
    {
      syntax_error ("transition '%s': missing type", id.c_str ());
    }

  attValue = dom_element_get_attr(transition_element, "type");
  type = TransitionUtil::getTypeCode (attValue);

  if (unlikely (type < 0))
    syntax_error ("transition '%s': bad type '%d'", id.c_str (), type);

  transition = new Transition (id, type);
  if (dom_element_try_get_attr(attValue, transition_element, "subtype"))
    {
      subtype = TransitionUtil::getSubtypeCode (type, attValue);
      if (subtype >= 0)
        {
          transition->setSubtype (subtype);
        }
      else
        {
          syntax_error ("transition: bad subtype");
        }
    }

  if (dom_element_try_get_attr(attValue, transition_element, "dur"))
    {
      dur = xstrtod (attValue.substr (0, attValue.length () - 1));
      transition->setDur (dur * 1000);
    }

  if (dom_element_try_get_attr(attValue, transition_element, "startProgress"))
    {
      transition->setStartProgress (xstrtod (attValue));
    }

  if (dom_element_try_get_attr(attValue, transition_element, "endProgress"))
    {
      transition->setEndProgress (xstrtod (attValue));
    }

  if (dom_element_try_get_attr(attValue, transition_element, "direction"))
    {
      direction = TransitionUtil::getDirectionCode (attValue);
      if (direction >= 0)
        {
          transition->setDirection (direction);
        }
      else
        {
          syntax_error ("transition: bad direction value");
        }
    }

  if (dom_element_try_get_attr(attValue, transition_element, "fadeColor"))
    {
      color = new SDL_Color ();
      ginga_color_input_to_sdl_color(attValue, color);
      transition->setFadeColor (color);
    }

  if (dom_element_try_get_attr(attValue, transition_element, "horzRepeat"))
    {
      transition->setHorzRepeat (xstrto_int (attValue));
    }

  if (dom_element_try_get_attr(attValue, transition_element, "vertRepeat"))
    {
      transition->setVertRepeat (xstrto_int (attValue));
    }

  if (dom_element_try_get_attr(attValue, transition_element, "borderWidth"))
    {
      transition->setBorderWidth (xstrto_int (attValue));
    }

  if (dom_element_try_get_attr(attValue, transition_element, "borderColor"))
    {
      color = new SDL_Color ();
      ginga_color_input_to_sdl_color(attValue, color);
      transition->setBorderColor (color);
    }

  return transition;
}

void
NclTransitionParser::addImportBaseToTransitionBase (TransitionBase *transBase,
                                                    DOMElement *importBase_element)
{
  string baseAlias, baseLocation;
  NclDocument *importedDocument;
  TransitionBase *importedTransitionBase;

  // get the external base alias and location
  baseAlias = dom_element_get_attr(importBase_element, "alias");
  baseLocation = dom_element_get_attr(importBase_element, "documentURI");

  importedDocument = getNclParser ()->importDocument (baseLocation);
  if (unlikely (importedDocument == NULL))
    {
      syntax_error ("importBase '%s': bad documentURI '%s'",
                    baseAlias.c_str (),
                    baseLocation.c_str ());
    }

  importedTransitionBase = importedDocument->getTransitionBase ();
  if (unlikely (importedTransitionBase == NULL))
    {
      syntax_error ("importBase '%s': no transition base in '%s'",
                    baseAlias.c_str (),
                    baseLocation.c_str ());
    }

  transBase->addBase (importedTransitionBase, baseAlias, baseLocation);
}

GINGA_NCLCONV_END
