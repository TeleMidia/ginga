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

GINGA_PRAGMA_DIAG_PUSH ()
GINGA_PRAGMA_DIAG_IGNORE (-Wsign-conversion)
GINGA_PRAGMA_DIAG_IGNORE (-Wundef)
#include <xercesc/framework/LocalFileInputSource.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
XERCES_CPP_NAMESPACE_USE
GINGA_PRAGMA_DIAG_POP ()

GINGA_NCL_BEGIN


// Xerces wrappers.

// Gets element tag.
static string
dom_elt_get_tag (const DOMElement *elt)
{
  char *str = XMLString::transcode (elt->getTagName ());
  string tagname (str);
  XMLString::release (&str);
  return tagname;
}

// Tests if element has a given attribute.
static bool
dom_elt_has_attribute (const DOMElement *elt, const string &name)
{
  XMLCh *xmlch = XMLString::transcode (name.c_str ());
  bool result = elt->hasAttribute (xmlch);
  XMLString::release (&xmlch);
  return result;
}

// Gets element attribute.
static string
dom_elt_get_attribute (const DOMElement *elt, const string &name)
{
  XMLCh *xmlch = XMLString::transcode (name.c_str ());
  char *str = XMLString::transcode (elt->getAttribute (xmlch));
  string value (str);
  XMLString::release (&xmlch);
  XMLString::release (&str);
  return value;
}

// Gets element attribute.
static bool
dom_elt_try_get_attribute (string &value, const DOMElement *elt,
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

// Gets element children.
static vector <DOMElement *>
dom_elt_get_children (DOMElement *elt)
{
  vector <DOMElement *> vet;
  for (DOMElement *child = elt->getFirstElementChild ();
       child != nullptr; child = child->getNextElementSibling ())
    {
      vet.push_back(child);
    }
  return vet;
}


// Common errors.

static inline string
__error_elt (const DOMElement *elt)
{
  string id = "";
  if (dom_elt_try_get_attribute (id, (elt), "id"))
    id = " id='" + id + "'";
  return "<" + dom_elt_get_tag (elt) + id + ">";
}

#define ERROR_SYNTAX_ELT(elt, fmt, ...)\
  ERROR_SYNTAX ("%s: " fmt, __error_elt ((elt)).c_str (), ## __VA_ARGS__)

#define ERROR_SYNTAX_ELT_BAD_ATTRIBUTE(elt, name)                       \
  ERROR_SYNTAX_ELT ((elt), "bad value for attribute '%s': '%s'",        \
                    string ((name)).c_str (),                           \
                    dom_elt_get_attribute ((elt), (name)).c_str ())

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
    string result = dom_elt_get_tag ((elt));                            \
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
    if (unlikely (!dom_elt_try_get_attribute (result, (elt), (name))))  \
      ERROR_SYNTAX_ELT_MISSING_ATTRIBUTE ((elt), (name));               \
    set_if_nonnull ((pvalue), result);                                  \
  }                                                                     \
  G_STMT_END

#define CHECK_ELT_ATTRIBUTE_NOT_SUPPORTED(elt, name)                    \
  G_STMT_START                                                          \
  {                                                                     \
    if (unlikely (dom_elt_has_attribute ((elt), (name))))               \
      ERROR_NOT_IMPLEMENTED ("%s: attribute '%s' is not supported",     \
                             __error_elt ((elt)).c_str (),              \
                             string ((name)).c_str ());                 \
  }                                                                     \
  G_STMT_END

#define CHECK_ELT_OPT_ATTRIBUTE(elt, name, pvalue, default)     \
  G_STMT_START                                                  \
  {                                                             \
    string result;                                              \
    if (!dom_elt_try_get_attribute (result, (elt), (name)))     \
      result = (default);                                       \
    set_if_nonnull ((pvalue), result);                          \
  }                                                             \
  G_STMT_END

#define CHECK_ELT_ID(elt, pvalue)                                       \
  G_STMT_START                                                          \
  {                                                                     \
    string result;                                                      \
    if (unlikely (!dom_elt_try_get_attribute (result, (elt), "id")))    \
      ERROR_SYNTAX_ELT_MISSING_ID ((elt));                              \
    if (unlikely (_doc->getNode (result) != nullptr))                   \
      ERROR_SYNTAX_ELT_DUPLICATED_ID ((elt), result);                   \
    set_if_nonnull ((pvalue), result);                                  \
  }                                                                     \
  G_STMT_END

#define CHECK_ELT_OPT_ID(elt, pvalue, default)                  \
  G_STMT_START                                                  \
  {                                                             \
    string result;                                              \
    if (dom_elt_try_get_attribute (result, (elt), "id"))        \
      {                                                         \
        if (unlikely (_doc->getNode (result) != nullptr))       \
          ERROR_SYNTAX_ELT_DUPLICATED_ID ((elt), result);       \
      }                                                         \
    else                                                        \
      {                                                         \
        result = (default);                                     \
      }                                                         \
    set_if_nonnull ((pvalue), result);                          \
  }                                                             \
  G_STMT_END


// Translation tables.

// Reserved conditions.
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

// Reserved actions.
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

// Maps event type name to event type code.
static map<string, EventType> event_type_table =
  {
   {"presentation", EventType::PRESENTATION},
   {"attribution", EventType::ATTRIBUTION},
   {"selection", EventType::SELECTION},
  };

// Maps condition name to condition code.
static map<string, EventStateTransition> event_transition_table =
  {
   {"starts", EventStateTransition::STARTS},
   {"stops", EventStateTransition::STOPS},
   {"aborts", EventStateTransition::ABORTS},
   {"pauses", EventStateTransition::PAUSES},
   {"resumes", EventStateTransition::RESUMES},
  };

// Maps action name to action code.
static map<string, SimpleActionType> event_action_type_table =
  {
   {"start", ACT_START},
   {"stop", ACT_STOP},
   {"abort", ACT_ABORT},
   {"pause", ACT_PAUSE},
   {"resume", ACT_RESUME},
  };


// Public.

/**
 * @brief Parses NCL document.
 * @param path Document path.
 * @param width Initial screen width (in pixels).
 * @param height Initial screen width (in pixels).
 * @return The resulting document.
 */
NclDocument *
Parser::parse (const string &path, int width, int height)
{
  Parser parser (width, height);
  return parser.parse0 (path);
}


// Private.

Parser::Parser (int width, int height)
{
  _doc = nullptr;
  g_assert_cmpint (width, >, 0);
  _width = width;
  g_assert_cmpint (height, >, 0);
  _height = height;
}

Parser::~Parser ()
{
  for (auto i: _switchMap)
    delete i.second;
}

NclDocument *
Parser::parse0 (const string &path)
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

void
Parser::parseNcl (DOMElement *elt)
{
  string id;

  CHECK_ELT_TAG (elt, "ncl", nullptr);
  CHECK_ELT_OPT_ATTRIBUTE (elt, "id", &id, "ncl");

  _doc = new NclDocument (id, _path);
  for (DOMElement *child: dom_elt_get_children (elt))
    {
      string tag = dom_elt_get_tag (child);
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
Parser::parseHead (DOMElement *elt)
{
  CHECK_ELT_TAG (elt, "head", nullptr);

  for (DOMElement *child: dom_elt_get_children (elt))
    {
      string tag = dom_elt_get_tag (child);
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
Parser::parse1 (const string &path)
{
  Parser parser (_width, _height);
  return parser.parse0 ((!xpathisuri (path) && !xpathisabs (path))
                        ? xpathbuildabs (_dirname, path) : path);
}

NclDocument *
Parser::parseImportNCL (DOMElement *elt, string *alias, string *uri)
{
  g_assert_nonnull (alias);
  g_assert_nonnull (uri);
  CHECK_ELT_TAG (elt, "importNCL", nullptr);
  CHECK_ELT_ATTRIBUTE (elt, "alias", alias);
  CHECK_ELT_ATTRIBUTE (elt, "documentURI", uri);
  return this->parse1 (*uri);
}

Base *
Parser::parseImportBase (DOMElement *elt, NclDocument **doc,
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

  *doc = this->parse1 (*uri);
  g_assert_nonnull (*doc);

  parent = (DOMElement*) elt->getParentNode ();
  g_assert_nonnull (parent);

  tag = dom_elt_get_tag (parent);
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
Parser::parseImportedDocumentBase (DOMElement *elt)
{
  for (DOMElement *child: dom_elt_get_children (elt))
    {
      string tag = dom_elt_get_tag (elt);
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
Parser::parseRuleBase (DOMElement *elt)
{
  RuleBase *base;
  string id;

  CHECK_ELT_TAG (elt, "ruleBase", nullptr);
  CHECK_ELT_OPT_ID (elt, &id, "");

  base = new RuleBase (id);
  for (DOMElement *child: dom_elt_get_children (elt))
    {
      string tag = dom_elt_get_tag (child);
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
Parser::parseCompositeRule (DOMElement *elt)
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
  for (DOMElement *child: dom_elt_get_children (elt))
    {
      string tag = dom_elt_get_tag (child);
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
Parser::parseRule (DOMElement *elt)
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
Parser::parseTransitionBase (DOMElement *elt)
{
  TransitionBase *base;
  string id;

  CHECK_ELT_TAG (elt, "transitionBase", nullptr);
  CHECK_ELT_OPT_ID (elt, &id, "");

  base = new TransitionBase (id);
  for(DOMElement *child: dom_elt_get_children (elt))
    {
      string tag = dom_elt_get_tag (child);
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
Parser::parseTransition (DOMElement *elt)
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

  if (dom_elt_try_get_attribute (value, elt, "subtype"))
    {
      int subtype = TransitionUtil::getSubtypeCode (type, value);
      trans->setSubtype (CLAMP (subtype, 0, G_MAXINT));
    }

  if (dom_elt_try_get_attribute (value, elt, "dur"))
    trans->setDuration (ginga_parse_time (value));

  if (dom_elt_try_get_attribute (value, elt, "startProgress"))
    trans->setStartProgress (xstrtod (value));

  if (dom_elt_try_get_attribute (value, elt, "endProgress"))
    trans->setEndProgress (xstrtod (value));

  if (dom_elt_try_get_attribute (value, elt, "direction"))
    {
      int dir = TransitionUtil::getDirectionCode (value);
      trans->setDirection ((short) CLAMP (dir, 0, G_MAXINT));
    }

  if (dom_elt_try_get_attribute (value, elt, "fadeColor"))
    trans->setFadeColor (ginga_parse_color (value));

  if (dom_elt_try_get_attribute (value, elt, "horzRepeat"))
    trans->setHorzRepeat (xstrtoint (value, 10));

  if (dom_elt_try_get_attribute (value, elt, "vertRepeat"))
    trans->setVertRepeat (xstrtoint (value, 10));

  if (dom_elt_try_get_attribute (value, elt, "borderWidth"))
    trans->setBorderWidth (xstrtoint (value, 10));

  if (dom_elt_try_get_attribute (value, elt, "borderColor"))
    trans->setBorderColor (ginga_parse_color (value));

  return trans;
}


// Private: Region.

RegionBase *
Parser::parseRegionBase (DOMElement *elt)
{
  RegionBase *base;
  string id;

  CHECK_ELT_TAG (elt, "regionBase", nullptr);
  CHECK_ELT_OPT_ID (elt, &id, "");

  base = new RegionBase (id);
  for (DOMElement *child: dom_elt_get_children (elt))
    {
      string tag = dom_elt_get_tag (child);
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
Parser::parseRegion (DOMElement *elt, RegionBase *base,
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
      parent_rect.w = _width;
      parent_rect.h = _height;
    }

  rect = parent_rect;
  z = zorder = 0;

  if (dom_elt_try_get_attribute (value, elt, "left"))
    rect.x += ginga_parse_percent (value, parent_rect.w, 0, G_MAXINT);

  if (dom_elt_try_get_attribute (value, elt, "top"))
    rect.y += ginga_parse_percent (value, parent_rect.h, 0, G_MAXINT);

  if (dom_elt_try_get_attribute (value, elt, "width"))
    rect.w = ginga_parse_percent (value, parent_rect.w, 0, G_MAXINT);

  if (dom_elt_try_get_attribute (value, elt, "height"))
    rect.h = ginga_parse_percent (value, parent_rect.h, 0, G_MAXINT);

  if (dom_elt_try_get_attribute (value, elt, "right"))
    {
    rect.x += parent_rect.w - rect.w
      - ginga_parse_percent (value, parent_rect.w, 0, G_MAXINT);
    }

  if (dom_elt_try_get_attribute (value, elt, "bottom"))
    {
      rect.y += parent_rect.h - rect.h
        - ginga_parse_percent (value, parent_rect.h, 0, G_MAXINT);
    }

  if (dom_elt_try_get_attribute (value, elt, "zIndex"))
    z = xstrtoint (value, 10);
  zorder = last_zorder++;

  region->setRect (rect);
  region->setZ (z, zorder);

  // Collect children.
  for (DOMElement *child: dom_elt_get_children (elt))
    {
      string tag = dom_elt_get_tag (child);
      if (tag == "region")
        base->addRegion (this->parseRegion (child, base, region));
      else
        ERROR_SYNTAX_ELT_UNKNOWN_CHILD (elt, child);
    }
  return region;
}


// Private: Descriptor.

DescriptorBase *
Parser::parseDescriptorBase (DOMElement *elt)
{
  DescriptorBase *base;
  string id;

  CHECK_ELT_TAG (elt, "descriptorBase", nullptr);
  CHECK_ELT_OPT_ID (elt, &id, "");

  base = new DescriptorBase (id);
  for (DOMElement *child: dom_elt_get_children (elt))
    {
      string tag = dom_elt_get_tag (child);
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
          ERROR_NOT_IMPLEMENTED ("%s: element is not supported",
                                 __error_elt (child).c_str ());
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
Parser::parseDescriptor (DOMElement *elt)
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
  if (dom_elt_try_get_attribute (value, elt, "region"))
    {
      LayoutRegion *region = _doc->getRegion (value);
      if (unlikely (region == nullptr))
        ERROR_SYNTAX_ELT_BAD_ATTRIBUTE (elt, "region");
      desc->setRegion (region);
    }

  for (auto attr: supported)
    {
      if (dom_elt_try_get_attribute (value, elt, attr))
        desc->addParameter (new Parameter (attr, value));
    }

  for (auto attr: transattr)
    {
      TransitionBase *base;

      if (!dom_elt_try_get_attribute (value, elt, attr))
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
  for (DOMElement *child: dom_elt_get_children (elt))
    {
      string tag = dom_elt_get_tag (child);
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
Parser::parseConnectorBase (DOMElement *elt)
{
  ConnectorBase *base;
  string id;

  CHECK_ELT_TAG (elt, "connectorBase", nullptr);
  CHECK_ELT_OPT_ID (elt, &id, "");

  base = new ConnectorBase (id);
  for (DOMElement *child: dom_elt_get_children (elt))
    {
      string tag = dom_elt_get_tag (child);
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
Parser::parseCausalConnector (DOMElement *elt)
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
  for (DOMElement *child: dom_elt_get_children (elt))
    {
      string tag = dom_elt_get_tag (child);
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
Parser::parseCompoundCondition (DOMElement *elt)
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

  if (dom_elt_try_get_attribute (value, elt, "delay"))
    cond->setDelay (value);

  // Collect children.
  for (DOMElement *child: dom_elt_get_children (elt))
    {
      string tag = dom_elt_get_tag (child);
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
Parser::parseSimpleCondition (DOMElement *elt)
{
  SimpleCondition *cond;
  string role;
  string value;

  EventType type;
  EventStateTransition trans;
  map<string, pair<int,int>>::iterator it;

  CHECK_ELT_TAG (elt, "simpleCondition", nullptr);
  CHECK_ELT_ATTRIBUTE (elt, "role", &role);

  type = (EventType) -1;
  trans = EventStateTransition::UNKNOWN;

  if ((it = reserved_condition_table.find (role))
      != reserved_condition_table.end ())
    {
      type = (EventType) it->second.first;
      trans = (EventStateTransition) it->second.second;
    }

  if (dom_elt_try_get_attribute (value, elt, "eventType"))
    {
      if (unlikely (type != (EventType) -1))
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

  if (dom_elt_try_get_attribute (value, elt, "transition"))
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

  g_assert (type != (EventType) -1);
  g_assert (trans != EventStateTransition::UNKNOWN);

  cond = new SimpleCondition (role);
  cond->setEventType (type);
  cond->setTransition (trans);

  if (type == EventType::SELECTION
      && dom_elt_try_get_attribute (value, elt, "key"))
    {
      cond->setKey (value);
    }

  if (dom_elt_try_get_attribute (value, elt, "delay"))
    cond->setDelay (value);

  if (dom_elt_try_get_attribute (value, elt, "qualifier"))
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
Parser::parseCompoundStatement (DOMElement *elt)
{
  CompoundStatement *stmt;
  string op;
  string neg;
  bool negated;

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

  if (_ginga_parse_bool (neg, &negated))
    stmt->setNegated (negated);
  else
    ERROR_SYNTAX_ELT_BAD_ATTRIBUTE (elt, "isNegated");

  // Collect children.
  for (DOMElement *child: dom_elt_get_children (elt))
    {
      string tag = dom_elt_get_tag (child);
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
Parser::parseAssessmentStatement (DOMElement *elt)
{
  AssessmentStatement *stmt;
  string comp;
  string value;

  CHECK_ELT_TAG (elt, "assessmentStatement", nullptr);
  CHECK_ELT_OPT_ATTRIBUTE (elt, "comparator", &comp, "eq");

  stmt = new AssessmentStatement (Comparator::fromString (comp));
  for (DOMElement *child: dom_elt_get_children (elt))
    {
      string tag = dom_elt_get_tag (child);
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
Parser::parseAttributeAssessment (DOMElement *elt)
{
  map<string, EventType>::iterator it;
  AttributeAssessment *assess;
  string role;
  string value;

  CHECK_ELT_TAG (elt, "attributeAssessment", nullptr);
  CHECK_ELT_ATTRIBUTE (elt, "role", &role);

  assess = new AttributeAssessment (role);
  if (dom_elt_try_get_attribute (value, elt, "eventType"))
    {
      if ((it = event_type_table.find (value)) != event_type_table.end ())
        {
           assess->setEventType (it->second);
        }
      else
        {
          ERROR_SYNTAX_ELT_BAD_ATTRIBUTE (elt, "eventType");
        }
    }

  if (dom_elt_try_get_attribute (value, elt, "attributeType"))
    assess->setAttributeType (EventUtil::getAttributeTypeCode (value));

  // parameter
  if (assess->getEventType () == EventType::SELECTION
      && dom_elt_try_get_attribute (value, elt, "key"))
    {
      assess->setKey (value);
    }

  if (dom_elt_try_get_attribute (value, elt, "offset"))
    assess->setOffset (value);

  return assess;
}

ValueAssessment *
Parser::parseValueAssessment (DOMElement *elt)
{
  string value;
  CHECK_ELT_TAG (elt, "valueAssessment", nullptr);
  CHECK_ELT_ATTRIBUTE (elt, "value", &value);
  return new ValueAssessment (value);
}

CompoundAction *
Parser::parseCompoundAction (DOMElement *elt)
{
  CompoundAction *action;
  string value;

  CHECK_ELT_TAG (elt, "compoundAction", nullptr);

  action = new CompoundAction ();

  if (dom_elt_try_get_attribute (value, elt, "delay"))
    action->setDelay (value);

  // Collect children.
  for (DOMElement *child: dom_elt_get_children (elt))
    {
      string tag = dom_elt_get_tag (child);
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
Parser::parseSimpleAction (DOMElement *elt)
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

  type = (EventType) -1;
  acttype = -1;

  if ((it = reserved_action_table.find (role))
      != reserved_action_table.end ())
    {
      type = (EventType) it->second.first;
      acttype = (SimpleActionType) it->second.second;
    }

  if (dom_elt_try_get_attribute (value, elt, "eventType"))
    {
      if (unlikely (type != (EventType) -1))
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

  if (dom_elt_try_get_attribute (value, elt, "actionType"))
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

  g_assert (type != (EventType) -1);
  g_assert (acttype != -1);

  action = new SimpleAction (role);
  action->setEventType (type);
  action->setActionType ((SimpleActionType) acttype);

  if (type == EventType::ATTRIBUTION
      && acttype == ACT_START
      && dom_elt_try_get_attribute (value, elt, "duration"))
    {
      Animation *anim = new Animation ();
      anim->setDuration (value);
      anim->setBy ("indefinite"); // TODO: Handle "by".
      action->setAnimation (anim);
    }

  if (dom_elt_try_get_attribute (value, elt, "delay"))
    action->setDelay (value);

  if (dom_elt_try_get_attribute (value, elt, "value"))
    action->setValue (value);

  // TODO: Handle repeatDelay and repeat attributes.

  return action;
}


// Private: Body.

ContextNode *
Parser::parseBody (DOMElement *elt)
{
  ContextNode *body;
  string id;

  CHECK_ELT_TAG (elt, "body", nullptr);
  CHECK_ELT_OPT_ID (elt, &id, _doc->getId ());

  body = new ContextNode (id);
  _doc->setBody (body);

  for (DOMElement *child: dom_elt_get_children (elt))
    {
      Node *node;
      string tag;

      tag = dom_elt_get_tag (child);
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

void
Parser::posCompileContext (DOMElement *elt, ContextNode *context)
{
  Node *node;
  string id;

  g_assert_nonnull (context);
  for(DOMElement *child: dom_elt_get_children (elt))
    {
      string tag = dom_elt_get_tag(child);
      if (tag == "context")
        {
          g_assert (dom_elt_try_get_attribute (id, child, "id"));
          node = context->getNode (id);
          g_assert_nonnull (node);
          if (instanceof (ContextNode *, node))
            this->posCompileContext (child, (ContextNode*) node);
        }
      else if (tag == "switch")
        {
          g_assert (dom_elt_try_get_attribute (id, child, "id"));
          node = context->getNode (id);
          g_assert_nonnull (node);
          if (instanceof (SwitchNode *, node))
            this->posCompileSwitch (child, (SwitchNode*) node);
        }
    }

  for (DOMElement *child: dom_elt_get_children (elt))
    {
      string tag = dom_elt_get_tag (child);
      if (tag == "link")
        context->addLink (this->parseLink (child, context));
      else if (tag == "port")
        context->addPort (this->parsePort (child, context));
    }
}

void
Parser::posCompileSwitch (DOMElement *elt, SwitchNode *swtch)
{
  Node *node;
  string id;

  g_assert_nonnull (swtch);
  for (DOMElement *child: dom_elt_get_children (elt))
    {
      string tag = dom_elt_get_tag(child);
      if (tag == "context")
        {
          g_assert (dom_elt_try_get_attribute (id, child, "id"));
          node = swtch->getNode (id);
          g_assert_nonnull (node);
          if (instanceof (ContextNode *, node))
            this->posCompileContext (child, (ContextNode*) node);
        }
      else if (tag ==  "switch")
        {
          g_assert (dom_elt_try_get_attribute (id, child, "id"));
          node = swtch->getNode (id);
          g_assert_nonnull (node);
          if (instanceof (SwitchNode *, node))
            this->posCompileSwitch (child, (SwitchNode*) node);
        }
    }

  for (DOMElement *child: dom_elt_get_children (elt))
    {
      string tag = dom_elt_get_tag (child);
      if (tag == "switchPort")
        swtch->addPort (this->parseSwitchPort (child, swtch));
    }
}

void
Parser::solveNodeReferences (CompositeNode *comp)
{
  const vector<Node *> *nodes;
  bool del = false;

  if (instanceof (SwitchNode *, comp))
    {
      map<string, map<string, Node *> *>::iterator it;
      map<string, Node *> *tab;
      vector<Node *> *aux_nodes = new vector<Node *>;
      del = true;

      if ((it = _switchMap.find (comp->getId ())) != _switchMap.end ())
        {
          tab = it->second;
          for (auto k: *tab)
            aux_nodes->push_back (k.second);
        }
      nodes = aux_nodes;
    }
  else
    {
      nodes = comp->getNodes ();
    }

  g_assert_nonnull (nodes);

  for (Node *node : *nodes)
  {
    g_assert_nonnull (node);
    if (instanceof (ReferNode *, node))
      {
        Entity *ref;
        NodeEntity *refNode;

        ref = ((ReferNode *) node)->getReferredEntity ();
        g_assert_nonnull (ref);

        refNode = (NodeEntity *)(_doc->getNode (ref->getId ()));
        g_assert_nonnull (refNode);

        ((ReferNode *) node)->setReferredEntity (refNode);
      }
    else if (instanceof (CompositeNode *, node))
      {
        this->solveNodeReferences ((CompositeNode *) node);
      }
  }

  if (del)
    delete nodes;
}


// Private: Context.

Node *
Parser::parseContext (DOMElement *elt)
{
  ContextNode *context;
  string id;

  CHECK_ELT_TAG (elt, "context", nullptr);
  CHECK_ELT_ID (elt, &id);
  CHECK_ELT_ATTRIBUTE_NOT_SUPPORTED (elt, "refer");

  context = new ContextNode (id);
  for (DOMElement *child: dom_elt_get_children (elt))
    {
      Node *node;
      string tag;

      tag = dom_elt_get_tag (child);
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
Parser::parsePort (DOMElement *elt, CompositeNode *context)
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

  targetEntity = cast (NodeEntity *, target);
  g_assert_nonnull (targetEntity);

  if (dom_elt_try_get_attribute (value, elt, "interface"))
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


// Private: Switch.

Node *
Parser::parseSwitch (DOMElement *elt)
{
  Node *swtch;
  string id;

  CHECK_ELT_TAG (elt, "switch", nullptr);
  CHECK_ELT_ID (elt, &id);
  CHECK_ELT_ATTRIBUTE_NOT_SUPPORTED (elt, "refer");

  swtch = new SwitchNode (id);
  _switchMap[id] = new map<string, Node *>;

  // Collect children.
  for (DOMElement *child: dom_elt_get_children (elt))
    {
      Node *node;
      string tag;

      tag = dom_elt_get_tag (child);
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
      map<string, Node *> *map = _switchMap[id];
      if (map->count (node->getId ()) == 0)
        (*map)[node->getId ()] = node;
    }

  // Collect skipped.
  for (DOMElement *child: dom_elt_get_children (elt)) // redo
    {
      string tag = dom_elt_get_tag (child);
      if (tag == "bindRule")
        {
          Node *node;
          Rule *rule;
          node = this->parseBindRule (child, (CompositeNode *) swtch, &rule);
          g_assert_nonnull (node);
          ((SwitchNode *) swtch)->addNode (node, rule);
        }
      else if (tag == "defaultComponent")
        {
          Node *node;
          string comp;
          map<string, Node *> *nodes;

          CHECK_ELT_TAG (child, "defaultComponent", nullptr);
          CHECK_ELT_ATTRIBUTE (child, "component", &comp);

          nodes = _switchMap[swtch->getId ()];
          g_assert_nonnull (nodes);

          if (unlikely (nodes->count (comp) == 0))
            ERROR_SYNTAX_ELT_BAD_ATTRIBUTE (child, "component");

          node = (*nodes)[comp];
          if (unlikely (node == nullptr))
            ERROR_SYNTAX_ELT_BAD_ATTRIBUTE (child, "component");

          ((SwitchNode *) swtch)->setDefaultNode (node);
        }
    }

  // Add nodes not mapped to switch.
  for (auto nodes: *_switchMap[swtch->getId ()])
    nodes.second->setParentComposition ((CompositeNode *) swtch);

  return swtch;
}

Node *
Parser::parseBindRule (DOMElement *elt, CompositeNode *parent,
                          Rule **rule)
{
  Node *node;
  string constituent;
  string ruleid;

  map<string, Node *> *nodes;

  CHECK_ELT_TAG (elt, "bindRule", nullptr);
  CHECK_ELT_ATTRIBUTE (elt, "constituent", &constituent);
  CHECK_ELT_ATTRIBUTE (elt, "rule", &ruleid);

  nodes = _switchMap[parent->getId ()];
  g_assert_nonnull (nodes);

  if (unlikely (nodes->count (constituent) == 0))
    ERROR_SYNTAX_ELT_BAD_ATTRIBUTE (elt, "constituent");

  node = (NodeEntity *)(*nodes)[constituent];
  if (unlikely (node == nullptr))
    ERROR_SYNTAX_ELT_BAD_ATTRIBUTE (elt, "constituent");

  g_assert_nonnull (rule);
  *rule = _doc->getRule (ruleid);
  if (unlikely (*rule == nullptr))
    ERROR_SYNTAX_ELT_BAD_ATTRIBUTE (elt, "rule");
  return node;
}

SwitchPort *
Parser::parseSwitchPort (DOMElement *elt, SwitchNode *swtch)
{
  SwitchPort *port;
  string id;

  g_assert_nonnull (swtch);

  CHECK_ELT_TAG (elt, "switchPort", nullptr);
  CHECK_ELT_ID (elt, &id);

  port = new SwitchPort (id, swtch);
  for (DOMElement *child: dom_elt_get_children (elt))
    {
      string tag = dom_elt_get_tag (child);
      if (tag == "mapping")
        {
          swtch->addPort (this->parseMapping (child, swtch, port));
        }
      else
        {
          ERROR_SYNTAX_ELT_UNKNOWN_CHILD (elt, child);
        }
    }
  return port;
}

Port *
Parser::parseMapping (DOMElement *elt, SwitchNode *swtch,
                         SwitchPort *port)
{
  Node *mapping;
  string id;
  string comp;
  string value;

  NodeEntity *mappingEntity;
  InterfacePoint *iface;

  CHECK_ELT_TAG (elt, "mapping", nullptr);
  CHECK_ELT_ATTRIBUTE (elt, "component", &comp);

  mapping = swtch->getNode (comp);
  if (unlikely (mapping == nullptr))
    ERROR_SYNTAX_ELT_BAD_ATTRIBUTE (elt, "component");

  mappingEntity = cast (NodeEntity *, mapping);
  g_assert_nonnull (mappingEntity);

  if (dom_elt_try_get_attribute (value, elt, "interface"))
    {
      iface = mappingEntity->getAnchor (value);
      if (iface == nullptr && instanceof (CompositeNode *, mappingEntity))
        iface = ((CompositeNode *) mappingEntity)->getPort (value);
      if (unlikely (iface == nullptr))
        ERROR_SYNTAX_ELT_BAD_ATTRIBUTE (elt, "interface");
    }
  else
    {
      iface = mappingEntity->getAnchor (0);
      g_assert_nonnull (iface);
    }
  return new Port (port->getId (), mapping, iface);
}


// Private: Media.

Node *
Parser::parseMedia (DOMElement *elt)
{
  Node *media;
  string id;
  string src;
  string value;

  CHECK_ELT_TAG (elt, "media", nullptr);
  CHECK_ELT_ID (elt, &id);

  // Refer?
  if (dom_elt_try_get_attribute (value, elt, "refer"))
    {
      Entity *refer;

      refer = (ContentNode *) _doc->getNode (value);
      if (unlikely (refer == nullptr))
        refer = new ReferredNode (value, (void *) elt); // FIXME: Crazy.

      media = new ReferNode (id);
      if (dom_elt_try_get_attribute (value, elt, "instance"))
        ((ReferNode *) media)->setInstanceType (value);
      ((ReferNode *) media)->setReferredEntity (refer);
    }
  else
    {
      media = new ContentNode (id, NULL, "");

      if (dom_elt_try_get_attribute (value, elt, "type"))
        ((ContentNode *) media)->setNodeType (value);

      CHECK_ELT_OPT_ATTRIBUTE (elt, "src", &src, "");
      if (!xpathisuri (src) && !xpathisabs (src))
        src = xpathbuildabs (_dirname, src);
      ((ContentNode *) media)
        ->setContent (new AbsoluteReferenceContent (src));

      if (dom_elt_try_get_attribute (value, elt, "descriptor"))
        {
          GenericDescriptor *desc = _doc->getDescriptor (value);
          if (unlikely (desc == nullptr))
            ERROR_SYNTAX_ELT_BAD_ATTRIBUTE (elt, "descriptor");
          ((ContentNode *) media)->setDescriptor (desc);
        }
    }

  // Collect children.
  for (DOMElement *child: dom_elt_get_children (elt))
    {
      string tag = dom_elt_get_tag (child);
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
Parser::parseProperty (DOMElement *elt)
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
Parser::parseArea (DOMElement *elt)
{
  string id;
  string value;

  CHECK_ELT_TAG (elt, "area", nullptr);
  CHECK_ELT_ID (elt, &id);
  CHECK_ELT_ATTRIBUTE_NOT_SUPPORTED (elt, "coords");
  CHECK_ELT_ATTRIBUTE_NOT_SUPPORTED (elt, "first");
  CHECK_ELT_ATTRIBUTE_NOT_SUPPORTED (elt, "last");

  if (dom_elt_has_attribute (elt, "begin")
      || dom_elt_has_attribute (elt, "end"))
    {
      GingaTime begin;
      GingaTime end;

      if (dom_elt_try_get_attribute (value, elt ,"begin"))
        begin = ginga_parse_time (value);
      else
        begin = 0;

      if (dom_elt_try_get_attribute (value, elt, "end"))
        end = ginga_parse_time (value);
      else
        end = GINGA_TIME_NONE;

      return new IntervalAnchor (id, begin, end);
    }
  else if (dom_elt_has_attribute (elt, "text"))
    {
      string text;
      string pos;
      CHECK_ELT_ATTRIBUTE (elt, "text", &text);
      CHECK_ELT_OPT_ATTRIBUTE (elt, "position", &pos, "0");
      return new TextAnchor (id, text, xstrtoint (pos, 10));
    }
  else if (dom_elt_has_attribute (elt, "label"))
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
Parser::parseLink (DOMElement *elt, CompositeNode *context)
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
  for (DOMElement *child: dom_elt_get_children (elt))
    {
      string tag = dom_elt_get_tag (child);
      if (tag == "linkParam")
        {
          link->addParameter (this->parseLinkParam (child));
        }
      else if (tag == "bind")
        {
          g_assert_nonnull (this->parseBind (child, link, context));
        }
      else
        {
          ERROR_SYNTAX_ELT_UNKNOWN_CHILD (elt, child);
        }
    }
  return link;
}

Parameter *
Parser::parseLinkParam (DOMElement *elt)
{
  string name;
  string value;
  CHECK_ELT_TAG (elt, "linkParam", nullptr);
  CHECK_ELT_ATTRIBUTE (elt, "name", &name);
  CHECK_ELT_ATTRIBUTE (elt, "value", &value);
  return new Parameter (name, value);
}

Bind *
Parser::parseBind (DOMElement *elt, Link *link, CompositeNode *context)
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

  targetEntity = cast (NodeEntity *, target);
  if (targetEntity == nullptr)
    {
      g_assert (instanceof (ReferNode *, target));
      targetEntity = cast (NodeEntity *, cast (ReferNode *, target)
                           ->getReferredEntity ());
    }
  g_assert_nonnull (targetEntity);

  iface = nullptr;
  desc = nullptr;

  if (dom_elt_try_get_attribute (value, elt, "interface"))
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

  if (dom_elt_try_get_attribute (value, elt, "descriptor"))
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
  for (DOMElement *child: dom_elt_get_children (elt))
    {
      string tag = dom_elt_get_tag (child);
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
Parser::parseBindParam (DOMElement *elt)
{
  string name;
  string value;
  CHECK_ELT_TAG (elt, "bindParam", nullptr);
  CHECK_ELT_ATTRIBUTE (elt, "name", &name);
  CHECK_ELT_ATTRIBUTE (elt, "value", &value);
  return new Parameter (name, value);
}


// Private: Error handlers.

void
Parser::warning (const SAXParseException &e)
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
Parser::error (const SAXParseException &e)
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
Parser::fatalError (const SAXParseException &e)
{
  this->error (e);
}

GINGA_NCL_END
