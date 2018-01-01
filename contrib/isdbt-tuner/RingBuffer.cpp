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

#include "ginga.h"
#include "RingBuffer.h"

#ifndef _MSC_VER
#include <unistd.h>
#endif

GINGA_BEGIN_DECLS
#include <sys/mman.h>
GINGA_END_DECLS

GINGA_BEGIN_DECLS

void
ring_buffer_create (struct ring_buffer *buffer, unsigned long order)
{
  char path[] = "/dev/shm/ring-buffer-XXXXXX";
  int file_descriptor;
  void *address;
  int status;

  file_descriptor = mkstemp (path);
  if (file_descriptor < 0)
    report_exceptional_condition ();

  status = unlink (path);
  if (status)
    report_exceptional_condition ();

  buffer->count_bytes = 1UL << order;
  buffer->write_offset_bytes = 0;
  buffer->read_offset_bytes = 0;

  status = ftruncate (file_descriptor, buffer->count_bytes);
  if (status)
    report_exceptional_condition ();

  buffer->address = mmap (NULL, buffer->count_bytes << 1, PROT_NONE,
                          MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);

  if (buffer->address == MAP_FAILED)
    report_exceptional_condition ();

  address
      = mmap (buffer->address, buffer->count_bytes, PROT_READ | PROT_WRITE,
              MAP_FIXED | MAP_SHARED, file_descriptor, 0);

  if (address != buffer->address)
    report_exceptional_condition ();

  address = mmap ((void*)((gintptr)buffer->address + (gintptr)buffer->count_bytes),
                  buffer->count_bytes, PROT_READ | PROT_WRITE,
                  MAP_FIXED | MAP_SHARED, file_descriptor, 0);

  if (address != (void*)((gintptr)buffer->address + (gintptr)buffer->count_bytes))
    report_exceptional_condition ();

  status = close (file_descriptor);
  if (status)
    report_exceptional_condition ();
}

void
ring_buffer_free (struct ring_buffer *buffer)
{
  int status;

  status = munmap (buffer->address, buffer->count_bytes << 1);
  if (status)
    report_exceptional_condition ();
}

void *
ring_buffer_write_address (struct ring_buffer *buffer)
{
  return (void*)((gintptr)buffer->address + (gintptr)buffer->write_offset_bytes);
}

void
ring_buffer_write_advance (struct ring_buffer *buffer,
                           unsigned long count_bytes)
{
  buffer->write_offset_bytes += count_bytes;
}

void *
ring_buffer_read_address (struct ring_buffer *buffer)
{
  return (void*)((gintptr)buffer->address + (gintptr)buffer->read_offset_bytes);
}

void
ring_buffer_read_advance (struct ring_buffer *buffer,
                          unsigned long count_bytes)
{
  buffer->read_offset_bytes += count_bytes;

  if (buffer->read_offset_bytes >= buffer->count_bytes)
    {
      buffer->read_offset_bytes -= buffer->count_bytes;
      buffer->write_offset_bytes -= buffer->count_bytes;
    }
}

unsigned long
ring_buffer_count_bytes (struct ring_buffer *buffer)
{
  return buffer->write_offset_bytes - buffer->read_offset_bytes;
}

unsigned long
ring_buffer_count_free_bytes (struct ring_buffer *buffer)
{
  return buffer->count_bytes - ring_buffer_count_bytes (buffer);
}

void
ring_buffer_clear (struct ring_buffer *buffer)
{
  buffer->write_offset_bytes = 0;
  buffer->read_offset_bytes = 0;
}

GINGA_END_DECLS
