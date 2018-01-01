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
#include "ParserLibXML.h"
#include "Ncl.h"

#include <libxml/tree.h>
#include <libxml/parser.h>

GINGA_NAMESPACE_BEGIN

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

typedef map<string, map<string, string>> ParserLibXML_Cache;
typedef struct ParserLibXML_State
{
  Rect rect;                    // screen dimensions
  xmlDoc *doc;                       // DOM tree
  NclDocument *ncl;                  // NCL tree
  vector<NclEntity *> stack;            // NCL entity stack
  string errmsg;                     // last error message
  int genid;                         // last generated id
  ParserLibXML_Cache cache;          // attrmap indexed by id
  map<string, xmlNode *> cacheelt;   // xmlNode indexed by id
  map<string, set<string>> cacheset; // cached ids indexed by tag
  map<string, void *> userdata;      // userdata attached to state
} ParserLibXML_State;

static inline G_GNUC_PRINTF (2,3) void
_st_err (ParserLibXML_State *st, const char *fmt, ...)
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

#if 0
#define ST_ERR_ELT_MISSING_CHILD(st, elt, child)\
  ST_ERR_ELT ((st), (elt), "Missing child <%s>", (child))
#endif

// Index state cache.
static bool
st_cache_index (ParserLibXML_State *st, const string &key,
                map<string, string> **result)
{
  ParserLibXML_Cache::iterator it;
  if ((it = st->cache.find (key)) == st->cache.end ())
    return false;
  tryset (result, &it->second);
  return true;
}

// Gets userdata attached to state.
static void *
st_get_data (ParserLibXML_State *st, const string &key)
{
  if (st->userdata.find (key) == st->userdata.end ())
    return nullptr;
  return st->userdata[key];
}

// Attaches userdata to state.
static void
st_set_data (ParserLibXML_State *st, const string &key, void *value)
{
  st->userdata[key] = value;
}

// Generates unique id.
static string
st_gen_id (ParserLibXML_State *st)
{
  return xstrbuild (".unamed-%d", st->genid++);
}

// Resolve id-ref in current state.
static bool
st_resolve_idref (ParserLibXML_State *st, const string &id,
                  set<string> tags, xmlNode **result_elt,
                  map<string,string> **result_cache)
{
  xmlNode *elt = st->cacheelt[id];
  if (elt == nullptr
      || elt->type != XML_ELEMENT_NODE
      || tags.find (toString (elt->name)) == tags.end ())
    {
      return false;
    }
  else
    {
      tryset (result_elt, elt);
      if (result_cache)
        g_assert (st_cache_index (st, id, result_cache));
      return true;
    }
}


// NCL syntax.

// Element push function.
typedef bool (NclEltPushFunc) (ParserLibXML_State *,
                               xmlNode *,
                               map<string, string> *,
                               NclEntity **);
// Element pop function.
typedef bool (NclEltPopFunc) (ParserLibXML_State *,
                              xmlNode *,
                              map<string, string> *,
                              vector<xmlNode *> *,
                              NclEntity *);
// Attribute info.
typedef struct NclAttrInfo
{
  string name;                  // attribute name
  bool required;                // whether attribute is required
} NclAttrInfo;

// Element info.
typedef struct NclEltInfo
{
  NclEltPushFunc *push;           // push function
  NclEltPopFunc *pop;             // pop function
  int flags;                      // processing flags
  vector<string> parents;         // possible parents
  vector<NclAttrInfo> attributes; // attributes
} NclEltInfo;

// Element processing flags.
typedef enum
{
  NCL_ELT_FLAG_CACHE  = 1<<0,   // cache element
  NCL_ELT_FLAG_GEN_ID = 2<<0,   // generate id if not present
} NclEltFlag;

// Forward declarations.
#define NCL_ELT_PUSH_DECL(elt)                  \
  static bool G_PASTE (ncl_push_, elt)          \
    (ParserLibXML_State *, xmlNode *,           \
     map<string, string> *, NclEntity **);

#define NCL_ELT_POP_DECL(elt)                                   \
  static bool G_PASTE (ncl_pop_, elt)                           \
    (ParserLibXML_State *, xmlNode *,                           \
     map<string, string> *, vector<xmlNode *> *, NclEntity *);

NCL_ELT_PUSH_DECL (ncl)
NCL_ELT_POP_DECL  (ncl)
NCL_ELT_PUSH_DECL (context)
NCL_ELT_POP_DECL  (context)
NCL_ELT_PUSH_DECL (port)
NCL_ELT_PUSH_DECL (media)
NCL_ELT_PUSH_DECL (property)
NCL_ELT_PUSH_DECL (link)
NCL_ELT_PUSH_DECL (region)
NCL_ELT_POP_DECL  (region)
NCL_ELT_PUSH_DECL (descriptorParam)
NCL_ELT_PUSH_DECL (causalConnector)
NCL_ELT_POP_DECL  (causalConnector)
NCL_ELT_PUSH_DECL (simpleConditionOrAction)

// Element map.
static map<string, NclEltInfo> ncl_eltmap =
{
 // Root.
 {"ncl",                        // name
  {ncl_push_ncl, ncl_pop_ncl,   // push & pop functions
   0,                           // flags
   {},                          // possible parents
   {{"id", false},              // attributes
    {"title", false},
    {"xmlns", false}}},
 },
 //
 // Body.
 //
 {"body",                       // -> NclContext
  {ncl_push_context, ncl_pop_context,
   0,
   {"ncl"},
   {{"id", false}}},
 },
 {"context",                    // -> NclContext
  {ncl_push_context, ncl_pop_context,
   NCL_ELT_FLAG_CACHE,
   {"body", "context"},
   {{"id", true}}},
 },
 {"port",                       // -> NclPort
  {ncl_push_port, nullptr,
   NCL_ELT_FLAG_CACHE,
   {"body", "context"},
   {{"id", true},
    {"component", true},
    {"interface", false}}},
 },
 {"media",                      // -> NclMedia
  {ncl_push_media, nullptr,
   NCL_ELT_FLAG_CACHE,
   {"body", "context"},
   {{"id", true},
    {"src", false},
    {"type", false},
    {"descriptor", false}}},
 },
 {"area",
  {nullptr, nullptr,
   0,
   {"media"},
   {{"id", true},
    {"begin", false},
    {"end", false}}},
 },
 {"property",                   // -> NclProperty
  {ncl_push_property, nullptr,
   0,
   {"body", "context", "media"},
  {{"name", true},
   {"value", false}}},
 },
 {"link",                       // -> NclLink
  {ncl_push_link, nullptr,
   NCL_ELT_FLAG_CACHE | NCL_ELT_FLAG_GEN_ID,
   {"body", "context"},
   {{"id", false},
    {"xconnector", true}}},
 },
 {"bind",
  {nullptr, nullptr,
   0,
   {"link"},
   {{"role", true},
    {"component", false},
    {"interface", false}}},
 },
 {"bindParam",
  {nullptr, nullptr,
   0,
   {"bind"},
   {{"name", true},
    {"value", true}}},
 },
 //
 // Head.
 //
 {"head",
  {nullptr, nullptr,
   0,
   {"ncl"}, {}},
 },
 {"regionBase",
  {nullptr, nullptr,
   0,
   {"head"},
   {{"id", false},
    {"device", false},
    {"region", false}}},
 },
 {"region",
  {ncl_push_region, ncl_pop_region,
   NCL_ELT_FLAG_CACHE,
   {"region", "regionBase"},
   {{"id", true},
    {"title", false},
    {"left", false},
    {"right", false},
    {"top", false},
    {"bottom", false},
    {"height", false},
    {"width", false},
    {"zIndex", false}}},
 },
 {"descriptorBase",
  {nullptr, nullptr,
   0,
   {"head"},
   {{"id", false}}},
 },
 {"descriptor",
  {nullptr, nullptr,
   NCL_ELT_FLAG_CACHE,
   {"descriptorBase"},
   {{"id", true},
    {"left", false},
    {"right", false},
    {"top", false},
    {"bottom", false},
    {"width", false},
    {"height", false},
    {"zIndex", false},
    {"region", false}}},
 },
 {"descriptorParam",
  {ncl_push_descriptorParam, nullptr,
   0,
   {"descriptor"},
   {{"name", true},
    {"value", true}}},
 },
 {"connectorBase",
  {nullptr, nullptr,
   0,
   {"head"},
   {{"id", false}}},
 },
 {"causalConnector",
  {ncl_push_causalConnector, ncl_pop_causalConnector,
   NCL_ELT_FLAG_CACHE,
   {"connectorBase"},
   {{"id", true}}},
 },
 {"connectorParam",
  {nullptr, nullptr,
   0,
   {"causalConnector"},
   {{"name", true}}},
 },
 {"compoundCondition",
  {nullptr, nullptr,
   0,
   {"causalConnector", "compoundCondition"},
   {{"operator", true},
    {"delay", false}}},
 },
 {"simpleCondition",
  {ncl_push_simpleConditionOrAction, nullptr,
   0,
   {"causalConnector", "compoundCondition"},
   {{"role", true},
    {"eventType", false},
    {"key", false},
    {"transition", false},
    {"delay", false},           // ignored
    {"min", false},             // ignored
    {"max", false},             // ignored
    {"qualifier", false}}},     // ignored
 },
 {"compoundAction",
  {nullptr, nullptr,
   0,
   {"causalConnector", "compoundAction"},
   {{"operator", false},
    {"delay", false}}},
 },
 {"simpleAction",
  {ncl_push_simpleConditionOrAction, nullptr,
   0,
   {"causalConnector", "compoundAction"},
   {{"role", true},
    {"eventType", false},
    {"actionType", false},
    {"delay", false},
    {"duration", false},
    {"value", false},
    {"min", false},             // ignored
    {"max", false},             // ignored
    {"min", false},             // ignored
    {"qualifier", false},       // ignored
    {"repeat", false},          // ignored
    {"repeatDelay", false},     // ignored
    {"by", false}}},            // ignored
 },
};

// Indexes element map.
static bool
ncl_eltmap_index (const string &tag, NclEltInfo **result)
{
  map<string, NclEltInfo>::iterator it;
  if ((it = ncl_eltmap.find (tag)) == ncl_eltmap.end ())
    return false;
  tryset (result, &it->second);
  return true;
}

// Gets possible children of a given element.
static map<string, bool>
ncl_eltmap_get_possible_children (const string &tag)
{
  map<string, bool> result;
  for (auto it: ncl_eltmap)
    for (auto parent: it.second.parents)
      if (parent == tag)
        result[it.first] = true;
  return result;
}


// Attribute parsing functions.

#define _NCL_ATTR_PARSE(Type, tab, str, result)         \
  G_STMT_START                                          \
  {                                                     \
    map<string, Type>::iterator it;                     \
    if ((it = (tab).find ((str))) == (tab).end ())      \
      return false;                                     \
    tryset ((result), it->second);                      \
    return true;                                        \
  }                                                     \
  G_STMT_END

static bool
ncl_attr_parse_eventType (string str, Event::Type *result)
{
  static map<string, Event::Type> tab =
    {
     {"presentation", Event::PRESENTATION},
     {"attribution", Event::ATTRIBUTION},
     {"selection", Event::SELECTION},
    };
  _NCL_ATTR_PARSE (Event::Type, tab, str, result);
}

static bool
ncl_attr_parse_transition (string str, Event::Transition *result)
{
  static map<string, Event::Transition> tab =
    {
     {"starts", Event::START},
     {"stops", Event::STOP},
     {"aborts", Event::ABORT},
     {"pauses", Event::PAUSE},
     {"resumes", Event::RESUME},
    };
  _NCL_ATTR_PARSE (Event::Transition, tab, str, result);
}

static bool
ncl_attr_parse_actionType (string str, Event::Transition *result)
{
  static map<string, Event::Transition> tab =
    {
     {"start", Event::START},
     {"stop", Event::STOP},
     {"abort", Event::ABORT},
     {"pause", Event::PAUSE},
     {"resume", Event::RESUME},
    };
  _NCL_ATTR_PARSE (Event::Transition, tab, str, result);
}


// NCL attribute map helper functions.

static inline bool
ncl_attrmap_index (map<string, string> *attr, const string &name,
                   string *result)
{
  map<string, string>::iterator it;
  if ((it = attr->find (name)) == attr->end ())
    return false;
  tryset (result, it->second);
  return true;
}

static inline string
ncl_attrmap_get (map<string, string> *attr, const string &name)
{
  string value;
  g_assert (ncl_attrmap_index (attr, name, &value));
  return value;
}

static inline string
ncl_attrmap_opt_get (map<string, string> *attr,
                     const string &name, const string &defvalue)
{
  string result;
  return ncl_attrmap_index (attr, name, &result) ? result : defvalue;
}


// NCL element push & pop functions.

static bool
ncl_push_ncl (ParserLibXML_State *st,
              unused (xmlNode *elt),
              map<string, string> *attr,
              unused (NclEntity **entity))
{
  string id = ncl_attrmap_opt_get (attr, "id", "ncl");
  string  url = (st->doc->URL) ? toString (st->doc->URL) : "";
  st->ncl = new NclDocument (id, url);
  return true;
}

static bool
ncl_pop_ncl (unused (ParserLibXML_State *st),
             unused (xmlNode *elt),
             unused (map<string, string> *attr),
             unused (vector<xmlNode *> *children),
             unused (NclEntity *entity))
{
  bool status = true;

  // Resolve descriptor's reference to region.
  for (auto desc_id: st->cacheset["descriptor"])
    {
      map<string, string> *desc_attr;
      xmlNode *desc_elt;

      map<string, string> *reg_attr;
      string reg_id;

      g_assert (st_cache_index (st, desc_id, &desc_attr));
      if (!ncl_attrmap_index (desc_attr, "region", &reg_id))
        continue;               // nothing to do

      desc_elt = st->cacheelt[desc_id];
      g_assert_nonnull (desc_elt);

      if (unlikely (!st_resolve_idref (st, reg_id, {"region"},
                                       nullptr, &reg_attr)))
        {
          status = ST_ERR_ELT_BAD_ATTR
            (st, desc_elt, "region", reg_id.c_str (), "no such region");
          goto done;
        }

      for (auto it: *reg_attr)
        {
          if (it.first == "id")
            continue;           // nothing to do
          if (ncl_attrmap_index (desc_attr, it.first, nullptr))
            continue;           // already defined
          (*desc_attr)[it.first] = it.second;
        }
    }

  // Resolve link's reference to connector.
  for (auto link_id: st->cacheset["link"])
    {
      map<string, string> *link_attr;
      xmlNode *link_elt;
      NclLink *link;

      string conn_id;
      NclConnector *conn;

      g_assert (st_cache_index (st, link_id, &link_attr));
      link_elt = st->cacheelt[link_id];
      g_assert_nonnull (link_elt);
      link = cast (NclLink *, st->ncl->getEntityById (link_id));
      g_assert_nonnull (link);

      conn_id = ncl_attrmap_get (link_attr, "xconnector");
      if (unlikely (!st_resolve_idref (st, conn_id, {"causalConnector"},
                                       nullptr, nullptr)))
        {
          status = ST_ERR_ELT_BAD_ATTR
            (st, link_elt, "xconnector", conn_id.c_str (),
             "no such connector");
          goto done;
        }

      conn = cast (NclConnector *, st->ncl->getEntityById (conn_id));
      g_assert_nonnull (conn);
      if (unlikely (!link->initConnector (conn)))
        {
          status = ST_ERR_ELT_BAD_ATTR
            (st, link_elt, "xconnector", conn_id.c_str (),
             "link does not match connector");
          goto done;
        }
    }

  // Resolve media's reference to descriptor.
  for (auto media_id: st->cacheset["media"])
    {
      map<string, string> *media_attr;
      NclMedia *media;
      xmlNode *media_elt;
      map<string, string> *desc_attr;
      string desc_id;

      g_assert (st_cache_index (st, media_id, &media_attr));
      if (!ncl_attrmap_index (media_attr, "descriptor", &desc_id))
        continue;               // nothing to do

      media = cast (NclMedia *, st->ncl->getEntityById (media_id));
      g_assert_nonnull (media);

      media_elt = st->cacheelt[media_id];
      g_assert_nonnull (media_elt);

      if (unlikely (!st_resolve_idref (st, desc_id, {"descriptor"},
                                       nullptr, &desc_attr)))
        {
          status = ST_ERR_ELT_BAD_ATTR
            (st, media_elt, "descriptor", desc_id.c_str (),
             "no such descriptor");
          goto done;
        }

      for (auto it: *desc_attr)
        {
          if (it.first == "id" && it.first == "region")
            continue;           // nothing to do
          if (media->hasProperty (it.first))
            continue;           // already defined
          media->setProperty (it.first, it.second);
        }
    }

 done:
  return status;
}

static bool
ncl_push_context (ParserLibXML_State *st,
                  xmlNode *elt,
                  map<string, string> *attr,
                  NclEntity **entity)
{
  NclContext *context;
  string id;

  id = ncl_attrmap_opt_get (attr, "id", st->ncl->getId ());
  if (toString (elt->name) == "body")
    {
      context = st->ncl->getRoot ();
    }
  else
    {
      NclComposition *parent = cast (NclComposition *, st->stack.back ());
      g_assert_nonnull (parent);
      context = new NclContext (st->ncl, id);
      parent->addNode (context);
    }
  g_assert_nonnull (context);
  *entity = context;            // push onto stack
  return true;
}

static bool
ncl_pop_context (unused (ParserLibXML_State *st),
                 unused (xmlNode *elt),
                 unused (map<string, string> *attr),
                 unused (vector<xmlNode *> *children),
                 unused (NclEntity *entity))
{
  NclContext *context;

  context = cast (NclContext *, entity);
  g_assert_nonnull (context);

  // Resolve port's references.
  for (auto port: *context->getPorts ())
    {
      map<string, string> *port_attr;
      xmlNode *port_elt;
      string port_id;
      string comp_id;
      string iface_id;

      NclNode *node;
      NclAnchor *anchor;

      port_id = port->getId ();
      g_assert (st_cache_index (st, port_id, &port_attr));

      port_elt = st->cacheelt[port_id];
      g_assert_nonnull (port_elt);

      comp_id = ncl_attrmap_get (port_attr, "component");
      if (unlikely (!st_resolve_idref (st, comp_id, {"context", "media"},
                                       nullptr, nullptr)))
        {
          return ST_ERR_ELT_BAD_ATTR
            (st, port_elt, "component", comp_id.c_str (),
             "no such component");
        }

      node = cast (NclNode *, st->ncl->getEntityById (comp_id));
      g_assert_nonnull (node);
      if (unlikely (node->getParent () != port->getParent ()))
        {
          return ST_ERR_ELT_BAD_ATTR
            (st, port_elt, "component", comp_id.c_str (),
             "no such component in scope");
        }
      port->initNode (node);

      anchor = nullptr;
      if (ncl_attrmap_index (port_attr, "interface", &iface_id))
        {
          anchor = node->getAnchor (iface_id);
          if (unlikely (anchor == nullptr))
            {
              return ST_ERR_ELT_BAD_ATTR
                (st, port_elt, "interface", iface_id.c_str (),
                 "no such interface");
            }
        }
      else
        {
          anchor = node->getLambda ();
        }
      port->initInterface (anchor);
    }
  return true;
}

static bool
ncl_push_port (ParserLibXML_State *st,
               unused (xmlNode *elt),
               map<string, string> *attr,
               unused (NclEntity **entity))
{
  NclPort *port;
  NclContext *context;

  port = new NclPort (st->ncl, ncl_attrmap_get (attr, "id"));
  context = cast (NclContext *, st->stack.back ());
  g_assert_nonnull (context);
  context->addPort (port);
  return true;
}

static bool
ncl_push_media (ParserLibXML_State *st,
                unused (xmlNode *elt),
                map<string, string> *attr,
                unused (NclEntity **entity))
{
  NclMedia *media;
  NclComposition *comp;

  media = new NclMedia (st->ncl, ncl_attrmap_get (attr, "id"), false);
  if (ncl_attrmap_index (attr, "src", nullptr))
    {
      string src = ncl_attrmap_get (attr, "src");
      if (!xpathisuri (src) && !xpathisabs (src))
        {
          string dir;
          if (st->doc->URL == nullptr)
            dir = "";
          else
            dir = xpathdirname (toString (st->doc->URL));
          src = xpathbuildabs (dir, src);
        }
      media->setSrc (src);
    }

  comp = cast (NclComposition *, st->stack.back ());
  g_assert_nonnull (comp);
  comp->addNode (media);
  *entity = media;              // push onto stack
  return true;
}

static bool
ncl_push_property (ParserLibXML_State *st,
                   unused (xmlNode *elt),
                   map<string, string> *attr,
                   unused (NclEntity **entity))
{
  NclNode *parent = cast (NclNode *, st->stack.back ());
  g_assert_nonnull (parent);
  parent->setProperty (ncl_attrmap_get (attr, "name"),
                       ncl_attrmap_opt_get (attr, "value", ""));
  return true;
}

static bool
ncl_push_link (ParserLibXML_State *st,
               unused (xmlNode *elt),
               map<string, string> *attr,
               NclEntity **entity)
{
  NclLink *link;
  NclContext *ctx;

  link = new NclLink (st->ncl, ncl_attrmap_get (attr, "id"));
  ctx = cast (NclContext *, st->stack.back ());
  g_assert_nonnull (ctx);
  ctx->addLink (link);
  *entity = link;               // push onto stack
  return true;
}

static bool
ncl_push_region (ParserLibXML_State *st,
                 unused (xmlNode *elt),
                 map<string, string> *attr,
                 unused (NclEntity **entity))
{
  static int last_zorder = 0;
  Rect screen_rect;
  Rect parent_rect;
  Rect rect;
  string value;

  g_assert_nonnull (elt->parent);
  if (toString (elt->parent->name) != "region") // root region
    {
      Rect *saved_rect = new Rect;
      *saved_rect = screen_rect = st->rect;
      st_set_data (st, "saved_rect", saved_rect);
    }
  else
    {
      Rect *saved_rect = (Rect *) st_get_data (st, "saved_rect");
      screen_rect = *saved_rect;
    }

  rect = parent_rect = st->rect;
  if (ncl_attrmap_index (attr, "left", &value))
    {
      rect.x += ginga_parse_percent (value, parent_rect.width, 0, G_MAXINT);
    }
  if (ncl_attrmap_index (attr, "top", &value))
    {
      rect.y += ginga_parse_percent
        (value, parent_rect.height, 0, G_MAXINT);
    }
  if (ncl_attrmap_index (attr, "width", &value))
    {
      rect.width = ginga_parse_percent
        (value, parent_rect.width, 0, G_MAXINT);
    }
  if (ncl_attrmap_index (attr, "height", &value))
    {
      rect.height = ginga_parse_percent
        (value, parent_rect.height, 0, G_MAXINT);
    }
  if (ncl_attrmap_index (attr, "right", &value))
    {
      rect.x += parent_rect.width - rect.width
        - ginga_parse_percent (value, parent_rect.width, 0, G_MAXINT);
    }
  if (ncl_attrmap_index (attr, "bottom", &value))
    {
      rect.y += parent_rect.height - rect.height
        - ginga_parse_percent (value, parent_rect.height, 0, G_MAXINT);
    }

  // FIXME: Handle zIndex.
  // FIXME: Do we need to export zorder?

  st->rect = rect;
  (*attr)["zorder"] = xstrbuild ("%d", last_zorder++);
  (*attr)["left"] = xstrbuild
    ("%.2f%%", ((double) rect.x / screen_rect.width) * 100.);
  (*attr)["top"] = xstrbuild
    ("%.2f%%", ((double) rect.y / screen_rect.height) * 100.);
  (*attr)["width"] = xstrbuild
    ("%.2f%%", ((double) rect.width / screen_rect.width) * 100.);
  (*attr)["height"] = xstrbuild
    ("%.2f%%", ((double) rect.height / screen_rect.height) * 100.);

  return true;
}

static bool
ncl_pop_region (ParserLibXML_State *st,
                unused (xmlNode *elt),
                unused (map<string, string> *attr),
                unused (vector<xmlNode *> *children),
                unused (NclEntity *entity))
{
  g_assert_nonnull (elt->parent);
  if (toString (elt->parent->name) != "region") // root region
    {
      Rect *saved_rect;
      saved_rect = (Rect *) st_get_data (st, "saved_rect");
      g_assert_nonnull (saved_rect);
      st->rect = *saved_rect;
      delete saved_rect;
    }
  return true;
}

static bool
ncl_push_descriptorParam (ParserLibXML_State *st,
                          xmlNode *elt,
                          map<string, string> *attr,
                          unused (NclEntity **entity))
{
  string desc_id;
  map<string, string> *desc;
  string name;
  string value;

  g_assert (xmlGetPropAsString (elt->parent, "id", &desc_id));
  g_assert (st_cache_index (st, desc_id, &desc));
  name = ncl_attrmap_get (attr, "name");
  value = ncl_attrmap_get (attr, "value");
  (*desc)[name] = value;
  return true;
}

static bool
ncl_push_causalConnector (ParserLibXML_State *st,
                          unused (xmlNode *elt),
                          map<string, string> *attr,
                          NclEntity **entity)
{
  NclConnector *conn;
  conn = new NclConnector (st->ncl, ncl_attrmap_get (attr, "id"));
  *entity = conn;               // push onto stack
  return true;
}

static bool
ncl_pop_causalConnector (ParserLibXML_State *st,
                         xmlNode *elt,
                         unused (map<string, string> *attr),
                         unused (vector<xmlNode *> *children),
                         NclEntity *entity)
{
  NclConnector *conn;

  conn = cast (NclConnector *, entity);
  g_assert_nonnull (conn);

  (void) st;
  (void) elt;

  // if (unlikely (conn->getCondition () == nullptr))
  //   return ST_ERR_ELT_MISSING_CHILD (st, elt, "simpleCondition");
  // if (unlikely ((conn->getActions ())->size ()  == 0))
  //   return ST_ERR_ELT_MISSING_CHILD (st, elt, "simpleAction");

  return true;
}

static bool
ncl_push_simpleConditionOrAction (ParserLibXML_State *st,
                                  xmlNode *elt,
                                  map<string, string> *attr,
                                  unused (NclEntity **entity))
{
  string role;
  Event::Type type;
  Event::Transition trans;
  NclConnector *parent;

  bool is_cond;
  bool reserved;
  string transname;

  role = ncl_attrmap_get (attr, "role");
  is_cond = (toString (elt->name) == "simpleCondition") ? true : false;

  if (is_cond)
    {
      reserved = NclCondition::isReserved (role, &type, &trans);
      transname = "transition";
    }
  else
    {
      reserved = NclAction::isReserved (role, &type, &trans);
      transname = "actionType";
    }

  if (reserved)
    {
      string str;
      if (unlikely (ncl_attrmap_index (attr, "eventType", &str)))
        return ST_ERR_ELT_BAD_ATTR
          (st, elt, "eventType", str.c_str (), "reserved role" );
      if (unlikely (ncl_attrmap_index (attr, transname, &str)))
        return ST_ERR_ELT_BAD_ATTR
          (st, elt, transname.c_str (), str.c_str (), "reserved role");
    }
  else
    {
      string str;
      if (unlikely (!ncl_attrmap_index (attr, "eventType", &str)))
        return ST_ERR_ELT_MISSING_ATTR (st, elt, "eventType");
      if (unlikely (!ncl_attr_parse_eventType (str, &type)))
        return ST_ERR_ELT_BAD_ATTR
          (st, elt, "eventType", str.c_str (), "invalid eventType");
      if (unlikely (!ncl_attrmap_index (attr, transname, &str)))
        return ST_ERR_ELT_MISSING_ATTR (st, elt, transname.c_str ());
      if (is_cond)
        {
          if (unlikely (!ncl_attr_parse_transition (str, &trans)))
            return ST_ERR_ELT_BAD_ATTR
              (st, elt, "transition", str.c_str (), "invalid transition");
        }
      else
        {
          if (unlikely (!ncl_attr_parse_actionType (str, &trans)))
            return ST_ERR_ELT_BAD_ATTR
              (st, elt, "actionType", str.c_str (), "invalid actionType");
        }
    }

  parent = cast (NclConnector *, st->stack.back ());
  g_assert_nonnull (parent);
  if (is_cond)
    {
      string key;
      NclCondition *cond;
      key = ncl_attrmap_opt_get (attr, "key", "");
      cond = new NclCondition (type, trans, nullptr, role, key);
      parent->addCondition (cond);
    }
  else
    {
      string delay;
      string value;
      string duration;
      NclAction *act;
      delay = ncl_attrmap_opt_get (attr, "delay", "");
      value = ncl_attrmap_opt_get (attr, "key", "");
      duration = ncl_attrmap_opt_get (attr, "duration", "");
      act = new NclAction (type, trans, role, delay, value, duration);
      parent->addAction (act);
    }
  return true;
}


// External API.

static bool
processElt (ParserLibXML_State *st, xmlNode *elt)
{
  string tag;
  NclEltInfo *einfo;
  bool status;

  map<string, string> _attr;
  map<string, string> *attr = &_attr;
  NclEntity *entity;
  map<string, bool> possible;
  vector<xmlNode *> children;

  status = true;
  tag = toString (elt->name);
  if (unlikely (!ncl_eltmap_index (tag, &einfo)))
    {
      status = ST_ERR_ELT_UNKNOWN (st, elt);
      goto done;
    }

  // Check parent.
  g_assert_nonnull (elt->parent);
  if (einfo->parents.size () > 0)
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
      for (auto par: einfo->parents)
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
  for (auto ainfo: einfo->attributes)
    {
      string value;
      if (!xmlGetPropAsString (elt, ainfo.name, &value))
        {
          if (ainfo.name == "id" && einfo->flags & NCL_ELT_FLAG_GEN_ID)
            {
              (*attr)["id"] = st_gen_id (st);
              continue;
            }
          if (!ainfo.required)
            {
              continue;
            }
          status = ST_ERR_ELT_MISSING_ATTR (st, elt, ainfo.name.c_str ());
          goto done;
        }
      (*attr)[ainfo.name] = value;
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
  if (ncl_attrmap_index (attr, "id", nullptr))
    {
      string id;
      const char *str;
      char c;

      id = ncl_attrmap_get (attr, "id");

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
      if (unlikely (st->ncl != nullptr
                    && st->ncl->getEntityById (id) != nullptr))
        {
          status = ST_ERR_ELT_BAD_ATTR
            (st, elt, "id", id.c_str (), "duplicated id");
          goto done;
        }

      // Insert attr-map and element's node into cache.
      if (einfo->flags & NCL_ELT_FLAG_CACHE)
        {
          st->cache[id] = map<string, string> (_attr);
          st->cacheelt[id] = elt;
          st->cacheset[tag].insert (id);
          attr = &st->cache[id];
        }
    }
  else
    {
      g_assert_false (einfo->flags & NCL_ELT_FLAG_CACHE);
    }

  // Push element.
  entity = nullptr;
  if (unlikely (einfo->push && !einfo->push (st, elt, attr, &entity)))
    {
      status = false;
      goto done;
    }

  // Push newly created entity onto entity stack.
  if (entity != nullptr)
    st->stack.push_back (entity);

  // Collect children.
  possible = ncl_eltmap_get_possible_children (tag);
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
  if (einfo->pop)
    status = einfo->pop (st, elt, attr, &children, entity);

  // Pop entity stack.
  if (entity != nullptr)
    st->stack.pop_back ();

 done:
  return status;
}

static NclDocument *
processDoc (xmlDoc *doc, int width, int height, string *errmsg)
{
  ParserLibXML_State st;
  xmlNode *root;

  g_assert_cmpint (width, >, 0);
  g_assert_cmpint (height, >, 0);
  st.rect = {0, 0, width, height};
  st.ncl = nullptr;
  st.doc = doc;
  st.genid = 0;

  root = xmlDocGetRootElement (doc);
  g_assert_nonnull (root);

  if (!processElt (&st, root))
    {
      tryset (errmsg, st.errmsg);
      if (st.ncl != nullptr)
        delete st.ncl;
      return nullptr;
    }

  g_assert_nonnull (st.ncl);
  return st.ncl;
}

NclDocument *
ParserLibXML::parseBuffer (const void *buf, size_t size,
                           int width, int height, string *errmsg)
{
# define FLAGS (XML_PARSE_NOERROR | XML_PARSE_NOWARNING)
  xmlDoc *doc;
  NclDocument *ncl;

  doc = xmlReadMemory ((const char *) buf, (int) size,
                       nullptr, nullptr, FLAGS);
  if (unlikely (doc == nullptr))
    {
      xmlError *err = xmlGetLastError ();
      g_assert_nonnull (err);
      tryset (errmsg, "XML error: " + string (err->message));
      return nullptr;
    }

  ncl = processDoc (doc, width, height, errmsg);
  xmlFreeDoc (doc);
  return ncl;
}

NclDocument *
ParserLibXML::parseFile (const string &path, int width, int height,
                         string *errmsg)
{
  xmlDoc *doc;
  NclDocument *ncl;

  doc = xmlReadFile (path.c_str (), nullptr, FLAGS);
  if (unlikely (doc == nullptr))
    {
      xmlError *err = xmlGetLastError ();
      g_assert_nonnull (err);
      tryset (errmsg, "XML error: " + string (err->message));
      return nullptr;
    }

  ncl = processDoc (doc, width, height, errmsg);
  xmlFreeDoc (doc);
  return ncl;
}

GINGA_NAMESPACE_END
