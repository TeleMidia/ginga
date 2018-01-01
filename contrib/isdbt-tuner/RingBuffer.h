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

#ifndef HAVE_RING_BUFFER__
#define HAVE_RING_BUFFER__

#include "ginga.h"

GINGA_BEGIN_DECLS

// Posix Ring Buffer implementation
//

#define report_exceptional_condition() abort ()

struct ring_buffer
{
  void *address;

  unsigned long count_bytes;
  unsigned long write_offset_bytes;
  unsigned long read_offset_bytes;
};

void ring_buffer_create (struct ring_buffer *buffer, unsigned long order);

void ring_buffer_free (struct ring_buffer *buffer);

void *ring_buffer_write_address (struct ring_buffer *buffer);

void ring_buffer_write_advance (struct ring_buffer *buffer,
                                unsigned long count_bytes);

void *ring_buffer_read_address (struct ring_buffer *buffer);

void ring_buffer_read_advance (struct ring_buffer *buffer,
                               unsigned long count_bytes);

unsigned long ring_buffer_count_bytes (struct ring_buffer *buffer);

unsigned long ring_buffer_count_free_bytes (struct ring_buffer *buffer);

void ring_buffer_clear (struct ring_buffer *buffer);

GINGA_END_DECLS

#endif /* HAVE_RING_BUFFER__ */
