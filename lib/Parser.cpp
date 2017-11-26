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
#include "Parser.h"

#include "Context.h"
#include "Document.h"
#include "Media.h"
#include "MediaSettings.h"

#include <libxml/tree.h>
#include <libxml/parser.h>

GINGA_PRAGMA_DIAG_IGNORE (-Wunused-function)
GINGA_PRAGMA_DIAG_IGNORE (-Wunused-macros)

GINGA_NAMESPACE_BEGIN


// XML helper macros and functions.
#define toCString(s) deconst (char *, (s))
#define toXmlChar(s) (xmlChar *)(deconst (char *, (s).c_str ()))
#define toString(s)  string (deconst (char *, (s)))

static inline bool
xmlGetPropAsString (xmlNode *node, const string &name, string *result)
{
  xmlChar *str = xmlGetProp (node, toXmlChar (name));
  if (str == nullptr)
    return false;
  tryset (result, toString (str));
  g_free (str);
  return true;
}

static bool
xmlIsValidName (const string &value, char *offending)
{
  const char *str;
  char c;

  str = value.c_str ();
  while ((c = *str++) != '\0')
    {
      if (!(g_ascii_isalnum (c) || c == '-' || c == '_'
            || c == ':' || c == '.'))
        {
          tryset (offending, c);
          return false;
        }
    }
  return true;
}


// Parser internal types.

// Element wrapper.
class ParserElt
{
public:
  ParserElt (xmlNode *);
  ~ParserElt ();

  string getTag ();
  xmlNode *getNode ();
  xmlNode *getParentNode ();

  const map<string, string> *getAttributes ();
  bool getAttribute (const string &, string *);
  bool setAttribute (const string &, const string &);

  bool getData (const string &, void **);
  bool setData (const string &, void *, UserDataCleanFunc fn=nullptr);

private:
  string _tag;                  // tag
  xmlNode *_node;               // XML node
  map<string, string> _attrs;   // attributes
  UserData _udata;              // user data
};

// Connector role data.
typedef struct ParserConnRole
{
  xmlNode *node;                // source node
  string role;                  // role label
  Event::Type eventType;        // role event type
  Event::Transition transition; // role transition
  bool condition;               // whether role is condition
  Predicate *predicate;         // role predicate (if condition)
  string value;                 // role value (if attribution)
  string key;                   // role key (if selection)
} ParserConnRole;

// Link bind data.
typedef struct ParserLinkBind
{
  xmlNode *node;                // source node
  string role;                  // bind role
  string component;             // bind component
  string iface;                 // bind interface
  map<string, string> params;   // bind parameters
} ParserLinkBind;

// Parser state.
class ParserState
{
public:
  enum Error
    {
     ERROR_NONE = 0,            // no error
     ERROR_ELT_UNKNOWN,
     ERROR_ELT_MISSING_PARENT,
     ERROR_ELT_BAD_PARENT,
     ERROR_ELT_UNKNOWN_ATTRIBUTE,
     ERROR_ELT_MISSING_ATTRIBUTE,
     ERROR_ELT_BAD_ATTRIBUTE,
     ERROR_ELT_UNKNOWN_CHILD,
     ERROR_ELT_MISSING_CHILD,
     ERROR_ELT_BAD_CHILD,
    };

  ParserState (int, int);
  ~ParserState ();
  ParserState::Error getError (string *);
  Document *process (xmlDoc *);

  // push & pop
  static bool pushNcl (ParserState *, ParserElt *);
  static bool popNcl (ParserState *, ParserElt *);
  static bool pushRegion (ParserState *, ParserElt *);
  static bool popRegion (ParserState *, ParserElt *);
  static bool pushDescriptorParam (ParserState *, ParserElt *);
  static bool pushCausalConnector (ParserState *, ParserElt *);
  static bool popCausalConnector (ParserState *, ParserElt *);
  static bool pushCompoundCondition (ParserState *, ParserElt *);
  static bool pushSimpleCondition (ParserState *, ParserElt *);
  static bool pushCompoundStatement (ParserState *, ParserElt *);
  static bool popCompoundStatement (ParserState *, ParserElt *);
  static bool popAssessmentStatement (ParserState *, ParserElt *);
  static bool pushAttributeAssessment (ParserState *, ParserElt *);
  static bool pushContext (ParserState *, ParserElt *);
  static bool popContext (ParserState *, ParserElt *);
  static bool pushPort (ParserState *, ParserElt *);
  static bool pushMedia (ParserState *, ParserElt *);
  static bool popMedia (ParserState *, ParserElt *);
  static bool pushArea (ParserState *, ParserElt *);
  static bool pushProperty (ParserState *, ParserElt *);
  static bool pushLink (ParserState *, ParserElt *);
  static bool pushLinkParam (ParserState *, ParserElt *);
  static bool pushBind (ParserState *, ParserElt *);

private:
  Document *_doc;                               // NCL document
  xmlDoc *_xml;                                 // DOM tree
  Rect _rect;                                   // screen dimensions
  int _genid;                                   // last generated id
  set<string> _unique;                          // unique attributes
  UserData _udata;                              // user data
  ParserState::Error _error;                    // last error
  string _errorMsg;                             // last error message
  map<xmlNode *, ParserElt *> _eltCache;        // element cache
  list<Object *> _objStack;                     // object stack

  string genId ();
  bool isInUniqueSet (const string &);
  void addToUniqueSet (const string &);
  bool getData (const string &, void **);
  bool setData (const string &, void *, UserDataCleanFunc fn=nullptr);

  // errors
  bool errElt (xmlNode *, ParserState::Error, const string &);
  bool errEltUnknown (xmlNode *);
  bool errEltMissingParent (xmlNode *);
  bool errEltBadParent (xmlNode *, const string &);
  bool errEltUnknownAttribute (xmlNode *, const string &);
  bool errEltMissingAttribute (xmlNode *, const string &);
  bool errEltBadAttribute (xmlNode *, const string &, const string &,
                           const string &explain="");
  bool errEltUnknownChild (xmlNode *, const string &);
  bool errEltMissingChild (xmlNode *, const list<string> &);
  bool errEltBadChild (xmlNode *, const string &, const string &explain="");

  // element cache
  bool eltCacheIndex (xmlNode *, ParserElt **);
  bool eltCacheIndexParent (xmlNode *, ParserElt **);
  bool eltCacheIndexById (const string &, ParserElt **,
                          const list<string> &);
  bool eltCacheIndexByTag (const list<string> &, list<ParserElt *> *);
  bool eltCacheAdd (ParserElt *);

  // object stack
  Object *objStackPeek ();
  Object *objStackPop ();
  void objStackPush (Object *);

  // node processing
  bool processNode (xmlNode *);
};

#define UDATA_GET(obj, key, ptr)                        \
  G_STMT_START                                          \
  {                                                     \
    *(ptr) = nullptr;                                   \
    g_assert ((obj)->getData ((key), (void **)(ptr)));  \
    g_assert_nonnull (*(ptr));                          \
  }                                                     \
  G_STMT_END

#define UDATA_SET(obj, key, ptr, fn)                    \
  G_STMT_START                                          \
  {                                                     \
    (obj)->setData ((key), (void *)(ptr), (fn));        \
  }                                                     \
  G_STMT_END


// NCL syntax.

// Element push functions.
typedef bool (ParserSyntaxEltPush) (ParserState *, ParserElt *);

// Element pop function.
typedef bool (ParserSyntaxEltPop) (ParserState *, ParserElt *);

// NCL attribute info.
typedef struct ParserSyntaxAttr
{
  string name;                  // attribute name
  int flags;                    // processing flags
} ParserSyntaxAttr;

// NCL element info.
typedef struct ParserSyntaxElt
{
  ParserSyntaxEltPush *push;           // push function
  ParserSyntaxEltPop *pop;             // pop function
  int flags;                           // processing flags
  vector<string> parents;              // possible parents
  vector<ParserSyntaxAttr> attributes; // attributes
} ParserSyntaxElt;

// Attribute processing flags.
typedef enum
{
  PARSER_SYNTAX_ATTR_REQUIRED = 1<<1, // required
  PARSER_SYNTAX_ATTR_UNIQUE   = 1<<2, // unique in document
  PARSER_SYNTAX_ATTR_NONEMPTY = 1<<3, // not empty
  PARSER_SYNTAX_ATTR_NAME     = 1<<4, // XML name
} ParserSyntaxAttrFlag;

#define ATTR_REQUIRED  (PARSER_SYNTAX_ATTR_REQUIRED)
#define ATTR_UNIQUE    (PARSER_SYNTAX_ATTR_UNIQUE)
#define ATTR_NONEMPTY  (PARSER_SYNTAX_ATTR_NONEMPTY)
#define ATTR_NAME      (PARSER_SYNTAX_ATTR_NAME)

#define ATTR_NONEMPTY_NAME           (ATTR_NONEMPTY | ATTR_NAME)
#define ATTR_REQUIRED_NONEMPTY_NAME  (ATTR_REQUIRED | ATTR_NONEMPTY_NAME)

#define ATTR_ID        (ATTR_UNIQUE | ATTR_REQUIRED_NONEMPTY_NAME)
#define ATTR_OPT_ID    (ATTR_UNIQUE | ATTR_NONEMPTY_NAME)
#define ATTR_IDREF     (ATTR_REQUIRED_NONEMPTY_NAME)
#define ATTR_OPT_IDREF (ATTR_NONEMPTY_NAME)

// Element processing flags.
typedef enum
{
  PARSER_SYNTAX_ELT_CACHE  = 1<<1, // cache element
  PARSER_SYNTAX_ELT_GEN_ID = 1<<2, // generate id if not present
} ParserSyntaxEltFlag;

#define ELT_CACHE  (PARSER_SYNTAX_ELT_CACHE)
#define ELT_GEN_ID (PARSER_SYNTAX_ELT_GEN_ID)

// Defines table index function.
#define PARSER_SYNTAX_TABLE_INDEX_DEFN(tabname, Type)           \
  static bool                                                   \
  parser_syntax_##tabname##_table_index (const string &str,     \
                                         Type *result)          \
  {                                                             \
    auto it = parser_syntax_##tabname##_table.find (str);       \
    if (it == parser_syntax_##tabname##_table.end ())           \
      return false;                                             \
    tryset (result, it->second);                                \
    return true;                                                \
  }

// NCL Grammar.
static map<string, ParserSyntaxElt> parser_syntax_table =
{
 {"ncl",                        // element name
  {ParserState::pushNcl,        // push function
   ParserState::popNcl,         // pop function
   0,                           // flags
   {},                          // possible parents
   {{"id", ATTR_OPT_ID},        // attributes
    {"title", 0},
    {"xmlns", 0}}},
 },
 {"head",
  {nullptr,
   nullptr,
   0,
   {"ncl"},
   {}},
 },
 {"regionBase",
  {nullptr,
   nullptr,
   0,
   {"head"},
   {{"id", ATTR_OPT_ID},        // unused
    {"device", 0},              // unused
    {"region", 0}}},            // unused
 },
 {"region",
  {ParserState::pushRegion,
   ParserState::popRegion,
   ELT_CACHE,
   {"region", "regionBase"},
   {{"id", ATTR_ID},
    {"left", 0},
    {"right", 0},
    {"top", 0},
    {"bottom", 0},
    {"height", 0},
    {"width", 0},
    {"zIndex", 0},
    {"title", 0}}},             // unused
 },
 {"descriptorBase",
  {nullptr,
   nullptr,
   0,
   {"head"},
   {{"id", ATTR_OPT_ID}}},      // unused
 },
 {"descriptor",
  {nullptr,
   nullptr,
   ELT_CACHE,
   {"descriptorBase"},
   {{"id", ATTR_ID},
    {"left", 0},
    {"right", 0},
    {"top", 0},
    {"bottom", 0},
    {"height", 0},
    {"width", 0},
    {"zIndex", 0},
    {"region", ATTR_OPT_IDREF},
    {"explicitDur", 0},
    {"freeze", 0},
    {"moveLeft", 0},
    {"moveRight", 0},
    {"moveUp", 0},
    {"moveDown", 0},
    {"focusIndex", 0},
    {"focusBorderColor", 0},
    {"focusBorderWidth", 0},
    {"focusBorderTransparency", 0},
    {"focusSrc", 0},
    {"focusSelSrc", 0},
    {"selBorderColor", 0},
    {"player", 0},              // unused
    {"transIn", 0},             // unused
    {"transOut", 0}}},          // unused
 },
 {"descriptorParam",
  {ParserState::pushDescriptorParam,
   nullptr,
   0,
   {"descriptor"},
   {{"name", ATTR_REQUIRED_NONEMPTY_NAME},
    {"value", ATTR_REQUIRED}}},
 },
 {"connectorBase",
  {nullptr,
   nullptr,
   0,
   {"head"},
   {{"id", ATTR_OPT_ID}}},       // unused
 },
 {"causalConnector",
  {ParserState::pushCausalConnector,
   ParserState::popCausalConnector,
   ELT_CACHE,
   {"connectorBase"},
   {{"id", ATTR_ID}}},
 },
 {"connectorParam",
  {nullptr,
   nullptr,
   0,
   {"causalConnector"},
   {{"name", ATTR_NONEMPTY_NAME}}}, // unused
 },
 {"compoundCondition",
  {ParserState::pushCompoundCondition,
   nullptr,
   ELT_CACHE,
   {"causalConnector", "compoundCondition"},
   {{"operator", 0},            // ignored
    {"delay", 0}}},             // ignored
 },
 {"simpleCondition",
  {ParserState::pushSimpleCondition,
   nullptr,
   0,
   {"causalConnector", "compoundCondition"},
   {{"role", ATTR_REQUIRED_NONEMPTY_NAME},
    {"eventType", 0},
    {"key", 0},
    {"transition", 0},
    {"delay", 0},               // ignored
    {"min", 0},                 // ignored
    {"max", 0},                 // ignored
    {"qualifier", 0}}},         // ignored
 },
 {"compoundAction",
  {nullptr,
   nullptr,
   0,
   {"causalConnector", "compoundAction"},
   {{"operator", 0},            // ignored
    {"delay", 0}}},             // ignored
 },
 {"simpleAction",
  {ParserState::pushSimpleCondition, // reused
   nullptr,
   0,
   {"causalConnector", "compoundAction"},
   {{"role", ATTR_REQUIRED_NONEMPTY_NAME},
    {"eventType", 0},
    {"actionType", 0},
    {"value", 0},
    {"delay", 0},               // ignored
    {"duration", 0},            // ignored
    {"min", 0},                 // ignored
    {"max", 0},                 // ignored
    {"min", 0},                 // ignored
    {"qualifier", 0},           // ignored
    {"repeat", 0},              // ignored
    {"repeatDelay", 0},         // ignored
    {"by", 0}}},                // ignored
 },
 {"compoundStatement",
  {ParserState::pushCompoundStatement,
   ParserState::popCompoundStatement,
   ELT_CACHE,
   {"compoundCondition", "compoundStatement"},
   {{"operator", ATTR_REQUIRED},
    {"isNegated", 0}}},
 },
 {"assessmentStatement",
  {nullptr,
   ParserState::popAssessmentStatement,
   ELT_CACHE,
   {"compoundCondition", "compoundStatement"},
   {{"comparator", ATTR_REQUIRED}}},
 },
 {"attributeAssessment",
  {ParserState::pushAttributeAssessment,
   nullptr,
   0,
   {"assessmentStatement"},
   {{"role", ATTR_REQUIRED_NONEMPTY_NAME},
    {"eventType", 0},                  // ignored
    {"key", 0},                        // ignored
    {"attributeType", 0},              // ignored
    {"offset", 0}}},                   // ignored
 },
 {"valueAssessment",
  {ParserState::pushAttributeAssessment, // reused
   nullptr,
   0,
   {"assessmentStatement"},
   {{"value", ATTR_REQUIRED}}},
 },
 {"body",
  {ParserState::pushContext,    // reused
   ParserState::popContext,     // reused
   ELT_CACHE,
   {"ncl"},
   {{"id", ATTR_OPT_ID}}},
 },
 {"context",
  {ParserState::pushContext,
   ParserState::popContext,
   ELT_CACHE,
   {"body", "context"},
   {{"id", ATTR_ID},
    {"refer", ATTR_OPT_IDREF}}}, // unused
 },
 {"port",
  {ParserState::pushPort,
   nullptr,
   ELT_CACHE,
   {"body", "context"},
   {{"id", ATTR_ID},
    {"component", ATTR_IDREF},
    {"interface", ATTR_OPT_IDREF}}},
 },
 {"media",
  {ParserState::pushMedia,
   ParserState::popMedia,
   ELT_CACHE,
   {"body", "context", "switch"},
   {{"id", ATTR_ID},
    {"src", 0},
    {"type", 0},
    {"descriptor", ATTR_OPT_IDREF},
    {"refer", ATTR_OPT_IDREF},  // unused
    {"instance", 0}}},          // unused
 },
 {"area",
  {ParserState::pushArea,
   nullptr,
   0,
   {"media"},
   {{"id", ATTR_ID},
    {"begin", 0},
    {"end", 0}}},
 },
 {"property",
  {ParserState::pushProperty,
   nullptr,
   0,
   {"body", "context", "media"},
   {{"name", ATTR_REQUIRED_NONEMPTY_NAME},
    {"value", 0}}},
 },
 {"link",
  {ParserState::pushLink,
   nullptr,
   ELT_CACHE | ELT_GEN_ID,
   {"body", "context"},
   {{"id", ATTR_OPT_ID},
    {"xconnector", ATTR_IDREF}}},
 },
 {"linkParam",
  {ParserState::pushLinkParam,
   nullptr,
   0,
   {"link"},
   {{"name", ATTR_REQUIRED_NONEMPTY_NAME},
    {"value", ATTR_REQUIRED}}},
 },
 {"bind",
  {ParserState::pushBind,
   nullptr,
   ELT_CACHE,
   {"link"},
   {{"role", ATTR_REQUIRED_NONEMPTY_NAME},
    {"component", ATTR_IDREF},
    {"interface", ATTR_OPT_IDREF}}},
 },
 {"bindParam",
  {ParserState::pushLinkParam,  // reused
   nullptr,
   0,
   {"bind"},
   {{"name", ATTR_REQUIRED_NONEMPTY_NAME},
    {"value", ATTR_REQUIRED}}},
 },
};

// Indexes syntax table.
static bool
parser_syntax_table_index (const string &tag, ParserSyntaxElt **result)
{
  auto it = parser_syntax_table.find (tag);
  if (it == parser_syntax_table.end ())
    return false;
  tryset (result, &it->second);
  return true;
}

// Gets possible children of a given element.
static map<string, bool>
parser_syntax_table_get_possible_children (const string &tag)
{
  map<string, bool> result;
  for (auto it: parser_syntax_table)
    for (auto parent: it.second.parents)
      if (parent == tag)
        result[it.first] = true;
  return result;
}

// Reserved connector roles.
static map<string, pair<Event::Type, Event::Transition>>
parser_syntax_reserved_role_table =
{
 {"onBegin",            {Event::PRESENTATION, Event::START}}, // conditions
 {"onEnd",              {Event::PRESENTATION, Event::STOP}},
 {"onAbort",            {Event::PRESENTATION, Event::ABORT}},
 {"onPause",            {Event::PRESENTATION, Event::PAUSE}},
 {"onResumes",          {Event::PRESENTATION, Event::RESUME}},
 {"onBeginAttribution", {Event::ATTRIBUTION,  Event::START}},
 {"onEndAttribution",   {Event::SELECTION,    Event::STOP}},
 {"onSelection",        {Event::SELECTION,    Event::START}},
 {"start",              {Event::PRESENTATION, Event::START}}, // actions
 {"stop",               {Event::PRESENTATION, Event::STOP}},
 {"abort",              {Event::PRESENTATION, Event::ABORT}},
 {"pause",              {Event::PRESENTATION, Event::PAUSE}},
 {"resume",             {Event::PRESENTATION, Event::RESUME}},
 {"set",                {Event::ATTRIBUTION,  Event::START}},
};

// Index reserved role table.
static bool
parser_syntax_reserved_role_table_index (const string &role,
                                         bool *condition,
                                         Event::Type *type,
                                         Event::Transition *transition)
{
  auto it = parser_syntax_reserved_role_table.find (role);
  if (it == parser_syntax_reserved_role_table.end ())
    return false;
  tryset (condition, xstrhasprefix (role, "on"));
  tryset (type, it->second.first);
  tryset (transition, it->second.second);
  return true;
}

// Known event types.
static map<string, Event::Type> parser_syntax_event_type_table =
{
 {"presentation", Event::PRESENTATION},
 {"attribution",  Event::ATTRIBUTION},
 {"selection",    Event::SELECTION},
};

// Known transitions.
static map<string, Event::Transition> parser_syntax_transition_table =
{
 {"start",   Event::START},
 {"starts",  Event::START},
 {"pause",   Event::PAUSE},
 {"pauses",  Event::PAUSE},
 {"resume",  Event::RESUME},
 {"resumes", Event::RESUME},
 {"stop",    Event::STOP},
 {"stops",   Event::STOP},
 {"abort",   Event::ABORT},
 {"aborts",  Event::ABORT},
};

// Known connectives.
static map<string, Predicate::Type> parser_syntax_connective_table =
{
 {"not", Predicate::NEGATION},
 {"and", Predicate::CONJUNCTION},
 {"or",  Predicate::DISJUNCTION},
};

// Known comparators.
static map<string, Predicate::Test> parser_syntax_comparator_table =
{
 {"eq",  Predicate::EQ},
 {"ne",  Predicate::NE},
 {"lt",  Predicate::LT},
 {"lte", Predicate::LE},
 {"gt",  Predicate::GT},
 {"gte", Predicate::GE},
};

PARSER_SYNTAX_TABLE_INDEX_DEFN (event_type, Event::Type);
PARSER_SYNTAX_TABLE_INDEX_DEFN (transition, Event::Transition);
PARSER_SYNTAX_TABLE_INDEX_DEFN (connective, Predicate::Type);
PARSER_SYNTAX_TABLE_INDEX_DEFN (comparator, Predicate::Test);

// Parse boolean.
static bool
parser_syntax_parse_bool (const string &str, bool *result)
{
  return ginga::try_parse_bool (str, result);
}


// ParserElt: public.

ParserElt::ParserElt (xmlNode *node)
{
  g_assert_nonnull (node);
  _node = node;
  _tag = toString (node->name);
}

ParserElt::~ParserElt ()
{
}

string
ParserElt::getTag ()
{
  return _tag;
}

xmlNode *
ParserElt::getNode ()
{
  return _node;
}

xmlNode *
ParserElt::getParentNode ()
{
  return _node->parent;
}

const map<string, string> *
ParserElt::getAttributes ()
{
  return &_attrs;
}

bool
ParserElt::getAttribute (const string &name, string *result)
{
  auto it = _attrs.find (name);
  if (it == _attrs.end ())
    return false;
  tryset (result, it->second);
  return true;
}

bool
ParserElt::setAttribute (const string &name, const string &value)
{
  bool result = !this->getAttribute (name, nullptr);
  _attrs[name] = value;
  return result;
}

bool
ParserElt::getData (const string &key, void **result)
{
  return _udata.getData (key, result);
}

bool
ParserElt::setData (const string &key, void *value, UserDataCleanFunc fn)
{
  return _udata.setData (key, value, fn);
}


// ParserState: private.

string
ParserState::genId ()
{
  return xstrbuild ("__unamed-%d__", _genid++);
}

bool
ParserState::isInUniqueSet (const string &str)
{
  return _unique.find (str) != _unique.end ();
}

void
ParserState::addToUniqueSet (const string &str)
{
  _unique.insert (str);
}

bool
ParserState::getData (const string &key, void **result)
{
  return _udata.getData (key, result);
}

bool
ParserState::setData (const string &key, void *value, UserDataCleanFunc fn)
{
  return _udata.setData (key, value, fn);
}

bool
ParserState::errElt (xmlNode *node, ParserState::Error error,
                     const string &message)
{
  g_assert (error != ParserState::ERROR_NONE);
  g_assert_nonnull (node);
  _error = error;
  _errorMsg = xstrbuild ("Syntax error at line %d: Element <%s>: ",
                         node->line, toCString (node->name)) + message;
  return false;
}

bool
ParserState::errEltUnknown (xmlNode *node)
{
  return this->errElt (node, ParserState::ERROR_ELT_UNKNOWN,
                       "Unknown element");
}

bool
ParserState::errEltMissingParent (xmlNode *node)
{
  return this->errElt (node, ParserState::ERROR_ELT_MISSING_PARENT,
                       "Missing parent");
}

bool
ParserState::errEltBadParent (xmlNode *node, const string &parent)
{
  return this->errElt (node, ParserState::ERROR_ELT_BAD_PARENT,
                       "Bad parent <" + parent + ">");
}

bool
ParserState::errEltUnknownAttribute (xmlNode *node, const string &name)
{
  return this->errElt (node, ParserState::ERROR_ELT_UNKNOWN_ATTRIBUTE,
                       "Unknown attribute '" + name + "'");
}

bool
ParserState::errEltMissingAttribute (xmlNode *node, const string &name)
{
  return this->errElt (node, ParserState::ERROR_ELT_MISSING_ATTRIBUTE,
                       "Missing attribute '" + name + "'");
}

bool
ParserState::errEltBadAttribute (xmlNode *node, const string &name,
                                 const string &value, const string &explain)
{
  string msg = "Bad value '" + value + "' for attribute '" + name + "'";
  if (explain != "")
    msg += " (" + explain + ")";
  return this->errElt (node, ParserState::ERROR_ELT_BAD_ATTRIBUTE, msg);
}

bool
ParserState::errEltUnknownChild (xmlNode *node, const string &name)
{
  return this->errElt (node, ParserState::ERROR_ELT_UNKNOWN_CHILD,
                       "Unknown child <" + name + ">");
}

bool
ParserState::errEltMissingChild (xmlNode *node,
                                 const list<string> &children)
{
  string msg;
  auto it = children.begin ();

  g_assert (it != children.end ());
  msg = "Missing child <" + *it + ">";
  while (++it != children.end ())
    msg += " or <" + *it + ">";

  return this->errElt (node, ParserState::ERROR_ELT_MISSING_CHILD, msg);
}

bool
ParserState::errEltBadChild (xmlNode *node, const string &child,
                             const string &explain)
{
  string msg = "Bad child <" + child + ">";
  if (explain != "")
    msg += " (" + explain + ")";
  return this->errElt (node, ParserState::ERROR_ELT_BAD_CHILD, msg);
}

bool
ParserState::eltCacheIndex (xmlNode *node, ParserElt **result)
{
  auto it = _eltCache.find (node);
  if (it == _eltCache.end ())
    return false;
  g_assert_nonnull (it->second);
  tryset (result, it->second);
  return true;
}

bool
ParserState::eltCacheIndexParent (xmlNode *node, ParserElt **result)
{
  if (node->parent == nullptr)
    return false;
  return this->eltCacheIndex (node->parent, result);
}

bool
ParserState::eltCacheIndexById (const string &id, ParserElt **result,
                                const list<string> &tags)
{
  string elt_tag;
  string elt_id;
  for (auto it: _eltCache)
    {
      // Check tag.
      elt_tag = it.second->getTag ();
      if (std::find (tags.begin (), tags.end (), elt_tag) == tags.end ())
        continue;

      // Check id.
      if (it.second->getAttribute ("id", &elt_id) && id == elt_id)
        {
          tryset (result, it.second);
          return true;
        }
    }
  return false;
}

bool
ParserState::eltCacheIndexByTag (const list<string> &tags,
                                 list<ParserElt *> *result)
{
  bool flag = false;
  g_assert_nonnull (result);
  for (auto it: _eltCache)
    {
      if (std::find (tags.begin (), tags.end (), it.second->getTag ())
          != tags.end ())
        {
          result->push_back (it.second);
          flag = true;
        }
    }
  return flag;
}

bool
ParserState::eltCacheAdd (ParserElt *elt)
{
  xmlNode *node = elt->getNode ();
  if (_eltCache.find (node) != _eltCache.end ())
    return false;
  _eltCache[node] = elt;
  return true;
}

Object *
ParserState::objStackPeek ()
{
  return (_objStack.empty ()) ? nullptr : _objStack.back ();
}

Object *
ParserState::objStackPop ()
{
  Object *obj = this->objStackPeek ();
  if (obj == nullptr)
    return nullptr;
  _objStack.pop_back ();
  return obj;
}

void
ParserState::objStackPush (Object *obj)
{
  g_assert_nonnull (obj);
  _objStack.push_back (obj);
}

bool
ParserState::processNode (xmlNode *node)
{
  string tag;
  ParserSyntaxElt *eltsyn;
  map<string, string> attrs;
  map<string, bool> possible;
  list<xmlNode *> children;
  ParserElt *elt;
  bool cached;
  bool status;

  g_assert_nonnull (node);
  tag = toString (node->name);

  // Check if element is known.
  if (unlikely (!parser_syntax_table_index (tag, &eltsyn)))
    return this->errEltUnknown (node);

  // Check parent.
  g_assert_nonnull (node->parent);
  if (eltsyn->parents.size () > 0)
    {
      string parent;
      bool found;

      if (unlikely (node->parent->type != XML_ELEMENT_NODE))
        return this->errEltMissingParent (node);

      parent = toString (node->parent->name);
      found = false;
      for (auto par: eltsyn->parents)
        {
          if (parent == par)
            {
              found = true;
              break;
            }
        }
      if (unlikely (!found))
        return this->errEltBadParent (node, parent);
    }

  // Collect attributes.
  for (auto attrsyn: eltsyn->attributes)
    {
      string value;
      string explain;

      if (!xmlGetPropAsString (node, attrsyn.name, &value)) // not found
        {
          if (attrsyn.name == "id" && eltsyn->flags & ELT_GEN_ID)
            {
              attrs["id"] = this->genId ();
              continue;
            }
          if (unlikely (attrsyn.flags & ATTR_REQUIRED))
            return this->errEltMissingAttribute (node, attrsyn.name);
          else
            continue;
        }

      if (unlikely ((attrsyn.flags & ATTR_NONEMPTY) && value == ""))
        {
          return this->errEltBadAttribute
            (node, attrsyn.name, value, "must not be empty");
        }

      if (attrsyn.flags & ATTR_NAME)
        {
          char offending;
          if (unlikely (!xmlIsValidName (value, &offending)))
            {
              return this->errEltBadAttribute
              (node, attrsyn.name, value,
               xstrbuild ("must not contain '%c'", offending));
            }
        }

      if (attrsyn.flags & ATTR_UNIQUE)
        {
          if (unlikely (this->isInUniqueSet (value)))
            {
              return this->errEltBadAttribute
                (node, attrsyn.name, value, "must be unique");
            }
          else
            {
              this->addToUniqueSet (value);
            }
        }

      attrs[attrsyn.name] = value;
    }

  // Check for unknown attributes.
  for (xmlAttr *prop = node->properties;
       prop != nullptr; prop = prop->next)
    {
      string name = toString (prop->name);
      if (unlikely (attrs.find (name) == attrs.end ()))
        return this->errEltUnknownAttribute (node, name);
    }

  // Collect children.
  possible = parser_syntax_table_get_possible_children (tag);
  for (xmlNode *child = node->children; child; child = child->next)
    {
      if (child->type != XML_ELEMENT_NODE)
        continue;

      string child_tag = toString (child->name);
      if (unlikely (possible.find (child_tag) == possible.end ()))
        return this->errEltUnknownChild (node, child_tag);

      children.push_back (child);
    }

  // Allocate and initialize element wrapper.
  elt = new ParserElt (node);
  for (auto it: attrs)
    g_assert (elt->setAttribute (it.first, it.second));

  // Initialize flags.
  cached = false;
  status = true;

  // Push element.
  if (unlikely (eltsyn->push != nullptr && !eltsyn->push (this, elt)))
    {
      status = false;
      goto done;
    }

  // Save element into cache.
  if (eltsyn->flags & ELT_CACHE)
    {
      cached = true;
      g_assert (this->eltCacheAdd (elt));
    }

  // Process each child.
  for (auto child: children)
    {
      if (unlikely (!this->processNode (child)))
        {
          status = false;
          goto done;
        }
    }

  // Pop element.
  if (unlikely (eltsyn->pop != nullptr && !eltsyn->pop (this, elt)))
    {
      status = false;
      goto done;
    }

 done:
  if (!cached)
    delete elt;
  return status;
}


// ParserState: public.

ParserState::ParserState (int width, int height)
{
  _doc = nullptr;
  _xml = nullptr;
  g_assert_cmpint (width, >, 0);
  g_assert_cmpint (height, >, 0);
  _rect = {0, 0, width, height};
  _genid = 0;
  _error = ParserState::ERROR_NONE;
  _errorMsg = "no error";
}

ParserState::~ParserState ()
{
  for (auto it: _eltCache)
    delete it.second;
}

ParserState::Error
ParserState::getError (string *message)
{
  tryset (message, _errorMsg);
  return _error;
}

Document *
ParserState::process (xmlDoc *xml)
{
  xmlNode *root;

  g_assert_nonnull (xml);
  _xml = xml;
  _doc = new Document ();

  root = xmlDocGetRootElement (xml);
  g_assert_nonnull (root);

  if (unlikely (!this->processNode (root)))
    {
      delete _doc;
      _doc = nullptr;
      return nullptr;
    }

  g_assert_nonnull (_doc);
  return _doc;
}


// ParserState: push & pop.

// <ncl>
bool
ParserState::pushNcl (ParserState *st, ParserElt *elt)
{
  Context *root;
  string id;

  root = st->_doc->getRoot ();
  g_assert_nonnull (root);

  if (elt->getAttribute ("id", &id))
    root->addAlias (id);

  st->objStackPush (root);
  return true;
}

bool
ParserState::popNcl (ParserState *st, unused (ParserElt *elt))
{
  list<ParserElt *> desc_list;
  list<ParserElt *> media_list;
  list<ParserElt *> link_list;

  // Resolve descriptor reference to region.
  // (I.e., move region attributes to associated descriptor.)
  if (st->eltCacheIndexByTag ({"descriptor"}, &desc_list))
    {
      for (auto desc_elt: desc_list)
        {
          string region_id;
          ParserElt *region_elt;

          if (!desc_elt->getAttribute ("region", &region_id))
            continue;           // nothing to do

          if (unlikely (!st->eltCacheIndexById
                        (region_id, &region_elt, {"region"})))
            {
              return st->errEltBadAttribute
                (desc_elt->getNode (), "region", region_id,
                 "no such region");
            }

          for (auto it: *region_elt->getAttributes ())
            if (it.first != "id")
              desc_elt->setAttribute (it.first, it.second);
        }
    }

  // Resolve media reference to descriptor.
  // (I.e., move descriptor attributes to associated media.)
  if (st->eltCacheIndexByTag ({"media"}, &media_list))
    {
      for (auto media_elt: media_list)
        {
          string media_id;
          Media *media;

          string desc_id;
          ParserElt *desc_elt;

          if (!media_elt->getAttribute ("descriptor", &desc_id))
            continue;           // nothing to do

          if (!st->eltCacheIndexById (desc_id, &desc_elt, {"descriptor"}))
            {
              return st->errEltBadAttribute (media_elt->getNode (),
                                             "descriptor", desc_id,
                                             "no such descriptor");
            }

          g_assert (media_elt->getAttribute ("id", &media_id));
          media = cast (Media *, st->_doc->getObjectByIdOrAlias (media_id));
          g_assert_nonnull (media);

          for (auto it: *desc_elt->getAttributes ())
            {
              if (it.first == "id" || it.first == "region")
                continue;       // nothing to do
              if (media->getAttributionEvent (it.first) != nullptr)
                continue;           // already defined
              media->addAttributionEvent (it.first);
              media->setProperty (it.first, it.second);
            }
        }
    }

  // Resolve link reference to connector.
  // (I.e., finish links parsing and add them to contexts.)
  if (st->eltCacheIndexByTag ({"link"}, &link_list))
    {
      for (auto link_elt: link_list)
        {
          string conn_id;
          ParserElt *conn_elt;
          set<string> *tests;
          list<ParserConnRole> *roles;
          list<ParserLinkBind> *binds;
          Context *ctx;

          list<pair<ParserConnRole *, ParserLinkBind *>> bound;
          list<pair<string, ParserLinkBind *>> bound_tests;
          map<string, ParserLinkBind *> ghosts;

          g_assert (link_elt->getAttribute ("xconnector", &conn_id));
          if (unlikely (!st->eltCacheIndexById (conn_id, &conn_elt,
                                                {"causalConnector"})))
            {
              return st->errEltBadAttribute (link_elt->getNode (),
                                             "xconnector", conn_id,
                                             "no such connector");
            }
          UDATA_GET (conn_elt, "tests", &tests);
          UDATA_GET (conn_elt, "roles", &roles);
          UDATA_GET (link_elt, "binds", &binds);
          UDATA_GET (link_elt, "context", &ctx);

          // Collect event, test and ghost binds.
          for (auto &bind: *binds)
            {
              bool found = false;
              for (auto &role: *roles)
                {
                  if (bind.role == role.role)
                    {
                      bound.push_back (std::make_pair (&role, &bind));
                      found = true;
                      break;
                    }
                }
              if (found)
                {
                  continue;     // found
                }
              if (tests->find (bind.role) != tests->end ())
                {
                  bound_tests.push_back (std::make_pair (bind.role, &bind));
                  continue;     // found
                }
              ghosts[bind.role] = &bind; // ghost bind
            }

          // Check if link matches connector.
          for (auto &role: *roles)
            {
              bool found = false;
              for (auto it: bound)
                {
                  if (&role == it.first)
                    {
                      found = true;
                      break;
                    }
                }
              if (found)
                continue;
              for (auto it: bound_tests)
                {
                  if (role.role == it.first)
                    {
                      found = true;
                      break;
                    }
                }
              if (found)
                continue;
              return st->errEltBadAttribute
                (link_elt->getNode (), "xconnector", conn_id,
                 "link does not match connector, "
                 "role '" + role.role + "' not bound");
            }

          // Merge link parameters into bind parameters.
          // TODO:

          // Resolve ghost binds.
          for (auto it: ghosts)
            {
              Object *obj;

              obj = ctx->getChildById (it.second->component);
              if (unlikely (obj == nullptr))
                {
                  return st->errEltBadAttribute
                    (it.second->node, "component", it.second->component,
                     "no such object in scope");
                }

              if (unlikely (it.second->iface == ""))
                {
                  return st->errEltBadAttribute
                    (it.second->node, "interface", it.second->iface,
                     "must not be empty");
                }
            }

          // Resolve tests.
          // TODO:

          // Resolve events.
          list<Action> conditions;
          list<Action> actions;
          for (auto it: bound)
            {
              Object *obj;
              Action act;
              string iface;

              obj = ctx->getChildById (it.second->component);
              if (unlikely (obj == nullptr))
                {
                  return st->errEltBadAttribute
                    (it.second->node, "component", it.second->component,
                     "no such object in scope");
                }

              iface = it.second->iface;
              switch (it.first->eventType)
                {
                case Event::PRESENTATION:
                  if (iface == "")
                    iface = "@lambda";
                  act.event = obj->getPresentationEvent (iface);
                  if (unlikely (act.event == nullptr))
                    {
                      return st->errEltBadAttribute
                        (it.second->node, "interface", it.second->iface,
                         "no such interface");
                    }
                  break;

                case Event::ATTRIBUTION:
                  if (unlikely (iface == ""))
                    {
                      return st->errEltBadAttribute
                        (it.second->node, "interface", iface,
                         "must not be empty");
                    }
                  obj->addAttributionEvent (iface);
                  act.event = obj->getAttributionEvent (iface);
                  g_assert_nonnull (act.event);
                  act.value = it.first->value; // fixme: resolve
                  break;

                case Event::SELECTION:
                  //
                  // FIXME: Handle selection events mapped by context ports.
                  //
                  g_assert (instanceof (Media *, obj));
                  if (unlikely (iface != ""))
                    {
                      return st->errEltBadAttribute
                        (it.second->node, "interface", iface,
                         "must be empty");
                    }
                  act.value = it.first->key; // fixme: resolve
                  obj->addSelectionEvent (act.value);
                  act.event = obj->getSelectionEvent (act.value);
                  g_assert_nonnull (act.event);
                  act.event->setParameter ("key", act.value);
                  break;
                default:
                  g_assert_not_reached ();
                }
              g_assert_nonnull (act.event);
              act.transition = it.first->transition;
              act.predicate = it.first->predicate; // fixme: resolve
            }
        }
    }

  g_assert_nonnull (st->objStackPop ());
  return true;
}

// <region>
static void
savedRectCleanup (void *ptr)
{
  delete (Rect *) ptr;
}

bool
ParserState::pushRegion (ParserState *st, ParserElt *elt)
{
  static int last_zorder = 0;
  xmlNode *parent_node;
  Rect *saved_rect;
  Rect screen;
  Rect parent;
  Rect rect;
  string str;

  parent_node = elt->getParentNode ();
  g_assert_nonnull (parent_node);

  if (toString (parent_node->name) != "region") // this is a root region
    {
      saved_rect = new Rect;
      screen = *saved_rect = st->_rect;
      UDATA_SET (st, "saved_rect", saved_rect, savedRectCleanup);
    }
  else
    {
      UDATA_GET (st, "saved_rect", &saved_rect);
      screen = *saved_rect;
    }

  rect = parent = st->_rect;
  if (elt->getAttribute ("left", &str))
    {
      rect.x += ginga::parse_percent (str, parent.width, 0, G_MAXINT);
    }
  if (elt->getAttribute ("top", &str))
    {
      rect.y += ginga::parse_percent (str, parent.height, 0, G_MAXINT);
    }
  if (elt->getAttribute ("width", &str))
    {
      rect.width = ginga::parse_percent (str, parent.width, 0, G_MAXINT);
    }
  if (elt->getAttribute ("height", &str))
    {
      rect.height = ginga::parse_percent (str, parent.height, 0, G_MAXINT);
    }
  if (elt->getAttribute ("right", &str))
    {
      rect.x += parent.width - rect.width
        - ginga::parse_percent (str, parent.width, 0, G_MAXINT);
    }
  if (elt->getAttribute ("bottom", &str))
    {
      rect.y += parent.height - rect.height
        - ginga::parse_percent (str, parent.height, 0, G_MAXINT);
    }

  // Update region position to absolute values.
  st->_rect = rect;
  double left = ((double) rect.x / screen.width) * 100.;
  double top = ((double) rect.y / screen.height) * 100.;
  double width = ((double) rect.width / screen.width) * 100.;
  double height = ((double) rect.height / screen.height) * 100.;
  elt->setAttribute ("zorder", xstrbuild ("%d", last_zorder++));
  elt->setAttribute ("left", xstrbuild ("%.2f%%", left));
  elt->setAttribute ( "top", xstrbuild ("%.2f%%", top));
  elt->setAttribute ("width", xstrbuild ("%.2f%%", width));
  elt->setAttribute ("height", xstrbuild ("%.2f%%", height));

  return true;
}

bool
ParserState::popRegion (ParserState *st, ParserElt *elt)
{
  xmlNode *parent_node;

  parent_node = elt->getParentNode ();
  g_assert_nonnull (parent_node);
  if (toString (parent_node->name) != "region") // root region
    g_assert (st->getData ("saved_rect", (void **) &st->_rect));
  return true;
}

// <descriptorParam>
bool
ParserState::pushDescriptorParam (ParserState *st, ParserElt *elt)
{
  ParserElt *parent_elt;
  string name;
  string value;

  g_assert (elt->getAttribute ("name", &name));
  g_assert (elt->getAttribute ("value", &value));

  // Move parameters to parent descriptor (as attributes).
  g_assert (st->eltCacheIndexParent (elt->getNode (), &parent_elt));
  parent_elt->setAttribute (name, value);

  return true;
}

// <causalConnector>
static void
assessCleanup (void *ptr)
{
  delete (set<string> *) ptr;
}

static void
rolesCleanup (void *ptr)
{
  auto roles = (list<ParserConnRole> *) ptr;
  for (auto role: *roles)
    if (role.predicate != nullptr)
      delete role.predicate;
  delete roles;
}

bool
ParserState::pushCausalConnector (ParserState *st, ParserElt *elt)
{
  UDATA_SET (st, "conn_elt", elt, nullptr);
  UDATA_SET (elt, "tests", new set<string> (), assessCleanup);
  UDATA_SET (elt, "roles", new list<ParserConnRole> (), rolesCleanup);
  return true;
}

bool
ParserState::popCausalConnector (ParserState *st, ParserElt *elt)
{
  list<ParserConnRole> *roles;
  int nconds;
  int nacts;

  UDATA_SET (st, "conn_elt", nullptr, nullptr);
  UDATA_GET (elt, "roles", &roles);
  nconds = nacts = 0;

  for (auto role: *roles)
    {
      if (role.condition)
        nconds++;
      else
        nacts++;
    }

  if (unlikely (nconds == 0))
    return st->errEltMissingChild (elt->getNode (), {"simpleCondition"});
  if (unlikely (nacts == 0))
    return st->errEltMissingChild (elt->getNode (), {"simpleAction"});

  return true;
}

// <compoundCondition>
static void
predCleanup (void *ptr)
{
  delete (Predicate *) ptr;
}

bool
ParserState::pushCompoundCondition (ParserState *st, ParserElt *elt)
{
  Predicate *pred;
  ParserElt *parent_elt;
  string parent_tag;

  pred = new Predicate (Predicate::CONJUNCTION);
  UDATA_SET (elt, "pred", pred, predCleanup);

  // Inherit parent compound condition predicate.
  g_assert (st->eltCacheIndexParent (elt->getNode (), &parent_elt));
  parent_tag = parent_elt->getTag ();
  if (parent_tag == "compoundCondition")
    {
      Predicate *parent_pred;
      UDATA_GET (parent_elt, "pred", &parent_pred);
      parent_pred = parent_pred->clone ();
      pred->addChild (pred);
    }
  else if (parent_tag == "causalConnector")
    {
      // nothing to do
    }
  else
    {
      g_assert_not_reached ();
    }

  return true;
}

// <simpleCondition> or <simpleAction>
bool
ParserState::pushSimpleCondition (ParserState *st, ParserElt *elt)
{
  ParserConnRole role;
  string transition;
  xmlNode *node;
  bool condition;
  ParserElt *parent_elt;
  ParserElt *conn_elt;
  list<ParserConnRole> *roles;

  role.node = elt->getNode ();
  g_assert (elt->getAttribute ("role", &role.role));
  if (elt->getTag () == "simpleCondition")
    {
      role.condition = true;
      transition = "transition";
    }
  else
    {
      role.condition = false;
      transition = "actionType";
    }

  node = elt->getNode ();
  g_assert_nonnull (node);

  if (parser_syntax_reserved_role_table_index
      (role.role, &condition, &role.eventType, &role.transition))
    {
      string str;
      Event::Type type;
      Event::Transition trans;

      if (unlikely (role.condition != condition))
        {
          return st->errEltBadAttribute
            (node, "role", role.role,
             "reserved role '" + role.role + "' must be"
             + string ((condition) ? "a condition" : "an action"));
        }

      if (unlikely (elt->getAttribute ("eventType", &str)
                    && (!parser_syntax_event_type_table_index (str, &type)
                        || type != role.eventType)))
        {
          return st->errEltBadAttribute
            (node, "eventType", str,
             "reserved role '" + role.role + "' cannot be overwritten");
        }

      if (unlikely (elt->getAttribute (transition, &str)
                    && (!parser_syntax_transition_table_index (str, &trans)
                        || trans != role.transition)))
        {
          return st->errEltBadAttribute
            (node, transition, str,
             "reserved role '" + role.role + "' cannot be overwritten");
        }
    }
  else
    {
      string str;
      if (unlikely (!elt->getAttribute ("eventType", &str)))
        {
          return st->errEltMissingAttribute (node, "eventType");
        }
      if (unlikely (!parser_syntax_event_type_table_index
                    (str, &role.eventType)))
        {
          return st->errEltBadAttribute (node, "eventType", str);
        }
      if (unlikely (!elt->getAttribute (transition, &str)))
        {
          return st->errEltMissingAttribute (node, transition);
        }
      if (unlikely (!parser_syntax_transition_table_index
                    (str, &role.transition)))
        {
          return st->errEltBadAttribute (node, transition, str);
        }
    }

  if (unlikely
      (!role.condition
       && role.eventType == Event::ATTRIBUTION
       && !elt->getAttribute ("value", &role.value)))
    {
      return st->errEltMissingAttribute (node, "value");
    }

  if (role.eventType == Event::SELECTION)
    elt->getAttribute ("key", &role.key);

  // Set role predicate.
  g_assert (st->eltCacheIndexParent (node, &parent_elt));
  if (parent_elt->getTag () == "compoundCondition")
    {
      Predicate *pred;
      UDATA_GET (parent_elt, "pred", &pred);
      role.predicate = pred->clone ();
      g_assert_nonnull (role.predicate);
    }
  else
    {
      role.predicate = nullptr;
    }

  UDATA_GET (st, "conn_elt", &conn_elt);
  UDATA_GET (conn_elt, "roles", &roles);
  roles->push_back (role);

  return true;
}

// <compoundStatement>
bool
ParserState::pushCompoundStatement (ParserState *st, ParserElt *elt)
{
  string str;
  Predicate::Type type;
  Predicate *pred;
  ParserElt *parent_elt;
  Predicate *parent_pred;
  bool negated;

  g_assert (elt->getAttribute ("operator", &str));
  if (unlikely (!parser_syntax_connective_table_index (str, &type)))
    {
      return st->errEltBadAttribute (elt->getNode (), "operator", str);
    }

  negated = false;
  if (unlikely (elt->getAttribute ("isNegated", &str)
                && !parser_syntax_parse_bool (str, &negated)))
    {
      return st->errEltBadAttribute (elt->getNode (), "isNegated", str);
    }

  g_assert (st->eltCacheIndexParent (elt->getNode (), &parent_elt));
  UDATA_GET (parent_elt, "pred", &parent_pred);
  if (unlikely (parent_pred->getType () == Predicate::NEGATION
                && parent_pred->getChildren ()->size () == 1))
    {
      return st->errEltBadChild (parent_elt->getNode (), elt->getTag (),
                                 "too many children");
    }

  pred = new Predicate (type);
  if (negated)
    {
      Predicate *neg = new Predicate (Predicate::NEGATION);
      neg->addChild (pred);
      parent_pred->addChild (neg);
    }
  else
    {
      parent_pred->addChild (pred);
    }
  UDATA_SET (elt, "pred", pred, nullptr);

  return true;
}

bool
ParserState::popCompoundStatement (ParserState *st, ParserElt *elt)
{
  Predicate *pred;
  Predicate::Type type;

  UDATA_GET (elt, "pred", &pred);
  type = pred->getType ();

  g_assert (type != Predicate::VERUM);
  g_assert (type != Predicate::FALSUM);
  g_assert (type != Predicate::ATOM);

  if (unlikely
      ((type == Predicate::NEGATION
       && pred->getChildren ()->size () == 0))
      || ((type == Predicate::CONJUNCTION
          || type == Predicate::DISJUNCTION)
          && pred->getChildren ()->size () < 2))
    {
      return st->errEltMissingChild
        (elt->getNode (), {"compoundStatement", "assessmentStatement"});
    }

  return true;
}

// <assessmentStatement>
bool
ParserState::popAssessmentStatement (ParserState *st, ParserElt *elt)
{
  string comp;
  string *left;
  string *right;

  Predicate::Test test;
  Predicate *pred;

  ParserElt *parent_elt;
  Predicate *parent_pred;

  g_assert (elt->getAttribute ("comparator", &comp));
  if (unlikely (!parser_syntax_comparator_table_index (comp, &test)))
    {
      return st->errEltBadAttribute (elt->getNode (), "comparator", comp);
    }

  if (unlikely (!elt->getData ("left", (void **) &left)
                || !elt->getData ("right", (void **) &right)))
    {
      return st->errEltMissingChild
        (elt->getNode (), {"attributeAssessment", "valueAssessment"});
    }

  g_assert (st->eltCacheIndexParent (elt->getNode (), &parent_elt));
  UDATA_GET (parent_elt, "pred", &parent_pred);
  if (unlikely (parent_pred->getType () == Predicate::NEGATION
                && parent_pred->getChildren ()->size () == 1))
    {
      return st->errEltBadChild (parent_elt->getNode (), elt->getTag (),
                                 "too many children");
    }

  pred = new Predicate (Predicate::ATOM);
  pred->setTest (*left, test, *right);
  parent_pred->addChild (pred);

  return true;
}

// <attributeAssessment> or <valueAssessment>
static void
leftOrRightCleanup (void *ptr)
{
  delete (string *) ptr;
}

bool
ParserState::pushAttributeAssessment (ParserState *st, ParserElt *elt)
{
  string tag;
  string str;
  ParserElt *parent_elt;

  tag = elt->getTag ();
  if (tag == "attributeAssessment")
    {
      ParserElt *conn_elt;
      set<string> *tests;

      g_assert (elt->getAttribute ("role", &str));
      UDATA_GET (st, "conn_elt", &conn_elt);
      UDATA_GET (conn_elt, "tests", &tests);
      tests->insert (str);
      str = "$" + str;
    }
  else if (tag == "valueAssessment")
    {
      g_assert (elt->getAttribute ("value", &str));
    }
  else
    {
      g_assert_not_reached ();
    }

  g_assert (st->eltCacheIndexParent (elt->getNode (), &parent_elt));
  if (!parent_elt->getData ("left", nullptr))
    UDATA_SET (parent_elt, "left", new string (str), leftOrRightCleanup);
  else if (!parent_elt->getData ("right", nullptr))
    UDATA_SET (parent_elt, "right", new string (str), leftOrRightCleanup);
  else
    return st->errEltBadChild (parent_elt->getNode (), elt->getTag (),
                               "too many children");
  return true;
}

// <body> and <context>
static void
portsCleanup (void *ptr)
{
  delete (list<string> *) ptr;
}

bool
ParserState::pushContext (ParserState *st, ParserElt *elt)
{
  Object *ctx;
  string id;

  if (elt->getTag () == "body")
    {
      ctx = cast (Context *, st->objStackPeek ());
      g_assert_nonnull (ctx);

      if (elt->getAttribute ("id", &id))
        ctx->addAlias (id);
    }
  else
    {
      Composition *parent;

      parent = cast (Composition *, st->objStackPeek ());
      g_assert_nonnull (parent);

      g_assert (elt->getAttribute ("id", &id));
      ctx = new Context (id);
      parent->addChild (ctx);
    }

  // Create port list.
  UDATA_SET (elt, "ports", new list<string> (), portsCleanup);

  // Push context onto stack.
  st->objStackPush (ctx);

  return true;
}

bool
ParserState::popContext (ParserState *st, ParserElt *elt)
{
  Context *ctx;
  list<string> *ports;

  ctx = cast (Context *, st->objStackPeek ());
  g_assert_nonnull (ctx);

  // Resolve port references.
  UDATA_GET (elt, "ports", &ports);
  for (auto port_id: *ports)
    {
      ParserElt *port_elt;
      string comp;
      string iface;
      Object *obj;
      Event *evt;

      g_assert (st->eltCacheIndexById (port_id, &port_elt, {"port"}));
      g_assert (port_elt->getAttribute ("component", &comp));

      obj = ctx->getChildById (comp);
      if (unlikely (obj == nullptr))
        {
          return st->errEltBadAttribute (port_elt->getNode (), "component",
                                         comp, "no such object in scope");
        }

      if (!port_elt->getAttribute ("interface", &iface))
        iface = "@lambda";

      evt = obj->getEvent (Event::PRESENTATION, iface);
      if (evt == nullptr)
        {
          evt = obj->getEvent (Event::ATTRIBUTION, iface);
          if (unlikely (evt == nullptr))
            {
              return st->errEltBadAttribute (port_elt->getNode (),
                                             "interface", iface,
                                             "no such interface");
            }
        }

      ctx->addPort (evt);
    }

  st->objStackPop ();
  return true;
}

// <port>
bool
ParserState::pushPort (ParserState *st, ParserElt *elt)
{
  string id;
  ParserElt *parent_elt;
  list<string> *ports;

  g_assert (elt->getAttribute ("id", &id));
  g_assert (st->eltCacheIndexParent (elt->getNode (), &parent_elt));
  UDATA_GET (parent_elt, "ports", &ports);
  ports->push_back (id);

  return true;
}

// <media>
bool
ParserState::pushMedia (ParserState *st, ParserElt *elt)
{
  Composition *parent;
  Media *media;
  string id;
  string type;

  g_assert (elt->getAttribute ("id", &id));
  if (elt->getAttribute ("type", &type)
      && type == "application/x-ginga-settings")
    {
      media = st->_doc->getSettings ();
      g_assert_nonnull (media);
      media->addAlias (id);
    }
  else
    {
      string src = "";
      if (elt->getAttribute ("src", &src)
          && !xpathisuri (src) && !xpathisabs (src))
        {
          string dir;
          if (st->_xml->URL == nullptr)
            dir = "";
          else
            dir = xpathdirname (toString (st->_xml->URL));
          src = xpathbuildabs (dir, src);
        }
      media = new Media (id, type, src);
      g_assert_nonnull (media);
    }

  parent = cast (Composition *, st->objStackPeek ());
  g_assert_nonnull (parent);
  parent->addChild (media);

  st->objStackPush (media);
  return true;
}

bool
ParserState::popMedia (ParserState *st, unused (ParserElt *elt))
{
  g_assert (instanceof (Media *, st->objStackPop ()));
  return true;
}

// <area>
bool
ParserState::pushArea (ParserState *st, ParserElt *elt)
{
  Media *media;
  string id;
  string str;
  Time begin, end;

  media = cast (Media *, st->objStackPeek ());
  g_assert_nonnull (media);

  g_assert (elt->getAttribute ("id", &id));
  begin = elt->getAttribute ("begin", &str)
    ? ginga::parse_time (str) : 0;
  end = elt->getAttribute ("end", &str)
    ? ginga::parse_time (str) : GINGA_TIME_NONE;

  media->addPresentationEvent (id, begin, end);
  return true;
}

// <property>
bool
ParserState::pushProperty (ParserState *st, ParserElt *elt)
{
  Object *obj;
  string name;
  string value;

  obj = cast (Object *, st->objStackPeek ());
  g_assert_nonnull (obj);

  g_assert (elt->getAttribute ("name", &name));
  if (!elt->getAttribute ("value", &value))
    value = "";

  obj->addAttributionEvent (name);
  obj->setProperty (name, value);
  return true;
}

// <link>
static void
bindsCleanup (void *ptr)
{
  delete (list<ParserLinkBind> *) ptr;
}

static void
paramsCleanup (void *ptr)
{
  delete (map<string, string> *) ptr;
}

bool
ParserState::pushLink (unused (ParserState *st), ParserElt *elt)
{
  Context *ctx;

  ctx = cast (Context *, st->objStackPeek ());
  g_assert_nonnull (ctx);

  UDATA_SET (elt, "context", ctx, nullptr);
  UDATA_SET (elt, "binds", new list<ParserLinkBind> (), bindsCleanup);
  UDATA_SET (elt, "params", (new map<string, string>), paramsCleanup);

  return true;
}

// <linkParam> and <bindParam>
bool
ParserState::pushLinkParam (ParserState *st, ParserElt *elt)
{
  string name;
  string value;
  ParserElt *parent_elt;
  map<string, string> *params;

  g_assert (elt->getAttribute ("name", &name));
  g_assert (elt->getAttribute ("value", &value));
  g_assert (st->eltCacheIndexParent (elt->getNode (), &parent_elt));
  UDATA_GET (parent_elt, "params", &params);
  (*params)[name] = value;

  return true;
}

// <bind>
bool
ParserState::pushBind (ParserState *st, ParserElt *elt)
{
  ParserLinkBind bind;
  ParserElt *parent_elt;
  list<ParserLinkBind> *binds;

  bind.node = elt->getNode ();
  g_assert (elt->getAttribute ("role", &bind.role));
  g_assert (elt->getAttribute ("component", &bind.component));
  elt->getAttribute ("interface", &bind.iface);
  UDATA_SET (elt, "params", &bind.params, nullptr);

  g_assert (st->eltCacheIndexParent (elt->getNode (), &parent_elt));
  UDATA_GET (parent_elt, "binds", &binds);
  binds->push_back (bind);

  return true;
}


// External API.

static Document *
process (xmlDoc *xml, int width, int height, string *errmsg)
{
  ParserState st (width, height);
  Document *doc;

  doc = st.process (xml);
  if (unlikely (doc == nullptr))
    {
      g_assert (st.getError (errmsg) != ParserState::ERROR_NONE);
      return nullptr;
    }

  return doc;
}

Document *
Parser::parseBuffer (const void *buf, size_t size,
                     int width, int height, string *errmsg)
{
# define FLAGS (XML_PARSE_NOERROR | XML_PARSE_NOWARNING)
  xmlDoc *xml;
  Document *doc;

  xml = xmlReadMemory ((const char *) buf, (int) size,
                       nullptr, nullptr, FLAGS);
  if (unlikely (xml == nullptr))
    {
      xmlError *err = xmlGetLastError ();
      g_assert_nonnull (err);
      tryset (errmsg, "XML error: " + xstrstrip (string (err->message)));
      return nullptr;
    }

  doc = process (xml, width, height, errmsg);
  xmlFreeDoc (xml);
  return doc;
}

Document *
Parser::parseFile (const string &path, int width, int height,
                   string *errmsg)
{
  xmlDoc *xml;
  Document *doc;

  xml = xmlReadFile (path.c_str (), nullptr, FLAGS);
  if (unlikely (xml == nullptr))
    {
      xmlError *err = xmlGetLastError ();
      g_assert_nonnull (err);
      tryset (errmsg, "XML error: " + xstrstrip (string (err->message)));
      return nullptr;
    }

  doc = process (xml, width, height, errmsg);
  xmlFreeDoc (xml);
  return doc;
}

GINGA_NAMESPACE_END
