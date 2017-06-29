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
#include "Parser.h"

#include "mb/Display.h"
using namespace ginga::mb;

GINGA_PRAGMA_DIAG_IGNORE (-Wunused-macros)

GINGA_FORMATTER_BEGIN


// Xerces wrappers.

/**
 * @brief Gets element tagname.
 * @param elt Element.
 * @return Element tagname.
 */
static string
dom_element_tagname (const DOMElement *elt)
{
  char *str = XMLString::transcode (elt->getTagName ());
  string tagname (str);
  XMLString::release (&str);
  return tagname;
}

/**
 * @brief Tests if element has a given attribute.
 * @param elt Element.
 * @param name Attribute name.
 * @return True if element has attribute, or false otherwise.
 */
static bool
dom_element_has_attr (const DOMElement *elt, const string &name)
{
  XMLCh *xmlch = XMLString::transcode (name.c_str ());
  bool result = elt->hasAttribute (xmlch);
  XMLString::release (&xmlch);
  return result;
}

/**
 * @brief Gets element attribute.
 * @param elt Element.
 * @param name Attribute name.
 * @return Attribute value or the empty string (no such attribute).
 */
static string
dom_element_get_attr (const DOMElement *elt, const string &name)
{
  XMLCh *xmlch = XMLString::transcode (name.c_str ());
  char *str = XMLString::transcode (elt->getAttribute (xmlch));
  string value (str);
  XMLString::release (&xmlch);
  XMLString::release (&str);
  return value;
}

/**
 * @brief Gets element attribute.
 * @param elt Element.
 * @param name Attribute name.
 * @param value Address to store the attribute value.
 * @return True if successful, or false otherwise.
 */
static bool
dom_element_try_get_attr (string &value, const DOMElement *elt,
                          const string &name)
{
  XMLCh *xmlch;
  bool status;

  xmlch = XMLString::transcode (name.c_str ());
  if (elt->hasAttribute (xmlch))
    {
      char *str = XMLString::transcode (elt->getAttribute (xmlch));
      value = string (str);
      status = true;
      XMLString::release (&str);
    }
  else
    {
      status = false;
    }
  XMLString::release(&xmlch);
  return status;
}

#define FOR_EACH_DOM_ELEM_CHILD(X, Y) \
  for ( X = Y->getFirstElementChild(); \
        X != nullptr; \
        X = X->getNextElementSibling())

static vector <DOMElement *>
dom_element_children (DOMElement *el)
{
  vector <DOMElement *> vet;

  DOMElement *child;
  FOR_EACH_DOM_ELEM_CHILD(child, el)
    {
      vet.push_back(child);
    }

  return vet;
}

static vector <DOMElement *>
dom_element_children_by_tagname (const DOMElement *elt,
                                 const string &tagname)
{
  vector <DOMElement *> vet;
  DOMElement *child;
  FOR_EACH_DOM_ELEM_CHILD(child, elt)
    if (dom_element_tagname (child) == tagname)
      vet.push_back(child);
  return vet;
}

static G_GNUC_UNUSED vector <DOMElement *>
dom_element_children_by_tagnames (DOMElement *elt,
                                  const vector<string> &tags)
{
  vector <DOMElement *> vet;
  DOMElement *child;
  FOR_EACH_DOM_ELEM_CHILD(child, elt)
    {
      if (std::find (tags.begin (), tags.end (),
                     dom_element_tagname (child)) != tags.end())
        {
          vet.push_back (child);
        }
    }
  return vet;
}


// Common errors.

static inline string
__error_elt (const DOMElement *elt)
{
  string id = "";
  if (dom_element_try_get_attr (id, (elt), "id"))
    id = " id='" + id + "'";
  return "<" + dom_element_tagname (elt) + id + ">";
}

#define ERROR_SYNTAX_ELT(elt, fmt, ...)\
  ERROR_SYNTAX ("%s: " fmt, __error_elt ((elt)).c_str (), ## __VA_ARGS__)

#define ERROR_SYNTAX_ELT_BAD_ATTRIBUTE(elt, name)                       \
  ERROR_SYNTAX_ELT ((elt), "bad value for attribute '%s': '%s'",        \
                    string ((name)).c_str (),                           \
                    dom_element_get_attr ((elt), (name)).c_str ())

#define ERROR_SYNTAX_ELT_MISSING_ATTRIBUTE(elt, name)           \
  ERROR_SYNTAX_ELT ((elt), "missing required attribute '%s'",   \
                    string ((name)).c_str ())

#define ERROR_SYNTAX_ELT_MISSING_ID(elt)\
  ERROR_SYNTAX_ELT ((elt), "missing id")

#define ERROR_SYNTAX_ELT_DUPLICATED_ID(elt, value)      \
  ERROR_SYNTAX_ELT ((elt), "duplicated id '%s'",        \
                    string ((value)).c_str ())

#define ERROR_SYNTAX_ELT_UNKNOWN_CHILD(elt, child)      \
  ERROR_SYNTAX_ELT ((elt), "unknown child element %s",  \
                    __error_elt ((child)).c_str ())

#define CHECK_ELT_TAG(elt, expected, pvalue)                            \
  G_STMT_START                                                          \
  {                                                                     \
    string result = dom_element_tagname ((elt));                        \
    string expect = (expected);                                         \
    if (unlikely (result != expect))                                    \
      ERROR_SYNTAX_ELT ((elt), "bad tagname '%s' (expected '%s')",      \
                        result.c_str (), (expect).c_str ());            \
    set_if_nonnull ((string *)(pvalue), result);                        \
  }                                                                     \
  G_STMT_END

#define CHECK_ELT_ATTRIBUTE(elt, name, pvalue)                          \
  G_STMT_START                                                          \
  {                                                                     \
    string result;                                                      \
    if (unlikely (!dom_element_try_get_attr (result, (elt), (name))))   \
      ERROR_SYNTAX_ELT_MISSING_ATTRIBUTE ((elt), (name));               \
    set_if_nonnull ((pvalue), result);                                  \
  }                                                                     \
  G_STMT_END

#define CHECK_ELT_ATTRIBUTE_NOT_SUPPORTED(elt, name)                    \
  G_STMT_START                                                          \
  {                                                                     \
    if (unlikely (dom_element_has_attr ((elt), (name))))                \
      ERROR_NOT_IMPLEMENTED ("%s: attribute '%s' is not supported",     \
                             __error_elt ((elt)).c_str (),              \
                             string ((name)).c_str ());                 \
  }                                                                     \
  G_STMT_END

#define CHECK_ELT_OPT_ATTRIBUTE(elt, name, pvalue, default)     \
  G_STMT_START                                                  \
  {                                                             \
    string result;                                              \
    if (!dom_element_try_get_attr (result, (elt), (name)))      \
      result = (default);                                       \
    set_if_nonnull ((pvalue), result);                          \
  }                                                             \
  G_STMT_END

#define CHECK_ELT_ID(elt, pvalue)                                       \
  G_STMT_START                                                          \
  {                                                                     \
    string result;                                                      \
    if (unlikely (!dom_element_try_get_attr (result, (elt), "id")))     \
      ERROR_SYNTAX_ELT_MISSING_ID ((elt));                              \
    if (unlikely (_doc->getNode (result) != nullptr))                   \
      ERROR_SYNTAX_ELT_DUPLICATED_ID ((elt), result);                   \
    set_if_nonnull ((pvalue), result);                                  \
  }                                                                     \
  G_STMT_END

#define CHECK_ELT_OPT_ID(elt, pvalue, default)                          \
  G_STMT_START                                                          \
  {                                                                     \
    string result;                                                      \
    if (unlikely (dom_element_try_get_attr (result, (elt), "id")        \
                  && _doc->getNode (result) != nullptr))                \
      {                                                                 \
        ERROR_SYNTAX_ELT_DUPLICATED_ID ((elt), result);                 \
      }                                                                 \
    else                                                                \
      {                                                                 \
        result = (default);                                             \
      }                                                                 \
    set_if_nonnull ((pvalue), result);                                  \
  }                                                                     \
  G_STMT_END


// Translation tables.

static map<string, pair<int,int>> reserved_condition_table =
  {
   {"onBegin",
    {(int) EventType::PRESENTATION,
     (int) EventStateTransition::STARTS}},
   {"onEnd",
    {(int) EventType::PRESENTATION,
     (int) EventStateTransition::STOPS}},
   {"onAbort",
    {(int) EventType::PRESENTATION,
     (int) EventStateTransition::ABORTS}},
   {"onPause",
    {(int) EventType::PRESENTATION,
     (int) EventStateTransition::PAUSES}},
   {"onResumes",
    {(int) EventType::PRESENTATION,
     (int) EventStateTransition::RESUMES}},
   {"onBeginAttribution",
    {(int) EventType::ATTRIBUTION,
     (int) EventStateTransition::STARTS}},
   {"onEndAttribution",
    {(int) EventType::SELECTION,
     (int) EventStateTransition::STOPS}},
   {"onSelection",
    {(int) EventType::SELECTION,
     (int) EventStateTransition::STARTS}},
  };

static map<string, pair<int,int>> reserved_action_table =
  {
   {"start",
    {(int) EventType::PRESENTATION,
     (int) ACT_START}},
   {"stop",
    {(int) EventType::PRESENTATION,
     (int) ACT_STOP}},
   {"abort",
    {(int) EventType::PRESENTATION,
     (int) ACT_ABORT}},
   {"pause",
    {(int) EventType::PRESENTATION,
     (int) ACT_PAUSE}},
   {"resume",
    {(int) EventType::PRESENTATION,
     (int) ACT_RESUME}},
   {"set",
    {(int) EventType::ATTRIBUTION,
     (int) ACT_START}},
  };

static map<string, EventType> event_type_table =
  {
   {"presentation", EventType::PRESENTATION},
   {"attribution", EventType::ATTRIBUTION},
   {"selection", EventType::SELECTION},
  };

static map<string, EventStateTransition> event_transition_table =
  {
   {"starts", EventStateTransition::STARTS},
   {"stops", EventStateTransition::STOPS},
   {"aborts", EventStateTransition::ABORTS},
   {"pauses", EventStateTransition::PAUSES},
   {"resumes", EventStateTransition::RESUMES},
  };

static map<string, SimpleActionType> event_action_type_table =
  {
   {"start", ACT_START},
   {"stop", ACT_STOP},
   {"abort", ACT_ABORT},
   {"pause", ACT_PAUSE},
   {"resume", ACT_RESUME},
  };


// Public.

NclParser::NclParser ()
{
  _doc = nullptr;
}

NclParser::~NclParser ()
{
  for (auto i : _switchConstituents)
    delete i.second;
}

void
NclParser::warning (const SAXParseException &e)
{
  char *file = XMLString::transcode (e.getSystemId ());
  char *errmsg = XMLString::transcode (e.getMessage ());
  if (file == nullptr || strlen (file) == 0)
    {
      g_warning ("%s", errmsg);
    }
  else
    {
      g_warning ("%s:%u.%u: %s", file,
                 (guint) e.getLineNumber (),
                 (guint) e.getColumnNumber (),
                 errmsg);
    }
  XMLString::release (&file);
  XMLString::release (&errmsg);
}

void G_GNUC_NORETURN
NclParser::error (const SAXParseException &e)
{
  char *file = XMLString::transcode (e.getSystemId ());
  char *errmsg = XMLString::transcode (e.getMessage ());
  if (file == nullptr || strlen (file) == 0)
    {
      g_error ("%s", errmsg);
    }
  else
    {
      g_error ("%s:%u.%u: %s", file,
               (guint) e.getLineNumber (),
               (guint) e.getColumnNumber (),
               errmsg);
    }
  XMLString::release (&file);
  XMLString::release (&errmsg);
}

void
NclParser::fatalError (const SAXParseException &e)
{
  this->error (e);
}

NclDocument *
NclParser::parse (const string &path)
{
  DOMDocument *dom;
  DOMElement *elt;
  XercesDOMParser *parser;

  _path = xpathmakeabs (path);
  _dirname = xpathdirname (path);

  XMLPlatformUtils::Initialize ();
  parser = new XercesDOMParser ();
  g_assert_nonnull (parser);

  parser->setValidationScheme (XercesDOMParser::Val_Auto);
  parser->setDoNamespaces (false);
  parser->setDoSchema (false);
  parser->setErrorHandler (this);
  parser->setCreateEntityReferenceNodes (false);

  XMLCh *xmlch = XMLString::transcode (path.c_str ());
  LocalFileInputSource src (xmlch);
  try
    {
      parser->parse (src);
      XMLString::release (&xmlch);
    }
  catch (...)
    {
      g_assert_not_reached ();
    }

  dom = parser->getDocument ();
  g_assert_nonnull (dom);

  elt = (DOMElement *) dom->getDocumentElement ();
  g_assert_nonnull (elt);

  g_assert_null (_doc);
  parseNcl (elt);               // fills _doc
  g_assert_nonnull (_doc);

  delete parser;

  return _doc;
}

NclDocument *
NclParser::importDocument (string &path)
{
  NclParser compiler;

  if (!xpathisuri (path) && !xpathisabs (path))
    path = xpathbuildabs (_dirname, path);

  return compiler.parse (path);
}

// STRUCTURE

void
NclParser::solveNodeReferences (CompositeNode *composition)
{
  NodeEntity *nodeEntity;
  Entity *referredNode;
  vector<Node *> *nodes;
  bool deleteNodes = false;

  if (instanceof (SwitchNode *, composition))
    {
      deleteNodes = true;
      nodes = getSwitchConstituents ((SwitchNode *)composition);
    }
  else
    {
      nodes = composition->getNodes ();
    }

  if (nodes)
    {
      return;
    }

  for (Node *node : *nodes)
  {
    if (node != NULL)
      {
        if (instanceof (ReferNode *, node))
          {
            referredNode = ((ReferNode *)node)->getReferredEntity ();
            if (referredNode != NULL)
              {
                if (instanceof (ReferredNode *, referredNode))
                  {
                    nodeEntity = (NodeEntity *)(_doc->getNode (
                                                  referredNode->getId ()));
                    if (nodeEntity)
                      {
                        ((ReferNode *)node)
                            ->setReferredEntity (nodeEntity
                                                 ->getDataEntity ());
                      }
                    else
                      {
                        ERROR_SYNTAX ("<media '%s'>: bad value to attribute refer '%s'",
                                      node->getId ().c_str (),
                                      referredNode->getId ().c_str ());
                      }
                  }
              }
          }
        else if (instanceof (CompositeNode *, node))
          {
            solveNodeReferences ((CompositeNode *)node);
          }
      }
  }
  if (deleteNodes)
    delete nodes;
}

// COMPONENTS

ContextNode *
NclParser::posCompileContext (DOMElement *context_element, ContextNode *context)
{
  g_assert_nonnull(context);

  for(DOMElement *child: dom_element_children (context_element))
    {
      string tagname = dom_element_tagname(child);
      if (tagname == "context")
        {
          string id = dom_element_get_attr (child, "id");
          Node *node = context->getNode (id);

          if (unlikely (node == NULL))
            {
              ERROR_SYNTAX ("bad context '%s'", id.c_str ());
            }
          else if (instanceof (ContextNode *, node))
            {
              posCompileContext (child, (ContextNode*)node);
            }
        }
      else if (tagname == "switch")
        {
          string id = dom_element_get_attr(child, "id");
          Node *node = _doc->getNode (id);

          if (unlikely (node == NULL))
            {
              ERROR_SYNTAX ("bad switch '%s'", id.c_str ());
            }
          else if (instanceof (SwitchNode *, node))
            {
              this->posCompileSwitch (child, (SwitchNode*)node);
            }
        }
    }

  for(DOMElement *child:
      dom_element_children_by_tagname (context_element, "link"))
    {
      Link *link = this->parseLink (child, context);
      g_assert_nonnull (link);
      context->addLink (link);
    }

  for(DOMElement *child:
      dom_element_children_by_tagname (context_element, "port"))
    {
      Port *port = this->parsePort (child, context);
      if (port)
        {
          context->addPort (port);
        }
    }
  return context;
}

// INTERFACES
SwitchPort *
NclParser::parseSwitchPort (DOMElement *switchPort_element,
                            SwitchNode *switchNode)
{
  SwitchPort *switchPort = createSwitchPort (switchPort_element, switchNode);

  if (unlikely (switchPort == NULL))
    {
      ERROR_SYNTAX ("switchPort: bad parent '%s'",
                    dom_element_tagname(switchPort_element).c_str ());
    }

    for(DOMElement *child:
        dom_element_children_by_tagname(switchPort_element, "mapping"))
      {
        Port *mapping = parseMapping (child, switchPort);
        if (mapping)
          {
            switchPort->addPort (mapping);
          }
      }

  return switchPort;
}

Port *
NclParser::parseMapping (DOMElement *parent, SwitchPort *switchPort)
{
  DOMElement *switchElement;
  SwitchNode *switchNode;
  NodeEntity *mappingNodeEntity;
  Node *mappingNode;
  InterfacePoint *interfacePoint;

  // FIXME: this is not safe!
  switchElement = (DOMElement *) parent->getParentNode ()->getParentNode ();

  string id = dom_element_get_attr(switchElement, "id");
  string component = dom_element_get_attr(parent, "component");

  // FIXME: this is not safe!
  switchNode = (SwitchNode *) _doc->getNode (id);
  mappingNode = switchNode->getNode (component);

  if (unlikely (mappingNode == NULL))
    ERROR_SYNTAX ("mapping: bad component '%s'", component.c_str ());

  // FIXME: this is not safe!
  mappingNodeEntity = (NodeEntity *) mappingNode->getDataEntity ();

  string interface;
  if (dom_element_try_get_attr(interface, parent, "interface"))
    {
      interfacePoint = mappingNodeEntity->getAnchor (interface);
      if (interfacePoint == NULL)
        {
          if (instanceof (CompositeNode *, mappingNodeEntity))
            {
              interfacePoint = ((CompositeNode *) mappingNodeEntity)
                ->getPort (interface);
            }
        }
    }
  else
    {
      interfacePoint = mappingNodeEntity->getAnchor (0);
    }

  if (unlikely (interfacePoint == NULL))
    ERROR_SYNTAX ("mapping: bad interface '%s'", interface.c_str ());

  return new Port (switchPort->getId (), mappingNode, interfacePoint);
}

SwitchPort *
NclParser::createSwitchPort (DOMElement *parent,
                                       SwitchNode *switchNode)
{
  SwitchPort *switchPort;
  string id;

  if (unlikely (!dom_element_has_attr(parent, "id")))
    ERROR_SYNTAX ("switchPort: missing id");

  id = dom_element_get_attr(parent, "id");

  if (unlikely (switchNode->getPort (id) != NULL))
    ERROR_SYNTAX ("switchPort '%s': duplicated id", id.c_str ());

  switchPort = new SwitchPort (id, switchNode);
  return switchPort;
}

// PRESENTATION_CONTROL

DescriptorSwitch *
NclParser::parseDescriptorSwitch (DOMElement *elt)
{
  DescriptorSwitch *descriptorSwitch;

  descriptorSwitch = createDescriptorSwitch (elt);
  g_assert_nonnull (descriptorSwitch);

  for (DOMElement *child: dom_element_children (elt))
    {
      string tagname = dom_element_tagname (child);
      if ( tagname == "descriptor")
        {
          Descriptor* desc = this->parseDescriptor (child);
          g_assert_nonnull (desc);
          addDescriptorToDescriptorSwitch (descriptorSwitch, desc);
        }
      else
        {
          ERROR_SYNTAX_ELT_UNKNOWN_CHILD (elt, child);
        }
    }

  for (DOMElement *child: dom_element_children (elt))
    {
      string tagname = dom_element_tagname (child);
      if (tagname == "bindRule")
        {
          addBindRuleToDescriptorSwitch (descriptorSwitch, child);
        }
      else if (tagname == "defaultDescriptor")
        {
          addDefaultDescriptorToDescriptorSwitch (descriptorSwitch, child);
        }
    }
  return descriptorSwitch;
}

vector<Node *> *
NclParser::getSwitchConstituents (SwitchNode *switchNode)
{
  map<string, map<string, Node *> *>::iterator i;
  map<string, Node *> *hTable;
  map<string, Node *>::iterator j;

  vector<Node *> *ret = new vector<Node *>;

  i = _switchConstituents.find (switchNode->getId ());
  if (i != _switchConstituents.end ())
    {
      hTable = i->second;

      j = hTable->begin ();
      while (j != hTable->end ())
        {
          ret->push_back ((Node *)j->second);
          ++j;
        }
    }

  // Users: you have to delete this vector after using it
  return ret;
}

DescriptorSwitch *
NclParser::createDescriptorSwitch (DOMElement *parentElement)
{
  DescriptorSwitch *descriptorSwitch =
      new DescriptorSwitch (dom_element_get_attr(parentElement, "id"));

  // vetores para conter componentes e regras do switch
  _switchConstituents[descriptorSwitch->getId ()] = new map<string, Node *>;

  return descriptorSwitch;
}

void
NclParser::addDescriptorToDescriptorSwitch (
    DescriptorSwitch *descriptorSwitch, GenericDescriptor *descriptor)
{
  map<string, Node *> *descriptors;
  try
    {
      if (_switchConstituents.count (descriptorSwitch->getId ()) != 0)
        {
          descriptors = _switchConstituents[descriptorSwitch->getId ()];

          if (descriptors->count (descriptor->getId ()) == 0)
            {
              (*descriptors)[descriptor->getId ()] = (NodeEntity *)descriptor;
            }
        }
    }
  catch (...)
    {
    }
}

void
NclParser::addBindRuleToDescriptorSwitch (
    DescriptorSwitch *descriptorSwitch, DOMElement *bindRule_element)
{
  map<string, Node *> *descriptors;
  GenericDescriptor *descriptor;
  NclDocument *ncldoc;
  Rule *ncmRule;

  if (_switchConstituents.count (descriptorSwitch->getId ()) == 0)
    {
      return;
    }
  descriptors = _switchConstituents[descriptorSwitch->getId ()];
  string constituent = dom_element_get_attr(bindRule_element, "constituent");
  if (descriptors->count (constituent) == 0)
    {
      return;
    }

  descriptor = (GenericDescriptor *)(*descriptors)[constituent];
  if (descriptor == NULL)
    {
      return;
    }

  ncldoc = this->_doc;
  ncmRule = ncldoc->getRule (dom_element_get_attr(bindRule_element, "rule"));
  if (ncmRule == NULL)
    {
      return;
    }

  descriptorSwitch->addDescriptor (descriptor, ncmRule);
}

void
NclParser::addBindRuleToSwitch (SwitchNode *switchNode, DOMElement *bindRule)
{
  map<string, Node *> *nodes;
  Node *node;
  NclDocument *ncldoc;
  Rule *ncmRule;

  if (_switchConstituents.count (switchNode->getId ()) == 0)
    {
      return;
    }

  nodes = _switchConstituents[switchNode->getId ()];
  if (nodes->count (dom_element_get_attr(bindRule, "constituent"))
      == 0)
    {
      return;
    }

  node = (NodeEntity *)(*nodes)[dom_element_get_attr(bindRule, "constituent") ];

  if (node == NULL)
    {
      return;
    }

  ncldoc = this->_doc;
  ncmRule = ncldoc->getRule (dom_element_get_attr(bindRule, "rule"));

  if (ncmRule == NULL)
    {
      return;
    }

  switchNode->addNode (node, ncmRule);
}

void
NclParser::addUnmappedNodesToSwitch (SwitchNode *switchNode)
{
  map<string, Node *> *nodes;
  map<string, Node *>::iterator i;

  if (_switchConstituents.count (switchNode->getId ()) == 0)
    {
      return;
    }

  nodes = _switchConstituents[switchNode->getId ()];
  i = nodes->begin ();
  while (i != nodes->end ())
    {
      if (switchNode->getNode (i->second->getId ()) == NULL)
        {
          switchNode->addNode (i->second, new Rule ("fake"));
        }
      else
        {
          i->second->setParentComposition (switchNode);
        }
      ++i;
    }
}

void
NclParser::addDefaultComponentToSwitch (
    SwitchNode *switchNode, DOMElement *defaultComponent)
{
  map<string, Node *> *nodes;
  Node *node;

  if (_switchConstituents.count (switchNode->getId ()) == 0)
    {
      return;
    }

  nodes = _switchConstituents[switchNode->getId ()];
  string component = dom_element_get_attr(defaultComponent, "component");
  if (nodes->count (component) == 0)
    {
      return;
    }

  node = (*nodes)[component];

  if (node == NULL)
    {
      return;
    }

  switchNode->setDefaultNode (node);
}

void
NclParser::addDefaultDescriptorToDescriptorSwitch (
    DescriptorSwitch *descriptorSwitch, DOMElement *defaultDescriptor)
{
  map<string, Node *> *descriptors;
  GenericDescriptor *descriptor;

  if (_switchConstituents.count (descriptorSwitch->getId ()) == 0)
    {
      return;
    }

  descriptors = _switchConstituents[descriptorSwitch->getId ()];
  if (descriptors->count (
        dom_element_get_attr(defaultDescriptor, "descriptor"))
      == 0)
    {
      return;
    }

  descriptor = (GenericDescriptor *)(*descriptors)[
      dom_element_get_attr(defaultDescriptor, "descriptor") ];

  if (descriptor == NULL)
    {
      return;
    }

  descriptorSwitch->setDefaultDescriptor (descriptor);
}

SwitchNode *
NclParser::posCompileSwitch (
    DOMElement *switchElement, SwitchNode *switchNode)
{
  for(DOMElement *child: dom_element_children(switchElement))
    {
      string tagname = dom_element_tagname(child);
      if (tagname == "context")
        {
          string id = dom_element_get_attr(child, "id");
          Node *node = _doc->getNode (id);

          if (instanceof (ContextNode *, node))
            {
              this->posCompileContext (child, (ContextNode*)node);
            }
        }
      else if (tagname ==  "switch")
        {
          string id = dom_element_get_attr(child, "id");
          Node * node = _doc->getNode (id);
          if (unlikely (node == NULL))
            {
              ERROR_SYNTAX ("node '%s' should be a switch",
                            dom_element_get_attr(child, "id").c_str ());
            }
          else if (instanceof (SwitchNode *, node))
            {
              posCompileSwitch (child, (SwitchNode*)node);
            }
        }
      else
        {
          // syntax_err/warn ??
        }
    }

  for (DOMElement *child: dom_element_children(switchElement))
    {
      string tagname = dom_element_tagname(child);
      if (tagname == "switchPort")
        {
          SwitchPort *switchPort = this->parseSwitchPort (child, switchNode);
          if (switchPort)
            {
              switchNode->addPort (switchPort);
            }
        }
    }

  return switchNode;
}


// -------------------------------------------------------------------------

void
NclParser::parseNcl (DOMElement *elt)
{
  string id;

  CHECK_ELT_TAG (elt, "ncl", nullptr);
  CHECK_ELT_OPT_ATTRIBUTE (elt, "id", &id, "ncl");

  _doc = new NclDocument (id, _path);
  for (DOMElement *child: dom_element_children (elt))
    {
      string tag = dom_element_tagname (child);
      if (tag == "head")
        {
          this->parseHead (child);
        }
      else if (tag == "body")
        {
          ContextNode *body = this->parseBody (child);
          g_assert_nonnull (body);
          this->solveNodeReferences (body);
          this->posCompileContext (child, body);
        }
      else
        {
          ERROR_SYNTAX_ELT_UNKNOWN_CHILD (elt, child);
        }
    }
}


// Head.

void
NclParser::parseHead (DOMElement *elt)
{
  CHECK_ELT_TAG (elt, "head", nullptr);

  for (DOMElement *child: dom_element_children (elt))
    {
      string tag = dom_element_tagname (child);
      if (tag == "importedDocumentBase")
        {
          this->parseImportedDocumentBase (child);
        }
      else if (tag == "ruleBase")
        {
          RuleBase *base = this->parseRuleBase (child);
          g_assert_nonnull (base);
          _doc->setRuleBase (base);
        }
      else if (tag == "transitionBase")
        {
          TransitionBase *base = this->parseTransitionBase (child);
          g_assert_nonnull (base);
          _doc->setTransitionBase (base);
        }
      else if (tag == "regionBase")
        {
          RegionBase *base = this->parseRegionBase (child);
          g_assert_nonnull (base);
          _doc->addRegionBase (base);
        }
      else if (tag == "descriptorBase")
        {
          DescriptorBase *base = this->parseDescriptorBase (child);
          g_assert_nonnull (base);
          _doc->setDescriptorBase (base);
        }
      else if (tag == "connectorBase")
        {
          ConnectorBase *base = this->parseConnectorBase (child);
          g_assert_nonnull (base);
          _doc->setConnectorBase (base);
        }
      else
        {
          ERROR_SYNTAX_ELT_UNKNOWN_CHILD (elt, child);
        }
    }
}


// Private: Import.

NclDocument *
NclParser::parseImportNCL (DOMElement *elt, string *alias, string *uri)
{
  g_assert_nonnull (alias);
  g_assert_nonnull (uri);

  CHECK_ELT_TAG (elt, "importNCL", nullptr);
  CHECK_ELT_ATTRIBUTE (elt, "alias", alias);
  CHECK_ELT_ATTRIBUTE (elt, "documentURI", uri);

  return this->importDocument (*uri);
}

Base *
NclParser::parseImportBase (DOMElement *elt, NclDocument **doc,
                            string *alias, string *uri)
{
  DOMElement *parent;
  string tag;

  g_assert_nonnull (doc);
  g_assert_nonnull (alias);
  g_assert_nonnull (uri);

  CHECK_ELT_TAG (elt, "importBase", nullptr);
  CHECK_ELT_ATTRIBUTE (elt, "alias", alias);
  CHECK_ELT_ATTRIBUTE (elt, "documentURI", uri);

  *doc = this->importDocument (*uri);
  g_assert_nonnull (*doc);

  parent = (DOMElement*) elt->getParentNode ();
  g_assert_nonnull (parent);

  tag = dom_element_tagname (parent);
  if (tag == "ruleBase")
    return (*doc)->getRuleBase ();
  else if (tag == "transitionBase")
    return (*doc)->getTransitionBase ();
  else if (tag == "regionBase")
    return (*doc)->getRegionBase (0);
  else if (tag == "descriptorBase")
    return (*doc)->getDescriptorBase ();
  else if (tag == "connectorBase")
    return (*doc)->getConnectorBase ();
  else
    g_assert_not_reached ();
}

void
NclParser::parseImportedDocumentBase (DOMElement *elt)
{
  for (DOMElement *child: dom_element_children (elt))
    {
      string tag = dom_element_tagname (elt);
      if (tag == "importNCL")
        {
          NclDocument *imported;
          string alias;
          string uri;
          imported = this->parseImportNCL (child, &alias, &uri);
          g_assert_nonnull (imported);
          _doc->addDocument (imported, alias, uri);
        }
      else
        {
          ERROR_SYNTAX_ELT_UNKNOWN_CHILD (elt, child);
        }
    }
}

// Private: Rule.

RuleBase *
NclParser::parseRuleBase (DOMElement *elt)
{
  RuleBase *base;
  string id;

  CHECK_ELT_TAG (elt, "ruleBase", nullptr);
  CHECK_ELT_OPT_ID (elt, &id, "");

  base = new RuleBase (id);
  for (DOMElement *child: dom_element_children (elt))
    {
      string tag = dom_element_tagname (child);
      if ( tag == "importBase")
        {
          NclDocument *doc;     // FIXME: this is lost (leak?)
          Base *imported;
          string alias;
          string uri;
          imported = this->parseImportBase (child, &doc, &alias, &uri);
          g_assert_nonnull (imported);
          base->addBase (imported, alias, uri);
        }
      else if (tag == "rule")
        {
          base->addRule (this->parseRule (child));
        }
      else if (tag == "compositeRule")
        {
          base->addRule (this->parseCompositeRule (child));
        }
      else
        {
          ERROR_SYNTAX_ELT_UNKNOWN_CHILD (elt, child);
        }
    }
  return base;
}

CompositeRule *
NclParser::parseCompositeRule (DOMElement *elt)
{
  CompositeRule *rule;
  string id;
  string value;
  short op;

  CHECK_ELT_TAG (elt, "compositeRule", nullptr);
  CHECK_ELT_ID (elt, &id);

  CHECK_ELT_ATTRIBUTE (elt, "operator", &value);
  if (value == "and")
    op = CompositeRule::OP_AND;
  else if (value == "or")
    op = CompositeRule::OP_OR;
  else
    ERROR_SYNTAX_ELT_BAD_ATTRIBUTE (elt, "operator");

  rule = new CompositeRule (id, op);
  for (DOMElement *child: dom_element_children (elt))
    {
      string tag = dom_element_tagname (child);
      if (tag == "rule")
        {
          rule->addRule (this->parseRule (child));
        }
      else if (tag == "compositeRule")
        {
          rule->addRule (this->parseCompositeRule (child));
        }
      else
        {
          ERROR_SYNTAX_ELT_UNKNOWN_CHILD (elt, child);
        }
    }
  return rule;
}

SimpleRule *
NclParser::parseRule (DOMElement *elt)
{
  string id;
  string var;
  string comp;
  string value;
  CHECK_ELT_TAG (elt, "rule", nullptr);
  CHECK_ELT_OPT_ID (elt, &id, "");
  CHECK_ELT_ATTRIBUTE (elt, "var", &var);
  CHECK_ELT_ATTRIBUTE (elt, "comparator", &value);
  CHECK_ELT_ATTRIBUTE (elt, "value", &value);
  return new SimpleRule (id, var, Comparator::fromString (comp), value);
}

// Private: Transition.

TransitionBase *
NclParser::parseTransitionBase (DOMElement *elt)
{
  TransitionBase *base;
  string id;

  CHECK_ELT_TAG (elt, "transitionBase", nullptr);
  CHECK_ELT_OPT_ID (elt, &id, "");

  base = new TransitionBase (id);
  for(DOMElement *child: dom_element_children (elt))
    {
      string tag = dom_element_tagname (child);
      if (tag == "importBase")
        {
          NclDocument *doc;     // FIXME: this is lost (leak?)
          Base *imported;
          string alias;
          string uri;
          imported = this->parseImportBase (child, &doc, &alias, &uri);
          g_assert_nonnull (imported);
          base->addBase (imported, alias, uri);
        }
      else if (tag == "transition")
        {
          Transition *trans = parseTransition (child);
          g_assert_nonnull (trans);
          base->addTransition (trans);
        }
      else
        {
          ERROR_SYNTAX_ELT_UNKNOWN_CHILD (elt, child);
        }
    }
  return base;
}

Transition *
NclParser::parseTransition (DOMElement *elt)
{
  Transition *trans;
  string id;
  string value;
  int type;

  CHECK_ELT_TAG (elt, "transition", nullptr);
  CHECK_ELT_ID (elt, &id);

  CHECK_ELT_ATTRIBUTE (elt, "type", &value);
  type = TransitionUtil::getTypeCode (value);
  if (unlikely (type < 0))
    ERROR_SYNTAX_ELT_BAD_ATTRIBUTE (elt, "type");

  trans = new Transition (id, type);

  if (dom_element_try_get_attr (value, elt, "subtype"))
    {
      int subtype = TransitionUtil::getSubtypeCode (type, value);
      trans->setSubtype (CLAMP (subtype, 0, G_MAXINT));
    }

  if (dom_element_try_get_attr (value, elt, "dur"))
    trans->setDuration (ginga_parse_time (value));

  if (dom_element_try_get_attr (value, elt, "startProgress"))
    trans->setStartProgress (xstrtod (value));

  if (dom_element_try_get_attr (value, elt, "endProgress"))
    trans->setEndProgress (xstrtod (value));

  if (dom_element_try_get_attr (value, elt, "direction"))
    {
      int dir = TransitionUtil::getDirectionCode (value);
      trans->setDirection ((short) CLAMP (dir, 0, G_MAXINT));
    }

  if (dom_element_try_get_attr (value, elt, "fadeColor"))
    trans->setFadeColor (ginga_parse_color (value));

  if (dom_element_try_get_attr (value, elt, "horzRepeat"))
    trans->setHorzRepeat (xstrtoint (value, 10));

  if (dom_element_try_get_attr (value, elt, "vertRepeat"))
    trans->setVertRepeat (xstrtoint (value, 10));

  if (dom_element_try_get_attr (value, elt, "borderWidth"))
    trans->setBorderWidth (xstrtoint (value, 10));

  if (dom_element_try_get_attr (value, elt, "borderColor"))
    trans->setBorderColor (ginga_parse_color (value));

  return trans;
}


// Private: Region.

RegionBase *
NclParser::parseRegionBase (DOMElement *elt)
{
  RegionBase *base;
  string id;

  CHECK_ELT_TAG (elt, "regionBase", nullptr);
  CHECK_ELT_OPT_ID (elt, &id, "");

  base = new RegionBase (id);
  for (DOMElement *child: dom_element_children (elt))
    {
      string tag = dom_element_tagname (child);
      if (tag == "importBase")
        {
          NclDocument *doc;     // FIXME: this is lost (leak?)
          Base *imported;
          string alias;
          string uri;
          imported = this->parseImportBase (child, &doc, &alias, &uri);
          g_assert_nonnull (imported);
          base->addBase (imported, alias, uri);
        }
      else if (tag == "region")
        {
          LayoutRegion *region = this->parseRegion (child, base, nullptr);
          g_assert_nonnull (region);
          base->addRegion (region);
        }
      else
        {
          ERROR_SYNTAX_ELT_UNKNOWN_CHILD (elt, child);
        }
    }
  return base;
}

LayoutRegion *
NclParser::parseRegion (DOMElement *elt, RegionBase *base,
                        LayoutRegion *parent)
{
  LayoutRegion *region;
  string id;
  string value;

  SDL_Rect parent_rect;
  SDL_Rect rect;
  int z;
  int zorder;
  static int last_zorder = 0;

  CHECK_ELT_TAG (elt, "region", nullptr);
  CHECK_ELT_ID (elt, &id);

  region = new LayoutRegion (id);
  if (parent != NULL)
    {
      parent_rect = parent->getRect ();
    }
  else
    {
      parent_rect.x = 0;
      parent_rect.y = 0;
      Ginga_Display->getSize (&parent_rect.w, &parent_rect.h);
    }

  rect = parent_rect;
  z = zorder = 0;

  if (dom_element_try_get_attr (value, elt, "left"))
    rect.x += ginga_parse_percent (value, parent_rect.w, 0, G_MAXINT);

  if (dom_element_try_get_attr (value, elt, "top"))
    rect.y += ginga_parse_percent (value, parent_rect.h, 0, G_MAXINT);

  if (dom_element_try_get_attr (value, elt, "width"))
    rect.w = ginga_parse_percent (value, parent_rect.w, 0, G_MAXINT);

  if (dom_element_try_get_attr (value, elt, "height"))
    rect.h = ginga_parse_percent (value, parent_rect.h, 0, G_MAXINT);

  if (dom_element_try_get_attr (value, elt, "right"))
    {
    rect.x += parent_rect.w - rect.w
      - ginga_parse_percent (value, parent_rect.w, 0, G_MAXINT);
    }

  if (dom_element_try_get_attr (value, elt, "bottom"))
    {
      rect.y += parent_rect.h - rect.h
        - ginga_parse_percent (value, parent_rect.h, 0, G_MAXINT);
    }

  if (dom_element_try_get_attr (value, elt, "zIndex"))
    z = xstrtoint (value, 10);
  zorder = last_zorder++;

  region->setRect (rect);
  region->setZ (z, zorder);

  // Collect children.
  for (DOMElement *child: dom_element_children (elt))
    {
      string tag = dom_element_tagname (child);
      if (tag == "region")
        base->addRegion (this->parseRegion (child, base, region));
      else
        ERROR_SYNTAX_ELT_UNKNOWN_CHILD (elt, child);
    }
  return region;
}


// Private: Descriptor.

DescriptorBase *
NclParser::parseDescriptorBase (DOMElement *elt)
{
  DescriptorBase *base;
  string id;

  CHECK_ELT_TAG (elt, "descriptorBase", nullptr);
  CHECK_ELT_OPT_ID (elt, &id, "");

  base = new DescriptorBase (id);
  for (DOMElement *child: dom_element_children (elt))
    {
      string tag = dom_element_tagname (child);
      if (tag == "importBase")
        {
          NclDocument *doc;     // FIXME: this is lost (leak?)
          Base *imported;
          string alias;
          string uri;

          imported = this->parseImportBase (child, &doc, &alias, &uri);
          g_assert_nonnull (imported);
          base->addBase (imported, alias, uri);

          // Import regions.
          RegionBase *regionBase = _doc->getRegionBase (0);
          if (regionBase == nullptr)
            {
              regionBase = new RegionBase ("");
              _doc->addRegionBase (regionBase);
            }
          for (auto item: *doc->getRegionBases ())
            regionBase->addBase (item.second, alias, uri);

          // Import rules.
          RuleBase *ruleBase = _doc->getRuleBase ();
          if (ruleBase == nullptr)
            {
              ruleBase = new RuleBase ("");
              _doc->setRuleBase (ruleBase);
            }
          ruleBase->addBase (doc->getRuleBase (), alias, uri);
        }
      else if (tag == "descriptorSwitch")
        {
          DescriptorSwitch *descSwitch;
          descSwitch = this->parseDescriptorSwitch (child);
          g_assert_nonnull (descSwitch);
          base->addDescriptor (descSwitch);
        }
      else if (tag == "descriptor")
        {
          Descriptor *desc = parseDescriptor (child);
          g_assert_nonnull (desc);
          base->addDescriptor (desc);
        }
      else
        {
          ERROR_SYNTAX_ELT_UNKNOWN_CHILD (elt, child);
        }
    }
  return base;
}

Descriptor *
NclParser::parseDescriptor (DOMElement *elt)
{
  // List of attributes that should be collected as parameters.
  static vector<string> supported =
    {
     "explicitDur",
     "focusBorderColor",
     "focusBorderTransparency",
     "focusBorderWidth",
     "focusIndex",
     "focusSelSrc",
     "focusSrc",
     "freeze",
     "moveDown",
     "moveLeft",
     "moveRight",
     "moveUp",
     "player",
     "selBorderColor",
    };

  // List of transition attributes.
  static vector<string> transattr = {"transIn", "transOut"};

  Descriptor *desc;
  string id;
  string value;

  CHECK_ELT_TAG (elt, "descriptor", nullptr);
  CHECK_ELT_ID (elt, &id);

  desc = new Descriptor (id);
  if (dom_element_try_get_attr (value, elt, "region"))
    {
      LayoutRegion *region = _doc->getRegion (value);
      if (unlikely (region == nullptr))
        ERROR_SYNTAX_ELT_BAD_ATTRIBUTE (elt, "region");
      desc->setRegion (region);
    }

  for (auto attr: supported)
    {
      if (dom_element_try_get_attr (value, elt, attr))
        desc->addParameter (new Parameter (attr, value));
    }

  for (auto attr: transattr)
    {
      TransitionBase *base;

      if (!dom_element_try_get_attr (value, elt, attr))
        continue;

      base = _doc->getTransitionBase ();
      if (base == nullptr)
        continue;

      vector<string> ids = ginga_parse_list (value, ';', 0, G_MAXINT);
      for (size_t i = 0; i < ids.size (); i++)
        {
          Transition *trans = base->getTransition (ids[i]);
          if (unlikely (trans == nullptr))
            ERROR_SYNTAX_ELT_BAD_ATTRIBUTE (elt, attr);

          if (attr == "transIn")
            desc->addInputTransition (trans, (int) i);
          else
            desc->addOutputTransition (trans, (int) i);
        }
    }

  // Collect children.
  for (DOMElement *child: dom_element_children (elt))
    {
      string tag = dom_element_tagname (child);
      if (tag == "descriptorParam")
        {
          string name;
          string value;
          CHECK_ELT_ATTRIBUTE (child, "name", &name);
          CHECK_ELT_OPT_ATTRIBUTE (child, "value", &value, "");
          desc->addParameter (new Parameter (name, value));
        }
      else
        {
          ERROR_SYNTAX_ELT_UNKNOWN_CHILD (elt, child);
        }
    }
  return desc;
}


// Private: Connector.

ConnectorBase *
NclParser::parseConnectorBase (DOMElement *elt)
{
  ConnectorBase *base;
  string id;

  CHECK_ELT_TAG (elt, "connectorBase", nullptr);
  CHECK_ELT_OPT_ID (elt, &id, "");

  base = new ConnectorBase (id);
  for (DOMElement *child: dom_element_children (elt))
    {
      string tag = dom_element_tagname (child);
      if (tag == "importBase")
        {
          NclDocument *doc;     // FIXME: this is lost (leak?)
          Base *imported;
          string alias;
          string uri;
          imported = this->parseImportBase (child, &doc, &alias, &uri);
          g_assert_nonnull (base);
          base->addBase (imported, alias, uri);
        }
      else if (tag ==  "causalConnector")
        {
          CausalConnector *conn = parseCausalConnector (child);
          g_assert_nonnull (conn);
          base->addConnector (conn);
        }
      else
        {
          ERROR_SYNTAX_ELT_UNKNOWN_CHILD (elt, child);
        }
    }
  return base;
}

CausalConnector *
NclParser::parseCausalConnector (DOMElement *elt)
{
  CausalConnector *conn;
  string id;
  int ncond;
  int nact;

  CHECK_ELT_TAG (elt, "causalConnector", nullptr);
  CHECK_ELT_ID (elt, &id);

  ncond = 0;
  nact = 0;

  conn = new CausalConnector (id);
  for (DOMElement *child: dom_element_children (elt))
    {
      string tag = dom_element_tagname (child);
      if (tag == "simpleCondition")
        {
          conn->setConditionExpression
            (this->parseSimpleCondition (child));
          ncond++;
        }
      else if (tag == "compoundCondition")
        {
          conn->setConditionExpression
            (this->parseCompoundCondition (child));
          ncond++;
        }
      else if (tag == "simpleAction")
        {
          conn->setAction (this->parseSimpleAction (child));
          nact++;
        }
      else if (tag == "compoundAction")
        {
          conn->setAction (this->parseCompoundAction (child));
          nact++;
        }
      else if (tag == "connectorParam")
        {
          string name;
          string type;
          CHECK_ELT_ATTRIBUTE (child, "name", &name);
          CHECK_ELT_OPT_ATTRIBUTE (child, "type", &type, "");
          conn->addParameter (new Parameter (name, type));
        }
      else
        {
          ERROR_SYNTAX_ELT_UNKNOWN_CHILD (elt, child);
        }
      if (unlikely (ncond > 1))
        ERROR_SYNTAX_ELT (elt, "too many conditions");
      if (unlikely (nact > 1))
        ERROR_SYNTAX_ELT (elt, "too many actions");
    }
  return conn;
}

CompoundCondition *
NclParser::parseCompoundCondition (DOMElement *elt)
{
  CompoundCondition *cond;
  string op;
  string value;

  CHECK_ELT_TAG (elt, "compoundCondition", nullptr);
  CHECK_ELT_ATTRIBUTE (elt, "operator", &op);

  cond = new CompoundCondition ();
  if (op == "and")
      cond->setOperator (CompoundCondition::OP_AND);
  else if (op == "or")
    cond->setOperator (CompoundCondition::OP_OR);
  else
    ERROR_SYNTAX_ELT_BAD_ATTRIBUTE (elt, "operator");

  if (dom_element_try_get_attr (value, elt, "delay"))
    cond->setDelay (value);

  // Collect children.
  for (DOMElement *child: dom_element_children (elt))
    {
      string tag = dom_element_tagname (child);
      if (tag == "simpleCondition")
        {
          cond->addConditionExpression
            (this->parseSimpleCondition (child));
        }
      else if (tag == "assessmentStatement")
        {
          cond->addConditionExpression
            (this->parseAssessmentStatement (child));
        }
      else if (tag == "compoundCondition")
        {
          cond->addConditionExpression
            (this->parseCompoundCondition (child));
        }
      else if (tag ==  "compoundStatement")
        {
          cond->addConditionExpression
            (this->parseCompoundStatement (child));
        }
      else
        {
          ERROR_SYNTAX_ELT_UNKNOWN_CHILD (elt, child);
        }
    }
  return cond;
}

SimpleCondition *
NclParser::parseSimpleCondition (DOMElement *elt)
{
  SimpleCondition *cond;
  string role;
  string value;

  EventType type;
  EventStateTransition trans;
  map<string, pair<int,int>>::iterator it;

  CHECK_ELT_TAG (elt, "simpleCondition", nullptr);
  CHECK_ELT_ATTRIBUTE (elt, "role", &role);

  type = EventType::UNKNOWN;
  trans = EventStateTransition::UNKNOWN;

  if ((it = reserved_condition_table.find (role))
      != reserved_condition_table.end ())
    {
      type = (EventType) it->second.first;
      trans = (EventStateTransition) it->second.second;
    }

  if (dom_element_try_get_attr (value, elt, "eventType"))
    {
      if (unlikely (type != EventType::UNKNOWN))
        {
          ERROR_SYNTAX_ELT (elt, "eventType of '%s' cannot be overridden",
                            role.c_str ());
        }
      map<string, EventType>::iterator it;
      if ((it = event_type_table.find (value)) == event_type_table.end ())
        {
          ERROR_SYNTAX_ELT (elt, "bad eventType '%s' for role '%s'",
                            value.c_str (), role.c_str ());
        }
      type = it->second;
    }

  if (dom_element_try_get_attr (value, elt, "transition"))
    {
      if (unlikely (trans != EventStateTransition::UNKNOWN))
        {
          ERROR_SYNTAX_ELT (elt, "transition of '%s' cannot be overridden",
                            role.c_str ());
        }
      map<string, EventStateTransition>::iterator it;
      if ((it = event_transition_table.find (value))
          == event_transition_table.end ())
        {
          ERROR_SYNTAX_ELT (elt, "bad transition '%s' for role '%s'",
                            value.c_str (), role.c_str ());
        }
      trans = it->second;
    }

  g_assert (type != EventType::UNKNOWN);
  g_assert (trans != EventStateTransition::UNKNOWN);

  cond = new SimpleCondition (role);
  cond->setEventType (type);
  cond->setTransition (trans);

  if (type == EventType::SELECTION
      && dom_element_try_get_attr (value, elt, "key"))
    {
      cond->setKey (value);
    }

  if (dom_element_try_get_attr (value, elt, "delay"))
    cond->setDelay (value);

  if (dom_element_try_get_attr (value, elt, "qualifier"))
    {
      if (value == "or")
        cond->setQualifier (CompoundCondition::OP_OR);
      else if (value == "and")
        cond->setQualifier (CompoundCondition::OP_AND);
      else
        ERROR_SYNTAX_ELT_BAD_ATTRIBUTE (elt, "qualifier");
    }

  return cond;
}

CompoundStatement *
NclParser::parseCompoundStatement (DOMElement *elt)
{
  CompoundStatement *stmt;
  string op;
  string neg;

  CHECK_ELT_TAG (elt, "compoundStatement", nullptr);
  CHECK_ELT_ATTRIBUTE (elt, "operator", &op);
  CHECK_ELT_OPT_ATTRIBUTE (elt, "isNegated", &neg, "false");

  stmt = new CompoundStatement ();

  if (op == "and")
    stmt->setOperator (CompoundStatement::OP_AND);
  else if (op == "or")
    stmt->setOperator (CompoundStatement::OP_OR);
  else
    ERROR_SYNTAX_ELT_BAD_ATTRIBUTE (elt, "operator");

  if (neg == "true")
    stmt->setNegated (true);
  else if (neg == "false")
    stmt->setNegated (false);
  else
    ERROR_SYNTAX_ELT_BAD_ATTRIBUTE (elt, "isNegated");

  // Collect children.
  for (DOMElement *child: dom_element_children (elt))
    {
      string tag = dom_element_tagname (child);
      if (tag == "assessmentStatement")
        {
          stmt->addStatement (this->parseAssessmentStatement (child));
        }
      else if (tag == "compoundStatement")
        {
          stmt->addStatement (this->parseCompoundStatement (child));
        }
      else
        {
          ERROR_SYNTAX_ELT_UNKNOWN_CHILD (elt, child);
        }
    }
  return stmt;
}

AssessmentStatement *
NclParser::parseAssessmentStatement (DOMElement *elt)
{
  AssessmentStatement *stmt;
  string comp;
  string value;

  CHECK_ELT_TAG (elt, "assessmentStatement", nullptr);
  CHECK_ELT_OPT_ATTRIBUTE (elt, "comparator", &comp, "eq");

  stmt = new AssessmentStatement (Comparator::fromString (comp));
  for (DOMElement *child: dom_element_children (elt))
    {
      string tag = dom_element_tagname (child);
      if (tag == "attributeAssessment")
        {
          AttributeAssessment *assess;
          assess = this->parseAttributeAssessment (child);
          if (stmt->getMainAssessment () == nullptr)
            stmt->setMainAssessment (assess);
          else
            stmt->setOtherAssessment (assess);
        }
      else if (tag == "valueAssessment")
        {
          stmt->setOtherAssessment (this->parseValueAssessment (child));
        }
      else
        {
          ERROR_SYNTAX_ELT_UNKNOWN_CHILD (elt, child);
        }
    }
  return stmt;
}

AttributeAssessment *
NclParser::parseAttributeAssessment (DOMElement *elt)
{
  AttributeAssessment *assess;
  string role;
  string value;

  CHECK_ELT_TAG (elt, "attributeAssessment", nullptr);
  CHECK_ELT_ATTRIBUTE (elt, "role", &role);

  assess = new AttributeAssessment (role);

  if (dom_element_try_get_attr (value, elt, "eventType"))
    assess->setEventType (EventUtil::getTypeCode (value));

  if (dom_element_try_get_attr (value, elt, "attributeType"))
    assess->setAttributeType (EventUtil::getAttributeTypeCode (value));

  // parameter
  if (assess->getEventType () == EventType::SELECTION
      && dom_element_try_get_attr (value, elt, "key"))
    {
      assess->setKey (value);
    }

  if (dom_element_try_get_attr (value, elt, "offset"))
    assess->setOffset (value);

  return assess;
}

ValueAssessment *
NclParser::parseValueAssessment (DOMElement *elt)
{
  string value;
  CHECK_ELT_TAG (elt, "valueAssessment", nullptr);
  CHECK_ELT_ATTRIBUTE (elt, "value", &value);
  return new ValueAssessment (value);
}

CompoundAction *
NclParser::parseCompoundAction (DOMElement *elt)
{
  CompoundAction *action;
  string value;

  CHECK_ELT_TAG (elt, "compoundAction", nullptr);

  action = new CompoundAction ();

  if (dom_element_try_get_attr (value, elt, "delay"))
    action->setDelay (value);

  // Collect children.
  for (DOMElement *child: dom_element_children (elt))
    {
      string tag = dom_element_tagname (child);
      if (tag == "simpleAction")
        {
          action->addAction (this->parseSimpleAction (child));
        }
      else if (tag == "compoundAction")
        {
          action->addAction (this->parseCompoundAction (child));
        }
      else
        {
          ERROR_SYNTAX_ELT_UNKNOWN_CHILD (elt, child);
        }
    }
  return action;
}

SimpleAction *
NclParser::parseSimpleAction (DOMElement *elt)
{
  SimpleAction *action;
  string tag;
  string role;
  string value;

  EventType type;
  int acttype;
  map<string, pair<int,int>>::iterator it;

  CHECK_ELT_TAG (elt, "simpleAction", nullptr);
  CHECK_ELT_ATTRIBUTE (elt, "role", &role);

  type = EventType::UNKNOWN;
  acttype = -1;

  if ((it = reserved_action_table.find (role))
      != reserved_action_table.end ())
    {
      type = (EventType) it->second.first;
      acttype = (SimpleActionType) it->second.second;
    }

  if (dom_element_try_get_attr (value, elt, "eventType"))
    {
      if (unlikely (type != EventType::UNKNOWN))
        {
          ERROR_SYNTAX_ELT (elt, "eventType '%s' cannot be overridden",
                            role.c_str ());
        }
      map<string, EventType>::iterator it;
      if ((it = event_type_table.find (value)) == event_type_table.end ())
        {
          ERROR_SYNTAX_ELT (elt, "bad eventType '%s' for role '%s'",
                            value.c_str (), role.c_str ());
        }
      type = it->second;
    }

  if (dom_element_try_get_attr (value, elt, "actionType"))
    {
      if (unlikely (acttype != -1))
        {
          ERROR_SYNTAX_ELT (elt, "actionType of '%s' cannot be overridden",
                            role.c_str ());
        }
      map<string, SimpleActionType>::iterator it;
      if ((it = event_action_type_table.find (value))
          == event_action_type_table.end ())
        {
          ERROR_SYNTAX_ELT_BAD_ATTRIBUTE (elt, "actionType");
        }
      acttype = it->second;
    }

  g_assert (type != EventType::UNKNOWN);
  g_assert (acttype != -1);

  action = new SimpleAction (role);
  action->setEventType (type);
  action->setActionType ((SimpleActionType) acttype);

  if (type == EventType::ATTRIBUTION
      && acttype == ACT_START
      && dom_element_try_get_attr (value, elt, "duration"))
    {
      Animation *anim = new Animation ();
      anim->setDuration (value);
      anim->setBy ("indefinite"); // TODO: Handle "by".
      action->setAnimation (anim);
    }

  if (dom_element_try_get_attr (value, elt, "delay"))
    action->setDelay (value);

  if (dom_element_try_get_attr (value, elt, "value"))
    action->setValue (value);

  // TODO: Handle repeatDelay and repeat attributes.

  return action;
}


// Private: Body.

ContextNode *
NclParser::parseBody (DOMElement *elt)
{
  ContextNode *body;
  string id;

  CHECK_ELT_TAG (elt, "body", nullptr);
  CHECK_ELT_OPT_ID (elt, &id, _doc->getId ());

  body = new ContextNode (id);
  _doc->setBody (body);

  for (DOMElement *child: dom_element_children (elt))
    {
      Node *node;
      string tag;

      tag = dom_element_tagname (child);
      if (tag == "port" || tag == "link")
        {
          continue;               // skip
        }

      if (tag == "property")
        {
          PropertyAnchor *prop = this->parseProperty (child);
          g_assert_nonnull (prop);
          body->addAnchor (prop);
          continue;
        }

      node = nullptr;
      if (tag == "media")
        node = this->parseMedia (child);
      else if (tag == "context")
        node = this->parseContext (child);
      else if (tag == "switch")
        node = this->parseSwitch (child);
      else
        ERROR_SYNTAX_ELT_UNKNOWN_CHILD (elt, child);

      g_assert_nonnull (node);
      body->addNode (node);
    }
  return body;
}


// Private: Context.

Node *
NclParser::parseContext (DOMElement *elt)
{
  ContextNode *context;
  string id;

  CHECK_ELT_TAG (elt, "context", nullptr);
  CHECK_ELT_ID (elt, &id);
  CHECK_ELT_ATTRIBUTE_NOT_SUPPORTED (elt, "refer");

  context = new ContextNode (id);
  for (DOMElement *child: dom_element_children (elt))
    {
      Node *node;
      string tag;

      tag = dom_element_tagname (child);
      if (tag == "port" || tag == "link")
        {
          continue;             // skip
        }

      if (tag == "property")
        {
          PropertyAnchor *prop = this->parseProperty (child);
          g_assert_nonnull (prop);
          context->addAnchor (prop);
          continue;
        }

      node = nullptr;
      if (tag == "media")
        node = this->parseMedia (child);
      else if (tag == "context")
        node = this->parseContext (child);
      else if (tag == "switch")
        node = this->parseSwitch (child);
      else
        ERROR_SYNTAX_ELT_UNKNOWN_CHILD (elt, child);

      g_assert_nonnull (node);
      context->addNode (node);
    }
  return context;
}

Port *
NclParser::parsePort (DOMElement *elt, CompositeNode *context)
{
  string id;
  string comp;
  string value;

  Node *target;
  NodeEntity *targetEntity;
  InterfacePoint *interface;

  CHECK_ELT_TAG (elt, "port", nullptr);
  CHECK_ELT_ID (elt, &id);
  CHECK_ELT_ATTRIBUTE (elt, "component", &comp);

  target = context->getNode (comp);
  if (unlikely (target == nullptr))
    ERROR_SYNTAX_ELT_BAD_ATTRIBUTE (elt, "component");

  targetEntity = cast (NodeEntity *, target->getDataEntity ());
  g_assert_nonnull (targetEntity);

  if (dom_element_try_get_attr (value, elt, "interface"))
    {
      interface = targetEntity->getAnchor (value);
      if (interface == nullptr)
        {
          if (instanceof (CompositeNode *, targetEntity))
            {
              interface = ((CompositeNode *) targetEntity)->getPort (value);
            }
          else
            {
              interface = target->getAnchor (value);
            }
        }
    }
  else                          // no interface
    {
      interface = targetEntity->getAnchor (0);
    }

  if (unlikely (interface == nullptr))
    ERROR_SYNTAX_ELT_BAD_ATTRIBUTE (elt, "interface");

  return new Port (id, target, interface);
}

Node *
NclParser::parseSwitch (DOMElement *elt)
{
  Node *swtch;
  string id;

  CHECK_ELT_TAG (elt, "switch", nullptr);
  CHECK_ELT_ID (elt, &id);
  CHECK_ELT_ATTRIBUTE_NOT_SUPPORTED (elt, "refer");

  swtch = new SwitchNode (id);
  _switchConstituents[id] = new map<string, Node *>;

  // Collect children.
  for (DOMElement *child: dom_element_children (elt))
    {
      Node *node;
      string tag;

      tag = dom_element_tagname (child);
      if (tag == "switchPort"
          || tag == "bindRule"
          || tag == "defaultComponent")
        {
          continue;             // skip
        }

      if (tag == "media")
        node = this->parseMedia (child);
      else if (tag == "context")
        node = this->parseContext (child);
      else if (tag == "switch")
        node = this->parseSwitch (child);
      else
        ERROR_SYNTAX_ELT_UNKNOWN_CHILD (elt, child);

      g_assert_nonnull (node);
      map<string, Node *> *map = _switchConstituents[id];
      if (map->count (node->getId ()) == 0)
        (*map)[node->getId ()] = node;
    }

  // Collect skipped.
  for (DOMElement *child: dom_element_children (elt)) // redo
    {
      string tag = dom_element_tagname (child);
      if (tag == "bindRule")
        {
          addBindRuleToSwitch ((SwitchNode *) swtch, child);
        }
      else if (tag == "defaultComponent")
        {
          addDefaultComponentToSwitch ((SwitchNode*) swtch, child);
        }
    }

  addUnmappedNodesToSwitch ((SwitchNode *) swtch);
  return swtch;
}


// Private: Media.

Node *
NclParser::parseMedia (DOMElement *elt)
{
  Node *media;
  string id;
  string src;
  string value;

  CHECK_ELT_TAG (elt, "media", nullptr);
  CHECK_ELT_ID (elt, &id);

  // Refer?
  if (dom_element_try_get_attr (value, elt, "refer"))
    {
      Entity *refer;

      refer = (ContentNode *) _doc->getNode (value);
      if (unlikely (refer == nullptr))
        refer = new ReferredNode (value, (void *) elt); // FIXME: Crazy.

      media = new ReferNode (id);
      if (dom_element_try_get_attr (value, elt, "instance"))
        ((ReferNode *) media)->setInstanceType (value);
      ((ReferNode *) media)->setReferredEntity (refer);
    }
  else
    {
      media = new ContentNode (id, NULL, "");

      if (dom_element_try_get_attr (value, elt, "type"))
        ((ContentNode *) media)->setNodeType (value);

      CHECK_ELT_OPT_ATTRIBUTE (elt, "src", &src, "");
      if (!xpathisuri (src) && !xpathisabs (src))
        src = xpathbuildabs (_dirname, src);
      ((ContentNode *) media)
        ->setContent (new AbsoluteReferenceContent (src));

      if (dom_element_try_get_attr (value, elt, "descriptor"))
        {
          GenericDescriptor *desc = _doc->getDescriptor (value);
          if (unlikely (desc == nullptr))
            ERROR_SYNTAX_ELT_BAD_ATTRIBUTE (elt, "descriptor");
          ((ContentNode *) media)->setDescriptor (desc);
        }
    }

  // Collect children.
  for (DOMElement *child: dom_element_children (elt))
    {
      string tag = dom_element_tagname (child);
      if (tag == "area")
        {
          Anchor *area = this->parseArea (child);
          g_assert_nonnull (area);
          media->addAnchor (area);
        }
      else if (tag == "property")
        {
          PropertyAnchor *prop = this->parseProperty (child);
          g_assert_nonnull (prop);
          media->addAnchor (prop);
        }
      else
        {
          ERROR_SYNTAX_ELT_UNKNOWN_CHILD (elt, child);
        }
    }
  return media;
}

PropertyAnchor *
NclParser::parseProperty (DOMElement *elt)
{
  PropertyAnchor *prop;
  string name;
  string value;

  CHECK_ELT_TAG (elt, "property", nullptr);
  CHECK_ELT_ATTRIBUTE (elt, "name", &name);
  CHECK_ELT_OPT_ATTRIBUTE (elt, "value", &value, "");

  prop = new PropertyAnchor (name);
  prop->setValue (value);
  return prop;
}

Anchor *
NclParser::parseArea (DOMElement *elt)
{
  string id;
  string value;

  CHECK_ELT_TAG (elt, "area", nullptr);
  CHECK_ELT_ID (elt, &id);
  CHECK_ELT_ATTRIBUTE_NOT_SUPPORTED (elt, "coords");
  CHECK_ELT_ATTRIBUTE_NOT_SUPPORTED (elt, "first");
  CHECK_ELT_ATTRIBUTE_NOT_SUPPORTED (elt, "last");

  if (dom_element_has_attr (elt, "begin")
      || dom_element_has_attr (elt, "end"))
    {
      GingaTime begin;
      GingaTime end;

      if (dom_element_try_get_attr (value, elt ,"begin"))
        begin = ginga_parse_time (value);
      else
        begin = 0;

      if (dom_element_try_get_attr (value, elt, "end"))
        end = ginga_parse_time (value);
      else
        end = GINGA_TIME_NONE;

      return new IntervalAnchor (id, begin, end);
    }
  else if (dom_element_has_attr (elt, "text"))
    {
      string text;
      string pos;
      CHECK_ELT_ATTRIBUTE (elt, "text", &text);
      CHECK_ELT_OPT_ATTRIBUTE (elt, "position", &pos, "0");
      return new TextAnchor (id, text, xstrtoint (pos, 10));
    }
  else if (dom_element_has_attr(elt, "label"))
    {
      string label;
      CHECK_ELT_ATTRIBUTE (elt, "label", &label);
      return new LabeledAnchor (id, label);
    }
  else
    {
      return new LabeledAnchor (id, id); // FIXME: Why?
    }
}


// Private: Link.

Link *
NclParser::parseLink (DOMElement *elt, CompositeNode *parent)
{
  Link *link;
  string id;
  string xconn;
  Connector *conn;

  CHECK_ELT_TAG (elt, "link", nullptr);
  CHECK_ELT_OPT_ID (elt, &id, "");
  CHECK_ELT_ATTRIBUTE (elt, "xconnector", &xconn);

  conn = _doc->getConnector (xconn);
  if (unlikely (conn == nullptr))
    ERROR_SYNTAX_ELT_BAD_ATTRIBUTE (elt, "xconnector");

  link = new CausalLink (id, conn);

  // Collect children.
  for (DOMElement *child: dom_element_children (elt))
    {
      string tag = dom_element_tagname (child);
      if (tag == "linkParam")
        {
          link->addParameter (this->parseLinkParam (child));
        }
      else if (tag == "bind")
        {
          g_assert_nonnull (this->parseBind (child, link, parent));
        }
      else
        {
          ERROR_SYNTAX_ELT_UNKNOWN_CHILD (elt, child);
        }
    }
  return link;
}

Parameter *
NclParser::parseLinkParam (DOMElement *elt)
{
  string name;
  string value;
  CHECK_ELT_TAG (elt, "linkParam", nullptr);
  CHECK_ELT_ATTRIBUTE (elt, "name", &name);
  CHECK_ELT_ATTRIBUTE (elt, "value", &value);
  return new Parameter (name, value);
}

Bind *
NclParser::parseBind (DOMElement *elt, Link *link, CompositeNode *context)
{
  Bind *bind;
  string label;
  string comp;
  string value;

  Role *role;
  CausalConnector *conn;

  Node *target;
  NodeEntity *targetEntity;
  InterfacePoint *iface;
  GenericDescriptor *desc;

  CHECK_ELT_TAG (elt, "bind", nullptr);
  CHECK_ELT_ATTRIBUTE (elt, "role", &label);
  CHECK_ELT_ATTRIBUTE (elt, "component", &comp);

  g_assert_nonnull (link);
  g_assert_nonnull (context);

  if (comp == context->getId ())
    {
      target = context;
    }
  else
    {
      target = context->getNode (comp);
      if (unlikely (target == nullptr))
        ERROR_SYNTAX_ELT_BAD_ATTRIBUTE (elt, "component");
    }
  g_assert_nonnull (target);

  targetEntity = (NodeEntity *)(target->getDataEntity ());
  iface = nullptr;
  desc = nullptr;

  if (dom_element_try_get_attr (value, elt, "interface"))
    {
      if (instanceof (CompositeNode *, targetEntity))
        {
          iface = ((CompositeNode *) targetEntity)->getPort (value);
        }
      else
        {
          iface = targetEntity->getAnchor (value);
          if (iface == nullptr) // retry
            {
              iface = target->getAnchor (value);
              if (iface == nullptr) // retry
                {
                  for (ReferNode *refer:
                         *targetEntity->getInstSameInstances ())
                    {
                      iface = refer->getAnchor (value);
                      if (iface != nullptr)
                        break;
                    }
                }
            }
        }
    }
  else                          // no interface
    {
      iface = targetEntity->getAnchor (0);
    }

  if (unlikely (iface == nullptr))
    ERROR_SYNTAX_ELT_BAD_ATTRIBUTE (elt, "interface");

  if (dom_element_try_get_attr (value, elt, "descriptor"))
    desc = _doc->getDescriptor (value);

  conn = cast (CausalConnector *, link->getConnector ());
  g_assert_nonnull (conn);

  role = conn->getRole (label);
  if (role == nullptr)          // ghost "get"
    {
      ConditionExpression *cond;
      AssessmentStatement *stmt;
      AttributeAssessment *assess;

      assess = new AttributeAssessment (label);
      assess->setEventType (EventType::ATTRIBUTION);
      assess->setAttributeType (AttributeType::NODE_PROPERTY);

      stmt = new AssessmentStatement (Comparator::CMP_NE);
      stmt->setMainAssessment (assess);
      stmt->setOtherAssessment (new ValueAssessment (label));

      cond = conn->getConditionExpression ();
      if (instanceof (CompoundCondition *, cond))
        {
          ((CompoundCondition *) cond)->addConditionExpression (stmt);
        }
      else
        {
          conn->setConditionExpression
            (new CompoundCondition (cond, stmt, CompoundCondition::OP_OR));
        }
      role = (Role *) assess;
    }
  g_assert_nonnull (role);

  bind = link->bind (target, iface, desc, role->getLabel ());
  g_assert_nonnull (bind);

  // Collect children.
  for (DOMElement *child: dom_element_children (elt))
    {
      string tag = dom_element_tagname (child);
      if (tag == "bindParam")
        {
          bind->addParameter (this->parseBindParam (child));
        }
      else
        {
          ERROR_SYNTAX_ELT_UNKNOWN_CHILD (elt, child);
        }
    }
  return bind;
}

Parameter *
NclParser::parseBindParam (DOMElement *elt)
{
  string name;
  string value;
  CHECK_ELT_TAG (elt, "bindParam", nullptr);
  CHECK_ELT_ATTRIBUTE (elt, "name", &name);
  CHECK_ELT_ATTRIBUTE (elt, "value", &value);
  return new Parameter (name, value);
}

GINGA_FORMATTER_END
