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

#ifndef TESTS_H
#define TESTS_H

#include "aux-ginga.h"
#include "ginga.h"

#include "Context.h"
#include "Document.h"
#include "Event.h"
#include "Media.h"
#include "MediaSettings.h"
#include "Parser.h"
#include "ParserLua.h"
#include "Switch.h"

#define PARSE_AND_START(fmt, doc, str)                                     \
  G_STMT_START                                                             \
  {                                                                        \
    string buf = str;                                                      \
    string errmsg;                                                         \
    *fmt = new Formatter (0);                                              \
    g_assert_nonnull (*fmt);                                               \
    if (!(*fmt)->start (buf.c_str (), buf.length (), &errmsg))             \
      {                                                                    \
        g_printerr ("*** Unexpected error: %s", errmsg.c_str ());          \
        g_assert_not_reached ();                                           \
      }                                                                    \
    *doc = (*fmt)->getDocument ();                                         \
    g_assert_nonnull (*doc);                                               \
  }                                                                        \
  G_STMT_END

#endif // TESTS_H
