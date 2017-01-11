/******************************************************************************
Este arquivo eh parte da implementacao do ambiente declarativo do middleware
Ginga (Ginga-NCL).

Direitos Autorais Reservados (c) 1989-2013 PUC-Rio/Laboratorio TeleMidia

Este programa eh software livre; voce pode redistribui-lo e/ou modificah-lo sob
os termos da Licenca Publica Geral GNU versao 2 conforme publicada pela Free
Software Foundation.

Este programa eh distribuido na expectativa de que seja util, porem, SEM
NENHUMA GARANTIA; nem mesmo a garantia implicita de COMERCIABILIDADE OU
ADEQUACAO A UMA FINALIDADE ESPECIFICA. Consulte a Licenca Publica Geral do
GNU versao 2 para mais detalhes.

Voce deve ter recebido uma copia da Licenca Publica Geral do GNU versao 2 junto
com este programa; se nao, escreva para a Free Software Foundation, Inc., no
endereco 59 Temple Street, Suite 330, Boston, MA 02111-1307 USA.

Para maiores informacoes:
ncl @ telemidia.puc-rio.br
http://www.ncl.org.br
http://www.ginga.org.br
http://www.telemidia.puc-rio.br
******************************************************************************
This file is part of the declarative environment of middleware Ginga (Ginga-NCL)

Copyright: 1989-2013 PUC-RIO/LABORATORIO TELEMIDIA, All Rights Reserved.

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License version 2 as published by
the Free Software Foundation.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License version 2 for more
details.

You should have received a copy of the GNU General Public License version 2
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA

For further information contact:
ncl @ telemidia.puc-rio.br
http://www.ncl.org.br
http://www.ginga.org.br
http://www.telemidia.puc-rio.br
*******************************************************************************/

#include "RingBuffer.h"

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
 
    address =
	mmap (buffer->address, buffer->count_bytes, PROT_READ | PROT_WRITE,
	      MAP_FIXED | MAP_SHARED, file_descriptor, 0);
 
    if (address != buffer->address)
	report_exceptional_condition ();
 
    address = mmap (buffer->address + buffer->count_bytes,
		    buffer->count_bytes, PROT_READ | PROT_WRITE,
		    MAP_FIXED | MAP_SHARED, file_descriptor, 0);
 
    if (address != buffer->address + buffer->count_bytes)
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
    
  return buffer->address + buffer->write_offset_bytes;
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
  return buffer->address + buffer->read_offset_bytes;
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

