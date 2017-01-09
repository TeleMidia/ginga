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

#include <librsvg/rsvg.h>
#include <cairo.h>

#include "mb/interface/sdl/content/image/SDLSvgDecoder.h"
#include "config.h"


using namespace std;


namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace mb {
   
    SDLSvgDecoder::SDLSvgDecoder(string filename) {

        filePath.assign(filename);
       
    }

    SDLSvgDecoder::~SDLSvgDecoder() {

    }

    SDL_Surface *SDLSvgDecoder::decode(int width, int height) {
        RsvgHandle* h;
        RsvgDimensionData dim;
        GError* e = NULL;
        cairo_surface_t *cairoSurface;
        cairo_t *cairoState;

        /* RSVG initiation */
        h = rsvg_handle_new_from_file(filePath.c_str(), &e);

        rsvg_handle_get_dimensions (h, &dim);

        // precisamos ver o quanto vamos escalar... Usando 1920x1080 como pior caso...
        // Ver: http://www.svgopen.org/2009/presentations/62-Rendering_SVG_graphics_with_libSDL_a_crossplatform_multimedia_library/index.pdf
        double scale = (dim.width > dim.height)? (double) 1920 / dim.width : (double) 1080 / dim.height;

        int x = floor(dim.width * scale) + 1;
        int y = floor(dim.height * scale) + 1;
        int stride = x * 4; // ARGB

        /* Cairo Initiation */
        uint8_t *image = (uint8_t *) malloc (stride * y); // ARGB uses 4 bytes / pixel

        // here the correct would be...
        cairoSurface = cairo_image_surface_create_for_data (image, CAIRO_FORMAT_ARGB32, x, y, stride);
        cairoState = cairo_create (cairoSurface);

        cairo_scale(cairoState, scale, scale);

        rsvg_handle_render_cairo (h, cairoState);


        // Use the following line for debug purposes
//        cairo_surface_write_to_png (cairoSurface, "/tmp/out.png");

        // Match ARGB32 format masks
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
        Uint32 rmask = 0x0000ff00;
        Uint32 gmask = 0x00ff0000;
        Uint32 bmask = 0xff000000;
        Uint32 amask = 0x000000ff;
#else
        Uint32 rmask = 0x00ff0000;
        Uint32 gmask = 0x0000ff00;
        Uint32 bmask = 0x000000ff;
        Uint32 amask = 0xff000000;
#endif

        SDL_Surface *sdlSurface = SDL_CreateRGBSurfaceFrom ( (void *) image, x, y, 32, stride, rmask, gmask, bmask, amask);

        cairo_surface_destroy (cairoSurface);
        cairo_destroy (cairoState);

        return sdlSurface;
    }


}
}
}
}
}
}

