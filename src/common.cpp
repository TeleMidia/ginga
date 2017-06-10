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

#include "ginga.h"


// Logging -----------------------------------------------------------------

// Parses G_STRFUNC to generate a log prefix.
string
__ginga_strfunc (const string &strfunc)
{
  string result;
  size_t i, j;

  i = strfunc.find ("(");
  result = strfunc.substr (0, i);

  i = result.rfind ("::");
  if (i == std::string::npos)
    {
      result += "::";
      goto done;
    }

  j = (result.substr (0, i - 1)).rfind ("::");
  if (j == std::string::npos)
    goto done;

  result = result.substr (j + 2, i - 1);

 done:
  return result + "()";
}


// Strings -----------------------------------------------------------------

// Converts string to double.
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

  set_if_nonnull (dp, d);
  return true;
}

// Converts string to gint64.
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

  set_if_nonnull (ip, i);
  return true;
}

// Converts string to guint64.
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

  set_if_nonnull (ip, u);
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

_GINGA_XSTRTO_DEFN  (int,    G_MININT,    G_MAXINT)
_GINGA_XSTRTO_DEFN  (int8,   G_MININT8,   G_MAXINT8)
_GINGA_XSTRTO_DEFN  (int64,  G_MININT64,  G_MAXINT64)

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

// Checks if string is of the form "\s*\d+%.*".
bool
xstrispercent (const string &s)
{
  gchar *end;
  return (g_strtod (s.c_str (), &end), *end == '%');
}

// Converts a number or percentage string to a number.
gdouble
xstrtodorpercent (const string &s, bool *perc)
{
  gchar *end;
  gdouble x = g_strtod (s.c_str (), &end);
  if (*end == '%')
    {
      set_if_nonnull (perc, true);
      return x / 100.;
    }
  else
    {
      set_if_nonnull (perc, false);
      return x;
    }
}

// Converts a time string to a number in nanoseconds.
// The following formats are supported: "NNs" or "NN:NN:NN".
bool
_xstrtotime (const string &s, GingaTime *time)
{
  gchar *dup;
  gchar *end;
  double x;

  dup = g_strdup (s.c_str ());
  g_strchomp (dup);

  x = g_strtod (dup, &end);
  if (*end == '\0' || streq (end, "s"))
    goto success;

  if (*end != ':')
    goto failure;

  end++;
  x = 3600 * x + 60 * g_strtod (end, &end);
  if (*end != ':')
    goto failure;

  end++;
  x += g_strtod (end, &end);
  if (*end != '\0')
    goto failure;

 success:
  g_free (dup);
  set_if_nonnull (time, (GingaTime)(x * GINGA_SECOND));
  return true;

 failure:
  g_free (dup);
  return false;
}

// Asserted version of the previous function.
GingaTime
xstrtotime (const string &s)
{
  GingaTime t;
  g_assert (_xstrtotime (s, &t));
  return t;
}

// Compares two strings ignoring case.
int
xstrcasecmp (const string &s1, const string &s2)
{
  return g_ascii_strcasecmp (s1.c_str (), s2.c_str ());
}

// Assigns format to string.
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

// Builds string from format.
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

// Converts string to uppercase.
string
xstrup (string s)
{
  gchar *dup = g_ascii_strup (s.c_str (), (gssize) s.size ());
  s.assign (dup);
  free (dup);
  return s;
}

// Converts string to lowercase.
string
xstrdown (string s)
{
  gchar *dup = g_ascii_strdown (s.c_str (), (gssize) s.size ());
  s.assign (dup);
  free (dup);
  return s;
}

// Removes leading and trailing whitespace from string.
string
xstrchomp (string s)
{
  gchar *dup = g_strdup (s.c_str ());
  g_strchomp (dup);
  s.assign (dup);
  g_free (dup);
  return s;
}

// Replaces all the occurences of <find_what> in the string <str> with the
// string <replace_with>
void
xstrreplaceall (string &str, const string &find_what,
                const string &replace_with)
{
  string::size_type pos = 0;
  while ((pos = str.find (find_what, pos)) != string::npos)
    {
      str.erase (pos, find_what.length ());
      str.insert (pos, replace_with);
      pos += replace_with.length ();
    }
}

// Splits strings according to delimiter.
vector<string>
xstrsplit (const string &s, char delim)
{
  vector<string> result;
  stringstream ss (s);
  string tok;

  while(getline (ss, tok, delim))
    result.push_back (tok);

  return result;
}


// Paths -------------------------------------------------------------------

// Returns the basename of path.
string
xpathbasename (string path)
{
  gchar *dir = g_path_get_basename (path.c_str ());
  path.assign (dir);
  g_free (dir);
  return path;
}

// Returns the dirname of path.
string
xpathdirname (string path)
{
  gchar *dir = g_path_get_dirname (path.c_str ());
  path.assign (dir);
  g_free (dir);
  return path;
}

// Returns true if path is absolute.
bool
xpathisabs (const string &path)
{
  return g_path_is_absolute (path.c_str ());
}

// Returns true if path is an URI.
bool
xpathisuri (const string &path)
{
  gchar * dup  = g_uri_parse_scheme (path.c_str ());
  return (dup == NULL) ? false : (g_free (dup), true);
}

// Makes path absolute.
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

// Builds a path.
string
xpathbuild (const string &a, const string &b)
{
  string path;
  gchar *dup = g_build_filename (a.c_str (), b.c_str (), NULL);
  path.assign (dup);
  g_free (dup);
  return path;
}

// Builds an absolute path.
string
xpathbuildabs (const string &a, const string &b)
{
  return xpathmakeabs (xpathbuild (a, b));
}
