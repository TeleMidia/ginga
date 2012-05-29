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
*******************************************************************************
Part of this file is extracted from SDL_ffmpeg library and ffplay
SDL_ffmpeg was created by Arjan Houben, but it is a discontinued library
SDL_ffmpeg was free software licensed under the LGPL
ffplay is part of FFmpeg library.
FFmpeg is free software licensed under the LGPL or GPL
Many thanks to these guys and to the community that support them!
*******************************************************************************/

#include "mb/interface/sdl/content/audio/SDL2ffmpeg.h"

#ifndef AV_LOG_SKIP_REPEATED
#define AV_LOG_SKIP_REPEATED 1
#endif

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace mb {

	SDL_AudioSpec SDL2ffmpeg::spec;
	short SDL2ffmpeg::refCount = 0;
	set<SDL2ffmpeg*> SDL2ffmpeg::aInstances;
	bool SDL2ffmpeg::init = false;
	pthread_mutex_t SDL2ffmpeg::aiMutex;

	SDL2ffmpeg::SDL2ffmpeg(
			IContinuousMediaProvider* cmp, const char *filename) {

		int err, i;

		this->cmp                            = cmp;
		wanted_stream[AVMEDIA_TYPE_AUDIO]    = -1;
		wanted_stream[AVMEDIA_TYPE_VIDEO]    = -1;
		wanted_stream[AVMEDIA_TYPE_SUBTITLE] = -1;
		seek_by_bytes                        = -1;
		av_sync_type                         = AV_SYNC_AUDIO_MASTER;
		start_time                           = AV_NOPTS_VALUE;
		duration                             = AV_NOPTS_VALUE;
		workaround_ffmpeg_bugs               = 1;
		fast                                 = 0;
		genpts                               = 0;
		lowres                               = 0;
		idct                                 = FF_IDCT_AUTO;
		skip_frame                           = AVDISCARD_DEFAULT;
		skip_idct                            = AVDISCARD_DEFAULT;
		skip_loop_filter                     = AVDISCARD_DEFAULT;
		error_concealment                    = 3;
		decoder_reorder_pts                  = -1;
		framedrop                            = -1;
		rdftspeed                            = 20;
		texture                              = NULL;
		hasPic                               = false;
		reof                                 = false;
		hasSDLAudio                          = false;
		abortRequest                         = false;
		allocate                             = false;
		monoStep                             = 0;
		status                               = ST_STOPPED;

		setSoundLevel(0.5);

		if (!init) {
			init = true;

			memset(&spec, 0, sizeof(spec));
			pthread_mutex_init(&aiMutex, NULL);

			av_log_set_flags(AV_LOG_SKIP_REPEATED);

			avcodec_register_all();
		    av_register_all();
		    avformat_network_init();
		    avdevice_register_all();

		    av_lockmgr_register(SDL2ffmpeg::lockmgr);
		}

		memset(&vPkt, 0, sizeof(vPkt));
		memset(&flush_pkt, 0, sizeof(flush_pkt));

		vs = (VideoState*)av_mallocz(sizeof(VideoState));
		if (vs) {
			av_strlcpy(vs->filename, filename, sizeof(vs->filename));

			av_init_packet(&flush_pkt);
		    flush_pkt.data = (uint8_t*)(intptr_t)"FLUSH";

		    read_init();

			/* start video display */
			vs->pictq_mutex = SDL_CreateMutex();
			vs->pictq_cond  = SDL_CreateCond();

			vs->subpq_mutex = SDL_CreateMutex();
			vs->subpq_cond  = SDL_CreateCond();

		    packet_queue_init(&vs->videoq);
		    packet_queue_init(&vs->audioq);
		    packet_queue_init(&vs->subtitleq);

			vs->av_sync_type = av_sync_type;

			pthread_mutex_lock(&aiMutex);
			openStreams();
			pthread_mutex_unlock(&aiMutex);
		}

	    refCount++;
	}

	SDL2ffmpeg::~SDL2ffmpeg() {
		set<SDL2ffmpeg*>::iterator i;

		clog << "SDL2ffmpeg::~SDL2ffmpeg" << endl;

		abortRequest = true;
		hasPic = false;

		pthread_mutex_lock(&aiMutex);

		i = aInstances.find(this);
		if (i != aInstances.end()) {
			aInstances.erase(i);
		}

		if (aInstances.empty()) {
			clog << endl;
			clog << "SDL2ffmpeg::~SDL2ffmpeg calling SDL_PauseAudio(1)";
			clog << endl << endl;

			SDL_PauseAudio(1);

			if (spec.size != 0) {
				memset(&spec, 0, sizeof(spec));

				SystemCompat::uSleep(10000);

				/*
				 * TODO: check why SDL_CloseAudio is causing SIGABRT for
				 *       fc16 pulse audio (closing 2nd time).
				 */
				try {
					SDL_CloseAudio();

				} catch ( const std::exception& ex ) {
					clog << "Caught exception: " << ex.what() << std::endl;

				} catch ( ... ) {
					clog << "unknown exception" << std::endl;
				}
			}
		}

		pthread_mutex_unlock(&aiMutex);

		release();

		if (texture != NULL) {
			SDL_DestroyTexture(texture);
			texture = NULL;
		}

		refCount--;

		if (refCount == 0) {
			init = false;
			pthread_mutex_destroy(&aiMutex);

			av_lockmgr_register(NULL);
			avformat_network_deinit();
		}
	}

	void SDL2ffmpeg::release() {
		clog << "SDL2ffmpeg::release" << endl;

		if (vs != NULL) {
			if (vs->ic) {
				avformat_close_input(&vs->ic);
				vs->ic = NULL; /* safety */
			}

			stream_close();
		}
	}

	void SDL2ffmpeg::close(bool quit) {
		/* close each stream */
		if (vs->audio_stream >= 0) {
			stream_component_close(vs->audio_stream);
			vs->audio_stream = -1;
		}

		if (vs->video_stream >= 0) {
			stream_component_close(vs->video_stream);
			vs->video_stream = -1;
		}

		if (vs->subtitle_stream >= 0) {
			stream_component_close(vs->subtitle_stream);
			vs->subtitle_stream = -1;
		}
	}

	string SDL2ffmpeg::ffmpegErr(int err) {
		string strErr = "";
		char* errmsg = (char*)malloc(250);
		av_strerror(err, errmsg, 250);

		strErr.assign(errmsg);

		free(errmsg);

		return strErr;
	}

	void SDL2ffmpeg::openStreams() {
		int st_index[AVMEDIA_TYPE_NB];

		memset(st_index, -1, sizeof(st_index));

		st_index[AVMEDIA_TYPE_VIDEO] = av_find_best_stream(
				vs->ic,
				AVMEDIA_TYPE_VIDEO,
				wanted_stream[AVMEDIA_TYPE_VIDEO],
				-1,
				NULL,
				0);

		st_index[AVMEDIA_TYPE_AUDIO] = av_find_best_stream(
				vs->ic,
				AVMEDIA_TYPE_AUDIO,
				wanted_stream[AVMEDIA_TYPE_AUDIO],
				st_index[AVMEDIA_TYPE_VIDEO],
				NULL,
				0);

		st_index[AVMEDIA_TYPE_SUBTITLE] = av_find_best_stream(
				vs->ic,
				AVMEDIA_TYPE_SUBTITLE,
				wanted_stream[AVMEDIA_TYPE_SUBTITLE],
				(st_index[AVMEDIA_TYPE_AUDIO] >= 0 ? st_index
						[AVMEDIA_TYPE_AUDIO] :
						st_index[AVMEDIA_TYPE_VIDEO]),
				NULL,
				0);

		/* open the streams */
		if (st_index[AVMEDIA_TYPE_AUDIO] >= 0) {
			stream_component_open(st_index[AVMEDIA_TYPE_AUDIO]);

		} else {
			clog << "SDL2ffmpeg::read_init '";
			clog << vs->filename << "' doesn't have any audio stream!";
			clog << endl;
		}

		if (st_index[AVMEDIA_TYPE_VIDEO] >= 0) {
			stream_component_open(st_index[AVMEDIA_TYPE_VIDEO]);

		} else {
			clog << "SDL2ffmpeg::read_init '";
			clog << vs->filename << "' doesn't have any video stream!";
			clog << endl;
		}

		if (st_index[AVMEDIA_TYPE_SUBTITLE] >= 0) {
			stream_component_open(st_index[AVMEDIA_TYPE_SUBTITLE]);

		} else {
			clog << "SDL2ffmpeg::read_init '";
			clog << vs->filename << "' doesn't have any subtitle stream!";
			clog << endl;
		}
	}

	bool SDL2ffmpeg::prepare() {
		bool wasEmpty = false;

		if (vs->audio_stream >= 0 && vs->audio_st != NULL) {
			pthread_mutex_lock(&aiMutex);
			wasEmpty = aInstances.empty();
			aInstances.insert(this);
			pthread_mutex_unlock(&aiMutex);

			if (vs->audio_hw_buf_size == 0) {
				clog << "SDL2ffmpeg::prepare Warning! buffer size = 0" << endl;
			}

			vs->audio_main_buf[0] = (uint8_t*)malloc(vs->audio_hw_buf_size);
			vs->audio_main_buf[1] = (uint8_t*)malloc(vs->audio_hw_buf_size);

			hasSDLAudio = true;

			if (wasEmpty) {
				SDL_PauseAudio(0);
			}

		} else if (vs->video_st != NULL) {
			vs->av_sync_type = AV_SYNC_VIDEO_MASTER;

		} else {
			vs->av_sync_type = AV_SYNC_EXTERNAL_CLOCK;
		}

		if (vs->video_st != NULL) {

			clog << "SDL2ffmpeg::prepare Video frame rate is '";
			clog << vs->video_st->r_frame_rate.num << "/";
			clog << vs->video_st->r_frame_rate.den << "'" << endl;

			clog << "SDL2ffmpeg::prepare Video average frame rate is '";
			clog << vs->video_st->avg_frame_rate.num << "/";
			clog << vs->video_st->avg_frame_rate.den << "'" << endl;

			clog << "SDL2ffmpeg::prepare Video time base frame rate is '";
			clog << vs->video_st->time_base.num << "/";
			clog << vs->video_st->time_base.den << "'" << endl;

			vs->video_tid = SDL_CreateThread(
					SDL2ffmpeg::video_thread, "video_thread", this);

			if (!vs->video_tid) {
				clog << "SDL2ffmpeg::prepare Warning! ";
				clog << "Can't create video thread" << endl;
			}
		}

		if (vs->subtitle_st != NULL) {
			vs->subtitle_tid = SDL_CreateThread(
					SDL2ffmpeg::subtitle_thread, "subtitle_thread", this);

			if (!vs->subtitle_tid) {
				clog << "SDL2ffmpeg::prepare Warning! ";
				clog << "Can't create subtitle thread" << endl;
			}
		}

		/*vs->refresh_tid = SDL_CreateThread(
				SDL2ffmpeg::refresh_thread, "refresh_thread", this);

		if (!vs->refresh_tid) {
			av_free(vs);

			clog << "SDL2ffmpeg::prepare Warning! ";
			clog << "Can't create refresh thread" << endl;

			return false;
		}*/

		vs->read_tid = SDL_CreateThread(
				SDL2ffmpeg::read_thread, "read_thread", this);

		if (!vs->read_tid) {
			av_free(vs);

			clog << "SDL2ffmpeg::prepare Warning! ";
			clog << "Can't create read thread" << endl;

			return false;
		}

		SDL_BuildAudioCVT(
				&acvt,
				wantedSpec.format, wantedSpec.channels, wantedSpec.freq,
				spec.format, spec.channels, spec.freq);

		return true;
	}

	void SDL2ffmpeg::play() {
		clog << "SDL2ffmpeg::play called" << endl;
		if (status == ST_STOPPED) {
			if (prepare()) {
				status = ST_PLAYING;
			}
		}
	}

	void SDL2ffmpeg::stop() {
		clog << "SDL2ffmpeg::stop(" << vs->filename << ")" << endl;

		abortRequest = true;
		status = ST_STOPPED;

		if (vs->video_stream >= 0) {
			packet_queue_abort(&vs->videoq);
		}

		if (vs->audio_stream >= 0) {
			packet_queue_abort(&vs->audioq);
		}

		if (vs->subtitle_stream >= 0) {
			packet_queue_abort(&vs->subtitleq);
		}

		SDL_CondSignal(vs->pictq_cond);
		SDL_CondSignal(vs->subpq_cond);

/*		set<SDL2ffmpeg*>::iterator i;
		pthread_mutex_lock(&aiMutex);

		i = aInstances.find(this);
		if (i != aInstances.end()) {
			aInstances.erase(i);

			if (aInstances.empty()) {
				clog << endl;
				clog << "SDL2ffmpeg::stop calling SDL_PauseAudio(1)";
				clog << endl << endl;

				SDL_PauseAudio(1);
			}
		}
		pthread_mutex_unlock(&aiMutex);*/
		clog << "SDL2ffmpeg::stop(" << vs->filename << ") all done" << endl;
	}

	void SDL2ffmpeg::pause() {
		clog << "SDL2ffmpeg::pause" << endl;

		if (status == ST_PLAYING) {
			status = ST_PAUSED;
			if (!vs->paused) {
				toggle_pause();
			}
		}
	}

	void SDL2ffmpeg::resume() {
		clog << "SDL2ffmpeg::resume" << endl;

		if (status == ST_PAUSED) {
			status = ST_PLAYING;
			if (vs->paused) {
				toggle_pause();
			}
		}
	}

	void SDL2ffmpeg::getOriginalResolution(int* w, int* h) {
		if (vs->video_st && vs->video_st->codec->width) {
			*w = vs->video_st->codec->width;
			*h = vs->video_st->codec->height;

		} else {

			clog << "SDL2ffmpeg::getOriginalResolution(" << vs->filename;
			clog << "Can't get video data. video_st = '" << vs->video_st;
			clog << "'";

			if (vs->video_st != NULL) {
				clog << ". Width = '" << vs->video_st->codec->width;
				clog << "'. Height = '" << vs->video_st->codec->height;
				clog << "'. Codec = '" << vs->video_st->codec;
				clog << "'. Codec name = '" << vs->video_st->codec->codec_name;
				clog << "'";
			}
			clog << endl;

			*w = 0;
			*h = 0;
		}
	}

	double SDL2ffmpeg::getDuration() {
		if (vs != NULL && vs->ic != NULL) {
			return vs->ic->duration/1000LL;
		}

		return 0.0;
	}

	double SDL2ffmpeg::getPosition() {
		double position = 0.0;

		clog << "SDL2ffmpeg::getPosition(" << vs->filename;
		clog << ") master_clock = '";
		clog << get_master_clock() << "' (duration = '" << getDuration();
		clog << "')" << endl;

		clog << "SDL2ffmpeg::getPosition(" << vs->filename;
		clog << ") current_pts = '";
		clog << vs->video_current_pts << "' (duration = '" << getDuration();
		clog << "')" << endl;

		if (vs->audio_stream >= 0 && vs->audio_st != NULL) {
			position = vs->audio_current_pts;

		} else if (vs->video_stream >= 0 && vs->video_st != NULL) {
			position = vs->video_current_pts;
		}

		if (position < 0.0) {
			position = 0.0;
		}

		return position;
	}

	void SDL2ffmpeg::seek(int64_t pos) {
		stream_seek(pos, 0, 0);
	}

	void SDL2ffmpeg::setTexture(SDL_Texture* texture) {
		this->texture = texture;
	}

	bool SDL2ffmpeg::hasTexture() {
		return (texture != NULL);
	}

	SDL_Texture* SDL2ffmpeg::getTexture() {
		return texture;
	}

	bool SDL2ffmpeg::hasPicture() {
		return hasPic;
	}

	void SDL2ffmpeg::setSoundLevel(float level) {
		if (level >= 1.0) {
			soundLevel = SDL_MIX_MAXVOLUME;

		} else if (level >= 0.0) {
			soundLevel = level * SDL_MIX_MAXVOLUME;
		}
	}

	bool SDL2ffmpeg::getAudioSpec(
			SDL_AudioSpec* spec, int sample_rate, uint8_t channels) {

		bool audioSpec = false;

		if (sample_rate > 0 && channels > 0) {
			spec->format   = AUDIO_S16SYS;
			spec->userdata = NULL;
			spec->callback = SDL2ffmpeg::sdl_audio_callback;
			spec->freq     = sample_rate;
			spec->channels = channels;

			if (wantedSpec.silence != 0) {
				spec->silence = wantedSpec.silence;

			} else {
				spec->silence = 0;
			}

			if (wantedSpec.samples != 0) {
				spec->samples = wantedSpec.samples;

			} else {
				spec->samples = ASD_SAMPLES;
			}

			audioSpec = true;

		} else {
			clog << "SDL2ffmpeg::getAudioSpec ";
			clog << "invalid parameters: ";
			clog << "spec address = '" << spec << "' ";
			clog << "sample rate = '" << sample_rate << "' ";
			clog << "channels = '" << (short)channels << "' ";
			clog << endl;

			audioSpec = false;
		}

		return audioSpec;
	}

	char* SDL2ffmpeg::interleave(uint8_t* src, int srcLen, double ratio) {
		int i, j;
		int cvtRatio = ratio + 1;
		char* cvtbuf = (char*)malloc(cvtRatio * srcLen);

		for (i = 0; i < srcLen; ++i) {
			for (j = 0; j < cvtRatio; ++j) {
				memcpy(cvtbuf + (i * cvtRatio) + j, src + i, 1);
			}
		}

		return cvtbuf;
	}

	char* SDL2ffmpeg::createCVT(
			uint8_t* src, int srcLen, double ratio, int sampleSize) {

		char* cvtbuf = (char*)malloc((ratio + 1) * srcLen);
		int loop = ratio + 1;
		int i;

		for (i = 0; i < loop; i++) {
			memcpy(cvtbuf + (srcLen * i), src, srcLen);
		}

		return cvtbuf;
	}

	void SDL2ffmpeg::clamp(short* buf, int len) {
		int i;
		long value;

		for (i = 0; i < len; i++) {
			value = (long)buf[i];
			if (value > 0x7fff) {
				value = 0x7fff;

			} else if (value < -0x7fff) {
				value = -0x7fff;
			}

			buf[i] = (short)value;
		}
	}

	struct SwsContext* SDL2ffmpeg::createContext(
			int inWidth,
			int inHeight,
			enum PixelFormat inFormat,
			int outWidth,
			int outHeight,
			enum PixelFormat outFormat) {

		struct SwsContext* ctx;

		ctx = sws_getContext(
				inWidth, inHeight, inFormat,
				outWidth, outHeight, outFormat,
				//SWS_BILINEAR,
				SWS_BICUBIC,
				0,
				0,
				0);

		return ctx;
	}

	int SDL2ffmpeg::nts_packet_queue_put(PacketQueue *q, AVPacket *pkt) {
		AVPacketList* pkt1;

		if (q->abort_request) {
			return -1;
		}

		pkt1 = (AVPacketList*)av_malloc(sizeof(AVPacketList));

		if (!pkt1) {
			return -1;
		}

		pkt1->pkt  = *pkt;
		pkt1->next = NULL;

		if (!q->last_pkt) {
			q->first_pkt = pkt1;

		} else {
			q->last_pkt->next = pkt1;
		}

		q->last_pkt = pkt1;
		q->nb_packets++;
		q->size += pkt1->pkt.size + sizeof(*pkt1);

		/* XXX: should duplicate packet data in DV case */
		SDL_CondSignal(q->cond);

		return 0;
	}

	int SDL2ffmpeg::packet_queue_put(PacketQueue *q, AVPacket *pkt) {
		int ret;

		/* duplicate the packet */
		if (pkt != &flush_pkt && av_dup_packet(pkt) < 0) {
			return -1;
		}

		SDL_LockMutex(q->mutex);
		ret = nts_packet_queue_put(q, pkt);
		SDL_UnlockMutex(q->mutex);

		if (pkt != &flush_pkt && ret < 0) {
			av_free_packet(pkt);
		}

		return ret;
	}

	void SDL2ffmpeg::packet_queue_init(PacketQueue *q) {
		memset(q, 0, sizeof(PacketQueue));

		q->mutex = SDL_CreateMutex();
		q->cond  = SDL_CreateCond();

		q->abort_request = 1;
	}

	void SDL2ffmpeg::packet_queue_flush(PacketQueue *q) {
		AVPacketList* pkt;
		AVPacketList* pkt1;

		SDL_LockMutex(q->mutex);

		for (pkt = q->first_pkt; pkt != NULL; pkt = pkt1) {
			pkt1 = pkt->next;
			av_free_packet(&pkt->pkt);
			av_freep(&pkt);
		}

		q->last_pkt   = NULL;
		q->first_pkt  = NULL;
		q->nb_packets = 0;
		q->size       = 0;

		SDL_UnlockMutex(q->mutex);
	}

	void SDL2ffmpeg::packet_queue_destroy(PacketQueue *q) {
		packet_queue_flush(q);

		SDL_DestroyMutex(q->mutex);
		SDL_DestroyCond(q->cond);
	}

	void SDL2ffmpeg::packet_queue_abort(PacketQueue *q) {
		SDL_LockMutex(q->mutex);

		q->abort_request = 1;
		SDL_CondSignal(q->cond);

		SDL_UnlockMutex(q->mutex);
	}

	void SDL2ffmpeg::packet_queue_start(PacketQueue *q) {
		SDL_LockMutex(q->mutex);
		q->abort_request = 0;
		nts_packet_queue_put(q, &flush_pkt);
		SDL_UnlockMutex(q->mutex);
	}

	/* return < 0 if aborted, 0 if no packet and > 0 if packet.  */
	int SDL2ffmpeg::packet_queue_get(PacketQueue *q, AVPacket *pkt, int block) {
		AVPacketList *pkt1;
		int ret;

		SDL_LockMutex(q->mutex);

		for (;;) {
			if (q->abort_request) {
				ret = -1;
				break;
			}
			pkt1 = q->first_pkt;
			if (pkt1) {
				q->first_pkt = pkt1->next;

				if (!q->first_pkt) {
					q->last_pkt = NULL;
				}

				q->nb_packets--;
				q->size -= pkt1->pkt.size + sizeof(*pkt1);
				*pkt = pkt1->pkt;
				av_free(pkt1);
				ret = 1;
				break;

			} else if (!block) {
				ret = 0;
				break;

			} else {
				SDL_CondWait(q->cond, q->mutex);
			}
		}

		SDL_UnlockMutex(q->mutex);
		return ret;
	}

/*
	inline void fill_rectangle(SDL_Surface *screen,
									  int x, int y, int w, int h, int color) {
		SDL_Rect rect;
		rect.x = x;
		rect.y = y;
		rect.w = w;
		rect.h = h;
		SDL_FillRect(screen, &rect, color);
	}

	void blend_subrect(
			AVPicture *dst, const AVSubtitleRect *rect, int imgw, int imgh) {

		int wrap, wrap3, width2, skip2;
		int y, u, v, a, u1, v1, a1, w, h;
		uint8_t *lum, *cb, *cr;
		const uint8_t *p;
		const uint32_t *pal;
		int dstx, dsty, dstw, dsth;

		dstw = av_clip(rect->w, 0, imgw);
		dsth = av_clip(rect->h, 0, imgh);
		dstx = av_clip(rect->x, 0, imgw - dstw);
		dsty = av_clip(rect->y, 0, imgh - dsth);
		lum = dst->data[0] + dsty * dst->linesize[0];
		cb = dst->data[1] + (dsty >> 1) * dst->linesize[1];
		cr = dst->data[2] + (dsty >> 1) * dst->linesize[2];

		width2 = ((dstw + 1) >> 1) + (dstx & ~dstw & 1);
		skip2 = dstx >> 1;
		wrap = dst->linesize[0];
		wrap3 = rect->pict.linesize[0];
		p = rect->pict.data[0];
		pal = (const uint32_t *)rect->pict.data[1];  // Now in YCrCb!

		if (dsty & 1) {
			lum += dstx;
			cb += skip2;
			cr += skip2;

			if (dstx & 1) {
				YUVA_IN(y, u, v, a, p, pal);
				lum[0] = ALPHA_BLEND(a, lum[0], y, 0);
				cb[0] = ALPHA_BLEND(a >> 2, cb[0], u, 0);
				cr[0] = ALPHA_BLEND(a >> 2, cr[0], v, 0);
				cb++;
				cr++;
				lum++;
				p += BPP;
			}
			for(w = dstw - (dstx & 1); w >= 2; w -= 2) {
				YUVA_IN(y, u, v, a, p, pal);
				u1 = u;
				v1 = v;
				a1 = a;
				lum[0] = ALPHA_BLEND(a, lum[0], y, 0);

				YUVA_IN(y, u, v, a, p + BPP, pal);
				u1 += u;
				v1 += v;
				a1 += a;
				lum[1] = ALPHA_BLEND(a, lum[1], y, 0);
				cb[0] = ALPHA_BLEND(a1 >> 2, cb[0], u1, 1);
				cr[0] = ALPHA_BLEND(a1 >> 2, cr[0], v1, 1);
				cb++;
				cr++;
				p += 2 * BPP;
				lum += 2;
			}
			if (w) {
				YUVA_IN(y, u, v, a, p, pal);
				lum[0] = ALPHA_BLEND(a, lum[0], y, 0);
				cb[0] = ALPHA_BLEND(a >> 2, cb[0], u, 0);
				cr[0] = ALPHA_BLEND(a >> 2, cr[0], v, 0);
				p++;
				lum++;
			}
			p += wrap3 - dstw * BPP;
			lum += wrap - dstw - dstx;
			cb += dst->linesize[1] - width2 - skip2;
			cr += dst->linesize[2] - width2 - skip2;
		}
		for(h = dsth - (dsty & 1); h >= 2; h -= 2) {
			lum += dstx;
			cb += skip2;
			cr += skip2;

			if (dstx & 1) {
				YUVA_IN(y, u, v, a, p, pal);
				u1 = u;
				v1 = v;
				a1 = a;
				lum[0] = ALPHA_BLEND(a, lum[0], y, 0);
				p += wrap3;
				lum += wrap;
				YUVA_IN(y, u, v, a, p, pal);
				u1 += u;
				v1 += v;
				a1 += a;
				lum[0] = ALPHA_BLEND(a, lum[0], y, 0);
				cb[0] = ALPHA_BLEND(a1 >> 2, cb[0], u1, 1);
				cr[0] = ALPHA_BLEND(a1 >> 2, cr[0], v1, 1);
				cb++;
				cr++;
				p += -wrap3 + BPP;
				lum += -wrap + 1;
			}
			for(w = dstw - (dstx & 1); w >= 2; w -= 2) {
				YUVA_IN(y, u, v, a, p, pal);
				u1 = u;
				v1 = v;
				a1 = a;
				lum[0] = ALPHA_BLEND(a, lum[0], y, 0);

				YUVA_IN(y, u, v, a, p + BPP, pal);
				u1 += u;
				v1 += v;
				a1 += a;
				lum[1] = ALPHA_BLEND(a, lum[1], y, 0);
				p += wrap3;
				lum += wrap;

				YUVA_IN(y, u, v, a, p, pal);
				u1 += u;
				v1 += v;
				a1 += a;
				lum[0] = ALPHA_BLEND(a, lum[0], y, 0);

				YUVA_IN(y, u, v, a, p + BPP, pal);
				u1 += u;
				v1 += v;
				a1 += a;
				lum[1] = ALPHA_BLEND(a, lum[1], y, 0);

				cb[0] = ALPHA_BLEND(a1 >> 2, cb[0], u1, 2);
				cr[0] = ALPHA_BLEND(a1 >> 2, cr[0], v1, 2);

				cb++;
				cr++;
				p += -wrap3 + 2 * BPP;
				lum += -wrap + 2;
			}
			if (w) {
				YUVA_IN(y, u, v, a, p, pal);
				u1 = u;
				v1 = v;
				a1 = a;
				lum[0] = ALPHA_BLEND(a, lum[0], y, 0);
				p += wrap3;
				lum += wrap;
				YUVA_IN(y, u, v, a, p, pal);
				u1 += u;
				v1 += v;
				a1 += a;
				lum[0] = ALPHA_BLEND(a, lum[0], y, 0);
				cb[0] = ALPHA_BLEND(a1 >> 2, cb[0], u1, 1);
				cr[0] = ALPHA_BLEND(a1 >> 2, cr[0], v1, 1);
				cb++;
				cr++;
				p += -wrap3 + BPP;
				lum += -wrap + 1;
			}
			p += wrap3 + (wrap3 - dstw * BPP);
			lum += wrap + (wrap - dstw - dstx);
			cb += dst->linesize[1] - width2 - skip2;
			cr += dst->linesize[2] - width2 - skip2;
		}
		// handle odd height
		if (h) {
			lum += dstx;
			cb += skip2;
			cr += skip2;

			if (dstx & 1) {
				YUVA_IN(y, u, v, a, p, pal);
				lum[0] = ALPHA_BLEND(a, lum[0], y, 0);
				cb[0] = ALPHA_BLEND(a >> 2, cb[0], u, 0);
				cr[0] = ALPHA_BLEND(a >> 2, cr[0], v, 0);
				cb++;
				cr++;
				lum++;
				p += BPP;
			}
			for(w = dstw - (dstx & 1); w >= 2; w -= 2) {
				YUVA_IN(y, u, v, a, p, pal);
				u1 = u;
				v1 = v;
				a1 = a;
				lum[0] = ALPHA_BLEND(a, lum[0], y, 0);

				YUVA_IN(y, u, v, a, p + BPP, pal);
				u1 += u;
				v1 += v;
				a1 += a;
				lum[1] = ALPHA_BLEND(a, lum[1], y, 0);
				cb[0] = ALPHA_BLEND(a1 >> 2, cb[0], u, 1);
				cr[0] = ALPHA_BLEND(a1 >> 2, cr[0], v, 1);
				cb++;
				cr++;
				p += 2 * BPP;
				lum += 2;
			}
			if (w) {
				YUVA_IN(y, u, v, a, p, pal);
				lum[0] = ALPHA_BLEND(a, lum[0], y, 0);
				cb[0] = ALPHA_BLEND(a >> 2, cb[0], u, 0);
				cr[0] = ALPHA_BLEND(a >> 2, cr[0], v, 0);
			}
		}
	}
	*/

	void SDL2ffmpeg::free_subpicture(SubPicture *sp) {
		avsubtitle_free(&sp->sub);
	}

	void SDL2ffmpeg::video_image_display() {
		VideoPicture *vp;
		SubPicture *sp;
		AVPicture pict;
		float aspect_ratio;
		int width, height, x, y;
		SDL_Rect rect;
		int i;

		vp = &vs->pictq[vs->pictq_rindex];
		if (vp->tex) {
			/* XXX: use variable in the frame */
			if (vs->video_st->sample_aspect_ratio.num) {
				aspect_ratio = av_q2d(vs->video_st->sample_aspect_ratio);

			} else if (vs->video_st->codec->sample_aspect_ratio.num) {
				aspect_ratio = av_q2d(vs->video_st->codec->sample_aspect_ratio);

			} else {
				aspect_ratio = 0;
			}

			if (aspect_ratio <= 0.0) {
				aspect_ratio = 1.0;
			}

			aspect_ratio *= (float)vp->width / (float)vp->height;

			if (vs->subtitle_st) {
				if (vs->subpq_size > 0) {
					sp = &vs->subpq[vs->subpq_rindex];

					if (vp->pts >= sp->pts +
							((float) sp->sub.start_display_time / 1000)) {

						/*SDL_LockYUVOverlay (vp->bmp);

						pict.data[0] = vp->bmp->pixels[0];
						pict.data[1] = vp->bmp->pixels[2];
						pict.data[2] = vp->bmp->pixels[1];

						pict.linesize[0] = vp->bmp->pitches[0];
						pict.linesize[1] = vp->bmp->pitches[2];
						pict.linesize[2] = vp->bmp->pitches[1];

						for (i = 0; i < sp->sub.num_rects; i++)
							blend_subrect(&pict, sp->sub.rects[i],
									vp->bmp->w, vp->bmp->h);

						SDL_UnlockYUVOverlay (vp->bmp);*/
					}
				}
			}

			/* XXX: we suppose the screen has a 1.0 pixel ratio */
			height = vp->height;
			width = ((int)rint(height * aspect_ratio)) & ~1;
			if (width > vp->width) {
				width = vp->width;
				height = ((int)rint(width / aspect_ratio)) & ~1;
			}
			x = (vp->width - width) / 2;
			y = (vp->height - height) / 2;
			vs->no_background = 0;
			rect.x = x;
			rect.y = y;
			rect.w = FFMAX(width,  1);
			rect.h = FFMAX(height, 1);

			if (vs->img_convert_ctx == NULL) {
				vs->img_convert_ctx = createContext(
						vp->width,
						vp->height,
						vp->pix_fmt,
						vp->width,
						vp->height,
						PIX_FMT_RGB24);
			}

			if (vs->img_convert_ctx == NULL) {
				clog << "SDL2ffmpeg::video_image_display Warning! ";
				clog << "Can't initialize the conversion context" << endl;
				return;
			}

			if (vp->src_frame) {
				uint8_t* pixels[AV_NUM_DATA_POINTERS];
				int pitch[AV_NUM_DATA_POINTERS];

				SDL_LockTexture(vp->tex, NULL, (void**)&pixels, &pitch[0]);

				if (vp->tex &&
						vp->src_frame->data &&
						vp->src_frame->linesize > 0 &&
						vp->height > 0 &&
						vs->img_convert_ctx) {

					sws_scale(
							vs->img_convert_ctx,
							(const uint8_t* const*)vp->src_frame->data,
							vp->src_frame->linesize,
							0, vp->height, pixels, pitch);
				}

				SDL_UnlockTexture(vp->tex);
				hasPic = false;

			} else {
				clog << "SDL2ffmpeg::video_image_display Warning! ";
				clog << "No source frame" << endl;
				return;
			}

		} else {
			clog << "SDL2ffmpeg::video_image_display Can't display video ";
			clog << "(NULL texture)" << endl;
		}
	}

	void SDL2ffmpeg::stream_close() {
		VideoPicture *vp;
		int i;

		/* XXX: use a special url_shutdown call to abort parse cleanly */
		abortRequest = 1;
		SDL_WaitThread(vs->read_tid, NULL);

	    packet_queue_destroy(&vs->videoq);
	    packet_queue_destroy(&vs->audioq);
	    packet_queue_destroy(&vs->subtitleq);

		/* free all pictures */
		for (i = 0; i < VIDEO_PICTURE_QUEUE_SIZE; i++) {
			vp = &vs->pictq[i];
			if (vp->tex) {
				vp->tex = NULL;
			}
		}
		SDL_DestroyMutex(vs->pictq_mutex);
		SDL_DestroyCond(vs->pictq_cond);
		SDL_DestroyMutex(vs->subpq_mutex);
		SDL_DestroyCond(vs->subpq_cond);

		if (vs->img_convert_ctx) {
			sws_freeContext(vs->img_convert_ctx);
		}

		av_free(vs);

		clog << "SDL2ffmpeg::stream_close all done" << endl;
	}

	/* display the current picture, if any */
	void SDL2ffmpeg::video_display() {
		if (vs->video_st) {
			video_image_display();
		}
	}

	/* get the current audio clock value */
	double SDL2ffmpeg::get_audio_clock() {
		if (!vs->paused) {
			return vs->audio_current_pts_drift + av_gettime() / 1000000.0;

		} else {
			return vs->audio_current_pts;
		}
	}

	/* get the current video clock value */
	double SDL2ffmpeg::get_video_clock() {
		if (!vs->paused) {
			return vs->video_current_pts_drift + av_gettime() / 1000000.0;

		} else {
			return vs->video_current_pts;
		}
	}

	/* get the current external clock value */
	double SDL2ffmpeg::get_external_clock() {
		int64_t ti;
		ti = av_gettime();
		return vs->external_clock + ((ti - vs->external_clock_time) * 1e-6);
	}

	/* get the current master clock value */
	double SDL2ffmpeg::get_master_clock() {
		double val;

		if (vs->av_sync_type == AV_SYNC_VIDEO_MASTER) {
			if (vs->video_st) {
				val = get_video_clock();

			} else {
				val = get_audio_clock();
			}

		} else if (vs->av_sync_type == AV_SYNC_AUDIO_MASTER) {
			if (vs->audio_st) {
				val = get_audio_clock();
			} else {
				val = get_video_clock();
			}

		} else {
			val = get_external_clock();
		}

		return val;
	}

	/* seek in the stream */
	void SDL2ffmpeg::stream_seek(int64_t pos, int64_t rel, int seek_by_bytes) {
		if (!vs->seek_req) {
			vs->seek_pos = pos;
			vs->seek_rel = rel;
			vs->seek_flags &= ~AVSEEK_FLAG_BYTE;
			if (seek_by_bytes) {
				vs->seek_flags |= AVSEEK_FLAG_BYTE;
			}
			vs->seek_req = 1;
		}
	}

	/* pause or resume the video */
	void SDL2ffmpeg::stream_toggle_pause() {
	    if (vs->paused) {
	        vs->frame_timer += av_gettime() /
	        		1000000.0 +
	        		vs->video_current_pts_drift - vs->video_current_pts;

	        if (vs->read_pause_return != AVERROR(ENOSYS)) {
	            vs->video_current_pts = vs->video_current_pts_drift +
	            		av_gettime() / 1000000.0;
	        }

	        vs->video_current_pts_drift = vs->video_current_pts -
	        		av_gettime() / 1000000.0;
	    }

	    vs->paused = !vs->paused;
	}

	double SDL2ffmpeg::compute_target_delay(double delay) {
		double sync_threshold, diff;

		/* update delay to follow master synchronisation source */
		if (((vs->av_sync_type == AV_SYNC_AUDIO_MASTER && vs->audio_st) ||
				vs->av_sync_type == AV_SYNC_EXTERNAL_CLOCK)) {

			/* if video is slave, we try to correct big delays by
	           duplicating or deleting a frame */
			diff = get_video_clock() - get_master_clock();

			/* skip or repeat frame. We take into account the
	           delay to compute the threshold. I still don't know
	           if it is the best guess */
			sync_threshold = FFMAX(AV_SYNC_THRESHOLD, delay);
			if (fabs(diff) < AV_NOSYNC_THRESHOLD) {
				if (diff <= -sync_threshold) {
					delay = 0;

				} else if (diff >= sync_threshold) {
					delay = 2 * delay;
				}
			}
		}

		av_dlog(NULL, "video: delay=%0.3f A-V=%f\n", delay, -diff);

		return delay;
	}

	void SDL2ffmpeg::pictq_next_picture() {
		/* update queue size and signal for next picture */
		if (++vs->pictq_rindex == VIDEO_PICTURE_QUEUE_SIZE) {
			vs->pictq_rindex = 0;
		}

		SDL_LockMutex(vs->pictq_mutex);
		vs->pictq_size--;
		SDL_CondSignal(vs->pictq_cond);
		SDL_UnlockMutex(vs->pictq_mutex);
	}

	void SDL2ffmpeg::update_video_pts(double pts, int64_t pos) {
		double time = av_gettime() / 1000000.0;
		/* update current video pts */
		vs->video_current_pts = pts;
		vs->video_current_pts_drift = vs->video_current_pts - time;
		vs->video_current_pos = pos;
		vs->frame_last_pts = pts;
	}

	/* called to display each frame */
	void SDL2ffmpeg::video_refresh(void *opaque) {
		SDL2ffmpeg* dec = (SDL2ffmpeg*)opaque;
		VideoState* vs  = dec->vs;
		VideoPicture *vp;
		double time;

		SubPicture *sp, *sp2;

		vs->refresh = 1;

		if (dec->allocate) {
			dec->alloc_picture();
		}

		if (vs->video_st) {
retry:
			if (vs->pictq_size == 0) {
				SDL_LockMutex(vs->pictq_mutex);
				if (vs->frame_last_dropped_pts != AV_NOPTS_VALUE &&
						vs->frame_last_dropped_pts > vs->frame_last_pts) {

					dec->update_video_pts(
							vs->frame_last_dropped_pts,
							vs->frame_last_dropped_pos);

					vs->frame_last_dropped_pts = AV_NOPTS_VALUE;
				}

				SDL_UnlockMutex(vs->pictq_mutex);
				// nothing to do, no picture to display in the queue

			} else {
				double last_duration, duration, delay;
				/* dequeue the picture */
				vp = &vs->pictq[vs->pictq_rindex];

				if (vp->skip) {
					dec->pictq_next_picture();
					goto retry;
				}

				if (vs->paused)
					goto display;

				/* compute nominal last_duration */
				last_duration = vp->pts - vs->frame_last_pts;
				if (last_duration > 0 && last_duration < 10.0) {
					/*
					 * if duration of the last frame was sane,
					 * update last_duration in video state
					 */
					vs->frame_last_duration = last_duration;
				}

				delay = dec->compute_target_delay(vs->frame_last_duration);

				time = av_gettime()/1000000.0;
				if (time < vs->frame_timer + delay) {
					vs->refresh = 0;
					return;
				}

				if (delay > 0) {
					vs->frame_timer += delay *
							FFMAX(1, floor((time-vs->frame_timer) / delay));
				}

				SDL_LockMutex(vs->pictq_mutex);
				dec->update_video_pts(vp->pts, vp->pos);
				SDL_UnlockMutex(vs->pictq_mutex);

				if (vs->pictq_size > 1) {
					VideoPicture *nextvp = &vs->pictq[(
							vs->pictq_rindex + 1) % VIDEO_PICTURE_QUEUE_SIZE];

					/*
					 *  More accurate this way, 1/time_base is often
					 *  not reflecting FPS
					 */
					duration = nextvp->pts - vp->pts;

				} else {
					duration = vp->duration;
				}

				if ((dec->framedrop>0 || (dec->framedrop && vs->audio_st)) &&
						time > vs->frame_timer + duration) {

					if (vs->pictq_size > 1) {
						vs->frame_drops_late++;
						dec->pictq_next_picture();
						goto retry;
					}
				}

				if (vs->subtitle_st) {
					if (vs->subtitle_stream_changed) {
						SDL_LockMutex(vs->subpq_mutex);

						while (vs->subpq_size) {
							dec->free_subpicture(&vs->subpq[vs->subpq_rindex]);

							/* update queue size and signal for next picture */
							if (++vs->subpq_rindex == SUBPICTURE_QUEUE_SIZE) {
								vs->subpq_rindex = 0;
							}

							vs->subpq_size--;
						}

						vs->subtitle_stream_changed = 0;

						SDL_CondSignal(vs->subpq_cond);
						SDL_UnlockMutex(vs->subpq_mutex);

					} else {
						if (vs->subpq_size > 0) {
							sp = &vs->subpq[vs->subpq_rindex];

							if (vs->subpq_size > 1) {
								sp2 = &vs->subpq[((vs->subpq_rindex + 1) %
										SUBPICTURE_QUEUE_SIZE)];

							} else {
								sp2 = NULL;
							}

							if ((vs->video_current_pts > (sp->pts + (
									(float) sp->sub.end_display_time / 1000)))
									|| (sp2 && vs->video_current_pts > (
											sp2->pts + ((float)
													sp2->sub.start_display_time
													/ 1000)))) {

								dec->free_subpicture(sp);

								/*
								 * update queue size and
								 * signal for next picture
								 */
								if (++vs->subpq_rindex ==
										SUBPICTURE_QUEUE_SIZE) {

									vs->subpq_rindex = 0;
								}

								SDL_LockMutex(vs->subpq_mutex);
								vs->subpq_size--;
								SDL_CondSignal(vs->subpq_cond);
								SDL_UnlockMutex(vs->subpq_mutex);
							}
						}
					}
				}

display:
				/* display picture */
				dec->video_display();

				if (!vs->paused) {
					dec->pictq_next_picture();
				}
			}
		}

		vs->refresh = 0;
	}


	/* allocate a picture (needs to do that in main thread to avoid
	 * potential locking problems
	 */
	void SDL2ffmpeg::alloc_picture() {
	    VideoPicture *vp;

	    vp = &vs->pictq[vs->pictq_windex];

	    vp->width   = vs->video_st->codec->width;
	    vp->height  = vs->video_st->codec->height;
	    vp->pix_fmt = vs->video_st->codec->pix_fmt;
	    vp->tex       = texture;

	    SDL_LockMutex(vs->pictq_mutex);
	    vp->allocated = 1;
	    allocate = false;
	    SDL_CondSignal(vs->pictq_cond);
	    SDL_UnlockMutex(vs->pictq_mutex);
	}

	int SDL2ffmpeg::queue_picture(
			AVFrame *src_frame, double pts1, int64_t pos) {

		VideoPicture *vp;
		double frame_delay, pts = pts1;

		/* compute the exact PTS for the picture if it is omitted in the stream
		 * pts1 is the dts of the pkt / pts of the frame */
		if (pts != 0) {
			/* update video clock with pts, if present */
			vs->video_clock = pts;

		} else {
			pts = vs->video_clock;
		}

		/* update video clock for next frame */
		frame_delay = av_q2d(vs->video_st->codec->time_base);
		/* for MPEG2, the frame can be repeated, so we update the
		   clock accordingly */
		frame_delay += src_frame->repeat_pict * (frame_delay * 0.5);
		vs->video_clock += frame_delay;

		/* wait until we have space to put a new picture */
		SDL_LockMutex(vs->pictq_mutex);

		while (vs->pictq_size >= VIDEO_PICTURE_QUEUE_SIZE &&
				!vs->videoq.abort_request) {

			SDL_CondWait(vs->pictq_cond, vs->pictq_mutex);
		}

		SDL_UnlockMutex(vs->pictq_mutex);

		if (vs->videoq.abort_request) {
			return -1;
		}

		vp = &vs->pictq[vs->pictq_windex];

		vp->duration = frame_delay;

		/* alloc or resize hardware picture buffer */
		if (!vp->tex || vp->reallocate ||
				vp->width != vs->video_st->codec->width ||
				vp->height != vs->video_st->codec->height) {

			//SDL_Event event;

			vp->allocated  = 0;
			vp->reallocate = 0;

			/*
			 * the allocation must be done in the main thread to avoid
			 * locking problems
			 *
			event.type = FF_ALLOC_EVENT;
			event.user.data1 = this;
			event.user.data2 = this->cmp;
			SDL_PushEvent(&event);*/

			allocate = true;

	        /* wait until the picture is allocated */
	        SDL_LockMutex(vs->pictq_mutex);
	        while (!vp->allocated && !vs->videoq.abort_request) {
	            SDL_CondWait(vs->pictq_cond, vs->pictq_mutex);
	        }

	        /*
	         * if the queue is aborted, we have to pop the pending ALLOC event
	         * or wait for the allocation to complete
	         *
	        if (vs->videoq.abort_request &&
	        		SDL_PeepEvents(
	        				&event,
	        				1,
	        				SDL_GETEVENT,
	        				FF_ALLOC_EVENT,
	        				FF_ALLOC_EVENT) != 1) {

	            while (!vp->allocated) {
	                SDL_CondWait(vs->pictq_cond, vs->pictq_mutex);
	            }
	        }*/
	        SDL_UnlockMutex(vs->pictq_mutex);

			if (vs->videoq.abort_request) {
				return -1;
			}
		}

		/* if the frame is not skipped, then display it */
		if (vp->tex) {
			vp->src_frame = src_frame;

			vp->pts  = pts;
			vp->pos  = pos;
			vp->skip = 0;

			/* now we can update the picture count */
			if (++vs->pictq_windex == VIDEO_PICTURE_QUEUE_SIZE) {
				vs->pictq_windex = 0;
			}

			SDL_LockMutex(vs->pictq_mutex);
			vs->pictq_size++;
			SDL_UnlockMutex(vs->pictq_mutex);
		}
		return 0;
	}

	int SDL2ffmpeg::get_video_frame(
			AVFrame *frame, int64_t *pts, AVPacket *pkt) {

		int got_picture, i;

		if (packet_queue_get(&vs->videoq, pkt, 1) < 0 ||
				vs->videoq.abort_request) {

			return -1;
		}

		if (pkt->data == flush_pkt.data) {
			avcodec_flush_buffers(vs->video_st->codec);

			SDL_LockMutex(vs->pictq_mutex);
			/*
			 * Make sure there are no long delay timers
			 * (ideally we should just flush the que but thats harder)
			 */
			for (i = 0; i < VIDEO_PICTURE_QUEUE_SIZE; i++) {
				vs->pictq[i].skip = 1;
			}

			while (vs->pictq_size && !vs->videoq.abort_request) {
				SDL_CondWait(vs->pictq_cond, vs->pictq_mutex);
			}

			vs->video_current_pos = -1;
			vs->frame_last_pts = AV_NOPTS_VALUE;
			vs->frame_last_duration = 0;
			vs->frame_timer = (double)av_gettime() / 1000000.0;
			vs->frame_last_dropped_pts = AV_NOPTS_VALUE;

			SDL_UnlockMutex(vs->pictq_mutex);

			return 0;
		}

		avcodec_decode_video2(vs->video_st->codec, frame, &got_picture, pkt);

		if (got_picture) {
			int ret = 1;

			if (decoder_reorder_pts == -1) {
				*pts = *(int64_t*)av_opt_ptr(
						avcodec_get_frame_class(),
						frame,
						"best_effort_timestamp");

			} else if (decoder_reorder_pts) {
				*pts = frame->pkt_pts;

			} else {
				*pts = frame->pkt_dts;
			}

			if (*pts == AV_NOPTS_VALUE) {
				*pts = 0;
			}

			if (((vs->av_sync_type == AV_SYNC_AUDIO_MASTER && vs->audio_st) ||
					vs->av_sync_type == AV_SYNC_EXTERNAL_CLOCK) &&
					(framedrop > 0 || (framedrop && vs->audio_st))) {

				SDL_LockMutex(vs->pictq_mutex);
				if (vs->frame_last_pts != AV_NOPTS_VALUE && *pts) {
					double clockdiff = get_video_clock() - get_master_clock();
					double dpts      = av_q2d(vs->video_st->time_base) * *pts;
					double ptsdiff   = dpts - vs->frame_last_pts;
					if (fabs(clockdiff) < AV_NOSYNC_THRESHOLD &&
							ptsdiff > 0 && ptsdiff < AV_NOSYNC_THRESHOLD &&
							clockdiff + ptsdiff -
									vs->frame_last_filter_delay < 0) {

						vs->frame_last_dropped_pos = pkt->pos;
						vs->frame_last_dropped_pts = dpts;
						vs->frame_drops_early++;
						ret = 0;
					}
				}

				SDL_UnlockMutex(vs->pictq_mutex);
			}

			if (ret) {
				vs->frame_last_returned_time = av_gettime() / 1000000.0;
			}

			return ret;
		}

		return 0;
	}

	int SDL2ffmpeg::video_thread(void *arg) {
		SDL2ffmpeg* dec = (SDL2ffmpeg*)arg;
		VideoState* vs  = dec->vs;
		AVFrame* frame  = avcodec_alloc_frame();
		int64_t pts_int = AV_NOPTS_VALUE, pos = -1;

		double pts;
		int ret;

		while (!dec->abortRequest) {
			AVPacket pkt;
			while (vs->paused && !vs->videoq.abort_request) {
				SDL_Delay(10);
			}

			ret = dec->get_video_frame(frame, &pts_int, &pkt);
			pos = pkt.pos;
			av_free_packet(&pkt);
	        if (ret == 0) {
	        	continue;
	        }

			if (ret < 0) {
				goto the_end;
			}

			vs->frame_last_filter_delay = av_gettime() / 1000000.0 -
					vs->frame_last_returned_time;

			if (fabs(vs->frame_last_filter_delay) > AV_NOSYNC_THRESHOLD / 10.0){
				vs->frame_last_filter_delay = 0;
			}

			pts = pts_int * av_q2d(vs->video_st->time_base);

			ret = dec->queue_picture(frame, pts, pos);

			if (ret < 0) {
				goto the_end;
			}

			if (vs->step) {
				dec->stream_toggle_pause();
			}
		}
the_end:
		avcodec_flush_buffers(vs->video_st->codec);
		av_free(frame);

		clog << "SDL2ffmpeg::video_thread all done" << endl;
		return 0;
	}


	int SDL2ffmpeg::subtitle_thread(void *arg) {
		/*VideoState *is = arg;
	    SubPicture *sp;
	    AVPacket pkt1, *pkt = &pkt1;
	    int got_subtitle;
	    double pts;
	    int i, j;
	    int r, g, b, y, u, v, a;

	    for (;;) {
	        while (is->paused && !is->subtitleq.abort_request) {
	            SDL_Delay(10);
	        }
	        if (packet_queue_get(&is->subtitleq, pkt, 1) < 0)
	            break;

	        if (pkt->data == flush_pkt.data) {
	            avcodec_flush_buffers(is->subtitle_st->codec);
	            continue;
	        }
	        SDL_LockMutex(is->subpq_mutex);
	        while (is->subpq_size >= SUBPICTURE_QUEUE_SIZE &&
	               !is->subtitleq.abort_request) {
	            SDL_CondWait(is->subpq_cond, is->subpq_mutex);
	        }
	        SDL_UnlockMutex(is->subpq_mutex);

	        if (is->subtitleq.abort_request)
	            return 0;

	        sp = &is->subpq[is->subpq_windex];

	       //NOTE: ipts is the PTS of the _first_ picture beginning in
	       //    this packet, if any
	        pts = 0;
	        if (pkt->pts != AV_NOPTS_VALUE)
	            pts = av_q2d(is->subtitle_st->time_base) * pkt->pts;

	        avcodec_decode_subtitle2(is->subtitle_st->codec, &sp->sub,
	                                 &got_subtitle, pkt);

	        if (got_subtitle && sp->sub.format == 0) {
	            sp->pts = pts;

	            for (i = 0; i < sp->sub.num_rects; i++)
	            {
	                for (j = 0; j < sp->sub.rects[i]->nb_colors; j++)
	                {
	                    RGBA_IN(r, g, b, a,
	                    (uint32_t*)sp->sub.rects[i]->pict.data[1] + j);
	                    y = RGB_TO_Y_CCIR(r, g, b);
	                    u = RGB_TO_U_CCIR(r, g, b, 0);
	                    v = RGB_TO_V_CCIR(r, g, b, 0);
	                    YUVA_OUT((uint32_t*)sp->sub.rects[i]->pict.data[1] + j,
	                     y, u, v, a);
	                }
	            }

	            // now we can update the picture count
	            if (++is->subpq_windex == SUBPICTURE_QUEUE_SIZE)
	                is->subpq_windex = 0;
	            SDL_LockMutex(is->subpq_mutex);
	            is->subpq_size++;
	            SDL_UnlockMutex(is->subpq_mutex);
	        }
	        av_free_packet(pkt);
	    }*/

		clog << "SDL2ffmpeg::subtitle_thread all done" << endl;
		return 0;
	}

	/*
	 * return the new audio buffer size (samples can be added or deleted
	 * to get better sync if video or external master clock)
	 */
	int SDL2ffmpeg::synchronize_audio(int nb_samples) {
		int wanted_nb_samples = nb_samples;

		/* if not master, then we try to remove or add samples to correct the clock */
		if (((vs->av_sync_type == AV_SYNC_VIDEO_MASTER && vs->video_st) ||
				vs->av_sync_type == AV_SYNC_EXTERNAL_CLOCK)) {

			double diff, avg_diff;
			int min_nb_samples, max_nb_samples;

			diff = get_audio_clock() - get_master_clock();

			if (diff < AV_NOSYNC_THRESHOLD) {
				vs->audio_diff_cum = diff +
						vs->audio_diff_avg_coef * vs->audio_diff_cum;

				if (vs->audio_diff_avg_count < AUDIO_DIFF_AVG_NB) {
					/* not enough measures to have a correct estimate */
					vs->audio_diff_avg_count++;

				} else {
					/* estimate the A-V difference */
					avg_diff = vs->audio_diff_cum *
							(1.0 - vs->audio_diff_avg_coef);

					if (fabs(avg_diff) >= vs->audio_diff_threshold) {
						wanted_nb_samples = nb_samples +
								(int)(diff * vs->audio_src_freq);

						min_nb_samples = ((nb_samples *
								(100 - SAMPLE_CORRECTION_PERCENT_MAX) / 100));

						max_nb_samples = ((nb_samples *
								(100 + SAMPLE_CORRECTION_PERCENT_MAX) / 100));

						wanted_nb_samples = FFMIN(FFMAX(
								wanted_nb_samples,
								min_nb_samples), max_nb_samples);
					}
				}

			} else {
				/* too big difference : may be initial PTS errors, so
	               reset A-V filter */
				vs->audio_diff_avg_count = 0;
				vs->audio_diff_cum       = 0;
			}
		}

		return wanted_nb_samples;
	}

	/* decode one audio frame and returns its uncompressed size */
	int SDL2ffmpeg::audio_decode_frame(double *pts_ptr) {
		AVPacket *pkt_temp;
		AVPacket *pkt;
		AVCodecContext *dec;
		int len1, len2, data_size, resampled_data_size;
		int64_t dec_channel_layout;
		int got_frame;
		double pts;
		int new_packet = 0;
		int flush_complete = 0;
		int wanted_nb_samples;

		if (vs->audio_st == NULL) {
			return -1;
		}

		pkt_temp = &vs->audio_pkt_temp;
		pkt = &vs->audio_pkt;
		dec = vs->audio_st->codec;

		while (!abortRequest) {
			/* NOTE: the audio packet can contain several frames */
			while (pkt_temp->size > 0 || (!pkt_temp->data && new_packet)) {
				if (!vs->frame) {
					if (!(vs->frame = avcodec_alloc_frame())){
						return AVERROR(ENOMEM);
					}

				} else {
					avcodec_get_frame_defaults(vs->frame);
				}

				if (flush_complete) {
					break;
				}

				new_packet = 0;
				len1 = avcodec_decode_audio4(
						dec, vs->frame, &got_frame, pkt_temp);

				if (len1 < 0) {
					/* if error, we skip the frame */
					pkt_temp->size = 0;
					break;
				}

				pkt_temp->data += len1;
				pkt_temp->size -= len1;

				if (!got_frame) {
					/* stop sending empty packets if the decoder is finished */
					if (!pkt_temp->data &&
							(dec->codec->capabilities & CODEC_CAP_DELAY)) {

						flush_complete = 1;
					}
					continue;
				}
				data_size = av_samples_get_buffer_size(
						NULL,
						dec->channels,
						vs->frame->nb_samples,
						dec->sample_fmt,
						1);

				dec_channel_layout = (dec->channel_layout && dec->channels ==
						av_get_channel_layout_nb_channels(
								dec->channel_layout)) ? dec->channel_layout :
										av_get_default_channel_layout(
												dec->channels);

				wanted_nb_samples = synchronize_audio(vs->frame->nb_samples);

				if (dec->sample_fmt != vs->audio_src_fmt ||
						dec_channel_layout != vs->audio_src_channel_layout ||
						dec->sample_rate != vs->audio_src_freq ||
						(wanted_nb_samples != vs->frame->nb_samples &&
								!vs->swr_ctx)) {

					if (vs->swr_ctx) {
						swr_free(&vs->swr_ctx);
					}

					vs->swr_ctx = swr_alloc_set_opts(
							NULL,
							vs->audio_tgt_channel_layout,
							vs->audio_tgt_fmt,
							vs->audio_tgt_freq,
							dec_channel_layout,
							dec->sample_fmt,
							dec->sample_rate,
							0, NULL);

					if (!vs->swr_ctx || swr_init(vs->swr_ctx) < 0) {
						fprintf(stderr,
								"Cannot create sample rate converter for conversion of %d Hz %s %d channels to %d Hz %s %d channels!\n",
								dec->sample_rate,
								av_get_sample_fmt_name(dec->sample_fmt),
								dec->channels,
								vs->audio_tgt_freq,
								av_get_sample_fmt_name(vs->audio_tgt_fmt),
								vs->audio_tgt_channels);
						break;
					}

					vs->audio_src_channel_layout = dec_channel_layout;
					vs->audio_src_channels = dec->channels;
					vs->audio_src_freq = dec->sample_rate;
					vs->audio_src_fmt = dec->sample_fmt;
				}

				resampled_data_size = data_size;
				if (vs->swr_ctx) {
					const uint8_t *in[] = { vs->frame->data[0] };
					uint8_t *out[] = {vs->audio_buf2};

					if (wanted_nb_samples != vs->frame->nb_samples) {
						if (swr_set_compensation(
								vs->swr_ctx,
								(wanted_nb_samples - vs->frame->nb_samples)
										* vs->audio_tgt_freq / dec->sample_rate,
								wanted_nb_samples
										* vs->audio_tgt_freq /
										dec->sample_rate) < 0) {

							fprintf(stderr, "swr_set_compensation() failed\n");
							break;
						}
					}

					len2 = swr_convert(
							vs->swr_ctx,
							out,
							sizeof(vs->audio_buf2) /
									vs->audio_tgt_channels /
									av_get_bytes_per_sample(vs->audio_tgt_fmt),
							in, vs->frame->nb_samples);

					if (len2 < 0) {
						fprintf(stderr, "audio_resample() failed\n");
						break;
					}

					if (len2 == sizeof(
							vs->audio_buf2) / vs->audio_tgt_channels /
							av_get_bytes_per_sample(vs->audio_tgt_fmt)) {

						fprintf(stderr, "warning: audio buffer is probably too small\n");
						swr_init(vs->swr_ctx);
					}

					vs->audio_buf = vs->audio_buf2;
					resampled_data_size = len2 * vs->audio_tgt_channels *
							av_get_bytes_per_sample(vs->audio_tgt_fmt);

				} else {
					vs->audio_buf = vs->frame->data[0];
				}

				/* if no pts, then compute it */
				pts = vs->audio_clock;
				*pts_ptr = pts;

	            vs->audio_clock += (double)data_size / (dec->channels *
	            		dec->sample_rate * av_get_bytes_per_sample(
	            				dec->sample_fmt));

				return resampled_data_size;
			}

			/* free the current packet */
			if (pkt->data) {
				av_free_packet(pkt);
			}
			memset(pkt_temp, 0, sizeof(*pkt_temp));

			if (vs->paused || vs->audioq.abort_request) {
				return -1;
			}

			/* read next packet */
			if ((new_packet = packet_queue_get(&vs->audioq, pkt, 1)) < 0) {
				return -1;
			}

			if (pkt->data == flush_pkt.data) {
				avcodec_flush_buffers(dec);
				flush_complete = 0;
			}

			*pkt_temp = *pkt;

			/* if update the audio clock with the pts */
			if (pkt->pts != AV_NOPTS_VALUE) {
				vs->audio_clock = av_q2d(vs->audio_st->time_base)*pkt->pts;
			}
		}

		return 0;
	}

	/* prepare a new audio buffer */
/*	void SDL2ffmpeg::sdl_audio_callback(void *opaque, Uint8 *stream, int len) {
		SDL2ffmpeg* dec;
		VideoState* vs;

		int audio_size, len1;
		int bytes_per_sec;
		int frame_size;
		double pts;

		set<SDL2ffmpeg*>::iterator i;

		pthread_mutex_lock(&aiMutex);

		i = aInstances.begin();
		while (i != aInstances.end()) {
			dec = (SDL2ffmpeg*)(*i);
			vs  = dec->vs;

			frame_size = av_samples_get_buffer_size(
					NULL, vs->audio_tgt_channels, 1, vs->audio_tgt_fmt, 1);

			dec->audio_callback_time = av_gettime();

			while (len > 0) {
				if (vs->audio_buf_index >= vs->audio_buf_size) {
					audio_size = dec->audio_decode_frame(&pts);
					if (audio_size < 0) {
*/
						/* if error, just output silence */
/*						vs->audio_buf      = vs->silence_buf;
						vs->audio_buf_size = sizeof(vs->silence_buf) / frame_size *
								frame_size;

					} else {
						vs->audio_buf_size = audio_size;
					}
					vs->audio_buf_index = 0;
				}
				len1 = vs->audio_buf_size - vs->audio_buf_index;
				if (len1 > len) {
					len1 = len;
				}

				memcpy(
						stream,
						(uint8_t *)vs->audio_buf + vs->audio_buf_index,
						len1);

				len -= len1;
				stream += len1;
				vs->audio_buf_index += len1;
			}

			bytes_per_sec = vs->audio_tgt_freq * vs->audio_tgt_channels *
					av_get_bytes_per_sample(vs->audio_tgt_fmt);

			vs->audio_write_buf_size = vs->audio_buf_size - vs->audio_buf_index;
*/
			/*
			 * Let's assume the audio driver that is used by SDL has two periods.
			 */
/*			vs->audio_current_pts = vs->audio_clock - (double)(2 *
					vs->audio_hw_buf_size + vs->audio_write_buf_size) /
					bytes_per_sec;

			vs->audio_current_pts_drift = vs->audio_current_pts -
					dec->audio_callback_time / 1000000.0;

			++i;
		}
		pthread_mutex_unlock(&aiMutex);
	}
*/

	void SDL2ffmpeg::sdl_audio_callback(void* opaque, Uint8* stream, int len) {
		set<SDL2ffmpeg*>::iterator i;
		SDL2ffmpeg* dec;
		VideoState* vs;

		int64_t audio_cb_time;
		int audio_size;
		int bytes_per_sec;
		int ret;
		double pts;
		bool hasTimeRefer = false;

		int64_t uResponseTime;
		int64_t elapsedTime;

		int cvt_len;

		unsigned int sleepTime;

		bool presented = false;

		//clog << "SDL2ffmpeg::sdl_audio_callback begin" << endl;

		/*if (wantedSpec.freq == 0 || wantedSpec.samples == 0) {
			clog << "SDL2ffmpeg::sdl_audio_callback Warning frequency is 0!";
			clog << " Exiting" << endl;
			return;
		}*/

		pthread_mutex_lock(&aiMutex);
		audio_cb_time = av_gettime();

		memset(stream, 0, len);

		i = aInstances.begin();
		while (i != aInstances.end()) {
			dec = (*i);
			vs  = dec->vs;

			if (dec->status == ST_PLAYING && vs != NULL &&
					vs->audio_stream >= 0 && vs->audio_st != NULL) {

				if (vs->video_stream >= 0 && vs->video_st != NULL) {
					hasTimeRefer = true;
				}

				dec->audio_refresh_decoder();
				if (vs->audio_main_buf_size[0] == vs->audio_hw_buf_size) {

					if (dec->acvt.needed &&
							dec->wantedSpec.channels <= spec.channels) {

						dec->acvt.len = vs->audio_hw_buf_size;

						if (dec->acvt.buf == NULL) {
							dec->acvt.buf = (Uint8*)malloc(
									dec->acvt.len * dec->acvt.len_mult);
						}

						if (dec->acvt.buf != NULL) {
							memcpy(
									dec->acvt.buf,
									vs->audio_main_buf[0],
									vs->audio_hw_buf_size);

							SDL_ConvertAudio(&dec->acvt);

							/*clog << endl;
							clog << "Converting(cb len = '" << len;
							clog << "', Dec = '";
							clog << dec << "', bytes to convert = '";
							clog << dec->acvt.len << "', soundLevel = '";
							clog << dec->soundLevel << "')" << endl;
							clog << "FROM: ";
							clog << "format '" << dec->wantedSpec.format << "' ";
							clog << "channels '";
							clog << (short)dec->wantedSpec.channels;
							clog << "' freq '" << dec->wantedSpec.freq << "' ";
							clog << "' samples '" << dec->wantedSpec.samples;
							clog << "' bufSize '" << vs->audio_hw_buf_size;
							clog << "'";
							clog << endl;
							clog << "TO: ";
							clog << "format '" << dec->spec.format;
							clog << "' channels '";
							clog << (short)dec->spec.channels;
							clog << "' freq '" << dec->spec.freq;
							clog << "' samples '" << dec->spec.samples;
							clog << "' converted size '";
							clog << dec->acvt.len_cvt;
							clog << "'";
							clog << "' converted length ratio '";
							clog << dec->acvt.len_ratio;
							clog << "'";
							clog << endl;

							clog << "Mixed '" << len << "' bytes" << endl;
							clog << "Total in this step '" << dec->monoStep;
							clog << "'" << endl;*/

							if (dec->soundLevel > 0) {
								SDL_MixAudio(
										stream,
										dec->acvt.buf,
										len,
										dec->soundLevel);
							}

							free(dec->acvt.buf);
							dec->acvt.buf = NULL;

							/*clog << "CB_LEN = '" << len << "'" << endl;
							clog << "MY_LEN = '" << vs->audio_write_buf_size;
							clog << "'" << endl;
							clog << "MULTI = '" << dec->acvt.len_mult;
							clog << "'" << endl;
							clog << "CVT_LEN = '" << dec->acvt.len_cvt;
							clog << "'" << endl;*/

							presented = true;
						}

					} else {

						if (dec->soundLevel > 0) {
							SDL_MixAudio(
									stream,
									vs->audio_main_buf[0],
									vs->audio_hw_buf_size,
									dec->soundLevel);
						}

						presented = true;
					}

				}/* else {
					clog << endl << endl;
					clog << "SDL2ffmpeg::sdl_audio_callback ";
					clog << "not this time for " << vs->filename;
					clog << " audio buffer size = ";
					clog << vs->audio_main_buf_size[0];
					clog << " HW buffer size = " << vs->audio_hw_buf_size;
					clog << " len = " << len;
					clog << " samples = " << dec->wantedSpec.samples;
					clog << " freq = " << dec->wantedSpec.freq;
					clog << " channels = " << (short)dec->wantedSpec.channels;
					clog << endl;
				}*/
			}

			if (presented) {
				presented = false;

				bytes_per_sec = vs->audio_tgt_freq *
						vs->audio_tgt_channels *
						av_get_bytes_per_sample(vs->audio_tgt_fmt);

				vs->audio_write_buf_size = vs->audio_hw_buf_size;

				vs->audio_current_pts = vs->audio_clock -
						(double)(2 * vs->audio_hw_buf_size +
								vs->audio_write_buf_size) / bytes_per_sec;

				vs->audio_current_pts_drift = vs->audio_current_pts -
						audio_cb_time / 1000000.0;

				vs->audio_main_buf_size[0] = 0;
			}

			++i;
		}

		elapsedTime = av_gettime() - audio_cb_time;
		sleepTime   = 1000000/30;

		pthread_mutex_unlock(&aiMutex);

		if (!hasTimeRefer && sleepTime > elapsedTime) {
			SystemCompat::uSleep(sleepTime - elapsedTime);
		}
	}

	int SDL2ffmpeg::audio_refresh_decoder() {
		int audio_size;
		double pts;
		int len1, offset;

		if (!abortRequest &&
				vs->audio_stream >= 0 && vs->audio_hw_buf_size != 0) {

			if (vs->audio_main_buf_size[0] >= vs->audio_hw_buf_size) {
				return 0;
			}

			while (vs->audio_main_buf_size[0] < vs->audio_hw_buf_size) {
				if (abortRequest || (reof && vs->audioq.size == 0)) {
					return -1;
				}

				audio_size = audio_decode_frame(&pts);
				if (abortRequest || audio_size < 0) {
					if (abortRequest) {
						return -1;

					} else {
						continue;
					}
				}

				audio_size = synchronize_audio(audio_size);
				if (!abortRequest && audio_size > 0) {
					if (vs->audio_main_buf_size[0] == 0 &&
							vs->audio_main_buf_size[1] != 0) {

						uint8_t* aux;

						aux = vs->audio_main_buf[0];
						vs->audio_main_buf[0] = vs->audio_main_buf[1];
						vs->audio_main_buf[1] = aux;

						vs->audio_main_buf_size[0] = vs->audio_main_buf_size[1];
						vs->audio_main_buf_size[1] = 0;
					}

					if (vs->audio_main_buf_size[0] == 0) {
						if (audio_size <= vs->audio_hw_buf_size) {
							memcpy(
									vs->audio_main_buf[0],
									vs->audio_buf,
									audio_size);

							vs->audio_main_buf_size[0] = audio_size;

						} else {
							len1 = audio_size - vs->audio_hw_buf_size;

							memcpy(
									vs->audio_main_buf[0],
									vs->audio_buf,
									vs->audio_hw_buf_size);

							vs->audio_main_buf_size[0] = vs->audio_hw_buf_size;

							if (len1 > vs->audio_hw_buf_size) {
								len1 = vs->audio_hw_buf_size;
							}

							memcpy(
									vs->audio_main_buf[1],
									vs->audio_buf + vs->audio_hw_buf_size,
									len1);

							vs->audio_main_buf_size[1] = len1;
						}

					} else {
						if (vs->audio_main_buf_size[0] + audio_size >
								vs->audio_hw_buf_size) {

							len1 = (vs->audio_main_buf_size[0] + audio_size)
									- vs->audio_hw_buf_size;

							offset = vs->audio_hw_buf_size -
									vs->audio_main_buf_size[0];

							memcpy(
									vs->audio_main_buf[0] +
											vs->audio_main_buf_size[0],
									vs->audio_buf,
									offset);

							vs->audio_main_buf_size[0] = vs->audio_hw_buf_size;

							if (len1 > vs->audio_hw_buf_size) {
								len1 = vs->audio_hw_buf_size;
							}

							memcpy(
									vs->audio_main_buf[1],
									vs->audio_buf + offset,
									len1);

							vs->audio_main_buf_size[1] = len1;

						} else {
							memcpy(
									vs->audio_main_buf[0] +
											vs->audio_main_buf_size[0],
									vs->audio_buf,
									audio_size);

							vs->audio_main_buf_size[0] = (vs->
									audio_main_buf_size[0] + audio_size);
						}
					}

				} else if (audio_size <= 0) {
					clog << "SDL2ffmpeg::audio_refresh_decoder exception";
					clog << endl;
				}
			}
		}

		return 1;
	}

	/* open a given stream. Return 0 if OK */
	int SDL2ffmpeg::stream_component_open(int stream_index) {
		AVFormatContext *ic = vs->ic;
		AVCodecContext *avctx;
		AVCodec *codec;
		AVDictionaryEntry *t = NULL;
		int64_t wanted_channel_layout = 0;
		int wanted_nb_channels;
		const char *env;

		if (stream_index < 0 || stream_index >= ic->nb_streams) {
			clog << "SDL2ffmpeg::stream_component_open Warning! Invalid ";
			clog << "index '" << stream_index << "'" << endl;
			return -1;
		}

		avctx = ic->streams[stream_index]->codec;

		codec = avcodec_find_decoder(avctx->codec_id);
		if (!codec) {
			clog << "SDL2ffmpeg::stream_component_open Warning! Can't ";
			clog << " find codec with id '" << avctx->codec_id << "'" << endl;
			return -1;
		}

		avctx->workaround_bugs   = workaround_ffmpeg_bugs;
		avctx->lowres            = lowres;

		if (avctx->lowres > codec->max_lowres) {
			avctx->lowres = codec->max_lowres;
		}

		avctx->idct_algo         = idct;
		avctx->skip_frame        = skip_frame;
		avctx->skip_idct         = skip_idct;
		avctx->skip_loop_filter  = skip_loop_filter;
		avctx->error_concealment = error_concealment;

		if (avctx->lowres || (codec->capabilities & CODEC_CAP_DR1)) {
			avctx->flags |= CODEC_FLAG_EMU_EDGE;
		}

		if (fast) {
			avctx->flags2 |= CODEC_FLAG2_FAST;
		}

		if (avctx->codec_type == AVMEDIA_TYPE_AUDIO) {
			memset(&vs->audio_pkt_temp, 0, sizeof(vs->audio_pkt_temp));
			env = SDL_getenv("SDL_AUDIO_CHANNELS");
			if (env) {
				wanted_channel_layout = av_get_default_channel_layout(
						SDL_atoi(env));
			}

			if (!wanted_channel_layout) {
				wanted_channel_layout = (avctx->channel_layout &&
						avctx->channels == av_get_channel_layout_nb_channels(
								avctx->channel_layout)) ? avctx->channel_layout
										: av_get_default_channel_layout(
												avctx->channels);

				wanted_channel_layout &= ~AV_CH_LAYOUT_STEREO_DOWNMIX;
				wanted_nb_channels = av_get_channel_layout_nb_channels(
						wanted_channel_layout);

				/*
				 * SDL only supports 1, 2, 4 or 6 channels at the moment,
				 * so we have to make sure not to request anything else.
				 */
				while (wanted_nb_channels > 0 && (wanted_nb_channels == 3 ||
						wanted_nb_channels == 5 || wanted_nb_channels > 6)) {

					wanted_nb_channels--;
					wanted_channel_layout = av_get_default_channel_layout(
							wanted_nb_channels);
				}
			}

			wantedSpec.channels = av_get_channel_layout_nb_channels(
					wanted_channel_layout);

			wantedSpec.freq = avctx->sample_rate;
			if (wantedSpec.freq <= 0 || wantedSpec.channels <= 0) {
				clog << "SDL2ffmpeg::stream_component_open Warning! ";
				clog << "Invalid sample rate or channel count!" << endl;
				return -1;
			}
		}

		if (!codec || avcodec_open2(avctx, codec, NULL) < 0) {
			clog << "SDL2ffmpeg::stream_component_open Warning! ";
			clog << "Can't open codec" << endl;
			return -1;
		}

		/* prepare audio output */
		if (avctx->codec_type == AVMEDIA_TYPE_AUDIO) {
			wantedSpec.format   = AUDIO_S16SYS;
			wantedSpec.silence  = 0;
			wantedSpec.samples  = SDL_AUDIO_BUFFER_SIZE;
			wantedSpec.callback = sdl_audio_callback;
			wantedSpec.userdata = this;
			wantedSpec.size     = 0;

			if (spec.size == 0) {
				if (SDL_OpenAudio(&wantedSpec, &spec) < 0) {
					clog << "SDL2ffmpeg::stream_component_open Warning! ";
					clog << "Can't open audio: ";
					clog << SDL_GetError() << endl;
					return -1;
				}

				clog << "SDL2ffmpeg::stream_component_open " << endl;
				clog << "Desired format = '" << wantedSpec.format;
				clog << "'" << endl;
				clog << "Desired silence = '" << wantedSpec.silence;
				clog << "'" << endl;
				clog << "Desired samples = '" << wantedSpec.samples;
				clog << "'" << endl;
				clog << "Desired size = '" << wantedSpec.size;
				clog << "'" << endl;
				clog << "Desired channels = '" << (int)wantedSpec.channels;
				clog << "'" << endl;
				clog << "Desired frequency = '" << wantedSpec.freq;
				clog << "'" << endl;
				clog << endl;
				clog << "Obtained format = '" << spec.format;
				clog << "'" << endl;
				clog << "Obtained silence = '" << spec.silence;
				clog << "'" << endl;
				clog << "Obtained samples = '" << spec.samples;
				clog << "'" << endl;
				clog << "Obtained size = '" << spec.size;
				clog << "'" << endl;
				clog << "Obtained channels = '" << (int)spec.channels;
				clog << "'" << endl;
				clog << "Obtained frequency = '" << spec.freq;
				clog << "'" << endl;

				if (spec.size == 0) {
					spec.size = wantedSpec.channels * wantedSpec.samples * 2;

				} else {
					wantedSpec.size = spec.size;
				}

			} else {

				wantedSpec.samples = spec.samples;

				clog << "SDL2ffmpeg::stream_component_open (2nd audio src)";
				clog << endl;
				clog << "Desired format = '" << wantedSpec.format;
				clog << "'" << endl;
				clog << "Desired silence = '" << wantedSpec.silence;
				clog << "'" << endl;
				clog << "Desired samples = '" << wantedSpec.samples;
				clog << "'" << endl;
				clog << "Desired size = '" << wantedSpec.size;
				clog << "'" << endl;
				clog << "Desired channels = '" << (int)wantedSpec.channels;
				clog << "'" << endl;
				clog << "Desired frequency = '" << wantedSpec.freq;
				clog << "'" << endl;
				clog << endl;
			}

			vs->audio_main_buf_size[0] = 0;
			vs->audio_main_buf_size[1] = 0;

			if (wantedSpec.size != 0) {
				vs->audio_hw_buf_size = wantedSpec.size;

			} else {
				vs->audio_hw_buf_size = wantedSpec.channels *
						wantedSpec.samples * 2;
			}

			clog << "SDL2ffmpeg::stream_component_open buffer size = '";
			clog << vs->audio_hw_buf_size << "'" << endl;

			if (spec.format != AUDIO_S16SYS) {
				clog << "SDL2ffmpeg::stream_component_open Warning! ";
				clog << "SDL advised audio format '" << spec.format << "'";
				clog << " is not supported!";

				return -1;
			}

			if (spec.channels != wantedSpec.channels) {
				wanted_channel_layout = av_get_default_channel_layout(
						spec.channels);

				if (!wanted_channel_layout) {
					clog << "SDL2ffmpeg::stream_component_open Warning! ";
					clog << "SDL advised channel count '" << spec.channels;
					clog << "' is not supported!" << endl;

					return -1;
				}
			}

			vs->audio_src_fmt = vs->audio_tgt_fmt = AV_SAMPLE_FMT_S16;
			vs->audio_src_freq = vs->audio_tgt_freq = spec.freq;
			vs->audio_src_channel_layout = vs->audio_tgt_channel_layout =
					wanted_channel_layout;

			vs->audio_src_channels = vs->audio_tgt_channels = spec.channels;
		}

		ic->streams[stream_index]->discard = AVDISCARD_DEFAULT;
		switch (avctx->codec_type) {
			case AVMEDIA_TYPE_AUDIO:
				vs->audio_stream = stream_index;
				vs->audio_st = ic->streams[stream_index];
				vs->audio_buf_size  = 0;
				vs->audio_buf_index = 0;

				/* init averaging filter */
				vs->audio_diff_avg_coef  = exp(log(0.01) / AUDIO_DIFF_AVG_NB);
				vs->audio_diff_avg_count = 0;
				/* since we do not have a precise anough audio fifo fullness,
				   we correct audio sync only if larger than this threshold */
				vs->audio_diff_threshold = 2.0 *
						SDL_AUDIO_BUFFER_SIZE / wantedSpec.freq;

				memset(&vs->audio_pkt, 0, sizeof(vs->audio_pkt));
				packet_queue_start(&vs->audioq);
				break;

			case AVMEDIA_TYPE_VIDEO:
				vs->video_stream = stream_index;
				vs->video_st = ic->streams[stream_index];

				packet_queue_start(&vs->videoq);
				break;

			case AVMEDIA_TYPE_SUBTITLE:
				vs->subtitle_stream = stream_index;
				vs->subtitle_st = ic->streams[stream_index];
				packet_queue_start(&vs->subtitleq);
				break;

			default:
				break;
		}

		return 0;
	}

	void SDL2ffmpeg::stream_component_close(int stream_index) {
		AVFormatContext *ic = vs->ic;
		AVCodecContext *avctx;

		if (stream_index < 0 || stream_index >= ic->nb_streams) {
			return;
		}

		avctx = ic->streams[stream_index]->codec;

		switch (avctx->codec_type) {
			case AVMEDIA_TYPE_AUDIO:
				packet_queue_abort(&vs->audioq);
				packet_queue_flush(&vs->audioq);
				av_free_packet(&vs->audio_pkt);
				if (vs->swr_ctx) {
					swr_free(&vs->swr_ctx);
				}

				av_freep(&vs->audio_buf1);
				vs->audio_buf = NULL;
				av_freep(&vs->frame);

				if (vs->rdft) {
					av_rdft_end(vs->rdft);
					av_freep(&vs->rdft_data);
					vs->rdft = NULL;
					vs->rdft_bits = 0;
				}
				break;

			case AVMEDIA_TYPE_VIDEO:
				packet_queue_abort(&vs->videoq);

				/*
				 * Note: we also signal this mutex to make sure we unblock the
				 * video thread in all cases
				 */
				SDL_LockMutex(vs->pictq_mutex);
				SDL_CondSignal(vs->pictq_cond);
				SDL_UnlockMutex(vs->pictq_mutex);

				SDL_WaitThread(vs->video_tid, NULL);

				packet_queue_flush(&vs->videoq);
				break;

			case AVMEDIA_TYPE_SUBTITLE:
				packet_queue_abort(&vs->subtitleq);

				/* note: we also signal this mutex to make sure we deblock the
				   video thread in all cases */
				SDL_LockMutex(vs->subpq_mutex);
				vs->subtitle_stream_changed = 1;

				SDL_CondSignal(vs->subpq_cond);
				SDL_UnlockMutex(vs->subpq_mutex);

				SDL_WaitThread(vs->subtitle_tid, NULL);

				packet_queue_flush(&vs->subtitleq);
				break;

			default:
				break;
		}

		ic->streams[stream_index]->discard = AVDISCARD_ALL;
		avcodec_close(avctx);
		switch (avctx->codec_type) {
			case AVMEDIA_TYPE_AUDIO:
				vs->audio_st = NULL;
				vs->audio_stream = -1;
				break;

			case AVMEDIA_TYPE_VIDEO:
				vs->video_st = NULL;
				vs->video_stream = -1;
				break;

			case AVMEDIA_TYPE_SUBTITLE:
				vs->subtitle_st = NULL;
				vs->subtitle_stream = -1;
				break;

			default:
				break;
		}
	}

	int SDL2ffmpeg::decode_interrupt_cb(void *ctx) {
		SDL2ffmpeg* dec = (SDL2ffmpeg*)ctx;
	    return dec->abortRequest;
	}

	int SDL2ffmpeg::read_init() {
		int err, i, ret = 0;

		vs->video_stream    = -1;
		vs->audio_stream    = -1;
		vs->subtitle_stream = -1;

		vs->ic = avformat_alloc_context();
		vs->ic->interrupt_callback.callback = SDL2ffmpeg::decode_interrupt_cb;
		vs->ic->interrupt_callback.opaque = this;

		err = avformat_open_input(&vs->ic, vs->filename, NULL, NULL);
		if (err < 0) {
			clog << "SDL2ffmpeg::read_init Warning! Problems with '";
			clog << vs->filename << "': '";
			clog << ffmpegErr(err) << "'" << endl;

			close(true);
			return -1;
		}

		if (genpts) {
			vs->ic->flags |= AVFMT_FLAG_GENPTS;
		}

		clog << "SDL2ffmpeg::read_init context flags = '" << vs->ic->flags;
		clog << "'" << endl;

		err = avformat_find_stream_info(vs->ic, NULL);
		if (err < 0) {
			clog << "SDL2ffmpeg::read_init Warning! Can't find stream info ";
			clog << "for '" << vs->filename << "': '";
			clog << ffmpegErr(err) << "'" << endl;

			close(true);
			return -1;
		}

		if (vs->ic->pb) {
			/*
			 *  FIXME hack, should not use url_feof() to test for the end
			 */
			vs->ic->pb->eof_reached = 0;
		}

		if (seek_by_bytes < 0) {
			seek_by_bytes = !!(vs->ic->iformat->flags & AVFMT_TS_DISCONT);
		}

		for (i = 0; i < vs->ic->nb_streams; i++) {
			vs->ic->streams[i]->discard = AVDISCARD_ALL;
		}

		return ret;
	}

	int SDL2ffmpeg::read_thread(void *arg) {
		SDL2ffmpeg* dec = (SDL2ffmpeg*)arg;
		VideoState* vs  = dec->vs;

		int err, i, ret;
		AVPacket pkt1, *pkt = &pkt1;
		int eof = 0;
		int pkt_in_play_range = 0;

		if (vs->video_stream < 0 && vs->audio_stream < 0) {
			clog << "SDL2ffmpeg::read_thread exiting: no streams in '";
			clog << vs->filename << "'";
			clog << endl;

			dec->close(true);
			return -1;
		}

		while (!dec->abortRequest) {
			if (vs->paused != vs->last_paused) {
				vs->last_paused = vs->paused;
				if (vs->paused) {
					vs->read_pause_return = av_read_pause(vs->ic);
				} else {
					av_read_play(vs->ic);
				}
			}

			if (vs->paused &&
					(!strcmp(vs->ic->iformat->name, "rtsp") ||
							(vs->ic->pb &&
									!strncmp(vs->filename, "mmsh:", 5)))) {

				/* wait 10 ms to avoid trying to get another packet */
				/* XXX: horrible */
				SDL_Delay(10);
				continue;
			}

			if (vs->seek_req) {
				int64_t seek_target = vs->seek_pos;
				int64_t seek_min    = vs->seek_rel > 0 ?
						seek_target - vs->seek_rel + 2: INT64_MIN;

				int64_t seek_max    = vs->seek_rel < 0 ?
						seek_target - vs->seek_rel - 2: INT64_MAX;

				/*
				 * FIXME the +-2 is due to rounding being not done in the
				 * correct direction in generation of the seek_pos/seek_rel
				 * variables
				 */

				ret = avformat_seek_file(
						vs->ic,
						-1,
						seek_min,
						seek_target,
						seek_max,
						vs->seek_flags);

				if (ret < 0) {
					fprintf(
							stderr,
							"%s: error while seeking\n",
							vs->ic->filename);

				} else {
					if (vs->audio_stream >= 0) {
						dec->packet_queue_flush(&vs->audioq);
						dec->packet_queue_put(&vs->audioq, &dec->flush_pkt);
					}

					if (vs->subtitle_stream >= 0) {
						dec->packet_queue_flush(&vs->subtitleq);
						dec->packet_queue_put(&vs->subtitleq, &dec->flush_pkt);
					}

					if (vs->video_stream >= 0) {
						dec->packet_queue_flush(&vs->videoq);
						dec->packet_queue_put(&vs->videoq, &dec->flush_pkt);
					}
				}
				vs->seek_req = 0;
				eof = 0;
			}

			/* if the queue are full, no need to read more */
			if (vs->audioq.size + vs->videoq.size + vs->subtitleq.size >
				MAX_QUEUE_SIZE ||
				((vs->audioq.nb_packets > MIN_FRAMES ||
						vs->audio_stream < 0 ||
						vs->audioq.abort_request) &&
						(vs->videoq.nb_packets > MIN_FRAMES ||
								vs->video_stream < 0 ||
								vs->videoq.abort_request) &&
						(vs->subtitleq.nb_packets > MIN_FRAMES ||
								vs->subtitle_stream < 0 ||
								vs->subtitleq.abort_request))) {

				/* wait 10 ms */
				SDL_Delay(10);
				continue;
			}

			if (eof) {
				if (vs->video_stream >= 0) {
					av_init_packet(pkt);
					pkt->data = NULL;
					pkt->size = 0;
					pkt->stream_index = vs->video_stream;
					dec->packet_queue_put(&vs->videoq, pkt);
				}

				if (vs->audio_stream >= 0 &&
						(vs->audio_st->codec->codec->capabilities &
								CODEC_CAP_DELAY)) {

					av_init_packet(pkt);
					pkt->data = NULL;
					pkt->size = 0;
					pkt->stream_index = vs->audio_stream;
					dec->packet_queue_put(&vs->audioq, pkt);
				}

				SDL_Delay(10);
				if (vs->audioq.size + vs->videoq.size +
						vs->subtitleq.size == 0) {

					dec->status = ST_STOPPED;
					clog << "SDL2ffmpeg::read_thread(" << vs->filename;
					clog << ") all done (EOF)" << endl;
					return AVERROR_EOF;
				}

				eof=0;
				continue;
			}

			ret = av_read_frame(vs->ic, pkt);
			if (ret < 0) {
				if (ret == AVERROR_EOF || url_feof(vs->ic->pb)) {
					eof = 1;
				}

				if (vs->ic->pb && vs->ic->pb->error) {
					break;
				}

				SDL_Delay(100); /* wait for user event */
				continue;
			}

			/*
			 * check if packet is in play range specified by user, then queue,
			 * otherwise discard
			 */

			pkt_in_play_range = dec->duration == AV_NOPTS_VALUE ||
					(pkt->pts - vs->ic->streams[pkt->stream_index]->start_time) *
					av_q2d(vs->ic->streams[pkt->stream_index]->time_base) -
					(double)(dec->start_time != AV_NOPTS_VALUE ?
							dec->start_time : 0) / 1000000
					<= ((double)dec->duration / 1000000);

			if (pkt->stream_index == vs->audio_stream && pkt_in_play_range) {
				dec->packet_queue_put(&vs->audioq, pkt);

			} else if (pkt->stream_index == vs->video_stream &&
					pkt_in_play_range) {

				dec->packet_queue_put(&vs->videoq, pkt);

			} else if (pkt->stream_index == vs->subtitle_stream &&
					pkt_in_play_range) {

				dec->packet_queue_put(&vs->subtitleq, pkt);

			} else {
				av_free_packet(pkt);
			}
		}

		clog << "SDL2ffmpeg::read_thread(" << vs->filename;
		clog << ") all done" << endl;
		return 0;
	}

	void SDL2ffmpeg::stream_cycle_channel(int codec_type) {
		AVFormatContext *ic = vs->ic;
		int start_index, stream_index;
		AVStream *st;

		if (codec_type == AVMEDIA_TYPE_VIDEO) {
			start_index = vs->video_stream;

		} else if (codec_type == AVMEDIA_TYPE_AUDIO) {
			start_index = vs->audio_stream;

		} else {
			start_index = vs->subtitle_stream;
		}

		if (start_index < (codec_type == AVMEDIA_TYPE_SUBTITLE ? -1 : 0)) {
			return;
		}

		stream_index = start_index;
		for (;;) {
			if (++stream_index >= vs->ic->nb_streams) {
				if (codec_type == AVMEDIA_TYPE_SUBTITLE) {
					stream_index = -1;
					goto the_end;

				} else {
					stream_index = 0;
				}
			}

			if (stream_index == start_index) {
				return;
			}

			st = ic->streams[stream_index];
			if (st->codec->codec_type == codec_type) {
				/* check that parameters are OK */
				switch (codec_type) {
					case AVMEDIA_TYPE_AUDIO:
						if (st->codec->sample_rate != 0 &&
								st->codec->channels != 0)
							goto the_end;
						break;

					case AVMEDIA_TYPE_VIDEO:
					case AVMEDIA_TYPE_SUBTITLE:
						goto the_end;

					default:
						break;
				}
			}
		}
the_end:
		stream_component_close(start_index);
		stream_component_open(stream_index);
	}

	void SDL2ffmpeg::toggle_pause() {
		stream_toggle_pause();
		vs->step = 0;
	}

	void SDL2ffmpeg::step_to_next_frame() {
		/* if the stream is paused unpause it, then step */
		if (vs->paused) {
			stream_toggle_pause();
		}
		vs->step = 1;
	}

	int SDL2ffmpeg::lockmgr(void **mtx, enum AVLockOp op) {
		switch(op) {
			case AV_LOCK_CREATE:
				*mtx = SDL_CreateMutex();
				if (!*mtx) {
					return 1;
				}
				return 0;

			case AV_LOCK_OBTAIN:
				return !!SDL_LockMutex((SDL_mutex*)*mtx);

			case AV_LOCK_RELEASE:
				return !!SDL_UnlockMutex((SDL_mutex*)*mtx);

			case AV_LOCK_DESTROY:
				SDL_UnlockMutex((SDL_mutex*)*mtx);
				SDL_DestroyMutex((SDL_mutex*)*mtx);
				return 0;
		}
		return 1;
	}
}
}
}
}
}
}
