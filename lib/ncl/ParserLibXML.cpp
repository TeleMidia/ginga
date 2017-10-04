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
#include "ParserLibXML.h"
#include "Ncl.h"

#include <libxml/tree.h>
#include <libxml/parser.h>

GINGA_NCL_BEGIN

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
  GingaRect rect;                    // screen dimensions
  xmlDoc *doc;                       // DOM tree
  NclDocument *ncl;                  // NCL tree
  vector<Entity *> stack;            // NCL entity stack
  string errmsg;                     // last error message
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

#define ST_ERR_ELT_MISSING_CHILD(st, elt, child)\
  ST_ERR_ELT ((st), (elt), "Missing child <%s>", (child))

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
                               Entity **);
// Element pop function.
typedef bool (NclEltPopFunc) (ParserLibXML_State *,
                              xmlNode *,
                              map<string, string> *,
                              vector<xmlNode *> *,
                              Entity *);
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
  bool cache;                     // whether to cache it
  vector<string> parents;         // possible parents
  vector<NclAttrInfo> attributes; // attributes
} NclEltInfo;

// Forward declarations.
#define NCL_ELT_PUSH_DECL(elt)                  \
  static bool G_PASTE (ncl_push_, elt)          \
    (ParserLibXML_State *, xmlNode *,           \
     map<string, string> *, Entity **);

#define NCL_ELT_POP_DECL(elt)                                   \
  static bool G_PASTE (ncl_pop_, elt)                           \
    (ParserLibXML_State *, xmlNode *,                           \
     map<string, string> *, vector<xmlNode *> *, Entity *);

NCL_ELT_PUSH_DECL (ncl)
NCL_ELT_POP_DECL  (ncl)
NCL_ELT_PUSH_DECL (context)
NCL_ELT_POP_DECL  (context)
NCL_ELT_PUSH_DECL (port)
NCL_ELT_PUSH_DECL (media)
NCL_ELT_PUSH_DECL (property)
NCL_ELT_PUSH_DECL (region)
NCL_ELT_POP_DECL  (region)
NCL_ELT_PUSH_DECL (descriptorParam)
NCL_ELT_PUSH_DECL (causalConnector)
NCL_ELT_POP_DECL  (causalConnector)
NCL_ELT_PUSH_DECL (simpleCondition)
NCL_ELT_PUSH_DECL (simpleAction)

// Element map.
static map<string, NclEltInfo> ncl_eltmap =
{
 // Root.
 {"ncl",
  {ncl_push_ncl, ncl_pop_ncl, false,
   {},
   {{"id", false},
    {"title", false},
    {"xmlns", false}}},
 },
 //
 // Body.
 //
 {"body",                       // -> Context
  {ncl_push_context, ncl_pop_context, false,
   {"ncl"},
   {{"id", false}}},
 },
 {"context",                    // -> Context
  {ncl_push_context, ncl_pop_context, true,
   {"body", "context"},
   {{"id", true}}},
 },
 {"port",                       // -> Port
  {ncl_push_port, nullptr, true,
   {"body", "context"},
   {{"id", true},
    {"component", true},
    {"interface", false}}},
 },
 {"media",                      // -> Media
  {ncl_push_media, nullptr, true,
   {"body", "context"},
   {{"id", true},
    {"src", false},
    {"type", false},
    {"descriptor", false}}},
 },
 {"area",
  {nullptr, nullptr, false,
   {"media"},
   {{"id", true},
    {"begin", false},
    {"end", false}}},
 },
 {"property",                   // -> Property
  {ncl_push_property, nullptr, false,
   {"body", "context", "media"},
  {{"name", true},
   {"value", false}}},
 },
 {"link",
  {nullptr, nullptr, false,
   {"body", "context"},
   {{"id", false},
    {"xconnector", true}}},
 },
 {"bind",
  {nullptr, nullptr, false,
   {"link"},
   {{"role", true},
    {"component", false},
    {"interface", false}}},
 },
 {"bindParam",
  {nullptr, nullptr, false,
   {"bind"},
   {{"name", true},
    {"value", true}}},
 },
 //
 // Head.
 //
 {"head",
  {nullptr, nullptr, false,
   {"ncl"}, {}},
 },
 {"regionBase",
  {nullptr, nullptr, false,
   {"head"},
   {{"id", false},
    {"device", false},
    {"region", false}}},
 },
 {"region",
  {ncl_push_region, ncl_pop_region, true,
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
  {nullptr, nullptr, false,
   {"head"},
   {{"id", false}}},
 },
 {"descriptor",
  {nullptr, nullptr, true,
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
  {ncl_push_descriptorParam, nullptr, false,
   {"descriptor"},
   {{"name", true},
    {"value", true}}},
 },
 {"connectorBase",
  {nullptr, nullptr, false, {"head"},
   {{"id", false}}},
 },
 {"causalConnector",
  {ncl_push_causalConnector, ncl_pop_causalConnector, false,
   {"connectorBase"},
   {{"id", true}}},
 },
 {"connectorParam",
  {nullptr, nullptr, false,
   {"causalConnector"},
   {{"name", true}}},
 },
 {"compoundCondition",
  {nullptr, nullptr, false,
   {"causalConnector", "compoundCondition"},
   {{"operator", true},
    {"delay", false}}},
 },
 {"simpleCondition",
  {ncl_push_simpleCondition, nullptr, false,
   {"causalConnector", "compoundCondition"},
   {{"role", true},
    {"delay", false},
    {"eventType", false},
    {"key", false},
    {"transition", false},
    {"min", false},
    {"max", false},
    {"qualifier", false}}},
 },
 {"compoundAction",
  {nullptr, nullptr, false,
   {"causalConnector", "compoundAction"},
   {{"operator", false},
    {"delay", false}}},
 },
 {"simpleAction",
  {ncl_push_simpleAction, nullptr, false,
   {"causalConnector", "compoundAction"},
   {{"role", true},
    {"delay", false},
    {"eventType", false},
    {"actionType", false},
    {"value", false},
    {"min", false},
    {"max", false},
    {"min", false},
    {"qualifier", false},
    {"repeat", false},
    {"repeatDelay", false},
    {"duration", false},
    {"by", false}}},
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
ncl_attr_parse_eventType (string str, EventType *result)
{
  static map<string, EventType> tab =
    {
     {"presentation", EventType::PRESENTATION},
     {"attribution", EventType::ATTRIBUTION},
     {"selection", EventType::SELECTION},
    };
  _NCL_ATTR_PARSE (EventType, tab, str, result);
}

static bool
ncl_attr_parse_transition (string str, EventStateTransition *result)
{
  static map<string, EventStateTransition> tab =
    {
     {"starts", EventStateTransition::START},
     {"stops", EventStateTransition::STOP},
     {"aborts", EventStateTransition::ABORT},
     {"pauses", EventStateTransition::PAUSE},
     {"resumes", EventStateTransition::RESUME},
    };
  _NCL_ATTR_PARSE (EventStateTransition, tab, str, result);
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
              unused (Entity **entity))
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
             unused (Entity *entity))
{
  bool status = true;

  // Resolve descriptor's references to regions.
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

  // Resolve media's references to descriptors.
  for (auto media_id: st->cacheset["media"])
    {
      map<string, string> *media_attr;
      Media *media;
      xmlNode *media_elt;
      map<string, string> *desc_attr;
      string desc_id;

      g_assert (st_cache_index (st, media_id, &media_attr));
      if (!ncl_attrmap_index (media_attr, "descriptor", &desc_id))
        continue;               // nothing to do

      media = cast (Media *, st->ncl->getEntityById (media_id));
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
                  Entity **entity)
{
  Context *context;
  string id;

  id = ncl_attrmap_opt_get (attr, "id", st->ncl->getId ());
  if (toString (elt->name) == "body")
    {
      context = st->ncl->getRoot ();
    }
  else
    {
      Composition *parent = cast (Composition *, st->stack.back ());
      g_assert_nonnull (parent);
      context = new Context (st->ncl, id);
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
                 unused (Entity *entity))
{
  Context *context;

  context = cast (Context *, entity);
  g_assert_nonnull (context);

  // Resolve port's references.
  for (auto port: *context->getPorts ())
    {
      map<string, string> *port_attr;
      xmlNode *port_elt;
      string port_id;
      string comp_id;
      string iface_id;

      Node *node;
      Anchor *anchor;

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

      node = cast (Node *, st->ncl->getEntityById (comp_id));
      g_assert_nonnull (node);
      if (unlikely (node->getParent () != port->getParent ()))
        {
          return ST_ERR_ELT_BAD_ATTR
            (st, port_elt, "component", comp_id.c_str (),
             "no such component in scope");
        }
      port->setNode (node);

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
      port->setInterface (anchor);
    }
  return true;
}

static bool
ncl_push_port (ParserLibXML_State *st,
               unused (xmlNode *elt),
               map<string, string> *attr,
               unused (Entity **entity))
{
  Port *port;
  Context *context;

  port = new Port (st->ncl, ncl_attrmap_get (attr, "id"));
  context = cast (Context *, st->stack.back ());
  g_assert_nonnull (context);
  context->addPort (port);
  return true;
}

static bool
ncl_push_media (ParserLibXML_State *st,
                unused (xmlNode *elt),
                map<string, string> *attr,
                unused (Entity **entity))
{
  Media *media;
  Composition *comp;

  media = new Media (st->ncl, ncl_attrmap_get (attr, "id"), false);
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

  comp = cast (Composition *, st->stack.back ());
  g_assert_nonnull (comp);
  comp->addNode (media);
  *entity = media;              // push onto stack
  return true;
}

static bool
ncl_push_property (ParserLibXML_State *st,
                   unused (xmlNode *elt),
                   map<string, string> *attr,
                   unused (Entity **entity))
{
  Node *parent = cast (Node *, st->stack.back ());
  g_assert_nonnull (parent);
  parent->setProperty (ncl_attrmap_get (attr, "name"),
                       ncl_attrmap_opt_get (attr, "value", ""));
  return true;
}

static bool
ncl_push_region (ParserLibXML_State *st,
                 unused (xmlNode *elt),
                 map<string, string> *attr,
                 unused (Entity **entity))
{
  static int last_zorder = 0;
  GingaRect screen_rect;
  GingaRect parent_rect;
  GingaRect rect;
  string value;

  g_assert_nonnull (elt->parent);
  if (toString (elt->parent->name) != "region") // root region
    {
      GingaRect *saved_rect = new GingaRect;
      *saved_rect = screen_rect = st->rect;
      st_set_data (st, "saved_rect", saved_rect);
    }
  else
    {
      GingaRect *saved_rect = (GingaRect *) st_get_data (st, "saved_rect");
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
                unused (Entity *entity))
{
  g_assert_nonnull (elt->parent);
  if (toString (elt->parent->name) != "region") // root region
    {
      GingaRect *saved_rect;
      saved_rect = (GingaRect *) st_get_data (st, "saved_rect");
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
                          unused (Entity **entity))
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
                          Entity **entity)
{
  Connector *conn;
  conn = new Connector (st->ncl, ncl_attrmap_get (attr, "id"));
  *entity = conn;               // push onto stack
  return true;
}

static bool
ncl_pop_causalConnector (unused (ParserLibXML_State *st),
                         xmlNode *elt,
                         unused (map<string, string> *attr),
                         unused (vector<xmlNode *> *children),
                         Entity *entity)
{
  Connector *conn;

  conn = cast (Connector *, entity);
  g_assert_nonnull (conn);

  if (unlikely (conn->getCondition () == nullptr))
    return ST_ERR_ELT_MISSING_CHILD (st, elt, "simpleCondition");
  if (unlikely (conn->getAction () == nullptr))
    return ST_ERR_ELT_MISSING_CHILD (st, elt, "simpleAction");

  return true;
}

static bool
ncl_push_simpleCondition (ParserLibXML_State *st,
                          xmlNode *elt,
                          map<string, string> *attr,
                          unused (Entity **entity))
{
  string role;
  EventType type;
  EventStateTransition trans;

  Connector *parent;
  SimpleCondition *cond;

  role = ncl_attrmap_get (attr, "role");
  if (SimpleCondition::isReserved (role, &type, &trans))
    {
      string str;
      if (unlikely (ncl_attrmap_index (attr, "eventType", &str)))
        return ST_ERR_ELT_BAD_ATTR
          (st, elt, "eventType", str.c_str (), "reserved role" );
      if (unlikely (ncl_attrmap_index (attr, "transition", &str)))
        return ST_ERR_ELT_BAD_ATTR
          (st, elt, "transition", str.c_str (), "reserved role");
    }
  else
    {
      string str;
      if (unlikely (!ncl_attrmap_index (attr, "eventType", &str)))
        return ST_ERR_ELT_MISSING_ATTR (st, elt, "eventType");
      if (unlikely (!ncl_attr_parse_eventType (str, &type)))
        return ST_ERR_ELT_BAD_ATTR
          (st, elt, "eventType", str.c_str (), "invalid type");

      if (unlikely (!ncl_attrmap_index (attr, "transition", &str)))
        return ST_ERR_ELT_MISSING_ATTR (st, elt, "transition");
      if (unlikely (!ncl_attr_parse_transition (str, &trans)))
        return ST_ERR_ELT_BAD_ATTR
          (st, elt, "transition", str.c_str (), "invalid transition");
    }

  parent = cast (Connector *, st->stack.back ());
  g_assert_nonnull (parent);

  cond = new SimpleCondition (type, trans, role, false, "",
                              ncl_attrmap_opt_get (attr, "key", ""));
  parent->setCondition (cond);
  return true;
}

static bool
ncl_push_simpleAction (unused (ParserLibXML_State *st),
                       unused (xmlNode *elt),
                       unused (map<string, string> *attr),
                       unused (Entity **entity))
{
  // string role;
  // EventType type;
  // EventStateTransition action;

  // role = ncl_attrmap_get (attr, "role");
  // if (SimpleAction::isReserved (role, &type, &action);
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
  Entity *entity;
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
          if (!ainfo.required)
            continue;

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
      if (einfo->cache)
        {
          st->cache[id] = map<string, string> (_attr);
          st->cacheelt[id] = elt;
          st->cacheset[tag].insert (id);
          attr = &st->cache[id];
        }
    }
  else
    {
      g_assert_false (einfo->cache);
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

GINGA_NCL_END
