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

#include <libxml/tree.h>
#include <libxml/parser.h>

GINGA_NCL_BEGIN

// Helper macros and functions.
#define toChar(s)    deconst (char *, (s))
#define toXmlChar(s) (xmlChar *)(deconst (char *, (s).c_str ()))
#define toString(s)  string (deconst (char *, (s)))

#define xmlGetPropChar(elt, name)\
  toChar (xmlGetProp ((elt), (name)))

static inline bool
xmlGetPropStr (xmlNode *elt, const string &_name, string *result)
{
  xmlChar *name = (xmlChar *) deconst (char *, _name.c_str ());
  xmlChar *value = xmlGetProp (elt, name);
  if (value == nullptr)
    return false;
  tryset (result, toString (value));
  return true;
}


// Parser internal state.

typedef struct ParserLibXML_State
{
  NclDocument *ncl;             // NCL tree
  xmlDoc *doc;                  // DOM tree
  string errmsg;                // last error message
} ParserLibXML_State;

#define _ST_ERR_FMT "NCL syntax error: "
#define ST_ERR(st, fmt, ...)\
  ((st)->errmsg = xstrbuild (_ST_ERR_FMT fmt, ## __VA_ARGS__), false)

#define ST_ERR_LINE(st, line, fmt, ...)\
  ST_ERR ((st), "line %d: " fmt, (line), __VA_ARGS__)

#define ST_ERR_ELT(st, elt, fmt, ...)\
  ST_ERR_LINE ((st), (elt)->line, fmt, ## __VA_ARGS__)

#define ST_ERR_ELT_UNKNOWN(st, elt)\
  ST_ERR_ELT ((st), (elt), "Unknown element <%s>", (elt)->name)

#define ST_ERR_ELT_ATTR_MISSING(st, elt, attrname)                      \
  ST_ERR_ELT ((st), (elt),                                              \
              "element <%s> is missing the required attribute \"%s\"",  \
              (elt)->name, (attrname))


// NCL syntax.

// Element parse function.
typedef bool (NclEltParseFunc) (ParserLibXML_State *, xmlNode *,
                                const string &, map<string, GValue> *);

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
  NclEltParseFunc *parse;         // element parse function
  vector<NclAttrInfo> attributes; // element attributes
} NclEltInfo;

// Forward declarations.
#define NCL_ELT_PARSE_DECL(elt)                         \
  static bool G_PASTE (ncl_elt_parse_, elt)             \
    (ParserLibXML_State *, xmlNode *, const string &,   \
     map<string, GValue> *);

NCL_ELT_PARSE_DECL (ncl)

// Element table.
static map<string, NclEltInfo> ncl_elt_table =
{
 {"ncl",
  {ncl_elt_parse_ncl,
   {{"id",    G_TYPE_STRING, false},
    {"title", G_TYPE_STRING, false},
    {"xmlns", G_TYPE_STRING, false},
   }}},
};

// Indexes element table.
static bool
ncl_elt_table_index (const string &tag, NclEltInfo **result)
{
  map<string, NclEltInfo>::iterator it;
  if ((it = ncl_elt_table.find (tag)) == ncl_elt_table.end ())
    return false;
  tryset (result, &it->second);
  return true;
}


// NCL attribute map helper functions.

static bool
ncl_attrmap_has (map<string, GValue> *attr, const string &name)
{
  map<string, GValue>::iterator it;
  return !((it = attr->find (name)) == attr->end ());
}

static bool
ncl_attrmap_get (map<string, GValue> *attr, const string &name,
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
  g_assert (ncl_attrmap_get (attr, name, &value));
  g_assert (G_VALUE_TYPE (value) == G_TYPE_STRING);
  return string (g_value_get_string (value));
}

static string
ncl_attrmap_get_opt_string (map<string, GValue> *attr,
                            const string &name, const string &defvalue)
{
  return ncl_attrmap_has (attr, name)
    ? ncl_attrmap_get_string (attr, name) : defvalue;
}


// NCL element parse functions.

static bool
ncl_elt_parse_ncl (ParserLibXML_State *st, xmlNode *elt,
                   unused (const string &tag), map<string, GValue> *attr)
{
  string id;
  string url;

  id = ncl_attrmap_get_opt_string (attr, "id", "ncl");
  url = (st->doc->URL) ? toString (st->doc->URL) : "";
  st->ncl = new NclDocument (id, url);

  for (xmlNode *child = elt->children; child; child = child->next)
    {
      if (child->type == XML_ELEMENT_NODE)
        TRACE (">>> %s", toChar (child->name));
    }

  return true;
}


// External API.

static bool
processElt (ParserLibXML_State *st, xmlNode *elt)
{
  string tag;
  NclEltInfo *einfo;
  map<string, GValue> attrmap;

  tag = toString (elt->name);
  if ((!ncl_elt_table_index (tag, &einfo)))
    return ST_ERR_ELT_UNKNOWN (st, elt);

  for (auto ainfo: einfo->attributes)
    {
      xmlChar *name;
      bool hasprop;
      GValue value = G_VALUE_INIT;

      name = toXmlChar (ainfo.name);
      hasprop = xmlHasProp (elt, name);

      if (unlikely (ainfo.required && !hasprop))
        return ST_ERR_ELT_ATTR_MISSING (st, elt, ainfo.name.c_str ());

      if (!hasprop)
        continue;

      g_value_init (&value, ainfo.type);
      switch (ainfo.type)
        {
        case G_TYPE_STRING:
          g_value_take_string (&value, xmlGetPropChar (elt, name));
          break;
        default:
          g_assert_not_reached ();
        }
      attrmap[ainfo.name] = value;
    }

  if (einfo->parse)
    return einfo->parse (st, elt, tag, &attrmap);
  else
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
      g_assert_null (st.ncl);
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
