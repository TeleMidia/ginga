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

#define ERROR_SYNTAX_ELT_BAD_ATTRIBUTE(elt, name)               \
  ERROR_SYNTAX_ELT ((elt), "bad value for attribute '%s'",      \
                    string ((name)).c_str ())

#define ERROR_SYNTAX_ELT_MISSING_ATTRIBUTE(elt, name)           \
  ERROR_SYNTAX_ELT ((elt), "missing required attribute '%s'",   \
                    string ((name)).c_str ())

#define ERROR_SYNTAX_ELT_MISSING_ID(elt)\
  ERROR_SYNTAX_ELT ((elt), "missing id")

#define ERROR_SYNTAX_ELT_DUPLICATED_ID(elt, value)      \
  ERROR_SYNTAX_ELT ((elt), "missing id '%s'",           \
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

  // TODO: We do not handle min and max (these are nonsensical).

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
NclParser::parseCompoundCondition (DOMElement *elt)
{
  CompoundCondition *compoundCond =
      createCompoundCondition (elt);
  g_assert_nonnull (compoundCond);

  for ( DOMElement *child: dom_element_children(elt))
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
          ERROR_SYNTAX_ELT_UNKNOWN_CHILD (elt, child);
        }
    }

  return compoundCond;
}

AssessmentStatement *
NclParser::parseAssessmentStatement (DOMElement *elt)
{
  AssessmentStatement *assStatement =
      createAssessmentStatement (elt);
  g_assert_nonnull (assStatement);

  for (DOMElement *child: dom_element_children (elt))
    {
      string tagname = dom_element_tagname (child);
      if (tagname == "attributeAssessment")
        {
          AttributeAssessment *attrStatement;
          attrStatement = parseAttributeAssessment (child);
          g_assert_nonnull (attrStatement);
          addAttributeAssessmentToAssessmentStatement (assStatement,
                                                       attrStatement);
        }
      else if (tagname == "valueAssessment")
        {
          ValueAssessment *valueAssessment;
          valueAssessment = parseValueAssessment (child);
          g_assert_nonnull (valueAssessment);
          assStatement->setOtherAssessment (valueAssessment);
        }
      else
        {
          ERROR_SYNTAX_ELT_UNKNOWN_CHILD (elt, child);
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
NclParser::parseCompoundStatement (DOMElement *elt)
{
  CompoundStatement *compoundStatement =
      createCompoundStatement (elt);
  g_assert_nonnull (compoundStatement);

  for ( DOMElement *child: dom_element_children (elt))
    {
      string tagname = dom_element_tagname(child);

      if (tagname == "assessmentStatement")
        {
          AssessmentStatement *assStatement;
          assStatement = parseAssessmentStatement (child);
          g_assert_nonnull (assStatement);
          compoundStatement->addStatement (assStatement);
        }
      else if (tagname == "compoundStatement")
        {
          CompoundStatement *compoundStatement_child;
          compoundStatement_child = parseCompoundStatement (child);
          g_assert_nonnull (compoundStatement_child);
          compoundStatement->addStatement (compoundStatement_child);
        }
      else
        {
          ERROR_SYNTAX_ELT_UNKNOWN_CHILD (elt, child);
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

  if (dom_element_try_get_attr (value, elt, "value"))
    action->setValue (value);

  // TODO: Handle qualifier (no!), repeatDelay and repeat attributes.

  return action;
}

CompoundAction *
NclParser::parseCompoundAction (DOMElement *elt)
{
  CompoundAction *compoundAction;

  compoundAction = createCompoundAction (elt);
  g_assert_nonnull (compoundAction);

  for (DOMElement *child: dom_element_children(elt))
    {
      string tagname = dom_element_tagname(child);
      if (tagname == "simpleAction")
        {
          SimpleAction *simpleAction;
          simpleAction = parseSimpleAction (child);
          g_assert_nonnull (simpleAction);
          compoundAction->addAction (simpleAction);
        }
      else if (tagname == "compoundAction")
        {
          CompoundAction *compoundAction_child;
          compoundAction_child = parseCompoundAction (child);
          g_assert_nonnull (compoundAction_child);
          compoundAction->addAction (compoundAction_child);
        }
      else
        {
          ERROR_SYNTAX_ELT_UNKNOWN_CHILD (elt, child);
        }
    }

  return compoundAction;
}


CompoundCondition *
NclParser::createCompoundCondition (DOMElement *elt)
{
  CompoundCondition *conditionExpression;
  string attValue;

  conditionExpression = new CompoundCondition ();

  string op = dom_element_get_attr(elt, "operator");

  if (op == "and")
    {
      conditionExpression->setOperator (CompoundCondition::OP_AND);
    }
  else
    {
      conditionExpression->setOperator (CompoundCondition::OP_OR);
    }

  // delay
  if (dom_element_try_get_attr (attValue, elt, "delay"))
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
NclParser::createCompoundStatement (DOMElement *elt)
{
  string attValue;
  CompoundStatement *compoundStatement = new CompoundStatement ();

  attValue = dom_element_get_attr(elt, "operator");
  if (attValue == "and")
    {
      compoundStatement->setOperator (CompoundStatement::OP_AND);
    }
  else
    {
      compoundStatement->setOperator (CompoundStatement::OP_OR);
    }

  // testing isNegated
  if (dom_element_try_get_attr(attValue, elt, "isNegated"))
    {
      compoundStatement->setNegated (attValue == "true");
    }

  return compoundStatement;
}

CompoundAction *
NclParser::createCompoundAction (DOMElement *elt)
{
  string attValue;
  CompoundAction *actionExpression = new CompoundAction ();

  attValue = dom_element_get_attr (elt, "operator");
  if (attValue == "seq")
    {
      actionExpression->setOperator (CompoundAction::OP_SEQ);
    }
  else
    {
      actionExpression->setOperator (CompoundAction::OP_PAR);
    }

  //  delay
  if (dom_element_try_get_attr(attValue, elt, "delay"))
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

// LINKING
Bind *
NclParser::parseBind (DOMElement *elt, Link *link)
{
  Bind *bind = createBind (elt, link);
  g_assert_nonnull (bind);

  for (DOMElement *child: dom_element_children (elt))
    {
      string tagname = dom_element_tagname (child);
      if (tagname == "bindParam")
        {
          Parameter *param = parseLinkOrBindParam (child);
          g_assert_nonnull (param);
          bind->addParameter (param);
        }
      else
        {
          ERROR_SYNTAX_ELT_UNKNOWN_CHILD (elt, child);
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
NclParser::parseLink (DOMElement *elt, CompositeNode *compositeNode)
{
  Link *link = createLink (elt, compositeNode);
  g_assert_nonnull (link);

  for (DOMElement *child: dom_element_children(elt))
    {
      string tagname = dom_element_tagname(child);
      if (tagname == "linkParam")
        {
          Parameter *param = parseLinkOrBindParam (child);
          g_assert_nonnull (param);
          link->addParameter (param);
        }
      else if (tagname == "bind")
        {
          Bind *bind = parseBind (child, link);
          g_assert_nonnull (bind);
        }
      else
        {
          ERROR_SYNTAX_ELT_UNKNOWN_CHILD (elt, child);
        }
    }

  return link;
}

Bind *
NclParser::createBind (DOMElement *elt, Link *link)
{
  string component, roleId, interfaceId;
  Role *role;
  Node *anchorNode;
  NodeEntity *anchorNodeEntity;
  InterfacePoint *interfacePoint = NULL;
  NclDocument *document;
  GenericDescriptor *descriptor;

  role = _connectorLinkParsing->getRole (dom_element_get_attr (elt, "role"));
  component = dom_element_get_attr(elt, "component");

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

  if (dom_element_try_get_attr (interfaceId, elt, "interface"))
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

  if (dom_element_has_attr(elt, "descriptor"))
    {
      document = this->_doc;
      descriptor = document->getDescriptor (
            dom_element_get_attr(elt, "descriptor"));
    }
  else
    {
      descriptor = NULL;
    }

  if (role == NULL)
    {
      // &got
      if (dom_element_try_get_attr(roleId, elt, "role"))
        {
          ConditionExpression *condition;
          CompoundCondition *compoundCondition;
          AssessmentStatement *statement;
          AttributeAssessment *assessment;
          ValueAssessment *otherAssessment;

          assessment = new AttributeAssessment (roleId);
          assessment->setEventType (EventType::ATTRIBUTION);
          assessment->setAttributeType (AttributeType::NODE_PROPERTY);

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
NclParser::createLink (DOMElement *elt, CompositeNode *compositeNode)
{
  NclDocument *document = this->_doc;
  string connectorId =
      dom_element_get_attr(elt, "xconnector");

  _connectorLinkParsing = document->getConnector (connectorId);
  if (unlikely (_connectorLinkParsing == NULL))
    {
      ERROR_SYNTAX ("link: bad xconnector '%s'", connectorId.c_str ());
    }

  g_assert (instanceof (CausalConnector *, _connectorLinkParsing));

  Link *link = new CausalLink (dom_element_get_attr (elt, "id"),
                               _connectorLinkParsing);
  _composite = compositeNode;

  return link;
}

// PRESENTATION_CONTROL


Node *
NclParser::parseSwitch (DOMElement *elt)
{
  Node *switch_node = createSwitch (elt);
  if (unlikely (switch_node == NULL))
    {
      ERROR_SYNTAX ( "switch: bad parent '%s'",
                     dom_element_tagname(elt).c_str ());
    }

  for (DOMElement *child: dom_element_children (elt))
    {
      string tagname = dom_element_tagname (child);
      if ( tagname == "media")
        {
          Node *media = this->parseMedia (child);
          g_assert_nonnull (media);
          addNodeToSwitch (switch_node, media);
        }
      else if (tagname == "context")
        {
          Node *ctx = this->parseContext (child);
          g_assert_nonnull (ctx);
          addNodeToSwitch (switch_node, ctx);
        }
      else if (tagname == "switch")
        {
          Node *switch_child = this->parseSwitch (child);
          g_assert_nonnull (switch_child);
          addNodeToSwitch (switch_node, switch_child);
        }
      else if (tagname == "switchPort"
               || tagname == "bindRule"
               || tagname == "defaultComponent")
        {
          // nothing to do
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
            addBindRuleToSwitch ((SwitchNode *) switch_node, child);
          }
        else if (tagname == "defaultComponent")
          {
            addDefaultComponentToSwitch ((SwitchNode*) switch_node, child);
          }
      }

  addUnmappedNodesToSwitch ((SwitchNode *) switch_node);
  return switch_node;
}

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

  node = _doc->getNode (id);
  if (unlikely (node != NULL))
    ERROR_SYNTAX ("switch '%s': duplicated id", id.c_str ());

  if (dom_element_try_get_attr(attValue, switch_element, "refer"))
    {
      try
        {
          referNode = (SwitchNode *) _doc->getNode (attValue);

          if (referNode == NULL)
            {
              referNode = (SwitchNode *)_doc->getNode (attValue);
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
          SimpleCondition *cond = this->parseSimpleCondition (child);
          g_assert_nonnull (cond);
          conn->setConditionExpression (cond);
          ncond++;
        }
      else if (tag == "compoundCondition")
        {
          CompoundCondition *cond = this->parseCompoundCondition (child);
          g_assert_nonnull (cond);
          conn->setConditionExpression (cond);
          ncond++;
        }
      else if (tag == "simpleAction")
        {
          SimpleAction *act = this->parseSimpleAction (child);
          g_assert_nonnull (act);
          conn->setAction (act);
          nact++;
        }
      else if (tag == "compoundAction")
        {
          CompoundAction *act = this->parseCompoundAction (child);
          g_assert_nonnull (act);
          conn->setAction (act);
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


// Body.

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
          continue;               // skip
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

GINGA_FORMATTER_END
