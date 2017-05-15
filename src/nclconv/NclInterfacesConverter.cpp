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
#include "NclInterfacesConverter.h"
#include "NclDocumentConverter.h"

GINGA_NCLCONV_BEGIN

NclInterfacesConverter::NclInterfacesConverter (
    NclDocumentParser *documentParser)
    : NclInterfacesParser (documentParser)
{
}

void *
NclInterfacesConverter::createPort (DOMElement *parent,
                                    void *objGrandParent)
{
  string id;
  string attValue;
  Node *portNode;
  NodeEntity *portNodeEntity;
  InterfacePoint *portInterfacePoint = NULL;
  Port *port = NULL;
  CompositeNode *context = (CompositeNode *)objGrandParent;

  if (unlikely (!parent->hasAttribute (XMLString::transcode ("id"))))
    syntax_error ("port: missing id");

  id = XMLString::transcode (parent->getAttribute
                             (XMLString::transcode ("id")));

  if (unlikely (context->getPort (id) != NULL))
    syntax_error ("port '%s': duplicated id", id.c_str ());

  if (!unlikely (parent->hasAttribute (XMLString::transcode ("component"))))
    syntax_error ("port '%s': missing component", id.c_str ());

  attValue = XMLString::transcode (parent->getAttribute
                                   (XMLString::transcode ("component")));

  portNode = context->getNode (attValue);
  if (unlikely (portNode == NULL))
    {
      syntax_error ("port '%s': bad component '%s'", id.c_str (),
                    attValue.c_str ());
    }

  portNodeEntity = (NodeEntity *)portNode->getDataEntity ();
  if (!parent->hasAttribute (XMLString::transcode ("interface")))
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
      attValue = XMLString::transcode
        (parent->getAttribute (XMLString::transcode ("interface")));

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
      attValue = XMLString::transcode
        (parent->getAttribute (XMLString::transcode ("interface")));
      syntax_error ("port '%s': bad interface '%s'", id.c_str (),
                    attValue.c_str ());
    }

  port = new Port (id, portNode, portInterfacePoint);
  return port;
}

SpatialAnchor *
NclInterfacesConverter::createSpatialAnchor (DOMElement *areaElement)
{
  SpatialAnchor *anchor = NULL;
  string coords, shape;

  if (areaElement->hasAttribute (XMLString::transcode ("coords")))
    {
      coords = XMLString::transcode (
          areaElement->getAttribute (XMLString::transcode ("coords")));

      if (areaElement->hasAttribute (XMLString::transcode ("shape")))
        {
          shape = XMLString::transcode (
              areaElement->getAttribute (XMLString::transcode ("shape")));
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
              XMLString::transcode (
                  areaElement->getAttribute (XMLString::transcode ("id"))),
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
NclInterfacesConverter::createTemporalAnchor (DOMElement *areaElement)
{
  IntervalAnchor *anchor = NULL;
  string begin, end;
  double begVal, endVal;
  short firstSyntax, lastSyntax;

  if (areaElement->hasAttribute (XMLString::transcode ("begin"))
      || areaElement->hasAttribute (XMLString::transcode ("end")))
    {
      if (areaElement->hasAttribute (XMLString::transcode ("begin")))
        {
          begin = XMLString::transcode (
              areaElement->getAttribute (XMLString::transcode ("begin")));

          begVal = ::ginga::util::strUTCToSec (begin) * 1000;
        }
      else
        {
          begVal = 0;
        }

      if (areaElement->hasAttribute (XMLString::transcode ("end")))
        {
          end = XMLString::transcode (
              areaElement->getAttribute (XMLString::transcode ("end")));

          endVal = ::ginga::util::strUTCToSec (end) * 1000;
        }
      else
        {
          endVal = IntervalAnchor::OBJECT_DURATION;
        }

      if (xnumeq (endVal, IntervalAnchor::OBJECT_DURATION) || endVal > begVal)
        {
          anchor = new RelativeTimeIntervalAnchor (
              XMLString::transcode (
                  areaElement->getAttribute (XMLString::transcode ("id"))),
              begVal, endVal);
        }
    }

  // region delimeted through sample identifications
  if (areaElement->hasAttribute (XMLString::transcode ("first"))
      || areaElement->hasAttribute (XMLString::transcode ("last")))
    {
      begVal = 0;
      endVal = IntervalAnchor::OBJECT_DURATION;
      firstSyntax = ContentAnchor::CAT_NPT;
      lastSyntax = ContentAnchor::CAT_NPT;

      if (areaElement->hasAttribute (XMLString::transcode ("first")))
        {
          begin = XMLString::transcode (
              areaElement->getAttribute (XMLString::transcode ("first")));

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

      if (areaElement->hasAttribute (XMLString::transcode ("last")))
        {
          end = XMLString::transcode (
              areaElement->getAttribute (XMLString::transcode ("last")));

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
          XMLString::transcode (
              areaElement->getAttribute (XMLString::transcode ("id"))),
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
NclInterfacesConverter::createProperty (DOMElement *parent,
                                        arg_unused (void *objGrandParent))
{
  string attributeName, attributeValue;
  PropertyAnchor *anchor;

  if (unlikely (!parent->hasAttribute (XMLString::transcode ("name"))))
    syntax_error ("property: missing name");

  attributeName = XMLString::transcode
    (parent->getAttribute (XMLString::transcode ("name")));

  anchor = new PropertyAnchor (attributeName);
  if (parent->hasAttribute (XMLString::transcode ("value")))
    {
      attributeValue = XMLString::transcode (
          parent->getAttribute (XMLString::transcode ("value")));
      anchor->setPropertyValue (attributeValue);
    }
  return anchor;
}

void *
NclInterfacesConverter::createArea (DOMElement *parent,
                                    arg_unused (void *objGrandParent))
{
  string anchorId;
  string position, anchorLabel;
  Anchor *anchor;

  if (unlikely (!parent->hasAttribute (XMLString::transcode ("id"))))
    syntax_error ("area: missing id");

  anchorId = XMLString::transcode
    (parent->getAttribute (XMLString::transcode ("id")));

  anchor = NULL;

  if (parent->hasAttribute (XMLString::transcode ("begin"))
      || parent->hasAttribute (XMLString::transcode ("end"))
      || parent->hasAttribute (XMLString::transcode ("first"))
      || parent->hasAttribute (XMLString::transcode ("last")))
    {
      anchor = createTemporalAnchor (parent);
    }
  else if (parent->hasAttribute (XMLString::transcode ("text")))
    {
      position = XMLString::transcode (
          parent->getAttribute (XMLString::transcode ("position")));

      anchor = new TextAnchor (
          anchorId, XMLString::transcode (parent->getAttribute (
                        XMLString::transcode ("text"))),
          xstrto_int (position));
    }
  else if (parent->hasAttribute (XMLString::transcode ("coords")))
    {
      anchor = createSpatialAnchor (parent);
    }
  else if (parent->hasAttribute (XMLString::transcode ("label")))
    {
      anchorLabel = XMLString::transcode (
          parent->getAttribute (XMLString::transcode ("label")));

      anchor = new LabeledAnchor (anchorId, anchorLabel);
    }
  else
    {
      anchor = new LabeledAnchor (anchorId, anchorId);
    }

  g_assert_nonnull (anchor);

  return anchor;
}

void *
NclInterfacesConverter::createMapping (DOMElement *parent,
                                       void *objGrandParent)
{
  DOMElement *switchElement;
  SwitchNode *switchNode;
  NodeEntity *mappingNodeEntity;
  Node *mappingNode;
  InterfacePoint *interfacePoint;
  Port *port;

  switchElement
      = (DOMElement *)parent->getParentNode ()->getParentNode ();

  switchNode
      = (SwitchNode *)((NclDocumentConverter *)getDocumentParser ())
            ->getNode (XMLString::transcode (
                switchElement->getAttribute (XMLString::transcode ("id"))));

  mappingNode = switchNode->getNode (XMLString::transcode (
      parent->getAttribute (XMLString::transcode ("component"))));

  if (unlikely (mappingNode == NULL))
    syntax_error ("mapping: bad component '%s'",
                  string (XMLString::transcode
                          (parent->getAttribute
                           (XMLString::transcode ("component")))).c_str ());

  mappingNodeEntity = (NodeEntity *)mappingNode->getDataEntity ();
  if (parent->hasAttribute (XMLString::transcode ("interface")))
    {
      interfacePoint = mappingNodeEntity->getAnchor (
          XMLString::transcode (parent->getAttribute (
              XMLString::transcode ("interface"))));

      if (interfacePoint == NULL)
        {
          if (mappingNodeEntity->instanceOf ("CompositeNode"))
            {
              interfacePoint
                  = ((CompositeNode *)mappingNodeEntity)
                        ->getPort (XMLString::transcode (
                            parent->getAttribute (
                                XMLString::transcode ("interface"))));
            }
        }
    }
  else
    {
      interfacePoint = mappingNodeEntity->getAnchor (0);
    }

  if (unlikely (interfacePoint == NULL))
    syntax_error ("mapping: bad interface '%s'",
                  string (XMLString::transcode
                          (parent->getAttribute
                           (XMLString::transcode ("interface")))).c_str ());

  port = new Port (((SwitchPort *)objGrandParent)->getId (), mappingNode,
                   interfacePoint);

  return port;
}

void *
NclInterfacesConverter::createSwitchPort (DOMElement *parent,
                                          void *objGrandParent)
{
  SwitchNode *switchNode;
  SwitchPort *switchPort;
  string id;

  switchNode = (SwitchNode *)objGrandParent;

  if (unlikely (!parent->hasAttribute (XMLString::transcode ("id"))))
    syntax_error ("switchPort: missing id");

  id = XMLString::transcode (
      parent->getAttribute (XMLString::transcode ("id")));

  if (unlikely (switchNode->getPort (id) != NULL))
    syntax_error ("switchPort '%s': duplicated id", id.c_str ());

  switchPort = new SwitchPort (id, switchNode);
  return switchPort;
}

void
NclInterfacesConverter::addMappingToSwitchPort (void *parentObject,
                                                void *childObject)
{
  ((SwitchPort *)parentObject)->addPort ((Port *)childObject);
}

GINGA_NCLCONV_END
