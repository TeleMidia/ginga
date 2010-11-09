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

Copyright: 1989-2007 PUC-RIO/LABORATORIO TELEMIDIA, All Rights Reserved.

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

#ifndef XineVideoProvider_H_
#define XineVideoProvider_H_

#include "../IContinuousMediaProvider.h"

using namespace ::br::pucrio::telemidia::ginga::core::system::io;

#ifndef METRONOM_INTERNAL
#define METRONOM_INTERNAL 1
#endif

#ifdef __cplusplus
extern "C" {
#endif
#include <stdio.h>
#include <directfb.h>
#include <xine.h>
#include <xine/xineutils.h>
#include <xine/xine_internal.h>
#ifdef __cplusplus
}
#endif

struct xine_renderer_container {
	bool isValid;
	ISurface* surface;
	IProviderListener* listener;
	xine_t* xine;
	xine_video_port_t* vo;
	xine_audio_port_t* ao;
	xine_stream_t* stream;
	char* config;
};

typedef struct xine_renderer_container XineRendererContainer;

/******************** Xine-DFB Driver Specific Data ***************************/
typedef void (*DVOutputCallback) ( void                  *cdata,
                                   int                    width,
                                   int                    height,
                                   double                 ratio,
                                   DFBSurfacePixelFormat  format,
                                   DFBRectangle          *dest_rect );

typedef struct {
     IDirectFBSurface *destination;
     IDirectFBSurface *subpicture;

     DVOutputCallback  output_cb;
     void             *output_cdata;

     DVFrameCallback  frame_cb;
     void            *frame_cdata;
} dfb_visual_t;
/******************** Xine-DFB Driver Specific Data ***************************/

#include <set>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace system {
namespace io {
	class XineVideoProvider : public IContinuousMediaProvider {
		protected:
			XineRendererContainer* rContainer;
			dfb_visual_t visual;
			int resumePos;
			int startPos;

		public:
			XineVideoProvider(const char* mrl);
			virtual ~XineVideoProvider();

		private:
			bool initializeXine(const char* mrl);
			void initializeAudio();

		public:
			void setAVPid(int aPid, int vPid);
			void* getContent();
			void feedBuffers();

		private:
			void getVideoSurfaceDescription(DFBSurfaceDescription* dsc);

		public:
			ISurface* getPerfectSurface();
			bool checkVideoResizeEvent(ISurface* frame);

		private:
			static void dynamicRenderCallBack(void* surface);
			static void outputCallBack(
					void                  *cdata,
                    int                    width,
                    int                    height,
                    double                 ratio,
                    DFBSurfacePixelFormat  format,
                    DFBRectangle          *dest_rect);

		public:
			void getOriginalResolution(int* width, int* height);
			double getTotalMediaTime();
			int64_t getVPts();
			double getMediaTime();
			void setMediaTime(double pos);

		private:
			bool updateVisualData(ISurface* surface);

		public:
			void playOver(
					ISurface* surface,
					bool hasVisual=true, IProviderListener* listener=NULL);

			void resume(ISurface* surface, bool hasVisual=true);
			void pause();
			void stop();
			void setSoundLevel(float level);
			bool releaseAll();
	};
}
}
}
}
}
}
}

#endif /*XineVideoProvider_H_*/
