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
#include "NclInterfacesParser.h"

#include "NclDocumentParser.h"

GINGA_PRAGMA_DIAG_IGNORE (-Wsign-conversion)

GINGA_NCLCONV_BEGIN

NclInterfacesParser::NclInterfacesParser (NclDocumentParser *documentParser)
    : ModuleParser (documentParser)
{
}

void *
NclInterfacesParser::parseSwitchPort (DOMElement *parentElement,
                                      void *objGrandParent)
{
  void *parentObject;

  parentObject = createSwitchPort (parentElement, objGrandParent);
  if (unlikely (parentObject == NULL))
    syntax_error ("switchPort: bad parent '%s'",
                  XMLString::transcode(parentElement->getTagName()));

  DOMNodeList *elementNodeList = parentElement->getChildNodes ();
  for (int i = 0; i < (int)elementNodeList->getLength (); i++)
    {
      DOMNode *node = elementNodeList->item (i);
      if (node->getNodeType () == DOMNode::ELEMENT_NODE)
        {
          DOMElement *element = (DOMElement *)node;
          string tagname = _documentParser->getTagname(element);

          if (XMLString::compareIString (tagname.c_str (), "mapping") == 0)
            {
              void *elementObject = parseMapping (element, parentObject);
              if (elementObject)
                {
                  addMappingToSwitchPort (parentObject, elementObject);
                }
            }
        }
    }

  return parentObject;
}

void *
NclInterfacesParser::parseMapping (DOMElement *parent,
                                   void *objGrandParent)
{
  DOMElement *switchElement;
  SwitchNode *switchNode;
  NodeEntity *mappingNodeEntity;
  Node *mappingNode;
  InterfacePoint *interfacePoint;
  Port *port;

  switchElement = (DOMElement *)parent->getParentNode ()->getParentNode ();

  string id = _documentParser->getAttribute(switchElement, "id");
  string component = _documentParser->getAttribute(parent, "component");

  switchNode = (SwitchNode *)getDocumentParser ()->getNode (id);
  mappingNode = switchNode->getNode (component);

  if (unlikely (mappingNode == NULL))
    syntax_error ("mapping: bad component '%s'", component.c_str ());

  mappingNodeEntity = (NodeEntity *)mappingNode->getDataEntity ();

  string interface;
  if (_documentParser->hasAttribute(parent, "interface"))
    {
      interface = _documentParser->getAttribute(parent, "interface");
      interfacePoint = mappingNodeEntity->getAnchor (interface);

      if (interfacePoint == NULL)
        {
          if (mappingNodeEntity->instanceOf ("CompositeNode"))
            {
              interfacePoint
                  = ((CompositeNode *)mappingNodeEntity)->getPort (interface);
            }
        }
    }
  else
    {
      interfacePoint = mappingNodeEntity->getAnchor (0);
    }

  if (unlikely (interfacePoint == NULL))
    syntax_error ("mapping: bad interface '%s'", interface);

  port = new Port (((SwitchPort *)objGrandParent)->getId (), mappingNode,
                   interfacePoint);

  return port;
}

Anchor *
NclInterfacesParser::parseArea (DOMElement *parent,
                                void *objGrandParent)
{
  string anchorId;
  string position, anchorLabel;
  Anchor *anchor;

  if (unlikely (!_documentParser->hasAttribute(parent, "id")))
    syntax_error ("area: missing id");

  anchorId = _documentParser->getAttribute(parent, "id");

  anchor = NULL;

  if (_documentParser->hasAttribute(parent, "begin")
      || _documentParser->hasAttribute(parent, "end")
      || _documentParser->hasAttribute(parent, "first")
      || _documentParser->hasAttribute(parent, "last"))
    {
      anchor = createTemporalAnchor (parent);
    }
  else if (_documentParser->hasAttribute (parent, "text"))
    {
      position = _documentParser->getAttribute(parent, "position");

      anchor = new TextAnchor (
          anchorId, _documentParser->getAttribute(parent, "text"),
          xstrto_int (position));
    }
  else if (_documentParser->hasAttribute(parent, "coords"))
    {
      anchor = createSpatialAnchor (parent);
    }
  else if (_documentParser->hasAttribute(parent, "label"))
    {
      anchorLabel = _documentParser->getAttribute(parent, "label");

      anchor = new LabeledAnchor (anchorId, anchorLabel);
    }
  else
    {
      anchor = new LabeledAnchor (anchorId, anchorId);
    }

  g_assert_nonnull (anchor);

  return anchor;
}

PropertyAnchor *
NclInterfacesParser::parseProperty (DOMElement *parent,
                                    Node *objGrandParent)
{
  string attributeName, attributeValue;
  PropertyAnchor *anchor;

  if (unlikely (!_documentParser->hasAttribute(parent, "name")))
    syntax_error ("property: missing name");

  attributeName = _documentParser->getAttribute(parent, "name");

  anchor = new PropertyAnchor (attributeName);
  if (_documentParser->hasAttribute (parent, "value"))
    {
      attributeValue = _documentParser->getAttribute(parent, "value");
      anchor->setPropertyValue (attributeValue);
    }

  return anchor;
}

void *
NclInterfacesParser::parsePort (DOMElement *parent,
                                void *objGrandParent)
{
  string id, attValue;
  Node *portNode;
  NodeEntity *portNodeEntity;
  InterfacePoint *portInterfacePoint = NULL;
  Port *port = NULL;
  CompositeNode *context = (CompositeNode *)objGrandParent;

  if (unlikely (!_documentParser->hasAttribute(parent, "id")))
    syntax_error ("port: missing id");

  id = _documentParser->getAttribute(parent, "id");

  if (unlikely (context->getPort (id) != NULL))
    syntax_error ("port '%s': duplicated id", id.c_str ());

  if (!unlikely (_documentParser->hasAttribute (parent, "component")))
    syntax_error ("port '%s': missing component", id.c_str ());

  attValue = _documentParser->getAttribute(parent, "component");

  portNode = context->getNode (attValue);
  if (unlikely (portNode == NULL))
    {
      syntax_error ("port '%s': bad component '%s'", id.c_str (),
                    attValue.c_str ());
    }

  portNodeEntity = (NodeEntity *)portNode->getDataEntity ();
  if (!_documentParser->hasAttribute(parent, "interface"))
    {
      if (portNode->instanceOf ("ReferNode")
          && ((ReferNode *)portNode)->getInstanceType () == "new")
        {
          portInterfacePoint = portNode->getAnchor (0);
          if (portInterfacePoint == NULL)
            {
              portInterfacePoint = new LambdaAnchor (portNode->getId ());
              portNode->addAnchor (0, (Anchor *)portInterfacePoint);
            }
        }
      else if (portNodeEntity->instanceOf ("Node"))
        {
          portInterfacePoint = portNodeEntity->getAnchor (0);
          if (unlikely (portInterfacePoint == NULL))
            {
              syntax_error ("port '%s': bad interface '%s'",
                            id.c_str (), portNodeEntity->getId ().c_str ());
            }
        }
      else
        {
          g_assert_not_reached ();
        }
    }
  else
    {
      attValue = _documentParser->getAttribute(parent, "interface");

      if (portNode->instanceOf ("ReferNode")
          && ((ReferNode *)portNode)->getInstanceType () == "new")
        {
          portInterfacePoint = portNode->getAnchor (attValue);
        }
      else
        {
          portInterfacePoint = portNodeEntity->getAnchor (attValue);
        }

      if (portInterfacePoint == NULL)
        {
          if (portNodeEntity->instanceOf ("CompositeNode"))
            {
              portInterfacePoint
                  = ((CompositeNode *)portNodeEntity)->getPort (attValue);
            }
          else
            {
              portInterfacePoint = portNode->getAnchor (attValue);
            }
        }
    }

  if (unlikely (portInterfacePoint == NULL))
    {
      attValue = _documentParser->getAttribute(parent, "interface");
      syntax_error ("port '%s': bad interface '%s'", id.c_str (),
                    attValue.c_str ());
    }

  port = new Port (id, portNode, portInterfacePoint);
  return port;
}

SpatialAnchor *
NclInterfacesParser::createSpatialAnchor (DOMElement *areaElement)
{
  SpatialAnchor *anchor = NULL;
  string coords, shape;

  if (_documentParser->hasAttribute(areaElement, "coords"))
    {
      coords = _documentParser->getAttribute(areaElement, "coords");

      if (_documentParser->hasAttribute(areaElement, "shape"))
        {
          shape = _documentParser->getAttribute(areaElement, "shape");
        }
      else
        {
          shape = "rect";
        }

      if (shape == "rect" || shape == "default")
        {
          long int x1, y1, x2, y2;
          sscanf (coords.c_str (), "%ld,%ld,%ld,%ld", &x1, &y1, &x2, &y2);
          anchor = new RectangleSpatialAnchor (
                _documentParser->getAttribute(areaElement, "id"),
                x1, y1, x2 - x1, y2 - y1);
        }
      else if (shape == "circle")
        {
          // TODO
        }
      else if (shape == "poly")
        {
          // TODO
        }
    }
  return anchor;
}

IntervalAnchor *
NclInterfacesParser::createTemporalAnchor (DOMElement *areaElement)
{
  IntervalAnchor *anchor = NULL;
  string begin, end;
  double begVal, endVal;
  short firstSyntax, lastSyntax;

  if (_documentParser->hasAttribute(areaElement, "begin")
      || _documentParser->hasAttribute(areaElement, "end"))
    {
      if (_documentParser->hasAttribute(areaElement ,"begin"))
        {
          begin = _documentParser->getAttribute(areaElement, "begin");

          begVal = ::ginga::util::strUTCToSec (begin) * 1000;
        }
      else
        {
          begVal = 0;
        }

      if (_documentParser->hasAttribute(areaElement, "end"))
        {
          end = _documentParser->getAttribute(areaElement, "end");

          endVal = ::ginga::util::strUTCToSec (end) * 1000;
        }
      else
        {
          endVal = IntervalAnchor::OBJECT_DURATION;
        }

      if (xnumeq (endVal, IntervalAnchor::OBJECT_DURATION) || endVal > begVal)
        {
          anchor = new RelativeTimeIntervalAnchor (
              _documentParser->getAttribute(areaElement, "id"),
              begVal, endVal);
        }
    }

  // region delimeted through sample identifications
  if (_documentParser->hasAttribute(areaElement, "first")
      || _documentParser->hasAttribute(areaElement, "last"))
    {
      begVal = 0;
      endVal = IntervalAnchor::OBJECT_DURATION;
      firstSyntax = ContentAnchor::CAT_NPT;
      lastSyntax = ContentAnchor::CAT_NPT;

      if (_documentParser->hasAttribute(areaElement, "first"))
        {
          begin = _documentParser->getAttribute(areaElement, "first");

          if (begin.find ("s") != std::string::npos)
            {
              firstSyntax = ContentAnchor::CAT_SAMPLES;
              begVal = xstrtod (
                  begin.substr (0, begin.length () - 1));
            }
          else if (begin.find ("f") != std::string::npos)
            {
              firstSyntax = ContentAnchor::CAT_FRAMES;
              begVal = xstrtod (
                  begin.substr (0, begin.length () - 1));
            }
          else if (begin.find ("npt") != std::string::npos
                   || begin.find ("NPT") != std::string::npos)
            {
              firstSyntax = ContentAnchor::CAT_NPT;
              begVal = xstrtod (
                  begin.substr (0, begin.length () - 3));
            }
        }

      if (_documentParser->hasAttribute(areaElement, "last"))
        {
          end = _documentParser->getAttribute(areaElement, "last");

          if (end.find ("s") != std::string::npos)
            {
              lastSyntax = ContentAnchor::CAT_SAMPLES;
              endVal
                  = xstrtod (end.substr (0, end.length () - 1));
            }
          else if (end.find ("f") != std::string::npos)
            {
              lastSyntax = ContentAnchor::CAT_FRAMES;
              endVal
                  = xstrtod (end.substr (0, end.length () - 1));
            }
          else if (end.find ("npt") != std::string::npos
                   || end.find ("NPT") != std::string::npos)
            {
              lastSyntax = ContentAnchor::CAT_NPT;
              endVal
                  = xstrtod (end.substr (0, end.length () - 3));
            }
        }

      anchor = new SampleIntervalAnchor (
            _documentParser->getAttribute(areaElement, "id"),
          begVal, endVal);

      ((SampleIntervalAnchor *)anchor)
          ->setValueSyntax (firstSyntax, lastSyntax);
    }

  if (anchor != NULL)
    {
      anchor->setStrValues (begin, end);
    }

  return anchor;
}

void *
NclInterfacesParser::createSwitchPort (DOMElement *parent,
                                       void *objGrandParent)
{
  SwitchNode *switchNode;
  SwitchPort *switchPort;
  string id;

  switchNode = (SwitchNode *)objGrandParent;

  if (unlikely (!_documentParser->hasAttribute(parent, "id")))
    syntax_error ("switchPort: missing id");

  id = _documentParser->getAttribute(parent, "id");

  if (unlikely (switchNode->getPort (id) != NULL))
    syntax_error ("switchPort '%s': duplicated id", id.c_str ());

  switchPort = new SwitchPort (id, switchNode);
  return switchPort;
}

void
NclInterfacesParser::addMappingToSwitchPort (void *parentObject,
                                             void *childObject)
{
  ((SwitchPort *)parentObject)->addPort ((Port *)childObject);
}

GINGA_NCLCONV_END
