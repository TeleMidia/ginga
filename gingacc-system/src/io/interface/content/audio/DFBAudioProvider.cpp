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

#include "system/io/LocalDeviceManager.h"
#include "system/io/interface/content/audio/DFBAudioProvider.h"
#include "system/io/interface/output/dfb/DFBSurface.h"

/* macro for a safe call to DirectFB functions */
#ifndef DFBCHECK
#define DFBCHECK(x...)                                            \
{                                                                 \
	DFBResult err = x;                                            \
	if (err != DFB_OK) {                                          \
		fprintf( stderr, "%s <%d>: \n\t", __FILE__, __LINE__ );   \
		DirectFBError( #x, err );                                 \
	}                                                             \
}
#endif /*DFBCHECK*/

#if DFBTM_PATCH
		static int dapRefs = 0;
#endif

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace system {
namespace io {
	DFBAudioProvider::DFBAudioProvider(const char* mrl) {
		IDirectFB* dfb;

#if DFBTM_PATCH
		dapRefs++;
#endif

		decoder = NULL;

		if (mrl != NULL) {
			dfb = (IDirectFB*)(LocalDeviceManager::getInstance()->getGfxRoot());
			DFBCHECK(dfb->CreateVideoProvider(dfb, mrl, &decoder));
		}
	}

	DFBAudioProvider::~DFBAudioProvider() {
		if (decoder != NULL) {
			decoder->Release(decoder);
			decoder = NULL;
		}

#if DFBTM_PATCH
		dapRefs--;
		if (dapRefs == 0) {
			DirectReleaseInterface("IDirectFBVideoProvider");
		}
#endif //DFBTM_PATCH
	}

	void* DFBAudioProvider::getContent() {
		return decoder;
	}

	ISurface* DFBAudioProvider::getPerfectSurface() {
		DFBSurfaceDescription dsc;

		DFBCHECK(decoder->GetSurfaceDescription(decoder, &dsc));
		return new DFBSurface(
				LocalDeviceManager::getInstance()->createSurface(&dsc));
	}

	void DFBAudioProvider::dynamicRenderCallBack(void* dec) {
		IDirectFBSurface* s = NULL;
		IDirectFBWindow* w = NULL;

		ISurface* someSurface = (ISurface*)dec;
		IDirectFBSurface* frame = NULL;

		if (someSurface == NULL) {
			return;
		}
		frame = (IDirectFBSurface*)(someSurface->getContent());

		if (frame == NULL) {
			return;
		}

		IWindow* someWindow = (IWindow*)(someSurface->getParent());
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
					ISurface* sur;
					IDirectFBSurface* s2;

					sur = new DFBSurface(
							someWindow->getW(), someWindow->getH());

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
	}

	double DFBAudioProvider::getTotalMediaTime() {
		if (decoder == NULL) {
			return 0;
		}

		double totalTime;

		decoder->GetLength(decoder, &totalTime);
		return totalTime;
	}

	double DFBAudioProvider::getMediaTime() {
		if (decoder == NULL) {
			return -1;
		}

		double currentTime;

		decoder->GetPos(decoder, &currentTime);
		return currentTime;
	}

	void DFBAudioProvider::setMediaTime(double pos) {
		if (decoder == NULL) {
			return;
		}

		decoder->SeekTo(decoder, pos);
	}

	void DFBAudioProvider::playOver(
			ISurface* surface, bool hasVisual, IProviderListener* listener) {
		if (decoder == NULL) {
			cout << "DFBAudioProvider::playOver decoder = NULL" << endl;
			return;
		}

		IDirectFBSurface* s;

		s = (IDirectFBSurface*)(surface->getContent());
		if (hasVisual) {
			DFBCHECK(decoder->PlayTo(
					decoder, s, NULL, dynamicRenderCallBack, (void*)surface));

		} else {
			DFBCHECK(decoder->PlayTo(decoder, s, NULL, NULL, NULL));
		}
	}

	void DFBAudioProvider::pause() {
		stop();
	}

	void DFBAudioProvider::resume(ISurface* surface, bool hasVisual) {
		if (decoder == NULL) {
			return;
		}

		playOver(surface, hasVisual);
	}

	void DFBAudioProvider::stop() {
		if (decoder == NULL) {
			return;
		}

		DFBCHECK(decoder->Stop(decoder));
	}

	void DFBAudioProvider::setSoundLevel(float level) {
		if (decoder == NULL) {
			return;
		}

		decoder->SetVolume(decoder, level);
	}

	bool DFBAudioProvider::releaseAll() {
		return false;
	}

	void DFBAudioProvider::getOriginalResolution(int* height, int* width) {

	}
}
}
}
}
}
}
}

extern "C" ::br::pucrio::telemidia::ginga::core::system::io::
		IContinuousMediaProvider* createDFBAudioProvider(const char* mrl) {

	return (new ::br::pucrio::telemidia::ginga::core::system::io::
			DFBAudioProvider(mrl));
}

extern "C" void destroyDFBAudioProvider(
		::br::pucrio::telemidia::ginga::core::system::io::
		IContinuousMediaProvider* cmp) {

	delete cmp;
}
