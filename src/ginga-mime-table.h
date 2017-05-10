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

#ifndef GINGA_MIME_TABLE_H
#define GINGA_MIME_TABLE_H

#include "ginga.h"

typedef struct _GingaMimeTable
{
  const char *ext;
  const char *mime;
} GingaMimeTable;

static const GingaMimeTable ginga_mime_table[] = {
  /* KEEP THIS SORTED ALPHABETICALLY */
  {"ac3", "audio/ac3"},
  {"avi", "video/x-msvideo"},
  {"bmp", "image/bmp"},
  {"bpg", "image/x-bpg"},
  {"class", "application/x-ginga-NCLet"},
  {"css", "text/css"},
  {"gif", "image/gif"},
  {"htm", "text/html"},
  {"html", "text/html"},
  {"jpeg", "image/jpeg"},
  {"jpg", "image/jpeg"},
  {"lua", "application/x-ginga-NCLua"},
  {"mov", "video/quicktime"},
  {"mp2", "audio/mp2"},
  {"mp3", "audio/mp3"},
  {"mp4", "video/mp4"},
  {"mpa", "audio/mpa"},
  {"mpeg", "video/mpeg"},
  {"mpg", "video/mpeg"},
  {"mpv", "video/mpv"},
  {"ncl", "application/x-ginga-ncl"},
  {"oga", "audio/ogg"},
  {"ogg", "audio/ogg"},
  {"ogv", "video/ogg"},
  {"opus", "audio/ogg"},
  {"png", "image/png"},
  {"smil", "application/smil"},
  {"spx", "audio/ogg"},
  {"srt", "text/srt"},
  {"ssml", "application/ssml+xml"},
  {"svg", "image/svg+xml"},
  {"svgz", "image/svg+xml"},
  {"ts", "video/mpeg"},
  {"txt", "text/plain"},
  {"wav", "audio/basic"},
  {"webp", "image/x-webp"},
  {"wmv", "video/x-ms-wmv"},
  {"xlet", "application/x-ginga-NCLet"},
  {"xlt", "application/x-ginga-NCLet"},
  {"xml", "text/xml"},
};

static G_GNUC_PURE int
ginga_mime_table_compar (const void *e1, const void *e2)
{
  const GingaMimeTable *c1;
  const GingaMimeTable *c2;

  c1 = (const GingaMimeTable *) e1;
  c2 = (const GingaMimeTable *) e2;

  return g_ascii_strcasecmp (c1->ext, c2->ext);
}

/* Gets the mime-type associated with the given extension.  If EXT is in
   mime table, stores its mime-type *MIME and returns true, otherwise
   returns false.  */
static inline gboolean
ginga_mime_table_index (const char *ext, const char **mime)
{
  GingaMimeTable key;
  GingaMimeTable *match;

  key.ext = ext;
  match = (GingaMimeTable *)
    bsearch (&key, ginga_mime_table, G_N_ELEMENTS (ginga_mime_table),
             sizeof (*ginga_mime_table), ginga_mime_table_compar);
  if (match == NULL)
    return FALSE;

  *mime = match->mime;
  return TRUE;
}

#endif /* GINGA_MIME_TABLE_H */
