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

#include "mb/LocalScreenManager.h"
#include "mb/interface/dfb/content/video/XineVideoProvider.h"
#include "mb/interface/dfb/output/DFBSurface.h"

#include "util/functions.h"
using namespace ::br::pucrio::telemidia::util;

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

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace mb {
	XineVideoProvider::XineVideoProvider(
			GingaScreenID screenId, const char* mrl) {

		myScreen             = screenId;
		rContainer           = new XineRendererContainer;
		rContainer->isValid  = false;
		rContainer->xine     = NULL;
		rContainer->vo       = NULL;
		rContainer->ao       = NULL;
		rContainer->stream   = NULL;
		rContainer->config   = NULL;
		rContainer->listener = NULL;
		rContainer->surface  = NULL;

		resumePos = 0;
		startPos  = 0;

		if (mrl != NULL) {
			initializeXine(mrl);
		}
	}

	XineVideoProvider::~XineVideoProvider() {
		if (rContainer != NULL) {
			if (rContainer->stream) {
				xine_stop(rContainer->stream);
				xine_close(rContainer->stream);
				xine_dispose(rContainer->stream);
			}

			if (rContainer->ao != NULL) {
				xine_close_audio_driver(rContainer->xine, rContainer->ao);
				rContainer->ao = NULL;
			}

			if (rContainer->vo != NULL) {
				xine_close_video_driver(rContainer->xine, rContainer->vo);
				rContainer->vo = NULL;
			}

			if (rContainer->config != NULL) {
				xine_config_save(rContainer->xine, rContainer->config);
				delete rContainer->config;
				rContainer->config = NULL;
			}

			if (rContainer->xine != NULL) {
				xine_exit(rContainer->xine);
				rContainer->xine = NULL;
			}

			rContainer->listener = NULL;
			rContainer->surface  = NULL;
			delete rContainer;
			rContainer = NULL;
		}
	}

	bool XineVideoProvider::initializeXine(const char* mrl) {
		/* fill the visual */
		visual.destination  = NULL;
		visual.subpicture   = NULL;
		visual.output_cb    = XineVideoProvider::outputCallBack;
		visual.output_cdata = (void*)rContainer;
		visual.frame_cb     = XineVideoProvider::dynamicRenderCallBack;
		visual.frame_cdata  = (void*)rContainer;

		rContainer->xine = xine_new();
		if (rContainer->xine == NULL) {
			clog << "XineVideoProvider::initializeXine xine_new() failed!";
			clog << endl;
			return false;
		}

		asprintf(&rContainer->config, "%s/.xine/config", xine_get_homedir());
		if (rContainer->config) {
			xine_config_load(rContainer->xine, rContainer->config);
		}

		xine_init(rContainer->xine);

		/* be verbose */
		xine_engine_set_param(
				rContainer->xine,
				XINE_ENGINE_PARAM_VERBOSITY,
				XINE_VERBOSITY_LOG);

		/* open DFB video driver */
		rContainer->vo = xine_open_video_driver(
				rContainer->xine,
				"DFB",
				XINE_VISUAL_TYPE_DFB,
				(void*)&visual);

		if (!rContainer->vo) {
			clog << "XineVideoProvider::initializeXine ";
			clog << "couldn't open video driver DFB!";
			clog << endl;
			return false;
		}

		/* open audio driver (auto) */
		rContainer->ao = xine_open_audio_driver(rContainer->xine, "auto", NULL);
		if (!rContainer->ao) {
			/* try Alsa */
			rContainer->ao = xine_open_audio_driver(
					rContainer->xine, "alsa", NULL);

			if (!rContainer->ao) {
				/* try OSS */
				rContainer->ao = xine_open_audio_driver(
						rContainer->xine, "oss", NULL);

				if (!rContainer->ao) {
					clog << "XineVideoProvider::initializeXine Warning! ";
					clog << "couldn't find a working audio driver!";
					clog << endl;

					/* disable audio */
					rContainer->ao = xine_open_audio_driver(
							rContainer->xine, "null", NULL);

					xine_engine_set_param(
							rContainer->xine, XINE_PARAM_IGNORE_AUDIO, 1);
				}
			}
		}

		rContainer->stream = xine_stream_new(
				rContainer->xine, rContainer->ao, rContainer->vo);

		if (!rContainer->stream) {
			clog << "XineVideoProvider::initializeXine ";
			clog << "couldn't create a new stream!";
			clog << endl;
			return false;
		}

		xine_set_param(rContainer->stream, XINE_PARAM_METRONOM_PREBUFFER, 6000);

		/* be verbose */
		xine_set_param(
				rContainer->stream, XINE_PARAM_VERBOSITY, XINE_VERBOSITY_LOG);

		initializeAudio();

		if (!xine_open(rContainer->stream, mrl)) {
			clog << "XineVideoProvider::initializeXine ";
			clog << "unsupported content!";
			clog << endl;
			return false;
		}

		clog << "XineVideoProvider::initializeXine OK!" << endl;
		return true;
	}

	void XineVideoProvider::initializeAudio() {
		xine_set_param(
				rContainer->stream, XINE_PARAM_AUDIO_REPORT_LEVEL, 1);

		xine_set_param(
				rContainer->stream, XINE_PARAM_AUDIO_CHANNEL_LOGICAL, -1);

		xine_set_param(
				rContainer->stream, XINE_PARAM_AUDIO_MUTE, 0);

		xine_set_param(
				rContainer->stream, XINE_PARAM_AUDIO_AMP_MUTE, 0);

		xine_set_param(
				rContainer->stream, XINE_PARAM_AUDIO_VOLUME, 100);

		xine_set_param(
				rContainer->stream, XINE_PARAM_AUDIO_COMPR_LEVEL, 100);

		xine_set_param(
				rContainer->stream, XINE_PARAM_AUDIO_AMP_LEVEL, 200);
	}

	void XineVideoProvider::setAVPid(int aPid, int vPid) {
		xine_event_t event;
		xine_pids_data_t data;

		event.type = XINE_EVENT_PIDS_CHANGE;
		event.data = &data;
		event.data_length = sizeof (xine_pids_data_t);

		data.apid = aPid;
		data.vpid = vPid;

		clog << "XineVideoProvider::setAVPid";
		clog << " aPid = '" << aPid << "'";
		clog << " vPid = '" << vPid << "'";
		clog << endl;
		xine_event_send(rContainer->stream, &event);

		initializeAudio();
	}

	void* XineVideoProvider::getContent() {
		return NULL;
	}

	void XineVideoProvider::feedBuffers() {
		int w, h;
		bool buffered = false;

		while (!buffered) {
			getOriginalResolution(&w, &h);

			if (w > 1 && h > 1) {
				buffered = true;
				break;
			}

			::usleep(150000);
		}
	}

	void XineVideoProvider::getVideoSurfaceDescription(
			DFBSurfaceDescription* dsc) {

		getOriginalResolution(&(dsc->width), &(dsc->height));
		dsc->flags = (DFBSurfaceDescriptionFlags)(DSDESC_WIDTH | DSDESC_HEIGHT);
	}

	ISurface* XineVideoProvider::getPerfectSurface() {
		DFBSurfaceDescription dsc;

		getVideoSurfaceDescription(&dsc);

		if (dsc.width < 1 || dsc.height < 1) {
			dsc.width  = 320;
			dsc.height = 240;
		}

		return new DFBSurface(
				myScreen,
				LocalScreenManager::getInstance()->createSurface(
						myScreen, &dsc));
	}

	bool XineVideoProvider::checkVideoResizeEvent(ISurface* frame) {
		IDirectFBSurface* s = NULL;
		DFBSurfaceDescription dsc;
		int w, h;

		getVideoSurfaceDescription(&dsc);

		frame->getSize(&w, &h);
		if (dsc.width != w || dsc.height != h) {
			clog << "XineVideoProvider::checkVideoResizeEvent ";
			clog << "width = '" << w << "' height = '" << h;
			clog << "' dsc.w = '" << dsc.width;
			clog << "' dsc.h = '" << dsc.height << "'";
			clog << endl;

			s = (IDirectFBSurface*)(
					LocalScreenManager::getInstance()->createSurface(
							myScreen, &dsc));

			visual.destination = s;
			frame->setContent(s);
			updateVisualData(frame);
			return true;

		} else {
			clog << "XineVideoProvider::checkVideoResizeEvent !update ";
			clog << "width = '" << w << "' height = '" << h << "'" << endl;
		}

		return false;
	}

	void XineVideoProvider::outputCallBack(
			void *cdata,
			int width,
			int height,
			double ratio,
			DFBSurfacePixelFormat format,
			DFBRectangle *dst) {

		XineRendererContainer* cont = NULL;
		IDirectFBSurface* frame     = NULL;

		cont = (XineRendererContainer*)cdata;
		if (cont == NULL) {
			return;
		}

		frame = (IDirectFBSurface*)(cont->surface->getContent());
		if (frame == NULL) {
			return;
		}

		dst->x = 0;
		dst->y = 0;
		frame->GetSize(frame, &dst->w, &dst->h);
	}

	void XineVideoProvider::dynamicRenderCallBack(void* rendererContainer) {
		IDirectFBWindow* w          = NULL;
		IDirectFBSurface* s         = NULL;
		IDirectFBSurface* frame     = NULL;
		XineRendererContainer* cont = NULL;
		IWindow* someWindow         = NULL;
		ISurface* someSurface       = NULL;

		bool fitTo;
		bool stretchTo;
		int winWidth, winHeight;
		int surWidth, surHeight;

		cont = (XineRendererContainer*)rendererContainer;
		if (cont == NULL) {
			return;
		}

		someSurface = cont->surface;
		if (someSurface == NULL) {
			return;
		}

		frame = (IDirectFBSurface*)(someSurface->getContent());
		if (frame == NULL) {
			return;
		}

		someWindow = (IWindow*)(someSurface->getParent());
		if (someWindow != NULL && cont->isValid) {
			DFBCHECK(frame->GetSize(frame, &surWidth, &surHeight));

			someWindow->lock();
			fitTo = someWindow->getFit();
			stretchTo = someWindow->getStretch();
			w = (IDirectFBWindow*)(someWindow->getContent());

			if (w == NULL || !cont->isValid) {
				someWindow->unlock();
				return;
			}

			//DFBCHECK(w->GetSize(w, &winWidth, &winHeight));
			DFBCHECK(w->GetSurface(w, &s));

			s->SetBlittingFlags(s, DSBLIT_NOFX);
			s->GetSize(s, &winWidth, &winHeight);

			if ((winWidth != surWidth || winHeight != surHeight)/* && fitTo*/) {
				//if (stretchTo) {
					DFBCHECK(s->StretchBlit(
						    s,
						    frame,
						    NULL,
						    NULL));

				/*} else {
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
				}*/

			} else {
				DFBCHECK(s->Blit(s, frame, NULL, 0, 0));
			}

			DFBCHECK(s->Flip(s, NULL, (DFBSurfaceFlipFlags)DSFLIP_NONE));
			someWindow->unlock();

		} else if (frame != NULL) {
			DFBCHECK(frame->Blit(frame, frame, NULL, 0, 0));
		}

		/*
		listener = cont->listener;
		dec = cont->dec;

		if (listener != NULL && dec != NULL) {
			endTime = listener->getEndTime();
			if (endTime <= 0) {
				dec->GetStatus(dec, &st);
				if (st == DVSTATE_FINISHED) {
					listener->finished();
				}
			}
		}*/
	}

	void XineVideoProvider::getOriginalResolution(int* width, int* height) {
		if (rContainer->stream != NULL) {
			*width = xine_get_stream_info(
					rContainer->stream, XINE_STREAM_INFO_VIDEO_WIDTH);

			*height = xine_get_stream_info(
					rContainer->stream, XINE_STREAM_INFO_VIDEO_HEIGHT);

		} else {
			*width  = 0;
			*height = 0;
		}
	}

	double XineVideoProvider::getTotalMediaTime() {
		/*int pos = 0;

		if (rContainer->stream == NULL) {
			return 0;
		}

		xine_get_pos_length(rContainer->stream, NULL, NULL, &pos);

		return (double)pos / 1000.0;*/

		return infinity();
	}

	double XineVideoProvider::getMediaTime() {
		int i, pos;

		if (rContainer->stream == NULL) {
			return -1;
		}

		for (i = 5; i--;) {
			if (xine_get_pos_length(rContainer->stream, NULL, &pos, NULL)) {
				break;
			}

			::usleep(1000);
		}

		return (double)pos / 1000.0;
	}

	int64_t XineVideoProvider::getVPts() {
		struct xine_stream_s* _stream;
		metronom_t* _metronom;

		if (rContainer->stream == NULL) {
			return -1;
		}

		_stream   = (struct xine_stream_s*)rContainer->stream;
		_metronom = _stream->metronom;

		return _metronom->last_video_pts;
	}

	void XineVideoProvider::setMediaTime(double pos) {
		if (rContainer->stream == NULL) {
			return;
		}

		if (!xine_get_stream_info(
				rContainer->stream, XINE_STREAM_INFO_SEEKABLE)) {

			return;
		}

		startPos = pos * 1000;
		if (rContainer->isValid) {
			xine_play(rContainer->stream, 0, startPos);
		}
	}

	bool XineVideoProvider::updateVisualData(ISurface* surface) {
		clog << "XineVideoProvider::updateVisualData " << endl;
		visual.destination = (IDirectFBSurface*)(surface->getContent());
		visual.subpicture  = NULL;

		if (!xine_port_send_gui_data(
				rContainer->vo, XINE_GUI_SEND_SELECT_VISUAL, (void*)&visual)) {

			clog << "XineVideoProvider::updateVisualData ";
			clog << "failed to update driver's visual!!";
			clog << endl;
			return false;
		}

		return true;
	}

	void XineVideoProvider::playOver(
			ISurface* surface, bool hasVisual, IProviderListener* listener) {

		rContainer->listener = listener;
		rContainer->surface  = surface;
		rContainer->isValid  = true;

		if (updateVisualData(surface)) {
			xine_play(rContainer->stream, 0, startPos);
		}
	}

	void XineVideoProvider::resume(ISurface* surface, bool hasVisual) {
		rContainer->surface  = surface;
		rContainer->isValid  = true;

		if (updateVisualData(surface)) {
			xine_play(rContainer->stream, 0, resumePos);
		}
	}

	void XineVideoProvider::pause() {
		stop();
	}

	void XineVideoProvider::stop() {
		rContainer->isValid = false;
		xine_get_pos_length(rContainer->stream, NULL, &resumePos, NULL);
		xine_stop(rContainer->stream);
	}

	void XineVideoProvider::setSoundLevel(float level) {
		float soundValue = level * 100;
		if (soundValue > 100.0) {
			soundValue = 100.0;

		} else if (level < 0) {
			soundValue = 0;
		}

		clog << "XineVideoProvider::setSoundLevel '" << level;
		clog << "'" << endl;
		xine_set_param(
				rContainer->stream, XINE_PARAM_AUDIO_VOLUME, soundValue);

		xine_set_param(
				rContainer->stream, XINE_PARAM_AUDIO_AMP_LEVEL, soundValue);
	}

	bool XineVideoProvider::releaseAll() {
		//TODO: release all structures
		return false;
	}
}
}
}
}
}
}

extern "C" ::br::pucrio::telemidia::ginga::core::mb::IContinuousMediaProvider*
		createXineVideoProvider(GingaScreenID screenId, const char* mrl) {

	return (new ::br::pucrio::telemidia::ginga::core::mb::XineVideoProvider(
			screenId, mrl));
}

extern "C" void destroyXineVideoProvider(
		::br::pucrio::telemidia::ginga::core::mb::
		IContinuousMediaProvider* cmp) {

	delete cmp;
}
