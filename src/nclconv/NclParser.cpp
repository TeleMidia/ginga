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
#include "NclParser.h"

GINGA_NCLCONV_BEGIN

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
  XMLCh *attr_xmlch = XMLString::transcode(attr.c_str());
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
  XMLCh *attr_xmlch = XMLString::transcode(attr.c_str());
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
  XMLCh *attr_xmlch = XMLString::transcode(attr.c_str());
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
  XMLCh *attr_name = XMLString::transcode (attr.c_str());
  XMLCh *attr_value = XMLString::transcode(value.c_str ());

  element->setAttribute(attr_name, attr_value);

  XMLString::release(&attr_name);
  XMLString::release(&attr_value);
}

// Removes the atribute from DOMElement *
static void
dom_element_remove_attr (DOMElement *element, const string &attr)
{
  XMLCh *attr_name = XMLString::transcode (attr.c_str());
  element->removeAttribute(attr_name);
  XMLString::release(&attr_name);
}

#define FOR_EACH_DOM_ELEM_CHILD(X, Y) \
  for ( X = Y->getFirstElementChild(); \
        X != nullptr; \
        X = X->getNextElementSibling() )

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
dom_element_children_by_tagnames (DOMElement *el,
                                  const vector<string> &tagnames)
{
  vector <DOMElement *> vet;

  DOMElement *child;
  FOR_EACH_DOM_ELEM_CHILD(child, el)
    {
      if (std::find(tagnames.begin(), tagnames.end(), dom_element_tagname(child))
          != tagnames.end() )
        {
          vet.push_back(child);
        }
    }

  return vet;
}

NclParser::NclParser (PrivateBaseContext *pbc, DeviceLayout *deviceLayout)
{
  g_assert_nonnull(pbc);
  g_assert_nonnull(deviceLayout);

  this->_ownManager = false;
  this->_privateBaseContext = pbc;
  this->_deviceLayout = deviceLayout;
}

NclParser::~NclParser ()
{
  for(auto i : _switchConstituents)
    {
      delete i.second;
    }
}

NclDocument *
NclParser::parseRootElement (DOMElement *rootElement)
{
  string tagName = dom_element_tagname(rootElement);
  if (unlikely (tagName != "ncl"))
    syntax_error ("bad root element '%s'", tagName.c_str ());

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

DeviceLayout *
NclParser::getDeviceLayout()
{
  return this->_deviceLayout;
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

PrivateBaseContext *
NclParser::getPrivateBaseContext ()
{
  return _privateBaseContext;
}

NclDocument *
NclParser::importDocument (string &path)
{
  if (!xpathisuri (path) && !xpathisabs (path))
    path = xpathbuildabs (this->getDirName (), path);

  return (NclDocument *)(_privateBaseContext
                          ->addVisibleDocument (path, _deviceLayout));
}


// STRUCTURE
ContextNode *
NclParser::parseBody (DOMElement *body_element)
{
  ContextNode *body = createBody (body_element);
  g_assert_nonnull (body);

  for (DOMElement *child : dom_element_children(body_element) )
    {
      string tagname = dom_element_tagname(child);
      Node *node = nullptr;
      if ( tagname == "media") // node will be a media
        {
          node = parseMedia (child);
        }
      else if (tagname == "context") // node will be a nested context
        {

          node = parseContext (child);
        }
      else if (tagname == "switch") // node will be a nested switch
        {
          node = parseSwitch (child);
        }
      else
        {
           // syntax_warning ?
        }

      if (node) // media, context or switch was created then we should add it to the body
        addNodeToContext(body, node);
    }

  for (DOMElement *child :
       dom_element_children_by_tagname(body_element, "property") )
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
       dom_element_children_by_tagname(head_element, "importedDocumentBase") )
    {
      parseImportedDocumentBase (child);
    }

  for (DOMElement *child :
       dom_element_children_by_tagname(head_element, "regionBase") )
    {
      RegionBase *regionBase = parseRegionBase (child);
      if (regionBase)
        {
          nclDoc->addRegionBase(regionBase);
        }
    }

  for (DOMElement *child :
       dom_element_children_by_tagname(head_element, "ruleBase") )
    {
      RuleBase *ruleBase = parseRuleBase (child);
      if (ruleBase)
        {
          nclDoc->setRuleBase (ruleBase);
          break;
        }
    }

  for (DOMElement *child :
       dom_element_children_by_tagname(head_element, "transitionBase") )
    {
      TransitionBase *transBase = parseTransitionBase (child);
      if (transBase)
        {
          nclDoc->setTransitionBase (transBase);
          break;
        }
    }

  for (DOMElement *child :
       dom_element_children_by_tagname(head_element, "descriptorBase") )
    {
      DescriptorBase *descBase = this->parseDescriptorBase (child);
      if (descBase)
        {
          nclDoc->setDescriptorBase (descBase);
          break;
        }
    }

  for (DOMElement *child :
       dom_element_children_by_tagname(head_element, "connectorBase") )
    {
      ConnectorBase *connBase = parseConnectorBase (child);
      if (connBase)
        {
          nclDoc->setConnectorBase(connBase);
          break;
        }
    }

  for (DOMElement *child :
       dom_element_children_by_tagname(head_element, "meta") )
    {
      Meta *meta = parseMeta (child);
      if (meta)
        {
          nclDoc->addMetainformation (meta);
          break;
        }
    }

  for (DOMElement *child :
       dom_element_children_by_tagname(head_element, "metadata") )
    {
      Metadata *metadata = parseMetadata (child);
      if (metadata)
        {
          nclDoc->addMetadata (metadata);
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
       dom_element_children_by_tagname(ncl_element, "head") )
    {
      parseHead (child);
    }

  for (DOMElement *child :
       dom_element_children_by_tagname(ncl_element, "body") )
    {
      ContextNode *body = parseBody (child);
      if (body)
        {
          posCompileBody (child, body);
          // addBodyToNcl (parentObject, body);
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
  // criar composicao a partir do elemento body do documento ncl
  // fazer uso do nome da composicao body que foi atribuido pelo
  // compilador
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

  if (composition->instanceOf ("SwitchNode"))
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
        if (node->instanceOf ("ReferNode"))
          {
            referredNode = ((ReferNode *)node)->getReferredEntity ();
            if (referredNode != NULL)
              {
                if (referredNode->instanceOf ("ReferredNode"))
                  {
                    nodeEntity = (NodeEntity *)(getNode (
                                                  referredNode->getId ()));

                    if (nodeEntity)
                      {
                        ((ReferNode *)node)
                            ->setReferredEntity (nodeEntity->getDataEntity ());
                      }
                    else
                      {
                        syntax_error ("media: bad refer '%s'",
                                      referredNode->getId ().c_str ());
                      }
                  }
              }
          }
        else if (node->instanceOf ("CompositeNode"))
          {
            solveNodeReferences ((CompositeNode *)node);
          }
      }
  }

  if (deleteNodes)
    {
      delete nodes;
    }
}

void *
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
NclParser::parseMedia (DOMElement *media_element)
{
  Node *media = createMedia (media_element);
  g_assert_nonnull (media);

  for (DOMElement *child: dom_element_children(media_element))
    {
      string tagname = dom_element_tagname(child);
      if (tagname == "area")
        {
          Anchor *area = this->parseArea (child);
          if (area)
            {
              if (media->instanceOf("ContentNode"))
                // createMedia can also return a ReferNode in which case the
                // following should be skipped
                {
                  addAnchorToMedia ((ContentNode*)media, area);
                }
            }
        }
      else if (tagname == "property")
        {
          PropertyAnchor *prop = this->parseProperty (child);
          if (prop)
            {
              if (media->instanceOf("ContentNode"))
                // createMedia can also return a ReferNode in which case the
                // following should be skipped
                {
                  addAnchorToMedia ((ContentNode*)media, prop);
                }
            }
        }
      else
        {
          syntax_warning( "'%s' is not known as child of 'media'."
                          " It will be ignored.",
                          tagname.c_str() );
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
          node = this->parseSwitch (child);
        }
      else
        {
          // syntax warning ?
        }

      if (node) // a media, context or switch was created so we need to add it to the current context
        {
          addNodeToContext (context, node);
        }
    }


  for (DOMElement *element:
       dom_element_children_by_tagname (parentElement, "property"))
    {
      PropertyAnchor *prop = this->parseProperty (element);

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
  if (context->instanceOf ("ContextNode"))
    {
      ((ContextNode *)context)->addAnchor (property);
    }
  else if (context->instanceOf ("ReferNode"))
    {
      ((ReferNode *)context)->addAnchor (property);
    }
}

void
NclParser::addNodeToContext (ContextNode *contextNode, Node *node)
{
  // adicionar um noh aa composicao
  contextNode->addNode (node);
}

void
NclParser::addNodeToContext (Entity *context, Node *node)
{
  if (context->instanceOf ("ContextNode"))
    {
      addNodeToContext ((ContextNode *)context, node);
    }
}

void
NclParser::addLinkToContext (ContextNode *context, Link *link)
{
  vector<Role *> *roles = link->getConnector ()->getRoles ();
  g_assert_nonnull(roles);

  for (Role *role: *roles)
    {
      unsigned int min = role->getMinCon ();
      unsigned int max = role->getMaxCon ();

      if (link->getNumRoleBinds (role) < min)
        {
          syntax_error ("link: too few binds for role '%s': %u",
                        role->getLabel ().c_str (), min);
        }
      else if (max > 0 && (link->getNumRoleBinds (role) > max))
        {
          syntax_error ("link: too many binds for role '%s': %u",
                        role->getLabel ().c_str (), max);
          return;
        }
    }

  delete roles;
  context->addLink (link);
}

void
NclParser::addAnchorToMedia (ContentNode *contentNode, Anchor *anchor)
{
  if (unlikely (contentNode->getAnchor (anchor->getId ()) != NULL))
    {
      syntax_error ("media '%s': duplicated area '%s'",
                    contentNode->getId ().c_str (),
                    anchor->getId ().c_str ());
    }

  contentNode->addAnchor (anchor);
}

Node *
NclParser::createContext (DOMElement *parentElement)
{
  NclDocument *document;
  string id, attValue;
  Node *node;
  Entity *referNode = NULL;
  ContextNode *context;
  GenericDescriptor *descriptor;

  if (unlikely (!dom_element_has_attr(parentElement, "id")))
    syntax_error ("context: missing id");

  id = dom_element_get_attr(parentElement, "id");

  node = this->getNode (id);
  if (unlikely (node != NULL))
    syntax_error ("context '%s': duplicated id", id.c_str ());

  if (dom_element_try_get_attr(attValue, parentElement, "refer"))
    {
      try
      {
        referNode = (ContextNode *)this->getNode (attValue);
        if (referNode)
          {
            document = this->getNclDocument ();
            referNode = (ContextNode *)(document->getNode (attValue));
            if (referNode == NULL)
              {
                referNode = (Entity *)(new ReferredNode (attValue, (void *)parentElement));
              }
          }
      }
      catch (...)
      {
        syntax_error ("context '%s': bad refer '%s'",
                      id.c_str (), attValue.c_str ());
      }

      node = new ReferNode (id);
      ((ReferNode *)node)->setReferredEntity (referNode);

      return node;
    }

  context = new ContextNode (id);
  if (dom_element_try_get_attr(attValue, parentElement, "descriptor"))
    {
      // adicionar um descritor a um objeto de midia
      document = this->getNclDocument ();
      descriptor = document->getDescriptor (attValue);
      if (descriptor != NULL)
        {
          context->setDescriptor (descriptor);
        }
      else
        {
          syntax_error ("context '%s': bad descriptor '%s'", id.c_str (),
                        attValue.c_str ());
        }
    }

  return context;
}

ContextNode *
NclParser::posCompileContext (DOMElement *context_element, ContextNode *context)
{
  g_assert_nonnull(context);

  for(DOMElement *child: dom_element_children(context_element))
    {
      string tagname = dom_element_tagname(child);
      if (tagname == "context")
        {
          string id = dom_element_get_attr(child, "id");
          Node *node = context->getNode (id);

          if (unlikely (node == NULL))
            {
              syntax_error ("bad context '%s'", id.c_str ());
            }
          else if (node->instanceOf ("ContextNode"))
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
              syntax_error ("bad switch '%s'", id.c_str ());
            }
          else if (node->instanceOf ("SwitchNode"))
            {
              this->posCompileSwitch (child, (SwitchNode*)node);
            }
        }
    }

  for(DOMElement *child:
      dom_element_children_by_tagname (context_element, "link"))
    {
      Link *link = this->parseLink (child, context);
      if (link)
        {
          addLinkToContext (context, link);
        }
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
NclParser::createMedia (DOMElement *parentElement)
{
  string attValue, id;
  NclDocument *document;
  Node *node;
  Entity *referNode;
  GenericDescriptor *descriptor;

  if (unlikely (!dom_element_has_attr(parentElement, "id")))
    syntax_error ("media: missing id");

  id = dom_element_get_attr(parentElement, "id");

  node = this->getNode (id);
  if (unlikely (node != NULL))
    syntax_error ("media '%s': duplicated id", id.c_str ());

  if (dom_element_try_get_attr(attValue, parentElement, "refer"))
    {
      try
      {
        referNode = (ContentNode *) this->getNode (attValue);
        if (referNode == NULL)
          {
            document = this->getNclDocument ();
            referNode = (ContentNode *)document->getNode (attValue);
            if (referNode == NULL)
              {
                referNode = new ReferredNode (attValue, (void *)parentElement);
              }
          }
      }
      catch (...)
      {
        syntax_error ("media '%s': bad refer '%s'",
                      id.c_str (), attValue.c_str ());
      }

      node = new ReferNode (id);
      if (dom_element_try_get_attr(attValue, parentElement, "instance"))
        {
          ((ReferNode *)node)->setInstanceType (attValue);
        }
      ((ReferNode *)node)->setReferredEntity (referNode);

      return node;
    }

  node = new ContentNode (id, NULL, "");

  if (dom_element_try_get_attr(attValue, parentElement, "type"))
    {
      ((ContentNode *)node)->setNodeType (attValue);
    }

  if (dom_element_try_get_attr(attValue, parentElement, "src"))
    {
      if (unlikely (attValue == ""))
        syntax_error ("media '%s': missing src", id.c_str ());

      if (!xpathisuri (attValue) && !xpathisabs (attValue))
        attValue = xpathbuildabs (this->getDirName (), attValue);

      ((ContentNode *)node)->setContent (new AbsoluteReferenceContent (attValue));
    }

  if (dom_element_try_get_attr(attValue, parentElement, "descriptor"))
    {
      document = this->getNclDocument ();
      descriptor = document->getDescriptor (attValue);
      if (descriptor != NULL)
        {
          ((ContentNode *)node)->setDescriptor (descriptor);
        }
      else
        {
          syntax_error ("media '%s': bad descriptor '%s'",
                        id.c_str (), attValue.c_str ());
        }
    }
  return node;
}

// IMPORT
void
NclParser::parseImportedDocumentBase (DOMElement *parentElement)
{
  g_assert_nonnull (parentElement);

  for (DOMElement *child:
       dom_element_children_by_tagname(parentElement, "importNCL"))
    {
      addImportNCLToImportedDocumentBase (child);
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

// META INFORMATION
Meta *
NclParser::parseMeta (DOMElement *meta_element)
{
  string name, content;

  if (!dom_element_try_get_attr(name, meta_element, "name"))
    {
      syntax_error ("meta: missing name");
    }

  if (!dom_element_try_get_attr(content, meta_element, "content"))
    {
      syntax_error ("meta: missing content");
    }

  return new Meta (name, deconst (void *, content.c_str ()));
}

Metadata *
NclParser::parseMetadata (arg_unused (DOMElement *parentElement))
{
  return new Metadata ();
}

// TRANSITION
TransitionBase *
NclParser::parseTransitionBase (DOMElement *transBase_element)
{
  TransitionBase *transBase = new TransitionBase (
        dom_element_get_attr(transBase_element, "id") );
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
          syntax_warning( "'%s' is not known as child of 'transitionBase'. "
                          "It will be ignored.",
                          tagname.c_str() );
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
      int subtype = TransitionUtil::getSubtypeCode (type, attValue);
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
      double dur = xstrtod (attValue.substr (0, attValue.length () - 1));
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
      short direction = TransitionUtil::getDirectionCode (attValue);
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
      color = new SDL_Color ();
      ginga_color_input_to_sdl_color(attValue, color);
      transition->setBorderColor (color);
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

// CONNECTORS
SimpleCondition *
NclParser::parseSimpleCondition (DOMElement *simpleCond_element)
{
  SimpleCondition *conditionExpression;
  string attValue, roleLabel;

  roleLabel = dom_element_get_attr(simpleCond_element, "role");

  conditionExpression = new SimpleCondition (roleLabel);

  compileRoleInformation (conditionExpression, simpleCond_element);

  // transition
  if (dom_element_try_get_attr(attValue, simpleCond_element, "transition"))
    {
      conditionExpression->setTransition (
            EventUtil::getTransitionCode (attValue));
    }

  // param
  if (conditionExpression->getEventType () == EventUtil::EVT_SELECTION)
    {
      if (dom_element_try_get_attr(attValue, simpleCond_element, "key"))
        {
          conditionExpression->setKey (attValue);
        }
    }

  // qualifier
  if (dom_element_try_get_attr(attValue, simpleCond_element, "qualifier"))
    {
      if (attValue == "or")
        {
          conditionExpression->setQualifier (CompoundCondition::OP_OR);
        }
      else
        {
          conditionExpression->setQualifier (CompoundCondition::OP_AND);
        }
    }

  // delay
  if (dom_element_try_get_attr(attValue, simpleCond_element, "delay"))
    {
      if (attValue[0] == '$')
        {
          conditionExpression->setDelay (attValue);
        }
      else
        {
          double delayValue;
          delayValue = xstrtod (
                           attValue.substr (0, (attValue.length () - 1)))
                       * 1000;

          conditionExpression->setDelay (xstrbuild ("%d", (int) delayValue));
        }
    }

  return conditionExpression;
}

CompoundCondition *
NclParser::parseCompoundCondition (DOMElement *compoundCond_element)
{
  CompoundCondition *compoundCond =
      createCompoundCondition (compoundCond_element);
  g_assert_nonnull (compoundCond);

  for ( DOMElement *child: dom_element_children(compoundCond_element) )
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
      else if ( tagname == "assessmentStatement" )
        {
          AssessmentStatement *assessmentStatement =
              parseAssessmentStatement (child);

          if (assessmentStatement)
            {
              compoundCond->addConditionExpression (assessmentStatement);
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
          syntax_warning( "'%s' is not known as child of 'compoundCondition'."
                          " It will be ignored.",
                          tagname.c_str() );
        }
    }

  return compoundCond;
}

AssessmentStatement *
NclParser::parseAssessmentStatement (DOMElement *assessmentStatement_element)
{
  AssessmentStatement *assessmentStatement =
      createAssessmentStatement (assessmentStatement_element);
  g_assert_nonnull (assessmentStatement);

  for ( DOMElement *child: dom_element_children(assessmentStatement_element) )
    {
      string tagname = dom_element_tagname(child);
      if (tagname == "attributeAssessment")
        {
          AttributeAssessment *attributeAssessment =
              parseAttributeAssessment (child);
          if (attributeAssessment)
            {
              addAttributeAssessmentToAssessmentStatement (assessmentStatement,
                                                           attributeAssessment);
            }
        }
      else if (tagname == "valueAssessment")
        {
          ValueAssessment *valueAssessment = parseValueAssessment (child);
          if (valueAssessment)
            {
              assessmentStatement->setOtherAssessment (valueAssessment);
            }
        }
      else
        {
          syntax_warning( "'%s' is not known as child of 'assessmentStatement'."
                          " It will be ignored.",
                          tagname.c_str() );
        }
    }

  return assessmentStatement;
}

AttributeAssessment *
NclParser::parseAttributeAssessment (DOMElement *attributeAssessment_element)
{
  AttributeAssessment *attributeAssessment;
  string attValue;

  string roleLabel = dom_element_get_attr(attributeAssessment_element, "role");

  attributeAssessment = new AttributeAssessment (roleLabel);

  // event type
  if (dom_element_try_get_attr(attValue, attributeAssessment_element, "eventType"))
    {
      attributeAssessment->setEventType (EventUtil::getTypeCode (attValue));
    }

  // event type
  if (dom_element_try_get_attr(attValue, attributeAssessment_element, "attributeType"))
    {
      attributeAssessment->setAttributeType (
            EventUtil::getAttributeTypeCode (attValue));
    }

  // parameter
  if (attributeAssessment->getEventType () == EventUtil::EVT_SELECTION)
    {
      if (dom_element_try_get_attr(attValue, attributeAssessment_element, "key"))
        {
          attributeAssessment->setKey (attValue);
        }
    }

  // testing offset
  if (dom_element_try_get_attr(attValue, attributeAssessment_element, "offset"))
    {
      attributeAssessment->setOffset (attValue);
    }

  return attributeAssessment;
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

  for ( DOMElement *child: dom_element_children(compoundStatement_element) )
    {
      string tagname = dom_element_tagname(child);

      if (tagname == "assessmentStatement")
        {
          AssessmentStatement *assessmentStatement =
              parseAssessmentStatement (child);
          if (assessmentStatement)
            {
              compoundStatement->addStatement (assessmentStatement);
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
          syntax_warning( "'%s' is not known as child of 'compoundStatement'."
                          " It will be ignored.",
                          tagname.c_str() );
        }
    }

  return compoundStatement;
}

SimpleAction *
NclParser::parseSimpleAction (DOMElement *simpleAction_element)
{
  SimpleAction *actionExpression;
  string attValue;

  attValue = dom_element_get_attr(simpleAction_element, "role");

  actionExpression = new SimpleAction (attValue);

  // transition
  if (dom_element_try_get_attr(attValue, simpleAction_element, "actionType"))
    {
      actionExpression->setActionType (
            SimpleAction::stringToActionType (attValue) );
    }

  if (dom_element_try_get_attr(attValue, simpleAction_element, "eventType"))
    {
      actionExpression->setEventType (EventUtil::getTypeCode (attValue));
    }

  // animation
  if (actionExpression->getEventType () == EventUtil::EVT_ATTRIBUTION
      && actionExpression->getActionType () == ACT_START)
    {
      Animation *animation = NULL;
      string durVal;
      string byVal;

      durVal = dom_element_get_attr(simpleAction_element, "duration");
      byVal = dom_element_get_attr(simpleAction_element, "by");

      if (durVal != "" || byVal != "")
        {
          double d;
          animation = new Animation ();

          if (durVal[0] == '$')
            {
              animation->setDuration (durVal);
            }
          else
            {
              animation->setDuration (xstrbuild ("%d", (int) xstrtimetod (durVal)));
            }

          if (_xstrtimetod (byVal, &d))
            {
              animation->setBy (xstrbuild ("%d", (int) d));
            }
          else
            {
              animation->setBy ("indefinite"); // default
            }
        }

      actionExpression->setAnimation (animation);
    }

  compileRoleInformation (actionExpression, simpleAction_element);

  if (dom_element_try_get_attr(attValue, simpleAction_element, "qualifier"))
    {
      if (attValue == "seq")
        {
          actionExpression->setQualifier (CompoundAction::OP_SEQ);
        }
      else
        { // any
          actionExpression->setQualifier (CompoundAction::OP_PAR);
        }
    }

  // testing delay
  if (dom_element_try_get_attr(attValue, simpleAction_element, "delay"))
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

  //  testing repeatDelay
  if (dom_element_try_get_attr(attValue, simpleAction_element, "repeatDelay"))
    {
      actionExpression->setDelay (attValue);  // is that right ?
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

  // repeat
  if (dom_element_try_get_attr(attValue, simpleAction_element, "repeat"))
    {
      if (attValue == "indefinite")
        {
          // This is insane :@
          actionExpression->setRepeat (xstrbuild ("%d", 2 ^ 30));
        }
      else
        {
          actionExpression->setRepeat (attValue);
        }
    }

  // testing value
  if (dom_element_try_get_attr(attValue, simpleAction_element, "value"))
    {
      actionExpression->setValue (attValue);
    }

  // returning action expression
  return actionExpression;
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
          syntax_warning( "'%s' is not known as child of 'compoundAction'."
                          " It will be ignored.",
                          tagname.c_str() );
        }
    }

  return compoundAction;
}

Parameter *
NclParser::parseConnectorParam (DOMElement *connectorParam_element)
{
  Parameter *param;
  param = new Parameter (
        dom_element_get_attr(connectorParam_element, "name"),
        dom_element_get_attr(connectorParam_element, "type") );

  return param;
}

CausalConnector *
NclParser::parseCausalConnector (DOMElement *causalConnector_element)
{
  // pre-compile attributes
  CausalConnector *causalConnector =
      createCausalConnector (causalConnector_element);
  g_assert_nonnull (causalConnector);

  for (DOMElement *child: dom_element_children(causalConnector_element) )
    {
      string tagname = dom_element_tagname(child);

      if (tagname == "simpleCondition")
        {
          SimpleCondition *simpleCondition = parseSimpleCondition (child);
          if (simpleCondition)
            {
              causalConnector->setConditionExpression (simpleCondition);
            }
        }
      else if (tagname == "simpleAction")
        {
          SimpleAction *simpleAction = parseSimpleAction (child);
          if (simpleAction)
            {
              causalConnector->setAction (simpleAction);
            }
        }
      else if (tagname == "compoundAction")
        {
          CompoundAction *compoundAction = parseCompoundAction (child);

          if (compoundAction)
            {
              causalConnector->setAction (compoundAction);
            }
        }
      else if (tagname == "connectorParam")
        {
          Parameter *param = parseConnectorParam (child);

          if (param)
            {
              causalConnector->addParameter (param);
            }
        }
      else if (tagname == "compoundCondition")
        {
          CompoundCondition *compoundCond = parseCompoundCondition (child);

          if (compoundCond)
            {
              causalConnector->setConditionExpression (compoundCond);
            }
        }
      else
        {
          syntax_warning( "'%s' is not known as child of 'causalConnector'."
                          " It will be ignored.",
                          tagname.c_str() );
        }
    }

  return causalConnector;
}

ConnectorBase *
NclParser::parseConnectorBase (DOMElement *connectorBase_element)
{
  ConnectorBase *connectorBase =
      createConnectorBase (connectorBase_element);
  g_assert_nonnull (connectorBase);

  for (DOMElement *child: dom_element_children(connectorBase_element))
    {
      string tagname = dom_element_tagname(child);
      if (tagname == "importBase")
        {
          addImportBaseToConnectorBase (connectorBase, child);
        }
      else if (tagname ==  "causalConnector")
        {
          CausalConnector *causalConnector = parseCausalConnector (child);
          if (causalConnector)
            {
              connectorBase->addConnector (causalConnector);
            }
        }
      else
        {
          syntax_warning( "'%s' is not known as child of 'connectorBase'."
                          " It will be ignored.",
                          tagname.c_str() );
        }
    }

  return connectorBase;
}

void
NclParser::addImportBaseToConnectorBase (ConnectorBase *connectorBase,
                                         DOMElement *childObject)
{
  string baseAlias, baseLocation;
  NclDocument *importedDocument;
  ConnectorBase *importedConnectorBase;

  // get the external base alias and location
  baseAlias = dom_element_get_attr(childObject, "alias");
  baseLocation = dom_element_get_attr(childObject, "documentURI");

  importedDocument = this->importDocument (baseLocation);
  if (unlikely (importedDocument == NULL))
    {
      syntax_error ("importBase '%s': bad documentURI '%s'",
                    baseAlias.c_str (),
                    baseLocation.c_str ());
    }

  importedConnectorBase = importedDocument->getConnectorBase ();
  if (unlikely (importedConnectorBase == NULL))
    {
      syntax_error ("importBase '%s': no connector base in '%s'",
                    baseAlias.c_str (),
                    baseLocation.c_str ());
    }

  connectorBase->addBase (importedConnectorBase, baseAlias, baseLocation);
}

CausalConnector *
NclParser::createCausalConnector (DOMElement *causalConnector_element)
{
  string connectorId = dom_element_get_attr(causalConnector_element, "id");
  CausalConnector *causalConn = new CausalConnector (connectorId);

//  this->_connector = causalConn;

  return causalConn;
}

ConnectorBase *
NclParser::createConnectorBase (DOMElement *parentElement)
{
  ConnectorBase *connBase;
  string connBaseId = dom_element_get_attr(parentElement, "id");
  connBase = new ConnectorBase (connBaseId);
  return connBase;
}

void
NclParser::compileRoleInformation (Role *role, DOMElement *role_element)
{
  string attValue;
  // event type
  if (dom_element_try_get_attr(attValue, role_element, "eventType"))
    {
      role->setEventType (EventUtil::getTypeCode (attValue));
    }

  //  cardinality
  if (dom_element_try_get_attr(attValue, role_element, "min"))
    {
      role->setMinCon ((xstrtoint (attValue, 10)));
    }

  if (dom_element_try_get_attr(attValue, role_element, "max"))
    {
      if (attValue == "unbounded")
        {
          role->setMaxCon (Role::UNBOUNDED);
        }
      else
        {
          role->setMaxCon (xstrtoint (attValue, 10));
        }
    }
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
      if (attValue[0] == '$')
        {
          conditionExpression->setDelay (attValue);
        }
      else
        {
          double delayValue = xstrtod (attValue.substr (
                                  0, (attValue.length () - 1)))
                              * 1000;

          conditionExpression->setDelay (xstrbuild ("%d", (int) delayValue));
        }
    }

  return conditionExpression;
}


AssessmentStatement *
NclParser::createAssessmentStatement (DOMElement *assessmentStatement_element)
{
  AssessmentStatement *assessmentStatement;
  string attValue;

  if (dom_element_try_get_attr(attValue, assessmentStatement_element, "comparator"))
    {
      assessmentStatement
          = new AssessmentStatement (Comparator::fromString (attValue));
    }
  else
    {
      assessmentStatement = new AssessmentStatement (Comparator::CMP_EQ);
    }

  return assessmentStatement;
}

CompoundStatement *
NclParser::createCompoundStatement (DOMElement *parentElement)
{
  string attValue;
  CompoundStatement *compoundStatement = new CompoundStatement ();

  attValue = dom_element_get_attr(parentElement, "operator");
  if (attValue == "and")
    {
      compoundStatement->setOperator (CompoundStatement::OP_AND);
    }
  else
    {
      compoundStatement->setOperator (CompoundStatement::OP_OR);
    }

  // testing isNegated
  if (dom_element_try_get_attr(attValue, parentElement, "isNegated"))
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

  //  testar delay
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
    AssessmentStatement *asssessmentStatement,
    AttributeAssessment *attributeAssessment)
{
  if (asssessmentStatement->getMainAssessment () == NULL)
    {
      asssessmentStatement->setMainAssessment (attributeAssessment);
    }
  else
    {
      asssessmentStatement->setOtherAssessment (attributeAssessment);
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
      syntax_error ("switchPort: bad parent '%s'",
                    dom_element_tagname(switchPort_element).c_str());
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
  Port *port;

  switchElement = (DOMElement *)parent->getParentNode ()->getParentNode (); // FIXME: this is not safe!

  string id = dom_element_get_attr(switchElement, "id");
  string component = dom_element_get_attr(parent, "component");

  switchNode = (SwitchNode *)this->getNode (id); // FIXME: not safe!
  mappingNode = switchNode->getNode (component);

  if (unlikely (mappingNode == NULL))
    syntax_error ("mapping: bad component '%s'", component.c_str ());

  mappingNodeEntity = (NodeEntity *)mappingNode->getDataEntity (); // FIXME: this is not safe!

  string interface;
  if (dom_element_try_get_attr(interface, parent, "interface"))
    {
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
    syntax_error ("mapping: bad interface '%s'", interface.c_str());

  port = new Port (switchPort->getId (), mappingNode, interfacePoint);

  return port;
}

Anchor *
NclParser::parseArea (DOMElement *parent)
{
  string anchorId;
  string position, anchorLabel;
  Anchor *anchor;

  if (unlikely (!dom_element_has_attr(parent, "id")))
    syntax_error ("area: missing id");

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
    syntax_error ("property: missing name");

  attributeName = dom_element_get_attr(parent, "name");

  anchor = new PropertyAnchor (attributeName);
  if (dom_element_try_get_attr(attributeValue, parent, "value"))
    {
      anchor->setPropertyValue (attributeValue);
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
    syntax_error ("port: missing id");

  id = dom_element_get_attr(parent, "id");

  if (unlikely (context->getPort (id) != NULL))
    syntax_error ("port '%s': duplicated id", id.c_str ());

  if (!unlikely (dom_element_has_attr (parent, "component")))
    syntax_error ("port '%s': missing component", id.c_str ());

  attValue = dom_element_get_attr(parent, "component");

  portNode = context->getNode (attValue);
  if (unlikely (portNode == NULL))
    {
      syntax_error ("port '%s': bad component '%s'", id.c_str (),
                    attValue.c_str ());
    }

  portNodeEntity = (NodeEntity *)portNode->getDataEntity ();
  if (!dom_element_has_attr(parent, "interface"))
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
      attValue = dom_element_get_attr(parent, "interface");

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
      attValue = dom_element_get_attr(parent, "interface");
      syntax_error ("port '%s': bad interface '%s'", id.c_str (),
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
          long int x1, y1, x2, y2;
          sscanf (coords.c_str (), "%ld,%ld,%ld,%ld", &x1, &y1, &x2, &y2);
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
  double begVal, endVal;
  short firstSyntax, lastSyntax;

  if (dom_element_has_attr(areaElement, "begin")
      || dom_element_has_attr(areaElement, "end"))
    {
      if (dom_element_try_get_attr(begin, areaElement ,"begin"))
        {
          begVal = xstrtimetod (begin) * 100;
        }
      else
        {
          begVal = 0;
        }

      if (dom_element_try_get_attr(end, areaElement, "end"))
        {
          endVal = xstrtimetod (end) * 1000;
        }
      else
        {
          endVal = IntervalAnchor::OBJECT_DURATION;
        }

      if (xnumeq (endVal, IntervalAnchor::OBJECT_DURATION) || endVal > begVal)
        {
          anchor = new RelativeTimeIntervalAnchor (
              dom_element_get_attr(areaElement, "id"),
              begVal, endVal);
        }
    }

  // region delimeted through sample identifications
  if (dom_element_has_attr(areaElement, "first")
      || dom_element_has_attr(areaElement, "last"))
    {
      begVal = 0;
      endVal = IntervalAnchor::OBJECT_DURATION;
      firstSyntax = ContentAnchor::CAT_NPT;
      lastSyntax = ContentAnchor::CAT_NPT;

      if (dom_element_try_get_attr(begin, areaElement, "first"))
        {
          if (begin.find ("s") != std::string::npos)
            {
              firstSyntax = ContentAnchor::CAT_SAMPLES;
              begVal = xstrtod (begin.substr (0, begin.length () - 1));
            }
          else if (begin.find ("f") != std::string::npos)
            {
              firstSyntax = ContentAnchor::CAT_FRAMES;
              begVal = xstrtod (begin.substr (0, begin.length () - 1));
            }
          else if (begin.find ("npt") != std::string::npos
                   || begin.find ("NPT") != std::string::npos)
            {
              firstSyntax = ContentAnchor::CAT_NPT;
              begVal = xstrtod (begin.substr (0, begin.length () - 3));
            }
        }

      if (dom_element_try_get_attr(end, areaElement, "last"))
        {
          if (end.find ("s") != std::string::npos)
            {
              lastSyntax = ContentAnchor::CAT_SAMPLES;
              endVal = xstrtod (end.substr (0, end.length () - 1));
            }
          else if (end.find ("f") != std::string::npos)
            {
              lastSyntax = ContentAnchor::CAT_FRAMES;
              endVal = xstrtod (end.substr (0, end.length () - 1));
            }
          else if (end.find ("npt") != std::string::npos
                   || end.find ("NPT") != std::string::npos)
            {
              lastSyntax = ContentAnchor::CAT_NPT;
              endVal = xstrtod (end.substr (0, end.length () - 3));
            }
        }

      anchor = new SampleIntervalAnchor (
            dom_element_get_attr(areaElement, "id"), begVal, endVal);

      ((SampleIntervalAnchor *)anchor)
          ->setValueSyntax (firstSyntax, lastSyntax);
    }

  if (anchor)
    {
      anchor->setStrValues (begin, end);
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
    syntax_error ("switchPort: missing id");

  id = dom_element_get_attr(parent, "id");

  if (unlikely (switchNode->getPort (id) != NULL))
    syntax_error ("switchPort '%s': duplicated id", id.c_str ());

  switchPort = new SwitchPort (id, switchNode);
  return switchPort;
}

LayoutRegion *
NclParser::parseRegion (DOMElement *region_element)
{
  LayoutRegion *region = createRegion (region_element);
  g_assert_nonnull (region);

  for (DOMElement *child:
       dom_element_children_by_tagname(region_element, "region"))
    {
      LayoutRegion *child_region = parseRegion (child);
      if (child_region)
        {
          region->addRegion (child_region);
        }
    }

  return region;
}

// LAYOUT
RegionBase *
NclParser::parseRegionBase (DOMElement *regionBase_element)
{
  RegionBase *regionBase = createRegionBase (regionBase_element);
  g_assert_nonnull (regionBase);

  for (DOMElement *child:
       dom_element_children(regionBase_element) )
    {
      string tagname = dom_element_tagname(child);
      if (tagname == "importBase")
        {
          addImportBaseToRegionBase (regionBase, child);
        }
      else if (tagname == "region")
        {
          LayoutRegion *region = parseRegion (child);
          if (region)
            {
              regionBase->addRegion (region);
            }
        }
      else
        {
          syntax_warning( "'%s' is not known as child of 'regionBase'."
                          " It will be ignored.",
                          tagname.c_str() );
        }
    }

  return regionBase;
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
NclParser::createRegionBase (DOMElement *parentElement)
{
  string device;
  RegionBase *regionBase;

  regionBase = new RegionBase (dom_element_get_attr(parentElement, "id"),
                               getDeviceLayout());

  // device attribute
  if (dom_element_try_get_attr(device, parentElement, "device"))
    {
      string mapId = dom_element_get_attr(parentElement, "region");
      regionBase->setDevice (device , mapId);
    }
  else
    {
      regionBase->setDevice (getDeviceLayout()->getLayoutName (), "");
    }

  return regionBase;
}

static void
set_perc_or_px (DOMElement *el, const string &att, LayoutRegion *region,
                bool (LayoutRegion::*setF)(double x, bool y))
{
  string att_value;
  if (dom_element_try_get_attr(att_value, el, att))
    {
      if (xstrispercent(att_value))
        ((region)->*setF)(xstrtodorpercent (att_value, NULL) * 100., true);
      else
        ((region)->*setF)(xstrtoint (att_value, 10), false);
    }
}

LayoutRegion *
NclParser::createRegion (DOMElement *region_element)
{
  string attr = dom_element_get_attr(region_element, "id");
  LayoutRegion *ncmRegion = new LayoutRegion (attr);

  if(dom_element_try_get_attr(attr, region_element, "title"))
    ncmRegion->setTitle(attr);

  set_perc_or_px(region_element, "left", ncmRegion, &LayoutRegion::setLeft);
  set_perc_or_px(region_element, "right", ncmRegion, &LayoutRegion::setRight);
  set_perc_or_px(region_element, "top", ncmRegion, &LayoutRegion::setTop);
  set_perc_or_px(region_element, "bottom", ncmRegion, &LayoutRegion::setBottom);
  set_perc_or_px(region_element, "width", ncmRegion, &LayoutRegion::setWidth);
  set_perc_or_px(region_element, "height", ncmRegion, &LayoutRegion::setHeight);

  if(dom_element_try_get_attr(attr, region_element, "zIndex"))
      ncmRegion->setZIndex (xstrtoint (attr, 10));

  return ncmRegion;
}

// LINKING
Bind *
NclParser::parseBind (DOMElement *bind_element, Link *link)
{
  Bind *bind = createBind (bind_element, link);
  g_assert_nonnull (bind);

  for (DOMElement *child: dom_element_children (bind_element))
    {
      string tagname = dom_element_tagname(child);
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
          syntax_warning( "'%s' is not known as child of 'bind'."
                          " It will be ignored.",
                          tagname.c_str() );
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
      dom_element_get_attr(parentElement, "value") );

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
          syntax_warning( "'%s' is not known as child of 'link'."
                          " It will be ignored.",
                          tagname.c_str() );
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
  set<ReferNode *> *sInsts;
  set<ReferNode *>::iterator i;

  role = _connectorLinkParsing->getRole (dom_element_get_attr(bind_element, "role"));
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
      syntax_error ("bind: bad interface for component '%s'",
                    component.c_str ());
    }

  anchorNodeEntity = (NodeEntity *)(anchorNode->getDataEntity ());

  if (dom_element_try_get_attr(interfaceId, bind_element, "interface"))
    {
      if (anchorNodeEntity == NULL)
        {
          interfacePoint = NULL;
        }
      else
        {
          if (anchorNode->instanceOf ("ReferNode")
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
              && anchorNodeEntity->instanceOf ("CompositeNode"))
            {
              interfacePoint = ((CompositeNode *)anchorNodeEntity)
                                   ->getPort (interfaceId);
            }
          else
            {
              interfacePoint = anchorNode->getAnchor (interfaceId);
              if (interfacePoint == NULL)
                {
                  sInsts = anchorNodeEntity->getInstSameInstances ();
                  if (sInsts != NULL)
                    {
                      for (i = sInsts->begin (); i != sInsts->end (); ++i)
                        {
                          interfacePoint = (*i)->getAnchor (interfaceId);
                          if (interfacePoint != NULL)
                            {
                              break;
                            }
                        }
                    }
                }
            }
        }
    }
  else if (anchorNodeEntity != NULL)
    {
      if (anchorNode->instanceOf ("ReferNode")
          && ((ReferNode *)anchorNode)->getInstanceType () == "new")
        {
          interfacePoint = anchorNode->getAnchor (0);
          if (interfacePoint == NULL)
            {
              interfacePoint = new LambdaAnchor (anchorNode->getId ());
              anchorNode->addAnchor (0, (Anchor *)interfacePoint);
            }
        }
      else if (anchorNodeEntity->instanceOf ("Node"))
        {
          interfacePoint = anchorNodeEntity->getAnchor (0);
        }
      else
        {
          syntax_error ("bind: bad interface for entity '%s'",
                        anchorNodeEntity->getId ().c_str ());
        }
    }
  else
    {
      interfacePoint = anchorNode->getAnchor (0);
    }

  // atribui o bind ao elo (link)
  if (dom_element_has_attr(bind_element, "descriptor"))
    {
      document = this->getNclDocument ();
      descriptor = document->getDescriptor (
            dom_element_get_attr(bind_element, "descriptor") );
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
          assessment->setEventType (EventUtil::EVT_ATTRIBUTION);
          assessment->setAttributeType (EventUtil::ATT_NODE_PROPERTY);
          assessment->setMinCon (0);
          assessment->setMaxCon (Role::UNBOUNDED);

          otherAssessment = new ValueAssessment (roleId);

          statement = new AssessmentStatement (Comparator::CMP_NE);
          statement->setMainAssessment (assessment);
          statement->setOtherAssessment (otherAssessment);

          condition
              = ((CausalConnector *)_connectorLinkParsing)->getConditionExpression ();

          if (condition->instanceOf ("CompoundCondition"))
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
          syntax_error ("bind: missing role");
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
      syntax_error ("link: bad xconnector '%s'", connectorId.c_str ());
    }

  g_assert (_connectorLinkParsing->instanceOf ("CausalConnector"));

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
          syntax_warning( "'%s' is not known as child of 'ruleBase'."
                          " It will be ignored.",
                          tagname.c_str() );
        }
    }

  return ruleBase;
}

SimpleRule *
NclParser::parseRule (DOMElement *parentElement)
{
  short ruleOp = Comparator::fromString(
        dom_element_get_attr(parentElement, "comparator") );

  string var = dom_element_get_attr(parentElement, "var");
  string value = dom_element_get_attr(parentElement, "value");
  string id = dom_element_get_attr(parentElement, "id");

//  XMLString::trim (var);
//  XMLString::trim (value);
  SimpleRule *simplePresentationRule = new SimpleRule ( id, var, ruleOp, value);

  return simplePresentationRule;
}

Node *
NclParser::parseSwitch (DOMElement *switch_element)
{
  Node *switch_node = createSwitch (switch_element);
  if (unlikely (switch_node == NULL))
    {
      syntax_error ( "switch: bad parent '%s'",
                     dom_element_tagname(switch_element).c_str() );
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
      else
        {
          // syntax warning ?
        }
    }

    for (DOMElement *child: dom_element_children(switch_element))
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
NclParser::parseCompositeRule (
    DOMElement *compositeRule_element)
{
  CompositeRule *compositeRule = createCompositeRule (compositeRule_element);
  g_assert_nonnull (compositeRule);

  for (DOMElement *child: dom_element_children(compositeRule_element))
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
          syntax_warning( "'%s' is not known as child of 'compositeRule'."
                          " It will be ignored.",
                          tagname.c_str() );
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
          // syntax warning ?
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
  NclDocument *document;
  SwitchNode *switchNode;

  if (unlikely (!dom_element_has_attr(switch_element, "id")))
    syntax_error ("switch: missing id");

  id = dom_element_get_attr(switch_element, "id");

  node = this->getNode (id);
  if (unlikely (node != NULL))
    syntax_error ("switch '%s': duplicated id", id.c_str ());

  if (dom_element_try_get_attr(attValue, switch_element, "refer"))
    {
      try
        {
          referNode
              = (SwitchNode *) this->getNode (attValue);

          if (referNode == NULL)
            {
              document = this->getNclDocument ();
              referNode = (SwitchNode *)document->getNode (attValue);
              if (referNode == NULL)
                {
                  referNode
                      = new ReferredNode (attValue, (void *)switch_element);
                }
            }
        }
      catch (...)
        {
          syntax_error ("switch '%s': bad refer '%s'",
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
NclParser::createRuleBase (DOMElement *parentElement)
{
  RuleBase *ruleBase;
  ruleBase = new RuleBase (dom_element_get_attr(parentElement, "id"));

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
NclParser::addBindRuleToSwitch ( SwitchNode *switchNode,
                                                    DOMElement *bindRule)
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

void *
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

          if (node->instanceOf ("ContextNode"))
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
              syntax_error ("node '%s' should be a switch",
                            dom_element_get_attr(child, "id").c_str ());
            }
          else if (node->instanceOf ("SwitchNode"))
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
          syntax_warning( "'%s' is not known as child of 'descriptor'."
                          " It will be ignored.",
                          tagname.c_str() );
        }
    }

  return descriptor;
}

DescriptorBase *
NclParser::parseDescriptorBase (DOMElement *descriptorBase_element)
{
  DescriptorBase *descBase = createDescriptorBase (descriptorBase_element);
  g_assert_nonnull (descBase);

  for (DOMElement *child: dom_element_children(descriptorBase_element))
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
          syntax_warning( "'%s' is not known as child of 'descriptorBase'."
                          " It will be ignored.",
                          tagname.c_str() );
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
      regionBase = new RegionBase ("dummy", this->getDeviceLayout());
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
NclParser::createDescriptorBase (DOMElement *descriptorBase_element)
{
  return new DescriptorBase (
        dom_element_get_attr(descriptorBase_element ,"id") );
}

Descriptor *
NclParser::createDescriptor (DOMElement *descriptor_element)
{
  Descriptor *descriptor;
  NclDocument *nclDoc;
  LayoutRegion *region;
  KeyNavigation *keyNavigation;
  string src;
  FocusDecoration *focusDecoration;
  SDL_Color *color;
  string attValue;
  TransitionBase *transitionBase;

  descriptor = new Descriptor (dom_element_get_attr(descriptor_element, "id"));

  nclDoc = this->getNclDocument ();
  g_assert_nonnull(nclDoc);

  // region
  if (dom_element_try_get_attr(attValue, descriptor_element, "region"))
    {
      region = nclDoc->getRegion (attValue);

      if (region)
        {
          descriptor->setRegion (region);
        }
      else
        {
          syntax_error ("descriptor: bad region for descritor '%s'",
                        descriptor->getId().c_str());
        }
    }

  // explicitDur
  if (dom_element_try_get_attr(attValue, descriptor_element, "explicitDur"))
    {
      descriptor->setExplicitDuration (xstrtimetod (attValue) * 1000);
    }

  if (dom_element_try_get_attr(attValue, descriptor_element,"freeze"))
    {
      if (attValue == "true")
        {
          descriptor->setFreeze (true);
        }
      else
        {
          descriptor->setFreeze (false);
        }
    }

  // player
  if (dom_element_try_get_attr(attValue, descriptor_element, "player"))
    {
      descriptor->setPlayerName (attValue);
    }

  // key navigation attributes
  keyNavigation = new KeyNavigation ();
  descriptor->setKeyNavigation (keyNavigation);
  if (dom_element_try_get_attr(attValue, descriptor_element, "focusIndex"))
    {
      keyNavigation->setFocusIndex (attValue);
    }

  if (dom_element_try_get_attr(attValue, descriptor_element, "moveUp"))
    {
      keyNavigation->setMoveUp (attValue);
    }

  if (dom_element_try_get_attr(attValue, descriptor_element, "moveDown"))
    {
      keyNavigation->setMoveDown (attValue);
    }

  if (dom_element_try_get_attr(attValue, descriptor_element, "moveLeft"))
    {
      keyNavigation->setMoveLeft (attValue);
    }

  if (dom_element_try_get_attr(attValue, descriptor_element, "moveRight"))
    {
      keyNavigation->setMoveRight (attValue);
    }

  focusDecoration = new FocusDecoration ();
  descriptor->setFocusDecoration (focusDecoration);
  if (dom_element_try_get_attr(src, descriptor_element, "focusSrc"))
    {
      if (!xpathisuri (src) && !xpathisabs (src))
        src = xpathbuildabs (this->getDirName (), src);

      focusDecoration->setFocusSrc (src);
    }

  if (dom_element_try_get_attr(attValue, descriptor_element, "focusBorderColor"))
    {
      color = new SDL_Color ();
      ginga_color_input_to_sdl_color(attValue, color);

      focusDecoration->setFocusBorderColor (color);
    }

  if (dom_element_try_get_attr(attValue, descriptor_element, "focusBorderWidth"))
    {
      focusDecoration->setFocusBorderWidth (xstrtoint (attValue, 10));
    }

  if (dom_element_try_get_attr(attValue, descriptor_element, "focusBorderTransparency"))
    {
      double alpha;
      alpha = xstrtod (attValue);
      focusDecoration->setFocusBorderTransparency (alpha);
    }

  if (dom_element_try_get_attr(src, descriptor_element, "focusSelSrc"))
    {
      if (!xpathisuri (src) && !xpathisabs (src))
        src = xpathbuildabs (this->getDirName (), src);

      focusDecoration->setFocusSelSrc (src);
    }

  if (dom_element_try_get_attr(attValue, descriptor_element, "selBorderColor"))
    {
      color = new SDL_Color ();
      ginga_color_input_to_sdl_color (attValue, color );
      focusDecoration->setSelBorderColor ( color );
    }

  if (dom_element_try_get_attr(attValue, descriptor_element, "transIn"))
    {
      if((transitionBase = nclDoc->getTransitionBase ()))
        {
          vector<string> transIds = xstrsplit (attValue, ';');
          for (uint i = 0; i < transIds.size(); i++)
            {
              Transition *transition
                  = transitionBase->getTransition (xstrchomp (transIds[i]));
              if (transition)
                {
                  descriptor->addInputTransition (transition, i);
                }
              else
                {
                  syntax_error ( "transition: bad transOut '%s'",
                                  transIds[i].c_str () );
                }
            }
        }
    }

  if (dom_element_try_get_attr(attValue, descriptor_element, "transOut"))
    {
      if((transitionBase = nclDoc->getTransitionBase ()))
        {
          vector<string> transIds = xstrsplit (attValue, ';');
          for (uint i = 0; i < transIds.size(); i++)
            {
              Transition *transition
                  = transitionBase->getTransition (xstrchomp (transIds[i]));
              if (transition)
                {
                  descriptor->addOutputTransition(transition, i);
                }
              else
                {
                  syntax_error ( "transition: bad transOut '%s'",
                                  transIds[i].c_str () );
                }
            }
        }
    }

  return descriptor;
}

GINGA_NCLCONV_END
