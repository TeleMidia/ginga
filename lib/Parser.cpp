/* Copyright (C) 2006-2018 PUC-Rio/Laboratorio TeleMidia

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
along with Ginga.  If not, see <https://www.gnu.org/licenses/>.  */

#include "aux-ginga.h"
#include "Parser.h"

#include "Context.h"
#include "Document.h"
#include "Media.h"
#include "MediaSettings.h"
#include "Switch.h"

#include <libxml/tree.h>
#include <libxml/parser.h>
#include <fontconfig/fontconfig.h>

GINGA_NAMESPACE_BEGIN

// XML helper macros and functions.
#define toCString(s) deconst (char *, (s))
#define toXmlChar(s) (xmlChar *) (deconst (char *, (s).c_str ()))
#define toCPPString(s) string (deconst (char *, (s)))

/// Flags to LibXML parser.
#define PARSER_LIBXML_FLAGS (XML_PARSE_NOERROR | XML_PARSE_NOWARNING)

/// Gets last XML error as C++ string.
static inline string
xmlGetLastErrorAsString ()
{
  xmlError *err;
  string errmsg;

  err = xmlGetLastError ();
  g_assert_nonnull (err);

  errmsg = (err->file != nullptr) ? toCPPString (err->file) + ": " : "";
  errmsg += "XML error";
  if (err->line > 0)
    errmsg += xstrbuild (" at line %d", err->line);
  errmsg += ": " + xstrstrip (string (err->message));

  return errmsg;
}

/// Gets node property as C++ string.
static inline bool
xmlGetPropAsString (xmlNode *node, const string &name, string *result)
{
  xmlChar *str = xmlGetProp (node, toXmlChar (name));
  if (str == nullptr)
    return false;
  tryset (result, toCPPString (str));
  g_free (str);
  return true;
}

/// Finds node children by tag.
static list<xmlNode *>
xmlFindAllChildren (xmlNode *node, const string &tag)
{
  list<xmlNode *> children;
  for (xmlNode *child = node->children; child; child = child->next)
    {
      if (child->type == XML_ELEMENT_NODE
          && toCPPString (child->name) == tag)
        {
          children.push_back (child);
        }
    }
  return children;
}

/// Tests whether value is a valid XML name.
static bool
xmlIsValidName (const string &value, char *offending)
{
  const char *str;
  char c;

  str = value.c_str ();
  while ((c = *str++) != '\0')
    {
      if (!(g_ascii_isalnum (c) || c == '-' || c == '_' || c == ':'
            || c == '.' || c == '#'))
        {
          tryset (offending, c);
          return false;
        }
    }
  return true;
}

// Parser internal types.

// Forward declarations.
typedef struct ParserSyntaxAttr ParserSyntaxAttr;
typedef struct ParserSyntaxElt ParserSyntaxElt;

/**
 * @brief Parser element wrapper.
 *
 * Data associated with a specific \c xmlNode.
 */
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
  bool setData (const string &, void *, UserDataCleanFunc fn = nullptr);

private:
  string _tag;                ///< Element tag.
  xmlNode *_node;             ///< Corresponding node in document tree.
  map<string, string> _attrs; ///< Element attributes.
  UserData _udata;            ///< Attached user data.
};

/**
 * @brief Connector role data.
 *
 * Data associated with a \<simpleCondition\> element or \<simpleAction\>
 * element occurring in the NCL document.
 */
typedef struct ParserConnRole
{
  xmlNode *node;                ///< Source node.
  string role;                  ///< Role label.
  Event::Type eventType;        ///< Role event type.
  Event::Transition transition; ///< Role transition.
  bool condition;               ///< Whether role is a condition.
  Predicate *predicate;         ///< Role predicate (if condition).
  string duration;              ///< Role duration (if action).
  string delay;                 ///< Role delay.
  string key;                   ///< Role key (if selection).
  string value;                 ///< Role value (if attribution).
} ParserConnRole;

/**
 * @brief Link bind data.
 *
 * Data associated with a \<bind\> element occurring in the NCL document.
 */
typedef struct ParserLinkBind
{
  xmlNode *node;              ///< Source node.
  string role;                ///< Bind role label.
  string component;           ///< Bind component.
  string iface;               ///< Bind interface.
  map<string, string> params; ///< Bind parameters.
} ParserLinkBind;

/**
 * @brief Parser state.
 *
 * Parser state while it's parsing the document.
 */
class ParserState
{
public:
  /// Parser error codes.
  enum Error
  {
    ERROR_NONE = 0,                     ///< No error.
    ERROR_ELT_UNKNOWN,                  ///< Unknown element.
    ERROR_ELT_MISSING_PARENT,           ///< Missing parent element.
    ERROR_ELT_BAD_PARENT,               ///< Bad parent element.
    ERROR_ELT_UNKNOWN_ATTRIBUTE,        ///< Unknown attribute.
    ERROR_ELT_MISSING_ATTRIBUTE,        ///< Missing attribute.
    ERROR_ELT_BAD_ATTRIBUTE,            ///< Bad attribute.
    ERROR_ELT_MUTUALLY_EXCL_ATTRIBUTES, ///< Mutually exclusive attributes.
    ERROR_ELT_UNKNOWN_CHILD,            ///< Unknown child element.
    ERROR_ELT_MISSING_CHILD,            ///< Missing child element.
    ERROR_ELT_BAD_CHILD,                ///< Bad child element.
    ERROR_ELT_IMPORT,                   ///< Error in imported document.
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
  static bool pushRule (ParserState *, ParserElt *);
  static bool pushImportBase (ParserState *, ParserElt *);
  static bool pushContext (ParserState *, ParserElt *);
  static bool popContext (ParserState *, ParserElt *);
  static bool pushPort (ParserState *, ParserElt *);
  static bool pushSwitch (ParserState *, ParserElt *);
  static bool popSwitch (ParserState *, ParserElt *);
  static bool pushBindRule (ParserState *, ParserElt *);
  static bool pushMedia (ParserState *, ParserElt *);
  static bool popMedia (ParserState *, ParserElt *);
  static bool pushArea (ParserState *, ParserElt *);
  static bool pushProperty (ParserState *, ParserElt *);
  static bool pushLink (ParserState *, ParserElt *);
  static bool pushLinkParam (ParserState *, ParserElt *);
  static bool pushBind (ParserState *, ParserElt *);

  static bool pushFont (ParserState *, ParserElt *);

private:
  Document *_doc;      ///< The resulting #Document.
  xmlDoc *_xml;        ///< The DOM tree being processed.
  int _genid;          ///< Last generated id.
  UserData _udata;     ///< Attached user data.
  set<string> _unique; ///< Unique attributes seen so far.

  ParserState::Error _error; ///< Last error code.
  string _errorMsg;          ///< Last error message.

  map<xmlNode *, ParserElt *> _eltCache;          ///< Element cache.
  map<string, list<ParserElt *> > _eltCacheByTag; ///< Element cache by tag.

  /// Alias stack for solving imports.
  list<pair<string, string> > _aliasStack;

  ///< #Object stack for solving object hierarchy.
  list<Object *> _objStack;

  ///< Rectangle stack for solving region hierarchy.
  list<Rect> _rectStack;

  ///< Reference map for solving the refer attribute in \<media\>.
  map<string, Media *> _referMap;

  string genId ();
  string getDirname ();
  bool isInUniqueSet (const string &);
  void addToUniqueSet (const string &);
  bool getData (const string &, void **);
  bool setData (const string &, void *, UserDataCleanFunc fn = nullptr);

  // Errors.
  bool errElt (xmlNode *, ParserState::Error, const string &);
  bool errEltUnknown (xmlNode *);
  bool errEltMissingParent (xmlNode *);
  bool errEltBadParent (xmlNode *);
  bool errEltUnknownAttribute (xmlNode *, const string &);
  bool errEltMissingAttribute (xmlNode *, const string &);
  bool errEltBadAttribute (xmlNode *, const string &, const string &,
                           const string &explain = "");
  bool errEltMutuallyExclAttributes (xmlNode *, const string &,
                                     const string &);
  bool errEltUnknownChild (xmlNode *, const string &);
  bool errEltMissingChild (xmlNode *, const list<string> &);
  bool errEltBadChild (xmlNode *, const string &,
                       const string &explain = "");
  bool errEltImport (xmlNode *, const string &explain = "");

  // Element cache.
  bool eltCacheIndex (xmlNode *, ParserElt **);
  bool eltCacheIndexParent (xmlNode *, ParserElt **);
  bool eltCacheIndexById (const string &, ParserElt **,
                          const list<string> &);
  size_t eltCacheIndexByTag (const list<string> &, list<ParserElt *> *);
  bool eltCacheAdd (ParserElt *);

  // Alias stack.
  string aliasStackCombine ();
  bool aliasStackPeek (string *, string *);
  bool aliasStackPop (string *, string *);
  bool aliasStackPush (const string &, const string &);

  // Object stack.
  Object *objStackPeek ();
  Object *objStackPop ();
  void objStackPush (Object *);

  // Rectangle stack.
  Rect rectStackPeek ();
  Rect rectStackPop ();
  void rectStackPush (Rect);

  // Reference map.
  bool referMapIndex (const string &, Media **);
  bool referMapAdd (const string &, Media *);

  // Reference solving.
  bool resolveComponent (Composition *, ParserElt *, Object **);
  bool resolveInterface (Context *, ParserElt *, Event **);
  string resolveParameter (const string &, const map<string, string> *,
                           const map<string, string> *,
                           const map<string, string> *);
  // Predicate solving.
  Predicate *obtainPredicate (xmlNode *);
  Predicate *solvePredicate (Predicate *, const map<string, string> *);

  // Node processing.
  ParserSyntaxElt *checkNode (xmlNode *, map<string, string> *,
                              list<xmlNode *> *);
  bool processNode (xmlNode *);
};

/// Asserted version of UserData::getData().
#define UDATA_GET(obj, key, ptr)                                           \
  G_STMT_START                                                             \
  {                                                                        \
    *(ptr) = nullptr;                                                      \
    g_assert ((obj)->getData ((key), (void **) (ptr)));                    \
    g_assert_nonnull (*(ptr));                                             \
  }                                                                        \
  G_STMT_END

/// Asserted version of UserData::setData().
#define UDATA_SET(obj, key, ptr, fn)                                       \
  G_STMT_START                                                             \
  {                                                                        \
    (obj)->setData ((key), (void *) (ptr), (fn));                          \
  }                                                                        \
  G_STMT_END

// NCL syntax.

/// Type for element push function.
typedef bool(ParserSyntaxEltPush) (ParserState *, ParserElt *);

/// Type for element pop function.
typedef bool(ParserSyntaxEltPop) (ParserState *, ParserElt *);

/**
 * @brief NCL attribute syntax.
 */
typedef struct ParserSyntaxAttr
{
  string name; ///< Attribute name.
  int flags;   ///< Processing flags.
} ParserSyntaxAttr;

/**
 * @brief NCL element syntax.
 */
typedef struct ParserSyntaxElt
{
  ParserSyntaxEltPush *push;         ///< Push function.
  ParserSyntaxEltPop *pop;           ///< Pop function.
  int flags;                         ///< Processing flags.
  list<string> parents;              ///< Possible parents.
  list<ParserSyntaxAttr> attributes; ///< Attributes.
} ParserSyntaxElt;

/**
 * @brief NCL attribute processing flags.
 */
typedef enum {
  PARSER_SYNTAX_ATTR_NONEMPTY = 1 << 1,   ///< Cannot be not empty.
  PARSER_SYNTAX_ATTR_REQUIRED = 1 << 2,   ///< Is required.
  PARSER_SYNTAX_ATTR_TYPE_ID = 1 << 3,    ///< Is an id.
  PARSER_SYNTAX_ATTR_TYPE_IDREF = 1 << 4, ///< Is an id-ref.
  PARSER_SYNTAX_ATTR_TYPE_NAME = 1 << 5,  ///< Is a name.
  PARSER_SYNTAX_ATTR_UNIQUE = 1 << 6,     ///< Must be unique in document.
} ParserSyntaxAttrFlag;

#define ATTR_NONEMPTY (PARSER_SYNTAX_ATTR_NONEMPTY)
#define ATTR_REQUIRED (PARSER_SYNTAX_ATTR_REQUIRED)
#define ATTR_TYPE_ID (PARSER_SYNTAX_ATTR_TYPE_ID)
#define ATTR_TYPE_IDREF (PARSER_SYNTAX_ATTR_TYPE_IDREF)
#define ATTR_TYPE_NAME (PARSER_SYNTAX_ATTR_TYPE_NAME)
#define ATTR_UNIQUE (PARSER_SYNTAX_ATTR_UNIQUE)

#define ATTR_NONEMPTY_NAME (ATTR_NONEMPTY | ATTR_TYPE_NAME)
#define ATTR_REQUIRED_NONEMPTY_NAME (ATTR_REQUIRED | ATTR_NONEMPTY_NAME)

#define ATTR_ID (ATTR_UNIQUE | ATTR_REQUIRED_NONEMPTY_NAME | ATTR_TYPE_ID)
#define ATTR_OPT_ID (ATTR_UNIQUE | ATTR_NONEMPTY_NAME | ATTR_TYPE_ID)

#define ATTR_IDREF (ATTR_REQUIRED_NONEMPTY_NAME | ATTR_TYPE_IDREF)
#define ATTR_OPT_IDREF (ATTR_NONEMPTY_NAME | ATTR_TYPE_IDREF)

/**
 * @brief NCL element processing flags.
 */
typedef enum {
  PARSER_SYNTAX_ELT_CACHE = 1 << 1,  ///< Save element in #Parser cache.
  PARSER_SYNTAX_ELT_GEN_ID = 1 << 2, ///< Generate id if not present.
} ParserSyntaxEltFlag;

#define ELT_CACHE (PARSER_SYNTAX_ELT_CACHE)
#define ELT_GEN_ID (PARSER_SYNTAX_ELT_GEN_ID)

// Defines table index function.
#define PARSER_SYNTAX_TABLE_INDEX_DEFN(tabname, Type)                      \
  static bool parser_syntax_##tabname##_table_index (const string &str,    \
                                                     Type *result)         \
  {                                                                        \
    auto it = parser_syntax_##tabname##_table.find (str);                  \
    if (it == parser_syntax_##tabname##_table.end ())                      \
      return false;                                                        \
    tryset (result, it->second);                                           \
    return true;                                                           \
  }

/// NCL syntax table (grammar).
static map<string, ParserSyntaxElt> parser_syntax_table = {
  {
      "ncl",                     // element name
      {ParserState::pushNcl,    // push function
        ParserState::popNcl,     // pop function
        0,                       // flags
        {},                      // possible parents
        { {"id", ATTR_OPT_ID}, // attributes
          {"title", 0},
          {"schemaLocation", 0},
          {"xmlns", 0} } },
  },
  {
      "head",
      {nullptr, nullptr, 0, {"ncl"}, {} },
  },
  {
      "regionBase",
      {nullptr,
        nullptr,
        ELT_CACHE,
        {"head"},
        { {"id", ATTR_OPT_ID}, // unused
          {"device", 0},       // unused
          {"region", 0} } },   // unused
  },
  {
      "region",
      {ParserState::pushRegion,
        ParserState::popRegion,
        ELT_CACHE,
        {"region", "regionBase"},
        { {"id", ATTR_ID},
          {"left", 0},
          {"right", 0},
          {"top", 0},
          {"bottom", 0},
          {"height", 0},
          {"width", 0},
          {"zIndex", 0},
          {"title", 0} } }, // unused
  },
  {
      "descriptorBase",
      {nullptr,
        nullptr,
        ELT_CACHE,
        {"head"},
        { {"id", ATTR_OPT_ID} } }, // unused
  },
  {
      "descriptor",
      {nullptr,
        nullptr,
        ELT_CACHE,
        {"descriptorBase"},
        { {"id", ATTR_ID},
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
          {"player", 0}, // unused
          {"transIn", 0},
          {"transOut", 0} } },
  },
  {
      "descriptorParam",
      {ParserState::pushDescriptorParam,
        nullptr,
        0,
        {"descriptor"},
        { {"name", ATTR_REQUIRED_NONEMPTY_NAME},
          {"value", ATTR_REQUIRED} } },
  },
  {
      "connectorBase",
      {nullptr,
        nullptr,
        ELT_CACHE,
        {"head"},
        { {"id", ATTR_OPT_ID} } }, // unused
  },
  {
      "causalConnector",
      {ParserState::pushCausalConnector,
        ParserState::popCausalConnector,
        ELT_CACHE,
        {"connectorBase"},
        { {"id", ATTR_ID} } },
  },
  {
      "connectorParam",
      {nullptr,
        nullptr,
        0,
        {"causalConnector"},
        { {"name", ATTR_NONEMPTY_NAME} } }, // unused
  },
  {
      "compoundCondition",
      {ParserState::pushCompoundCondition,
        nullptr,
        ELT_CACHE,
        {"causalConnector", "compoundCondition"},
        { {"operator", 0},  // unused
          {"delay", 0} } }, // unused
  },
  {
      "simpleCondition",
      {ParserState::pushSimpleCondition,
        nullptr,
        0,
        {"causalConnector", "compoundCondition"},
        { {"role", ATTR_REQUIRED_NONEMPTY_NAME},
          {"eventType", 0},
          {"key", 0},
          {"transition", 0},
          {"delay", 0},         // unused
          {"min", 0},           // unused
          {"max", 0},           // unused
          {"qualifier", 0} } }, // unused
  },
  {
      "compoundAction",
      {nullptr,
        nullptr,
        0,
        {"causalConnector", "compoundAction"},
        { {"operator", 0},  // unused
          {"delay", 0} } }, // unused
  },
  {
      "simpleAction",
      {ParserState::pushSimpleCondition, // reused
        nullptr,
        0,
        {"causalConnector", "compoundAction"},
        { {"role", ATTR_REQUIRED_NONEMPTY_NAME},
          {"eventType", 0},
          {"actionType", 0},
          {"duration", 0},
          {"value", 0},
          {"delay", 0},       // unused
          {"min", 0},         // unused
          {"max", 0},         // unused
          {"qualifier", 0},   // unused
          {"repeat", 0},      // unused
          {"repeatDelay", 0}, // unused
          {"by", 0} } },      // unused
  },
  {
      "compoundStatement",
      {ParserState::pushCompoundStatement,
        ParserState::popCompoundStatement,
        ELT_CACHE,
        {"compoundCondition", "compoundStatement"},
        { {"operator", ATTR_REQUIRED}, {"isNegated", 0} } },
  },
  {
      "assessmentStatement",
      {nullptr,
        ParserState::popAssessmentStatement,
        ELT_CACHE,
        {"compoundCondition", "compoundStatement"},
        { {"comparator", ATTR_REQUIRED} } },
  },
  {
      "attributeAssessment",
      {ParserState::pushAttributeAssessment,
        nullptr,
        0,
        {"assessmentStatement"},
        { {"role", ATTR_REQUIRED_NONEMPTY_NAME},
          {"eventType", 0},     // unused
          {"key", 0},           // unused
          {"attributeType", 0}, // unused
          {"offset", 0} } },    // unused
  },
  {
      "valueAssessment",
      {ParserState::pushAttributeAssessment, // reused
        nullptr,
        0,
        {"assessmentStatement"},
        { {"value", ATTR_REQUIRED} } },
  },
  {
      "ruleBase",
      {nullptr,
        nullptr,
        ELT_CACHE,
        {"head"},
        { {"id", ATTR_OPT_ID} } }, // unused
  },
  {
      "compositeRule",
      {ParserState::pushRule,
        nullptr,
        ELT_CACHE,
        {"ruleBase", "compositeRule"},
        { {"id", ATTR_ID}, {"operator", ATTR_REQUIRED} } },
  },
  {
      "rule",
      {ParserState::pushRule,
        nullptr,
        ELT_CACHE,
        {"ruleBase", "compositeRule"},
        { {"id", ATTR_ID},
          {"var", ATTR_REQUIRED_NONEMPTY_NAME},
          {"comparator", ATTR_REQUIRED},
          {"value", ATTR_REQUIRED} } },
  },
  {
      "transitionBase",
      {nullptr,
        nullptr,
        ELT_CACHE,
        {"head"},
        { {"id", ATTR_OPT_ID} } },
  },
  {
      "transition",
      {nullptr,
        nullptr,
        ELT_CACHE,
        {"transitionBase"},
        { {"id", ATTR_ID},
          {"type", ATTR_REQUIRED_NONEMPTY_NAME},
          {"subtype", ATTR_NONEMPTY_NAME},
          {"dur", 0},
          {"startProgress", 0},
          {"endProgress", 0},
          {"direction", 0},
          {"fadeColor", 0},
          {"horzRepeat", 0},
          {"vertRepeat", 0},
          {"borderWidth", 0},
          {"borderColor", 0} } },
  },
  {
      "importBase",
      {ParserState::pushImportBase,
        nullptr,
        ELT_CACHE,
        {"connectorBase", "descriptorBase", "regionBase", "ruleBase",
          "transitionBase", "fontBase"},
        { {"alias", ATTR_REQUIRED_NONEMPTY_NAME},
          {"documentURI", ATTR_REQUIRED},
          {"region", 0},     // unused
          {"baseId", 0} } }, // unused
  },
  {
    "fontBase",
    {
      nullptr,
      nullptr,
      ELT_CACHE,
      {"head"},
      {} // no attributes
    }
  },
  {
    "font",
    {ParserState::pushFont,
      nullptr,
      ELT_CACHE,
      {"fontBase"},
      { {"family", ATTR_REQUIRED},
        {"src", ATTR_REQUIRED} } },
  },
  {
      "body",
      {ParserState::pushContext, // reused
        ParserState::popContext,  // reused
        ELT_CACHE,
        {"ncl"},
        { {"id", ATTR_OPT_ID} } },
  },
  {
      "context",
      {ParserState::pushContext,
        ParserState::popContext,
        ELT_CACHE,
        {"body", "context", "switch"},
        { {"id", ATTR_ID}, {"refer", ATTR_OPT_IDREF} } }, // unused
  },
  {
      "port",
      {ParserState::pushPort,
        nullptr,
        ELT_CACHE,
        {"body", "context"},
        { {"id", ATTR_ID},
          {"component", ATTR_IDREF},
          {"interface", ATTR_OPT_IDREF} } },
  },
  {
      "switch",
      {ParserState::pushSwitch,
        ParserState::popSwitch,
        ELT_CACHE,
        {"body", "context", "switch"},
        { {"id", ATTR_ID}, {"refer", ATTR_OPT_IDREF} } },
  },
  {
      "bindRule",
      {ParserState::pushBindRule,
        nullptr,
        ELT_CACHE,
        {"switch"},
        { {"constituent", ATTR_IDREF}, {"rule", ATTR_IDREF} } },
  },
  {
      "defaultComponent",
      {ParserState::pushBindRule,
        nullptr,
        ELT_CACHE,
        {"switch"},
        { {"component", ATTR_IDREF} } },
  },
  {
      "media",
      {ParserState::pushMedia,
        ParserState::popMedia,
        ELT_CACHE,
        {"body", "context", "switch"},
        { {"id", ATTR_ID},
          {"src", 0},
          {"type", 0},
          {"descriptor", ATTR_OPT_IDREF},
          {"refer", ATTR_OPT_IDREF},
          {"instance", 0} } }, // unused
  },
  {
      "area",
      {ParserState::pushArea,
        nullptr,
        0,
        {"media"},
        { {"id", ATTR_ID},
          {"begin", 0},
          {"end", 0},
          {"label", 0} } }, // unused
  },
  {
      "property",
      {ParserState::pushProperty,
        nullptr,
        0,
        {"body", "context", "media"},
        { {"name", ATTR_REQUIRED_NONEMPTY_NAME}, {"value", 0} } },
  },
  {
      "link",
      {ParserState::pushLink,
        nullptr,
        ELT_CACHE | ELT_GEN_ID,
        {"body", "context"},
        { {"id", ATTR_OPT_ID}, {"xconnector", ATTR_IDREF} } },
  },
  {
      "linkParam",
      {ParserState::pushLinkParam,
        nullptr,
        0,
        {"link"},
        { {"name", ATTR_REQUIRED_NONEMPTY_NAME},
          {"value", ATTR_REQUIRED} } },
  },
  {
      "bind",
      {ParserState::pushBind,
        nullptr,
        ELT_CACHE,
        {"link"},
        { {"role", ATTR_REQUIRED_NONEMPTY_NAME},
          {"component", ATTR_IDREF},
          {"interface", ATTR_OPT_IDREF} } },
  },
  {
      "bindParam",
      {ParserState::pushLinkParam, // reused
        nullptr,
        0,
        {"bind"},
        { {"name", ATTR_REQUIRED_NONEMPTY_NAME},
          {"value", ATTR_REQUIRED} } },
  },
};

/// Indexes syntax table.
static bool
parser_syntax_table_index (const string &tag, ParserSyntaxElt **result)
{
  auto it = parser_syntax_table.find (tag);
  if (it == parser_syntax_table.end ())
    return false;
  tryset (result, &it->second);
  return true;
}

/// Gets possible children of a given element.
static map<string, bool>
parser_syntax_table_get_possible_children (const string &tag)
{
  map<string, bool> result;
  for (auto it : parser_syntax_table)
    for (auto parent : it.second.parents)
      if (parent == tag)
        result[it.first] = true;
  return result;
}

/// Reserved connector roles.
static map<string, pair<Event::Type, Event::Transition> >
    parser_syntax_reserved_role_table = {
      {"onBegin", {Event::PRESENTATION, Event::START} }, // conditions
      {"onEnd", {Event::PRESENTATION, Event::STOP} },
      {"onAbort", {Event::PRESENTATION, Event::ABORT} },
      {"onPause", {Event::PRESENTATION, Event::PAUSE} },
      {"onResumes", {Event::PRESENTATION, Event::RESUME} },
      {"onBeginAttribution", {Event::ATTRIBUTION, Event::START} },
      {"onEndAttribution", {Event::ATTRIBUTION, Event::STOP} },
      {"onSelection", {Event::SELECTION, Event::START} },
      {"onBeginSelection", {Event::SELECTION, Event::START} },
      {"onEndSelection", {Event::SELECTION, Event::STOP} },
      {"start", {Event::PRESENTATION, Event::START} }, // actions
      {"stop", {Event::PRESENTATION, Event::STOP} },
      {"abort", {Event::PRESENTATION, Event::ABORT} },
      {"pause", {Event::PRESENTATION, Event::PAUSE} },
      {"resume", {Event::PRESENTATION, Event::RESUME} },
      {"set", {Event::ATTRIBUTION, Event::START} },
    };

/// Index reserved role table.
static bool
parser_syntax_reserved_role_table_index (const string &role,
                                         bool *condition, Event::Type *type,
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

/// Known event types.
static map<string, Event::Type> parser_syntax_event_type_table = {
  {"presentation", Event::PRESENTATION},
  {"attribution", Event::ATTRIBUTION},
  {"selection", Event::SELECTION},
};

/// Known transitions.
static map<string, Event::Transition> parser_syntax_transition_table = {
  {"start", Event::START},   {"starts", Event::START},
  {"pause", Event::PAUSE},   {"pauses", Event::PAUSE},
  {"resume", Event::RESUME}, {"resumes", Event::RESUME},
  {"stop", Event::STOP},     {"stops", Event::STOP},
  {"abort", Event::ABORT},   {"aborts", Event::ABORT},
};

/// Known logical connectives.
static map<string, Predicate::Type> parser_syntax_connective_table = {
  {"not", Predicate::NEGATION},
  {"and", Predicate::CONJUNCTION},
  {"or", Predicate::DISJUNCTION},
};

/// Known string comparators.
static map<string, Predicate::Test> parser_syntax_comparator_table = {
  {"eq", Predicate::EQ},  // ==
  {"ne", Predicate::NE},  // !=
  {"lt", Predicate::LT},  // <
  {"lte", Predicate::LE}, // <=
  {"gt", Predicate::GT},  // >
  {"gte", Predicate::GE}, // >=
};

// Index functions.
PARSER_SYNTAX_TABLE_INDEX_DEFN (event_type, Event::Type);
PARSER_SYNTAX_TABLE_INDEX_DEFN (transition, Event::Transition);
PARSER_SYNTAX_TABLE_INDEX_DEFN (connective, Predicate::Type);
PARSER_SYNTAX_TABLE_INDEX_DEFN (comparator, Predicate::Test);

/// Parses boolean.
static bool
parser_syntax_parse_bool (const string &str, bool *result)
{
  return ginga::try_parse_bool (str, result);
}

// ParserElt: public.

/**
 * @brief Creates a new element.
 * @param node The node to wrap.
 * @return New #ParserElt.
 */
ParserElt::ParserElt (xmlNode *node)
{
  g_assert_nonnull (node);
  _node = node;
  _tag = toCPPString (node->name);
}

/**
 * @brief Destroys element.
 */
ParserElt::~ParserElt ()
{
}

/**
 * @brief Gets element tag.
 * @return Tag.
 */
string
ParserElt::getTag ()
{
  return _tag;
}

/**
 * @brief Gets element node.
 * @return Associated node.
 */
xmlNode *
ParserElt::getNode ()
{
  return _node;
}

/**
 * @brief Gets element parent node.
 * @return Parent of associated node.
 */
xmlNode *
ParserElt::getParentNode ()
{
  return _node->parent;
}

/**
 * @brief Gets element attribute map.
 * @return Attribute map.
 */
const map<string, string> *
ParserElt::getAttributes ()
{
  return &_attrs;
}

/**
 * @brief Gets element attribute value
 * @param name Attribute name.
 * @param[out] value Variable to store the attribute value (if any).
 * @return \c true if successful, or false otherwise.
 */
bool
ParserElt::getAttribute (const string &name, string *value)
{
  auto it = _attrs.find (name);
  if (it == _attrs.end ())
    return false;
  tryset (value, it->second);
  return true;
}

/**
 * @brief Sets element attribute value.
 * @param name Attribute name.
 * @param value Attribute value.
 * @return \c true if the attribute was previously unset, or \c false
 * otherwise.
 */
bool
ParserElt::setAttribute (const string &name, const string &value)
{
  bool result = !this->getAttribute (name, nullptr);
  _attrs[name] = value;
  return result;
}

/**
 * @brief Gets user data attached to element.
 * @param key User data key.
 * @param[out] value Variable to store the user data value (if any).
 * @return \c true if successful, or \c false otherwise.
 */
bool
ParserElt::getData (const string &key, void **value)
{
  return _udata.getData (key, value);
}

/**
 * @brief Attaches user data to element.
 * @param key User data key.
 * @param value User data value.
 * @param fn Cleanup function (used to destroy data when #ParserElt is
 * destroyed or key is overwritten).
 * @return \c true if key was previously unset, or \c false otherwise.
 */
bool
ParserElt::setData (const string &key, void *value, UserDataCleanFunc fn)
{
  return _udata.setData (key, value, fn);
}

// ParserState: private.

/**
 * @brief Generates an unique id.
 * @return Unique id.
 */
string
ParserState::genId ()
{
  string id = xstrbuild ("__unamed-%d__", _genid++);
  g_assert_false (isInUniqueSet (id));
  return id;
}

/**
 * @brief Gets the directory part of current loaded XML file.
 * @return Directory part if there is a loaded XML file, or the empty string
 * otherwise.
 */
string
ParserState::getDirname ()
{
  return (_xml != nullptr && _xml->URL != nullptr)
             ? xpathdirname (toCPPString (_xml->URL))
             : "";
}

/**
 * @brief Tests whether id is unique (hasn't been seen yet).
 * @param id The id to test.
 * @return \c true if successful, or \c false otherwise.
 */
bool
ParserState::isInUniqueSet (const string &id)
{
  return _unique.find (id) != _unique.end ();
}

/**
 * @brief Adds id to the set of known ids.
 * @param id The id to add.
 */
void
ParserState::addToUniqueSet (const string &id)
{
  _unique.insert (id);
}

/**
 * @brief Gets user data attached to parser state.
 * @param key User data key.
 * @param[out] value Variable to store user data value (if any).
 * @return \c true if successful, or \c false otherwise.
 */
bool
ParserState::getData (const string &key, void **value)
{
  return _udata.getData (key, value);
}

/**
 * @brief Attaches user data to parser state.
 * @param key User data key.
 * @param value User data value.
 * @param fn Cleanup function (used to destroy data when #ParserElt is
 * destroyed or key is overwritten).
 * @return \c true if key was previously unset, or \c false otherwise.
 */
bool
ParserState::setData (const string &key, void *value, UserDataCleanFunc fn)
{
  return _udata.setData (key, value, fn);
}

// ParserState: private (error handling).

/**
 * @brief Sets parser error code and error message.
 * @param node The node that caused the error.
 * @param error The error code to be set.
 * @param message The error message to be set.
 * @return \c false.
 */
bool
ParserState::errElt (xmlNode *node, ParserState::Error error,
                     const string &message)
{
  g_assert (error != ParserState::ERROR_NONE);
  g_assert_nonnull (node);

  _error = error;
  _errorMsg = "";
  if (node->doc->URL != nullptr)
    {
      string path = toCPPString (node->doc->URL);
      if (!xpathisabs (path))
        path = xpathbuildabs (this->getDirname (), path);
      _errorMsg = path + ": ";
    }
  _errorMsg
      += xstrbuild ("Element <%s> at line %d: ", toCString (node->name),
                    node->line)
         + message;
  return false;
}

/**
 * @brief Sets parser error to "Unknown element".
 * @param node The node that caused the error.
 * @return \c false.
 */
bool
ParserState::errEltUnknown (xmlNode *node)
{
  return this->errElt (node, ParserState::ERROR_ELT_UNKNOWN,
                       "Unknown element");
}

/**
 * @brief Sets parser error to "Missing parent".
 * @param node The node that caused the error.
 * @return \c false.
 */
bool
ParserState::errEltMissingParent (xmlNode *node)
{
  return this->errElt (node, ParserState::ERROR_ELT_MISSING_PARENT,
                       "Missing parent");
}

/**
 * @brief Sets parser error to "Bad parent".
 * @param node The node that caused the error.
 * @return \c false.
 */
bool
ParserState::errEltBadParent (xmlNode *node)
{
  string parent;
  g_assert_nonnull (node->parent);
  parent = toCPPString (node->parent->name);
  return this->errElt (node, ParserState::ERROR_ELT_BAD_PARENT,
                       "Bad parent <" + parent + ">");
}

/**
 * @brief Sets parser error to "Unknown attribute".
 * @param node The node that caused the error.
 * @param name The name of the attribute that caused the error.
 * @return \c false.
 */
bool
ParserState::errEltUnknownAttribute (xmlNode *node, const string &name)
{
  return this->errElt (node, ParserState::ERROR_ELT_UNKNOWN_ATTRIBUTE,
                       "Unknown attribute '" + name + "'");
}

/**
 * @brief Sets parser error to "Missing attribute".
 * @param node The node that caused the error.
 * @param name The name of the attribute that caused the error.
 * @return \c false.
 */
bool
ParserState::errEltMissingAttribute (xmlNode *node, const string &name)
{
  return this->errElt (node, ParserState::ERROR_ELT_MISSING_ATTRIBUTE,
                       "Missing attribute '" + name + "'");
}

/**
 * @brief Sets parser error to "Bad value for attribute".
 * @param node The node that caused the error.
 * @param name The name of the attribute that caused the error.
 * @param value The value of the attribute that caused the error.
 * @param explain Further explanation.
 * @return \c false.
 */
bool
ParserState::errEltBadAttribute (xmlNode *node, const string &name,
                                 const string &value, const string &explain)
{
  string msg = "Bad value '" + value + "' for attribute '" + name + "'";
  if (explain != "")
    msg += " (" + explain + ")";
  return this->errElt (node, ParserState::ERROR_ELT_BAD_ATTRIBUTE, msg);
}

/**
 * @brief Sets parser error to "Attributes are mutually exclusive".
 * @param node The node that caused the error.
 * @param attr1 The name of the first attribute.
 * @param attr2 The name of the second attribute.
 * @return \c false.
 */
bool
ParserState::errEltMutuallyExclAttributes (xmlNode *node,
                                           const string &attr1,
                                           const string &attr2)
{
  return this->errElt (node,
                       ParserState::ERROR_ELT_MUTUALLY_EXCL_ATTRIBUTES,
                       "Attributes '" + attr1 + "' and '" + attr2
                           + "' are mutually exclusive");
}

/**
 * @brief Sets parser error to "Unknown child".
 * @param node The node that caused the error.
 * @param name The name of the child element that caused the error.
 * @return \c false.
 */
bool
ParserState::errEltUnknownChild (xmlNode *node, const string &name)
{
  return this->errElt (node, ParserState::ERROR_ELT_UNKNOWN_CHILD,
                       "Unknown child <" + name + ">");
}

/**
 * @brief Sets parser error to "Missing child".
 * @param node The node that caused the error.
 * @param children The list of names of children that caused the error.
 * @return \c false.
 */
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

/**
 * @brief Sets parser error to "Bad child".
 * @param node The node that caused the error.
 * @param name The name of the child that caused the error.
 * @param explain Further explanation.
 * @return \c false.
 */
bool
ParserState::errEltBadChild (xmlNode *node, const string &name,
                             const string &explain)
{
  string msg = "Bad child <" + name + ">";
  if (explain != "")
    msg += " (" + explain + ")";
  return this->errElt (node, ParserState::ERROR_ELT_BAD_CHILD, msg);
}

/**
 * @brief Sets parser error to "Syntax error in imported document".
 * @param node The \<importBase\> node that caused the error.
 * @param explain Further explanation.
 * return \c false
 */
bool
ParserState::errEltImport (xmlNode *node, const string &explain)
{
  string msg = "Syntax error in imported document";
  if (explain != "")
    msg += " (" + explain + ")";
  return this->errElt (node, ParserState::ERROR_ELT_IMPORT, msg);
}

// ParserState: private (element cache).

/**
 * @brief Indexes element cache by node.
 * @param node The node to be used as key.
 * @param[out] elt Variable to store the element associated with \p node (if
 * any).
 * @return \c true if successful, or \c false otherwise.
 */
bool
ParserState::eltCacheIndex (xmlNode *node, ParserElt **elt)
{
  auto it = _eltCache.find (node);
  if (it == _eltCache.end ())
    return false;
  g_assert_nonnull (it->second);
  tryset (elt, it->second);
  return true;
}

/**
 * @brief Indexes element cache by node parent.
 * @param node The node whose parent will be used as key.
 * @param[out] elt Variable to store the element associated with \p node (if
 * any).
 * @return \c true if successful, or \c false otherwise.
 */
bool
ParserState::eltCacheIndexParent (xmlNode *node, ParserElt **elt)
{
  return (node->parent != nullptr) ? this->eltCacheIndex (node->parent, elt)
                                   : false;
}

/**
 * @brief Indexes element cache by id.
 * @param id The id to be used as key.
 * @param[out] elt Variable to store the element associated with \p id (if
 * any).
 * @param tags Accepted tags (match only elements with one of these).
 * @return \c true if successful, or \c false otherwise.
 */
bool
ParserState::eltCacheIndexById (const string &id, ParserElt **elt,
                                const list<string> &tags)
{
  for (auto tag : tags)
    {
      auto it = _eltCacheByTag.find (tag);
      if (it == _eltCacheByTag.end ())
        continue;
      for (auto other : it->second)
        {
          string other_id;
          if (other->getAttribute ("id", &other_id) && other_id == id)
            {
              tryset (elt, other);
              return true;
            }
        }
    }
  return false;
}

/**
 * @brief Indexes element cache by tag.
 * @param tags The tags to be used as keys.
 * @param[out] result Variable to store the list of matched elements.
 * @return The number of matched elements.
 */
size_t
ParserState::eltCacheIndexByTag (const list<string> &tags,
                                 list<ParserElt *> *result)
{
  size_t n = 0;
  g_assert_nonnull (result);
  for (auto tag : tags)
    {
      auto it = _eltCacheByTag.find (tag);
      if (it == _eltCacheByTag.end ())
        continue;
      for (auto elt : it->second)
        {
          result->push_back (elt);
          n++;
        }
    }
  return n;
}

/**
 * @brief Adds element to cache.
 * @param elt The element to add.
 * @return \c true if successful, or \c false otherwise (already in cache).
 */
bool
ParserState::eltCacheAdd (ParserElt *elt)
{
  xmlNode *node;

  node = elt->getNode ();
  if (_eltCache.find (node) != _eltCache.end ())
    return false;
  _eltCache[node] = elt;
  _eltCacheByTag[elt->getTag ()].push_back (elt);
  return true;
}

// ParserState: private (alias stack).

/**
 * @brief Combines all aliases in alias stack.
 * @return The combined alias.
 */
string
ParserState::aliasStackCombine ()
{
  string result = "";
  for (auto &it : _aliasStack)
    result += it.first + "#";
  return result;
}

/**
 * @brief Peeks at alias stack.
 * @param alias Variable to store the alias at top of stack.
 * @param path Variable to store the path at top of stack.
 * @return \c true if successful, or \c false otherwise
 * (alias stack is empty).
 */
bool
ParserState::aliasStackPeek (string *alias, string *path)
{
  pair<string, string> top;

  if (_aliasStack.empty ())
    return false;

  top = _aliasStack.back ();
  tryset (alias, top.first);
  tryset (path, top.second);
  return true;
}

/**
 * @brief Pops alias stack.
 * @param alias Variable to store the popped alias.
 * @param path Variable to store the popped path.
 * @return \c true if successful, or \c false otherwise
 * (alias stack is empty).
 */
bool
ParserState::aliasStackPop (string *alias, string *path)
{
  if (!this->aliasStackPeek (alias, path))
    return false;
  _aliasStack.pop_back ();
  return true;
}

/**
 * @brief Pushes alias and path onto alias stack.
 * @param alias The alias to push.
 * @param path The path to push.
 * @return \c true if successful, or \c false otherwise
 * (path already in stack).
 */
bool
ParserState::aliasStackPush (const string &alias, const string &path)
{
  for (auto &it : _aliasStack)
    if (it.second == path)
      return false;
  _aliasStack.push_back (std::make_pair (alias, path));
  return true;
}

// ParserState: private (object stack).

/**
 * @brief Peeks at object stack.
 * @return The top of object stack or null if stack is empty.
 */
Object *
ParserState::objStackPeek ()
{
  return (_objStack.empty ()) ? nullptr : _objStack.back ();
}

/**
 * @brief Pops object from stack.
 * @return The popped object or null if the stack is empty.
 */
Object *
ParserState::objStackPop ()
{
  Object *obj = this->objStackPeek ();
  if (obj == nullptr)
    return nullptr;
  _objStack.pop_back ();
  return obj;
}

/**
 * @brief Pushes object onto stack.
 * @param obj The object to push.
 */
void
ParserState::objStackPush (Object *obj)
{
  g_assert_nonnull (obj);
  _objStack.push_back (obj);
}

// ParserState: private (rectangle stack).

/**
 * @brief Peeks at rectangle stack.
 * @return The top of rectangle stack.
 *
 * This function aborts when called with the empty stack.
 */
Rect
ParserState::rectStackPeek ()
{
  g_assert_false (_rectStack.empty ());
  return _rectStack.back ();
}

/**
 * @brief Pops rectangle from stack.
 * @return The popped rectangle.
 *
 * This function aborts when called with the empty stack.
 */
Rect
ParserState::rectStackPop ()
{
  Rect rect = this->rectStackPeek ();
  _rectStack.pop_back ();
  return rect;
}

/**
 * @brief Pushes rectangle onto stack.
 * @param rect The rectangle to push.
 */
void
ParserState::rectStackPush (Rect rect)
{
  _rectStack.push_back (rect);
}

// ParserState: private (refer map).

/**
 * @brief Indexes refer map.
 * @param id The id of the reference.
 * @param[out] media Variable to store referenced media.
 * @return \c true if successful, or \c false otherwise.
 */
bool
ParserState::referMapIndex (const string &id, Media **media)
{
  auto it = _referMap.find (id);
  if (it == _referMap.end ())
    return false;
  tryset (media, it->second);
  return true;
}

/**
 * @brief Adds entry to refer map.
 * @param id The id of the reference.
 * @param media The referenced media.
 * @return \c true if successful, or \c false otherwise (already in map).
 */
bool
ParserState::referMapAdd (const string &id, Media *media)
{
  auto it = _referMap.find (id);
  if (it != _referMap.end ())
    return false;
  _referMap[id] = media;
  return true;
}

// ParserState: private (reference solving).

/**
 * @brief Resolves reference to component within a context.
 *
 * This function assumes that \p elt has an attribute "component" which
 * contains the id of the component to be resolved.  In case resolution
 * fails, the function sets #Parser error accordingly.
 *
 * @param scope The composition that determines the resolution scope.
 * @param elt The element to be resolved.
 * @param[out] obj Variable to store the resulting object (if any).
 * @return \c true if successful, or \c false otherwise.
 */
bool
ParserState::resolveComponent (Composition *scope, ParserElt *elt,
                               Object **obj)
{
  string label;
  string comp;
  string refer;

  label = (elt->getTag () == "bindRule") ? "constituent" : "component";
  g_assert (elt->getAttribute (label, &comp));

  // Check if component refers to scope itself.
  if (comp == scope->getId () || scope->hasAlias (comp))
    {
      tryset (obj, scope);
      return true;
    }

  // Check if component refers to a child of scope.
  Object *child = scope->getChildByIdOrAlias (comp);
  if (child != nullptr)
    {
      tryset (obj, child);
      return true;
    }

  // Check if component refers to a reference (refer) object.
  Media *media;
  if (this->referMapIndex (comp, &media))
    {
      tryset (obj, media);
      return true;
    }

  // Not found.
  return this->errEltBadAttribute (elt->getNode (), label, comp,
                                   "no such object in scope");
}

/**
 * @brief Resolves reference to an interface within a context.
 *
 * This function assumes that \p elt has attributes "component" and
 * "interface" which identify the interface to be resolved.  In case
 * resolution fails, the function sets #Parser error accordingly.
 *
 * @param ctx The contexts that determines the resolution scope.
 * @param elt The element to be resolved.
 * @param[out] evt Variable to store the resulting event (if any).
 * @return \c true if successful, or \c false otherwise.
 */
bool
ParserState::resolveInterface (Context *ctx, ParserElt *elt, Event **evt)
{
  string comp;
  string iface;
  Object *obj;
  Event *result;

  if (unlikely (!this->resolveComponent (ctx, elt, &obj)))
    return false;

  if (!elt->getAttribute ("interface", &iface))
    {
      result = obj->getLambda ();
      goto success;
    }

  result = nullptr;
  if (instanceof (Media *, obj))
    {
      result = obj->getPresentationEvent (iface);
      if (result == nullptr)
        {
          result = obj->getAttributionEvent (iface);
          if (unlikely (result == nullptr))
            goto fail;
        }
    }
  else if (instanceof (Context *, obj))
    {
      if (obj == ctx)
        {
          result = obj->getAttributionEvent (iface);
          if (unlikely (result == nullptr))
            goto fail;
        }
      else
        {
          ParserElt *iface_elt;
          ParserElt *parent_elt;

          if (unlikely (
                  !this->eltCacheIndexById (iface, &iface_elt, {"port"})))
            {
              result = obj->getAttributionEvent (iface);
              if (likely (result != nullptr))
                goto success; // interface point to context property
              else
                goto fail;
            }

          // Interface points to context port: resolve it recursively.
          g_assert (this->eltCacheIndexParent (iface_elt->getNode (),
                                               &parent_elt));
          if (parent_elt->getTag () == "body")
            {
              ctx = _doc->getRoot ();
            }
          else
            {
              string id;
              g_assert (parent_elt->getAttribute ("id", &id));
              ctx = cast (Context *, _doc->getObjectById (id));
              g_assert_nonnull (ctx);
            }

          if (ctx->getId () != obj->getId ())
            goto fail;

          return this->resolveInterface (ctx, iface_elt, evt);
        }
    }
  else if (instanceof (Switch *, obj))
    {
      goto fail; // not accessible by external elements
    }
  else
    {
      g_assert_not_reached ();
    }

success:
  g_assert_nonnull (result);
  tryset (evt, result);
  return true;

fail:
  return this->errEltBadAttribute (elt->getNode (), "interface", iface,
                                   "no such interface");
}

/**
 * @brief Resolve reference to (bind, link, or ghost) parameter.
 *
 * The function uses the translation tables in the order they were given,
 * and returns as soon as any of them resolves the reference.
 *
 * @param ref The reference to resolve.
 * @param bindParams The bind parameter translation table.
 * @param linkParams The link parameter translation table.
 * @param ghosts The ghost binds translation table.
 * @return The resolved reference or \p ref itself if it is not a reference.
 */
string
ParserState::resolveParameter (const string &ref,
                               const map<string, string> *bindParams,
                               const map<string, string> *linkParams,
                               const map<string, string> *ghosts)
{
  string name;
  string result;

  if (ref[0] != '$')
    return ref; // nothing to do

  name = ref.substr (1, ref.length () - 1);
  auto it_bind = bindParams->find (name);
  if (it_bind != bindParams->end ())
    {
      result = it_bind->second;
    }
  else
    {
      auto it_link = linkParams->find (name);
      if (it_link != linkParams->end ())
        result = it_link->second;
      else
        return ref; // unknown reference
    }

  if (result[0] != '$')
    return result;

  name = result.substr (1, result.length () - 1);
  auto it_ghost = ghosts->find (name);
  if (it_ghost == ghosts->end ())
    return result; // unknown reference

  return it_ghost->second;
}

// ParserState: private (predicate solving).

/**
 * @brief Obtains predicate associated with a \<simpleCondition\> node.
 *
 * This function collects all predicates inherited by \c node and, if there
 * are such predicates, combines them into a new predicate and returns it.
 *
 * @param node The simple condition node.
 * @return A new predicate for \p node, or \c nullptr if \p node has no
 * associated predicate.
 */
Predicate *
ParserState::obtainPredicate (xmlNode *node)
{
  list<Predicate *> pred_list;
  ParserElt *elt;
  Predicate *pred;

  g_assert (toCPPString (node->name) == "simpleCondition");
  g_assert_nonnull (node->parent);
  node = node->parent;

  g_assert (this->eltCacheIndex (node, &elt));
  while (elt->getTag () != "causalConnector")
    {
      g_assert (elt->getTag () == "compoundCondition");
      if (elt->getData ("pred", (void **) &pred))
        pred_list.push_front (pred);
      g_assert (this->eltCacheIndexParent (elt->getNode (), &elt));
    }

  switch (pred_list.size ())
    {
    case 0: // no predicate
      {
        return nullptr;
      }
    case 1:
      { // single predicate
        pred = pred_list.back ();
        g_assert (pred->getType () == Predicate::CONJUNCTION);
        auto children = pred->getChildren ();
        switch (children->size ())
          {
          case 0:
            return nullptr;
          case 1:
            return children->front ()->clone ();
          default:
            return pred->clone ();
          }
        break;
      }
    default: // multiple predicates
      {
        pred = new Predicate (Predicate::CONJUNCTION);
        for (auto p : pred_list)
          {
            g_assert (p->getType () == Predicate::CONJUNCTION);
            auto children = p->getChildren ();
            switch (children->size ())
              {
              case 0:
                continue;
              case 1:
                pred->addChild (children->front ()->clone ());
                break;
              default:
                pred->addChild (p->clone ());
                break;
              }
          }
        return pred;
      }
    }
  g_assert_not_reached ();
}

/**
 * @brief Obtains predicate by solving role references in another predicate.
 *
 * This function does not modify predicate \p pred.
 *
 * @param pred The predicate to solve.
 * @param tr The role translation table mapping role references to property
 * references.
 * @return A new predicate obtained by solving \p pred via table \p tr.
 */
Predicate *
ParserState::solvePredicate (Predicate *pred, const map<string, string> *tr)
{
  list<Predicate *> buf;
  Predicate *result;

  result = pred->clone ();
  g_assert_nonnull (result);

  buf.push_back (result);
  while (!buf.empty ())
    {
      Predicate *p = buf.back ();
      buf.pop_back ();
      switch (p->getType ())
        {
        case Predicate::FALSUM:
        case Predicate::VERUM:
          break; // nothing to do
        case Predicate::ATOM:
          {
            Predicate::Test test;
            string left, right, ghost;
            p->getTest (&left, &test, &right);
            if (left[0] == '$')
              {
                auto it = tr->find (left.substr (1, left.length () - 1));
                if (it != tr->end ())
                  left = it->second;
              }
            if (right[0] == '$')
              {
                auto it = tr->find (right.substr (1, right.length () - 1));
                if (it != tr->end ())
                  right = it->second;
              }
            p->setTest (left, test, right);
            break;
          }
        case Predicate::NEGATION:
        case Predicate::CONJUNCTION:
        case Predicate::DISJUNCTION:
          for (auto child : *p->getChildren ())
            buf.push_back (child);
          break;
        default:
          g_assert_not_reached ();
        }
    }
  return result;
}

// ParserState: private (node processing).

/**
 * @brief Checks node syntax according to syntax table.
 * @param node The node to check.
 * @param[out] attrs Variable to store node's attributes.
 * @param[out] children Variable to store node's children.
 * @return Pointer to entry in syntax table if successful, otherwise returns
 * \c nullptr and sets #Parser error accordingly.
 */
ParserSyntaxElt *
ParserState::checkNode (xmlNode *node, map<string, string> *attrs,
                        list<xmlNode *> *children)
{
  string tag;
  ParserSyntaxElt *eltsyn;
  map<string, bool> possible;

  g_assert_nonnull (node);
  tag = toCPPString (node->name);

  // Check if element is known.
  if (unlikely (!parser_syntax_table_index (tag, &eltsyn)))
    return (this->errEltUnknown (node), nullptr);

  // Check parent.
  g_assert_nonnull (node->parent);
  if (eltsyn->parents.size () > 0)
    {
      string parent;
      bool found;

      if (unlikely (node->parent->type != XML_ELEMENT_NODE))
        return (this->errEltMissingParent (node), nullptr);

      parent = toCPPString (node->parent->name);
      found = false;
      for (auto par : eltsyn->parents)
        {
          if (parent == par)
            {
              found = true;
              break;
            }
        }
      if (unlikely (!found))
        return (this->errEltBadParent (node), nullptr);
    }

  // Collect attributes.
  for (auto attrsyn : eltsyn->attributes)
    {
      string value;
      string explain;

      if (!xmlGetPropAsString (node, attrsyn.name, &value)) // not found
        {
          if (attrsyn.name == "id" && eltsyn->flags & ELT_GEN_ID)
            {
              if (attrs != nullptr)
                (*attrs)["id"] = this->genId ();
              continue;
            }
          if (unlikely (attrsyn.flags & ATTR_REQUIRED))
            {
              return (this->errEltMissingAttribute (node, attrsyn.name),
                      nullptr);
            }
          else
            {
              continue;
            }
        }

      if (unlikely ((attrsyn.flags & ATTR_NONEMPTY) && value == ""))
        {
          return (this->errEltBadAttribute (node, attrsyn.name, value,
                                            "must not be empty"),
                  nullptr);
        }

      if (attrsyn.flags & ATTR_TYPE_NAME)
        {
          char offending;
          if (unlikely (!xmlIsValidName (value, &offending)))
            {
              return (this->errEltBadAttribute (
                          node, attrsyn.name, value,
                          xstrbuild ("must not contain '%c'", offending)),
                      nullptr);
            }
        }

      if ((attrsyn.flags & ATTR_TYPE_ID)
          || (attrsyn.flags & ATTR_TYPE_IDREF))
        {
          value = this->aliasStackCombine () + value;
        }

      if (attrsyn.flags & ATTR_UNIQUE)
        {
          if (unlikely (this->isInUniqueSet (value)))
            {
              return (this->errEltBadAttribute (node, attrsyn.name, value,
                                                "must be unique"),
                      nullptr);
            }
          else
            {
              this->addToUniqueSet (value);
            }
        }

      if (attrs != nullptr)
        (*attrs)[attrsyn.name] = value;
    }

  // Check for unknown attributes.
  if (attrs != nullptr)
    {
      for (xmlAttr *prop = node->properties; prop != nullptr;
           prop = prop->next)
        {
          string name = toCPPString (prop->name);
          if (unlikely (attrs->find (name) == attrs->end ()))
            return (this->errEltUnknownAttribute (node, name), nullptr);
        }
    }

  // Collect children.
  possible = parser_syntax_table_get_possible_children (tag);
  for (xmlNode *child = node->children; child; child = child->next)
    {
      if (child->type != XML_ELEMENT_NODE)
        continue;

      string child_tag = toCPPString (child->name);
      if (unlikely (possible.find (child_tag) == possible.end ()))
        return (this->errEltUnknownChild (node, child_tag), nullptr);

      if (children != nullptr)
        children->push_back (child);
    }

  return eltsyn;
}

/**
 * @brief Processes node.
 *
 * After being called by ParserState::process(), starting from the root
 * node, this function proceeds recursively, processing each node in the
 * input document tree.  For each node, it checks its syntax (according to
 * #parser_syntax_table), calls the corresponding push function (if any),
 * processes the node's children, and calls the corresponding pop function
 * (if any).  At any moment, if something goes wrong the function sets
 * #Parser error and returns false.
 *
 * @param node The node to process.
 * @return \c true if successful, or \c false otherwise.
 */
bool
ParserState::processNode (xmlNode *node)
{
  map<string, string> attrs;
  list<xmlNode *> children;
  ParserSyntaxElt *eltsyn;
  ParserElt *elt;
  bool cached;
  bool status;

  // Check node.
  eltsyn = this->checkNode (node, &attrs, &children);
  if (unlikely (eltsyn == nullptr))
    return false;

  // Allocate and initialize element wrapper.
  elt = new ParserElt (node);
  for (auto it : attrs)
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
  for (auto child : children)
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

/**
 * @brief Creates a new state.
 * @param width Initial screen width (in pixels).
 * @param height Initial screen height (in pixels).
 * @return New #ParserState.
 */
ParserState::ParserState (int width, int height)
{
  _doc = nullptr;
  _xml = nullptr;
  g_assert_cmpint (width, >, 0);
  g_assert_cmpint (height, >, 0);
  _genid = 0;
  _error = ParserState::ERROR_NONE;
  _errorMsg = "no error";
  this->rectStackPush ({0, 0, width, height});
}

/**
 * @brief Destroys state.
 */
ParserState::~ParserState ()
{
  for (auto it : _eltCache)
    delete it.second;
}

/**
 * @brief Gets last parser error.
 * @param[out] message Variable to store the last error message (if any).
 * @return Last error code.
 */
ParserState::Error
ParserState::getError (string *message)
{
  tryset (message, _errorMsg);
  return _error;
}

/**
 * @brief Processes XML document.
 *
 * This function is a wrapper over ParserState::processNode().  It calls the
 * later with the root node of the given XML document and, if nothing goes
 * wrong, returns the resulting document.
 *
 * @param xml The XML document to process.
 * @return The resulting #Document if successful, or null otherwise.
 */
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

/**
 * @brief Starts the processing of \<ncl\> element.
 * @param st #ParserState.
 * @param elt Element wrapper.
 * @return \c true if successful, or \c false otherwise.
 */
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

/**
 * @brief Ends the processing of \<ncl\> element.
 *
 * This function resolves all non-local references.
 *
 * @param st #ParserState.
 * @param elt Element wrapper.
 * @return \c true if successful, or \c false otherwise.
 */
bool
ParserState::popNcl (ParserState *st, unused (ParserElt *elt))
{
  list<ParserElt *> desc_list;
  list<ParserElt *> media_list;
  list<ParserElt *> switch_list;
  list<ParserElt *> link_list;

  // Resolve descriptor references to region/transition.
  // (I.e., move region/transition attributes to associated descriptor.)
  if (st->eltCacheIndexByTag ({"descriptor"}, &desc_list) > 0)
    {
      for (auto desc_elt : desc_list)
        {
          string region_id;
          ParserElt *region_elt;

          static const string trans_attr[] = {"transIn", "transOut"};
          string trans_id;
          ParserElt *trans_elt;

          string desc_id;
          g_assert (desc_elt->getAttribute ("id", &desc_id));

          if (desc_elt->getAttribute ("region", &region_id))
            {
              if (unlikely (!st->eltCacheIndexById (region_id, &region_elt,
                                                    {"region"})))
                {
                  return st->errEltBadAttribute (desc_elt->getNode (),
                                                 "region", region_id,
                                                 "no such region");
                }
              for (auto it : *region_elt->getAttributes ())
                if (it.first != "id")
                  desc_elt->setAttribute (it.first, it.second);
            }

          for (gsize i = 0; i < G_N_ELEMENTS (trans_attr); i++)
            {
              string val;
              string str;

              if (!desc_elt->getAttribute (trans_attr[i], &trans_id))
                continue;

              if (unlikely (!st->eltCacheIndexById (trans_id, &trans_elt,
                                                    {"transition"})))
                {
                  return st->errEltBadAttribute (desc_elt->getNode (),
                                                 trans_attr[i], trans_id,
                                                 "no such transition");
                }

              g_assert (trans_elt->getAttribute ("type", &str));
              val = "{type='" + str + "',";
              val += xstrbuild (
                  "\
subtype='%s',\
dur='%s',\
startProgress='%s',\
endProgress='%s',\
direction='%s',\
fadeColor='%s',\
horzRepeat='%s',\
vertRepeat='%s',\
borderWidth='%s',\
borderColor='%s'}",
                  (trans_elt->getAttribute ("subtype", &str)) ? str.c_str ()
                                                              : "",
                  +(trans_elt->getAttribute ("dur", &str)) ? str.c_str ()
                                                           : "0",
                  +(trans_elt->getAttribute ("startProgress", &str))
                      ? str.c_str ()
                      : "0",
                  +(trans_elt->getAttribute ("endProgress", &str))
                      ? str.c_str ()
                      : "0",
                  +(trans_elt->getAttribute ("direction", &str))
                      ? str.c_str ()
                      : "forward",
                  +(trans_elt->getAttribute ("fadeColor", &str))
                      ? str.c_str ()
                      : "",
                  +(trans_elt->getAttribute ("horzRepeat", &str))
                      ? str.c_str ()
                      : "0",
                  +(trans_elt->getAttribute ("vertRepeat", &str))
                      ? str.c_str ()
                      : "0",
                  +(trans_elt->getAttribute ("borderWidth", &str))
                      ? str.c_str ()
                      : "0",
                  +(trans_elt->getAttribute ("borderColor", &str))
                      ? str.c_str ()
                      : "");
              desc_elt->setAttribute (trans_attr[i], val);
            }
        }
    }

  // Resolve media reference to descriptor, i.e., move descriptor attributes
  // to associated media, and check for unresolved refers.
  if (st->eltCacheIndexByTag ({"media"}, &media_list) > 0)
    {
      for (auto media_elt : media_list)
        {
          string media_id;
          Media *media;

          string desc_id;
          string refer;

          // Move descriptor attributes.
          if (media_elt->getAttribute ("descriptor", &desc_id))
            {
              ParserElt *desc_elt;

              if (unlikely (!st->eltCacheIndexById (desc_id, &desc_elt,
                                                    {"descriptor"})))
                {
                  return st->errEltBadAttribute (media_elt->getNode (),
                                                 "descriptor", desc_id,
                                                 "no such descriptor");
                }

              g_assert (media_elt->getAttribute ("id", &media_id));
              media = cast (Media *,
                            st->_doc->getObjectByIdOrAlias (media_id));
              g_assert_nonnull (media);

              for (auto it : *desc_elt->getAttributes ())
                {
                  if (it.first == "id" || it.first == "region")
                    continue; // nothing to do
                  if (media->getAttributionEvent (it.first) != nullptr)
                    continue; // already defined
                  media->addAttributionEvent (it.first);
                  media->setProperty (it.first, it.second);
                }
            }

          // Check refer.
          if (media_elt->getAttribute ("refer", &refer))
            {
              ParserElt *refer_elt;

              if (unlikely (!st->eltCacheIndexById (refer, &refer_elt,
                                                    {"media"})))
                {
                  return st->errEltBadAttribute (media_elt->getNode (),
                                                 "refer", refer,
                                                 "no such media object");
                }
              if (refer_elt->getAttribute ("refer", nullptr))
                {
                  return st->errEltBadAttribute (
                      media_elt->getNode (), "refer", refer,
                      "cannot refer to a reference");
                }
            }
        }
    }

  // Resolve bind rules and default components in switches.
  // (I.e., finish parsing <bindRule> and <defaultComponent>.)
  if (st->eltCacheIndexByTag ({"switch"}, &switch_list) > 0)
    {
      for (auto switch_elt : switch_list)
        {
          string switch_id;
          Switch *swtch;

          list<pair<ParserElt *, Object *> > *rules;
          list<Object *> defaults;

          g_assert (switch_elt->getAttribute ("id", &switch_id));
          swtch
              = cast (Switch *, st->_doc->getObjectByIdOrAlias (switch_id));
          g_assert_nonnull (swtch);

          UDATA_GET (switch_elt, "rules", &rules);
          for (auto it : *rules)
            {
              ParserElt *bind_elt;
              Object *obj;

              string rule_id;
              ParserElt *rule_elt;
              Predicate *pred;

              bind_elt = it.first;
              g_assert_nonnull (bind_elt);

              obj = it.second;
              g_assert_nonnull (obj);

              if (bind_elt->getTag () == "defaultComponent")
                {
                  defaults.push_back (obj);
                  continue;
                }

              g_assert (bind_elt->getAttribute ("rule", &rule_id));
              if (!st->eltCacheIndexById (rule_id, &rule_elt,
                                          {"rule", "compositeRule"}))
                {
                  return st->errEltBadAttribute (bind_elt->getNode (),
                                                 "rule", rule_id,
                                                 "no such rule");
                }

              UDATA_GET (rule_elt, "pred", &pred);
              if (pred->getType () != Predicate::ATOM
                  && pred->getChildren ()->size () == 0)
                {
                  swtch->addRule (obj, new Predicate (Predicate::FALSUM));
                }
              else
                {
                  swtch->addRule (obj, pred->clone ());
                }
            }

          // Add defaults to the end of rule list.
          for (auto obj : defaults)
            swtch->addRule (obj, new Predicate (Predicate::VERUM));
        }
    }

  // Resolve link reference to connector.
  // (I.e., finish links parsing and add them to contexts.)
  if (st->eltCacheIndexByTag ({"link"}, &link_list) > 0)
    {
      for (auto link_elt : link_list)
        {
          string conn_id;
          ParserElt *conn_elt;
          set<string> *tests;
          list<ParserConnRole> *roles;
          list<ParserLinkBind> *binds;
          map<string, string> *params;
          Context *ctx;

          list<pair<ParserConnRole *, ParserLinkBind *> > bound;
          list<ParserLinkBind *> tests_buf;
          map<string, string> tests_map;
          list<ParserLinkBind *> ghosts_buf;
          map<string, string> ghosts_map;

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
          UDATA_GET (link_elt, "params", &params);
          UDATA_GET (link_elt, "context", &ctx);

          // Process binds.
          for (auto &bind : *binds)
            {
              for (auto &role : *roles)
                {
                  if (bind.role != role.role)
                    continue;

                  // Attach predicate to condition.
                  if (toCPPString (role.node->name) == "simpleCondition")
                    {
                      Predicate *pred = st->obtainPredicate (role.node);
                      if (role.predicate != nullptr)
                        delete role.predicate;
                      role.predicate = pred;
                    }

                  // Mark role-bind pair as bound.
                  bound.push_back (std::make_pair (&role, &bind));
                  break;
                }
              if (bound.size () > 0 && bound.back ().second == &bind)
                {
                  continue; // done
                }
              else if (tests->find (bind.role) != tests->end ())
                {
                  // Mark bind as test.
                  tests_buf.push_back (&bind);
                }
              else
                {
                  // Mark bind as ghost.
                  ghosts_buf.push_back (&bind);
                }
            }

          // Check if all event roles are bound.
          for (auto &role : *roles)
            {
              bool found = false;
              for (auto it : bound)
                {
                  if (&role == it.first)
                    {
                      found = true;
                      break;
                    }
                }
              if (unlikely (!found))
                {
                  return st->errEltBadAttribute (
                      link_elt->getNode (), "xconnector", conn_id,
                      "link does not match connector, "
                      "role '"
                          + role.role + "' not bound");
                }
            }

          // Check if all test roles are bound.
          for (auto label : *tests)
            {
              bool found = false;
              for (auto bind : tests_buf)
                {
                  if (label == bind->role)
                    {
                      found = true;
                      break;
                    }
                }
              if (unlikely (!found))
                {
                  return st->errEltBadAttribute (
                      link_elt->getNode (), "xconnector", conn_id,
                      "link does not match connector, "
                      "role '"
                          + label + "' not bound");
                }
            }

          // Resolve test and binds.
          for (auto &it : {std::make_pair (&tests_buf, &tests_map),
                            std::make_pair (&ghosts_buf, &ghosts_map) })
            {
              for (auto &bind : *it.first)
                {
                  ParserElt *elt;
                  Event *evt;
                  g_assert_nonnull (bind->node);
                  g_assert (st->eltCacheIndex (bind->node, &elt));
                  if (unlikely (!st->resolveInterface (ctx, elt, &evt)))
                    return false;
                  (*it.second)[bind->role] = "$" + evt->getFullId ();
                }
            }

          // Resolve event roles.
          list<Action> conditions;
          list<Action> actions;
          for (auto it : bound)
            {
              ParserConnRole *role;
              ParserLinkBind *bind;
              ParserElt *bind_elt;
              Object *obj;
              Event *evt;
              Event::Type evtType;
              Action act;
              string iface;

              role = it.first;
              g_assert_nonnull (role);

              bind = it.second;
              g_assert_nonnull (bind);
              g_assert_nonnull (bind->node);
              g_assert (st->eltCacheIndex (bind->node, &bind_elt));

              if (unlikely (!st->resolveInterface (ctx, bind_elt, &evt)))
                return false;

              evtType = evt->getType ();
              obj = evt->getObject ();
              g_assert_nonnull (obj);

              switch (role->eventType)
                {
                case Event::PRESENTATION:
                  {
                    if (unlikely (evtType != role->eventType))
                      {
                        return st->errEltBadAttribute (
                            bind->node, "interface", bind->iface,
                            "expected a presentation event");
                      }
                    act.event = evt;
                    break;
                  }
                case Event::ATTRIBUTION:
                  {
                    if (unlikely (evtType != role->eventType))
                      {
                        return st->errEltBadAttribute (
                            bind->node, "interface", bind->iface,
                            "expected an attribution event");
                      }
                    act.event = evt;
                    act.value = st->resolveParameter (
                        role->value, &bind->params, params, &ghosts_map);
                    break;
                  }
                case Event::SELECTION:
                  {
                    if (unlikely ((evtType == Event::PRESENTATION
                                   && evt->getId () != "@lambda")
                                  || evtType == Event::ATTRIBUTION))
                      {
                        return st->errEltBadAttribute (
                            bind->node, "interface", evt->getId (),
                            "must be empty");
                      }
                    act.value = st->resolveParameter (
                        role->key, &bind->params, params, &ghosts_map);
                    obj->addSelectionEvent (act.value);
                    act.event = obj->getSelectionEvent (act.value);
                    g_assert_nonnull (act.event);
                    act.event->setParameter ("key", act.value);
                    break;
                  }
                default:
                  g_assert_not_reached ();
                }
              g_assert_nonnull (act.event);
              act.transition = role->transition;

              act.duration = st->resolveParameter (
                  role->duration, &bind->params, params, &ghosts_map);

              act.delay = st->resolveParameter (role->delay, &bind->params,
                                                params, &ghosts_map);

              act.predicate = nullptr;
              if (role->predicate != nullptr)
                {
                  Predicate::Type type = role->predicate->getType ();
                  switch (type)
                    {
                    case Predicate::FALSUM:
                    case Predicate::VERUM:
                    case Predicate::ATOM:
                      act.predicate = st->solvePredicate (role->predicate,
                                                          &tests_map);
                      break;
                    case Predicate::NEGATION:
                    case Predicate::CONJUNCTION:
                    case Predicate::DISJUNCTION:
                      if (role->predicate->getChildren ()->size () > 0)
                        act.predicate = st->solvePredicate (role->predicate,
                                                            &tests_map);
                      break;
                    default:
                      g_assert_not_reached ();
                    }
                }

              if (role->condition)
                conditions.push_back (act);
              else
                actions.push_back (act);
            }
          ctx->addLink (conditions, actions);
        }
    }

  g_assert_nonnull (st->objStackPop ());
  return true;
}

/**
 * @brief Starts the processing of \<region\> element.
 *
 * This function uses the initial screen dimensions stored in #ParserState
 * to convert into absolute values any relative values used in \<region\>
 * attributes.
 *
 * @fn ParserState::pushRegion
 * @param st #ParserState.
 * @param elt Element wrapper.
 * @return \c true if successful, or \c false otherwise.
 */
bool
ParserState::pushRegion (ParserState *st, ParserElt *elt)
{
  static int last_zorder = 0;
  xmlNode *parent_node;
  Rect screen;
  Rect parent;
  Rect rect;
  string str;

  parent_node = elt->getParentNode ();
  g_assert_nonnull (parent_node);

  screen = st->_rectStack.front ();
  rect = parent = st->rectStackPeek ();
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
  st->rectStackPush (rect);
  double left = ((double) rect.x / screen.width) * 100.;
  double top = ((double) rect.y / screen.height) * 100.;
  double width = ((double) rect.width / screen.width) * 100.;
  double height = ((double) rect.height / screen.height) * 100.;

  elt->setAttribute ("zOrder", xstrbuild ("%d", last_zorder++));
  elt->setAttribute ("left", xstrbuild ("%g%%", left));
  elt->setAttribute ("top", xstrbuild ("%g%%", top));
  elt->setAttribute ("width", xstrbuild ("%g%%", width));
  elt->setAttribute ("height", xstrbuild ("%g%%", height));

  return true;
}

/**
 * @brief Ends the processing of \<region\> element.
 * @param st #ParserState.
 * @param elt Element wrapper.
 * @return \c true if successful, or \c false otherwise.
 */
bool
ParserState::popRegion (ParserState *st, unused (ParserElt *elt))
{
  st->rectStackPop ();
  return true;
}

/**
 * @brief Starts the processing of \<descriptorParam\> element.
 * @param st #ParserState.
 * @param elt Element wrapper.
 * @return \c true if successful, or \c false otherwise.
 */
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

/**
 * @brief Starts the processing of \<causalConnector\> element.
 * @fn ParserState::pushCausalConnector
 * @param st #ParserState.
 * @param elt Element wrapper.
 * @return \c true if successful, or \c false otherwise.
 */

/// Cleans up the set of test roles attached to connector #ParserElt.
static void
testsCleanup (void *ptr)
{
  delete (set<string> *) ptr;
}

/// Cleans up the list of #ParserConnRole attached to connector #ParserElt.
static void
rolesCleanup (void *ptr)
{
  auto roles = (list<ParserConnRole> *) ptr;
  for (auto role : *roles)
    if (role.predicate != nullptr)
      delete role.predicate;
  delete roles;
}

bool
ParserState::pushCausalConnector (ParserState *st, ParserElt *elt)
{
  UDATA_SET (st, "conn-elt", elt, nullptr);
  UDATA_SET (elt, "tests", new set<string> (), testsCleanup);
  UDATA_SET (elt, "roles", new list<ParserConnRole> (), rolesCleanup);
  return true;
}

/**
 * @brief Ends the processing of \<causalConnector\> element.
 * @param st #ParserState.
 * @param elt Element wrapper.
 * @return \c true if successful, or \c false otherwise.
 */
bool
ParserState::popCausalConnector (ParserState *st, ParserElt *elt)
{
  list<ParserConnRole> *roles;
  int nconds;
  int nacts;

  UDATA_SET (st, "conn-elt", nullptr, nullptr);
  UDATA_GET (elt, "roles", &roles);
  nconds = nacts = 0;

  for (auto role : *roles)
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

/**
 * @brief Starts the processing of \<compoundCondition\> element.
 * @fn ParserState::pushCompoundCondition
 * @param st #ParserState.
 * @param elt Element wrapper.
 * @return \c true if successful, or \c false otherwise.
 */

/// Cleans up the #Predicate attached to compound condition #ParserElt.
static void
predCleanup (void *ptr)
{
  delete (Predicate *) ptr;
}

bool
ParserState::pushCompoundCondition (unused (ParserState *st),
                                    ParserElt *elt)
{
  UDATA_SET (elt, "pred", new Predicate (Predicate::CONJUNCTION),
             predCleanup);
  return true;
}

/**
 * @brief Starts the processing of \<simpleCondition\> or \<simpleAction\>
 * element.
 *
 * This function parsers \p elt and stores it as a #ParserConnRole in the
 * list of rules of the current connector.
 *
 * @param st #ParserState.
 * @param elt Element wrapper.
 * @return \c true if successful, or \c false otherwise.
 */
bool
ParserState::pushSimpleCondition (ParserState *st, ParserElt *elt)
{
  ParserConnRole role;
  string transition;
  xmlNode *node;
  bool condition;
  ParserElt *conn_elt;
  list<ParserConnRole> *roles;
  string delay;

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
  role.predicate = nullptr;

  node = elt->getNode ();
  g_assert_nonnull (node);

  if (parser_syntax_reserved_role_table_index (
          role.role, &condition, &role.eventType, &role.transition))
    {
      string str;
      Event::Type type;
      Event::Transition trans;

      if (unlikely (role.condition != condition))
        {
          return st->errEltBadAttribute (
              node, "role", role.role,
              "reserved role '" + role.role + "' must be "
                  + string ((condition) ? "a condition" : "an action"));
        }

      if (unlikely (elt->getAttribute ("eventType", &str)
                    && (!parser_syntax_event_type_table_index (str, &type)
                        || type != role.eventType)))
        {
          return st->errEltBadAttribute (node, "eventType", str,
                                         "reserved role '" + role.role
                                             + "' cannot be overwritten");
        }

      if (unlikely (elt->getAttribute (transition, &str)
                    && (!parser_syntax_transition_table_index (str, &trans)
                        || trans != role.transition)))
        {
          return st->errEltBadAttribute (node, transition, str,
                                         "reserved role '" + role.role
                                             + "' cannot be overwritten");
        }
    }
  else
    {
      string str;
      if (unlikely (!elt->getAttribute ("eventType", &str)))
        {
          return st->errEltMissingAttribute (node, "eventType");
        }
      if (unlikely (
              !parser_syntax_event_type_table_index (str, &role.eventType)))
        {
          return st->errEltBadAttribute (node, "eventType", str);
        }
      if (unlikely (!elt->getAttribute (transition, &str)))
        {
          return st->errEltMissingAttribute (node, transition);
        }
      if (unlikely (!parser_syntax_transition_table_index (
              str, &role.transition)))
        {
          return st->errEltBadAttribute (node, transition, str);
        }
    }

  if (!role.condition)
    elt->getAttribute ("duration", &role.duration);

  if (!role.condition)
    elt->getAttribute ("delay", &role.delay);

  if (role.eventType == Event::SELECTION)
    elt->getAttribute ("key", &role.key);

  if (unlikely (!role.condition && role.eventType == Event::ATTRIBUTION
                && !elt->getAttribute ("value", &role.value)))
    {
      return st->errEltMissingAttribute (node, "value");
    }

  UDATA_GET (st, "conn-elt", &conn_elt);
  UDATA_GET (conn_elt, "roles", &roles);
  roles->push_back (role);

  return true;
}

/**
 * @brief Starts the processing of \<compoundStatement\> element.
 *
 * This function creates an associated compound #Predicate for \p elt.
 *
 * @param st #ParserState.
 * @param elt Element wrapper.
 * @return \c true if successful, or \c false otherwise.
 */
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

/**
 * @brief Ends the processing of \<compoundStatement\> element.
 * @param st #ParserState.
 * @param elt Element wrapper.
 * @return \c true if successful, or \c false otherwise.
 */
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

  if (unlikely ((type == Predicate::NEGATION
                 && pred->getChildren ()->size () == 0))
      || ((type == Predicate::CONJUNCTION || type == Predicate::DISJUNCTION)
          && pred->getChildren ()->size () < 2))
    {
      return st->errEltMissingChild (
          elt->getNode (), {"compoundStatement", "assessmentStatement"});
    }

  return true;
}

/**
 * @brief Ends the processing of \<assessmentStatement\> element.
 *
 * This function creates an associated atomic #Predicate for \p elt.
 *
 * @param st #ParserState.
 * @param elt Element wrapper.
 * @return \c true if successful, or \c false otherwise.
 */
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
      return st->errEltMissingChild (
          elt->getNode (), {"attributeAssessment", "valueAssessment"});
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

/**
 * @brief Starts the processing of \<attributeAssessment\> or
 * \<valueAssessment\> element.
 * @fn ParserState::pushAttributeAssessment
 * @param st #ParserState.
 * @param elt Element wrapper.
 * @return \c true if successful, or \c false otherwise.
 */

/// Cleans up the strings attached to attribute assessment #ParserElt.
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
      UDATA_GET (st, "conn-elt", &conn_elt);
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

/**
 * @brief Starts the processing of \<rule\> or \<compositeRule\> element.
 * @fn ParserState::pushRule
 * @param st #ParserState.
 * @param elt Element wrapper.
 * @return \c true if successful, or \c false otherwise.
 */

/// Cleans up the #Predicate attached to rule or composite rule #ParserElt.
static void
rulePredCleanup (void *ptr)
{
  delete (Predicate *) ptr;
}

bool
ParserState::pushRule (ParserState *st, ParserElt *elt)
{
  Predicate *pred;
  ParserElt *parent_elt;
  Predicate *parent_pred;

  if (elt->getTag () == "rule")
    {
      string var;
      string comp;
      string value;
      Predicate::Test test;

      g_assert (elt->getAttribute ("var", &var));
      g_assert (elt->getAttribute ("comparator", &comp));
      if (unlikely (!parser_syntax_comparator_table_index (comp, &test)))
        return st->errEltBadAttribute (elt->getNode (), "comparator", comp);

      g_assert (elt->getAttribute ("value", &value));
      pred = new Predicate (Predicate::ATOM);
      pred->setTest ("$__settings__." + var, test, value);
    }
  else if (elt->getTag () == "compositeRule")
    {
      string op;
      Predicate::Type type;

      g_assert (elt->getAttribute ("operator", &op));
      if (unlikely (!parser_syntax_connective_table_index (op, &type)))
        return st->errEltBadAttribute (elt->getNode (), "operator", op);

      pred = new Predicate (type);
    }
  else
    {
      g_assert_not_reached ();
    }
  g_assert_nonnull (pred);

  g_assert (st->eltCacheIndexParent (elt->getNode (), &parent_elt));
  if (parent_elt->getTag () == "ruleBase")
    {
      UDATA_SET (elt, "pred", pred, rulePredCleanup);
    }
  else
    {
      UDATA_GET (parent_elt, "pred", &parent_pred);
      if (unlikely (parent_pred->getType () == Predicate::NEGATION
                    && parent_pred->getChildren ()->size () == 1))
        {
          delete pred;
          return st->errEltBadChild (parent_elt->getNode (), elt->getTag (),
                                     "too many children");
        }
      UDATA_SET (elt, "pred", pred, nullptr);
      parent_pred->addChild (pred);
    }

  return true;
}

/**
 * @brief Starts the processing of \<importBase\> element.
 *
 * This function uses the #ParserState alias stack to collect and process
 * nested imports.
 *
 * @fn ParserState::pushImportBase
 * @param st #ParserState.
 * @param elt Element wrapper.
 * @return \c true if successful, or \c false otherwise.
 *
 * @todo Check for circular imports.
 */

/// Cleans up the document associated with \<importBase\> element.
static void
xmlDocCleanup (void *ptr)
{
  xmlFreeDoc ((xmlDoc *) ptr);
}

bool
ParserState::pushImportBase (ParserState *st, ParserElt *elt)
{
  ParserElt *parent_elt;
  string alias;
  string path;

  xmlDoc *xml;
  xmlNode *root;
  xmlNode *head;

  list<xmlNode *> children;
  bool status;

  g_assert (st->eltCacheIndexParent (elt->getNode (), &parent_elt));
  g_assert (elt->getAttribute ("alias", &alias));
  g_assert (elt->getAttribute ("documentURI", &path));

  // Make import path absolute.
  if (!xpathisabs (path))
    {
      string dir;
      dir = (st->aliasStackPeek (nullptr, &dir)) ? xpathdirname (dir)
                                                 : st->getDirname ();
      path = xpathbuildabs (dir, path);
    }

  // Push import alias and path onto alias stack.
  if (unlikely (!st->aliasStackPush (alias, path)))
    {
      return st->errEltImport (elt->getNode (), "circular import");
    }

  // Read the imported document.
  xml = xmlReadFile (path.c_str (), nullptr, PARSER_LIBXML_FLAGS);
  if (unlikely (xml == nullptr))
    {
      string errmsg = xmlGetLastErrorAsString ();
      return st->errEltImport (elt->getNode (), errmsg);
    }

  UDATA_SET (elt, "xmlDoc", xml, xmlDocCleanup);
  root = xmlDocGetRootElement (xml);
  g_assert_nonnull (root);

  // Check imported document root.
  if (unlikely (st->checkNode (root, nullptr, nullptr) == nullptr))
    return false;

  // Get imported document head.
  children = xmlFindAllChildren (root, "head");
  if (unlikely (children.size () == 0))
    goto fail_no_such_base;

  // Check imported document head.
  // (We're assuming that there is only one imported head.)
  head = children.front ();
  if (unlikely (st->checkNode (head, nullptr, nullptr) == nullptr))
    return false;

  // Get all occurrences of the desired base.
  children = xmlFindAllChildren (head, parent_elt->getTag ());
  if (unlikely (children.size () == 0))
    goto fail_no_such_base;

  // If we're importing a descriptor base, make sure we also import region
  // and transition bases.
  if (parent_elt->getTag () == "descriptorBase")
    {
      list<string> extra = {"regionBase", "transitionBase"};
      for (auto &it : extra)
        for (auto child : xmlFindAllChildren (head, it))
          children.push_back (child);
    }

  // Process all imported base.
  for (auto base : children)
    if (unlikely (!(status = st->processNode (base))))
      break;

  g_assert (st->aliasStackPop (nullptr, nullptr));
  return status;

fail_no_such_base:
  return st->errEltImport (elt->getNode (), "no <" + parent_elt->getTag ()
                                                + "> in imported document");
}

/**
 * @brief Starts the processing of \<body\> or \<context\> element.
 *
 * This function parses \p elt and pushes it as a #Context onto the object
 * stack.
 *
 * @fn ParserState::pushContext
 * @param st #ParserState.
 * @param elt Element wrapper.
 * @return \c true if successful, or \c false otherwise.
 */

/// Cleans up the list of port ids attached to context #ParserElt.
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

/**
 * @brief Ends the processing of \<body\> or \<context\> element.
 *
 * This function resolves context ports and pops the object stack.
 *
 * @param st #ParserState.
 * @param elt Element wrapper.
 * @return \c true if successful, or \c false otherwise.
 */
bool
ParserState::popContext (ParserState *st, ParserElt *elt)
{
  Context *ctx;
  list<string> *ports;

  ctx = cast (Context *, st->objStackPeek ());
  g_assert_nonnull (ctx);

  // Resolve port references.
  UDATA_GET (elt, "ports", &ports);
  for (auto port_id : *ports)
    {
      ParserElt *port_elt;
      Event *evt;

      g_assert (st->eltCacheIndexById (port_id, &port_elt, {"port"}));
      if (unlikely (!st->resolveInterface (ctx, port_elt, &evt)))
        return false;

      ctx->addPort (evt);
    }

  st->objStackPop ();
  return true;
}

/**
 * @brief Starts the processing of \<port\> element.
 * @param st #ParserState.
 * @param elt Element wrapper.
 * @return \c true if successful, or \c false otherwise.
 */
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

/**
 * @brief Starts the processing of \<switch\>.
 *
 * This function parsers \p elt and pushes it as a #Switch onto the object
 * stack.
 *
 * @fn ParserState::pushSwitch
 * @param st #ParserState.
 * @param elt Element wrapper.
 * @return \c true if successful, or \c false otherwise.
 */

/// Cleans up the rules attached to switch #ParserElt.
static void
rulesCleanup (void *ptr)
{
  delete (list<pair<ParserElt *, Object *> > *) ptr;
}

bool
ParserState::pushSwitch (ParserState *st, ParserElt *elt)
{
  Composition *parent;
  Object *swtch;
  string id;

  parent = cast (Composition *, st->objStackPeek ());
  g_assert_nonnull (parent);

  g_assert (elt->getAttribute ("id", &id));
  swtch = new Switch (id);
  parent->addChild (swtch);

  // Create rule list.
  UDATA_SET (elt, "rules", (new list<pair<ParserElt *, Object *> > ()),
             rulesCleanup);

  // Push context onto stack.
  st->objStackPush (swtch);

  return true;
}

/**
 * @brief Ends the processing of \<switch\> element.
 *
 * This function pops the object stack.
 *
 * @param st #ParserState.
 * @param elt Element wrapper.
 * @return \c true if successful, or \c false otherwise.
 */
bool
ParserState::popSwitch (ParserState *st, unused (ParserElt *elt))
{
  Switch *swtch;
  list<pair<ParserElt *, Object *> > *rules;

  swtch = cast (Switch *, st->objStackPeek ());
  g_assert_nonnull (swtch);

  // Resolve bind-rule and default-component  references.
  UDATA_GET (elt, "rules", &rules);
  for (auto &it : *rules)
    {
      ParserElt *bind_elt;
      Object *obj;

      bind_elt = it.first;
      g_assert_nonnull (bind_elt);

      if (unlikely (!st->resolveComponent (swtch, bind_elt, &obj)))
        return false;

      g_assert_nonnull (obj);
      it.second = obj;
    }

  st->objStackPop ();
  return true;
}

/**
 * @brief Starts the processing of \<bindRule\> or \<defaultComponent\>
 * element.
 *
 * @param st #ParserState.
 * @param elt Element wrapper.
 * @return \c true if successful, or \c false otherwise.
 */
bool
ParserState::pushBindRule (ParserState *st, ParserElt *elt)
{
  ParserElt *parent_elt;
  list<pair<ParserElt *, Object *> > *rules;

  if (elt->getTag () == "bindRule")
    {
      g_assert (elt->getAttribute ("constituent", nullptr));
      g_assert (elt->getAttribute ("rule", nullptr));
    }
  else if (elt->getTag () == "defaultComponent")
    {
      g_assert (elt->getAttribute ("component", nullptr));
    }
  else
    {
      g_assert_not_reached ();
    }
  g_assert (st->eltCacheIndexParent (elt->getNode (), &parent_elt));
  UDATA_GET (parent_elt, "rules", &rules);
  rules->push_back (std::make_pair (elt, nullptr));

  return true;
}

/**
 * @brief Starts the processing of \<media\> element.
 *
 * This function parsers \p elt and pushes it as a #Media onto the object
 * stack.
 *
 * @param st #ParserState.
 * @param elt Element wrapper.
 * @return \c true if successful, or \c false otherwise.
 */
bool
ParserState::pushMedia (ParserState *st, ParserElt *elt)
{
  Composition *parent;
  Media *media;
  string id;
  string type;
  string refer;
  string src;

  g_assert (elt->getAttribute ("id", &id));
  if (elt->getAttribute ("type", &type))
    {
      if (unlikely (elt->getAttribute ("refer", &refer)))
        {
          return st->errEltMutuallyExclAttributes (elt->getNode (), "type",
                                                   "refer");
        }
      if (type == "application/x-ginga-settings")
        {
          media = st->_doc->getSettings ();
          g_assert_nonnull (media);
          media->addAlias (id);
          goto done;
        }
    }

  if (elt->getAttribute ("refer", &refer))
    {
      if (unlikely (elt->getAttribute ("src", &src)))
        {
          return st->errEltMutuallyExclAttributes (elt->getNode (), "src",
                                                   "refer");
        }

      media = cast (Media *, st->_doc->getObjectByIdOrAlias (refer));
      if (media != nullptr)
        goto almost_done;
    }
  else
    {
      elt->getAttribute ("src", &src);
      if (src != "" && !xpathisuri (src) && !xpathisabs (src))
        {
          string dir = st->getDirname ();
          src = xpathbuildabs (dir, src);
        }

      if (st->referMapIndex (id, &media))
        {
          media->setProperty ("uri", src);
          goto done;
        }
    }

  media = new Media (id);
  media->setProperty ("uri", src);
  media->setProperty ("type", type);

  parent = cast (Composition *, st->objStackPeek ());
  g_assert_nonnull (parent);
  parent->addChild (media);

almost_done:
  if (refer != "")
    {
      media->addAlias (id);
      media->addAlias (refer);
      st->referMapAdd (refer, media);
      g_assert (st->referMapAdd (id, media));
    }

done:
  st->objStackPush (media);
  return true;
}

/**
 * @brief Ends the processing of \<media\> element.
 *
 * This function pops the object stack.
 *
 * @param st #ParserState.
 * @param elt Element wrapper.
 * @return \c true if successful, or \c false otherwise.
 */
bool
ParserState::popMedia (ParserState *st, unused (ParserElt *elt))
{
  g_assert (instanceof (Media *, st->objStackPop ()));
  return true;
}

/**
 * @brief Starts the processing of \<area\> element.
 * @param st #ParserState.
 * @param elt Element wrapper.
 * @return \c true if successful, or \c false otherwise.
 */
bool
ParserState::pushArea (ParserState *st, ParserElt *elt)
{
  Media *media;
  string id;
  string str;
  string label;
  Time begin, end;

  media = cast (Media *, st->objStackPeek ());
  g_assert_nonnull (media);

  g_assert (elt->getAttribute ("id", &id));

  if (elt->getAttribute ("label", &label) && label != "")
    {
      if (unlikely (elt->getAttribute ("begin", &str)))
        {
          return st->errEltMutuallyExclAttributes (elt->getNode (), "label",
                                                   "begin");
        }
      if (unlikely (elt->getAttribute ("end", &str)))
        {
          return st->errEltMutuallyExclAttributes (elt->getNode (), "label",
                                                   "end");
        }
      media->addPresentationEvent (id, label);
    }
  else
    {
      begin = 0;
      if (elt->getAttribute ("begin", &str))
        {
          if (unlikely (!ginga::try_parse_time (str, &begin)))
            {
              return st->errEltBadAttribute (elt->getNode (), "begin", str);
            }
        }

      end = GINGA_TIME_NONE;
      if (elt->getAttribute ("end", &str))
        {
          if (unlikely (!ginga::try_parse_time (str, &end)))
            {
              return st->errEltBadAttribute (elt->getNode (), "end", str);
            }
        }

      media->addPresentationEvent (id, begin, end);
    }

  return true;
}

/**
 * @brief Starts the processing of \<property\> element.
 * @param st #ParserState.
 * @param elt Element wrapper.
 * @return \c true if successful, or \c false otherwise.
 */
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

/**
 * @brief Starts the processing of \<link\> element.
 * @fn ParserState::pushLink
 * @param st #ParserState.
 * @param elt Element wrapper.
 * @return \c true if successful, or \c false otherwise.
 */

/// Cleans up list of #ParserLinkBind attached to link #ParserElt.
static void
bindsCleanup (void *ptr)
{
  delete (list<ParserLinkBind> *) ptr;
}

/// Cleans up parameter map attached to link #ParserElt.
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

/**
 * @brief Starts the processing of \<linkParam\> or \<bindParam\> element.
 * @param st #ParserState.
 * @param elt Element wrapper.
 * @return \c true if successful, or \c false otherwise.
 */
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

/**
 * @brief Starts the processing of \<bind\> element.
 *
 * This function parsers \p elt and stores it as a #ParserLinkBind in the
 * list of binds of the current link.
 *
 * @param st #ParserState.
 * @param elt Element wrapper.
 * @return \c true if successful, or \c false otherwise.
 */
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

  g_assert (st->eltCacheIndexParent (elt->getNode (), &parent_elt));
  UDATA_GET (parent_elt, "binds", &binds);
  binds->push_back (bind);
  UDATA_SET (elt, "params", &binds->back ().params, nullptr);

  return true;
}

bool
ParserState::pushFont (ParserState *st, ParserElt *elt)
{
  std::string family, src;
  g_assert (elt->getAttribute ("family", &family));
  g_assert (elt->getAttribute ("src", &src));

  const FcChar8 *fcfamily = (const FcChar8 *) family.c_str();
  std::string abs_src = xpathbuildabs (st->getDirname(), src);
  const FcChar8 *fcfilename = (const FcChar8 *) abs_src.c_str();

  FcBool fontAddStatus = FcConfigAppFontAddFile (NULL, fcfilename);

  TRACE ("Adding font family='%s' src='%s' success: %d.",
         family.c_str(), src.c_str(), fontAddStatus);

  if (fontAddStatus)
    {
      FcFontSet *set = FcConfigGetFonts (FcConfigGetCurrent(),
                                         FcSetApplication);
      FcPattern *new_font = set->fonts[set->nfont-1];

      // Add the family name specified in the <font> elt.
      FcPatternAddString (new_font, FC_FAMILY, fcfamily);
    }

  return fontAddStatus;
}

// External API.

/// Helper function used by Parser::parseBuffer() and Parser::parseFile().
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

/**
 * @brief Parses NCL document from memory buffer.
 * @fn Parser::parseBuffer
 * @param buf Buffer.
 * @param size Buffer size in bytes.
 * @param width Initial screen width (in pixels).
 * @param height Initial screen height (in pixels).
 * @param[out] errmsg Variable to store the error message (if any).
 * @return The resulting #Document if successful, or null otherwise.
 */
Document *
Parser::parseBuffer (const void *buf, size_t size, int width, int height,
                     string *errmsg)
{
  xmlDoc *xml;
  Document *doc;

  xml = xmlReadMemory ((const char *) buf, (int) size, nullptr, nullptr,
                       PARSER_LIBXML_FLAGS);
  if (unlikely (xml == nullptr))
    {
      tryset (errmsg, xmlGetLastErrorAsString ());
      return nullptr;
    }

  doc = process (xml, width, height, errmsg);
  xmlFreeDoc (xml);
  return doc;
}

/**
 * @brief Parses NCL document from file.
 * @param path File path.
 * @param width Initial screen width (in pixels).
 * @param height Initial screen height (in pixels).
 * @param[out] errmsg Variable to store the error message (if any).
 * @return The resulting #Document if successful, or null otherwise.
 */
Document *
Parser::parseFile (const string &path, int width, int height,
                   string *errmsg)
{
  xmlDoc *xml;
  Document *doc;

  xml = xmlReadFile (path.c_str (), nullptr, PARSER_LIBXML_FLAGS);
  if (unlikely (xml == nullptr))
    {
      tryset (errmsg, xmlGetLastErrorAsString ());
      return nullptr;
    }

  doc = process (xml, width, height, errmsg);
  xmlFreeDoc (xml);
  return doc;
}

GINGA_NAMESPACE_END
