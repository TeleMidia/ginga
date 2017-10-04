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

GINGA_PRAGMA_DIAG_IGNORE (-Wimplicit-fallthrough)


// Logging -----------------------------------------------------------------

/**
 * @brief Parses G_STRFUNC string to generate a log prefix.
 * @param strfunc String generated by G_STRFUNC macro.
 * @return Log prefix.
 */
string
__ginga_strfunc (const string &strfunc)
{
  string result;
  size_t i;

  i = strfunc.find ("(");
  g_assert (i != std::string::npos);
  result = strfunc.substr (0, i);

  i = result.rfind (" ");
  if (i != std::string::npos)
    result = result.substr (i + 1);

  return result + "()";
}


// Parsing and evaluation --------------------------------------------------

/**
 * @brief Parses boolean string.
 * @param s Boolean string.
 * @param result Variable to store the resulting boolean.
 * @return True if successful, or false otherwise.
 */
bool
_ginga_parse_bool (const string &s, bool *result)
{
  if (xstrcaseeq (s, "true"))
    {
      tryset (result, true);
      return true;
    }
  else if (xstrcaseeq (s, "false"))
    {
      tryset (result, false);
      return true;
    }
  else
    {
      return false;
    }
}

/**
 * @brief Parses color string.
 * @param s Color string.
 * @param result Variable to store the resulting color.
 * @return True if successful, or false otherwise.
 */
bool
_ginga_parse_color (const string &s, GingaColor *result)
{
  if (s == "")
    {
      GingaColor none = {0,0,0,0};
      tryset (result, none);
      return true;
    }
  else
    {
      return gdk_rgba_parse (result, s.c_str ());
    }
}

/**
 * @brief Parses list of string items.
 * @param s List string.
 * @param sep Separator.
 * @param min Minimum number of items.
 * @param max Maximum number of items.
 * @param result Variable to store the resulting items.
 * @return True if successful, or false otherwise.
 */
bool
_ginga_parse_list (const string &s, char sep, size_t min, size_t max,
                   vector<string> *result)
{
  vector<string> items;
  size_t n;

  items = xstrsplit (s, sep);
  n = items.size ();
  if (n < min || n > max)
    return false;

  for (size_t i = 0; i < n; i++)
    items[i] = xstrstrip (items[i]);

  tryset (result, items);
  return true;
}

/**
 * @brief Parses number or percent string to an integer.
 * @param s Number or percent string.
 * @param base Base value used to convert percent to integer.
 * @param min Minimum resulting integer.
 * @param max Maximum resulting integer.
 * @return The resulting integer.
 */
int
ginga_parse_percent (const string &s, int base, int min, int max)
{
  bool percent;
  double d;
  int result;

  d = xstrtodorpercent (s, &percent);
  if (percent)
    {
      result = (int) lround (d * base);
    }
  else
    {
      result = (int) lround (d);
    }
  return (int) CLAMP (result, min, max);
}

/**
 * Parses time string ("Ns" or "NN:NN:NN").
 * @param s Time string.
 * @param result Variable to store the resulting time.
 * @return True if successful, or false otherwise.
 */
bool
_ginga_parse_time (const string &s, GingaTime *result)
{
  gchar *dup;
  gchar *end;
  double secs;

  dup = g_strdup (s.c_str ());
  g_strchomp (dup);

  secs = g_strtod (dup, &end);
  if (*end == '\0' || g_str_equal (end, "s"))
    goto success;

  if (*end != ':')
    goto failure;

  end++;
  secs = 3600 * secs + 60 * g_strtod (end, &end);
  if (*end != ':')
    goto failure;

  end++;
  secs += g_strtod (end, &end);
  if (*end != '\0')
    goto failure;

 success:
  g_free (dup);
  tryset (result, (GingaTime)(secs * GINGA_SECOND));
  return true;

 failure:
  g_free (dup);
  return false;
}

// Asserted wrappers for _ginga_parse_*.
#define _GINGA_PARSE_DEFN(Type, Name, Str)                      \
  Type                                                          \
  ginga_parse_##Name (const string &s)                          \
  {                                                             \
    Type result;                                                \
    if (unlikely (!_ginga_parse_##Name (s, &result)))           \
      ERROR_SYNTAX ("invalid %s string '%s'", Str, s.c_str ()); \
    return result;                                              \
  }

_GINGA_PARSE_DEFN (bool, bool, "boolean")
_GINGA_PARSE_DEFN (GingaColor, color, "color")
_GINGA_PARSE_DEFN (GingaTime, time, "time")

vector<string>
ginga_parse_list (const string &s, char sep, size_t min, size_t max)
{
  vector<string> result;
  if (unlikely (!_ginga_parse_list (s, sep, min, max, &result)))
    ERROR_SYNTAX ("invalid list string '%s'", s.c_str ());
  return result;
}

/**
 * @brief Parses comparator string.
 * @param s Comparator string.
 * @param result Variable to store the resulting comparator.
 * @return True if success, or false otherwise.
 */
bool
_ginga_parse_comparator (const string &s, string *result)
{
  if (xstrcaseeq (s, "eq")
      || xstrcaseeq (s, "ne")
      || xstrcaseeq (s, "lt")
      || xstrcaseeq (s, "lte")
      || xstrcaseeq (s, "gt")
      || xstrcaseeq (s, "gte"))
    {
      tryset (result, xstrdown (s));
      return true;
    }
  else
    {
      return false;
    }
}

/**
 * @brief Evaluates comparator with the given operands.
 * @param s Operator ("eq", "ne", "lt", "lte", "gt", or "gte").
 * @param a First operand.
 * @param b Second operand.
 */
bool
ginga_eval_comparator (const string &s, const string &a, const string &b)
{
  string comp;

  g_assert (_ginga_parse_comparator (s, &comp));

  if (comp == "eq")
    {
      if (a == b)
        return true;
    }
  else if (comp == "ne")
    {
      if (a != b)
        return true;
    }
  else if (comp == "lt")
    {
      if (a.compare (b) < 0)
        return true;
    }
  else if (comp == "lte")
    {
      if (a.compare (b) <= 0)
        return true;
    }
  else if (comp == "gt")
    {
      if (a.compare (b) > 0)
        return true;
    }
  else if (comp == "gte")
    {
      if (a.compare (b) >= 0)
        return true;
    }
  else
    g_assert_not_reached ();

  return false;
}


// Strings -----------------------------------------------------------------

/**
 * @brief Converts string to a floating-point number.
 * @param s Number string.
 * @param dp Variable to store the resulting floating-point number.
 * @return True if successful, or false otherwise.
 */
bool
_xstrtod (const string &s, double *dp)
{
  const gchar *c_str;
  gchar *endptr;
  double d;

  c_str = s.c_str ();
  d = g_ascii_strtod (c_str, &endptr);
  if (endptr == c_str)
    return false;

  tryset (dp, d);
  return true;
}

/**
 * @brief Converts string to a 64-bit integer.
 * @param s Number string.
 * @param ip Variable to store the resulting integer.
 * @param base The base to be used in the conversion.
 * @return True if successful, or false otherwise.
 */
bool
_xstrtoll (const string &s, gint64 *ip, guint base)
{
  const gchar *c_str;
  gchar *endptr;
  gint64 i;

  c_str = s.c_str ();
  i = g_ascii_strtoll (c_str, &endptr, base);
  if (endptr == c_str)
    return false;

  tryset (ip, i);
  return true;
}

/**
 * @brief Converts string to an unsigned 64-bit integer.
 * @param s Number string.
 * @param ip Variable to store the resulting unsigned integer.
 * @param base The base to be used in the conversion.
 * @return True if successful, or false otherwise.
 */
bool
_xstrtoull (const string &s, guint64 *ip, guint base)
{
  const gchar *c_str;
  gchar *endptr;
  guint64 u;

  c_str = s.c_str ();
  u = g_ascii_strtoull (c_str, &endptr, base);
  if (endptr == c_str)
    return false;

  tryset (ip, u);
  return true;
}

// Asserted wrappers for _xstrtod, _xstrtoll, and _xstrtoull.
double
xstrtod (const string &s)
{
  double d;
  g_assert (_xstrtod (s, &d));
  return d;
}

#define _GINGA_XSTRTO_DEFN(Type, Typemin, Typemax)      \
  g##Type                                               \
  xstrto##Type (const string &s, guint8 base)           \
  {                                                     \
    gint64 x=0;                                         \
    g_assert (_xstrtoll (s, &x, base));                 \
    return (g##Type)(CLAMP (x, Typemin, Typemax));      \
  }

_GINGA_XSTRTO_DEFN (int,    G_MININT,    G_MAXINT)
_GINGA_XSTRTO_DEFN (int8,   G_MININT8,   G_MAXINT8)
_GINGA_XSTRTO_DEFN (int64,  G_MININT64,  G_MAXINT64)

#define _GINGA_XSTRTOU_DEFN(Type, Typemax)              \
  g##Type                                               \
  xstrto##Type (const string &s, guint8 base)           \
  {                                                     \
    guint64 x=0;                                        \
    g_assert (_xstrtoull (s, &x, base));                \
    return (g##Type)(MIN (x, Typemax));                 \
  }

_GINGA_XSTRTOU_DEFN (uint,   G_MAXUINT)
_GINGA_XSTRTOU_DEFN (uint8,  G_MAXUINT8)
_GINGA_XSTRTOU_DEFN (uint64, G_MAXUINT64)

/**
 * @brief Checks if string is a percent value.
 * @param s Value string.
 * @return True if successful, or false otherwise.
 */
bool
xstrispercent (const string &s)
{
  const gchar *str = s.c_str ();
  gchar *end;

  if (str[0] == '%')
    return false;

  g_ascii_strtod (str, &end);
  return *end == '%';
}

/**
 * @brief Converts number or percent string to floating-point number.
 * @param s Number or percent string.
 * @param perc Variable to store whether the converted value is a number or
 * percent value.
 * @result The resulting number.
 */
// Converts a number or percent string to a number.
gdouble
xstrtodorpercent (const string &s, bool *perc)
{
  gchar *end;
  gdouble x = g_ascii_strtod (s.c_str (), &end);
  if (*end == '%')
    {
      tryset (perc, true);
      return x / 100.;
    }
  else
    {
      tryset (perc, false);
      return x;
    }
}

/**
 * @brief Compares two strings ignoring case.
 * @param s1 First string.
 * @param s2 Second string.
 * @return -1, 0, or 1
 */
int
xstrcasecmp (const string &s1, const string &s2)
{
  return g_ascii_strcasecmp (s1.c_str (), s2.c_str ());
}

/**
 * @brief Tests string string prefix.
 * @param s String.
 * @param prefix Prefix.
 * @return True if successful, or false otherwise.
 */
bool
xstrhasprefix (const string &s, const string &prefix)
{
  return g_str_has_prefix (s.c_str (), prefix.c_str ());
}

/**
 * @brief Tests if string suffix.
 * @param s String.
 * @param suffix Suffix.
 * @return True if successful, or false otherwise.
 */
bool
xstrhassuffix (const string &s, const string &suffix)
{
  return g_str_has_suffix (s.c_str (), suffix.c_str ());
}

/**
 * @brief Assigns format to string.
 * @param s Resulting string.
 * @param format Format string.
 * @return The number of bytes assigned.
 */
int
xstrassign (string &s, const char *format, ...)
{
  va_list args;
  char *c_str = NULL;
  int n;

  va_start (args, format);
  n = g_vasprintf (&c_str, format, args);
  va_end (args);

  g_assert (n >= 0);
  g_assert_nonnull (c_str);
  s.assign (c_str);
  g_free (c_str);

  return n;
}

/**
 * @brief Builds string from format.
 * @param format Format string.
 * @return The resulting string.
 */
string
xstrbuild (const char *format, ...)
{
  va_list args;
  char *c_str = NULL;
  int n;
  string s;

  va_start (args, format);
  n = g_vasprintf (&c_str, format, args);
  va_end (args);

  g_assert (n >= 0);
  g_assert_nonnull (c_str);
  s.assign (c_str);
  g_free (c_str);

  return s;
}

/**
 * @brief Converts string to uppercase.
 */
string
xstrup (string s)
{
  gchar *dup = g_ascii_strup (s.c_str (), (gssize) s.size ());
  s.assign (dup);
  free (dup);
  return s;
}

/**
 * @brief Converts string to lowercase.
 */
string
xstrdown (string s)
{
  gchar *dup = g_ascii_strdown (s.c_str (), (gssize) s.size ());
  s.assign (dup);
  free (dup);
  return s;
}

/**
 * @brief Removes leading and trailing whitespace from string.
 */
string
xstrstrip (string s)
{
  gchar *dup = g_strdup (s.c_str ());
  s.assign (g_strstrip (dup));
  g_free (dup);
  return s;
}

/**
 * @brief Splits strings according to separator.
 * @param s String.
 * @param sep Separator.
 * @return The resulting vector.
 */
vector<string>
xstrsplit (const string &s, char sep)
{
  vector<string> result;
  stringstream ss (s);
  string tok;

  while (getline (ss, tok, sep))
    result.push_back (tok);

  return result;
}


// Paths -------------------------------------------------------------------

/**
 * @brief Returns the basename of path.
 */
string
xpathbasename (string path)
{
  gchar *dir = g_path_get_basename (path.c_str ());
  path.assign (dir);
  g_free (dir);
  return path;
}

/**
 * @brief Returns the dirname of path.
 */
string
xpathdirname (string path)
{
  gchar *dir = g_path_get_dirname (path.c_str ());
  path.assign (dir);
  g_free (dir);
  return path;
}

/**
 * @brief Checks if path is absolute.
 */
bool
xpathisabs (const string &path)
{
  return g_path_is_absolute (path.c_str ());
}

/**
 * @brief Checks if path is an URI.
 */
bool
xpathisuri (const string &path)
{
  gchar * dup  = g_uri_parse_scheme (path.c_str ());
  return (dup == NULL) ? false : (g_free (dup), true);
}

/**
 * @brief Makes path absolute.
 */
string
xpathmakeabs (string path)
{
  if (!xpathisabs (path))
    {
      gchar *cwd = g_get_current_dir ();
      gchar *dup = g_build_filename (cwd, path.c_str (), NULL);
      g_free (cwd);
      path.assign (dup);
      g_free (dup);
    }
  return path;
}

/**
 * @brief Builds a path from the given components.
 */
string
xpathbuild (const string &a, const string &b)
{
  string path;
  gchar *dup = g_build_filename (a.c_str (), b.c_str (), NULL);
  path.assign (dup);
  g_free (dup);
  return path;
}

/**
 * @brief Builds an absolute path from the given components.
 */
string
xpathbuildabs (const string &a, const string &b)
{
  return xpathmakeabs (xpathbuild (a, b));
}


// OpenGL ------------------------------------------------------------------

#if ENABLED (OPENGL)

/**
 * @brief gl_create_texture Creates a new uninitialized OpenGL texture
 */
void
gl_create_texture (GLuint *gltex)
{
  glGenTextures (1, gltex);
  glBindTexture (GL_TEXTURE_2D, *gltex);

  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  CHECK_GL_ERROR
}

/**
 * @brief gl_create_texture Creates a new OpenGL texture and initializes it with
 */
void
gl_create_texture (GLuint *gltex, int tex_w, int tex_h, unsigned char *data)
{
  gl_create_texture (gltex);
  glTexImage2D (GL_TEXTURE_2D, 0, 4,
                tex_w, tex_h, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);

  CHECK_GL_ERROR
}

/**
 * @brief gl_delete_texture Deletes the
 */
void
gl_delete_texture (GLuint *gltex)
{
  if (*gltex != (GLuint) -1)
    {
      glDeleteTextures (1, gltex);
    }

  CHECK_GL_ERROR
}

void
gl_update_texture (GLuint gltex, int tex_w, int tex_h, unsigned char *data)
{
  glActiveTexture (gltex);
  glTexImage2D (GL_TEXTURE_2D, 0, 4,
                tex_w, tex_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

  CHECK_GL_ERROR
}

void
gl_update_subtexture (GLuint gltex,
                      int xoffset, int yoffset, int width, int height,
                      unsigned char *data)
{
  glBindTexture (GL_TEXTURE_2D, gltex);
  glTexSubImage2D (GL_TEXTURE_2D,
                   0,
                   xoffset,
                   yoffset,
                   width,
                   height,
                   GL_BGRA,
                   GL_UNSIGNED_BYTE,
                   data);

  CHECK_GL_ERROR
}

void
gl_update_texture (GLuint gltex, cairo_surface_t *surf)
{
  int tex_w = cairo_image_surface_get_width (surf);
  int tex_h = cairo_image_surface_get_height (surf);
  unsigned char* data = cairo_image_surface_get_data (surf);

  gl_update_subtexture (gltex, 0, 0, tex_w, tex_h, data);
}

#endif

