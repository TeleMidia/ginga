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

GINGA_FORMATTER_BEGIN

// dom_element_* functions are internal functions that safely wraps Xerces
// calls.

// Gets the DOMElement tagname as a std::string and free resources allocated
// by Xerces.
static string
dom_element_tagname (const DOMElement *el)
{
  char *tagname = XMLString::transcode (el->getTagName ());
  string tagname_str (tagname);
  XMLString::release(&tagname);

  return tagname_str;
}

// Checks if DOMElement* has an attribute.
static bool
dom_element_has_attr (const DOMElement *el, const string &attr)
{
  XMLCh *attr_xmlch = XMLString::transcode(attr.c_str ());
  bool result = el->hasAttribute(attr_xmlch);
  XMLString::release(&attr_xmlch);

  return result;
}

// Gets the value of an attribute of DOMElement* as a std::string and free
// resources allocated by Xerces.  Similar to Xerces, it returns an empty
// string if there is no attribute.
static string
dom_element_get_attr (const DOMElement *element, const string &attr)
{
  XMLCh *attr_xmlch = XMLString::transcode(attr.c_str ());
  char *attr_value_ch = XMLString::transcode(element->getAttribute (attr_xmlch));
  string attr_value_str(attr_value_ch);

  XMLString::release(&attr_xmlch);
  XMLString::release(&attr_value_ch);

  return attr_value_str;
}

// If attribute exists in DOMElement *, gets its value in gotAttr variable
// and returns true.  Otherwise, does not change gotAttr and returns false.
static bool
dom_element_try_get_attr (string &gotAttr,
                          const DOMElement *element,
                          const string &attr)
{
  bool has_attr = false;
  XMLCh *attr_xmlch = XMLString::transcode(attr.c_str ());
  if (element->hasAttribute(attr_xmlch))
    {
      has_attr = true;
      char *attr_value_ch =
          XMLString::transcode(element->getAttribute (attr_xmlch));
      gotAttr = attr_value_ch;
      XMLString::release(&attr_value_ch);
    }
  XMLString::release(&attr_xmlch);

  return has_attr;
}

// Sets the value of an attribute of DOMElement* free resources allocated by
// Xerces.
static void
dom_element_set_attr (DOMElement *element,
                      const string &attr, const string &value)
{
  XMLCh *attr_name = XMLString::transcode (attr.c_str ());
  XMLCh *attr_value = XMLString::transcode(value.c_str ());

  element->setAttribute(attr_name, attr_value);

  XMLString::release(&attr_name);
  XMLString::release(&attr_value);
}

// Removes the atribute from DOMElement *
static void
dom_element_remove_attr (DOMElement *element, const string &attr)
{
  XMLCh *attr_name = XMLString::transcode (attr.c_str ());
  element->removeAttribute(attr_name);
  XMLString::release(&attr_name);
}

#define FOR_EACH_DOM_ELEM_CHILD(X, Y) \
  for ( X = Y->getFirstElementChild(); \
        X != nullptr; \
        X = X->getNextElementSibling())

static vector <DOMElement *>
dom_element_children(DOMElement *el)
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
dom_element_children_by_tagname(DOMElement *el, const string &tagname)
{
  vector <DOMElement *> vet;

  DOMElement *child;
  FOR_EACH_DOM_ELEM_CHILD(child, el)
    {
      if (dom_element_tagname(child) == tagname)
        {
          vet.push_back(child);
        }
    }

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

#define ERROR_SYNTAX_UNKNOWN_CHILD(parent, child)\
  ERROR_SYNTAX ("%s: unknown child element '%s'", (parent), (child))


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
  this->_ownManager = false;
}

NclParser::~NclParser ()
{
  for (auto i : _switchConstituents)
    delete i.second;
}

NclDocument *
NclParser::parseRootElement (DOMElement *rootElement)
{
  string tagName = dom_element_tagname(rootElement);
  if (unlikely (tagName != "ncl"))
    ERROR_SYNTAX ("bad root element '%s'", tagName.c_str ());
  return parseNcl (rootElement);
}

string
NclParser::getDirName ()
{
  return this->_dirname;
}

string
NclParser::getPath ()
{
  return this->_path;
}

NclDocument *
NclParser::getNclDocument ()
{
  return this->_ncl;
}

void
NclParser::setNclDocument (NclDocument *ncl)
{
  this->_ncl = ncl;
}
void
NclParser::warning (const SAXParseException &e)
{
  char *file = XMLString::transcode (e.getSystemId ());
  char *errMsg = XMLString::transcode (e.getMessage ());
  if (file == NULL || strlen (file) <= 0)
    g_warning ("%s", errMsg);
  else
    g_warning ("%s:%u.%u: %s", file,
               (guint)e.getLineNumber (),
               (guint)e.getColumnNumber (),
               errMsg);
  XMLString::release(&file);
}

void G_GNUC_NORETURN
NclParser::error (const SAXParseException &e)
{
  char *file = XMLString::transcode (e.getSystemId ());
  char *errMsg = XMLString::transcode (e.getMessage ());
  if (file == NULL || strlen (file) <= 0)
    g_error ("%s", errMsg);
  else
    g_error ("%s:%u.%u: %s", file,
             (guint)e.getLineNumber (),
             (guint)e.getColumnNumber (),
             errMsg);

  XMLString::release(&file);
  XMLString::release(&errMsg);

  exit (EXIT_FAILURE);
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

  this->_path = xpathmakeabs (path);
  this->_dirname = xpathdirname (path);

  XMLPlatformUtils::Initialize ();
  parser = new XercesDOMParser ();
  g_assert_nonnull (parser);

  parser->setValidationScheme (XercesDOMParser::Val_Auto);
  parser->setDoNamespaces (false);
  parser->setDoSchema (false);
  parser->setErrorHandler (this);
  parser->setCreateEntityReferenceNodes (false);

  XMLCh *path_xmlch = XMLString::transcode (path.c_str ());
  LocalFileInputSource src (path_xmlch);
  try
    {
      parser->parse (src);
      XMLString::release(&path_xmlch);
    }
  catch (...)
    {
      g_assert_not_reached ();
    }

  dom = parser->getDocument ();
  g_assert_nonnull (dom);

  elt = (DOMElement *) dom->getDocumentElement ();
  g_assert_nonnull (elt);

  this->_ncl = (NclDocument *) parseRootElement (elt);
  g_assert_nonnull (this->_ncl);

  delete parser;

  // FIXME: Should we call this?
  // XMLPlatformUtils::Terminate ();

  return this->_ncl;
}

Node *
NclParser::getNode (const string &nodeId)
{
  NclDocument *doc = getNclDocument ();
  g_assert_nonnull(doc);

  return doc->getNode (nodeId);
}

NclDocument *
NclParser::importDocument (string &path)
{
  NclParser compiler;

  if (!xpathisuri (path) && !xpathisabs (path))
    path = xpathbuildabs (this->getDirName (), path);

  return compiler.parse (path);
}


// STRUCTURE
ContextNode *
NclParser::parseBody (DOMElement *body_element)
{
  ContextNode *body = createBody (body_element);
  g_assert_nonnull (body);

  for (DOMElement *child: dom_element_children(body_element))
    {
      string tagname = dom_element_tagname (child);
      Node *node = nullptr;

      if (tagname == "media")
        {
          node = parseMedia (child);
        }
      else if (tagname == "context")
        {
          node = parseContext (child);
        }
      else if (tagname == "switch")
        {
          node = parseSwitch (child);
        }
      else if (tagname == "property"
               || tagname == "port"
               || tagname == "link")
        {
          // nothing to do
        }
      else
        {
          ERROR_SYNTAX_UNKNOWN_CHILD ("body", tagname.c_str ());
        }

      if (node != nullptr)
        addNodeToContext (body, node);
    }

  for (DOMElement *child :
       dom_element_children_by_tagname(body_element, "property"))
    {
      PropertyAnchor *prop = parseProperty (child);
      if (prop)
        {
          addPropertyToContext (body, prop); // add property to body
        }
    }

  return body;
}

void
NclParser::parseHead (DOMElement *head_element)
{
  NclDocument *nclDoc = getNclDocument();
  g_assert_nonnull (nclDoc);

  for (DOMElement *child :
       dom_element_children_by_tagname(head_element, "importedDocumentBase"))
    {
      parseImportedDocumentBase (child);
    }

  for (DOMElement *child :
       dom_element_children_by_tagname(head_element, "regionBase"))
    {
      RegionBase *regionBase = parseRegionBase (child);
      if (regionBase)
        nclDoc->addRegionBase (regionBase);
    }

  for (DOMElement *child :
       dom_element_children_by_tagname(head_element, "ruleBase"))
    {
      RuleBase *ruleBase = parseRuleBase (child);
      if (ruleBase)
        {
          nclDoc->setRuleBase (ruleBase);
          break;
        }
    }

  for (DOMElement *child :
       dom_element_children_by_tagname(head_element, "transitionBase"))
    {
      TransitionBase *transBase = parseTransitionBase (child);
      if (transBase)
        {
          nclDoc->setTransitionBase (transBase);
          break;
        }
    }

  for (DOMElement *child :
       dom_element_children_by_tagname(head_element, "descriptorBase"))
    {
      DescriptorBase *descBase = this->parseDescriptorBase (child);
      if (descBase)
        {
          nclDoc->setDescriptorBase (descBase);
          break;
        }
    }

  for (DOMElement *child :
       dom_element_children_by_tagname(head_element, "connectorBase"))
    {
      ConnectorBase *connBase = parseConnectorBase (child);
      if (connBase)
        {
          nclDoc->setConnectorBase(connBase);
          break;
        }
    }
}

NclDocument *
NclParser::parseNcl (DOMElement *ncl_element)
{
  NclDocument* parentObject = createNcl (ncl_element);
  g_assert_nonnull (parentObject);

  for (DOMElement *child :
       dom_element_children_by_tagname(ncl_element, "head"))
    {
      parseHead (child);
    }

  for (DOMElement *child :
       dom_element_children_by_tagname(ncl_element, "body"))
    {
      ContextNode *body = parseBody (child);
      if (body)
        {
          posCompileBody (child, body);
          break;
        }
    }

  // syntax_warn/err:
  // what if there are other children (different from <head> and <body>)

  return parentObject;
}

ContextNode *
NclParser::createBody (DOMElement *body_element)
{
  NclDocument *document;
  ContextNode *context;

  document = getNclDocument ();
  if (!dom_element_has_attr(body_element, "id"))
    {
      dom_element_set_attr (body_element, "id", document->getId());
      context = (ContextNode *) createContext (body_element);
      dom_element_remove_attr (body_element, "id");
    }
  else
    {
      context = (ContextNode *) createContext (body_element);
    }
  document->setBody (context);
  return context;
}

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
                    nodeEntity = (NodeEntity *)(getNode (
                                                  referredNode->getId ()));
                    if (nodeEntity)
                      {
                        ((ReferNode *)node)
                            ->setReferredEntity (nodeEntity
                                                 ->getDataEntity ());
                      }
                    else
                      {
                        ERROR_SYNTAX ("media: bad refer '%s'",
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

ContextNode *
NclParser::posCompileBody (DOMElement *parentElement,
                                    ContextNode *body)
{
  solveNodeReferences (body);
  return posCompileContext (parentElement, body);
}

NclDocument*
NclParser::createNcl (DOMElement *parentElement)
{
  string docName;
  NclDocument *document;

  docName = dom_element_get_attr (parentElement, "id");

  if (docName == "")
    docName = "ncl";

  document = new NclDocument (docName, getPath ());
  g_assert_nonnull (document);

  setNclDocument (document);

  return document;
}


// COMPONENTS
Node *
NclParser::parseMedia (DOMElement *elt)
{
  Node *media = createMedia (elt);
  g_assert_nonnull (media);

  for (DOMElement *child: dom_element_children (elt))
    {
      string tagname = dom_element_tagname(child);
      if (tagname == "area")
        {
          Anchor *area = this->parseArea (child);
          if (area)
            addAnchorToMedia ((ContentNode *) media, area);
        }
      else if (tagname == "property")
        {
          PropertyAnchor *prop = this->parseProperty (child);
          if (prop)
            addAnchorToMedia ((ContentNode*) media, prop);
        }
      else
        {
          ERROR_SYNTAX_UNKNOWN_CHILD ("media", tagname.c_str ());
        }
    }

  return media;
}

Node *
NclParser::parseContext (DOMElement *parentElement)
{
  Node *context = createContext (parentElement);
  g_assert_nonnull (context);

  for (DOMElement *child: dom_element_children (parentElement))
    {
      string tagname = dom_element_tagname(child);
      Node *node = nullptr;
      if (tagname == "media")
        {
          node = parseMedia (child);
        }
      else if (tagname == "context")
        {
          node = parseContext (child);
        }
      else if (tagname == "switch")
        {
          node = parseSwitch (child);
        }
      else if (tagname == "property"
               || tagname == "port"
               || tagname == "link")
        {
          // nothing to do
        }
      else
        {
          ERROR_SYNTAX_UNKNOWN_CHILD ("context", tagname.c_str ());
        }

      if (node != nullptr)
        addNodeToContext (context, node);
    }

  for (DOMElement *child:
       dom_element_children_by_tagname (parentElement, "property"))
    {
      PropertyAnchor *prop = this->parseProperty (child);
      if (prop)
        {
          addPropertyToContext (context, prop);
        }
    }

  return context;
}

void
NclParser::addPropertyToContext (Entity *context, Anchor *property)
{
  if (instanceof (ContextNode *, context))
    {
      ((ContextNode *)context)->addAnchor (property);
    }
  else if (instanceof (ReferNode *, context))
    {
      ((ReferNode *)context)->addAnchor (property);
    }
}

void
NclParser::addNodeToContext (ContextNode *contextNode, Node *node)
{
  contextNode->addNode (node);
}

void
NclParser::addNodeToContext (Entity *context, Node *node)
{
  if (instanceof (ContextNode *, context))
    {
      addNodeToContext ((ContextNode *)context, node);
    }
}

void
NclParser::addAnchorToMedia (ContentNode *contentNode, Anchor *anchor)
{
  if (unlikely (contentNode->getAnchor (anchor->getId ()) != NULL))
    {
      ERROR_SYNTAX ("media '%s': duplicated area '%s'",
                    contentNode->getId ().c_str (),
                    anchor->getId ().c_str ());
    }

  contentNode->addAnchor (anchor);
}

Node *
NclParser::createContext (DOMElement *context_element)
{
  string id, attValue;
  Node *node;
  Entity *referNode = NULL;
  ContextNode *context;
  GenericDescriptor *descriptor;

  if (unlikely (!dom_element_has_attr(context_element, "id")))
    ERROR_SYNTAX ("context: missing id");

  id = dom_element_get_attr(context_element, "id");

  node = this->getNode (id);
  if (unlikely (node != NULL))
    ERROR_SYNTAX ("context '%s': duplicated id", id.c_str ());

  if (dom_element_try_get_attr(attValue, context_element, "refer"))
    {
      try
      {
        referNode = (ContextNode *)this->getNode (attValue);
        if (referNode)
          {
            referNode = (ContextNode *)(getNclDocument ()->getNode (attValue));
            if (referNode == NULL)
              {
                referNode = (Entity *)(new ReferredNode (attValue,
                                                      (void *)context_element));
              }
          }
      }
      catch (...)
      {
        ERROR_SYNTAX ("context '%s': bad refer '%s'",
                      id.c_str (), attValue.c_str ());
      }

      node = new ReferNode (id);
      ((ReferNode *)node)->setReferredEntity (referNode);

      return node;
    }

  context = new ContextNode (id);
  if (dom_element_try_get_attr(attValue, context_element, "descriptor"))
    {
      // adicionar um descritor a um objeto de midia
      descriptor = getNclDocument ()->getDescriptor (attValue);
      if (descriptor != NULL)
        {
          context->setDescriptor (descriptor);
        }
      else
        {
          ERROR_SYNTAX ("context '%s': bad descriptor '%s'",
                        id.c_str (), attValue.c_str ());
        }
    }

  return context;
}

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
          Node *node = this->getNode (id);

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

Node *
NclParser::createMedia (DOMElement *media_element)
{
  string attValue, id;
  Node *node;
  Entity *referNode;
  GenericDescriptor *descriptor;

  if (unlikely (!dom_element_has_attr(media_element, "id")))
    ERROR_SYNTAX ("media: missing id");

  id = dom_element_get_attr(media_element, "id");

  node = this->getNode (id);
  if (unlikely (node != NULL))
    ERROR_SYNTAX ("media '%s': duplicated id", id.c_str ());

  if (dom_element_try_get_attr(attValue, media_element, "refer"))
    {
      try
      {
        referNode = (ContentNode *) this->getNode (attValue);
        if (referNode == NULL)
          {
            referNode = (ContentNode *)getNclDocument ()->getNode (attValue);
            if (referNode == NULL)
              {
                referNode = new ReferredNode (attValue, (void *)media_element);
              }
          }
      }
      catch (...)
      {
        ERROR_SYNTAX ("media '%s': bad refer '%s'",
                      id.c_str (), attValue.c_str ());
      }

      node = new ReferNode (id);
      if (dom_element_try_get_attr(attValue, media_element, "instance"))
        {
          ((ReferNode *)node)->setInstanceType (attValue);
        }
      ((ReferNode *)node)->setReferredEntity (referNode);

      return node;
    }

  node = new ContentNode (id, NULL, "");

  if (dom_element_try_get_attr(attValue, media_element, "type"))
    {
      ((ContentNode *)node)->setNodeType (attValue);
    }

  if (dom_element_try_get_attr(attValue, media_element, "src"))
    {
      if (unlikely (attValue == ""))
        ERROR_SYNTAX ("media '%s': missing src", id.c_str ());

      if (!xpathisuri (attValue) && !xpathisabs (attValue))
        attValue = xpathbuildabs (this->getDirName (), attValue);

      ((ContentNode *)node)->setContent (
            new AbsoluteReferenceContent (attValue));
    }

  if (dom_element_try_get_attr(attValue, media_element, "descriptor"))
    {
      descriptor = getNclDocument ()->getDescriptor (attValue);
      if (descriptor != NULL)
        {
          ((ContentNode *)node)->setDescriptor (descriptor);
        }
      else
        {
          ERROR_SYNTAX ("media '%s': bad descriptor '%s'",
                        id.c_str (), attValue.c_str ());
        }
    }
  return node;
}

// IMPORT
void
NclParser::parseImportedDocumentBase (DOMElement *importedDocumentBase_element)
{
  g_assert_nonnull (importedDocumentBase_element);

  for (DOMElement *child: dom_element_children(importedDocumentBase_element))
    {
      string tagname = dom_element_tagname(importedDocumentBase_element);
      if (tagname == "importNCL")
        {
          addImportNCLToImportedDocumentBase (child);
        }
      else
        {
          ERROR_SYNTAX_UNKNOWN_CHILD ("importedDocumentBase",
                                      tagname.c_str ());
        }
    }
}

void
NclParser::addImportNCLToImportedDocumentBase (DOMElement *importNCL_element)
{
  string docAlias, docLocation;
  NclDocument *thisDocument, *importedDocument;

  docAlias = dom_element_get_attr(importNCL_element, "alias");
  docLocation = dom_element_get_attr(importNCL_element, "documentURI");

  importedDocument = this->importDocument (docLocation);
  if (importedDocument != NULL)
    {
      thisDocument = this->getNclDocument ();
      thisDocument->addDocument (importedDocument, docAlias, docLocation);
    }
}

// TRANSITION
TransitionBase *
NclParser::parseTransitionBase (DOMElement *transBase_element)
{
  TransitionBase *transBase = new TransitionBase (
        dom_element_get_attr(transBase_element, "id"));
  g_assert_nonnull (transBase);

  for(DOMElement *child: dom_element_children(transBase_element))
    {
      string tagname = dom_element_tagname(child);
      if (tagname == "importBase")
        {
          addImportBaseToTransitionBase (transBase, child);
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
          ERROR_SYNTAX_UNKNOWN_CHILD ("transitionBase", tagname.c_str ());
        }
    }

  return transBase;
}

Transition *
NclParser::parseTransition (DOMElement *transition_element)
{
  Transition *transition;
  string id, attValue;
  int type;

  if (unlikely (!dom_element_has_attr(transition_element, "id")))
    ERROR_SYNTAX ("transition: missing id");

  id = dom_element_get_attr(transition_element, "id");
  if (unlikely (!dom_element_has_attr(transition_element, "type")))
    ERROR_SYNTAX ("transition '%s': missing type", id.c_str ());

  attValue = dom_element_get_attr(transition_element, "type");
  type = TransitionUtil::getTypeCode (attValue);

  if (unlikely (type < 0))
    ERROR_SYNTAX ("transition '%s': bad type '%d'", id.c_str (), type);

  transition = new Transition (id, type);
  if (dom_element_try_get_attr(attValue, transition_element, "subtype"))
    {
      int subtype = TransitionUtil::getSubtypeCode (type, attValue);
      if (subtype >= 0)
        {
          transition->setSubtype (subtype);
        }
      else
        {
          ERROR_SYNTAX ("transition: bad subtype");
        }
    }

  if (dom_element_try_get_attr(attValue, transition_element, "dur"))
    {
      double dur = xstrtod (attValue.substr (0, attValue.length () - 1));
      transition->setDur (dur * 1000);
    }

  if (dom_element_try_get_attr (attValue, transition_element,
                                "startProgress"))
    {
      transition->setStartProgress (xstrtod (attValue));
    }

  if (dom_element_try_get_attr (attValue, transition_element,
                                "endProgress"))
    {
      transition->setEndProgress (xstrtod (attValue));
    }

  if (dom_element_try_get_attr (attValue, transition_element, "direction"))
    {
      short direction = TransitionUtil::getDirectionCode (attValue);
      if (unlikely (direction < 0))
        ERROR_SYNTAX ("transition: bad direction value");
      transition->setDirection (direction);
    }

  if (dom_element_try_get_attr(attValue, transition_element, "fadeColor"))
    {
      transition->setFadeColor (ginga_parse_color (attValue));
    }

  if (dom_element_try_get_attr(attValue, transition_element, "horzRepeat"))
    {
      transition->setHorzRepeat (xstrtoint (attValue, 10));
    }

  if (dom_element_try_get_attr(attValue, transition_element, "vertRepeat"))
    {
      transition->setVertRepeat (xstrtoint (attValue, 10));
    }

  if (dom_element_try_get_attr(attValue, transition_element, "borderWidth"))
    {
      transition->setBorderWidth (xstrtoint (attValue, 10));
    }

  if (dom_element_try_get_attr(attValue, transition_element, "borderColor"))
    {
      transition->setBorderColor (ginga_parse_color (attValue));
    }

  return transition;
}

void
NclParser::addImportBaseToTransitionBase (TransitionBase *transBase,
                                          DOMElement *importBase_element)
{
  string baseAlias, baseLocation;
  NclDocument *importedDocument;
  TransitionBase *importedTransitionBase;

  // get the external base alias and location
  baseAlias = dom_element_get_attr(importBase_element, "alias");
  baseLocation = dom_element_get_attr(importBase_element, "documentURI");

  importedDocument = this->importDocument (baseLocation);
  if (unlikely (importedDocument == NULL))
    {
      ERROR_SYNTAX ("importBase '%s': bad documentURI '%s'",
                    baseAlias.c_str (),
                    baseLocation.c_str ());
    }

  importedTransitionBase = importedDocument->getTransitionBase ();
  if (unlikely (importedTransitionBase == NULL))
    {
      ERROR_SYNTAX ("importBase '%s': no transition base in '%s'",
                    baseAlias.c_str (),
                    baseLocation.c_str ());
    }

  transBase->addBase (importedTransitionBase, baseAlias, baseLocation);
}

// CONNECTORS
SimpleCondition *
NclParser::parseSimpleCondition (DOMElement *elt)
{
  string tag;
  string role;
  string value;

  EventType type;
  EventStateTransition trans;
  map<string, pair<int,int>>::iterator it;

  SimpleCondition *cond;        // result

  role =  dom_element_get_attr (elt, "role");
  if (unlikely (role == ""))
    ERROR_SYNTAX ("%s: empty role", tag.c_str ());

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
          ERROR_SYNTAX ("%s: eventType of role '%s' cannot be overridden",
                        tag.c_str (), role.c_str ());
        }
      map<string, EventType>::iterator it;
      if ((it = event_type_table.find (value)) == event_type_table.end ())
        {
          ERROR_SYNTAX ("%s: bad eventType '%s' for role '%s'",
                        tag.c_str (), value.c_str (), role.c_str ());
        }
      type = it->second;
    }

  if (dom_element_try_get_attr (value, elt, "transition"))
    {
      if (unlikely (trans != EventStateTransition::UNKNOWN))
        {
          ERROR_SYNTAX ("%s: transition of role '%s' cannot be overridden",
                        tag.c_str (), role.c_str ());
        }
      map<string, EventStateTransition>::iterator it;
      if ((it = event_transition_table.find (value))
          == event_transition_table.end ())
        {
          ERROR_SYNTAX ("%s: bad transition '%s' for role '%s'",
                        tag.c_str (), value.c_str (), role.c_str ());
        }
      trans = it->second;
    }

  g_assert (type != EventType::UNKNOWN);
  g_assert (trans != EventStateTransition::UNKNOWN);

  cond = new SimpleCondition (role);
  cond->setEventType (type);
  cond->setTransition (trans);

  // TODO: We do not check min and max.
  cond->setMinCon (0);
  cond->setMaxCon (0);

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
        ERROR_SYNTAX ("%s: bad qualifier '%s'",
                      tag.c_str (), value.c_str ());
    }

  return cond;
}

CompoundCondition *
NclParser::parseCompoundCondition (DOMElement *compoundCond_element)
{
  CompoundCondition *compoundCond =
      createCompoundCondition (compoundCond_element);
  g_assert_nonnull (compoundCond);

  for ( DOMElement *child: dom_element_children(compoundCond_element))
    {
      string tagname = dom_element_tagname(child);

      if ( tagname == "simpleCondition")
        {
          SimpleCondition *simpleCond = parseSimpleCondition (child);
          if (simpleCond)
            {
              compoundCond->addConditionExpression (simpleCond);
            }
        }
      else if ( tagname == "assessmentStatement")
        {
          AssessmentStatement *assStatement = parseAssessmentStatement (child);
          if (assStatement)
            {
              compoundCond->addConditionExpression (assStatement);
            }
        }
      else if ( tagname == "compoundCondition")
        {
          CompoundCondition *compoundCond_child =
              parseCompoundCondition (child);

          if (compoundCond_child)
            {
              compoundCond->addConditionExpression (compoundCond_child);
            }
        }
      else if ( tagname ==  "compoundStatement")
        {
          CompoundStatement *compoundStatement = parseCompoundStatement (child);

          if (compoundStatement)
            {
              compoundCond->addConditionExpression (compoundStatement);
            }
        }
      else
        {
          ERROR_SYNTAX_UNKNOWN_CHILD ("compoundCondition",
                                      tagname.c_str ());
        }
    }

  return compoundCond;
}

AssessmentStatement *
NclParser::parseAssessmentStatement (DOMElement *assessmentStatement_element)
{
  AssessmentStatement *assStatement =
      createAssessmentStatement (assessmentStatement_element);
  g_assert_nonnull (assStatement);

  for ( DOMElement *child: dom_element_children(assessmentStatement_element))
    {
      string tagname = dom_element_tagname(child);
      if (tagname == "attributeAssessment")
        {
          AttributeAssessment *attrStatement = parseAttributeAssessment (child);
          if (attrStatement)
            {
              addAttributeAssessmentToAssessmentStatement (assStatement,
                                                           attrStatement);
            }
        }
      else if (tagname == "valueAssessment")
        {
          ValueAssessment *valueAssessment = parseValueAssessment (child);
          if (valueAssessment)
            {
              assStatement->setOtherAssessment (valueAssessment);
            }
        }
      else
        {
          ERROR_SYNTAX_UNKNOWN_CHILD ("assessmentStatement",
                                      tagname.c_str ());
        }
    }

  return assStatement;
}

AttributeAssessment *
NclParser::parseAttributeAssessment (DOMElement *attributeAssessment_element)
{
  AttributeAssessment *attrAssessment;
  string attValue;

  string roleLabel = dom_element_get_attr(attributeAssessment_element, "role");

  attrAssessment = new AttributeAssessment (roleLabel);

  // event type
  if (dom_element_try_get_attr(attValue, attributeAssessment_element, "eventType"))
    {
      attrAssessment->setEventType (EventUtil::getTypeCode (attValue));
    }

  // event type
  if (dom_element_try_get_attr(attValue, attributeAssessment_element, "attributeType"))
    {
      attrAssessment->setAttributeType (
            EventUtil::getAttributeTypeCode (attValue));
    }

  // parameter
  if (attrAssessment->getEventType () == EventType::SELECTION)
    {
      if (dom_element_try_get_attr(attValue, attributeAssessment_element, "key"))
        {
          attrAssessment->setKey (attValue);
        }
    }

  // testing offset
  if (dom_element_try_get_attr(attValue, attributeAssessment_element, "offset"))
    {
      attrAssessment->setOffset (attValue);
    }

  return attrAssessment;
}

ValueAssessment *
NclParser::parseValueAssessment (DOMElement *valueAssessment_element)
{
  string attValue = dom_element_get_attr(valueAssessment_element, "value");

  return new ValueAssessment (attValue);
}

CompoundStatement *
NclParser::parseCompoundStatement (DOMElement *compoundStatement_element)
{
  CompoundStatement *compoundStatement =
      createCompoundStatement (compoundStatement_element);
  g_assert_nonnull (compoundStatement);

  for ( DOMElement *child: dom_element_children(compoundStatement_element))
    {
      string tagname = dom_element_tagname(child);

      if (tagname == "assessmentStatement")
        {
          AssessmentStatement *assStatement = parseAssessmentStatement (child);
          if (assStatement)
            {
              compoundStatement->addStatement (assStatement);
            }
        }
      else if (tagname == "compoundStatement")
        {
          CompoundStatement *compoundStatement_child =
              parseCompoundStatement (child);
          if (compoundStatement_child)
            {
              compoundStatement->addStatement (compoundStatement_child);
            }
        }
      else
        {
          ERROR_SYNTAX_UNKNOWN_CHILD ("compoundStatement",
                                      tagname.c_str ());
        }
    }

  return compoundStatement;
}

SimpleAction *
NclParser::parseSimpleAction (DOMElement *elt)
{
  string tag;
  string role;
  string value;

  EventType type;
  int acttype;
  map<string, pair<int,int>>::iterator it;

  SimpleAction *action;         // result

  role =  dom_element_get_attr (elt, "role");
  if (unlikely (role == ""))
    ERROR_SYNTAX ("%s: empty role", tag.c_str ());

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
          ERROR_SYNTAX ("%s: eventType of role '%s' cannot be overridden",
                        tag.c_str (), role.c_str ());
        }
      map<string, EventType>::iterator it;
      if ((it = event_type_table.find (value)) == event_type_table.end ())
        {
          ERROR_SYNTAX ("%s: bad eventType '%s' for role '%s'",
                        tag.c_str (), value.c_str (), role.c_str ());
        }
      type = it->second;
    }

  if (dom_element_try_get_attr (value, elt, "actionType"))
    {
      if (unlikely (acttype != -1))
        {
          ERROR_SYNTAX ("%s: actionType of role '%s' cannot be overridden",
                        tag.c_str (), role.c_str ());
        }
      map<string, SimpleActionType>::iterator it;
      if ((it = event_action_type_table.find (value))
          == event_action_type_table.end ())
        {
          ERROR_SYNTAX ("%s: bad actionType '%s' for role '%s'",
                        tag.c_str (), value.c_str (), role.c_str ());
        }
      acttype = it->second;
    }

  g_assert (type != EventType::UNKNOWN);
  g_assert (acttype != -1);

  action = new SimpleAction (role);
  action->setEventType (type);
  action->setActionType ((SimpleActionType) acttype);

  if (type == EventType::ATTRIBUTION && acttype == ACT_START
      && dom_element_try_get_attr (value, elt, "duration"))
    {
      Animation *anim = new Animation ();
      anim->setDuration (value);
      anim->setBy ("indefinite"); // TODO: Handle "by".
      action->setAnimation (anim);
    }

  if (dom_element_try_get_attr (value, elt, "delay"))
    action->setDelay (value);

  if (dom_element_try_get_attr (value, elt, "qualifier"))
    {
      if (value == "seq")
        action->setQualifier (CompoundAction::OP_SEQ);
      else if (value == "par")
        action->setQualifier (CompoundAction::OP_PAR);
      else
        ERROR_SYNTAX ("%s: bad qualifier '%s'",
                      tag.c_str (), value.c_str ());
    }

  if (dom_element_try_get_attr (value, elt, "value"))
    action->setValue (value);

  // TODO: Handle repeatDelay and repeat attributes.

  return action;
}

CompoundAction *
NclParser::parseCompoundAction (DOMElement *compoundAction_element)
{
  CompoundAction *compoundAction =
      createCompoundAction (compoundAction_element);
  g_assert_nonnull (compoundAction);

  for (DOMElement *child: dom_element_children(compoundAction_element))
    {
      string tagname = dom_element_tagname(child);
      if (tagname == "simpleAction")
        {
          SimpleAction *simpleAction = parseSimpleAction (child);
          if (simpleAction)
            {
              compoundAction->addAction (simpleAction);
            }
        }
      else if (tagname == "compoundAction")
        {
          CompoundAction *compoundAction_child = parseCompoundAction (child);
          if (compoundAction_child)
            {
              compoundAction->addAction (compoundAction_child);
            }
        }
      else
        {
          ERROR_SYNTAX_UNKNOWN_CHILD ("compoundAction", tagname.c_str ());
        }
    }

  return compoundAction;
}

CausalConnector *
NclParser::parseCausalConnector (DOMElement *elt)
{
  CausalConnector *conn;
  string id;

  if (unlikely (!dom_element_try_get_attr (id, elt, "id")))
    ERROR_SYNTAX ("%s: missing id", dom_element_tagname (elt).c_str ());

  conn = new CausalConnector (id);
  for (DOMElement *child: dom_element_children (elt))
    {
      string tagname = dom_element_tagname(child);

      if (tagname == "simpleCondition")
        {
          SimpleCondition *simpleCondition = parseSimpleCondition (child);
          g_assert_nonnull (simpleCondition);
          conn->setConditionExpression (simpleCondition);
        }
      else if (tagname == "simpleAction")
        {
          SimpleAction *simpleAction = parseSimpleAction (child);
          g_assert_nonnull (simpleAction);
          conn->setAction (simpleAction);
        }
      else if (tagname == "compoundAction")
        {
          CompoundAction *compoundAction = parseCompoundAction (child);
          g_assert_nonnull (compoundAction);
          conn->setAction (compoundAction);
        }
      else if (tagname == "connectorParam")
        {
          Parameter *param;
          param = new Parameter (dom_element_get_attr (child, "name"),
                                 dom_element_get_attr (child, "type"));
          conn->addParameter (param);
        }
      else if (tagname == "compoundCondition")
        {
          CompoundCondition *compoundCond = parseCompoundCondition (child);
          g_assert_nonnull (compoundCond);
          conn->setConditionExpression (compoundCond);
        }
      else
        {
          ERROR_SYNTAX_UNKNOWN_CHILD (dom_element_tagname (elt).c_str (),
                                      tagname.c_str ());
        }
    }
  return conn;
}

ConnectorBase *
NclParser::parseConnectorBase (DOMElement *elt)
{
  ConnectorBase *connBase;

  connBase = new ConnectorBase (dom_element_get_attr (elt, "id"));
  for (DOMElement *child: dom_element_children (elt))
    {
      string tag = dom_element_tagname (child);
      if (tag == "importBase")
        {
          addImportBaseToConnectorBase (connBase, child);
        }
      else if (tag ==  "causalConnector")
        {
          CausalConnector *conn = parseCausalConnector (child);
          g_assert_nonnull (conn);
          connBase->addConnector (conn);
        }
      else
        {
          ERROR_SYNTAX_UNKNOWN_CHILD
            (dom_element_tagname (elt).c_str (), tag.c_str ());
        }
    }

  return connBase;
}

void
NclParser::addImportBaseToConnectorBase (ConnectorBase *connBase,
                                         DOMElement *importBase_element)
{
  string baseAlias, baseLocation;
  NclDocument *importedDocument;
  ConnectorBase *importedConnectorBase;

  // get the external base alias and location
  baseAlias = dom_element_get_attr(importBase_element, "alias");
  baseLocation = dom_element_get_attr(importBase_element, "documentURI");

  importedDocument = this->importDocument (baseLocation);
  if (unlikely (importedDocument == NULL))
    {
      ERROR_SYNTAX ("importBase '%s': bad documentURI '%s'",
                    baseAlias.c_str (),
                    baseLocation.c_str ());
    }

  importedConnectorBase = importedDocument->getConnectorBase ();
  if (unlikely (importedConnectorBase == NULL))
    {
      ERROR_SYNTAX ("importBase '%s': no connector base in '%s'",
                    baseAlias.c_str (),
                    baseLocation.c_str ());
    }

  connBase->addBase (importedConnectorBase, baseAlias, baseLocation);
}

CompoundCondition *
NclParser::createCompoundCondition (DOMElement *compoundCond_element)
{
  CompoundCondition *conditionExpression;
  string attValue;

  conditionExpression = new CompoundCondition ();

  string op = dom_element_get_attr(compoundCond_element, "operator");

  if (op == "and")
    {
      conditionExpression->setOperator (CompoundCondition::OP_AND);
    }
  else
    {
      conditionExpression->setOperator (CompoundCondition::OP_OR);
    }

  // delay
  if (dom_element_try_get_attr(attValue, compoundCond_element, "delay"))
    {
      conditionExpression->setDelay (attValue);
    }

  return conditionExpression;
}


AssessmentStatement *
NclParser::createAssessmentStatement (DOMElement *assStatement_element)
{
  AssessmentStatement *assStatement;
  string attValue;

  if (dom_element_try_get_attr(attValue, assStatement_element, "comparator"))
    {
      assStatement
          = new AssessmentStatement (Comparator::fromString (attValue));
    }
  else
    {
      assStatement = new AssessmentStatement (Comparator::CMP_EQ);
    }

  return assStatement;
}

CompoundStatement *
NclParser::createCompoundStatement (DOMElement *compoundStatement_element)
{
  string attValue;
  CompoundStatement *compoundStatement = new CompoundStatement ();

  attValue = dom_element_get_attr(compoundStatement_element, "operator");
  if (attValue == "and")
    {
      compoundStatement->setOperator (CompoundStatement::OP_AND);
    }
  else
    {
      compoundStatement->setOperator (CompoundStatement::OP_OR);
    }

  // testing isNegated
  if (dom_element_try_get_attr(attValue, compoundStatement_element, "isNegated"))
    {
      compoundStatement->setNegated (attValue == "true");
    }

  return compoundStatement;
}

CompoundAction *
NclParser::createCompoundAction (DOMElement *compoundAction_element)
{
  string attValue;
  CompoundAction *actionExpression = new CompoundAction ();

  attValue = dom_element_get_attr(compoundAction_element, "operator");
  if (attValue == "seq")
    {
      actionExpression->setOperator (CompoundAction::OP_SEQ);
    }
  else
    {
      actionExpression->setOperator (CompoundAction::OP_PAR);
    }

  //  delay
  if (dom_element_try_get_attr(attValue, compoundAction_element, "delay"))
    {
      if (attValue[0] == '$')
        {
          actionExpression->setDelay (attValue);
        }
      else
        {
          actionExpression->setDelay (
              xstrbuild ("%d", (xstrtoint (
                        attValue.substr (0, attValue.length () - 1), 10)
                    * 1000)));
        }
    }

  return actionExpression;
}

void
NclParser::addAttributeAssessmentToAssessmentStatement (
    AssessmentStatement *assStatement,
    AttributeAssessment *attrStatement)
{
  if (assStatement->getMainAssessment () == NULL)
    {
      assStatement->setMainAssessment (attrStatement);
    }
  else
    {
      assStatement->setOtherAssessment (attrStatement);
    }
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
  switchNode = (SwitchNode *)this->getNode (id);
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

Anchor *
NclParser::parseArea (DOMElement *parent)
{
  string anchorId;
  string position, anchorLabel;
  Anchor *anchor;

  if (unlikely (!dom_element_has_attr(parent, "id")))
    ERROR_SYNTAX ("area: missing id");

  anchorId = dom_element_get_attr(parent, "id");
  anchor = NULL;

  if (dom_element_has_attr(parent, "begin")
      || dom_element_has_attr(parent, "end")
      || dom_element_has_attr(parent, "first")
      || dom_element_has_attr(parent, "last"))
    {
      anchor = createTemporalAnchor (parent);
    }
  else if (dom_element_has_attr (parent, "text"))
    {
      position = dom_element_get_attr(parent, "position");

      anchor = new TextAnchor (
          anchorId, dom_element_get_attr(parent, "text"),
          xstrtoint (position, 10));
    }
  else if (dom_element_has_attr(parent, "coords"))
    {
      anchor = createSpatialAnchor (parent);
    }
  else if (dom_element_has_attr(parent, "label"))
    {
      anchorLabel = dom_element_get_attr(parent, "label");

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
NclParser::parseProperty (DOMElement *parent)
{
  string attributeName, attributeValue;
  PropertyAnchor *anchor;

  if (unlikely (!dom_element_has_attr(parent, "name")))
    ERROR_SYNTAX ("property: missing name");

  attributeName = dom_element_get_attr(parent, "name");

  anchor = new PropertyAnchor (attributeName);
  if (dom_element_try_get_attr(attributeValue, parent, "value"))
    {
      anchor->setValue (attributeValue);
    }

  return anchor;
}

Port *
NclParser::parsePort (DOMElement *parent, CompositeNode *context)
{
  string id, attValue;
  Node *portNode;
  NodeEntity *portNodeEntity;
  InterfacePoint *portInterfacePoint = NULL;
  Port *port = NULL;

  if (unlikely (!dom_element_has_attr(parent, "id")))
    ERROR_SYNTAX ("port: missing id");

  id = dom_element_get_attr(parent, "id");

  if (unlikely (context->getPort (id) != NULL))
    ERROR_SYNTAX ("port '%s': duplicated id", id.c_str ());

  if (!unlikely (dom_element_has_attr (parent, "component")))
    ERROR_SYNTAX ("port '%s': missing component", id.c_str ());

  attValue = dom_element_get_attr(parent, "component");

  portNode = context->getNode (attValue);
  if (unlikely (portNode == NULL))
    {
      ERROR_SYNTAX ("port '%s': bad component '%s'", id.c_str (),
                    attValue.c_str ());
    }

  portNodeEntity = (NodeEntity *)portNode->getDataEntity ();
  if (!dom_element_has_attr(parent, "interface"))
    {
      if (instanceof (ReferNode *, portNode)
          && ((ReferNode *)portNode)->getInstanceType () == "new")
        {
          portInterfacePoint = portNode->getAnchor (0);
          if (portInterfacePoint == NULL)
            {
              portInterfacePoint = new LambdaAnchor (portNode->getId ());
              portNode->addAnchor (0, (Anchor *)portInterfacePoint);
            }
        }
      else if (instanceof (Node *, portNodeEntity))
        {
          portInterfacePoint = portNodeEntity->getAnchor (0);
          if (unlikely (portInterfacePoint == NULL))
            {
              ERROR_SYNTAX ("port '%s': bad interface '%s'",
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
      attValue = dom_element_get_attr(parent, "interface");

      if (instanceof (ReferNode *, portNode)
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
          if (instanceof (CompositeNode *, portNodeEntity))
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
      attValue = dom_element_get_attr(parent, "interface");
      ERROR_SYNTAX ("port '%s': bad interface '%s'", id.c_str (),
                    attValue.c_str ());
    }

  port = new Port (id, portNode, portInterfacePoint);
  return port;
}

SpatialAnchor *
NclParser::createSpatialAnchor (DOMElement *areaElement)
{
  SpatialAnchor *anchor = NULL;
  string coords, shape;

  if (dom_element_try_get_attr(coords, areaElement, "coords"))
    {
      if (!dom_element_try_get_attr(shape, areaElement, "shape"))
        {
          shape = "rect";
        }

      if (shape == "rect" || shape == "default")
        {
          int x1, y1, x2, y2;
          sscanf (coords.c_str (), "%d,%d,%d,%d", &x1, &y1, &x2, &y2);
          anchor = new RectangleSpatialAnchor (
                dom_element_get_attr(areaElement, "id"),
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
NclParser::createTemporalAnchor (DOMElement *areaElement)
{
  IntervalAnchor *anchor = NULL;
  string begin, end;
  GingaTime begVal, endVal;

  if (dom_element_has_attr (areaElement, "begin")
      || dom_element_has_attr (areaElement, "end"))
    {
      if (dom_element_try_get_attr (begin, areaElement ,"begin"))
        {
          begVal = ginga_parse_time (begin);
        }
      else
        {
          begVal = 0;
        }

      if (dom_element_try_get_attr (end, areaElement, "end"))
        {
          endVal = ginga_parse_time (end);
        }
      else
        {
          endVal = GINGA_TIME_NONE;
        }

      anchor = new IntervalAnchor
        (dom_element_get_attr (areaElement, "id"), begVal, endVal);
    }

  // Region delimeted through sample identifications
  if (dom_element_has_attr (areaElement, "first"))
    {
      ERROR_NOT_IMPLEMENTED ("area: attribute 'first' is not supported");
    }

  if (dom_element_has_attr (areaElement, "last"))
    {
      ERROR_NOT_IMPLEMENTED ("area: attribute 'last' is not supported");
    }

  return anchor;
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

LayoutRegion *
NclParser::parseRegion (DOMElement *elt, LayoutRegion *parent,
                        RegionBase *base)
{
  LayoutRegion *region = createRegion (elt, parent);
  g_assert_nonnull (region);

  for (DOMElement *child_elt:
         dom_element_children_by_tagname (elt, "region"))
    {
      LayoutRegion *child_reg = parseRegion (child_elt, region, base);
      g_assert_nonnull (child_reg);
      base->addRegion (child_reg);
    }

  return region;
}

// LAYOUT
RegionBase *
NclParser::parseRegionBase (DOMElement *elt)
{
  RegionBase *base = createRegionBase (elt);
  g_assert_nonnull (base);

  for (DOMElement *child_elt: dom_element_children (elt))
    {
      string tag = dom_element_tagname (child_elt);
      if (tag == "importBase")
        {
          addImportBaseToRegionBase (base, child_elt);
        }
      else if (tag == "region")
        {
          LayoutRegion *region = parseRegion (child_elt, NULL, base);
          g_assert_nonnull (region);
          base->addRegion (region);
        }
      else
        {
          ERROR_SYNTAX_UNKNOWN_CHILD ("regionBase", tag.c_str ());
        }
    }
  return base;
}

void
NclParser::addImportBaseToRegionBase (RegionBase *regionBase,
                                      DOMElement *importBase_element)
{
  string baseAlias, baseLocation;
  NclDocument *importedDocument;

  // get the external base alias and location
  baseAlias = dom_element_get_attr(importBase_element, "alias");
  baseLocation = dom_element_get_attr(importBase_element, "documentURI");

  importedDocument = this->importDocument (baseLocation);
  if (importedDocument == NULL)
    {
      return;
    }

  // insert the imported base into the document region base
  for(auto base: *(importedDocument->getRegionBases ()))
    {
      regionBase->addBase (base.second, baseAlias, baseLocation);
    }
}

RegionBase *
NclParser::createRegionBase (DOMElement *elt)
{
  return new RegionBase (dom_element_get_attr (elt, "id"));
}

LayoutRegion *
NclParser::createRegion (DOMElement *elt, LayoutRegion *parent)
{
  LayoutRegion *region;
  SDL_Rect parent_rect;
  SDL_Rect rect;
  int z;
  int zorder;
  static int last_zorder = 0;

  string val;

  region = new LayoutRegion (dom_element_get_attr (elt, "id"));
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
  z = 0;
  zorder = 0;

  if (dom_element_try_get_attr (val, elt, "left"))
    rect.x += ginga_parse_percent (val, parent_rect.w, 0, G_MAXINT);

  if (dom_element_try_get_attr (val, elt, "top"))
    rect.y += ginga_parse_percent (val, parent_rect.h, 0, G_MAXINT);

  if (dom_element_try_get_attr (val, elt, "width"))
    rect.w = ginga_parse_percent (val, parent_rect.w, 0, G_MAXINT);

  if (dom_element_try_get_attr (val, elt, "height"))
    rect.h = ginga_parse_percent (val, parent_rect.h, 0, G_MAXINT);

  if (dom_element_try_get_attr (val, elt, "right"))
    {
    rect.x += parent_rect.w - rect.w
      - ginga_parse_percent (val, parent_rect.w, 0, G_MAXINT);
    }

  if (dom_element_try_get_attr (val, elt, "bottom"))
    {
      rect.y += parent_rect.h - rect.h
        - ginga_parse_percent (val, parent_rect.h, 0, G_MAXINT);
    }

  if (dom_element_try_get_attr (val, elt, "zIndex"))
    z = xstrtoint (val, 10);
  zorder = last_zorder++;

  region->setRect (rect);
  region->setZ (z, zorder);
  region->dump ();

  return region;
}

// LINKING
Bind *
NclParser::parseBind (DOMElement *bind_element, Link *link)
{
  Bind *bind = createBind (bind_element, link);
  g_assert_nonnull (bind);

  for (DOMElement *child: dom_element_children (bind_element))
    {
      string tagname = dom_element_tagname (child);
      if (tagname == "bindParam")
        {
          Parameter *param = parseLinkOrBindParam (child);
          if (param)
            {
              bind->addParameter (param);
            }
        }
      else
        {
          ERROR_SYNTAX_UNKNOWN_CHILD ("bind", tagname.c_str ());
        }
    }

  return bind;
}

Parameter *
NclParser::parseLinkOrBindParam (DOMElement *parentElement)
{
  Parameter *param;
  param = new Parameter (
      dom_element_get_attr(parentElement, "name"),
      dom_element_get_attr(parentElement, "value"));

  return param;
}

Link *
NclParser::parseLink (DOMElement *link_element,
                             CompositeNode *compositeNode)
{
  Link *link = createLink (link_element, compositeNode);
  g_assert_nonnull (link);

  for (DOMElement *child: dom_element_children(link_element))
    {
      string tagname = dom_element_tagname(child);
      if (tagname == "linkParam")
        {
          Parameter *param = parseLinkOrBindParam(child);
          if (param)
            {
              link->addParameter (param);
            }
        }
      else if (tagname == "bind")
        {
          Bind *bind = parseBind (child, link);
          if (bind)
            {
              // nothing to do, since to be created the bind needs to be
              // associated with its link
            }
        }
      else
        {
          ERROR_SYNTAX_UNKNOWN_CHILD ("link", tagname.c_str ());
        }
    }

  return link;
}

Bind *
NclParser::createBind (DOMElement *bind_element, Link *link)
{
  string component, roleId, interfaceId;
  Role *role;
  Node *anchorNode;
  NodeEntity *anchorNodeEntity;
  InterfacePoint *interfacePoint = NULL;
  NclDocument *document;
  GenericDescriptor *descriptor;

  role = _connectorLinkParsing->getRole (dom_element_get_attr (bind_element, "role"));
  component = dom_element_get_attr(bind_element, "component");

  if (_composite->getId () == component)
    {
      anchorNode = (Node *)_composite;
    }
  else
    {
      anchorNode = (Node *)(_composite->getNode (component));
    }

  if (unlikely (anchorNode == NULL))
    {
      ERROR_SYNTAX ("bind: bad component '%s'",
                    component.c_str ());
    }

  anchorNodeEntity = (NodeEntity *)(anchorNode->getDataEntity ());

  if (dom_element_try_get_attr (interfaceId, bind_element, "interface"))
    {
      if (anchorNodeEntity == NULL)
        {
          interfacePoint = NULL;
        }
      else
        {
          if (instanceof (ReferNode *, anchorNode)
              && ((ReferNode *)anchorNode)->getInstanceType () == "new")
            {
              interfacePoint = anchorNode->getAnchor (interfaceId);
            }
          else
            {
              interfacePoint = anchorNodeEntity->getAnchor (interfaceId);
            }
        }

      if (interfacePoint == NULL)
        {
          if (anchorNodeEntity != NULL
              && instanceof (CompositeNode *, anchorNodeEntity))
            {
              interfacePoint = ((CompositeNode *)anchorNodeEntity)
                                   ->getPort (interfaceId);
            }
          else
            {
              interfacePoint = anchorNode->getAnchor (interfaceId);
              if (interfacePoint == NULL)
                {
                  for (ReferNode *referNode:
                       *anchorNodeEntity->getInstSameInstances())
                    {
                      interfacePoint = referNode->getAnchor (interfaceId);
                      if (interfacePoint != NULL)
                        {
                          break;
                        }
                    }
                }
            }
        }
      if (unlikely (interfacePoint == NULL))
        {
          ERROR_SYNTAX ("bind: bad interface '%s' for component '%s'",
                        interfaceId.c_str (), component.c_str ());
        }
    }
  else if (anchorNodeEntity != NULL)
    {
      if (instanceof (ReferNode *, anchorNode)
          && ((ReferNode *)anchorNode)->getInstanceType () == "new")
        {
          interfacePoint = anchorNode->getAnchor (0);
          if (interfacePoint == NULL)
            {
              interfacePoint = new LambdaAnchor (anchorNode->getId ());
              anchorNode->addAnchor (0, (Anchor *)interfacePoint);
            }
        }
      else if (instanceof (Node *, anchorNodeEntity))
        {
          interfacePoint = anchorNodeEntity->getAnchor (0);
        }
      else
        {
          ERROR_SYNTAX ("bind: bad interface for entity '%s'",
                        anchorNodeEntity->getId ().c_str ());
        }
    }
  else
    {
      interfacePoint = anchorNode->getAnchor (0);
    }

  if (dom_element_has_attr(bind_element, "descriptor"))
    {
      document = this->getNclDocument ();
      descriptor = document->getDescriptor (
            dom_element_get_attr(bind_element, "descriptor"));
    }
  else
    {
      descriptor = NULL;
    }

  if (role == NULL)
    {
      // &got
      if (dom_element_try_get_attr(roleId, bind_element, "role"))
        {
          ConditionExpression *condition;
          CompoundCondition *compoundCondition;
          AssessmentStatement *statement;
          AttributeAssessment *assessment;
          ValueAssessment *otherAssessment;

          assessment = new AttributeAssessment (roleId);
          assessment->setEventType (EventType::ATTRIBUTION);
          assessment->setAttributeType (AttributeType::NODE_PROPERTY);
          assessment->setMinCon (0);
          assessment->setMaxCon (0);

          otherAssessment = new ValueAssessment (roleId);

          statement = new AssessmentStatement (Comparator::CMP_NE);
          statement->setMainAssessment (assessment);
          statement->setOtherAssessment (otherAssessment);

          condition
              = ((CausalConnector *)_connectorLinkParsing)->getConditionExpression ();

          if (instanceof (CompoundCondition *, condition))
            {
              ((CompoundCondition *)condition)
                  ->addConditionExpression (statement);
            }
          else
            {
              compoundCondition = new CompoundCondition (
                  condition, statement, CompoundCondition::OP_OR);

              ((CausalConnector *)_connectorLinkParsing)
                  ->setConditionExpression (
                      (ConditionExpression *)compoundCondition);
            }
          role = (Role *)assessment;
        }
      else
        {
          ERROR_SYNTAX ("bind: missing role");
        }
    }

  return link->bind (anchorNode, interfacePoint, descriptor, role->getLabel ());
}

Link *
NclParser::createLink (DOMElement *link_element,
                              CompositeNode *compositeNode)
{
  NclDocument *document = this->getNclDocument ();
  string connectorId =
      dom_element_get_attr(link_element, "xconnector");

  _connectorLinkParsing = document->getConnector (connectorId);
  if (unlikely (_connectorLinkParsing == NULL))
    {
      ERROR_SYNTAX ("link: bad xconnector '%s'", connectorId.c_str ());
    }

  g_assert (instanceof (CausalConnector *, _connectorLinkParsing));

  Link *link = new CausalLink (dom_element_get_attr (link_element, "id"),
                               _connectorLinkParsing);
  _composite = compositeNode;

  return link;
}

// PRESENTATION_CONTROL
RuleBase *
NclParser::parseRuleBase (DOMElement *ruleBase_element)
{
  RuleBase *ruleBase = createRuleBase (ruleBase_element);
  g_assert_nonnull (ruleBase);

  for (DOMElement *child: dom_element_children(ruleBase_element))
    {
      string tagname = dom_element_tagname(child);
      if ( tagname == "importBase")
        {
          addImportBaseToRuleBase (ruleBase, child);
        }
      else if (tagname == "rule")
        {
          SimpleRule *rule = parseRule (child);
          if (rule)
            {
              ruleBase->addRule (rule);
            }
        }
      else if (tagname == "compositeRule")
        {
          CompositeRule *compositeRule = parseCompositeRule (child);
          if (compositeRule)
            {
              ruleBase->addRule (compositeRule);
            }
        }
      else
        {
          ERROR_SYNTAX_UNKNOWN_CHILD ("ruleBase", tagname.c_str ());
        }
    }

  return ruleBase;
}

SimpleRule *
NclParser::parseRule (DOMElement *elt)
{
  string var = dom_element_get_attr (elt, "var");
  string value = dom_element_get_attr (elt, "value");
  string id = dom_element_get_attr (elt, "id");
  Comparator::Op ruleOp = Comparator::fromString
    (dom_element_get_attr (elt, "comparator"));

  return new SimpleRule (id, var, ruleOp, value);
}

Node *
NclParser::parseSwitch (DOMElement *switch_element)
{
  Node *switch_node = createSwitch (switch_element);
  if (unlikely (switch_node == NULL))
    {
      ERROR_SYNTAX ( "switch: bad parent '%s'",
                     dom_element_tagname(switch_element).c_str ());
    }

  for (DOMElement *element: dom_element_children(switch_element))
    {
      string tagname = dom_element_tagname(element);
      if ( tagname == "media")
        {
          Node *media = this->parseMedia (element);
          if (media)
            {
              addNodeToSwitch (switch_node, media);
            }
        }
      else if (tagname == "context")
        {
          Node *ctx = this->parseContext (element);
          if (ctx)
            {
              addNodeToSwitch (switch_node, ctx);
            }
        }
      else if (tagname == "switch")
        {
          Node *switch_child = this->parseSwitch (element);
          if (switch_child)
            {
              addNodeToSwitch (switch_node, switch_child);
            }
        }
      else if (tagname == "switchPort"
               || tagname == "bindRule"
               || tagname == "defaultComponent")
        {
          // nothing to do
        }
      else
        {
          ERROR_SYNTAX_UNKNOWN_CHILD ("switch", tagname.c_str ());
        }
    }

    for (DOMElement *child: dom_element_children (switch_element))
      {
        string tagname = dom_element_tagname(child);
        if (tagname == "bindRule")
          {
            addBindRuleToSwitch ((SwitchNode *)switch_node, child);
          }
        else if (tagname == "defaultComponent")
          {
            addDefaultComponentToSwitch ((SwitchNode*)switch_node, child);
          }
      }

  addUnmappedNodesToSwitch ((SwitchNode *)switch_node);

  return switch_node;
}

CompositeRule *
NclParser::parseCompositeRule (DOMElement *elt)
{
  CompositeRule *compositeRule = createCompositeRule (elt);
  g_assert_nonnull (compositeRule);

  for (DOMElement *child: dom_element_children(elt))
    {
      string tagname = dom_element_tagname(child);
      if (tagname == "rule")
        {
          SimpleRule *simpleRule = parseRule (child);
          if (simpleRule)
            {
              compositeRule->addRule (simpleRule);
            }
        }
      else if (tagname == "compositeRule")
        {
          CompositeRule *child_compositeRule = parseCompositeRule (child);
          if (child_compositeRule)
            {
              compositeRule->addRule (child_compositeRule);
            }
        }
      else
        {
          ERROR_SYNTAX_UNKNOWN_CHILD ("compositeRule", tagname.c_str ());
        }
    }

  return compositeRule;
}

DescriptorSwitch *
NclParser::parseDescriptorSwitch (DOMElement *descriptorSwitch_element)
{
  DescriptorSwitch *descriptorSwitch =
      createDescriptorSwitch (descriptorSwitch_element);
  g_assert_nonnull (descriptorSwitch);

  for (DOMElement *child: dom_element_children(descriptorSwitch_element))
    {
      string tagname = dom_element_tagname(child);
      if ( tagname == "descriptor")
        {
          Descriptor* desc = this->parseDescriptor (child);
          if (desc)
            {
              addDescriptorToDescriptorSwitch (descriptorSwitch, desc);
            }
        }
      else
        {
          ERROR_SYNTAX_UNKNOWN_CHILD ("descriptorSwitch", tagname.c_str ());
        }
    }

  for (DOMElement *child: dom_element_children(descriptorSwitch_element))
    {
      string tagname = dom_element_tagname(child);
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

CompositeRule *
NclParser::createCompositeRule (DOMElement *parentElement)
{
  CompositeRule *compositePresentationRule;
  short ruleOp = CompositeRule::OP_AND;

  string op = dom_element_get_attr(parentElement, "operator");
  if (op == "and")
    {
      ruleOp = CompositeRule::OP_AND;
    }
  else if (op == "or")
    {
      ruleOp = CompositeRule::OP_OR;
    }

  compositePresentationRule = new CompositeRule (
        dom_element_get_attr(parentElement, "id"),
        ruleOp);

  return compositePresentationRule;
}

Node *
NclParser::createSwitch (DOMElement *switch_element)
{
  string id;
  Node *node;
  string attValue;
  Entity *referNode;
  SwitchNode *switchNode;

  if (unlikely (!dom_element_has_attr(switch_element, "id")))
    ERROR_SYNTAX ("switch: missing id");

  id = dom_element_get_attr(switch_element, "id");

  node = this->getNode (id);
  if (unlikely (node != NULL))
    ERROR_SYNTAX ("switch '%s': duplicated id", id.c_str ());

  if (dom_element_try_get_attr(attValue, switch_element, "refer"))
    {
      try
        {
          referNode = (SwitchNode *) this->getNode (attValue);

          if (referNode == NULL)
            {
              referNode = (SwitchNode *)getNclDocument ()->getNode (attValue);
              if (referNode == NULL)
                {
                  referNode
                      = new ReferredNode (attValue, (void *)switch_element);
                }
            }
        }
      catch (...)
        {
          ERROR_SYNTAX ("switch '%s': bad refer '%s'",
                        id.c_str (), attValue.c_str ());
        }

      node = new ReferNode (id);
      ((ReferNode *)node)->setReferredEntity (referNode);

      return node;
    }

  switchNode = new SwitchNode (id);
  _switchConstituents[switchNode->getId ()] = new map<string, Node *>;

  return switchNode;
}

RuleBase *
NclParser::createRuleBase (DOMElement *ruleBase_element)
{
  RuleBase *ruleBase
      = new RuleBase (dom_element_get_attr(ruleBase_element, "id"));

  return ruleBase;
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
NclParser::addImportBaseToRuleBase (
    RuleBase *ruleBase, DOMElement *importBase_element)
{
  string baseAlias, baseLocation;
  NclDocument *importedDocument;
  RuleBase *importedRuleBase;

  baseAlias = dom_element_get_attr(importBase_element, "alias");
  baseLocation = dom_element_get_attr(importBase_element, "documentURI");

  importedDocument = this->importDocument (baseLocation);
  g_assert_nonnull(importedDocument);

  importedRuleBase = importedDocument->getRuleBase ();
  g_assert_nonnull(importedRuleBase);

  ruleBase->addBase (importedRuleBase, baseAlias, baseLocation);
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

  ncldoc = this->getNclDocument ();
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

  ncldoc = this->getNclDocument ();
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

void
NclParser::addNodeToSwitch ( Node *switchNode, Node *node)
{
  map<string, Node *> *nodes;

  if (_switchConstituents.count (switchNode->getId ()) == 0)
    {
      _switchConstituents[switchNode->getId ()] = new map<string, Node *>();
    }

  nodes = _switchConstituents[switchNode->getId ()];
  if (nodes->count (node->getId ()) == 0)
    {
      (*nodes)[node->getId ()] = node;
    }
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
          Node *node = this->getNode (id);

          if (instanceof (ContextNode *, node))
            {
              this->posCompileContext (child, (ContextNode*)node);
            }
        }
      else if (tagname ==  "switch")
        {
          string id = dom_element_get_attr(child, "id");
          Node * node = this->getNode (id);
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

// PRESENTATION SPECIFICATION
Descriptor *
NclParser::parseDescriptor (DOMElement *descriptor_element)
{
  Descriptor *descriptor = createDescriptor (descriptor_element);
  g_assert_nonnull (descriptor);

  for(DOMElement *child: dom_element_children(descriptor_element))
    {
      string tagname = dom_element_tagname(child);
      if (tagname == "descriptorParam")
        {
          string pName = dom_element_get_attr(child, "name");
          string pValue = dom_element_get_attr(child, "value");
          descriptor->addParameter (new Parameter (pName, pValue));
        }
      else
        {
          ERROR_SYNTAX_UNKNOWN_CHILD ("descriptor", tagname.c_str ());
        }
    }

  return descriptor;
}

DescriptorBase *
NclParser::parseDescriptorBase (DOMElement *elt)
{
  DescriptorBase *descBase = createDescriptorBase (elt);
  g_assert_nonnull (descBase);

  for (DOMElement *child: dom_element_children(elt))
    {
      string tagname = dom_element_tagname(child);
      if (tagname == "importBase")
        {
          addImportBaseToDescriptorBase (descBase, child);
        }
      else if (tagname == "descriptorSwitch")
        {
          DescriptorSwitch *descSwitch = this->parseDescriptorSwitch (child);
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
          ERROR_SYNTAX_UNKNOWN_CHILD ("descriptorBase", tagname.c_str ());
        }
    }

  return descBase;
}

void
NclParser::addImportBaseToDescriptorBase (
    DescriptorBase *descriptorBase, DOMElement *childObject)
{
  string baseAlias, baseLocation;
  NclDocument *importedDocument, *thisDocument;
  DescriptorBase *importedDescriptorBase;
  RegionBase *regionBase;

  map<int, RegionBase *> *regionBases;
  map<int, RegionBase *>::iterator i;

  RuleBase *ruleBase;

  // get the external base alias and location
  baseAlias = dom_element_get_attr(childObject, "alias");
  baseLocation = dom_element_get_attr(childObject, "documentURI");

  importedDocument = this->importDocument (baseLocation);
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
  descriptorBase->addBase (importedDescriptorBase, baseAlias, baseLocation);

  // importing descriptor bases implies importing region, rule, and cost
  // function bases in order to maintain reference consistency
  thisDocument = this->getNclDocument ();
  regionBase = thisDocument->getRegionBase (0);
  if (regionBase == NULL)
    {
      regionBase = new RegionBase ("dummy");
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
      thisDocument->getRuleBase ()->addBase (ruleBase, baseAlias, baseLocation);
    }
}

DescriptorBase *
NclParser::createDescriptorBase (DOMElement *elt)
{
  return new DescriptorBase (dom_element_get_attr (elt ,"id"));
}

Descriptor *
NclParser::createDescriptor (DOMElement *elt)
{
  Descriptor *descriptor;
  NclDocument *nclDoc;
  LayoutRegion *region;
  KeyNavigation *keyNavigation;
  string src;
  FocusDecoration *focusDecoration;
  string attValue;

  descriptor = new Descriptor (dom_element_get_attr(elt, "id"));

  nclDoc = this->getNclDocument ();
  g_assert_nonnull(nclDoc);

  // region
  if (dom_element_try_get_attr(attValue, elt, "region"))
    {
      region = nclDoc->getRegion (attValue);
      if (unlikely (region == NULL))
        {
          ERROR_SYNTAX ("descriptor: bad region for descritor '%s'",
                        descriptor->getId().c_str ());
        }
      descriptor->setRegion (region);
    }

  // explicitDur
  if (dom_element_try_get_attr (attValue, elt, "explicitDur"))
    {
      descriptor->setExplicitDuration (ginga_parse_time (attValue));
    }

  if (dom_element_try_get_attr (attValue, elt, "freeze"))
    {
      ERROR_NOT_IMPLEMENTED ("freeze attribute is not supported");
    }

  // player
  if (dom_element_try_get_attr (attValue, elt, "player"))
    {
      descriptor->setPlayerName (attValue);
    }

  // key navigation attributes
  keyNavigation = new KeyNavigation ();
  descriptor->setKeyNavigation (keyNavigation);

  // a lambda to check the existence of an attribute and set the
  // keyNavigation
  typedef void (KeyNavigation::*memberf_pointer)(const string &);
  map<string, memberf_pointer> to_call = {
    {"focusIndex", &KeyNavigation::setFocusIndex},
    {"moveUp",     &KeyNavigation::setMoveUp},
    {"moveDown",   &KeyNavigation::setMoveDown},
    {"moveLeft",   &KeyNavigation::setMoveLeft},
    {"moveRight",  &KeyNavigation::setMoveRight},
  };

  for(auto a: to_call)
    if (dom_element_try_get_attr(attValue, elt, a.first))
      (keyNavigation->*to_call[a.first]) (attValue);

  focusDecoration = new FocusDecoration ();
  descriptor->setFocusDecoration (focusDecoration);
  if (dom_element_try_get_attr(src, elt, "focusSrc"))
    {
      if (!xpathisuri (src) && !xpathisabs (src))
        src = xpathbuildabs (this->getDirName (), src);
      focusDecoration->setFocusSrc (src);
    }

  if (dom_element_try_get_attr (attValue, elt, "focusBorderColor"))
    {
      focusDecoration->setFocusBorderColor (ginga_parse_color (attValue));
    }

  if (dom_element_try_get_attr(attValue, elt, "focusBorderWidth"))
    {
      focusDecoration->setFocusBorderWidth (xstrtoint (attValue, 10));
    }

  if (dom_element_try_get_attr (attValue, elt, "focusBorderTransparency"))
    {
      double alpha;
      alpha = xstrtod (attValue);
      focusDecoration->setFocusBorderTransparency (alpha);
    }

  if (dom_element_try_get_attr(src, elt, "focusSelSrc"))
    {
      if (!xpathisuri (src) && !xpathisabs (src))
        src = xpathbuildabs (this->getDirName (), src);

      focusDecoration->setFocusSelSrc (src);
    }

  if (dom_element_try_get_attr (attValue, elt, "selBorderColor"))
    {
      focusDecoration->setSelBorderColor (ginga_parse_color (attValue));
    }

  // a lambda to parse the transIn/transOut attribute value and add input
  // output transitions to descriptor
  auto parse_transInOut = [&] (const string &transInOut)
    {
      if (dom_element_try_get_attr(attValue, elt, transInOut))
      {
        TransitionBase *transBase = nclDoc->getTransitionBase ();
        if((transBase))
          {
            vector<string> transIds = xstrsplit (attValue, ';');
            for (uint i = 0; i < transIds.size(); i++)
              {
                Transition *trans
                    = transBase->getTransition (xstrstrip (transIds[i]));
                if (trans)
                  {
                    if (transInOut == "transIn")
                      descriptor->addInputTransition (trans, (int) i);
                    else if (transInOut == "transOut")
                      descriptor->addOutputTransition(trans, (int) i);
                  }
                else
                  {
                    ERROR_SYNTAX ( "transition: bad %s '%s'",
                                   transInOut.c_str (),
                                   transIds[i].c_str ());
                  }
              }
          }
      }
    };

  parse_transInOut ("transIn");  //call the above lambda for transIn attr
  parse_transInOut ("transOut"); //call the above lambda for transOut attr

  return descriptor;
}

GINGA_FORMATTER_END
