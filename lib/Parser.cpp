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
#include "Media.h"
#include "MediaSettings.h"

#include <libxml/tree.h>
#include <libxml/parser.h>

GINGA_NAMESPACE_BEGIN

GINGA_PRAGMA_DIAG_IGNORE (-Wunused-function)

// Helper macros and functions.
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


// Parser internal state.

typedef map<string, xmlNode *> ParserEltCache;
typedef map<string, map<string, string>> ParserAttrCache;

typedef struct ParserState
{
  xmlDoc *doc;                  // DOM tree
  Rect rect;                    // screen dimensions
  int genid;                    // last generated id
  string errmsg;                // last error message

  list<Object *> objStack;      // object stack
  ParserAttrCache cachedAttrs;  // attrmap indexed by id
  ParserEltCache cachedElts;    // xmlNode indexed by id

  set<Object *> *objects;       // all objects
  map<string, Object *> objMap; // all objects
  Object *root;                 // root object
} ParserState;

static inline G_GNUC_PRINTF (2,3) void
_st_err (ParserState *st, const char *fmt, ...)
{
  va_list args;
  char *c_str = nullptr;
  int n;

  va_start (args, fmt);
  n = g_vasprintf (&c_str, fmt, args);
  va_end (args);

  g_assert (n >= 0);
  g_assert_nonnull (c_str);
  st->errmsg.assign (c_str);
  g_free (c_str);
}

#define ST_ERR(st, fmt, ...)\
  (_st_err ((st), fmt, ## __VA_ARGS__), false)

#define ST_ERR_LINE(st, line, fmt, ...)\
  ST_ERR ((st), "Syntax error at line %d: " fmt, (line), ## __VA_ARGS__)

#define ST_ERR_ELT(st, elt, fmt, ...)                   \
  ST_ERR_LINE ((st), (elt)->line, "Element <%s>: " fmt, \
               toCString ((elt)->name), ## __VA_ARGS__)

#define ST_ERR_ELT_UNKNOWN(st, elt)\
  ST_ERR_ELT ((st), (elt), "Unknown element")

#define ST_ERR_ELT_MISSING_PARENT(st, elt)\
  ST_ERR_ELT ((st), (elt), "Missing parent")

#define ST_ERR_ELT_BAD_PARENT(st, elt, parent)\
  ST_ERR_ELT ((st), (elt), "Bad parent <%s>", (parent))

#define ST_ERR_ELT_UNKNOWN_ATTR(st, elt, attr)\
  ST_ERR_ELT ((st), (elt), "Unknown attribute '%s'", (attr))

#define ST_ERR_ELT_MISSING_ATTR(st, elt, attr)\
  ST_ERR_ELT ((st), (elt), "Missing attribute '%s'", (attr))

#define ST_ERR_ELT_BAD_ATTR(st, elt, attr, val, explain)                \
  ST_ERR_ELT ((st), (elt), "Bad value '%s' for attribute '%s'%s",       \
              (val), (attr), (explain != nullptr)                       \
              ? (" (" + string (explain) + ")").c_str () : "")

#define ST_ERR_ELT_UNKNOWN_CHILD(st, elt, child)\
  ST_ERR_ELT ((st), (elt), "Unknown child <%s>", (child))

// Generates unique id.
static string
st_gen_id (ParserState *st)
{
  return xstrbuild (".unamed-%d", st->genid++);
}

// Index object map.
static bool
st_objmap_index (ParserState *st, const string &id, Object **obj)
{
  MAP_GET_IMPL (st->objMap, id, obj);
}


// NCL syntax.

// Element push function.
typedef bool (ParserPushFunc) (ParserState *, xmlNode *,
                               map<string, string> *, Object **);
// Element pop function.
typedef bool (ParserPopFunc) (ParserState *, xmlNode *,
                              map<string, string> *, list<xmlNode *> *,
                              Object *);
// Attribute info.
typedef struct ParserAttrData
{
  string name;                  // attribute name
  bool required;                // whether attribute is required
} ParserAttrData;

// Element info.
typedef struct ParserEltData
{
  ParserPushFunc *push;              // push function
  ParserPopFunc *pop;                // pop function
  int flags;                         // processing flags
  vector<string> parents;            // possible parents
  vector<ParserAttrData> attributes; // attributes
} ParserEltData;

// Element processing flags.
typedef enum
{
  PARSER_ELT_FLAG_CACHE  = 1<<0, // cache element
  PARSER_ELT_FLAG_GEN_ID = 2<<0, // generate id if not present
} ParserEltFlag;

// Forward declarations.
#define PARSER_ELT_PUSH_DECL(elt)                       \
  static bool G_PASTE (parser_push_, elt)               \
    (ParserState *, xmlNode *, map<string, string> *,   \
     Object **);

#define PARSER_ELT_POP_DECL(elt)                        \
  static bool G_PASTE (parser_pop_, elt)                \
    (ParserState *, xmlNode *, map<string, string> *,   \
     list<xmlNode *> *, Object *);

PARSER_ELT_PUSH_DECL (ncl)
PARSER_ELT_POP_DECL  (ncl)
PARSER_ELT_PUSH_DECL (context)
PARSER_ELT_POP_DECL  (context)
PARSER_ELT_PUSH_DECL (media)
PARSER_ELT_POP_DECL  (media)


static map<string, ParserEltData> parser_eltmap =
{
 {"ncl",                        // element name
  {parser_push_ncl,             // push function
   parser_pop_ncl,              // pop function
   0,                           // flags
   {},                          // possible parents
   {{"id", false},              // attributes (name, required)
    {"title", false},
    {"xmlns", false}}},
 },
 //
 // Head.
 //
 {"head",
  {nullptr, nullptr,
   0,
   {"ncl"}, {}},
 },
 //
 // Body.
 //
 {"body",                       // -> Context
  {parser_push_context,
   parser_pop_context,
   0,
   {"ncl"},
   {{"id", false}}},
 },
 {"media",                      // -> Media
  {parser_push_media,
   nullptr,
   0,
   {"body", "context", "switch"},
   {{"id", true},
    {"src", false},
    {"type", false},
    {"descriptor", false}}},
 },
};

// Indexes element map.
static bool
parser_eltmap_index (const string &tag, ParserEltData **result)
{
  map<string, ParserEltData>::iterator it;
  if ((it = parser_eltmap.find (tag)) == parser_eltmap.end ())
    return false;
  tryset (result, &it->second);
  return true;
}

// Gets possible children of a given element.
static map<string, bool>
parser_eltmap_get_possible_children (const string &tag)
{
  map<string, bool> result;
  for (auto it: parser_eltmap)
    for (auto parent: it.second.parents)
      if (parent == tag)
        result[it.first] = true;
  return result;
}


// NCL attribute map helper functions.

static inline bool
parser_attrmap_index (map<string, string> *attr, const string &name,
                      string *result)
{
  MAP_GET_IMPL (*attr, name, result);
}

static inline string
parser_attrmap_get (map<string, string> *attr, const string &name)
{
  string value;
  g_assert (parser_attrmap_index (attr, name, &value));
  return value;
}

static inline string
parser_attrmap_opt_get (map<string, string> *attr,
                        const string &name, const string &defvalue)
{
  string result;
  return parser_attrmap_index (attr, name, &result) ? result : defvalue;
}


// Parse <ncl>.

static bool
parser_push_ncl (ParserState *st, unused (xmlNode *elt),
                 unused (map<string, string> *attr),
                 Object **result)
{
  g_assert_null (st->root);
  st->root = new Context (parser_attrmap_opt_get (attr, "id", "__ncl__"));
  *result = st->root;
  return true;
}

static bool
parser_pop_ncl (unused (ParserState *st), unused (xmlNode *elt),
                unused (map<string, string> *attr),
                unused (list<xmlNode *> *children),
                unused (Object *object))
{
  return true;
}


// Parse <context>.

static bool
parser_push_context (ParserState *st,
                     xmlNode *elt,
                     map<string, string> *attr,
                     Object **result)
{
  Object *ctx;
  string id;
  list<string> *ports;

  id = parser_attrmap_opt_get (attr, "id", st->root->getId ());
  if (toString (elt->name) == "body")
    {
      g_assert (st->objStack.size () == 1);
      ctx = cast (Context *, st->objStack.back ());
      g_assert_nonnull (ctx);
    }
  else
    {
      Composition *parent;

      parent = cast (Composition *, st->objStack.back ());
      g_assert_nonnull (parent);

      ctx = new Context (id);
      parent->addChild (ctx);
    }

  // Create port list.
  ports = new list<string> ();
  g_assert (ctx->setData ("ports", ports));

  // Push context onto stack.
  *result = ctx;

  return true;
}

static bool
parser_pop_context (unused (ParserState *st), unused (xmlNode *elt),
                    unused (map<string, string> *attr),
                    unused (list<xmlNode *> *children),
                    Object *object)
{
  Context *ctx;
  list<string> *ports;

  ctx = cast (Context *, object);
  g_assert_nonnull (ctx);

  // TODO: Resolve port's references.

  // Destroy port list.
  g_assert (ctx->getData ("ports", (void **) &ports));
  delete ports;

  return true;
}


// Parse <media>.

static bool
parser_push_media (ParserState *st,
                   unused (xmlNode *elt),
                   map<string, string> *attr,
                   Object **result)
{
  Composition *parent;
  Media *media;
  string id;
  string type;

  id = parser_attrmap_get (attr, "id");

  if (parser_attrmap_index (attr, "refer", nullptr))
    g_assert_not_reached ();    // TODO

  if (parser_attrmap_index (attr, "type", &type)
      && type == "application/x-ginga-settings")
    {
      media = new MediaSettings (id);
    }
  else
    {
      string src = "";
      if (parser_attrmap_index (attr, "src", &src)
          && !xpathisuri (src) && !xpathisabs (src))
        {
          string dir;
          if (st->doc->URL == nullptr)
            dir = "";
          else
            dir = xpathdirname (toString (st->doc->URL));
          src = xpathbuildabs (dir, src);
        }
      media = new Media (id, type, src);
    }

  parent = cast (Composition *, st->objStack.back ());
  g_assert_nonnull (parent);
  parent->addChild (media);

  *result = media;              // push onto stack
  return true;
}


// Internal functions.

static bool
processElt (ParserState *st, xmlNode *elt)
{
  string tag;
  ParserEltData *edata;
  bool status;

  map<string, string> _attr;
  map<string, string> *attr = &_attr;
  Object *object;
  map<string, bool> possible;
  list<xmlNode *> children;

  status = true;
  tag = toString (elt->name);
  if (unlikely (!parser_eltmap_index (tag, &edata)))
    {
      status = ST_ERR_ELT_UNKNOWN (st, elt);
      goto done;
    }

  // Check parent.
  g_assert_nonnull (elt->parent);
  if (edata->parents.size () > 0)
    {
      string parent;
      bool found;

      if (unlikely (elt->parent->type != XML_ELEMENT_NODE))
        {
          status = ST_ERR_ELT_MISSING_PARENT (st, elt);
          goto done;
        }

      parent = toString (elt->parent->name);
      found = false;
      for (auto par: edata->parents)
        {
          if (parent == par)
            {
              found = true;
              break;
            }
        }
      if (unlikely (!found))
        {
          status = ST_ERR_ELT_BAD_PARENT
            (st, elt, toCString (elt->parent->name));
          goto done;
        }
    }

  // Store attributes in attr-map.
  for (auto adata: edata->attributes)
    {
      string value;
      if (!xmlGetPropAsString (elt, adata.name, &value))
        {
          if (adata.name == "id" && edata->flags & PARSER_ELT_FLAG_GEN_ID)
            {
              (*attr)["id"] = st_gen_id (st);
              continue;
            }
          if (!adata.required)
            {
              continue;
            }
          status = ST_ERR_ELT_MISSING_ATTR (st, elt, adata.name.c_str ());
          goto done;
        }
      (*attr)[adata.name] = value;
    }

  // Check for unknown attributes.
  for (xmlAttr *prop = elt->properties; prop != nullptr; prop = prop->next)
    {
      string name = toString (prop->name);
      if (unlikely (attr->find (name) == attr->end ()))
        {
          status = ST_ERR_ELT_UNKNOWN_ATTR (st, elt, name.c_str ());
          goto done;
        }
    }

  // Collect id.
  if (parser_attrmap_index (attr, "id", nullptr))
    {
      string id;
      const char *str;
      char c;

      id = parser_attrmap_get (attr, "id");

      // Check if id is valid.
      str = id.c_str ();
      while ((c = *str++) != '\0')
        {
          if (unlikely (!(isalnum (c) || c == '-' || c == '_'
                          || c == ':' || c == '.')))
            {
              string explain = xstrbuild ("must not contain '%c'", c);
              status = ST_ERR_ELT_BAD_ATTR
                (st, elt, "id", id.c_str (), explain.c_str ());
              goto done;
            }
        }

      // Check if id is unique.
      if (unlikely (st_objmap_index (st, id, nullptr)))
        {
          status = ST_ERR_ELT_BAD_ATTR
            (st, elt, "id", id.c_str (), "duplicated id");
          goto done;
        }

      // Insert attr-map and element's node into cache.
      if (edata->flags & PARSER_ELT_FLAG_CACHE)
        {
          st->cachedAttrs[id] = map<string, string> (_attr);
          st->cachedElts[id] = elt;
          attr = &st->cachedAttrs[id];
        }
    }
  else
    {
      g_assert_false (edata->flags & PARSER_ELT_FLAG_CACHE);
    }

  // Push element.
  object = nullptr;
  if (unlikely (edata->push && !edata->push (st, elt, attr, &object)))
    {
      status = false;
      goto done;
    }

  // Push newly created entity onto entity stack.
  if (object != nullptr)
    st->objStack.push_back (object);

  // Collect children.
  possible = parser_eltmap_get_possible_children (tag);
  for (xmlNode *child = elt->children; child; child = child->next)
    {
      if (child->type != XML_ELEMENT_NODE)
        continue;

      string child_tag = toString (child->name);
      if (unlikely (possible.find (child_tag) == possible.end ()))
        {
          status = ST_ERR_ELT_UNKNOWN_CHILD (st, elt, child->name);
          goto done;
        }

      if (unlikely (!processElt (st, child)))
        {
         status = false;
         goto done;
        }

      children.push_back (child);
    }

  // Pop element.
  if (edata->pop)
    status = edata->pop (st, elt, attr, &children, object);

  // Pop object stack.
  if (object != nullptr)
    st->objStack.pop_back ();

 done:
  return status;
}

static set<Object *> *
processDoc (xmlDoc *doc, int width, int height, string *errmsg)
{
  ParserState st;
  xmlNode *root;

  g_assert_nonnull (doc);
  st.doc = doc;

  g_assert_cmpint (width, >, 0);
  g_assert_cmpint (height, >, 0);
  st.rect = {0, 0, width, height};
  st.genid = 0;
  st.objects = new set<Object *> ();
  st.root = nullptr;

  root = xmlDocGetRootElement (doc);
  g_assert_nonnull (root);

  if (!processElt (&st, root))
    {
      tryset (errmsg, st.errmsg);
      if (st.objects != nullptr)
        delete st.objects;
      return nullptr;
    }

  g_assert_nonnull (st.objects);
  return st.objects;
}


// External API.

set<Object *> *
Parser::parseBuffer (const void *buf, size_t size,
                     int width, int height, string *errmsg)
{
# define FLAGS (XML_PARSE_NOERROR | XML_PARSE_NOWARNING)
  xmlDoc *doc;
  set<Object *> *objs;

  doc = xmlReadMemory ((const char *) buf, (int) size,
                       nullptr, nullptr, FLAGS);
  if (unlikely (doc == nullptr))
    {
      xmlError *err = xmlGetLastError ();
      g_assert_nonnull (err);
      tryset (errmsg, "XML error: " + string (err->message));
      return nullptr;
    }

  objs = processDoc (doc, width, height, errmsg);
  xmlFreeDoc (doc);
  return objs;
}

set<Object *> *
Parser::parseFile (const string &path, int width, int height,
                   string *errmsg)
{
  xmlDoc *doc;
  set<Object *> *objs;

  doc = xmlReadFile (path.c_str (), nullptr, FLAGS);
  if (unlikely (doc == nullptr))
    {
      xmlError *err = xmlGetLastError ();
      g_assert_nonnull (err);
      tryset (errmsg, "XML error: " + string (err->message));
      return nullptr;
    }

  objs = processDoc (doc, width, height, errmsg);
  xmlFreeDoc (doc);
  return objs;
}


GINGA_NAMESPACE_END
