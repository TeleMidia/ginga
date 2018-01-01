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
#include "DsmccCrc.h"

GINGA_PRAGMA_DIAG_IGNORE (-Wsign-conversion)

GINGA_DATAPROC_BEGIN

Crc32::Crc32 ()
{
  unsigned int i = 0;
  unsigned int j = 0;
  unsigned int c = 0;

  for (i = 0; i < 0x100; i++)
    {
      c = (i << 24);

      for (j = 0; j < 8; j++)
        {
          if (c & 0x80000000)
            {
              c = (c << 1) ^ 0x04C11DB7;
            }
          else
            {
              c = (c << 1);
            }
        }

      dsmcc_crc_tab[i] = c;
    }
}

Crc32::~Crc32 () {}

unsigned int
Crc32::crc (char *data, unsigned int len)
{
  unsigned int i = 0;
  unsigned int crc = 0xFFFFFFFF;

  for (i = 0; i < len; i++)
    {
      crc = (crc << 8) ^ dsmcc_crc_tab[((crc >> 24) ^ data[i]) & 0xFF];
    }

  return crc;
}

GINGA_DATAPROC_END
