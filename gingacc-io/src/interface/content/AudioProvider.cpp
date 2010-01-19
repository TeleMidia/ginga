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

#include "../../../include/AudioProvider.h"
#include "../../../include/Graphics.h"
#include "../../../include/IOHandler.h"

#if HAVE_FUSIONSOUND
struct fusiosoundstruct {
	IFusionSoundMusicProvider* provider;
	IFusionSound* sound;
	IFusionSoundStream* stream;
	IFusionSoundPlayback* playback;
	//IFusionSoundBuffer* buffer;
} typedef fusionSoundProvider;
#endif

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace io {
	AudioProvider::AudioProvider(char* mrl) {
#if HAVE_FUSIONSOUND
		fusionSoundProvider* dec;
		dec = new fusionSoundProvider;

		//FSBufferDescription b_desc;
		FSStreamDescription   s_desc;
		FSTrackDescription    t_dsc;

		dec->sound = (IFusionSound*)(Graphics::getFSRoot());

		dec->sound->CreateMusicProvider(
				dec->sound, mrl, &(dec->provider));

		//dec->provider->SelectTrack( dec->provider, 0 );
		//dec->provider->GetTrackDescription( dec->provider, &t_dsc );

/*		dec->provider->GetBufferDescription( dec->provider, &b_desc );
		dec->provider->GetStreamDescription( dec->provider, &s_desc );

		b_desc.flags         = (FSBufferDescriptionFlags)(
				b_desc.flags | FSBDF_SAMPLEFORMAT | FSBDF_CHANNELS |
				FSBDF_LENGTH);

		b_desc.sampleformat  = FSSF_S16;
		b_desc.channels      = 2;
		b_desc.length        = 1024;

		s_desc.flags         = (FSStreamDescriptionFlags)(
				s_desc.flags | FSSDF_SAMPLEFORMAT | FSSDF_CHANNELS);

		s_desc.sampleformat  = FSSF_S16;
		s_desc.channels      = 2;

		dec->sound->CreateStream( dec->sound, &s_desc, &(dec->stream) );
		//dec->sound->CreateBuffer( dec->sound, &b_desc, &(dec->buffer) );
*/
		dec->provider->GetStreamDescription( dec->provider, &s_desc );

		s_desc.flags         = (FSStreamDescriptionFlags)(
				s_desc.flags | FSSDF_SAMPLEFORMAT | FSSDF_CHANNELS);

		s_desc.sampleformat  = FSSF_S16;
		s_desc.channels      = 2;

		dec->sound->CreateStream( dec->sound, &s_desc, &(dec->stream) );
		dec->stream->GetPlayback( dec->stream, &(dec->playback) );

        /* Reset pitch. */
		//dec->playback->SetPitch( dec->playback, 1.0 );

		this->decoder = (void*)(dec);

#elif HAVE_DIRECTFB
		this->decoder = IOHandler::createVideoProvider(mrl);
#endif
	}

	AudioProvider::~AudioProvider() {
#if HAVE_FUSIONSOUND
		fusionSoundProvider* ap;
		ap = (fusionSoundProvider*)dec;

		if (ap->provider)
			ap->provider->Release( ap->provider );
		if (ap->playback)
			ap->playback->Release( ap->playback );
		/*if (ap->buffer)
			ap->buffer->Release( ap->buffer );*/
		if (ap->stream)
			ap->stream->Release( ap->stream );

#elif HAVE_DIRECTFB
		IDirectFBVideoProvider* vp;
		vp = (IDirectFBVideoProvider*)decoder;

		DFBCHECK(vp->Stop(vp));
		IOHandler::releaseVideoProvider(this);
		decoder = NULL;
#endif
	}

	void* AudioProvider::getContent() {
		return decoder;
	}

	Surface* AudioProvider::getPerfectSurface() {
#if HAVE_FUSIONSOUND
		return NULL;

#elif HAVE_DIRECTFB
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

	void AudioProvider::dynamicRenderCallBack(int len, void* dec) {
#if HAVE_FUSIONSOUND
		void* data;
		int frames, bytes;
		fusionSoundProvider* ap;
	    ap = (fusionSoundProvider*)dec;

		/*ap->buffer->Lock(ap->buffer, &data, &frames, &bytes);
		ap->stream->Write(ap->stream, data, 1024);
		ap->buffer->Unlock(ap->buffer);*/

#elif HAVE_DIRECTFB
		IDirectFBSurface* s = NULL;
		IDirectFBWindow* w = NULL;

		Surface* someSurface = (Surface*)dec;
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

	double AudioProvider::getTotalMediaTime() {
		if (decoder == NULL) {
			return 0;
		}

#if HAVE_FUSIONSOUND
		IFusionSoundMusicProvider* p;
		p = ((fusionSoundProvider*)decoder)->provider;

#elif HAVE_DIRECTFB
		IDirectFBVideoProvider* p;
		p = (IDirectFBVideoProvider*)decoder;
#endif

#if HAVE_FUSIONSOUND or HAVE_DIRECTFB
		double totalTime;

		p->GetLength(p, &totalTime);
		return totalTime;
#endif
		return 0;
	}

	double AudioProvider::getMediaTime() {
		if (decoder == NULL) {
			return -1;
		}

#if HAVE_FUSIONSOUND
		IFusionSoundMusicProvider* p;
		p = ((fusionSoundProvider*)decoder)->provider;

#elif HAVE_DIRECTFB
		IDirectFBVideoProvider* p;
		p = (IDirectFBVideoProvider*)decoder;
#endif

#if HAVE_FUSIONSOUND or HAVE_DIRECTFB
		double currentTime;

		p->GetPos(p, &currentTime);
		return currentTime;
#endif
		return -1;
	}

	void AudioProvider::setMediaTime(double pos) {
		if (decoder == NULL) {
			return;
		}

#if HAVE_FUSIONSOUND
		IFusionSoundMusicProvider* p;
		p = ((fusionSoundProvider*)decoder)->provider;

#elif HAVE_DIRECTFB
		IDirectFBVideoProvider* p;
		p = (IDirectFBVideoProvider*)decoder;
#endif

#if HAVE_FUSIONSOUND or HAVE_DIRECTFB
		p->SeekTo(p, pos);
#endif
	}

	void AudioProvider::playOver(Surface* surface, bool hasVisual) {
		if (decoder == NULL) {
			cout << "AudioProvider::playOver decoder = NULL" << endl;
			return;
		}

#if HAVE_FUSIONSOUND
		fusionSoundProvider* ap;

		ap = (fusionSoundProvider*)decoder;
		/*ap->provider->PlayToBuffer(
				ap->provider,
				ap->buffer,
				dynamicRenderCallBack,
				ap);*/

		ap->provider->PlayToStream(
				ap->provider,
				ap->stream);

		/*ap->stream->Wait( ap->stream, 0 );
		while (true) {
			::usleep(10000);
		}*/
#elif HAVE_DIRECTFB
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

	void AudioProvider::pause() {
		stop();
	}

	void AudioProvider::resume(Surface* surface, bool hasVisual) {
		if (decoder == NULL) {
			return;
		}

#if HAVE_FUSIONSOUND
		fusionSoundProvider* ap;

		ap = (fusionSoundProvider*)decoder;
		ap->provider->PlayToStream(ap->provider, ap->stream);

#elif HAVE_DIRECTFB
		play(surface, hasVisual);
#endif
	}

	void AudioProvider::stop() {
		if (decoder == NULL) {
			return;
		}

#if HAVE_FUSIONSOUND
		fusionSoundProvider* ap;

		ap = (fusionSoundProvider*)decoder;
		ap->provider->Stop(ap->provider);

#elif HAVE_DIRECTFB
		IDirectFBVideoProvider* vp;
		vp = (IDirectFBVideoProvider*)decoder;
		DFBCHECK(vp->Stop(vp));
#endif
	}

	void AudioProvider::setSoundLevel(float level) {
		if (decoder == NULL) {
			return;
		}

#if HAVE_FUSIONSOUND
		IFusionSoundPlayback* p;

		p = ((fusionSoundProvider*)decoder)->playback;

#elif HAVE_DIRECTFB
		IDirectFBVideoProvider* p;
		p = (IDirectFBVideoProvider*)decoder;
#endif

#if HAVE_FUSIONSOUND or HAVE_DIRECTFB
		p->SetVolume(p, level);
#endif
	}
}
}
}
}
}
}
