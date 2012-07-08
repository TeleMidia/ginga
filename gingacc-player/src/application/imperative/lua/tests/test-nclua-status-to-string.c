/* test-nclua-status-to-string.c -- Check nclua_status_to_string.
   Copyright (C) 2012 PUC-Rio/Laboratorio TeleMidia

This program is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the Free
Software Foundation; either version 2 of the License, or (at your option)
any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc., 51
Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.  */

#include "tests.h"

#define check(status, message)                          \
  do {                                                  \
    const char *s = nclua_status_to_string (status);    \
    assert (strcmp (s, message) == 0);                  \
  } while (0)

int
main (void)
{
  const char *unknown = "<unknown error status>";

  check (NCLUA_STATUS_SUCCESS,           "no error has occurred");
  check (NCLUA_STATUS_NO_MEMORY,         "out of memory");
  check (NCLUA_STATUS_NULL_POINTER,      "NULL pointer");
  check (NCLUA_STATUS_INVALID_LUA_STATE, "invalid lua_State");
  check (NCLUA_STATUS_INVALID_STATUS,    "invalid value for nclua_status_t");
  check (NCLUA_STATUS_LAST_STATUS,       unknown);
  check (-1,                             unknown);
  check (NCLUA_STATUS_LAST_STATUS + 1,   unknown);

  exit (EXIT_SUCCESS);
}
