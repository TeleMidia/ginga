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

#define xmlGetPropCString(elt, name)\
  toCString (xmlGetProp ((elt), (name)))


// Parser internal state.

typedef struct ParserLibXML_State
{
  xmlDoc *doc;                  // DOM tree
  NclDocument *ncl;             // NCL tree
  vector<Entity *> stack;       // NCL entity stack
  string errmsg;                // last error message
} ParserLibXML_State;

static inline G_GNUC_PRINTF (2,3) void
_st_err (ParserLibXML_State *st, const char *fmt, ...)
{
  va_list args;
  char *c_str = NULL;
  int n;

  va_start (args, fmt);
  n = g_vasprintf (&c_str, fmt, args);
  va_end (args);

  g_assert (n >= 0);
  g_assert_nonnull (c_str);
  st->errmsg.assign (c_str);
  WARNING ("%s", c_str);
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

#define ST_ERR_ELT_MISSING_ATTR(st, elt, attr)\
  ST_ERR_ELT ((st), (elt), "Missing attribute '%s'", (attr))

#define ST_ERR_ELT_MISSING_CHILD(st, elt, child)\
  ST_ERR_ELT ((st), (elt), "Missing child <%s>", (child))

#define ST_ERR_ELT_BAD_CHILD(st, etl, child)\
  ST_ERR_ELT ((st), (elt), "Unexpected child <%s>", (child))


// NCL syntax.

// Element push function.
typedef bool (NclEltPushFunc) (ParserLibXML_State *,
                               xmlNode *,
                               map<string, GValue> *,
                               Entity **);
// Element pop function.
typedef bool (NclEltPopFunc) (ParserLibXML_State *,
                              xmlNode *,
                              map<string, GValue> *,
                              vector<xmlNode *> *,
                              Entity *);
// Attribute info.
typedef struct NclAttrInfo
{
  string name;                  // attribute name
  GType type;                   // attribute type
  bool required;                // whether attribute is required
} NclAttrInfo;

// Element info.
typedef struct NclEltInfo
{
  NclEltPushFunc *push;           // push function
  NclEltPopFunc *pop;             // pop function
  vector<string> parents;         // possible parents
  vector<NclAttrInfo> attributes; // attributes
} NclEltInfo;

// Forward declarations.
#define NCL_ELT_PUSH_DECL(elt)                  \
  static bool G_PASTE (ncl_push_, elt)          \
    (ParserLibXML_State *, xmlNode *,           \
     map<string, GValue> *, Entity **);

#define NCL_ELT_POP_DECL(elt)                                   \
  static bool G_PASTE (ncl_pop_, elt)                           \
    (ParserLibXML_State *, xmlNode *,                           \
     map<string, GValue> *, vector<xmlNode *> *, Entity *);

NCL_ELT_PUSH_DECL (ncl)
NCL_ELT_POP_DECL  (ncl)
NCL_ELT_PUSH_DECL (body)
NCL_ELT_POP_DECL  (body)
NCL_ELT_PUSH_DECL (port)
NCL_ELT_PUSH_DECL (media)

// Element map.
static map<string, NclEltInfo> ncl_eltmap =
{
 {"ncl",
  {ncl_push_ncl, ncl_pop_ncl, {},
   {{"id", G_TYPE_STRING, false},
    {"title", G_TYPE_STRING, false},
    {"xmlns", G_TYPE_STRING, false}}},
 },
 {"head",
  {nullptr, nullptr, {"ncl"}, {}},
 },
 {"body",
  {ncl_push_body, ncl_pop_body, {"ncl"},
   {{"id", G_TYPE_STRING, false}}},
 },
 {"port",
  {ncl_push_port, nullptr, {"body", "context"},
   {{"id", G_TYPE_STRING, true},
    {"component", G_TYPE_STRING, true},
    {"interface", G_TYPE_STRING, false}}},
 },
 {"media",
  {ncl_push_media, nullptr, {"body", "context"},
   {{"id", G_TYPE_STRING, true},
    {"src", G_TYPE_STRING, false}}}},
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

// Gets possible children for a given element.
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

static bool
ncl_attrmap_index (map<string, GValue> *attr, const string &name,
                   GValue **result)
{
  map<string, GValue>::iterator it;
  if ((it = attr->find (name)) == attr->end ())
    return false;
  tryset (result, &it->second);
  return true;
}

static string
ncl_attrmap_get_string (map<string, GValue> *attr, const string &name)
{
  GValue *value;
  g_assert (ncl_attrmap_index (attr, name, &value));
  g_assert (G_VALUE_TYPE (value) == G_TYPE_STRING);
  return string (g_value_get_string (value));
}

static string
ncl_attrmap_get_opt_string (map<string, GValue> *attr,
                            const string &name, const string &defvalue)
{
  return ncl_attrmap_index (attr, name, nullptr)
    ? ncl_attrmap_get_string (attr, name) : defvalue;
}


// NCL element push & pop functions.

static bool
ncl_push_ncl (ParserLibXML_State *st,
              unused (xmlNode *elt),
              map<string, GValue> *attr,
              unused (Entity **entity))
{
  string id = ncl_attrmap_get_opt_string (attr, "id", "ncl");
  string  url = (st->doc->URL) ? toString (st->doc->URL) : "";
  st->ncl = new NclDocument (id, url);
  return true;
}

static bool
ncl_pop_ncl (unused (ParserLibXML_State *st),
             unused (xmlNode *elt),
             unused (map<string, GValue> *attr),
             unused (vector<xmlNode *> *children),
             unused (Entity *entity))
{
  return true;
}

static bool
ncl_push_body (ParserLibXML_State *st,
               unused (xmlNode *elt),
               map<string, GValue> *attr,
               Entity **entity)
{
  Context *body;
  string id;

  id = ncl_attrmap_get_opt_string (attr, "id", st->ncl->getId ());
  body = new Context (st->ncl, id);
  st->ncl->setBody (body);
  *entity = body;

  return true;
}

static bool
ncl_pop_body (unused (ParserLibXML_State *st),
              unused (xmlNode *elt),
              unused (map<string, GValue> *attr),
              unused (vector<xmlNode *> *children),
              unused (Entity *entity))
{
  return true;
}

static bool
ncl_push_port (ParserLibXML_State *st,
               unused (xmlNode *elt),
               map<string, GValue> *attr,
               unused (Entity **entity))
{
  Port *port;
  Context *context;

  port = new Port (st->ncl, ncl_attrmap_get_string (attr, "id"));
  context = cast (Context *, st->stack.back ());
  g_assert_nonnull (context);
  context->addPort (port);
  return true;
}

static bool
ncl_push_media (ParserLibXML_State *st,
                unused (xmlNode *elt),
                map<string, GValue> *attr,
                unused (Entity **entity))
{
  Media *media;
  Composition *comp;

  media = new Media (st->ncl, ncl_attrmap_get_string (attr, "id"), false);
  comp = cast (Composition *, st->stack.back ());
  g_assert_nonnull (comp);
  comp->addNode (media);
  return true;
}


// External API.

static bool
processElt (ParserLibXML_State *st, xmlNode *elt)
{
  string tag;
  NclEltInfo *einfo;

  map<string, GValue> attr;
  Entity *entity;
  map<string, bool> possible;
  vector<xmlNode *> children;

  tag = toString (elt->name);
  if (unlikely (!ncl_eltmap_index (tag, &einfo)))
    return ST_ERR_ELT_UNKNOWN (st, elt);

  // Check parent.
  g_assert_nonnull (elt->parent);
  if (einfo->parents.size () > 0)
    {
      string parent;
      bool found;

      if (unlikely (elt->parent->type != XML_ELEMENT_NODE))
        return ST_ERR_ELT_MISSING_PARENT (st, elt);

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
        return ST_ERR_ELT_BAD_PARENT
          (st, elt, toCString (elt->parent->name));
    }

  // Collect attributes.
  for (auto ainfo: einfo->attributes)
    {
      xmlChar *name;
      bool hasprop;
      GValue value = G_VALUE_INIT;

      name = toXmlChar (ainfo.name);
      hasprop = xmlHasProp (elt, name);

      if (unlikely (ainfo.required && !hasprop))
        return ST_ERR_ELT_MISSING_ATTR (st, elt, ainfo.name.c_str ());

      if (!hasprop)
        continue;

      g_value_init (&value, ainfo.type);
      switch (ainfo.type)
        {
        case G_TYPE_STRING:
          g_value_take_string (&value, xmlGetPropCString (elt, name));
          break;
        default:
          g_assert_not_reached ();
        }
      attr[ainfo.name] = value;
      g_value_unset (&value);
    }

  // Push element.
  entity = nullptr;
  if (unlikely (einfo->push && !einfo->push (st, elt, &attr, &entity)))
    return false;

  // Push newly created entity onto stack.
  if (entity != nullptr)
    st->stack.push_back (entity);

  // Collect children.
  possible = ncl_eltmap_get_possible_children (tag);
  for (xmlNode *child = elt->children; child; child = child->next)
    {
      if (child->type != XML_ELEMENT_NODE)
        continue;

      if (unlikely (!processElt (st, child)))
        return false;

      string child_tag = toString (child->name);
      if (unlikely (possible.find (child_tag)) == possible.end ())
        return ST_ERR_ELT_BAD_CHILD (st, elt, child->name);

      children.push_back (child);
    }

  // Pop element.
  if (einfo->pop)
    return einfo->pop (st, elt, &attr, &children, entity);

  return true;
}

static NclDocument *
processDoc (xmlDoc *doc, string *errmsg)
{
  ParserLibXML_State st;
  xmlNode *root;

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
ParserLibXML::parseBuffer (const void *buf,
                           size_t size,
                           unused (int width),
                           unused (int height),
                           string *errmsg)
{
# define FLAGS (XML_PARSE_NOERROR | XML_PARSE_NOWARNING)
  xmlDoc *doc;
  NclDocument *ncl;

  doc = xmlReadMemory ((const char *) buf, (int) size, NULL, NULL, FLAGS);
  if (unlikely (doc == nullptr))
    {
      xmlError *err = xmlGetLastError ();
      g_assert_nonnull (err);
      tryset (errmsg, "XML error: " + string (err->message));
      return nullptr;
    }

  ncl = processDoc (doc, errmsg);
  xmlFreeDoc (doc);
  return ncl;
}

NclDocument *
ParserLibXML::parseFile (const string &path,
                         unused (int width),
                         unused (int height),
                         string *errmsg)
{
  xmlDoc *doc;
  NclDocument *ncl;

  doc = xmlReadFile (path.c_str (), NULL, FLAGS);
  if (unlikely (doc == nullptr))
    {
      xmlError *err = xmlGetLastError ();
      g_assert_nonnull (err);
      tryset (errmsg, "XML error: " + string (err->message));
      return nullptr;
    }

  ncl = processDoc (doc, errmsg);
  xmlFreeDoc (doc);
  return ncl;
}

GINGA_NCL_END
