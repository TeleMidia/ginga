/* nclua-status.c -- nclua_status_t handling.
   Copyright (C) 2006-2012 PUC-Rio/Laboratorio TeleMidia

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

#include "nclua.h"
#include "nclua-private.h"

/* Provides a human-readable description of a nclua_status_t.
   Returns a string representation of STATUS.  */

const char *
nclua_status_to_string (nclua_status_t status)
{
  switch (status)
    {
    case NCLUA_STATUS_SUCCESS:
      return "no error has occurred";

    case NCLUA_STATUS_NO_MEMORY:
      return "out of memory";

    case NCLUA_STATUS_NULL_POINTER:
      return "NULL pointer";

    case NCLUA_STATUS_FILE_NOT_FOUND:
      return "file not found";

    case NCLUA_STATUS_INVALID_STATUS:
      return "invalid value for nclua_status_t";

    case NCLUA_STATUS_LAST_STATUS:
    default:
      return "<unknown error status>";
    }
  ASSERT_NOT_REACHED;
}
