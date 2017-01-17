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

#include "config.h"
#include "SDLBpgDecoder.h"

extern "C" {
#include <stdint.h>
#include <libbpg.h>
}

using namespace std;

GINGA_MB_BEGIN

   
    SDLBpgDecoder::SDLBpgDecoder(string filename) {

        filePath.assign(filename);
       
    }

    SDLBpgDecoder::~SDLBpgDecoder() {

    }

    SDL_Surface *SDLBpgDecoder::decode() {
        BPGDecoderContext *s;
        BPGImageInfo bi_s, *bi = &bi_s;
        uint8_t *buf;
        int len, y;
        SDL_Surface *img;
        uint32_t rmask, gmask, bmask, amask;
        int i;

        FILE *f = fopen(filePath.c_str(), "r");
        fseek(f, 0, SEEK_END);
        len = ftell(f);
        fseek(f, 0, SEEK_SET);
        if (len < 0)
            return NULL;
        buf = (uint8_t *) malloc(len);
        if (!buf)
            return NULL;
        if (fread(buf, 1, len, f) != len)
            return NULL;

        fclose(f);
   
        s = bpg_decoder_open();
        if (bpg_decoder_decode(s, buf, len) < 0)
            return NULL;
       
        bpg_decoder_get_info(s, bi);
       
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
        rmask = 0xff000000;
        gmask = 0x00ff0000;
        bmask = 0x0000ff00;
        amask = 0x000000ff;
#else
        rmask = 0x000000ff;
        gmask = 0x0000ff00;
        bmask = 0x00ff0000;
        amask = 0xff000000;
#endif
       
        if (bpg_decoder_start(s, BPG_OUTPUT_FORMAT_RGBA32) < 0)
            return NULL;
       

        img = SDL_CreateRGBSurface(0, bi->width, bi->height, 32,
                                   rmask, gmask, bmask, amask);
        if (!img)
            return NULL;
       
        SDL_LockSurface(img);
        for(y = 0; y < bi->height; y++) {
            bpg_decoder_get_line(s, (uint8_t *)img->pixels + y * img->pitch);
        }
        SDL_UnlockSurface(img);
       
        bpg_decoder_close(s);
        return img;
    }

GINGA_MB_END
