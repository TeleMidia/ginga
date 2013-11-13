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

		this->cmp                            = cmp;
		wanted_stream[AVMEDIA_TYPE_AUDIO]    = -1;
		wanted_stream[AVMEDIA_TYPE_VIDEO]    = -1;
		seek_by_bytes                        = -1;
		av_sync_type                         = AV_SYNC_AUDIO_MASTER;
		start_time                           = AV_NOPTS_VALUE;
		duration                             = AV_NOPTS_VALUE;
		workaround_ffmpeg_bugs               = 1;
		fast                                 = 0;
		genpts                               = 0;
		lowres                               = 0;
		error_concealment                    = 3;
		decoder_reorder_pts                  = -1;
		framedrop                            = -1;
		infinite_buffer                      = -1;
		rdftspeed                            = 0.02;
		vfilters                             = NULL;
		afilters                             = NULL;
		texture                              = NULL;
		hasPic                               = false;
		reof                                 = false;
		hasSDLAudio                          = false;
		abortRequest                         = false;
		monoStep                             = 0;
		status                               = ST_STOPPED;

		setSoundLevel(1.0);

		if (!init) {
			init = true;

			memset(&spec, 0, sizeof(spec));
			Thread::mutexInit(&aiMutex);

			av_log_set_flags(AV_LOG_QUIET);
			av_log_set_level(0);

			avcodec_register_all();
		    av_register_all();
		    avformat_network_init();
		    avdevice_register_all();
			avfilter_register_all();

		    av_lockmgr_register(SDL2ffmpeg::lockmgr);
		}

		memset(&vPkt, 0, sizeof(vPkt));
		memset(&flush_pkt, 0, sizeof(flush_pkt));

		vs = (VideoState*)av_mallocz(sizeof(VideoState));
		if (vs) {
			av_strlcpy(vs->filename, filename, sizeof(vs->filename));

			av_init_packet(&flush_pkt);
		    flush_pkt.data = (uint8_t*)(intptr_t)"FLUSH";

		    if (read_init() >=0) {

		    	SDLDeviceScreen::lockSDL();

				/* start video display */
				vs->pictq_mutex = SDL_CreateMutex();
				vs->pictq_cond  = SDL_CreateCond();

				SDLDeviceScreen::unlockSDL();

				packet_queue_init(&vs->videoq);
				packet_queue_init(&vs->audioq);

				vs->continue_read_thread = SDL_CreateCond();

				init_clock(&vs->vidclk, &vs->videoq.serial);
				init_clock(&vs->audclk, &vs->audioq.serial);
				init_clock(&vs->extclk, &vs->extclk.serial);
				vs->audio_clock_serial = -1;
				vs->audio_last_serial = -1;

				vs->av_sync_type = av_sync_type;

				Thread::mutexLock(&aiMutex);
				openStreams();
				Thread::mutexUnlock(&aiMutex);
		    }
		}

	    refCount++;
	}

	SDL2ffmpeg::~SDL2ffmpeg() {
		set<SDL2ffmpeg*>::iterator i;

		clog << "SDL2ffmpeg::~SDL2ffmpeg" << endl;

		abortRequest = true;
		hasPic = false;

		Thread::mutexLock(&aiMutex);
		i = aInstances.find(this);
		if (i != aInstances.end()) {
			aInstances.erase(i);
		}
		Thread::mutexUnlock(&aiMutex);

		release();

		SDLDeviceScreen::releaseTexture(texture);
		texture = NULL;

		refCount--;
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

		//AVFILTER
		av_freep(&vfilters);
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

		/* open the streams */
		if (st_index[AVMEDIA_TYPE_AUDIO] >= 0) {
			stream_component_open(st_index[AVMEDIA_TYPE_AUDIO]);

		} else {
			clog << "SDL2ffmpeg::openStreams '";
			clog << vs->filename << "' doesn't have any audio stream!";
			clog << endl;
		}

		if (st_index[AVMEDIA_TYPE_VIDEO] >= 0) {
			stream_component_open(st_index[AVMEDIA_TYPE_VIDEO]);

		} else {
			clog << "SDL2ffmpeg::openStreams '";
			clog << vs->filename << "' doesn't have any video stream!";
			clog << endl;
		}
	}

	bool SDL2ffmpeg::prepare() {
		bool wasEmpty = false;

		if (vs->audio_stream >= 0 && vs->audio_st != NULL) {
			Thread::mutexLock(&aiMutex);
			wasEmpty = aInstances.empty();
			aInstances.insert(this);
			Thread::mutexUnlock(&aiMutex);

			if (vs->audio_hw_buf_size == 0) {
				clog << "SDL2ffmpeg::prepare Warning! buffer size = 0" << endl;
			}

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

			SDLDeviceScreen::lockSDL();
			vs->video_tid = SDL_CreateThread(
					SDL2ffmpeg::video_thread, "video_thread", this);

			SDLDeviceScreen::unlockSDL();

			if (!vs->video_tid) {
				clog << "SDL2ffmpeg::prepare Warning! ";
				clog << "Can't create video thread" << endl;
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

		SDLDeviceScreen::lockSDL();
		vs->read_tid = SDL_CreateThread(
				SDL2ffmpeg::read_thread, "read_thread", this);

		SDLDeviceScreen::unlockSDL();

		if (!vs->read_tid) {
			av_free(vs);

			clog << "SDL2ffmpeg::prepare Warning! ";
			clog << "Can't create read thread" << endl;

			return false;
		}

		SDLDeviceScreen::lockSDL();
		if ((unsigned int)wantedSpec.channels > 0 &&
				(unsigned int)wantedSpec.channels < 8 &&
				(unsigned int)spec.channels > 0 &&
				(unsigned int)spec.channels < 8) {

			SDL_BuildAudioCVT(
					&acvt,
					wantedSpec.format, wantedSpec.channels, wantedSpec.freq,
					spec.format, spec.channels, spec.freq);
		}

		SDLDeviceScreen::unlockSDL();

		return true;
	}

	bool SDL2ffmpeg::hasVideoStream() {
		if (vs == NULL || vs->video_stream < 0) {
			return false;
		}

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

		soundLevel   = 0;
		abortRequest = true;
		status       = ST_STOPPED;

		if (vs->video_stream >= 0) {
			packet_queue_abort(&vs->videoq);
		}

		if (vs->audio_stream >= 0) {
			packet_queue_abort(&vs->audioq);
		}

		SDL_CondSignal(vs->pictq_cond);

/*		set<SDL2ffmpeg*>::iterator i;
		Thread::mutexLock(&aiMutex);

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
		Thread::mutexUnlock(&aiMutex);*/
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
		/*
		clog << "SDL2ffmpeg::getPosition(" << vs->filename;
		clog << ") master_clock = '";
		clog << get_master_clock() << "' (duration = '" << getDuration();
		clog << "')" << endl;

		clog << "SDL2ffmpeg::getPosition(" << vs->filename;
		clog << ") current_pts = '";
		clog << vs->video_current_pts << "' (duration = '" << getDuration();
		clog << "')" << endl;
		*/
		if (vs->audio_stream >= 0 && vs->audio_st != NULL) {
			position = vs->audclk.pts;

		} else if (vs->video_stream >= 0 && vs->video_st != NULL) {
			position = vs->vidclk.pts;
		}

		if (position < 0.0 || isnan(position)) {
			position = 0.0;
		}

		return position;
	}

	void SDL2ffmpeg::seek(int64_t pos) {
        if (vs->ic->start_time != AV_NOPTS_VALUE) {
        	pos += vs->ic->start_time;
        }

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

		if (sample_rate > 0 && (unsigned int)channels > 0) {
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

	int SDL2ffmpeg::cmp_audio_fmts(
			enum AVSampleFormat fmt1,
			int64_t channel_count1,
			enum AVSampleFormat fmt2, 
			int64_t channel_count2) {

		/* If channel count == 1, planar and non-planar formats are the same */
		if (channel_count1 == 1 && channel_count2 == 1) {
			return av_get_packed_sample_fmt(fmt1) != av_get_packed_sample_fmt(fmt2);
		} else {
			return channel_count1 != channel_count2 || fmt1 != fmt2;
		}
	}

	int64_t SDL2ffmpeg::get_valid_channel_layout(
			int64_t channel_layout, int channels) {

		if (channel_layout && av_get_channel_layout_nb_channels(
				channel_layout) == channels) {

			return channel_layout;
		} else {
			return 0;
		}
	}

	int SDL2ffmpeg::nts_packet_queue_put(PacketQueue *q, AVPacket *pkt) {
		MyAVPacketList* pkt1;

		if (q->abort_request) {
			return -1;
		}

		pkt1 = (MyAVPacketList*)av_malloc(sizeof(MyAVPacketList));
		if (!pkt1) {
			return -1;
		}

		pkt1->pkt  = *pkt;
		pkt1->next = NULL;

		if (pkt == &flush_pkt) {
			q->serial++;
		}
		pkt1->serial = q->serial;

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

		SDLDeviceScreen::lockSDL();
		q->mutex = SDL_CreateMutex();
		q->cond  = SDL_CreateCond();
		SDLDeviceScreen::unlockSDL();

		q->abort_request = 1;
	}

	void SDL2ffmpeg::packet_queue_flush(PacketQueue *q) {
		MyAVPacketList* pkt;
		MyAVPacketList* pkt1;

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

		SDLDeviceScreen::lockSDL();
		SDL_DestroyMutex(q->mutex);
		SDL_DestroyCond(q->cond);
		SDLDeviceScreen::unlockSDL();
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
	int SDL2ffmpeg::packet_queue_get(
			PacketQueue *q, AVPacket *pkt, int block, int* serial) {

		MyAVPacketList *pkt1;
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
				if (serial) {
					*serial = pkt1->serial;
				}
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

	void SDL2ffmpeg::video_image_display() {
		VideoPicture *vp;

		vp = &vs->pictq[vs->pictq_rindex];
		if (vp->tex) {
			if (vp->src_frame && !abortRequest) {
		        Uint32 format;
		        int textureAccess, w, h;
		        bool locked = true;
				AVPicture pict = { { 0 } };

		        if (SDL_LockTexture(vp->tex, NULL, (void**)&pict.data, &pict.linesize[0]) != 0) {
					clog << "SDL2ffmpeg::video_image_display(" << vs->filename;
					clog << ") Warning! ";
					clog << "Can't lock texture: " << SDL_GetError() << endl;
					if (!SDLDeviceScreen::hasTexture(vp->tex)) {
						clog << "SDL2ffmpeg::video_image_display(";
						clog << vs->filename << ") Warning! ";
						clog << "vp->tex(" << vp->tex << ") ";
						clog << "is out of scope " << endl;
						return;
					}
					locked = false;
		        }

		        SDL_QueryTexture(vp->tex, &format, &textureAccess, &w, &h);

				if (!abortRequest) {
					if (vp->tex &&
							vp->src_frame->data &&
							vp->height > 0) {

						AVPixelFormat fmt = (AVPixelFormat)vp->src_frame->format;

						if (fmt == PIX_FMT_NONE) {
							fmt = PIX_FMT_YUV420P;
						}

						//FIXME: use direct rendering
						/*av_picture_copy(
								&pict,
								(AVPicture*)vp->src_frame,
								(AVPixelFormat)fmt,
								vp->width,
								vp->height);*/

						/*
						 * FIXME: we are using filters only to deinterlace video.
						 *        we should use them to convert pixel formats as well.
						 */

						SwsContext* ctx = NULL;

						ctx = sws_getCachedContext(
								ctx,
								vp->width,
								vp->height,
								(AVPixelFormat)fmt,
								w,
								h,
								PIX_FMT_RGB24,
								SWS_BILINEAR,
								0,
								0,
								0);

						sws_scale(
								ctx,
								(const uint8_t* const*)vp->src_frame->data,
								vp->src_frame->linesize,
								0, vp->height, pict.data, pict.linesize);
					}
				}

				if (locked) {
					SDL_UnlockTexture(vp->tex);
				}

				hasPic = false;

			} else {
				clog << "SDL2ffmpeg::video_image_display aborting";
				clog << endl;
			}

		} else {
			clog << "SDL2ffmpeg::video_image_display Can't display video ";
			clog << "(NULL texture)" << endl;
		}

		av_frame_unref(vp->src_frame);
	}

	void SDL2ffmpeg::stream_close() {
		VideoPicture *vp;
		int i;

		/* XXX: use a special url_shutdown call to abort parse cleanly */
		abortRequest = 1;
		SDL_WaitThread(vs->read_tid, NULL);

	    packet_queue_destroy(&vs->videoq);
	    packet_queue_destroy(&vs->audioq);

		/* free all pictures */
		for (i = 0; i < VIDEO_PICTURE_QUEUE_SIZE; i++) {
			vp = &vs->pictq[i];
			if (vp->tex) {
				vp->tex = NULL;
			}
		}

		SDLDeviceScreen::lockSDL();
		SDL_DestroyMutex(vs->pictq_mutex);
		SDL_DestroyCond(vs->pictq_cond);
		SDL_DestroyCond(vs->continue_read_thread);
		SDLDeviceScreen::unlockSDL();

		av_free(vs);

		clog << "SDL2ffmpeg::stream_close all done" << endl;
	}

	/* display the current picture, if any */
	void SDL2ffmpeg::video_display() {
		if (vs->video_st) {
			video_image_display();
		}
	}

	double SDL2ffmpeg::get_clock(Clock* c) {
		if (*c->queue_serial != c->serial) {
			return NAN;
		}

		if (c->paused) {
			return c->pts;
		} else {
			double time = av_gettime() / 1000000.0;
			return c->pts_drift + time - (time - c->last_updated) * (1.0 - c->speed);
		}
	}

	void SDL2ffmpeg::set_clock_at(Clock* c, double pts, int serial, double time) {
		c->pts = pts;
		c->last_updated = time;
		c->pts_drift = c->pts - time;
		c->serial = serial;
	}

	void SDL2ffmpeg::set_clock(Clock* c, double pts, int serial) {
		double time = av_gettime() / 1000000.0;
		set_clock_at(c, pts, serial, time);
	}

	void SDL2ffmpeg::set_clock_speed(Clock *c, double speed) {
		set_clock(c, get_clock(c), c->serial);
		c->speed = speed;
	}

	void SDL2ffmpeg::init_clock(Clock *c, int *queue_serial) {
		c->speed = 1.0;
		c->paused = 0;
		c->queue_serial = queue_serial;
		set_clock(c, NAN, -1);
	}

	void SDL2ffmpeg::sync_clock_to_slave(Clock* c, Clock* slave) {
		double clock = get_clock(c);
		double slave_clock = get_clock(slave);

		if (!isnan(slave_clock) && 
				(isnan(clock) || fabs(clock - slave_clock) > AV_NOSYNC_THRESHOLD)) {

			set_clock(c, slave_clock, slave->serial);
		}
	}

	int SDL2ffmpeg::get_master_sync_type() {
		if (vs->av_sync_type == AV_SYNC_VIDEO_MASTER) {
			if (vs->video_st) {
				return AV_SYNC_VIDEO_MASTER;

			} else {
				return AV_SYNC_AUDIO_MASTER;
			}

		} else if (vs->av_sync_type == AV_SYNC_AUDIO_MASTER) {
			if (vs->audio_st) {
				return AV_SYNC_AUDIO_MASTER;

			} else {
				return AV_SYNC_EXTERNAL_CLOCK;
			}

		} else {
			return AV_SYNC_EXTERNAL_CLOCK;
		}
	}

	/* get the current master clock value */
	double SDL2ffmpeg::get_master_clock() {
		double val;

		switch (get_master_sync_type()) {
			case AV_SYNC_VIDEO_MASTER:
				val = get_clock(&vs->vidclk);
				break;

			case AV_SYNC_AUDIO_MASTER:
				val = get_clock(&vs->audclk);
				break;

			default:
				val = get_clock(&vs->extclk);
				break;
		}

		return val;
	}

	void SDL2ffmpeg::check_external_clock_speed() {
		if (vs->video_stream >= 0 && vs->videoq.nb_packets <= MIN_FRAMES / 2 ||
				vs->audio_stream >= 0 && vs->audioq.nb_packets <= MIN_FRAMES / 2) {

			set_clock_speed(
					&vs->extclk, 
					FFMAX(EXTERNAL_CLOCK_SPEED_MIN, vs->extclk.speed - EXTERNAL_CLOCK_SPEED_STEP));

		} else if ((vs->video_stream < 0 || vs->videoq.nb_packets > MIN_FRAMES * 2) &&
				(vs->audio_stream < 0 || vs->audioq.nb_packets > MIN_FRAMES * 2)) {

			set_clock_speed(
					&vs->extclk, 
					FFMIN(EXTERNAL_CLOCK_SPEED_MAX, vs->extclk.speed + EXTERNAL_CLOCK_SPEED_STEP));
		} else {
			double speed = vs->extclk.speed;
			if (speed != 1.0) {
				set_clock_speed(
						&vs->extclk, 
						speed + EXTERNAL_CLOCK_SPEED_STEP * (1.0 - speed) / fabs(1.0 - speed));
			}
		}
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
			SDL_CondSignal(vs->continue_read_thread);
		}
	}

	/* pause or resume the video */
	void SDL2ffmpeg::stream_toggle_pause() {
		if (vs->paused) {
			vs->frame_timer += av_gettime() / 1000000.0 + vs->vidclk.pts_drift - vs->vidclk.pts;
			if (vs->read_pause_return != AVERROR(ENOSYS)) {
				vs->vidclk.paused = 0;
			}
			set_clock(&vs->vidclk, get_clock(&vs->vidclk), vs->vidclk.serial);
		}
		set_clock(&vs->extclk, get_clock(&vs->extclk), vs->extclk.serial);
		vs->paused = vs->audclk.paused = vs->vidclk.paused = vs->extclk.paused = !vs->paused;
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

	double SDL2ffmpeg::compute_target_delay(double delay) {
		double sync_threshold, diff;

		/* update delay to follow master synchronisation source */
		if (get_master_sync_type() != AV_SYNC_VIDEO_MASTER) {
			/* if video is slave, we try to correct big delays by
			duplicating or deleting a frame */
			diff = get_clock(&vs->vidclk) - get_master_clock();

			/* skip or repeat frame. We take into account the
	           delay to compute the threshold. I still don't know
	           if it is the best guess */
			sync_threshold = FFMAX(AV_SYNC_THRESHOLD_MIN, FFMIN(AV_SYNC_THRESHOLD_MAX, delay));
			if (!isnan(diff) && fabs(diff) < vs->max_frame_duration) {
				if (diff <= -sync_threshold) {
					delay = FFMAX(0, delay + diff);
				} else if (diff >= sync_threshold && delay > AV_SYNC_FRAMEDUP_THRESHOLD) {
					delay = delay + diff;
				} else if (diff >= sync_threshold) {
					delay = 2 * delay;
				}
			}
		}

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

	int SDL2ffmpeg::pictq_prev_picture() {
		VideoPicture *prevvp;
		int ret = 0;
		/* update queue size and signal for the previous picture */
		prevvp = &vs->pictq[(vs->pictq_rindex + VIDEO_PICTURE_QUEUE_SIZE - 1) % VIDEO_PICTURE_QUEUE_SIZE];
		if (prevvp->allocated && prevvp->serial == vs->videoq.serial) {
			SDL_LockMutex(vs->pictq_mutex);
			if (vs->pictq_size < VIDEO_PICTURE_QUEUE_SIZE) {
				if (--vs->pictq_rindex == -1) {
					vs->pictq_rindex = VIDEO_PICTURE_QUEUE_SIZE - 1;
				}
				vs->pictq_size++;
				ret = 1;
			}
			SDL_CondSignal(vs->pictq_cond);
			SDL_UnlockMutex(vs->pictq_mutex);
		}
		return ret;
	}

	void SDL2ffmpeg::update_video_pts(double pts, int64_t pos, int serial) {
		/* update current video pts */
		set_clock(&vs->vidclk, pts, serial);
		sync_clock_to_slave(&vs->extclk, &vs->vidclk);
		vs->video_current_pos = pos;
		vs->frame_last_pts = pts;
	}

	/* called to display each frame */
	void SDL2ffmpeg::video_refresh(void* opaque, double* remaining_time) {
		SDL2ffmpeg* dec = (SDL2ffmpeg*)opaque;
		VideoState* vs  = dec->vs;
		VideoPicture *vp;
		double time;

		assert(dec != NULL);

		if (dec->abortRequest) {
			return;
		}

		if (!vs->paused && 
				dec->get_master_sync_type() == AV_SYNC_EXTERNAL_CLOCK && 
				vs->realtime) {

			dec->check_external_clock_speed();
		}

		if (vs->video_st == NULL && vs->audio_st != NULL) {
			time = av_gettime() / 1000000.0;
			if (vs->force_refresh || vs->last_vis_time + dec->rdftspeed < time) {
				vs->last_vis_time = time;
			}

			*remaining_time = FFMIN(*remaining_time, vs->last_vis_time + dec->rdftspeed - time);
		}

		if (vs->video_st) {
			int redisplay = 0;
			if (vs->force_refresh) {
				redisplay = dec->pictq_prev_picture();
			}
retry:
			if (vs->pictq_size == 0) {
				SDL_LockMutex(vs->pictq_mutex);
				if (vs->frame_last_dropped_pts != AV_NOPTS_VALUE &&
						vs->frame_last_dropped_pts > vs->frame_last_pts) {

					dec->update_video_pts(
							vs->frame_last_dropped_pts,
							vs->frame_last_dropped_pos,
							vs->frame_last_dropped_serial);

					vs->frame_last_dropped_pts = AV_NOPTS_VALUE;
				}

				SDL_UnlockMutex(vs->pictq_mutex);
				// nothing to do, no picture to display in the queue

			} else {
				double last_duration, duration, delay;
				/* dequeue the picture */
				vp = &vs->pictq[vs->pictq_rindex];

				if (vp->serial != vs->videoq.serial) {
					dec->pictq_next_picture();
					redisplay = 0;
					goto retry;
				}

				if (vs->paused) {
					goto display;
				}

				/* compute nominal last_duration */
				last_duration = vp->pts - vs->frame_last_pts;
				if (!isnan(last_duration) && last_duration > 0 && last_duration < vs->max_frame_duration) {
					/*
					 * if duration of the last frame was sane,
					 * update last_duration in video state
					 */
					vs->frame_last_duration = last_duration;
				}

				if (redisplay) {
					delay = 0.0;

				} else {
					delay = dec->compute_target_delay(vs->frame_last_duration);
				}

				time= av_gettime()/1000000.0;
				if (time < vs->frame_timer + delay && !redisplay) {
					*remaining_time = FFMIN(
							vs->frame_timer + delay - time,
							*remaining_time);
					return;
				}

				vs->frame_timer += delay;
				if (delay > 0 && time - vs->frame_timer > AV_SYNC_THRESHOLD_MAX) {
					vs->frame_timer = time;
				}

				SDL_LockMutex(vs->pictq_mutex);
				if (!redisplay && !isnan(vp->pts)) {
					dec->update_video_pts(vp->pts, vp->pos, vp->serial);
				}
				SDL_UnlockMutex(vs->pictq_mutex);

				if (vs->pictq_size > 1) {
					VideoPicture *nextvp = &vs->pictq[(vs->pictq_rindex + 1) % VIDEO_PICTURE_QUEUE_SIZE];
					duration = nextvp->pts - vp->pts;
					if (!vs->step &&
							(redisplay || 
							dec->framedrop > 0 ||
							(dec->framedrop && dec->get_master_sync_type() != AV_SYNC_VIDEO_MASTER)) && 
							time > vs->frame_timer + duration) {

						if (!redisplay) {
							vs->frame_drops_late++;
						}

						dec->pictq_next_picture();
						redisplay = 0;
						goto retry;
					}
				}

display:
				/* display picture */
				dec->video_display();

				dec->pictq_next_picture();

				if (vs->step && !vs->paused) {
					dec->stream_toggle_pause();
				}
			}
		}

		vs->force_refresh = 0;
	}


	/* allocate a picture (needs to do that in main thread to avoid
	 * potential locking problems
	 */
	void SDL2ffmpeg::alloc_picture() {
	    VideoPicture *vp;

	    vp = &vs->pictq[vs->pictq_windex];

	    vp->width   = vs->video_st->codec->width;
	    vp->height  = vs->video_st->codec->height;
	    vp->tex     = texture;

	    SDL_LockMutex(vs->pictq_mutex);
	    vp->allocated = 1;
	    SDL_CondSignal(vs->pictq_cond);
	    SDL_UnlockMutex(vs->pictq_mutex);
	}

	int SDL2ffmpeg::queue_picture(
			AVFrame *src_frame, double pts, int64_t pos, int serial) {

		VideoPicture *vp;

		/* wait until we have space to put a new picture */
		SDL_LockMutex(vs->pictq_mutex);

		/* keep the last already displayed picture in the queue */
		while (vs->pictq_size >= VIDEO_PICTURE_QUEUE_SIZE - 1 &&
				!vs->videoq.abort_request) {

			SDL_CondWait(vs->pictq_cond, vs->pictq_mutex);
		}

		SDL_UnlockMutex(vs->pictq_mutex);

		if (vs->videoq.abort_request) {
			return -1;
		}

		vp = &vs->pictq[vs->pictq_windex];

		vp->sar = src_frame->sample_aspect_ratio;

		/* alloc or resize hardware picture buffer */
		if (!vp->tex || vp->reallocate || !vp->allocated ||
				vp->width != src_frame->width ||
				vp->height != src_frame->height) {

			//SDL_Event event;

			vp->allocated  = 0;
			vp->reallocate = 0;
			vp->width = src_frame->width;
			vp->height = src_frame->height;

			/*
			 * the allocation must be done in the main thread to avoid
			 * locking problems
			 *
			event.type = FF_ALLOC_EVENT;
			event.user.data1 = this;
			event.user.data2 = this->cmp;
			SDL_PushEvent(&event);*/

			alloc_picture();

	        /* wait until the picture is allocated */
			/*SDL_LockMutex(vs->pictq_mutex);
	        while (!vp->allocated && !vs->videoq.abort_request) {
	            SDL_CondWait(vs->pictq_cond, vs->pictq_mutex);
	        }*/

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
			vp->src_frame = src_frame; //tm code
			vp->pts       = pts;
			vp->pos       = pos;
			vp->serial    = serial;

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

	int SDL2ffmpeg::get_video_frame(AVFrame* frame, AVPacket* pkt, int* serial) {
		int got_picture;

		if (packet_queue_get(&vs->videoq, pkt, 1, serial) < 0 ||
				vs->videoq.abort_request) {

			return -1;
		}

		if (pkt->data == flush_pkt.data) {
			avcodec_flush_buffers(vs->video_st->codec);

			SDL_LockMutex(vs->pictq_mutex);
			/*
			 * Make sure there are no long delay timers
			 * (ideally we should just flush the qeue but that is harder)
			 */
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

		if (avcodec_decode_video2(
				vs->video_st->codec, frame, &got_picture, pkt) < 0) {

			return 0;
		}

		if (!got_picture && !pkt->data) {
			vs->video_finished = *serial;
		}

		if (got_picture) {
			int ret = 1;
			double dpts = NAN;

			if (decoder_reorder_pts == -1) {
				frame->pts = av_frame_get_best_effort_timestamp(frame);

			} else if (decoder_reorder_pts) {
				frame->pts = frame->pkt_pts;
			} else {
				frame->pts = frame->pkt_dts;
			}

			if (frame->pts != AV_NOPTS_VALUE) {
				dpts = av_q2d(vs->video_st->time_base) * frame->pts;
			}

			frame->sample_aspect_ratio = av_guess_sample_aspect_ratio(
					vs->ic, vs->video_st, frame);

			if (framedrop > 0 || (framedrop && get_master_sync_type() != AV_SYNC_VIDEO_MASTER)) {
				SDL_LockMutex(vs->pictq_mutex);
				if (vs->frame_last_pts != AV_NOPTS_VALUE && frame->pts != AV_NOPTS_VALUE) {
					double clockdiff = get_clock(&vs->vidclk) - get_master_clock();
					double ptsdiff = dpts - vs->frame_last_pts;
					if (!isnan(clockdiff) && fabs(clockdiff) < AV_NOSYNC_THRESHOLD &&
							!isnan(ptsdiff) && ptsdiff > 0 && ptsdiff < AV_NOSYNC_THRESHOLD &&
							clockdiff + ptsdiff - vs->frame_last_filter_delay < 0 &&
							vs->videoq.nb_packets) {

						vs->frame_last_dropped_pos = pkt->pos;
						vs->frame_last_dropped_pts = dpts;
						vs->frame_last_dropped_serial = *serial;
						vs->frame_drops_early++;

						av_frame_unref(frame);
						ret = 0;
					}
				}

				SDL_UnlockMutex(vs->pictq_mutex);
			}

			return ret;
		}

		return 0;
	}

	// AVFILTER begin
	int SDL2ffmpeg::configure_filtergraph(
			AVFilterGraph *graph, 
			const char *filtergraph, 
			AVFilterContext *source_ctx, 
			AVFilterContext *sink_ctx) {

		int ret;
		AVFilterInOut *outputs = NULL, *inputs = NULL;

		if (filtergraph) {
			outputs = avfilter_inout_alloc();
			inputs  = avfilter_inout_alloc();
			if (!outputs || !inputs) {
				ret = AVERROR(ENOMEM);
				goto fail;
			}

			outputs->name       = av_strdup("in");
			outputs->filter_ctx = source_ctx;
			outputs->pad_idx    = 0;
			outputs->next       = NULL;

			inputs->name        = av_strdup("out");
			inputs->filter_ctx  = sink_ctx;
			inputs->pad_idx     = 0;
			inputs->next        = NULL;

			if ((ret = avfilter_graph_parse_ptr(graph, filtergraph, &inputs, &outputs, NULL)) < 0) {
				goto fail;
			}

		} else {
			if ((ret = avfilter_link(source_ctx, 0, sink_ctx, 0)) < 0) {
				goto fail;
			}
		}

		ret = avfilter_graph_config(graph, NULL);
fail:
		avfilter_inout_free(&outputs);
		avfilter_inout_free(&inputs);
		return ret;
	}

	int SDL2ffmpeg::configure_video_filters(AVFilterGraph *graph, const char *vfilters, AVFrame *frame) {
		static const enum AVPixelFormat pix_fmts[] = { AV_PIX_FMT_YUV420P, AV_PIX_FMT_NONE };
		char sws_flags_str[128];
		char buffersrc_args[256];
		int ret;
		AVFilterContext *filt_src = NULL, *filt_out = NULL, *filt_fmt = NULL, *filt_crop = NULL, *filt_deint = NULL;
		AVCodecContext *codec = vs->video_st->codec;
		AVRational fr = av_guess_frame_rate(vs->ic, vs->video_st, NULL);

		graph->scale_sws_opts = av_strdup(sws_flags_str);

		snprintf(
				buffersrc_args, 
				sizeof(buffersrc_args),
				"video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d",
				frame->width, 
				frame->height, 
				frame->format,
				vs->video_st->time_base.num, 
				vs->video_st->time_base.den,
				codec->sample_aspect_ratio.num, 
				FFMAX(codec->sample_aspect_ratio.den, 1));

		if (fr.num && fr.den) {
			av_strlcatf(buffersrc_args, sizeof(buffersrc_args), ":frame_rate=%d/%d", fr.num, fr.den);
		}

		if ((ret = avfilter_graph_create_filter(
				&filt_src, 
				avfilter_get_by_name("buffer"), 
				"sdl2ffmpeg", 
				buffersrc_args, 
				NULL, 
				graph)) < 0) {

			goto fail;
		}

		ret = avfilter_graph_create_filter(
				&filt_out, 
				avfilter_get_by_name("buffersink"),
				"sdl2ffmpeg_buffersink",
				NULL,
				NULL,
				graph);

		if (ret < 0) {
			goto fail;
		}

		/*if ((ret = av_opt_set_int_list(
				filt_out, 
				"pix_fmts", 
				out_pix_fmts, 
				AV_PIX_FMT_NONE,
				AV_OPT_SEARCH_CHILDREN)) < 0) {

			goto fail;
		}*/

		/* SDL YUV code is not handling odd width/height for some driver
		* combinations, therefore we crop the picture to an even width/height. */
		/*if ((ret = avfilter_graph_create_filter(
				&filt_crop, 
				avfilter_get_by_name("crop"),
				"sdl2ffmpeg_crop",
				"floor(in_w/2)*2:floor(in_h/2)*2", 
				NULL, 
				graph)) < 0) {

			goto fail;
		}*/

		if ((ret = avfilter_graph_create_filter(
				&filt_deint, 
				avfilter_get_by_name("yadif"),
				"sdl2ffmpeg_yadif",
				"parity=-1:deint=1", 
				NULL, 
				graph)) < 0) {

			goto fail;
		}

		if ((ret = avfilter_link(filt_deint, 0, filt_out, 0)) < 0) {
			goto fail;
		}

		if ((ret = configure_filtergraph(graph, vfilters, filt_src, filt_deint)) < 0) {
			goto fail;
		}

		vs->in_video_filter  = filt_src;
		vs->out_video_filter = filt_out;
fail:
		return ret;
	}

	int SDL2ffmpeg::configure_audio_filters(const char *afilters, int force_output_format) {
		static const enum AVSampleFormat sample_fmts[] = { AV_SAMPLE_FMT_S16, AV_SAMPLE_FMT_NONE };
		int sample_rates[2] = { 0, -1 };
		int64_t channel_layouts[2] = { 0, -1 };
		int channels[2] = { 0, -1 };
		AVFilterContext *filt_asrc = NULL, *filt_asink = NULL;
		char asrc_args[256];
		int ret;

		avfilter_graph_free(&vs->agraph);
		if (!(vs->agraph = avfilter_graph_alloc())) {
			return AVERROR(ENOMEM);
		}

		ret = snprintf(
				asrc_args,
				sizeof(asrc_args),
				"sample_rate=%d:sample_fmt=%s:channels=%d:time_base=%d/%d",
				vs->audio_filter_src.freq,
				av_get_sample_fmt_name(vs->audio_filter_src.fmt),
				vs->audio_filter_src.channels,
				1, 
				vs->audio_filter_src.freq);

		if (vs->audio_filter_src.channel_layout) {
			snprintf(
					asrc_args + ret,
					sizeof(asrc_args) - ret,
					":channel_layout=0x%"PRIx64,
					vs->audio_filter_src.channel_layout);
		}

		ret = avfilter_graph_create_filter(
				&filt_asrc,
				avfilter_get_by_name("abuffer"),
				"sdl2ffmpeg_abuffer",
				asrc_args, 
				NULL, 
				vs->agraph);

		if (ret < 0) {
			goto end;
		}

		ret = avfilter_graph_create_filter(
				&filt_asink,
				avfilter_get_by_name("abuffersink"),
				"sdl2ffmpeg_abuffersink",
				NULL, 
				NULL, 
				vs->agraph);

		if (ret < 0) {
			goto end;
		}

		if ((ret = av_opt_set_int_list(
				filt_asink,
				"sample_fmts",
				sample_fmts,
				AV_SAMPLE_FMT_NONE,
				AV_OPT_SEARCH_CHILDREN)) < 0) {

			goto end;
		}

		if ((ret = av_opt_set_int(
				filt_asink,
				"all_channel_counts",
				1, 
				AV_OPT_SEARCH_CHILDREN)) < 0) {

			goto end;
		}

		if (force_output_format) {
			channel_layouts[0] = vs->audio_tgt.channel_layout;
			channels       [0] = vs->audio_tgt.channels;
			sample_rates   [0] = vs->audio_tgt.freq;

			if ((ret = av_opt_set_int(
					filt_asink, 
					"all_channel_counts", 
					0, 
					AV_OPT_SEARCH_CHILDREN)) < 0) {

				goto end;
			}

			if ((ret = av_opt_set_int_list(
					filt_asink, 
					"channel_layouts", 
					channel_layouts,  
					-1, 
					AV_OPT_SEARCH_CHILDREN)) < 0) {

				goto end;
			}

			if ((ret = av_opt_set_int_list(
					filt_asink, 
					"channel_counts", 
					channels,
					-1, 
					AV_OPT_SEARCH_CHILDREN)) < 0) {

				goto end;
			}

			if ((ret = av_opt_set_int_list(
					filt_asink, 
					"sample_rates",
					sample_rates,
					-1, 
					AV_OPT_SEARCH_CHILDREN)) < 0) {

				goto end;
			}
		}


		if ((ret = configure_filtergraph(
				vs->agraph, 
				afilters, 
				filt_asrc, 
				filt_asink)) < 0) {

			goto end;
		}

		vs->in_audio_filter  = filt_asrc;
		vs->out_audio_filter = filt_asink;

end:
		if (ret < 0) {
			avfilter_graph_free(&vs->agraph);
		}
		return ret;
	}
	// AVFILTER end

	int SDL2ffmpeg::video_thread(void *arg) {
		AVPacket pkt    = { 0 };
		SDL2ffmpeg* dec = (SDL2ffmpeg*)arg;
		VideoState* vs  = dec->vs;
		AVFrame* frame  = av_frame_alloc();

		double pts;
		int ret;
		int serial = 0;

		// AVFILTER begin
		AVFilterGraph *graph = avfilter_graph_alloc();
		AVFilterContext *filt_out = NULL, *filt_in = NULL;
		int last_w = 0;
		int last_h = 0;
		enum AVPixelFormat last_format = (AVPixelFormat)-2;
		int last_serial = -1;
		// AVFILTER end

		while (!dec->abortRequest) {
			while (vs->paused && !vs->videoq.abort_request) {
				SDL_Delay(10);
			}

			avcodec_get_frame_defaults(frame);
			av_free_packet(&pkt);

			ret = dec->get_video_frame(frame, &pkt, &serial);
			if (ret < 0) {
				goto the_end;
			}

			if (!ret) {
				continue;
			}

			// AVFILTER begin
			if (last_w != frame->width || 
					last_h != frame->height ||
					last_format != frame->format ||
					last_serial != serial) {

				avfilter_graph_free(&graph);
				graph = avfilter_graph_alloc();
				if ((ret = dec->configure_video_filters(graph, dec->vfilters, frame)) < 0) {
					dec->abortRequest = true;
					av_free_packet(&pkt);
					goto the_end;
				}

				filt_in  = vs->in_video_filter;
				filt_out = vs->out_video_filter;
				last_w = frame->width;
				last_h = frame->height;
				last_format = (AVPixelFormat)frame->format;
				last_serial = serial;
			}

			ret = av_buffersrc_add_frame(filt_in, frame);
			if (ret < 0) {
				goto the_end;
			}

			av_frame_unref(frame);
			avcodec_get_frame_defaults(frame);
			av_free_packet(&pkt);

			while (ret >= 0) {
				vs->frame_last_returned_time = av_gettime() / 1000000.0;

				ret = av_buffersink_get_frame_flags(filt_out, frame, 0);
				if (ret < 0) {
					if (ret == AVERROR_EOF) {
						vs->video_finished = serial;
					}
					ret = 0;
					break;
				}

				vs->frame_last_filter_delay = av_gettime() / 1000000.0 - vs->frame_last_returned_time;
				if (fabs(vs->frame_last_filter_delay) > AV_NOSYNC_THRESHOLD / 10.0) {
					vs->frame_last_filter_delay = 0;
				}

				pts = (frame->pts == AV_NOPTS_VALUE) ? NAN : frame->pts * av_q2d(filt_out->inputs[0]->time_base);
				ret = dec->queue_picture(
						frame, 
						pts, 
						av_frame_get_pkt_pos(frame), 
						serial);

//				av_frame_unref(frame);
			}
			// AVFILTER end

			if (ret < 0) {
				goto the_end;
			}
		}
the_end:
		avcodec_flush_buffers(vs->video_st->codec);
		avfilter_graph_free(&graph);
		av_free_packet(&pkt);
		av_frame_free(&frame);

		clog << "SDL2ffmpeg::video_thread all done" << endl;
		return 0;
	}

	/*
	 * return the new audio buffer size (samples can be added or deleted
	 * to get better sync if video or external master clock)
	 */
	int SDL2ffmpeg::synchronize_audio(int nb_samples) {
		int wanted_nb_samples = nb_samples;

		/* if not master, then we try to remove or add samples to correct the clock */
		if (get_master_sync_type() != AV_SYNC_AUDIO_MASTER) {
			double diff, avg_diff;
			int min_nb_samples, max_nb_samples;

			diff = get_clock(&vs->audclk) - get_master_clock();

			if (!isnan(diff) && fabs(diff) < AV_NOSYNC_THRESHOLD) {
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
								(int)(diff * vs->audio_src.freq);

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
	int SDL2ffmpeg::audio_decode_frame() {
		AVPacket *pkt_temp;
		AVPacket *pkt;
		AVCodecContext *dec;
		int len1, data_size, resampled_data_size;
		int64_t dec_channel_layout;
		int got_frame;
		av_unused double audio_clock0;
		int wanted_nb_samples;
		AVRational tb;
		int ret;
		int reconfigure;

		if (vs->audio_st == NULL || abortRequest || status == ST_STOPPED) {
			return -1;
		}

		pkt_temp = &vs->audio_pkt_temp;
		pkt = &vs->audio_pkt;
		dec = vs->audio_st->codec;

		while (!abortRequest) {
			/* NOTE: the audio packet can contain several frames */
			while (pkt_temp->stream_index != -1 || vs->audio_buf_frames_pending) {
				if (!vs->frame) {
					if (!(vs->frame = avcodec_alloc_frame())){
						return AVERROR(ENOMEM);
					}

				} else {
					av_frame_unref(vs->frame);
					avcodec_get_frame_defaults(vs->frame);
				}

				if (vs->audioq.serial != vs->audio_pkt_temp_serial) {
					break;
				}

				if (vs->paused) {
					return -1;
				}

				if (!vs->audio_buf_frames_pending) {
					len1 = avcodec_decode_audio4(dec, vs->frame, &got_frame, pkt_temp);
					if (len1 < 0) {
						/* if error, we skip the frame */
						pkt_temp->size = 0;
						break;
					}

					pkt_temp->dts =
					pkt_temp->pts = AV_NOPTS_VALUE;
					pkt_temp->data += len1;
					pkt_temp->size -= len1;
					if (pkt_temp->data && pkt_temp->size <= 0 || !pkt_temp->data && !got_frame) {
						pkt_temp->stream_index = -1;
					}

					if (!pkt_temp->data && !got_frame) {
						vs->audio_finished = vs->audio_pkt_temp_serial;
					}

					if (!got_frame) {
						continue;
					}

					tb.num = 1;
					tb.den = vs->frame->sample_rate;
					if (vs->frame->pts != AV_NOPTS_VALUE) {
						vs->frame->pts = av_rescale_q(vs->frame->pts, dec->time_base, tb);

					} else if (vs->frame->pkt_pts != AV_NOPTS_VALUE) {
						vs->frame->pts = av_rescale_q(vs->frame->pkt_pts, vs->audio_st->time_base, tb);

					} else if (vs->audio_frame_next_pts != AV_NOPTS_VALUE) {
						AVRational rescavr = {1, vs->audio_filter_src.freq};
						vs->frame->pts = av_rescale_q(vs->audio_frame_next_pts, rescavr, tb);
					}

					if (vs->frame->pts != AV_NOPTS_VALUE) {
						vs->audio_frame_next_pts = vs->frame->pts + vs->frame->nb_samples;
					}

					dec_channel_layout = get_valid_channel_layout(
							vs->frame->channel_layout, 
							av_frame_get_channels(vs->frame));

					reconfigure = cmp_audio_fmts(
							vs->audio_filter_src.fmt, 
							vs->audio_filter_src.channels,
							(AVSampleFormat)vs->frame->format, 
							av_frame_get_channels(vs->frame))    ||
							vs->audio_filter_src.channel_layout != dec_channel_layout ||
							vs->audio_filter_src.freq           != vs->frame->sample_rate ||
							vs->audio_pkt_temp_serial           != vs->audio_last_serial;

					if (reconfigure) {
						char buf1[1024], buf2[1024];
						av_get_channel_layout_string(buf1, sizeof(buf1), -1, vs->audio_filter_src.channel_layout);
						av_get_channel_layout_string(buf2, sizeof(buf2), -1, dec_channel_layout);

						vs->audio_filter_src.fmt            = (AVSampleFormat)vs->frame->format;
						vs->audio_filter_src.channels       = av_frame_get_channels(vs->frame);
						vs->audio_filter_src.channel_layout = dec_channel_layout;
						vs->audio_filter_src.freq           = vs->frame->sample_rate;
						vs->audio_last_serial               = vs->audio_pkt_temp_serial;

						if ((ret = configure_audio_filters(afilters, 1)) < 0) {
							return ret;
						}
					}

					if ((ret = av_buffersrc_add_frame(vs->in_audio_filter, vs->frame)) < 0) {
						return ret;
					}
					av_frame_unref(vs->frame);
				}
				if ((ret = av_buffersink_get_frame_flags(vs->out_audio_filter, vs->frame, 0)) < 0) {
					if (ret == AVERROR(EAGAIN)) {
						vs->audio_buf_frames_pending = 0;
						continue;
					}

					if (ret == AVERROR_EOF) {
						vs->audio_finished = vs->audio_pkt_temp_serial;
					}
					return ret;
				}
				vs->audio_buf_frames_pending = 1;
				tb = vs->out_audio_filter->inputs[0]->time_base;

				data_size = av_samples_get_buffer_size(
						NULL, 
						av_frame_get_channels(vs->frame),
						vs->frame->nb_samples,
						(AVSampleFormat)vs->frame->format, 
						1);

				dec_channel_layout = (vs->frame->channel_layout && 
						av_frame_get_channels(vs->frame) == av_get_channel_layout_nb_channels(vs->frame->channel_layout)) ?
						vs->frame->channel_layout : av_get_default_channel_layout(av_frame_get_channels(vs->frame));

				wanted_nb_samples = synchronize_audio(vs->frame->nb_samples);

				if (vs->frame->format != vs->audio_src.fmt ||
						dec_channel_layout != vs->audio_src.channel_layout ||
						vs->frame->sample_rate != vs->audio_src.freq ||
						(wanted_nb_samples != vs->frame->nb_samples && !vs->swr_ctx)) {

					swr_free(&vs->swr_ctx);
					vs->swr_ctx = swr_alloc_set_opts(
							NULL,
							vs->audio_tgt.channel_layout, 
							vs->audio_tgt.fmt, 
							vs->audio_tgt.freq,
							dec_channel_layout,
							(AVSampleFormat)vs->frame->format, 
							vs->frame->sample_rate, 
							0, 
							NULL);

					if (!vs->swr_ctx || swr_init(vs->swr_ctx) < 0) {
						clog << "SDL2ffmpeg::audio_decode_frame ";
						clog << "Cannot create sample rate converter for";
						clog << " conversion of '" << dec->sample_rate << "'";
						clog << endl;
						break;
					}

					vs->audio_src.channel_layout = dec_channel_layout;
					vs->audio_src.channels = av_frame_get_channels(vs->frame);
					vs->audio_src.freq = vs->frame->sample_rate;
					vs->audio_src.fmt = (AVSampleFormat)vs->frame->format;
				}

				if (vs->swr_ctx) {
					const uint8_t **in = (const uint8_t **)vs->frame->extended_data;
					uint8_t **out = &vs->audio_buf1;
					int out_count = (int64_t)wanted_nb_samples * vs->audio_tgt.freq / vs->frame->sample_rate + 256;
					int out_size  = av_samples_get_buffer_size(NULL, vs->audio_tgt.channels, out_count, vs->audio_tgt.fmt, 0);
					int len2;

					if (out_size < 0) {
						break;
					}

					if (wanted_nb_samples != vs->frame->nb_samples) {
						if (swr_set_compensation(
								vs->swr_ctx, 
								(wanted_nb_samples - vs->frame->nb_samples) * vs->audio_tgt.freq / vs->frame->sample_rate,
								wanted_nb_samples * vs->audio_tgt.freq / vs->frame->sample_rate) < 0) {

							clog << "SDL2ffmpeg::audio_decode_frame ";
							clog << "swr_set_compensation() failed" << endl;
							break;
						}
					}

					av_fast_malloc(&vs->audio_buf1, &vs->audio_buf1_size, out_size);
					if (!vs->audio_buf1) {
						return AVERROR(ENOMEM);
					}

					len2 = swr_convert(vs->swr_ctx, out, out_count, in, vs->frame->nb_samples);
					if (len2 < 0) {
						clog << "SDL2ffmpeg::audio_decode_frame ";
						clog << "audio_resample() failed" << endl;
						break;
					}

					if (len2 == out_count) {
						clog << "SDL2ffmpeg::audio_decode_frame ";
						clog << "Warning! audio buffer is probably too small";
						clog << endl;

						swr_init(vs->swr_ctx);
					}

					vs->audio_buf = vs->audio_buf1;
					resampled_data_size = len2 * vs->audio_tgt.channels * av_get_bytes_per_sample(vs->audio_tgt.fmt);

				} else {
					vs->audio_buf = vs->frame->data[0];
					resampled_data_size = data_size;
				}

				audio_clock0 = vs->audio_clock;

				/* update the audio clock with the pts */
				if (vs->frame->pts != AV_NOPTS_VALUE) {
					vs->audio_clock = vs->frame->pts * av_q2d(tb) + (double)vs->frame->nb_samples / vs->frame->sample_rate;

				} else {
					vs->audio_clock = NAN;
				}

				vs->audio_clock_serial = vs->audio_pkt_temp_serial;
				return resampled_data_size;
			}

			/* free the current packet */
			if (pkt->data) {
				av_free_packet(pkt);
			}

			memset(pkt_temp, 0, sizeof(*pkt_temp));
			pkt_temp->stream_index = -1;

			if (vs->audioq.abort_request) {
				return -1;
			}

			if (vs->audioq.nb_packets == 0) {
				SDL_CondSignal(vs->continue_read_thread);
			}

			/* read next packet */
			if ((packet_queue_get(&vs->audioq, pkt, 1, &vs->audio_pkt_temp_serial)) < 0) {
				return -1;
			}

			if (pkt->data == flush_pkt.data) {
				avcodec_flush_buffers(dec);
				vs->audio_buf_frames_pending = 0;
				vs->audio_frame_next_pts = AV_NOPTS_VALUE;
				if ((vs->ic->iformat->flags & (AVFMT_NOBINSEARCH | AVFMT_NOGENSEARCH | AVFMT_NO_BYTE_SEEK)) && !vs->ic->iformat->read_seek) {
					vs->audio_frame_next_pts = vs->audio_st->start_time;
				}
			}

			*pkt_temp = *pkt;
		}
		return 0;
	}

	void SDL2ffmpeg::sdl_audio_callback(void* opaque, Uint8* stream, int len) {
		set<SDL2ffmpeg*>::iterator i;
		SDL2ffmpeg* dec;
		VideoState* vs;
		int audio_size, len1;
		int bytes_per_sec;
		int frame_size;
		int64_t audio_cb_time;
		bool skip = false;
		int currentLen, offset;

		Thread::mutexLock(&aiMutex);
		audio_cb_time = av_gettime();

		memset(stream, 0, len);

		i = aInstances.begin();
		while (i != aInstances.end()) {
			dec = (*i);
			vs  = dec->vs;

			if (dec->status == ST_PLAYING && vs != NULL &&
					vs->audio_stream >= 0 && vs->audio_st != NULL) {

				currentLen = len;
				offset = 0;
				frame_size = av_samples_get_buffer_size(
						NULL,
						vs->audio_tgt.channels,
						1,
						vs->audio_tgt.fmt, 
						1);

				while (currentLen > 0) {
					if (vs->audio_buf_index >= vs->audio_buf_size) {
						audio_size = dec->audio_decode_frame();
						if (audio_size < 0) {
							if (dec->abortRequest || dec->status == ST_STOPPED) {
								++i;
								skip = true;
								break;
							}

							/* if error, just output silence */
							vs->audio_buf      = vs->silence_buf;
							vs->audio_buf_size = sizeof(vs->silence_buf) / frame_size * frame_size;

						} else {
							vs->audio_buf_size = audio_size;
						}

						vs->audio_buf_index = 0;
					}
					len1 = vs->audio_buf_size - vs->audio_buf_index;
					if (len1 > currentLen) {
						len1 = currentLen;
					}

					SDL_MixAudio(
							stream + offset,
							(uint8_t *)vs->audio_buf + vs->audio_buf_index,
							len1,
							dec->soundLevel);

					currentLen -= len1;
					offset += len1;
					vs->audio_buf_index += len1;
				}

				if (skip) {
					skip = false;
					continue;
				}

				bytes_per_sec = vs->audio_tgt.freq * vs->audio_tgt.channels * av_get_bytes_per_sample(vs->audio_tgt.fmt);
				vs->audio_write_buf_size = vs->audio_buf_size - vs->audio_buf_index;
				/* Let's assume the audio driver that is used by SDL has two periods. */
				if (!isnan(vs->audio_clock)) {
					dec->set_clock_at(&vs->audclk, vs->audio_clock - (double)(2 * vs->audio_hw_buf_size + vs->audio_write_buf_size) / bytes_per_sec, vs->audio_clock_serial, audio_cb_time / 1000000.0);
					dec->sync_clock_to_slave(&vs->extclk, &vs->audclk);
				}
			}

			++i;
		}
		
		Thread::mutexUnlock(&aiMutex);
	}

	int SDL2ffmpeg::audio_open(
			int64_t wanted_channel_layout,
			int wanted_nb_channels,
			int wanted_sample_rate,
			struct AudioParams *audio_hw_params) {

		Uint8 tmpChannels;
		int tmpFreq;

		const char *env;
		const int next_nb_channels[] = {0, 0, 1, 6, 2, 6, 4, 6};

		env = SDL_getenv("SDL_AUDIO_CHANNELS");
		if (env) {
			wanted_nb_channels = SDL_atoi(env);
			wanted_channel_layout = av_get_default_channel_layout(
					wanted_nb_channels);
		}

		if (!wanted_channel_layout ||
				wanted_nb_channels != av_get_channel_layout_nb_channels(
						wanted_channel_layout)) {

			wanted_channel_layout = av_get_default_channel_layout(
					wanted_nb_channels);

			wanted_channel_layout &= ~AV_CH_LAYOUT_STEREO_DOWNMIX;
		}

		tmpChannels = (unsigned int)av_get_channel_layout_nb_channels(
				wanted_channel_layout);

		tmpFreq = wanted_sample_rate;
		if (tmpFreq <= 0 || (unsigned int)tmpChannels == 0 ||
				(unsigned int)tmpChannels > 7) {

			fprintf(stderr, "Invalid sample rate or channel count!\n");
			return -1;
		}

		wantedSpec.format   = AUDIO_S16SYS;
		wantedSpec.silence  = 0;
		wantedSpec.samples  = SDL_AUDIO_BUFFER_SIZE;
		wantedSpec.callback = sdl_audio_callback;
		wantedSpec.userdata = NULL;
		wantedSpec.freq     = 44100;
		wantedSpec.channels = 2;

		if (spec.size == 0) {
			while (SDL_OpenAudio(&wantedSpec, &spec) < 0) {
				wantedSpec.channels = (unsigned int)next_nb_channels[FFMIN(
						7, wantedSpec.channels)];

				if ((unsigned int)wantedSpec.channels == 0 ||
						(unsigned int)wantedSpec.channels > 7) {

					return -1;
				}

				wanted_channel_layout = av_get_default_channel_layout(
						(int)wantedSpec.channels);
			}

			if (spec.size == 0) {
				spec.size = (unsigned int)wantedSpec.channels *
						(unsigned int)wantedSpec.samples * 2;

			} else {
				wantedSpec.size = spec.size;
			}

		} else {

			wantedSpec.samples = spec.samples;

			clog << "SDL2ffmpeg::stream_component_open (2nd audio src = '";
			clog << vs->filename << ")";
			clog << endl;
			clog << "Desired format = '" << wantedSpec.format;
			clog << "'" << endl;
			clog << "Desired silence = '" << wantedSpec.silence;
			clog << "'" << endl;
			clog << "Desired samples = '" << wantedSpec.samples;
			clog << "'" << endl;
			clog << "Desired size = '" << wantedSpec.size;
			clog << "'" << endl;
			clog << "Desired channels = '" << (unsigned int)wantedSpec.channels;
			clog << "'" << endl;
			clog << "Desired frequency = '" << wantedSpec.freq;
			clog << "'" << endl;
			clog << endl;
		}

		if (spec.format != AUDIO_S16SYS) {
			return -1;
		}

		wantedSpec.freq     = tmpFreq;
		wantedSpec.channels = tmpChannels;

		if (spec.channels != wantedSpec.channels) {
			wanted_channel_layout = av_get_default_channel_layout(
					(int)spec.channels);

			if (!wanted_channel_layout) {
				return -1;
			}
		}

		audio_hw_params->fmt            = AV_SAMPLE_FMT_S16;
		audio_hw_params->freq           = spec.freq;
		audio_hw_params->channel_layout = wanted_channel_layout;
		audio_hw_params->channels       = (int)spec.channels;

		return spec.size;
	}

	/* open a given stream. Return 0 if OK */
	int SDL2ffmpeg::stream_component_open(int stream_index) {
		AVFormatContext* ic = vs->ic;
		AVCodecContext* avctx;
		AVCodec* codec;
		AVDictionaryEntry* t = NULL;
		AVDictionary* opts;
		int sample_rate, nb_channels;
		int64_t channel_layout;
		int ret;

		if (stream_index < 0 || stream_index >= ic->nb_streams) {
			clog << "SDL2ffmpeg::stream_component_open Warning! Invalid ";
			clog << "index '" << stream_index << "'" << endl;
			return -1;
		}

		avctx = ic->streams[stream_index]->codec;

		codec = avcodec_find_decoder(avctx->codec_id);

		switch (avctx->codec_type) {
			case AVMEDIA_TYPE_AUDIO:
				vs->last_audio_stream = stream_index;
				break;

			case AVMEDIA_TYPE_VIDEO:
				vs->last_video_stream = stream_index;
				break;
		}

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

		avctx->error_concealment = error_concealment;

		if (avctx->lowres || (codec->capabilities & CODEC_CAP_DR1)) {
			avctx->flags |= CODEC_FLAG_EMU_EDGE;
		}

		if (fast) {
			avctx->flags2 |= CODEC_FLAG2_FAST;
		}

		if (codec->capabilities & CODEC_CAP_DR1) {
			avctx->flags |= CODEC_FLAG_EMU_EDGE;
		}

		opts = filter_codec_opts(NULL, avctx->codec_id, ic, ic->streams[stream_index], codec);
		if (!av_dict_get(opts, "threads", NULL, 0)) {
			av_dict_set(&opts, "threads", "auto", 0);
		}

		if (avctx->lowres) {
			av_dict_set(&opts, "lowres", av_asprintf("%d", avctx->lowres), AV_DICT_DONT_STRDUP_VAL);
		}

		if (avctx->codec_type == AVMEDIA_TYPE_VIDEO || avctx->codec_type == AVMEDIA_TYPE_AUDIO) {
			av_dict_set(&opts, "refcounted_frames", "1", 0);
		}

		if (avcodec_open2(avctx, codec, &opts) < 0) {
			return -1;
		}

		if ((t = av_dict_get(opts, "", NULL, AV_DICT_IGNORE_SUFFIX))) {
			av_log(NULL, AV_LOG_ERROR, "Option %s not found.\n", t->key);
			return AVERROR_OPTION_NOT_FOUND;
		}

		if (!codec || avcodec_open2(avctx, codec, NULL) < 0) {
			clog << "SDL2ffmpeg::stream_component_open Warning! ";
			clog << "Can't open codec" << endl;
			return -1;
		}

		ic->streams[stream_index]->discard = AVDISCARD_DEFAULT;
		switch (avctx->codec_type) {
			case AVMEDIA_TYPE_AUDIO:
				AVFilterLink *link;

				vs->audio_filter_src.freq           = avctx->sample_rate;
				vs->audio_filter_src.channels       = avctx->channels;
				vs->audio_filter_src.channel_layout = get_valid_channel_layout(avctx->channel_layout, avctx->channels);
				vs->audio_filter_src.fmt            = avctx->sample_fmt;
				if ((ret = configure_audio_filters(afilters, 0)) < 0) {
					return ret;
				}
				link = vs->out_audio_filter->inputs[0];
				sample_rate    = link->sample_rate;
				nb_channels    = link->channels;
				channel_layout = link->channel_layout;

				/* prepare audio output */
				if ((ret = audio_open(channel_layout, nb_channels, sample_rate, &vs->audio_tgt)) < 0) {
					return ret;
				}
				vs->audio_hw_buf_size = ret;
				vs->audio_src = vs->audio_tgt;
				vs->audio_buf_size  = 0;
				vs->audio_buf_index = 0;

				/* init averaging filter */
				vs->audio_diff_avg_coef  = exp(log(0.01) / AUDIO_DIFF_AVG_NB);
				vs->audio_diff_avg_count = 0;
				/* since we do not have a precise enough audio fifo fullness,
				   we correct audio sync only if larger than this threshold */
				vs->audio_diff_threshold = 2.0 * vs->audio_hw_buf_size /
						av_samples_get_buffer_size(
								NULL, vs->audio_tgt.channels,
								vs->audio_tgt.freq,
								vs->audio_tgt.fmt,
								1);

				memset(&vs->audio_pkt, 0, sizeof(vs->audio_pkt));
				memset(&vs->audio_pkt_temp, 0, sizeof(vs->audio_pkt_temp));
				vs->audio_pkt_temp.stream_index = -1;

				vs->audio_stream = stream_index;
				vs->audio_st = ic->streams[stream_index];
				packet_queue_start(&vs->audioq);
				break;

			case AVMEDIA_TYPE_VIDEO:
				vs->video_stream = stream_index;
				vs->video_st = ic->streams[stream_index];

				packet_queue_start(&vs->videoq);
				vs->queue_attachments_req = 1;
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
				swr_free(&vs->swr_ctx);

				av_freep(&vs->audio_buf1);
				vs->audio_buf1_size = 0;
				vs->audio_buf = NULL;
				av_frame_free(&vs->frame);

				if (vs->rdft) {
					av_rdft_end(vs->rdft);
					av_freep(&vs->rdft_data);
					vs->rdft = NULL;
					vs->rdft_bits = 0;
				}
				avfilter_graph_free(&vs->agraph);
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

			default:
				break;
		}
	}

	int SDL2ffmpeg::decode_interrupt_cb(void *ctx) {
		SDL2ffmpeg* dec = (SDL2ffmpeg*)ctx;
	    return dec->abortRequest;
	}

	int SDL2ffmpeg::is_realtime(AVFormatContext *s) {
		if (!strcmp(s->iformat->name, "rtp") || 
				!strcmp(s->iformat->name, "rtsp") || 
				!strcmp(s->iformat->name, "sdp")) {

			return 1;
		}

		if (s->pb && (!strncmp(s->filename, "rtp:", 4) ||
				!strncmp(s->filename, "udp:", 4))) {
					return 1;
		}

		return 0;
	}

	int SDL2ffmpeg::read_init() {
		int err, i, ret = 0;

		vs->video_stream    = -1;
		vs->audio_stream    = -1;

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
			seek_by_bytes = !!(vs->ic->iformat->flags & AVFMT_TS_DISCONT) && strcmp("ogg", vs->ic->iformat->name);
		}

		vs->max_frame_duration = (vs->ic->iformat->flags & AVFMT_TS_DISCONT) ? 10.0 : 3600.0;
		vs->realtime = is_realtime(vs->ic);

		for (i = 0; i < vs->ic->nb_streams; i++) {
			vs->ic->streams[i]->discard = AVDISCARD_ALL;
		}

		if (infinite_buffer < 0 && vs->realtime) {
			infinite_buffer = 1;
		}

		return ret;
	}

	int SDL2ffmpeg::read_thread(void *arg) {
		SDL2ffmpeg* dec = (SDL2ffmpeg*)arg;
		VideoState* vs  = dec->vs;

		int ret;
		AVPacket pkt1, *pkt = &pkt1;
		int eof = 0;
		int64_t stream_start_time;
		int pkt_in_play_range = 0;
		SDL_mutex *wait_mutex = SDL_CreateMutex();

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
					clog << "SDL2ffmpeg::read_thread ";
					clog << "error while seeking '" << vs->ic->filename << "'";
					clog << endl;

				} else {
					if (vs->audio_stream >= 0) {
						dec->packet_queue_flush(&vs->audioq);
						dec->packet_queue_put(&vs->audioq, &dec->flush_pkt);
					}

					if (vs->video_stream >= 0) {
						dec->packet_queue_flush(&vs->videoq);
						dec->packet_queue_put(&vs->videoq, &dec->flush_pkt);
					}

					if (vs->seek_flags & AVSEEK_FLAG_BYTE) {
						dec->set_clock(&vs->extclk, NAN, 0);

					} else {
						dec->set_clock(&vs->extclk, seek_target / (double)AV_TIME_BASE, 0);
					}
				}
				vs->seek_req = 0;
				vs->queue_attachments_req = 1;
				eof = 0;
				if (vs->paused) {
					dec->step_to_next_frame();
				}
			}

			if (vs->queue_attachments_req) {
				if (vs->video_st && vs->video_st->disposition & AV_DISPOSITION_ATTACHED_PIC) {
					AVPacket copy;
					if ((ret = av_copy_packet(&copy, &vs->video_st->attached_pic)) < 0) {
						dec->stop();
						return 0;
					}
					dec->packet_queue_put(&vs->videoq, &copy);
				}
				vs->queue_attachments_req = 0;
			}

			/* if the queue are full, no need to read more */
			if (dec->infinite_buffer < 1 &&
					(vs->audioq.size + vs->videoq.size >
					MAX_QUEUE_SIZE || ((vs->audioq.nb_packets > MIN_FRAMES ||
						vs->audio_stream < 0 ||
						vs->audioq.abort_request) &&
						(vs->videoq.nb_packets > MIN_FRAMES ||
								vs->video_stream < 0 ||
								vs->videoq.abort_request || 
								(vs->video_st->disposition & AV_DISPOSITION_ATTACHED_PIC))))) {

				/* wait 10 ms */
				SDL_LockMutex(wait_mutex);
				SDL_CondWaitTimeout(vs->continue_read_thread, wait_mutex, 10);
				SDL_UnlockMutex(wait_mutex);
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

				if (vs->audio_stream >= 0) {
					av_init_packet(pkt);
					pkt->data = NULL;
					pkt->size = 0;
					pkt->stream_index = vs->audio_stream;
					dec->packet_queue_put(&vs->audioq, pkt);
				}

				SDL_Delay(10);
				if (vs->audioq.size + vs->videoq.size == 0) {
					dec->stop();

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

				SDL_LockMutex(wait_mutex);
				SDL_CondWaitTimeout(vs->continue_read_thread, wait_mutex, 10);
				SDL_UnlockMutex(wait_mutex);
				continue;
			}

			stream_start_time = vs->ic->streams[pkt->stream_index]->start_time;

			/*
			 * check if packet is in play range specified by user, then queue,
			 * otherwise discard
			 */
			pkt_in_play_range = dec->duration == AV_NOPTS_VALUE ||
					(pkt->pts - (stream_start_time != AV_NOPTS_VALUE ? stream_start_time : 0)) *
					av_q2d(vs->ic->streams[pkt->stream_index]->time_base) -
					(double)(dec->start_time != AV_NOPTS_VALUE ?
							dec->start_time : 0) / 1000000
					<= ((double)dec->duration / 1000000);

			if (pkt->stream_index == vs->audio_stream && pkt_in_play_range) {
				dec->packet_queue_put(&vs->audioq, pkt);

			} else if (pkt->stream_index == vs->video_stream &&
					pkt_in_play_range &&
					!(vs->video_st->disposition & AV_DISPOSITION_ATTACHED_PIC)) {

				dec->packet_queue_put(&vs->videoq, pkt);

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
		int start_index, stream_index = 0;
		AVStream *st;

		if (codec_type == AVMEDIA_TYPE_VIDEO) {
			start_index = vs->video_stream;

		} else if (codec_type == AVMEDIA_TYPE_AUDIO) {
			start_index = vs->audio_stream;
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

	AVDictionary* SDL2ffmpeg::filter_codec_opts(
			AVDictionary *opts, 
			enum AVCodecID codec_id,
			AVFormatContext *s, 
			AVStream *st, 
			AVCodec *codec) {

		AVDictionary* ret = NULL;
		AVDictionaryEntry *t = NULL;

		int flags = s->oformat ? AV_OPT_FLAG_ENCODING_PARAM : AV_OPT_FLAG_DECODING_PARAM;
		char prefix = 0;
		const AVClass* cc = avcodec_get_class();

		if (!codec) {
			codec = s->oformat ? avcodec_find_encoder(codec_id) : avcodec_find_decoder(codec_id);
		}

		if (!codec) {
			return NULL;
		}

		switch (codec->type) {
			case AVMEDIA_TYPE_VIDEO:
				prefix  = 'v';
				flags  |= AV_OPT_FLAG_VIDEO_PARAM;
				break;

			case AVMEDIA_TYPE_AUDIO:
				prefix  = 'a';
				flags  |= AV_OPT_FLAG_AUDIO_PARAM;
				break;
		}

		while (t = av_dict_get(opts, "", t, AV_DICT_IGNORE_SUFFIX)) {
			char *p = strchr(t->key, ':');

			if (av_opt_find(&cc, t->key, NULL, flags, AV_OPT_SEARCH_FAKE_OBJ) || 
					(codec && codec->priv_class && av_opt_find(
							&codec->priv_class, t->key, NULL, flags, AV_OPT_SEARCH_FAKE_OBJ))) {

				av_dict_set(&ret, t->key, t->value, 0);

			} else if (t->key[0] == prefix && av_opt_find(
					&cc, t->key + 1, NULL, flags, AV_OPT_SEARCH_FAKE_OBJ))  {

				av_dict_set(&ret, t->key + 1, t->value, 0);
			}

			if (p) {
				*p = ':';
			}
		}
		return ret;
	}
}
}
}
}
}
}
