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

#include "ginga-internal.h"
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

#define ST_ERR_ELT_UNKNOWN_CHILD(st, etl, child)\
  ST_ERR_ELT ((st), (elt), "Unknown child <%s>", (child))

// Index state cache.
static G_GNUC_UNUSED bool
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
NCL_ELT_PUSH_DECL (body)
NCL_ELT_POP_DECL  (body)
NCL_ELT_PUSH_DECL (port)
NCL_ELT_PUSH_DECL (media)
NCL_ELT_PUSH_DECL (property)
NCL_ELT_PUSH_DECL (region)
NCL_ELT_POP_DECL  (region)
NCL_ELT_PUSH_DECL (descriptorParam)

// Element map.
static map<string, NclEltInfo> ncl_eltmap =
{
 //
 // Elements that are converted to entities.
 //
 {"ncl",
  {ncl_push_ncl, ncl_pop_ncl, false, {},
   {{"id", false},
    {"title", false},
    {"xmlns", false}}},
 },
 {"body",
  {ncl_push_body, ncl_pop_body, false, {"ncl"},
   {{"id", false}}},
 },
 {"port",
  {ncl_push_port, nullptr, true, {"body", "context"},
   {{"id", true},
    {"component", true},
    {"interface", false}}},
 },
 {"media",
  {ncl_push_media, nullptr, true, {"body", "context"},
   {{"id", true},
    {"src", false},
    {"descriptor", false}}},
 },
 {"property",
  {ncl_push_property, nullptr, false, {"body", "context", "media"},
  {{"name", true},
   {"value", false}}},
 },
 //
 // Elements that are not converted to entities:
 //
 {"head",
  {nullptr, nullptr, false, {"ncl"}, {}},
 },
 {"regionBase",
  {nullptr, nullptr, false, {"head"},
   {{"id", false},
    {"device", false},
    {"region", false}}},
 },
 {"region",
  {ncl_push_region, ncl_pop_region, true, {"region", "regionBase"},
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
  {nullptr, nullptr, false, {"head"},
   {{"id", false}}},
 },
 {"descriptor",
  {nullptr, nullptr, true, {"descriptorBase"},
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
  {ncl_push_descriptorParam, nullptr, false, {"descriptor"},
   {{"name", true},
    {"value", true}}},
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

      if (unlikely (!st_cache_index (st, reg_id, &reg_attr)))
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

      if (unlikely (!st_cache_index (st, desc_id, &desc_attr)))
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
ncl_push_body (ParserLibXML_State *st,
               unused (xmlNode *elt),
               map<string, string> *attr,
               Entity **entity)
{
  Context *body;
  string id;

  id = ncl_attrmap_opt_get (attr, "id", st->ncl->getId ());
  body = st->ncl->getRoot ();
  g_assert_nonnull (body);
  *entity = body;               // push onto stack
  return true;
}

static bool
ncl_pop_body (unused (ParserLibXML_State *st),
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
      string comp;
      string iface;

      Node *node;
      Anchor *anchor;

      port_id = port->getId ();
      g_assert (st_cache_index (st, port_id, &port_attr));

      port_elt = st->cacheelt[port_id];
      g_assert_nonnull (port_elt);

      comp = ncl_attrmap_get (port_attr, "component");
      node = cast (Node *, st->ncl->getEntityById (comp));
      if (unlikely (node == nullptr))
        {
          return ST_ERR_ELT_BAD_ATTR
            (st, port_elt, "component", comp.c_str (), "no such element");
        }
      port->setNode (node);

      anchor = nullptr;
      if (ncl_attrmap_index (port_attr, "interface", &iface))
        {
          anchor = node->getAnchor (iface);
          if (unlikely (anchor == nullptr))
            {
              return ST_ERR_ELT_BAD_ATTR
                (st, port_elt, "interface", comp.c_str (),
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
        src = xpathbuildabs (xpathdirname (toString (st->doc->URL)), src);
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
                       ncl_attrmap_get (attr, "value"));
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

      if (unlikely (!processElt (st, child)))
        {
         status = false;
         goto done;
        }

      string child_tag = toString (child->name);
      if (unlikely (possible.find (child_tag) == possible.end ()))
        {
          status = ST_ERR_ELT_UNKNOWN_CHILD (st, elt, child->name);
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
