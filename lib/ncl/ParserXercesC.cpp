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

#include "aux-ginga.h"
#include "ParserXercesC.h"
#include "Ncl.h"

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


// Syntax checks.

// Element data.
typedef struct EltData
{
  map<string,GType> required;   // required attributes
  map<string,GType> optional;   // optional attributes
} EltData;

// Element table.
static map<string, EltData> elt_table =
{
 {"ncl", {{},{
  {"id",    G_TYPE_STRING},
  {"title", G_TYPE_STRING},
  {"xmlns", G_TYPE_STRING},
 }}},
};

// Indexes element table.
static G_GNUC_UNUSED bool
elt_table_index (const string &tag, EltData **result)
{
  map<string, EltData>::iterator it;
  if ((it = elt_table.find (tag)) == elt_table.end ())
    return false;
  tryset (result, &it->second);
  return true;
}

static G_GNUC_UNUSED bool
check_elt (const DOMElement *elt, string *errmsg)
{
  string tag;
  EltData *data;

  tag = dom_elt_get_tag (elt);
  if (!elt_table_index (tag, &data))
    {
      tryset (errmsg, xstrbuild ("unknown element <%s>", tag.c_str ()));
      return false;
    }
  return true;
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
    tryset ((string *)(pvalue), result);                                \
  }                                                                     \
  G_STMT_END

#define CHECK_ELT_ATTRIBUTE(elt, name, pvalue)                          \
  G_STMT_START                                                          \
  {                                                                     \
    string result;                                                      \
    if (unlikely (!dom_elt_try_get_attribute (result, (elt), (name))))  \
      ERROR_SYNTAX_ELT_MISSING_ATTRIBUTE ((elt), (name));               \
    tryset ((pvalue), result);                                          \
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
    tryset ((pvalue), result);                                  \
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
    tryset ((pvalue), result);                                          \
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
    tryset ((pvalue), result);                                  \
  }                                                             \
  G_STMT_END

#define CHECK_ELT_OPT_ID_AUTO(elt, pvalue, Elt)                 \
  G_STMT_START                                                  \
  {                                                             \
    static int __opt_auto_id_##Elt = 1;                         \
    string autoid = xstrbuild ("unnamed-%s-%d",                 \
                               G_STRINGIFY (Elt),               \
                               (__opt_auto_id_##Elt)++);        \
    CHECK_ELT_OPT_ID ((elt), (pvalue), autoid);                 \
  }                                                             \
  G_STMT_END


// Translation tables.

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
   {"starts", EventStateTransition::START},
   {"stops", EventStateTransition::STOP},
   {"aborts", EventStateTransition::ABORT},
   {"pauses", EventStateTransition::PAUSE},
   {"resumes", EventStateTransition::RESUME},
  };

// Maps action name to action code.
static map<string, EventStateTransition> event_action_type_table =
  {
   {"start", EventStateTransition::START},
   {"stop", EventStateTransition::STOP},
   {"abort", EventStateTransition::ABORT},
   {"pause", EventStateTransition::PAUSE},
   {"resume", EventStateTransition::RESUME},
  };


// Public.

/**
 * @brief Parses NCL document.
 * @param path Document path.
 * @param width Initial screen width (in pixels).
 * @param height Initial screen width (in pixels).
 * @param errmsg Address of the variable to store error message.
 * @return The resulting document.
 */
NclDocument *
ParserXercesC::parse (const string &path, int width, int height,
                      string *errmsg)
{
  ParserXercesC parser (width, height);
  NclDocument *result;
  result = parser.parse0 (path);
  if (result == nullptr)
    tryset (errmsg, parser.getErrMsg ());
  return result;
}


// Private.

string
ParserXercesC::getErrMsg ()
{
  return _errmsg;
}

void
ParserXercesC::setErrMsg (const string &msg)
{
  _errmsg = msg;
}

ParserXercesC::ParserXercesC (int width, int height)
{
  _doc = nullptr;
  g_assert_cmpint (width, >, 0);
  _width = width;
  g_assert_cmpint (height, >, 0);
  _height = height;
}

ParserXercesC::~ParserXercesC ()
{
  for (auto i: _switchMap)
    delete i.second;
}

NclDocument *
ParserXercesC::parse0 (const string &path)
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
  if (!unlikely (parseNcl (elt)))
    {
      if (_doc != nullptr)
        delete _doc;
      _doc = nullptr;
    }

  delete parser;

  return _doc;
}

bool
ParserXercesC::parseNcl (DOMElement *elt)
{
  string id;

  if (unlikely (!check_elt (elt, &_errmsg)))
    return false;

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
          Context *body = this->parseBody (child);
          g_assert_nonnull (body);
          this->solveNodeReferences (body);
          this->posCompileContext (child, body);
        }
      else
        {
          ERROR_SYNTAX_ELT_UNKNOWN_CHILD (elt, child);
        }
    }
  return true;
}


// Private: Head.

void
ParserXercesC::parseHead (DOMElement *elt)
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
ParserXercesC::parse1 (const string &path)
{
  ParserXercesC parser (_width, _height);
  return parser.parse0 ((!xpathisuri (path) && !xpathisabs (path))
                        ? xpathbuildabs (_dirname, path) : path);
}

NclDocument *
ParserXercesC::parseImportNCL (DOMElement *elt, string *alias, string *uri)
{
  g_assert_nonnull (alias);
  g_assert_nonnull (uri);
  CHECK_ELT_TAG (elt, "importNCL", nullptr);
  CHECK_ELT_ATTRIBUTE (elt, "alias", alias);
  CHECK_ELT_ATTRIBUTE (elt, "documentURI", uri);
  return this->parse1 (*uri);
}

Base *
ParserXercesC::parseImportBase (DOMElement *elt, NclDocument **doc,
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
ParserXercesC::parseImportedDocumentBase (DOMElement *elt)
{
  for (DOMElement *child: dom_elt_get_children (elt))
    {
      string tag = dom_elt_get_tag (child);
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
ParserXercesC::parseRuleBase (DOMElement *elt)
{
  RuleBase *base;
  string id;

  CHECK_ELT_TAG (elt, "ruleBase", nullptr);
  CHECK_ELT_OPT_ID_AUTO (elt, &id, ruleBase);

  base = new RuleBase (_doc, id);
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
ParserXercesC::parseCompositeRule (DOMElement *elt)
{
  CompositeRule *rule;
  string id;
  string value;

  CHECK_ELT_TAG (elt, "compositeRule", nullptr);
  CHECK_ELT_ID (elt, &id);

  CHECK_ELT_ATTRIBUTE (elt, "operator", &value);
  if (value == "and")
    rule = new CompositeRule (_doc, id, true);
  else if (value == "or")
    rule = new CompositeRule (_doc, id, false);
  else
    ERROR_SYNTAX_ELT_BAD_ATTRIBUTE (elt, "operator");

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
ParserXercesC::parseRule (DOMElement *elt)
{
  string id;
  string var;
  string comp;
  string value;
  CHECK_ELT_TAG (elt, "rule", nullptr);
  CHECK_ELT_OPT_ID_AUTO (elt, &id, rule);
  CHECK_ELT_ATTRIBUTE (elt, "var", &var);
  CHECK_ELT_ATTRIBUTE (elt, "value", &value);
  CHECK_ELT_ATTRIBUTE (elt, "comparator", &comp);
  if (unlikely (!_ginga_parse_comparator (comp, &comp)))
    ERROR_SYNTAX_ELT_BAD_ATTRIBUTE (elt, "comparator");
  return new SimpleRule (_doc, id, var, comp, value);
}

// Private: Transition.

TransitionBase *
ParserXercesC::parseTransitionBase (DOMElement *elt)
{
  TransitionBase *base;
  string id;

  CHECK_ELT_TAG (elt, "transitionBase", nullptr);
  CHECK_ELT_OPT_ID_AUTO (elt, &id, transitionBase);

  base = new TransitionBase (_doc, id);
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
ParserXercesC::parseTransition (DOMElement *elt)
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

  trans = new Transition (_doc, id, type);

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
ParserXercesC::parseRegionBase (DOMElement *elt)
{
  RegionBase *base;
  string id;

  CHECK_ELT_TAG (elt, "regionBase", nullptr);
  CHECK_ELT_OPT_ID_AUTO (elt, &id, regionBase);

  base = new RegionBase (_doc, id);
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
          Region *region = this->parseRegion (child, base, nullptr);
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

Region *
ParserXercesC::parseRegion (DOMElement *elt, RegionBase *base, Region *parent)
{
  Region *region;
  string id;
  string value;

  GingaRect parent_rect;
  GingaRect rect;
  int z;
  int zorder;
  static int last_zorder = 0;

  CHECK_ELT_TAG (elt, "region", nullptr);
  CHECK_ELT_ID (elt, &id);

  region = new Region (_doc, id);
  if (parent != NULL)
    {
      parent_rect = parent->getRect ();
    }
  else
    {
      parent_rect.x = 0;
      parent_rect.y = 0;
      parent_rect.width = _width;
      parent_rect.height = _height;
    }

  rect = parent_rect;
  z = zorder = 0;

  if (dom_elt_try_get_attribute (value, elt, "left"))
    {
      region->setLeft (value);
      rect.x += ginga_parse_percent (value, parent_rect.width, 0, G_MAXINT);
    }

  if (dom_elt_try_get_attribute (value, elt, "top"))
    {
      region->setTop (value);
      rect.y += ginga_parse_percent (value, parent_rect.height, 0, G_MAXINT);
    }

  if (dom_elt_try_get_attribute (value, elt, "width"))
    {
      region->setWidth (value);
      rect.width = ginga_parse_percent
        (value, parent_rect.width, 0, G_MAXINT);
    }

  if (dom_elt_try_get_attribute (value, elt, "height"))
    {
      region->setHeight (value);
      rect.height = ginga_parse_percent
        (value, parent_rect.height, 0, G_MAXINT);
    }

  if (dom_elt_try_get_attribute (value, elt, "right"))
    {
      region->setRight (value);
      rect.x += parent_rect.width - rect.width
        - ginga_parse_percent (value, parent_rect.width, 0, G_MAXINT);
    }

  if (dom_elt_try_get_attribute (value, elt, "bottom"))
    {
      region->setBottom (value);
      rect.y += parent_rect.height - rect.height
        - ginga_parse_percent (value, parent_rect.height, 0, G_MAXINT);
    }

  if (dom_elt_try_get_attribute (value, elt, "zIndex"))
    z = xstrtoint (value, 10);
  zorder = last_zorder++;

  string left = xstrbuild ("%.2f%%", ((double) rect.x / _width) * 100.);
  string top = xstrbuild ("%.2f%%", ((double) rect.y / _height) * 100.);
  string width = xstrbuild ("%.2f%%", ((double) rect.width / _width) * 100.);
  string height = xstrbuild ("%.2f%%", ((double) rect.height / _height) * 100.);

  region->setLeft (left);
  region->setTop (top);
  region->setWidth (width);
  region->setHeight (height);

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
ParserXercesC::parseDescriptorBase (DOMElement *elt)
{
  DescriptorBase *base;
  string id;

  CHECK_ELT_TAG (elt, "descriptorBase", nullptr);
  CHECK_ELT_OPT_ID_AUTO (elt, &id, descriptorBase);

  base = new DescriptorBase (_doc, id);
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
              regionBase = new RegionBase (_doc, "");
              _doc->addRegionBase (regionBase);
            }
          for (auto item: *doc->getRegionBases ())
            regionBase->addBase (item.second, alias, uri);

          // Import rules.
          RuleBase *ruleBase = _doc->getRuleBase ();
          if (ruleBase == nullptr)
            {
              ruleBase = new RuleBase (_doc, "");
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
ParserXercesC::parseDescriptor (DOMElement *elt)
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

  desc = new Descriptor (_doc, id);
  if (dom_elt_try_get_attribute (value, elt, "region"))
    {
      Region *region = _doc->getRegion (value);
      if (unlikely (region == nullptr))
        ERROR_SYNTAX_ELT_BAD_ATTRIBUTE (elt, "region");
      desc->initRegion (region);
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
            desc->addInputTransition (trans);
          else
            desc->addOutputTransition (trans);
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
ParserXercesC::parseConnectorBase (DOMElement *elt)
{
  ConnectorBase *base;
  string id;

  CHECK_ELT_TAG (elt, "connectorBase", nullptr);
  CHECK_ELT_OPT_ID_AUTO (elt, &id, connectorBase);

  base = new ConnectorBase (_doc, id);
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
          Connector *conn = parseCausalConnector (child);
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

Connector *
ParserXercesC::parseCausalConnector (DOMElement *elt)
{
  Connector *conn;
  string id;
  int ncond;
  int nact;

  CHECK_ELT_TAG (elt, "causalConnector", nullptr);
  CHECK_ELT_ID (elt, &id);

  ncond = 0;
  nact = 0;

  conn = new Connector (_doc, id);
  for (DOMElement *child: dom_elt_get_children (elt))
    {
      string tag = dom_elt_get_tag (child);
      if (tag == "simpleCondition")
        {
          this->parseCondition (conn, child);
          ncond++;
        }
      else if (tag == "compoundCondition")
        {
          this->parseCompoundCondition (conn, child);
          ncond++;
        }
      else if (tag == "simpleAction")
        {
          this->parseSimpleAction (conn, child);
          nact++;
        }
      else if (tag == "compoundAction")
        {
          this->parseCompoundAction (conn, child);
          nact++;
        }
      else if (tag == "connectorParam")
        {
          string name;
          string type;
          CHECK_ELT_ATTRIBUTE (child, "name", &name);
          CHECK_ELT_OPT_ATTRIBUTE (child, "type", &type, "");
          // This is useless:
          // conn->addParameter (new Parameter (name, type));
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

void
ParserXercesC::parseCompoundCondition (Connector *conn, DOMElement *elt)
{
  string op;
  string value;

  CHECK_ELT_TAG (elt, "compoundCondition", nullptr);
  CHECK_ELT_ATTRIBUTE (elt, "operator", &op);

  if (op != "and" and op != "or")
    ERROR_SYNTAX_ELT_BAD_ATTRIBUTE (elt, "operator");

  // Collect children.
  for (DOMElement *child: dom_elt_get_children (elt))
    {
      string tag = dom_elt_get_tag (child);
      if (tag == "simpleCondition")
        {
          this->parseCondition (conn, child);
        }
      else if (tag == "assessmentStatement")
        {
          continue;
          // cond->addCondition
          //   (this->parseAssessmentStatement (child));
        }
      else if (tag == "compoundCondition")
        {
          this->parseCompoundCondition (conn, child);
        }
      else if (tag ==  "compoundStatement")
        {
          continue;
          // cond->addCondition
          //   (this->parseCompoundStatement (child));
        }
      else
        {
          ERROR_SYNTAX_ELT_UNKNOWN_CHILD (elt, child);
        }
    }
}

void
ParserXercesC::parseCondition (Connector *conn, DOMElement *elt)
{
  string str;
  string role;
  string key;
  string qualifier;

  EventType type;
  EventStateTransition trans;
  map<string, pair<int,int>>::iterator it;

  CHECK_ELT_TAG (elt, "simpleCondition", nullptr);
  CHECK_ELT_ATTRIBUTE (elt, "role", &role);
  CHECK_ELT_OPT_ATTRIBUTE (elt, "qualifier", &qualifier, "or");
  CHECK_ELT_OPT_ATTRIBUTE (elt, "key", &key, "");

  type = (EventType) -1;
  trans = (EventStateTransition) -1;

  Condition::isReserved (role, &type, &trans);

  if (dom_elt_try_get_attribute (str, elt, "eventType"))
    {
      if (unlikely (type != (EventType) -1))
        {
          ERROR_SYNTAX_ELT (elt, "eventType of '%s' cannot be overridden",
                            role.c_str ());
        }
      map<string, EventType>::iterator it;
      if ((it = event_type_table.find (str)) == event_type_table.end ())
        {
          ERROR_SYNTAX_ELT (elt, "bad eventType '%s' for role '%s'",
                            str.c_str (), role.c_str ());
        }
      type = it->second;
    }

  if (dom_elt_try_get_attribute (str, elt, "transition"))
    {
      if (unlikely (trans != (EventStateTransition) -1))
        {
          ERROR_SYNTAX_ELT (elt, "transition of '%s' cannot be overridden",
                            role.c_str ());
        }
      map<string, EventStateTransition>::iterator it;
      if ((it = event_transition_table.find (str))
          == event_transition_table.end ())
        {
          ERROR_SYNTAX_ELT (elt, "bad transition '%s' for role '%s'",
                            str.c_str (), role.c_str ());
        }
      trans = it->second;
    }

  g_assert (type != (EventType) -1);
  g_assert (trans != (EventStateTransition) -1);

  if (qualifier != "and" && qualifier != "or")
    ERROR_SYNTAX_ELT_BAD_ATTRIBUTE (elt, "qualifier");

  Condition *cond = new Condition (type, trans, nullptr, role, key);
  g_assert (conn->addCondition (cond));
}

// CompoundStatement *
// ParserXercesC::parseCompoundStatement (DOMElement *elt)
// {
//   CompoundStatement *stmt;
//   string op;
//   string neg;
//   CHECK_ELT_TAG (elt, "compoundStatement", nullptr);
//   CHECK_ELT_ATTRIBUTE (elt, "operator", &op);
//   CHECK_ELT_OPT_ATTRIBUTE (elt, "isNegated", &neg, "false");
//   if (unlikely (op != "and" && op != "or"))
//     ERROR_SYNTAX_ELT_BAD_ATTRIBUTE (elt, "operator");
//   if (unlikely (neg != "true" && neg != "false"))
//     ERROR_SYNTAX_ELT_BAD_ATTRIBUTE (elt, "isNegated");
//   stmt = new CompoundStatement (op == "and", neg == "true");
//   // Collect children.
//   for (DOMElement *child: dom_elt_get_children (elt))
//     {
//       string tag = dom_elt_get_tag (child);
//       if (tag == "assessmentStatement")
//         {
//           stmt->addStatement (this->parseAssessmentStatement (child));
//         }
//       else if (tag == "compoundStatement")
//         {
//           stmt->addStatement (this->parseCompoundStatement (child));
//         }
//       else
//         {
//           ERROR_SYNTAX_ELT_UNKNOWN_CHILD (elt, child);
//         }
//     }
//   return stmt;
// }

// AssessmentStatement *
// ParserXercesC::parseAssessmentStatement (DOMElement *elt)
// {
//   AssessmentStatement *stmt;
//   string comp;
//   string value;
//   CHECK_ELT_TAG (elt, "assessmentStatement", nullptr);
//   CHECK_ELT_ATTRIBUTE (elt, "comparator", &comp);
//   if (unlikely (!_ginga_parse_comparator (comp, &comp)))
//     ERROR_SYNTAX_ELT_BAD_ATTRIBUTE (elt, "comparator");
//   stmt = new AssessmentStatement (comp);
//   for (DOMElement *child: dom_elt_get_children (elt))
//     {
//       string tag = dom_elt_get_tag (child);
//       if (tag == "attributeAssessment")
//         {
//           AttributeAssessment *assess;
//           assess = this->parseAttributeAssessment (child);
//           if (stmt->getMainAssessment () == nullptr)
//             stmt->setMainAssessment (assess);
//           else
//             stmt->setOtherAssessment (assess);
//         }
//       else if (tag == "valueAssessment")
//         {
//           stmt->setOtherAssessment (this->parseValueAssessment (child));
//         }
//       else
//         {
//           ERROR_SYNTAX_ELT_UNKNOWN_CHILD (elt, child);
//         }
//     }
//   return stmt;
// }

// AttributeAssessment *
// ParserXercesC::parseAttributeAssessment (DOMElement *elt)
// {
//   map<string, EventType>::iterator it;
//   string role;
//   string type;
//   string key;
//   string offset;
//   EventType evttype;
//   CHECK_ELT_TAG (elt, "attributeAssessment", nullptr);
//   CHECK_ELT_ATTRIBUTE (elt, "role", &role);
//   CHECK_ELT_OPT_ATTRIBUTE (elt, "type", &type, "attribution");
//   CHECK_ELT_OPT_ATTRIBUTE (elt, "key", &key, "");
//   CHECK_ELT_OPT_ATTRIBUTE (elt, "offset", &offset, "");
//   if ((it = event_type_table.find (type)) == event_type_table.end ())
//     ERROR_SYNTAX_ELT_BAD_ATTRIBUTE (elt, "eventType");
//   evttype = it->second;
//   return new AttributeAssessment (evttype, role, key, offset);
// }

// ValueAssessment *
// ParserXercesC::parseValueAssessment (DOMElement *elt)
// {
//   string value;
//   CHECK_ELT_TAG (elt, "valueAssessment", nullptr);
//   CHECK_ELT_ATTRIBUTE (elt, "value", &value);
//   return new ValueAssessment (value);
// }

void
ParserXercesC::parseCompoundAction (Connector *conn, DOMElement *elt)
{
  string value;

  CHECK_ELT_TAG (elt, "compoundAction", nullptr);
  CHECK_ELT_ATTRIBUTE_NOT_SUPPORTED (elt, "delay");

  // Collect children.
  for (DOMElement *child: dom_elt_get_children (elt))
    {
      string tag = dom_elt_get_tag (child);
      if (tag == "simpleAction")
        {
          this->parseSimpleAction (conn, child);
        }
      else if (tag == "compoundAction")
        {
          this->parseCompoundAction (conn, child);
        }
      else
        {
          ERROR_SYNTAX_ELT_UNKNOWN_CHILD (elt, child);
        }
    }
}

void
ParserXercesC::parseSimpleAction (Connector *conn, DOMElement *elt)
{
  string str;
  string tag;
  string role;
  string delay;
  string value;
  string duration;

  EventType type;
  EventStateTransition acttype;
  map<string, pair<int,int>>::iterator it;

  CHECK_ELT_TAG (elt, "simpleAction", nullptr);
  CHECK_ELT_ATTRIBUTE (elt, "role", &role);
  CHECK_ELT_OPT_ATTRIBUTE (elt, "delay", &delay, "0s");
  CHECK_ELT_OPT_ATTRIBUTE (elt, "value", &value, "0s");
  CHECK_ELT_OPT_ATTRIBUTE (elt, "duration", &duration, "0s");

  type = (EventType) -1;
  acttype = (EventStateTransition) -1;

  Action::isReserved (role, &type, &acttype);

  if (dom_elt_try_get_attribute (str, elt, "eventType"))
    {
      if (unlikely (type != (EventType) -1))
        {
          ERROR_SYNTAX_ELT (elt, "eventType '%s' cannot be overridden",
                            role.c_str ());
        }
      map<string, EventType>::iterator it;
      if ((it = event_type_table.find (str)) == event_type_table.end ())
        {
          ERROR_SYNTAX_ELT (elt, "bad eventType '%s' for role '%s'",
                            str.c_str (), role.c_str ());
        }
      type = it->second;
    }

  if (dom_elt_try_get_attribute (str, elt, "actionType"))
    {
      if (unlikely (acttype != (EventStateTransition) -1))
        {
          ERROR_SYNTAX_ELT (elt, "actionType of '%s' cannot be overridden",
                            role.c_str ());
        }
      map<string, EventStateTransition>::iterator it;
      if ((it = event_action_type_table.find (str))
          == event_action_type_table.end ())
        {
          ERROR_SYNTAX_ELT_BAD_ATTRIBUTE (elt, "actionType");
        }
      acttype = it->second;
    }

  g_assert (type != (EventType) -1);
  g_assert (acttype != (EventStateTransition) -1);

  Action *act = new Action (type, acttype, role, delay, value, duration);
  g_assert (conn->addAction (act));
}


// Private: Body.

Context *
ParserXercesC::parseBody (DOMElement *elt)
{
  Context *body;
  string id;

  CHECK_ELT_TAG (elt, "body", nullptr);
  CHECK_ELT_OPT_ID (elt, &id, _doc->getId ());

  body = _doc->getRoot ();

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
          Property *prop = this->parseProperty (child);
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
ParserXercesC::posCompileContext (DOMElement *elt, Context *context)
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
          if (instanceof (Context *, node))
            this->posCompileContext (child, (Context*) node);
        }
      else if (tag == "switch")
        {
          g_assert (dom_elt_try_get_attribute (id, child, "id"));
          node = context->getNode (id);
          g_assert_nonnull (node);
          if (instanceof (Switch *, node))
            this->posCompileSwitch (child, (Switch*) node);
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
ParserXercesC::posCompileSwitch (DOMElement *elt, Switch *swtch)
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
          if (instanceof (Context *, node))
            this->posCompileContext (child, (Context*) node);
        }
      else if (tag ==  "switch")
        {
          g_assert (dom_elt_try_get_attribute (id, child, "id"));
          node = swtch->getNode (id);
          g_assert_nonnull (node);
          if (instanceof (Switch *, node))
            this->posCompileSwitch (child, (Switch*) node);
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
ParserXercesC::solveNodeReferences (Composition *comp)
{
  const vector<Node *> *nodes;
  bool del = false;

  if (instanceof (Switch *, comp))
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
    if (instanceof (Refer *, node))
      {
        Entity *ref;
        Media *refNode;

        ref = ((Refer *) node)->getReferred ();
        g_assert_nonnull (ref);

        refNode = cast (Media *, _doc->getNode (ref->getId ()));
        g_assert_nonnull (refNode);

        ((Refer *) node)->initReferred (refNode);
      }
    else if (instanceof (Composition *, node))
      {
        this->solveNodeReferences ((Composition *) node);
      }
  }

  if (del)
    delete nodes;
}


// Private: Context.

Node *
ParserXercesC::parseContext (DOMElement *elt)
{
  Context *context;
  string id;

  CHECK_ELT_TAG (elt, "context", nullptr);
  CHECK_ELT_ID (elt, &id);
  CHECK_ELT_ATTRIBUTE_NOT_SUPPORTED (elt, "refer");

  context = new Context (_doc, id);
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
          Property *prop = this->parseProperty (child);
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
ParserXercesC::parsePort (DOMElement *elt, Composition *context)
{
  string id;
  string comp;
  string value;

  Node *target;
  Anchor *iface;

  CHECK_ELT_TAG (elt, "port", nullptr);
  CHECK_ELT_ID (elt, &id);
  CHECK_ELT_ATTRIBUTE (elt, "component", &comp);

  target = context->getNode (comp);
  if (unlikely (target == nullptr))
    ERROR_SYNTAX_ELT_BAD_ATTRIBUTE (elt, "component");

  if (dom_elt_try_get_attribute (value, elt, "interface"))
    {
      iface = target->getAnchor (value);
      if (iface == nullptr)
        {
          if (instanceof (Composition *, target))
            {
              iface = ((Composition *) target)->getPort (value);
            }
          else
            {
              iface = target->getAnchor (value);
            }
        }
    }
  else                          // no interface
    {
      iface = target->getLambda ();
    }

  if (unlikely (iface == nullptr))
    ERROR_SYNTAX_ELT_BAD_ATTRIBUTE (elt, "interface");

  Port *port = new Port (_doc, id);
  port->setNode (target);
  port->setInterface (iface);
  return port;
}


// Private: Switch.

Node *
ParserXercesC::parseSwitch (DOMElement *elt)
{
  Node *swtch;
  string id;

  CHECK_ELT_TAG (elt, "switch", nullptr);
  CHECK_ELT_ID (elt, &id);
  CHECK_ELT_ATTRIBUTE_NOT_SUPPORTED (elt, "refer");

  swtch = new Switch (_doc, id);
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
          node = this->parseBindRule (child, (Composition *) swtch, &rule);
          g_assert_nonnull (node);
          ((Switch *) swtch)->addNode (node, rule);
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

          ((Switch *) swtch)->setDefaultNode (node);
        }
    }

  return swtch;
}

Node *
ParserXercesC::parseBindRule (DOMElement *elt, Composition *parent,
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

  node = (Node *)(*nodes)[constituent];
  if (unlikely (node == nullptr))
    ERROR_SYNTAX_ELT_BAD_ATTRIBUTE (elt, "constituent");

  g_assert_nonnull (rule);
  *rule = _doc->getRule (ruleid);
  if (unlikely (*rule == nullptr))
    ERROR_SYNTAX_ELT_BAD_ATTRIBUTE (elt, "rule");
  return node;
}

SwitchPort *
ParserXercesC::parseSwitchPort (DOMElement *elt, Switch *swtch)
{
  SwitchPort *port;
  string id;

  g_assert_nonnull (swtch);

  CHECK_ELT_TAG (elt, "switchPort", nullptr);
  CHECK_ELT_ID (elt, &id);

  port = new SwitchPort (_doc, id);
  port->setNode (swtch);
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
ParserXercesC::parseMapping (DOMElement *elt, Switch *swtch,
                      SwitchPort *port)
{
  Node *mapping;
  string id;
  string comp;
  string value;
  Anchor *iface;

  CHECK_ELT_TAG (elt, "mapping", nullptr);
  CHECK_ELT_ATTRIBUTE (elt, "component", &comp);

  mapping = swtch->getNode (comp);
  if (unlikely (mapping == nullptr))
    ERROR_SYNTAX_ELT_BAD_ATTRIBUTE (elt, "component");

  if (dom_elt_try_get_attribute (value, elt, "interface"))
    {
      iface = mapping->getAnchor (value);
      if (iface == nullptr && instanceof (Composition *, mapping))
        iface = ((Composition *) mapping)->getPort (value);
      if (unlikely (iface == nullptr))
        ERROR_SYNTAX_ELT_BAD_ATTRIBUTE (elt, "interface");
    }
  else
    {
      iface = mapping->getLambda ();
      g_assert_nonnull (iface);
    }
  Port *node = new Port (_doc, port->getId ());
  node->setNode (mapping);
  node->setInterface (iface);
  return node;
}


// Private: Media.

Node *
ParserXercesC::parseMedia (DOMElement *elt)
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
      Media *refer;

      refer = cast (Media *, _doc->getNode (value));
      g_assert_nonnull (refer);

      media = new Refer (_doc, id);
      ((Refer *) media)->initReferred (refer);
    }
  else
    {
      if (dom_elt_try_get_attribute (value, elt, "type")
          && value == "application/x-ginga-settings") // settings?
        {
          media = new Media (_doc, id, true);
        }
      else
        {
          media = new Media (_doc, id, false);
        }

      CHECK_ELT_OPT_ATTRIBUTE (elt, "src", &src, "");
      if (!xpathisuri (src) && !xpathisabs (src))
        src = xpathbuildabs (_dirname, src);
      cast (Media *, media)->setSrc (src);

      if (dom_elt_try_get_attribute (value, elt, "descriptor"))
        {
          Descriptor *desc = _doc->getDescriptor (value);
          if (unlikely (desc == nullptr))
            ERROR_SYNTAX_ELT_BAD_ATTRIBUTE (elt, "descriptor");
          ((Media *) media)->initDescriptor (desc);
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
          Property *prop = this->parseProperty (child);
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

Property *
ParserXercesC::parseProperty (DOMElement *elt)
{
  Property *prop;
  string name;
  string value;

  CHECK_ELT_TAG (elt, "property", nullptr);
  CHECK_ELT_ATTRIBUTE (elt, "name", &name);
  CHECK_ELT_OPT_ATTRIBUTE (elt, "value", &value, "");

  prop = new Property (_doc, name);
  prop->setValue (value);
  return prop;
}

Anchor *
ParserXercesC::parseArea (DOMElement *elt)
{
  string id;
  string value;

  CHECK_ELT_TAG (elt, "area", nullptr);
  CHECK_ELT_ID (elt, &id);
  CHECK_ELT_ATTRIBUTE_NOT_SUPPORTED (elt, "coords");
  CHECK_ELT_ATTRIBUTE_NOT_SUPPORTED (elt, "first");
  CHECK_ELT_ATTRIBUTE_NOT_SUPPORTED (elt, "last");
  CHECK_ELT_ATTRIBUTE_NOT_SUPPORTED (elt, "text");

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

      return new Area (_doc, id, begin, end);
    }
  else if (dom_elt_has_attribute (elt, "label"))
    {
      string label;
      CHECK_ELT_ATTRIBUTE (elt, "label", &label);
      return new AreaLabeled (_doc, id, label);
    }
  else
    {
      ERROR_SYNTAX_ELT (elt, "missing time or label specifier");
    }
}


// Private: Link.

Link *
ParserXercesC::parseLink (DOMElement *elt, Context *context)
{
  Link *link;
  string id;
  string xconn;
  Connector *conn;
  map<string, string> params;

  CHECK_ELT_TAG (elt, "link", nullptr);
  CHECK_ELT_OPT_ID_AUTO (elt, &id, link);
  CHECK_ELT_ATTRIBUTE (elt, "xconnector", &xconn);

  conn = _doc->getConnector (xconn);
  if (unlikely (conn == nullptr))
    ERROR_SYNTAX_ELT_BAD_ATTRIBUTE (elt, "xconnector");

  link = new Link (_doc, id);
  context->addLink (link);
  g_assert (link->initConnector (conn));

  // Collect children.
  for (DOMElement *child: dom_elt_get_children (elt))
    {
      string tag = dom_elt_get_tag (child);
      if (tag == "linkParam")
        {
          string name, value;
          CHECK_ELT_TAG (elt, "linkParam", nullptr);
          CHECK_ELT_ATTRIBUTE (elt, "name", &name);
          CHECK_ELT_ATTRIBUTE (elt, "value", &value);
          params[name] = value;
        }
      else if (tag == "bind")
        {
          this->parseBind (child, link, &params, context);
        }
      else
        {
          ERROR_SYNTAX_ELT_UNKNOWN_CHILD (elt, child);
        }
    }
  return link;
}

Bind *
ParserXercesC::parseBind (DOMElement *elt, Link *link,
                          unused (map<string, string> *params),
                          Context *context)
{
  Bind *bind;
  string label;
  string comp;
  string value;

  Role *role;
  Connector *conn;

  Node *target;
  Node *derefer;
  Anchor *iface;

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

  derefer = cast (Node *, target->derefer ());
  g_assert_nonnull (derefer);

  iface = nullptr;

  if (dom_elt_try_get_attribute (value, elt, "interface"))
    {
      if (instanceof (Composition *, derefer))
        {
          iface = ((Composition *) derefer)->getPort (value);
        }
      else
        {
          iface = derefer->getAnchor (value);
          if (iface == nullptr) // retry
            {
              iface = target->getAnchor (value);
              if (iface == nullptr) // retry
                {
                  for (Refer *refer:
                         *cast (Media *, derefer)
                         ->getInstSameInstances ())
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
      iface = derefer->getLambda ();
    }

  if (unlikely (iface == nullptr))
    ERROR_SYNTAX_ELT_BAD_ATTRIBUTE (elt, "interface");

  conn = cast (Connector *, link->getConnector ());
  g_assert_nonnull (conn);

  role = conn->getRole (label); // ghost "get"
  if (role == nullptr)
    return nullptr;
  // if (role == nullptr)
  //   {
  //     Condition *cond;
  //     AssessmentStatement *stmt;
  //     AttributeAssessment *assess;
  //     assess = new AttributeAssessment (EventType::ATTRIBUTION, label, "", "");
  //     stmt = new AssessmentStatement ("ne");
  //     stmt->setMainAssessment (assess);
  //     stmt->setOtherAssessment (new ValueAssessment (label));
  //     cond = conn->getCondition ();
  //     if (instanceof (CompoundCondition *, cond))
  //       {
  //         ((CompoundCondition *) cond)->addCondition (stmt);
  //       }
  //     else
  //       {
  //         conn->initCondition (new CompoundCondition (cond, stmt));
  //       }
  //     role = (Role *) assess;
  //   }
  g_assert_nonnull (role);

  bind = new Bind (role, target, iface);
  for (auto it: *params)
    bind->setParameter (it.first, it.second);
  link->addBind (bind);

  // Collect children.
  for (DOMElement *child: dom_elt_get_children (elt))
    {
      string tag = dom_elt_get_tag (child);
      if (tag == "bindParam")
        {
          Parameter *par = this->parseBindParam (child);
          bind->setParameter (par->getName (), par->getValue ());
        }
      else
        {
          ERROR_SYNTAX_ELT_UNKNOWN_CHILD (elt, child);
        }
    }
  return bind;
}

Parameter *
ParserXercesC::parseBindParam (DOMElement *elt)
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
ParserXercesC::warning (const SAXParseException &e)
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
ParserXercesC::error (const SAXParseException &e)
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
ParserXercesC::fatalError (const SAXParseException &e)
{
  this->error (e);
}

GINGA_NCL_END
