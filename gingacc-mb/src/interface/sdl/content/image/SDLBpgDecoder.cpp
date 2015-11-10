/******************************************************************************
Este arquivo eh parte da implementacao do ambiente declarativo do middleware
Ginga (Ginga-NCL).

Direitos Autorais Reservados (c) 1989-2007 PUC-Rio/Laboratorio TeleMidia

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

Copyright: 1989-2015 PUC-RIO/LABORATORIO TELEMIDIA, All Rights Reserved.

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

#include <stdint.h>

#include "mb/interface/sdl/content/image/SDLBpgDecoder.h"
#include "config.h"

extern "C"{
#include <libbpg.h>
}


using namespace std;


namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace mb {
    
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
}
}
}
}
}
}

