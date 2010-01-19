/******************************************************************************
Este arquivo eh parte da implementacao do ambiente declarativo do middleware
Ginga (Ginga-NCL).

Direitos Autorais Reservados (c) 1989-2007 PUC-Rio/Laboratorio TeleMidia

Este programa eh software livre; voce pode redistribui-lo e/ou modificah-lo sob
os termos da Licen�a Publica Geral GNU versao 2 conforme publicada pela Free
Software Foundation.

Este programa eh distribu�do na expectativa de que seja util, porem, SEM
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

#include "../../../include/Graphics.h"
#include "../../../include/IOHandler.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace io {
	VideoProvider::VideoProvider(char* mrl) {
		this->decoder = IOHandler::createVideoProvider(mrl);
	}

	VideoProvider::~VideoProvider() {
#if HAVE_DIRECTFB
		IDirectFBVideoProvider* vp;
		vp = (IDirectFBVideoProvider*)decoder;

		DFBCHECK(vp->Stop(vp));
		IOHandler::releaseVideoProvider(this);
		decoder = NULL;
#endif
	}

	void* VideoProvider::getContent() {
		return decoder;
	}

	Surface* VideoProvider::getPerfectSurface() {
#if HAVE_DIRECTFB
		IDirectFBVideoProvider* vp;
		IDirectFBSurface* s = NULL;
		DFBSurfaceDescription dsc;

		vp = (IDirectFBVideoProvider*)decoder;
		DFBCHECK(vp->GetSurfaceDescription(vp, &dsc));
		return new Surface(IOHandler::createSurface(&dsc));
#else
		return NULL;
#endif
	}

	void VideoProvider::dynamicRenderCallBack(void* surface) {
#if HAVE_DIRECTFB
		IDirectFBSurface* s = NULL;
		IDirectFBWindow* w = NULL;

		Surface* someSurface = (Surface*)surface;
		IDirectFBSurface* frame = NULL;

		if (someSurface == NULL) {
			return;
		}
		frame = (IDirectFBSurface*)(someSurface->getContent());

		if (frame == NULL) {
			return;
		}

		Window* someWindow = (Window*)(someSurface->getParent());
		if (someWindow != NULL) {
			bool fitTo;
			bool stretchTo;
			int winWidth, winHeight;
			int surWidth, surHeight;

			DFBCHECK(frame->GetSize(frame, &surWidth, &surHeight));

			someWindow->lock();
			fitTo = someWindow->getFit();
			stretchTo = someWindow->getStretch();
			w = (IDirectFBWindow*)(someWindow->getContent());
			DFBCHECK(w->GetSize(w, &winWidth, &winHeight));
			DFBCHECK(w->GetSurface(w, &s));

			if ((winWidth != surWidth || winHeight != surHeight) && fitTo) {
				if (stretchTo) {
					DFBCHECK(s->StretchBlit(
						    s,
						    frame,
						    NULL,
						    NULL));

					DFBCHECK(s->Flip(
							s, NULL, (DFBSurfaceFlipFlags)DSFLIP_BLIT));

				} else {
					Surface* sur;
					IDirectFBSurface* s2;

					sur = new Surface(someWindow->getW(), someWindow->getH());
					s2 = (IDirectFBSurface*)(sur->getContent());

					DFBCHECK(s2->StretchBlit(
						    s2,
						    s,
						    NULL,
						    NULL));

					DFBCHECK(s2->Flip(
							s2, NULL, (DFBSurfaceFlipFlags)DSFLIP_BLIT));

					DFBCHECK(s->Blit(s, s2, NULL, 0, 0));
					DFBCHECK(s->Flip(
							s, NULL, (DFBSurfaceFlipFlags) DSFLIP_BLIT));

					delete sur;
				}

			} else {
				DFBCHECK(s->Blit(s, frame, NULL, 0, 0));
				DFBCHECK(s->Flip(s, NULL, (DFBSurfaceFlipFlags) DSFLIP_BLIT));
			}
			someWindow->unlock();

		} else if (frame != NULL) {
			DFBCHECK(frame->Blit(frame, frame, NULL, 0, 0));
		}
#endif
	}

	void VideoProvider::getOriginalResolution(int* height, int* width) {
#if HAVE_DIRECTFB
		IDirectFBVideoProvider* vp;
		DFBSurfaceDescription dsc;

		if (decoder != NULL) {
			vp = (IDirectFBVideoProvider*)decoder;
			DFBCHECK(vp->GetSurfaceDescription(vp, &dsc));
			*width = dsc.width;
			*height = dsc.height;
		}
#endif
	}

	double VideoProvider::getTotalMediaTime() {
#if HAVE_DIRECTFB
		IDirectFBVideoProvider* vp;
		DFBResult res;
		double size;

		if (decoder != NULL) {
			vp = (IDirectFBVideoProvider*)decoder;
			res = vp->GetLength(vp, &size);
			if (res == DFB_OK) {
				return size;
			}
		}
#endif
		return 0;
	}

	double VideoProvider::getMediaTime() {
#if HAVE_DIRECTFB
		IDirectFBVideoProvider* vp;
		DFBResult res;
		double pos;

		if (decoder != NULL) {
			vp = (IDirectFBVideoProvider*)decoder;
			res = vp->GetPos(vp, &pos);
			if (res != DFB_EOF) {
				return pos;
			}
		}
#endif
		return -1;
	}

	void VideoProvider::setMediaTime(double pos) {
#if HAVE_DIRECTFB
		IDirectFBVideoProvider* vp;

		if (decoder != NULL) {
			vp = (IDirectFBVideoProvider*)decoder;
			DFBCHECK(vp->SeekTo(vp, pos));
		}
#endif
	}

	void VideoProvider::playOver(Surface* surface, bool hasVisual) {
#if HAVE_DIRECTFB
		IDirectFBVideoProvider* vp;
		IDirectFBSurface* s;

		surface->clear();

		vp = (IDirectFBVideoProvider*)decoder;
		s = (IDirectFBSurface*)(surface->getContent());

		if (hasVisual) {
			DFBCHECK(vp->PlayTo(
					vp, s, NULL, dynamicRenderCallBack, (void*)surface));

		} else {
			DFBCHECK(vp->PlayTo(vp, s, NULL, NULL, NULL));
		}
#endif
	}

	void VideoProvider::resume(Surface* surface, bool hasVisual) {
		playOver(surface, hasVisual);
	}

	void VideoProvider::pause() {
#if HAVE_DIRECTFB
		stop();
#endif
	}

	void VideoProvider::stop() {
#if HAVE_DIRECTFB
		IDirectFBVideoProvider* vp;
		vp = (IDirectFBVideoProvider*)decoder;
		DFBCHECK(vp->Stop(vp));
#endif
	}

	void VideoProvider::setSoundLevel(float level) {
#if HAVE_DIRECTFB
		IDirectFBVideoProvider* vp;
		vp = (IDirectFBVideoProvider*)decoder;
		DFBCHECK(vp->SetVolume(vp, level));
#endif
	}
}
}
}
}
}
}
