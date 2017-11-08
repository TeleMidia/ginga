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

GINGA_NAMESPACE_BEGIN


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
static map<string, FormatterEvent::Type> event_type_table =
  {
   {"presentation", FormatterEvent::PRESENTATION},
   {"attribution", FormatterEvent::ATTRIBUTION},
   {"selection", FormatterEvent::SELECTION},
  };

// Maps condition name to condition code.
static map<string, FormatterEvent::Transition> event_transition_table =
  {
   {"starts", FormatterEvent::START},
   {"stops", FormatterEvent::STOP},
   {"aborts", FormatterEvent::ABORT},
   {"pauses", FormatterEvent::PAUSE},
   {"resumes", FormatterEvent::RESUME},
  };

// Maps action name to action code.
static map<string, FormatterEvent::Transition> event_action_type_table =
  {
   {"start", FormatterEvent::START},
   {"stop", FormatterEvent::STOP},
   {"abort", FormatterEvent::ABORT},
   {"pause", FormatterEvent::PAUSE},
   {"resume", FormatterEvent::RESUME},
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
  for (auto item: _rules)
    if (item.second->getParent () == nullptr)
      delete item.second;
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
          NclContext *body = this->parseBody (child);
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
          this->parseRuleBase (child);
        }
      else if (tag == "transitionBase")
        {
          WARNING_NOT_IMPLEMENTED ("transitions are not supported");
        }
      else if (tag == "regionBase")
        {
          this->parseRegionBase (child);
        }
      else if (tag == "descriptorBase")
        {
          this->parseDescriptorBase (child);
        }
      else if (tag == "connectorBase")
        {
          NclConnectorBase *base = this->parseConnectorBase (child);
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

NclConnectorBase *
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
  if (tag == "connectorBase")
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

void
ParserXercesC::parseRuleBase (DOMElement *elt)
{
  string id;

  CHECK_ELT_TAG (elt, "ruleBase", nullptr);
  CHECK_ELT_OPT_ID_AUTO (elt, &id, ruleBase);

  for (DOMElement *child: dom_elt_get_children (elt))
    {
      string tag = dom_elt_get_tag (child);
      if ( tag == "importBase")
        {
          ERROR_NOT_IMPLEMENTED ("%s: element is not supported",
                                 __error_elt (child).c_str ());
        }
      else if (tag == "rule")
        {
          this->parseRule (child, nullptr);
        }
      else if (tag == "compositeRule")
        {
          this->parseCompositeRule (child, nullptr);
        }
      else
        {
          ERROR_SYNTAX_ELT_UNKNOWN_CHILD (elt, child);
        }
    }
}

void
ParserXercesC::parseCompositeRule (DOMElement *elt,
                                   FormatterPredicate *parent)
{
  string id;
  string value;

  FormatterPredicate *pred;
  PredicateType type;

  CHECK_ELT_TAG (elt, "compositeRule", nullptr);
  CHECK_ELT_ID (elt, &id);

  CHECK_ELT_ATTRIBUTE (elt, "operator", &value);
  if (xstrcaseeq (value, "and"))
    type = PredicateType::CONJUNCTION;
  else if (xstrcaseeq (value, "or"))
    type = PredicateType::DISJUNCTION;
  else if (xstrcaseeq (value, "not"))
    type = PredicateType::NEGATION;
  else
    ERROR_SYNTAX_ELT_BAD_ATTRIBUTE (elt, "operator");

  pred = new FormatterPredicate (type);
  if (parent != nullptr)
    parent->addChild (pred);

  for (DOMElement *child: dom_elt_get_children (elt))
    {
      string tag = dom_elt_get_tag (child);
      if (tag == "rule")
        {
          this->parseRule (child, pred);
        }
      else if (tag == "compositeRule")
        {
          this->parseCompositeRule (child, pred);
        }
      else
        {
          ERROR_SYNTAX_ELT_UNKNOWN_CHILD (elt, child);
        }
    }
  _rules[id] = pred;
}

void
ParserXercesC::parseRule (DOMElement *elt, FormatterPredicate *parent)
{
  string id;
  string var;
  string comp;
  string value;

  FormatterPredicate *pred;
  PredicateTestType test;

  CHECK_ELT_TAG (elt, "rule", nullptr);
  CHECK_ELT_OPT_ID_AUTO (elt, &id, rule);
  CHECK_ELT_ATTRIBUTE (elt, "var", &var);
  CHECK_ELT_ATTRIBUTE (elt, "value", &value);
  CHECK_ELT_ATTRIBUTE (elt, "comparator", &comp);

  if (xstrcaseeq (comp, "eq"))
    test = PredicateTestType::EQ;
  else if (xstrcaseeq (comp, "ne"))
    test = PredicateTestType::NE;
  else if (xstrcaseeq (comp, "lt"))
    test = PredicateTestType::LT;
  else if (xstrcaseeq (comp, "lte"))
    test = PredicateTestType::LE;
  else if (xstrcaseeq (comp, "gt"))
    test = PredicateTestType::GT;
  else if (xstrcaseeq (comp, "gte"))
    test = PredicateTestType::GE;
  else
    ERROR_SYNTAX_ELT_BAD_ATTRIBUTE (elt, "comparator");

  pred = new FormatterPredicate (PredicateType::ATOM);
  pred->setTest ("$__settings__." + var, test, value);

  if (parent != nullptr)
    parent->addChild (pred);

  _rules[id] = pred;
}

// Private: Transition.

void
ParserXercesC::parseTransitionBase (DOMElement *elt)
{
  string id;

  CHECK_ELT_TAG (elt, "transitionBase", nullptr);
  CHECK_ELT_OPT_ID_AUTO (elt, &id, transitionBase);

  for(DOMElement *child: dom_elt_get_children (elt))
    {
      string tag = dom_elt_get_tag (child);
      if (tag == "importBase")
        {
          ERROR_NOT_IMPLEMENTED ("%s: element is not supported",
                                 __error_elt (child).c_str ());
        }
      else if (tag == "transition")
        {
          parseTransition (child);
        }
      else
        {
          ERROR_SYNTAX_ELT_UNKNOWN_CHILD (elt, child);
        }
    }
}

void
ParserXercesC::parseTransition (DOMElement *elt)
{
  string id;
  string value;

  CHECK_ELT_TAG (elt, "transition", nullptr);
  CHECK_ELT_ID (elt, &id);

  CHECK_ELT_ATTRIBUTE (elt, "type", &value);
  if (dom_elt_try_get_attribute (value, elt, "subtype"))
    {
      // No-op.
    }
  if (dom_elt_try_get_attribute (value, elt, "dur"))
    {
      // No-op.
    }
  if (dom_elt_try_get_attribute (value, elt, "startProgress"))
    {
      // No-op.
    }
  if (dom_elt_try_get_attribute (value, elt, "endProgress"))
    {
      // No-op.
    }
  if (dom_elt_try_get_attribute (value, elt, "direction"))
    {
      // No-op.
    }
  if (dom_elt_try_get_attribute (value, elt, "fadeColor"))
    {
      // No-op.
    }
  if (dom_elt_try_get_attribute (value, elt, "horzRepeat"))
    {
      // No-op.
    }
  if (dom_elt_try_get_attribute (value, elt, "vertRepeat"))
    {
      // No-op.
    }
  if (dom_elt_try_get_attribute (value, elt, "borderWidth"))
    {
      // No-op.
    }
  if (dom_elt_try_get_attribute (value, elt, "borderColor"))
    {
      // No-op.
    }
}


// Private: Region.

void
ParserXercesC::parseRegionBase (DOMElement *elt)
{
  string id;

  CHECK_ELT_TAG (elt, "regionBase", nullptr);
  CHECK_ELT_OPT_ID_AUTO (elt, &id, regionBase);

  for (DOMElement *child: dom_elt_get_children (elt))
    {
      string tag = dom_elt_get_tag (child);

      if (tag == "importBase")
        {
          ERROR_NOT_IMPLEMENTED ("%s: element is not supported",
                                 __error_elt (child).c_str ());
        }
      else if (tag == "region")
        {
          this->parseRegion (child, {0, 0, _width, _height});
        }
      else
        {
          ERROR_SYNTAX_ELT_UNKNOWN_CHILD (elt, child);
        }
    }
}

void
ParserXercesC::parseRegion (DOMElement *elt, GingaRect parent_rect)
{
  string id;
  string value;

  GingaRect rect;
  int z;
  int zorder;
  static int last_zorder = 0;

  CHECK_ELT_TAG (elt, "region", nullptr);
  CHECK_ELT_ID (elt, &id);

  rect = parent_rect;
  z = zorder = 0;

  if (dom_elt_try_get_attribute (value, elt, "left"))
    {
      rect.x += ginga_parse_percent (value, parent_rect.width, 0, G_MAXINT);
    }

  if (dom_elt_try_get_attribute (value, elt, "top"))
    {
      rect.y += ginga_parse_percent (value, parent_rect.height, 0, G_MAXINT);
    }

  if (dom_elt_try_get_attribute (value, elt, "width"))
    {
      rect.width = ginga_parse_percent
        (value, parent_rect.width, 0, G_MAXINT);
    }

  if (dom_elt_try_get_attribute (value, elt, "height"))
    {
      rect.height = ginga_parse_percent
        (value, parent_rect.height, 0, G_MAXINT);
    }

  if (dom_elt_try_get_attribute (value, elt, "right"))
    {
      rect.x += parent_rect.width - rect.width
        - ginga_parse_percent (value, parent_rect.width, 0, G_MAXINT);
    }

  if (dom_elt_try_get_attribute (value, elt, "bottom"))
    {
      rect.y += parent_rect.height - rect.height
        - ginga_parse_percent (value, parent_rect.height, 0, G_MAXINT);
    }

  if (dom_elt_try_get_attribute (value, elt, "zIndex"))
    z = xstrtoint (value, 10);
  zorder = last_zorder++;

  GingaRect screen = {0, 0, _width, _height};
  _regions[id]["zIndex"] = xstrbuild ("%d", z);
  _regions[id]["zorder"] = xstrbuild ("%d", zorder);
  _regions[id]["left"] = xstrbuild
    ("%.2f%%", ((double) rect.x / screen.width) * 100.);
  _regions[id]["top"] = xstrbuild
    ("%.2f%%", ((double) rect.y / screen.height) * 100.);
  _regions[id]["width"] = xstrbuild
    ("%.2f%%", ((double) rect.width / screen.width) * 100.);
  _regions[id]["height"] = xstrbuild
    ("%.2f%%", ((double) rect.height / screen.height) * 100.);

  // Collect children.
  for (DOMElement *child: dom_elt_get_children (elt))
    {
      string tag = dom_elt_get_tag (child);
      if (tag == "region")
        this->parseRegion (child, rect);
      else
        ERROR_SYNTAX_ELT_UNKNOWN_CHILD (elt, child);
    }
}


// Private: Descriptor.

void
ParserXercesC::parseDescriptorBase (DOMElement *elt)
{
  string id;

  CHECK_ELT_TAG (elt, "descriptorBase", nullptr);
  CHECK_ELT_OPT_ID_AUTO (elt, &id, descriptorBase);

  for (DOMElement *child: dom_elt_get_children (elt))
    {
      string tag = dom_elt_get_tag (child);
      if (tag == "importBase" || tag == "descriptorSwitch")
        {
          ERROR_NOT_IMPLEMENTED ("%s: element is not supported",
                                 __error_elt (child).c_str ());
        }
      else if (tag == "descriptor")
        {
          parseDescriptor (child);
        }
      else
        {
          ERROR_SYNTAX_ELT_UNKNOWN_CHILD (elt, child);
        }
    }
}

void
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
     "transIn",
     "transOut",
    };

  // List of transition attributes.
  static vector<string> transattr = {"transIn", "transOut"};

  string id;
  string value;

  CHECK_ELT_TAG (elt, "descriptor", nullptr);
  CHECK_ELT_ID (elt, &id);

  _descriptors[id]["_id"] = id;

  if (dom_elt_try_get_attribute (value, elt, "region"))
    {
      if (unlikely (_regions.find (value) == _regions.end ()))
        ERROR_SYNTAX_ELT_BAD_ATTRIBUTE (elt, "region");

      for (auto it: _regions[value])
        _descriptors[id][it.first] = it.second;
    }

  for (auto attr: supported)
    if (dom_elt_try_get_attribute (value, elt, attr))
      _descriptors[id][attr] = value;

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
          _descriptors[id][name] = value;
        }
      else
        {
          ERROR_SYNTAX_ELT_UNKNOWN_CHILD (elt, child);
        }
    }
}


// Private: NclConnector.

NclConnectorBase *
ParserXercesC::parseConnectorBase (DOMElement *elt)
{
  NclConnectorBase *base;
  string id;

  CHECK_ELT_TAG (elt, "connectorBase", nullptr);
  CHECK_ELT_OPT_ID_AUTO (elt, &id, connectorBase);

  base = new NclConnectorBase (_doc, id);
  for (DOMElement *child: dom_elt_get_children (elt))
    {
      string tag = dom_elt_get_tag (child);
      if (tag == "importBase")
        {
          NclDocument *doc;     // FIXME: this is lost (leak?)
          NclConnectorBase *imported;
          string alias;
          string uri;
          imported = this->parseImportBase (child, &doc, &alias, &uri);
          g_assert_nonnull (base);
          base->addBase (imported, alias, uri);
        }
      else if (tag ==  "causalConnector")
        {
          NclConnector *conn = parseCausalConnector (child);
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

NclConnector *
ParserXercesC::parseCausalConnector (DOMElement *elt)
{
  NclConnector *conn;
  string id;
  int ncond;
  int nact;

  CHECK_ELT_TAG (elt, "causalConnector", nullptr);
  CHECK_ELT_ID (elt, &id);

  ncond = 0;
  nact = 0;

  conn = new NclConnector (_doc, id);
  for (DOMElement *child: dom_elt_get_children (elt))
    {
      string tag = dom_elt_get_tag (child);
      if (tag == "simpleCondition")
        {
          this->parseCondition (child, conn, nullptr);
          ncond++;
        }
      else if (tag == "compoundCondition")
        {
          this->parseCompoundCondition (child, conn, nullptr);
          ncond++;
        }
      else if (tag == "simpleAction")
        {
          this->parseSimpleAction (child, conn);
          nact++;
        }
      else if (tag == "compoundAction")
        {
          this->parseCompoundAction (child, conn);
          nact++;
        }
      else if (tag == "connectorParam")
        {
          string name;
          string type;
          CHECK_ELT_ATTRIBUTE (child, "name", &name);
          CHECK_ELT_OPT_ATTRIBUTE (child, "type", &type, "");
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

FormatterPredicate *
ParserXercesC::parseAssessmentStatement (DOMElement *elt)
{
  string comp;

  PredicateTestType test;
  string role_left = "";
  string role_right = "";
  string left;
  string right;

  CHECK_ELT_TAG (elt, "assessmentStatement", nullptr);
  CHECK_ELT_ATTRIBUTE (elt, "comparator", &comp);

  if (comp == "eq")
    test = PredicateTestType::EQ;
  else if (comp == "ne")
    test = PredicateTestType::NE;
  else if (comp == "lt")
    test = PredicateTestType::LT;
  else if (comp == "lte")
    test = PredicateTestType::LE;
  else if (comp == "gt")
    test = PredicateTestType::GT;
  else if (comp == "gte")
    test = PredicateTestType::GE;
  else
    ERROR_SYNTAX_ELT_BAD_ATTRIBUTE (elt, "comparator");

  DOMElement *left_elt = elt->getFirstElementChild ();
  g_assert_nonnull (left_elt);

  string tag = dom_elt_get_tag (left_elt);
  if (tag == "attributeAssessment")
    {
      CHECK_ELT_ATTRIBUTE (left_elt, "role", &role_left);
      left = "$" + role_left;
    }
  else if (tag == "valueAssessment")
    {
      CHECK_ELT_ATTRIBUTE (left_elt, "value", &left);
    }
  else
    {
      ERROR_SYNTAX_ELT_UNKNOWN_CHILD (elt, left_elt);
    }

  DOMElement *right_elt = left_elt->getNextElementSibling ();
  g_assert_nonnull (right_elt);

  tag = dom_elt_get_tag (right_elt);
  if (tag == "attributeAssessment")
    {
      CHECK_ELT_ATTRIBUTE (right_elt, "role", &role_right);
      right = "$" + role_right;
    }
  else if (tag == "valueAssessment")
    {
      CHECK_ELT_ATTRIBUTE (right_elt, "value", &right);
    }
  else
    {
      ERROR_SYNTAX_ELT_UNKNOWN_CHILD (elt, right_elt);
    }

  FormatterPredicate *pred = new FormatterPredicate (PredicateType::ATOM);
  pred->setTest (left, test, right);

  // if (role_left != "")
  //   conn->addPredicateRole (role_left, pred);
  // if (role_right != "")
  //   conn->addPredicateRole (role_right, pred);

  return pred;
}

void
ParserXercesC::parseCompoundCondition (DOMElement *elt,
                                       NclConnector *conn,
                                       FormatterPredicate *parent_pred)
{
  string op;
  string value;

  FormatterPredicate *pred;
  vector <FormatterPredicate *> preds;

  CHECK_ELT_TAG (elt, "compoundCondition", nullptr);
  CHECK_ELT_ATTRIBUTE (elt, "operator", &op);

  if (op != "and" and op != "or")
    ERROR_SYNTAX_ELT_BAD_ATTRIBUTE (elt, "operator");

  // Collect statements.
  for (DOMElement *child: dom_elt_get_children (elt))
    {
      string tag;
      tag  = dom_elt_get_tag (child);
      if (tag == "assessmentStatement")
        {
          pred = this->parseAssessmentStatement (child);
          g_assert_nonnull (pred);
          preds.push_back (pred);
        }
      else if (tag == "compoundStatement")
        {
          g_assert_not_reached ();
        }
      else
        {
          continue;
        }
    }

  // Collapse collected predicates.
  if (preds.size () == 0)
    {
      pred = parent_pred;
    }
  else
    {
      pred = new FormatterPredicate (PredicateType::CONJUNCTION);
      for (auto p: preds)
        pred->addChild (p);
      if (parent_pred != nullptr)
        {
          parent_pred->addChild (pred);
          pred = parent_pred;
        }
    }

  // Collect children.
  for (DOMElement *child: dom_elt_get_children (elt))
    {
      string tag = dom_elt_get_tag (child);
      if (tag == "assessmentStatement" || tag == "compoundStatement")
        continue;

      if (tag == "simpleCondition")
        {
          this->parseCondition (child, conn, pred);
        }
      else if (tag == "compoundCondition")
        {
          this->parseCompoundCondition (child, conn, pred);
        }
      else
        {
          ERROR_SYNTAX_ELT_UNKNOWN_CHILD (elt, child);
        }
    }
}

void
ParserXercesC::parseCondition (DOMElement *elt, NclConnector *conn,
                               FormatterPredicate *pred)
{
  string str;
  string role;
  string key;
  string qualifier;

  FormatterEvent::Type type;
  FormatterEvent::Transition trans;
  map<string, pair<int,int>>::iterator it;

  CHECK_ELT_TAG (elt, "simpleCondition", nullptr);
  CHECK_ELT_ATTRIBUTE (elt, "role", &role);
  CHECK_ELT_OPT_ATTRIBUTE (elt, "qualifier", &qualifier, "or");
  CHECK_ELT_OPT_ATTRIBUTE (elt, "key", &key, "");

  type = (FormatterEvent::Type) -1;
  trans = (FormatterEvent::Transition) -1;

  NclCondition::isReserved (role, &type, &trans);

  if (dom_elt_try_get_attribute (str, elt, "eventType"))
    {
      if (unlikely ((int) type != -1))
        {
          ERROR_SYNTAX_ELT (elt, "eventType of '%s' cannot be overridden",
                            role.c_str ());
        }
      map<string, FormatterEvent::Type>::iterator it;
      if ((it = event_type_table.find (str)) == event_type_table.end ())
        {
          ERROR_SYNTAX_ELT (elt, "bad eventType '%s' for role '%s'",
                            str.c_str (), role.c_str ());
        }
      type = it->second;
    }

  if (dom_elt_try_get_attribute (str, elt, "transition"))
    {
      if (unlikely ((int) trans != -1))
        {
          ERROR_SYNTAX_ELT (elt, "transition of '%s' cannot be overridden",
                            role.c_str ());
        }
      map<string, FormatterEvent::Transition>::iterator it;
      if ((it = event_transition_table.find (str))
          == event_transition_table.end ())
        {
          ERROR_SYNTAX_ELT (elt, "bad transition '%s' for role '%s'",
                            str.c_str (), role.c_str ());
        }
      trans = it->second;
    }

  g_assert ((int) type != -1);
  g_assert ((int) trans != -1);

  if (qualifier != "and" && qualifier != "or")
    ERROR_SYNTAX_ELT_BAD_ATTRIBUTE (elt, "qualifier");

  NclCondition *cond = new NclCondition (type, trans, pred, role, key);
  conn->addCondition (cond);
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
//   map<string, FormatterEvent::Type>::iterator it;
//   string role;
//   string type;
//   string key;
//   string offset;
//   FormatterEvent::Type evttype;
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
ParserXercesC::parseCompoundAction (DOMElement *elt, NclConnector *conn)
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
          this->parseSimpleAction (child, conn);
        }
      else if (tag == "compoundAction")
        {
          this->parseCompoundAction (child, conn);
        }
      else
        {
          ERROR_SYNTAX_ELT_UNKNOWN_CHILD (elt, child);
        }
    }
}

void
ParserXercesC::parseSimpleAction (DOMElement *elt, NclConnector *conn)
{
  string str;
  string tag;
  string role;
  string delay;
  string value;
  string dur;

  FormatterEvent::Type type;
  FormatterEvent::Transition acttype;
  map<string, pair<int,int>>::iterator it;

  CHECK_ELT_TAG (elt, "simpleAction", nullptr);
  CHECK_ELT_ATTRIBUTE (elt, "role", &role);
  CHECK_ELT_OPT_ATTRIBUTE (elt, "delay", &delay, "0s");
  CHECK_ELT_OPT_ATTRIBUTE (elt, "value", &value, "0s");
  CHECK_ELT_OPT_ATTRIBUTE (elt, "dur", &dur, "0s");

  type = (FormatterEvent::Type) -1;
  acttype = (FormatterEvent::Transition) -1;

  NclAction::isReserved (role, &type, &acttype);

  if (dom_elt_try_get_attribute (str, elt, "eventType"))
    {
      if (unlikely ((int) type != -1))
        {
          ERROR_SYNTAX_ELT (elt, "eventType '%s' cannot be overridden",
                            role.c_str ());
        }
      map<string, FormatterEvent::Type>::iterator it;
      if ((it = event_type_table.find (str)) == event_type_table.end ())
        {
          ERROR_SYNTAX_ELT (elt, "bad eventType '%s' for role '%s'",
                            str.c_str (), role.c_str ());
        }
      type = it->second;
    }

  if (dom_elt_try_get_attribute (str, elt, "actionType"))
    {
      if (unlikely ((int) acttype != -1))
        {
          ERROR_SYNTAX_ELT (elt, "actionType of '%s' cannot be overridden",
                            role.c_str ());
        }
      map<string, FormatterEvent::Transition>::iterator it;
      if ((it = event_action_type_table.find (str))
          == event_action_type_table.end ())
        {
          ERROR_SYNTAX_ELT_BAD_ATTRIBUTE (elt, "actionType");
        }
      acttype = it->second;
    }

  g_assert ((int) type != -1);
  g_assert ((int) acttype != -1);

  NclAction *act = new NclAction (type, acttype, role, delay, value, dur);
  conn->addAction (act);
}


// Private: Body.

NclContext *
ParserXercesC::parseBody (DOMElement *elt)
{
  NclContext *body;
  string id;

  CHECK_ELT_TAG (elt, "body", nullptr);
  CHECK_ELT_OPT_ID (elt, &id, _doc->getId ());

  body = _doc->getRoot ();
  for (DOMElement *child: dom_elt_get_children (elt))
    {
      NclNode *node;
      string tag;

      tag = dom_elt_get_tag (child);
      if (tag == "port" || tag == "link")
        {
          continue;               // skip
        }

      if (tag == "property")
        {
          NclProperty *prop = this->parseProperty (child);
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
ParserXercesC::posCompileContext (DOMElement *elt, NclContext *context)
{
  NclNode *node;
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
          if (instanceof (NclContext *, node))
            this->posCompileContext (child, (NclContext*) node);
        }
      else if (tag == "switch")
        {
          g_assert (dom_elt_try_get_attribute (id, child, "id"));
          node = context->getNode (id);
          g_assert_nonnull (node);
          if (instanceof (NclSwitch *, node))
            this->posCompileSwitch (child, (NclSwitch*) node);
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
ParserXercesC::posCompileSwitch (DOMElement *elt, NclSwitch *swtch)
{
  NclNode *node;
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
          if (instanceof (NclContext *, node))
            this->posCompileContext (child, (NclContext*) node);
        }
      else if (tag ==  "switch")
        {
          g_assert (dom_elt_try_get_attribute (id, child, "id"));
          node = swtch->getNode (id);
          g_assert_nonnull (node);
          if (instanceof (NclSwitch *, node))
            this->posCompileSwitch (child, (NclSwitch*) node);
        }
    }
}

void
ParserXercesC::solveNodeReferences (NclComposition *comp)
{
  const vector<NclNode *> *nodes;
  bool del = false;

  if (instanceof (NclSwitch *, comp))
    {
      vector<NclNode *> *aux_nodes = new vector<NclNode *>;
      del = true;
      for (auto item: *cast (NclSwitch *, comp)->getRules ())
        aux_nodes->push_back (item.first);
      nodes = aux_nodes;
    }
  else
    {
      nodes = comp->getNodes ();
    }

  g_assert_nonnull (nodes);

  for (NclNode *node : *nodes)
  {
    g_assert_nonnull (node);
    if (instanceof (NclMediaRefer *, node))
      {
        NclEntity *ref;
        NclMedia *refNode;

        ref = ((NclMediaRefer *) node)->getReferred ();
        g_assert_nonnull (ref);

        refNode = cast (NclMedia *, _doc->getNode (ref->getId ()));
        g_assert_nonnull (refNode);

        ((NclMediaRefer *) node)->initReferred (refNode);
      }
    else if (instanceof (NclComposition *, node))
      {
        this->solveNodeReferences ((NclComposition *) node);
      }
  }

  if (del)
    delete nodes;
}


// Private: NclContext.

NclNode *
ParserXercesC::parseContext (DOMElement *elt)
{
  NclContext *context;
  string id;

  CHECK_ELT_TAG (elt, "context", nullptr);
  CHECK_ELT_ID (elt, &id);
  CHECK_ELT_ATTRIBUTE_NOT_SUPPORTED (elt, "refer");

  context = new NclContext (_doc, id);
  for (DOMElement *child: dom_elt_get_children (elt))
    {
      NclNode *node;
      string tag;

      tag = dom_elt_get_tag (child);
      if (tag == "port" || tag == "link")
        {
          continue;             // skip
        }

      if (tag == "property")
        {
          NclProperty *prop = this->parseProperty (child);
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

NclPort *
ParserXercesC::parsePort (DOMElement *elt, NclComposition *context)
{
  string id;
  string comp;
  string value;

  NclNode *target;
  NclAnchor *iface;

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
          if (instanceof (NclComposition *, target))
            {
              iface = ((NclComposition *) target)->getPort (value);
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

  NclPort *port = new NclPort (_doc, id);
  port->initNode (target);
  port->initInterface (iface);
  return port;
}


// Private: NclSwitch.

NclNode *
ParserXercesC::parseSwitch (DOMElement *elt)
{
  string id;

  NclNode *swtch;
  NclNode *defcomp;
  set<NclNode *> swtch_children;

  CHECK_ELT_TAG (elt, "switch", nullptr);
  CHECK_ELT_ID (elt, &id);
  CHECK_ELT_ATTRIBUTE_NOT_SUPPORTED (elt, "refer");

  swtch = new NclSwitch (_doc, id);
  defcomp = nullptr;

  // Collect children.
  for (DOMElement *child: dom_elt_get_children (elt))
    {
      NclNode *node;
      string tag;

      tag = dom_elt_get_tag (child);
      if (tag == "defaultComponent" || tag == "bindRule")
        {
          continue;               // skip
        }

      if (tag == "switchPort")
        {
          WARNING_NOT_IMPLEMENTED ("switchPort is supported");
          continue;
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
      swtch_children.insert (node);
    }

  // Collect skipped.
  for (DOMElement *child: dom_elt_get_children (elt))
    {
      string tag = dom_elt_get_tag (child);
      if (tag == "bindRule")
        {
          NclNode *node;
          string constituent;
          string rule;

          CHECK_ELT_TAG (child, "bindRule", nullptr);
          CHECK_ELT_ATTRIBUTE (child, "constituent", &constituent);
          CHECK_ELT_ATTRIBUTE (child, "rule", &rule);

          node = nullptr;
          for (auto swtch_child: swtch_children)
            if (swtch_child->getId () == constituent)
              node = swtch_child;

          if (unlikely (node == nullptr))
            ERROR_SYNTAX_ELT_BAD_ATTRIBUTE (child, "constituent");

          if (unlikely (_rules.find (rule) == _rules.end ()))
            ERROR_SYNTAX_ELT_BAD_ATTRIBUTE (child, "rule");

          cast (NclSwitch *, swtch)->addNode (node, _rules[rule]->clone ());
        }
      else if (tag == "defaultComponent")
        {
          NclNode *node;
          string component;

          CHECK_ELT_TAG (child, "defaultComponent", nullptr);
          CHECK_ELT_ATTRIBUTE (child, "component", &component);

          node = nullptr;
          for (auto swtch_child: swtch_children)
            if (swtch_child->getId () == component)
              node = swtch_child;

          if (unlikely (node == nullptr))
            ERROR_SYNTAX_ELT_BAD_ATTRIBUTE (child, "component");

          defcomp = node;
        }
    }

  // Insert default component as the last, tautological rule.
  if (defcomp != nullptr)
    {
      cast (NclSwitch *, swtch)->addNode
        (defcomp, new FormatterPredicate (PredicateType::VERUM));
    }

  return swtch;
}


// Private: NclMedia.

NclNode *
ParserXercesC::parseMedia (DOMElement *elt)
{
  NclNode *media;
  string id;
  string src;
  string value;

  CHECK_ELT_TAG (elt, "media", nullptr);
  CHECK_ELT_ID (elt, &id);

  // NclMediaRefer?
  if (dom_elt_try_get_attribute (value, elt, "refer"))
    {
      NclMedia *refer;

      refer = cast (NclMedia *, _doc->getNode (value));
      g_assert_nonnull (refer);

      media = new NclMediaRefer (_doc, id);
      ((NclMediaRefer *) media)->initReferred (refer);
    }
  else
    {
      if (dom_elt_try_get_attribute (value, elt, "type")
          && value == "application/x-ginga-settings") // settings?
        {
          media = new NclMedia (_doc, id, true);
        }
      else
        {
          media = new NclMedia (_doc, id, false);
        }

      CHECK_ELT_OPT_ATTRIBUTE (elt, "src", &src, "");
      if (!xpathisuri (src) && !xpathisabs (src))
        src = xpathbuildabs (_dirname, src);
      cast (NclMedia *, media)->setSrc (src);

      if (dom_elt_try_get_attribute (value, elt, "descriptor"))
        {
          if (unlikely (_descriptors.find (value) == _descriptors.end ()))
            ERROR_SYNTAX_ELT_BAD_ATTRIBUTE (elt, "descriptor");
          for (auto it: _descriptors[value])
            {
              if (it.first == "_id")
                continue;       // ignored
              media->setProperty (it.first, it.second);
            }
        }
    }

  // Collect children.
  for (DOMElement *child: dom_elt_get_children (elt))
    {
      string tag = dom_elt_get_tag (child);
      if (tag == "area")
        {
          NclAnchor *area = this->parseArea (child);
          g_assert_nonnull (area);
          media->addAnchor (area);
        }
      else if (tag == "property")
        {
          string name;
          string value;

          CHECK_ELT_TAG (child, "property", nullptr);
          CHECK_ELT_ATTRIBUTE (child, "name", &name);
          CHECK_ELT_OPT_ATTRIBUTE (child, "value", &value, "");

          media->setProperty (name, value);

          // NclProperty *prop = this->parseProperty (child);
          // g_assert_nonnull (prop);
          // media->addAnchor (prop);
        }
      else
        {
          ERROR_SYNTAX_ELT_UNKNOWN_CHILD (elt, child);
        }
    }
  return media;
}

NclProperty *
ParserXercesC::parseProperty (DOMElement *elt)
{
  NclProperty *prop;
  string name;
  string value;

  CHECK_ELT_TAG (elt, "property", nullptr);
  CHECK_ELT_ATTRIBUTE (elt, "name", &name);
  CHECK_ELT_OPT_ATTRIBUTE (elt, "value", &value, "");

  prop = new NclProperty (_doc, name);
  prop->setValue (value);
  return prop;
}

NclAnchor *
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

      return new NclArea (_doc, id, begin, end);
    }
  else if (dom_elt_has_attribute (elt, "label"))
    {
      string label;
      CHECK_ELT_ATTRIBUTE (elt, "label", &label);
      return new NclAreaLabeled (_doc, id, label);
    }
  else
    {
      ERROR_SYNTAX_ELT (elt, "missing time or label specifier");
    }
}


// Private: NclLink.

NclLink *
ParserXercesC::parseLink (DOMElement *elt, NclContext *context)
{
  NclLink *link;
  string id;
  string xconn;
  NclConnector *conn;
  map<string, string> params;

  CHECK_ELT_TAG (elt, "link", nullptr);
  CHECK_ELT_OPT_ID_AUTO (elt, &id, link);
  CHECK_ELT_ATTRIBUTE (elt, "xconnector", &xconn);

  conn = _doc->getConnector (xconn);
  if (unlikely (conn == nullptr))
    ERROR_SYNTAX_ELT_BAD_ATTRIBUTE (elt, "xconnector");

  link = new NclLink (_doc, id);

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
          this->parseBind (child, link, conn, &params, context);
        }
      else
        {
          ERROR_SYNTAX_ELT_UNKNOWN_CHILD (elt, child);
        }
    }
  return link;
}

NclBind *
ParserXercesC::parseBind (DOMElement *elt, NclLink *link,
                          NclConnector *conn,
                          map<string, string> *params,
                          NclContext *context)
{
  NclBind *bind;
  string label;
  string comp;
  string value;

  NclRole *role;
  NclNode *target;
  NclNode *derefer;
  NclAnchor *iface;

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

  derefer = cast (NclNode *, target->derefer ());
  g_assert_nonnull (derefer);

  iface = nullptr;

  if (dom_elt_try_get_attribute (value, elt, "interface"))
    {
      if (instanceof (NclComposition *, derefer))
        {
          iface = ((NclComposition *) derefer)->getPort (value);
        }
      else
        {
          iface = derefer->getAnchor (value);
          if (iface == nullptr) // retry
            {
              iface = target->getAnchor (value);
              if (iface == nullptr) // retry
                {
                  for (NclMediaRefer *refer:
                         *cast (NclMedia *, derefer)
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

  role = conn->getRole (label); // ghost "get"
  if (role == nullptr)
    {
      link->setGhostBind (label, xstrbuild ("$%s.%s",
                                            target->getId().c_str (),
                                            iface->getId ().c_str ()));
      return nullptr;
    }
  g_assert_nonnull (role);


  NclBind::RoleType roleType;
  FormatterEvent::Type eventType;
  FormatterEvent::Transition transition;
  FormatterPredicate *pred;

  roleType = (instanceof (NclCondition *, role))
    ? NclBind::CONDITION : NclBind::ACTION;
  eventType = role->getEventType ();
  transition = (roleType == NclBind::CONDITION)
    ? cast (NclCondition *, role)->getTransition ()
    : cast (NclAction *, role)->getTransition ();

  pred = (roleType == NclBind::CONDITION)
    ? cast (NclCondition *, role)->getPredicate ()
    : nullptr;

  bind = new NclBind (label, roleType, eventType, transition, pred,
                      target, iface);

  // Collect link parameters.
  for (auto it: *params)
    {
      string name;
      string value;
      name = it.first;
      value = it.second;
      bind->setParameter (name, value);
    }

  // Collect bind parameters.
  for (DOMElement *child: dom_elt_get_children (elt))
    {
      string tag = dom_elt_get_tag (child);
      if (tag == "bindParam")
        {
          string name;
          string value;
          CHECK_ELT_TAG (child, "bindParam", nullptr);
          CHECK_ELT_ATTRIBUTE (child, "name", &name);
          CHECK_ELT_ATTRIBUTE (child, "value", &value);
          bind->setParameter (name, value);
        }
      else
        {
          ERROR_SYNTAX_ELT_UNKNOWN_CHILD (elt, child);
        }
    }

  // Solve bind parameters.
  if (roleType == NclBind::CONDITION
      && eventType == FormatterEvent::SELECTION)
    {
      NclCondition *cond = cast (NclCondition *, role);
      g_assert_nonnull (cond);

      string key = cond->getKey ();
      if (key[0] == '$')
        bind->getParameter (key.substr (1, key.length () - 1), &key);
      bind->setParameter ("key", key);
    }
  else if (roleType == NclBind::ACTION)
    {
      NclAction *act = cast (NclAction *, role);
      g_assert_nonnull (act);

      string dur = act->getDuration ();
      if (dur[0] == '$')
        bind->getParameter (dur.substr (1, dur.length () - 1), &dur);
      bind->setParameter ("duration", dur);

      string value = act->getValue ();
      if (value[0] == '$')
        bind->getParameter (value.substr (1, value.length () - 1), &value);
      bind->setParameter ("value", value);
    }

  // Solve ghosts.
  for (auto it: *bind->getParameters ())
    {
      if (it.second[0] == '$')
        {
          string value = it.second.substr (1, it.second.length () - 1);
          string ghost = link->getGhostBind (value);
          if (ghost != "")
            bind->setParameter (it.first, ghost);
        }
    }

  link->addBind (bind);
  return bind;
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

GINGA_NAMESPACE_END
