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

	SDL_AudioSpec SDL2ffmpeg::wantedSpec;
	short SDL2ffmpeg::refCount = 0;
	set<SDL2ffmpeg*> SDL2ffmpeg::aInstances;
	bool SDL2ffmpeg::init = false;
	pthread_mutex_t SDL2ffmpeg::aiMutex;

	SDL2ffmpeg::SDL2ffmpeg(const char *filename) {
		int st_index[AVMEDIA_TYPE_NB];
		int err, i;

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
		monoStep                             = 0;
		status                               = ST_STOPPED;

		setSoundLevel(0.5);

		if (!init) {
			init = true;

			memset(&wantedSpec, 0, sizeof(wantedSpec));
			memset(&spec, 0, sizeof(spec));

			getAudioSpec(&wantedSpec, ASD_FREQ, ASD_CHANNELS);
//			wantedSpec.size = ASD_BUF_SIZE;
			pthread_mutex_init(&aiMutex, NULL);

			if (SDL_OpenAudio(&wantedSpec, &spec) >= 0) {

				clog << "Audio device opened with the following specification:";
				clog << endl;

				clog << "Channels: " << (short)spec.channels;
				clog << " (" << (short)wantedSpec.channels << ")";
				clog << endl;

				clog << "Frequency: " << spec.freq;
				clog << " (" << wantedSpec.freq << ")" << endl;

				clog << "Format: " << spec.format;
				clog << " (" << wantedSpec.format << ")" << endl;

				clog << "Samples: " << spec.samples;
				clog << " (" << wantedSpec.samples << ")" << endl;

				clog << "Size: " << spec.size;
				clog << " (" << wantedSpec.size << ")" << endl;
				clog << endl;

				if (spec.channels != 0) {
					memcpy(&wantedSpec, &spec, sizeof(spec));

					memset(&spec, 0, sizeof(spec));
				}

				clog << "Audio device acquired the following specification:";
				clog << endl;
				clog << "Channels: " << (short)wantedSpec.channels << endl;
				clog << "Frequency: " << wantedSpec.freq << endl;
				clog << "Format: " << wantedSpec.format << endl;
				clog << "Samples: " << wantedSpec.samples << endl;
				clog << "Size: " << wantedSpec.size << endl;
				clog << endl;
			}
		}

		memset(&flush_pkt, 0, sizeof(flush_pkt));
		memset(&vPkt, 0, sizeof(vPkt));

		vs = (VideoState*)av_mallocz(sizeof(VideoState));
		if (vs) {
			av_strlcpy(vs->filename, filename, sizeof(vs->filename));

			if (refCount == 0) {
				av_log_set_flags(AV_LOG_SKIP_REPEATED);

				/* register all codecs, demux and protocols */
				avcodec_register_all();
				av_register_all();
				avformat_network_init();
			}

			av_init_packet(&flush_pkt);
			flush_pkt.data = (uint8_t*)"FLUSH";

			memset(st_index, -1, sizeof(st_index));
			vs->video_stream    = -1;
			vs->audio_stream    = -1;
			vs->subtitle_stream = -1;

			vs->ic = avformat_alloc_context();

			vs->ic->interrupt_callback.opaque = this;

			err = avformat_open_input(&vs->ic, vs->filename, NULL, NULL);
			if (err >= 0) {
				if (genpts) {
					vs->ic->flags |= AVFMT_FLAG_GENPTS;
				}

				err = avformat_find_stream_info(vs->ic, NULL);
				if (err >= 0) {
					if (vs->ic->pb) {
						vs->ic->pb->eof_reached = 0;
					}

					if (seek_by_bytes < 0) {
						seek_by_bytes= !!(
								vs->ic->iformat->flags & AVFMT_TS_DISCONT);
					}

					for (i = 0; i < vs->ic->nb_streams; i++) {
						vs->ic->streams[i]->discard = AVDISCARD_ALL;
					}

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
						clog << "SDL2ffmpeg::SDL2ffmpeg ";
						clog << "Can't select any audio stream in mrl '";
						clog << vs->filename << "'" << endl;
					}

					if (st_index[AVMEDIA_TYPE_VIDEO] >= 0) {
						stream_component_open(st_index[AVMEDIA_TYPE_VIDEO]);

					} else {
						clog << "SDL2ffmpeg::SDL2ffmpeg ";
						clog << "Can't select any video stream in mrl '";
						clog << vs->filename << "'" << endl;
					}

					if (st_index[AVMEDIA_TYPE_SUBTITLE] >= 0) {
						stream_component_open(st_index[AVMEDIA_TYPE_SUBTITLE]);

					} else {
						clog << "SDL2ffmpeg::SDL2ffmpeg ";
						clog << "Can't select any subtitle stream in mrl '";
						clog << vs->filename << "'" << endl;
					}

					vs->av_sync_type = av_sync_type;

				    vs->pictq_mutex = SDL_CreateMutex();
				    vs->pictq_cond  = SDL_CreateCond();

				    vs->subpq_mutex = SDL_CreateMutex();
				    vs->subpq_cond  = SDL_CreateCond();

				} else {
					clog << "SDL2ffmpeg::SDL2ffmpeg ";
					clog << "Can't find stream info for '" << filename;
					clog << "'" << endl;
				}

			} else {
				clog << "SDL2ffmpeg::SDL2ffmpeg ";
				clog << "Can't open '" << filename << "'" << endl;
			}
		}

		refCount++;
	}

	SDL2ffmpeg::~SDL2ffmpeg() {
		clog << "SDL2ffmpeg::~SDL2ffmpeg" << endl;

		hasPic = false;

		release();

		if (texture != NULL) {
			SDL_DestroyTexture(texture);
			texture = NULL;
		}

		refCount--;

		if (refCount == 0) {
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

	bool SDL2ffmpeg::prepare() {
		SDL_AudioSpec obtained;

		if (vs->audio_stream >= 0) {
			getAudioSpec(&spec, audioFreq, audioChannels);

			clog << "SDL2ffmpeg::prepare ";
			clog << "Audio content has the following specification: ";
			clog << endl;
			clog << "Channels: " << (short)spec.channels;
			clog << endl;
			clog << "Frequency: " << spec.freq << endl;
			clog << "Format: " << spec.format << endl;
			clog << "Samples: " << spec.samples << endl;
			clog << endl;

			SDL_BuildAudioCVT(
					&acvt,
					spec.format, spec.channels, spec.freq,
					wantedSpec.format, wantedSpec.channels, wantedSpec.freq);

			spec.size              = spec.channels * spec.samples * 2;
			vs->audio_hw_buf_size  = spec.size;
			vs->audio_main_buf[0]  = (uint8_t*)malloc(spec.size);
			vs->audio_main_buf[1]  = (uint8_t*)malloc(spec.size);

			vs->audio_src_fmt      = vs->audio_tgt_fmt = AV_SAMPLE_FMT_S16;

			vs->audio_src_freq     = vs->audio_tgt_freq = spec.freq;
			vs->audio_src_channels = vs->audio_tgt_channels = spec.channels;

			vs->audio_src_channel_layout = vs->audio_tgt_channel_layout =
					av_get_default_channel_layout(spec.channels);

/*			is->audio_tgt_freq           = wantedSpec.freq;
			is->audio_tgt_channels       = wantedSpec.channels;
			is->audio_tgt_channel_layout = av_get_default_channel_layout(
					wantedSpec.channels);
*/

			vs->audio_diff_threshold = (2.0 * spec.samples / spec.freq);
					//2.0 * SDL_AUDIO_BUFFER_SIZE / spec.freq);

			/* init averaging filter */
			vs->audio_diff_avg_coef  = exp(log(0.01) / AUDIO_DIFF_AVG_NB);
			vs->audio_diff_avg_count = 0;
			/* since we do not have a precise anough audio fifo fullness,
			   we correct audio sync only if larger than this threshold */
		}

		return true;
	}

	void SDL2ffmpeg::play() {
		clog << "SDL2ffmpeg::play called" << endl;
		if (status == ST_STOPPED) {
			prepare();
			status = ST_PLAYING;

			if (vs->audio_st != NULL && vs->audio_stream >= 0) {
				pthread_mutex_lock(&aiMutex);
				aInstances.insert(this);
				pthread_mutex_unlock(&aiMutex);

				if (!hasSDLAudio) {
					clog << "SDL2ffmpeg::play calling SDL_PauseAudio(0)";
					clog << endl;

					hasSDLAudio = true;
					SDL_PauseAudio(0);
				}
			}

			if (vs->video_st != NULL && vs->video_stream >= 0) {
				pthread_t vTId;
				pthread_attr_t vTAttr;

				pthread_attr_init(&vTAttr);
				pthread_attr_setdetachstate(&vTAttr, PTHREAD_CREATE_DETACHED);
				pthread_attr_setscope(&vTAttr, PTHREAD_SCOPE_SYSTEM);

				pthread_create(&vTId, &vTAttr, t_video_refresh_decoder, this);
				pthread_detach(vTId);
			}

			if ((vs->video_st != NULL && vs->video_stream >= 0) ||
					(vs->audio_st != NULL && vs->audio_stream >= 0)) {

				pthread_t rTId;
				pthread_attr_t rTAttr;

				pthread_attr_init(&rTAttr);
				pthread_attr_setdetachstate(&rTAttr, PTHREAD_CREATE_DETACHED);
				pthread_attr_setscope(&rTAttr, PTHREAD_SCOPE_SYSTEM);

				pthread_create(&rTId, &rTAttr, t_read_content, this);
				pthread_detach(rTId);
			}
		}
	}

	void SDL2ffmpeg::stop() {
		set<SDL2ffmpeg*>::iterator i;

		clog << "SDL2ffmpeg::stop(" << vs->filename << ")" << endl;

		abortRequest                = true;
		vs->videoq.abort_request    = 1;
		vs->audioq.abort_request    = 1;
		vs->subtitleq.abort_request = 1;

		status = ST_STOPPED;

		if (vs->video_stream >= 0) {
			SDL_CondSignal(vs->videoq.cond);
			SDL_CondSignal(vs->pictq_cond);
		}

		if (vs->audio_stream >= 0) {
			SDL_CondSignal(vs->audioq.cond);
		}

		if (vs->subtitle_stream >= 0) {
			SDL_CondSignal(vs->subtitleq.cond);
		}

		pthread_mutex_lock(&aiMutex);

		i = aInstances.find(this);
		if (i != aInstances.end()) {
			aInstances.erase(i);
		}

		if (aInstances.empty() && hasSDLAudio) {
			clog << endl;
			clog << "SDL2ffmpeg::stop calling SDL_PauseAudio(1)";
			clog << endl << endl;

			hasSDLAudio = false;
			SDL_PauseAudio(1);
		}

		pthread_mutex_unlock(&aiMutex);

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
		if (vs->video_st && vs->video_st->codec->width){
			*w = vs->video_st->codec->width;
			*h = vs->video_st->codec->height;

		} else {
			*w = 0;
			*h = 0;
		}
	}

	double SDL2ffmpeg::getDuration() {
		return vs->ic->duration/1000LL;
	}

	double SDL2ffmpeg::getPosition() {
		double position = 0.0;

		if (vs->audio_stream >= 0 && vs->audio_st) {
			position = vs->audio_current_pts;

		} else if (vs->video_stream >= 0 && vs->video_st) {
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

		} else {
			soundLevel = level * SDL_MIX_MAXVOLUME;
		}
	}


	int SDL2ffmpeg::packet_queue_put(PacketQueue *q, AVPacket *pkt) {
		AVPacketList* pkt1;

		/* duplicate the packet */
		if (pkt != &flush_pkt && av_dup_packet(pkt) < 0) {
			return -1;
		}

		pkt1 = (AVPacketList*)av_malloc(sizeof(AVPacketList));

		if (!pkt1) {
			return -1;
		}

		pkt1->pkt  = *pkt;
		pkt1->next = NULL;

		SDL_LockMutex(q->mutex);
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

		SDL_UnlockMutex(q->mutex);
		return 0;
	}

	void SDL2ffmpeg::packet_queue_init(PacketQueue *q) {
		memset(q, 0, sizeof(PacketQueue));

		q->mutex = SDL_CreateMutex();
		q->cond  = SDL_CreateCond();

		packet_queue_put(q, &flush_pkt);
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

	void SDL2ffmpeg::packet_queue_end(PacketQueue *q) {
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

	/* allocate a picture (needs to do that in main thread to avoid
	   potential locking problems */
	void SDL2ffmpeg::alloc_picture() {
		VideoPicture *vp;

		vp = &vs->pictq[vs->pictq_windex];

		vp->width     = vs->video_st->codec->width;
		vp->height    = vs->video_st->codec->height;
		vp->pix_fmt   = vs->video_st->codec->pix_fmt;
		vp->tex       = texture;

		SDL_LockMutex(vs->pictq_mutex);
		vp->allocated = 1;
		SDL_CondSignal(vs->pictq_cond);
		SDL_UnlockMutex(vs->pictq_mutex);
	}

	void SDL2ffmpeg::free_subpicture(SubPicture *sp) {
		avsubtitle_free(&sp->sub);
	}

	void SDL2ffmpeg::video_image_display() {
		VideoPicture *vp;
		SubPicture *sp;
		float aspect_ratio;
		int width, height, x, y;
		SDL_Rect rect;

		if (vs->pictq_size <= 0 || !hasPic) {
			return;
		}

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
							((float)sp->sub.start_display_time / 1000)) {

						/*
						pict.data[0]     = vp->bmp->pixels;
						pict.linesize[0] = vp->bmp->pitch;

						for (i = 0; i < sp->sub.num_rects; i++)
							blend_subrect(&pict, sp->sub.rects[i],
										  vp->bmp->w, vp->bmp->h);

						SDL_LockYUVOverlay (vp->bmp);

						pict.data[0] = vp->bmp->pixels[0];
						pict.data[1] = vp->bmp->pixels[2];
						pict.data[2] = vp->bmp->pixels[1];

						pict.linesize[0] = vp->bmp->pitches[0];
						pict.linesize[1] = vp->bmp->pitches[2];
						pict.linesize[2] = vp->bmp->pitches[1];

						for (i = 0; i < sp->sub.num_rects; i++)
							blend_subrect(&pict, sp->sub.rects[i],
										  vp->bmp->w, vp->bmp->h);

						SDL_UnlockYUVOverlay (vp->bmp);
						 */
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

			if (vp->tex) {
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
					fprintf(stderr, "Can't initialize the conversion context\n");
					return;
				}

				if (vp->src_frame) {
					uint8_t* pixels[AV_NUM_DATA_POINTERS];
					int pitch[AV_NUM_DATA_POINTERS];

					AVPicture pict;

					SDL_LockTexture(vp->tex, NULL, (void**)&pixels, &pitch[0]);

					sws_scale(
							vs->img_convert_ctx,
							(const uint8_t* const*)vp->src_frame->data,
							vp->src_frame->linesize,
							0, vp->height, pixels, pitch);

					SDL_UnlockTexture(vp->tex);
					hasPic = false;
				}
			}
		}
	}

	void SDL2ffmpeg::stream_close() {
		VideoPicture *vp;
		int i;

		clog << "SDL2ffmpeg::stream_close" << endl;

		abortRequest = true;

		for (i=0;i<VIDEO_PICTURE_QUEUE_SIZE; i++) {
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

		vs = NULL;
	}

	/* display the current picture, if any */
	void SDL2ffmpeg::video_display() {
		video_image_display();
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

		if (vs->av_sync_type == AV_SYNC_AUDIO_MASTER) {
			if (vs->audio_st) {
				val = get_audio_clock();

			} else {
				val = get_video_clock();
			}

		} else 	if (vs->av_sync_type == AV_SYNC_VIDEO_MASTER) {
			if (vs->video_st) {
				val = get_video_clock();

			} else {
				val = get_audio_clock();
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
			vs->frame_timer += av_gettime() / 1000000.0 +
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

			vp->allocated  = 0;
			vp->reallocate = 0;

			alloc_picture();
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

		if (packet_queue_get(&vs->videoq, pkt, 0) < 0 ||
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

	/* return the new audio buffer size (samples can be added or deleted
	   to get better sync if video or external master clock) */
	int SDL2ffmpeg::synchronize_audio(
			short *samples, int samples_size1, double pts) {

		int n, samples_size;
		double ref_clock;

		n = av_get_bytes_per_sample(vs->audio_src_fmt) * vs->audio_src_channels;
		samples_size = samples_size1;

		/*
		 * if not master,
		 * then we try to remove or add samples to correct the clock
		 */
		if (((vs->av_sync_type == AV_SYNC_VIDEO_MASTER && vs->video_st) ||
				vs->av_sync_type == AV_SYNC_EXTERNAL_CLOCK)) {

			double diff, avg_diff;
			int wanted_size, min_size, max_size, nb_samples;

			ref_clock = get_master_clock();
			diff = get_audio_clock() - ref_clock;

			if (diff < AV_NOSYNC_THRESHOLD) {
				vs->audio_diff_cum = diff + vs->audio_diff_avg_coef *
						vs->audio_diff_cum;

				if (vs->audio_diff_avg_count < AUDIO_DIFF_AVG_NB) {
					/* not enough measures to have a correct estimate */
					vs->audio_diff_avg_count++;

				} else {
					/* estimate the A-V difference */
					avg_diff = vs->audio_diff_cum * (
							1.0 - vs->audio_diff_avg_coef);

					if (fabs(avg_diff) >= vs->audio_diff_threshold) {
						wanted_size = samples_size + ((int)(
								diff * vs->audio_tgt_freq) * n);

						nb_samples = samples_size / n;

						min_size = ((nb_samples * (100 -
								SAMPLE_CORRECTION_PERCENT_MAX)) / 100) * n;

						max_size = ((nb_samples * (100 +
								SAMPLE_CORRECTION_PERCENT_MAX)) / 100) * n;

						if (wanted_size < min_size) {
							wanted_size = min_size;

						} else if (wanted_size > FFMIN3(
								max_size,
								samples_size,
								sizeof(vs->audio_buf2))) {

							wanted_size = FFMIN3(
									max_size,
									samples_size,
									sizeof(vs->audio_buf2));
						}

						/* add or remove samples to correction the synchro */
						if (wanted_size < samples_size) {
							/* remove samples */
							samples_size = wanted_size;

						} else if (wanted_size > samples_size) {
							uint8_t *samples_end, *q;
							int nb;

							/* add samples */
							nb = (samples_size - wanted_size);
							samples_end = (uint8_t *)samples + samples_size - n;
							q = samples_end + n;
							while (nb > 0) {
								memcpy(q, samples_end, n);
								q += n;
								nb -= n;
							}
							samples_size = wanted_size;
						}
					}
				}

			} else {
				/* too big difference : may be initial PTS errors, so
				   reset A-V filter */
				vs->audio_diff_avg_count = 0;
				vs->audio_diff_cum = 0;
			}
		}

		return samples_size;
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

		if (abortRequest ||
				vs->audio_stream < 0 || !vs->audio_st || vs->paused) {

			return -1;
		}

		pkt_temp  = &vs->audio_pkt_temp;
		pkt       = &vs->audio_pkt;
		dec       = vs->audio_st->codec;
		data_size = -1;

		/* NOTE: the audio packet can contain several frames */
		while (pkt_temp->size > 0 || (!pkt_temp->data && new_packet)) {
			if (!vs->frame) {

				if (!(vs->frame = avcodec_alloc_frame())) {
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
					dec->sample_fmt, 1);

			dec_channel_layout = (dec->channel_layout &&
					dec->channels == av_get_channel_layout_nb_channels(
							dec->channel_layout)) ? dec->channel_layout :
									av_get_default_channel_layout(
											dec->channels);

			if (dec->sample_fmt != vs->audio_tgt_fmt ||
					dec_channel_layout != vs->audio_tgt_channel_layout ||
					dec->sample_rate != vs->audio_tgt_freq) {

				if (vs->swr_ctx) {
					swr_free(&vs->swr_ctx);
				}

				vs->swr_ctx = swr_alloc_set_opts(
						NULL,
						vs->audio_tgt_channel_layout,
						vs->audio_tgt_fmt,
						vs->audio_tgt_freq,
						vs->audio_src_channel_layout,
						vs->audio_src_fmt,
						vs->audio_src_freq,
						0, NULL);

				if (!vs->swr_ctx || swr_init(vs->swr_ctx) < 0) {
					clog << "SDL2ffmpeg::audio_decode_frame " << stderr;
					clog << " Can't create sample rate converter for ";
					clog << dec->sample_rate << "Hz ";
					clog << av_get_sample_fmt_name(dec->sample_fmt);
					clog << " " << dec->channels << " channels to ";
					clog << vs->audio_tgt_freq << "Hz ";
					clog << av_get_sample_fmt_name(vs->audio_tgt_fmt);
					clog << " " << vs->audio_tgt_channels << " channels!";
					clog << endl;
					break;
				}

				vs->audio_src_channel_layout = dec_channel_layout;
				vs->audio_src_channels       = dec->channels;
				vs->audio_src_freq           = dec->sample_rate;
				vs->audio_src_fmt            = dec->sample_fmt;
			}

			resampled_data_size = data_size;
			if (vs->swr_ctx) {
				const uint8_t *in[] = { vs->frame->data[0] };
				uint8_t *out[] = {vs->audio_buf2};
				len2 = swr_convert(
						vs->swr_ctx,
						out,
						sizeof(vs->audio_buf2) /
								vs->audio_tgt_channels /
								av_get_bytes_per_sample(vs->audio_tgt_fmt),
						in,
						data_size / dec->channels / av_get_bytes_per_sample(
								dec->sample_fmt));

				if (len2 < 0) {
					fprintf(stderr, "audio_resample() failed\n");
					break;
				}

				if (len2 == sizeof(vs->audio_buf2) /
						vs->audio_tgt_channels /
						av_get_bytes_per_sample(vs->audio_tgt_fmt)) {

					fprintf(stderr, "warning: audio buffer is too small\n");
					swr_init(vs->swr_ctx);
				}

				vs->audio_buf = vs->audio_buf2;
				resampled_data_size = len2 * vs->audio_tgt_channels *
						av_get_bytes_per_sample(vs->audio_tgt_fmt);

				pts = vs->audio_clock;
				*pts_ptr = pts;
				vs->audio_clock += (double)resampled_data_size /
						(vs->audio_tgt_channels *
								vs->audio_tgt_freq * av_get_bytes_per_sample(
										vs->audio_tgt_fmt));

			} else {
				vs->audio_buf = vs->frame->data[0];

				pts = vs->audio_clock;
				*pts_ptr = pts;
				vs->audio_clock += (double)data_size / (dec->channels *
						dec->sample_rate * av_get_bytes_per_sample(
								dec->sample_fmt));
			}

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
		if ((new_packet = packet_queue_get(&vs->audioq, pkt, 0)) < 0) {
			return -1;
		}

		if (pkt->data == flush_pkt.data) {
			avcodec_flush_buffers(dec);
		}

		*pkt_temp = *pkt;

		if (pkt->pts != AV_NOPTS_VALUE) {
			vs->audio_clock = av_q2d(vs->audio_st->time_base)*pkt->pts;
		}

		return data_size;
	}

	/* open a given stream. Return 0 if OK */
	int SDL2ffmpeg::stream_component_open(int stream_index) {
		AVFormatContext *ic = vs->ic;
		AVCodecContext *avctx;
		AVCodec *codec;

		int64_t wanted_channel_layout = 0;

		if (stream_index < 0 || stream_index >= ic->nb_streams) {
			clog << "SDL2ffmpeg::stream_component_open ";
			clog << "invalid index";
			clog << endl;
			return -1;
		}

		avctx = ic->streams[stream_index]->codec;

		codec = avcodec_find_decoder(avctx->codec_id);
		if (!codec) {
			clog << "SDL2ffmpeg::stream_component_open ";
			clog << "Can't find codec for '" << avctx->codec_type << "'";
			clog << endl;
			return -1;
		}

		avctx->workaround_bugs = workaround_ffmpeg_bugs;
		avctx->lowres = lowres;
		if (avctx->lowres > codec->max_lowres) {
			avctx->lowres = codec->max_lowres;
		}

		if (avctx->lowres) {
			avctx->flags |= CODEC_FLAG_EMU_EDGE;
		}

		avctx->idct_algo = idct;

		if (fast) {
			avctx->flags2 |= CODEC_FLAG2_FAST;
		}

		avctx->skip_frame        = skip_frame;
		avctx->skip_idct         = skip_idct;
		avctx->skip_loop_filter  = skip_loop_filter;
		avctx->error_concealment = error_concealment;

		if (codec->capabilities & CODEC_CAP_DR1) {
			avctx->flags |= CODEC_FLAG_EMU_EDGE;
		}

		if (avctx->codec_type == AVMEDIA_TYPE_AUDIO) {
			wanted_channel_layout = (
					avctx->channel_layout &&
					avctx->channels == av_get_channel_layout_nb_channels(
							avctx->channels)) ? avctx->channel_layout :
									av_get_default_channel_layout(
											avctx->channels);

			wanted_channel_layout &= ~AV_CH_LAYOUT_STEREO_DOWNMIX;

			vs->audio_src_channel_layout = wanted_channel_layout;

			audioChannels = av_get_channel_layout_nb_channels(
					wanted_channel_layout);

			audioFreq = avctx->sample_rate;
			if (audioFreq <= 0) {
				clog << "SDL2ffmpeg::stream_component_open ";
				clog << "Invalid sample rate or channel count!";
				clog << endl;

				return -1;
			}
		}

		if (avcodec_open2(avctx, codec, NULL) < 0) {
			clog << "SDL2ffmpeg::stream_component_open ";
			clog << "can't open codec '" << codec->name << "'";
			clog << endl;
			return -1;
		}

		ic->streams[stream_index]->discard = AVDISCARD_DEFAULT;
		switch(avctx->codec_type) {
			case AVMEDIA_TYPE_AUDIO:
				vs->audio_stream           = stream_index;
				vs->audio_st               = ic->streams[stream_index];

				vs->audio_hw_buf_size      = 0;

				vs->audio_main_buf_size[0] = 0;
				vs->audio_main_buf_size[1] = 0;

				memset(&vs->audio_pkt, 0, sizeof(vs->audio_pkt));

				packet_queue_init(&vs->audioq);
				break;

			case AVMEDIA_TYPE_VIDEO:
				vs->video_stream = stream_index;
				vs->video_st = ic->streams[stream_index];

				packet_queue_init(&vs->videoq);
				break;

			case AVMEDIA_TYPE_SUBTITLE:
				vs->subtitle_stream = stream_index;
				vs->subtitle_st = ic->streams[stream_index];

				packet_queue_init(&vs->subtitleq);

				pthread_t sTId;
				pthread_attr_t sTAttr;

				pthread_attr_init(&sTAttr);
				pthread_attr_setdetachstate(&sTAttr, PTHREAD_CREATE_DETACHED);
				pthread_attr_setscope(&sTAttr, PTHREAD_SCOPE_SYSTEM);

				pthread_create(&sTId, &sTAttr, subtitle_refresh, this);
				pthread_detach(sTId);

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
		if (avctx == NULL) {
			return;
		}

		switch(avctx->codec_type) {
			case AVMEDIA_TYPE_AUDIO:
				packet_queue_abort(&vs->audioq);
				packet_queue_end(&vs->audioq);

				if (vs->swr_ctx) {
					swr_free(&vs->swr_ctx);
				}

				av_free_packet(&vs->audio_pkt);
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

		        SDL_LockMutex(vs->pictq_mutex);
		        SDL_CondSignal(vs->pictq_cond);
		        SDL_UnlockMutex(vs->pictq_mutex);

				packet_queue_end(&vs->videoq);
				break;

			case AVMEDIA_TYPE_SUBTITLE:
				packet_queue_abort(&vs->subtitleq);

		        SDL_LockMutex(vs->subpq_mutex);
		        vs->subtitle_stream_changed = 1;

		        SDL_CondSignal(vs->subpq_cond);
		        SDL_UnlockMutex(vs->subpq_mutex);

		        packet_queue_end(&vs->subtitleq);

				break;

			default:
				break;
		}

		ic->streams[stream_index]->discard = AVDISCARD_ALL;

		avcodec_close(avctx);
		switch(avctx->codec_type) {
			case AVMEDIA_TYPE_AUDIO:
				vs->audio_st        = NULL;
				vs->audio_stream    = -1;
				break;

			case AVMEDIA_TYPE_VIDEO:
				vs->video_st        = NULL;
				vs->video_stream    = -1;
				break;

			case AVMEDIA_TYPE_SUBTITLE:
				vs->subtitle_st     = NULL;
				vs->subtitle_stream = -1;
				break;

			default:
				break;

		}

		ic->streams[stream_index]->codec = NULL;
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
		for(;;) {
			if (++stream_index >= vs->ic->nb_streams) {
				if (codec_type == AVMEDIA_TYPE_SUBTITLE) {
					stream_index = -1;
					return;

				} else
					stream_index = 0;
			}
			if (stream_index == start_index)
				return;
			st = ic->streams[stream_index];
			if (st->codec->codec_type == codec_type) {
				/* check that parameters are OK */
				switch(codec_type) {
				case AVMEDIA_TYPE_AUDIO:
					if (st->codec->sample_rate != 0 &&
							st->codec->channels != 0) {

						stream_component_close(start_index);
						stream_component_open(stream_index);
						return;
					}
					break;

				case AVMEDIA_TYPE_VIDEO:
				case AVMEDIA_TYPE_SUBTITLE:
					stream_component_close(start_index);
					stream_component_open(stream_index);
					return;
				default:
					break;

				}
			}
		}
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

				audio_size = synchronize_audio(
						(short*)(int16_t*)vs->audio_buf,
						audio_size,
						pts);

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

	/* called to display each frame */
	void SDL2ffmpeg::video_refresh_content() {
		VideoPicture *vp;
		SubPicture *sp, *sp2;
		int len2;

		if (abortRequest) {
			return;
		}

		if (vs != NULL && vs->video_stream >= 0 && vs->video_st) {
			if (vs->pictq_size == 0) {
				SDL_LockMutex(vs->pictq_mutex);
				if (vs->frame_last_dropped_pts != AV_NOPTS_VALUE &&
						vs->frame_last_dropped_pts > vs->frame_last_pts) {

					update_video_pts(
							vs->frame_last_dropped_pts,
							vs->frame_last_dropped_pos);

					vs->frame_last_dropped_pts = AV_NOPTS_VALUE;
				}
				SDL_UnlockMutex(vs->pictq_mutex);
				//nothing to do, no picture to display in the que

			} else {
				double cur_time, last_duration, duration, delay;
				/* dequeue the picture */
				vp = &vs->pictq[vs->pictq_rindex];

				if (vp->skip) {

					/*clog << "SDL2ffmpeg::video_refresh_content(";
					clog << vs->filename << ") skipping frame ";
					clog << endl;*/

					pictq_next_picture();
					video_refresh_content();
					return;
				}

				/* compute nominal last_duration */
				last_duration = vp->pts - vs->frame_last_pts;
				if (last_duration > 0 && last_duration < 10.0) {
					/*
					 * if duration of the last frame was sane,
					 * update last_duration in video state
					 */
					vs->frame_last_duration = last_duration;
				}
				delay = compute_target_delay(vs->frame_last_duration);

				cur_time = av_gettime()/1000000.0;
				if (cur_time < vs->frame_timer + delay) {
					double st = ((vs->frame_timer + delay) - cur_time) * 500;

					/*clog << "SDL2ffmpeg::video_refresh_content(";
					clog << vs->filename << ") frame is too ";
					clog << "young: should be presented in '" << st;
					clog << "' ms" << endl;*/

					//SystemCompat::uSleep(st * 1000);
					return;
				}

				if (delay > 0) {
					vs->frame_timer += delay * FFMAX(
							1, floor((cur_time-vs->frame_timer) / delay));
				}

				SDL_LockMutex(vs->pictq_mutex);
				update_video_pts(vp->pts, vp->pos);
				SDL_UnlockMutex(vs->pictq_mutex);

				if (vs->pictq_size > 1) {
					VideoPicture *nextvp = &vs->pictq[(
							vs->pictq_rindex+1)%VIDEO_PICTURE_QUEUE_SIZE];

					duration = nextvp->pts - vp->pts;

				} else {
					duration = vp->duration;
				}

				if ((framedrop > 0 || (framedrop && vs->audio_st)) &&
						cur_time > vs->frame_timer + duration) {

					if (vs->pictq_size > 1) {
						/*clog << "SDL2ffmpeg::video_refresh_content(";
						clog << vs->filename << ") dropping ";
						clog << "frame" << endl;*/

						vs->frame_drops_late++;
						pictq_next_picture();
						video_refresh_content();
						return;
					}
				}

				if (vs->subtitle_st) {
					if (vs->subtitle_stream_changed) {
						SDL_LockMutex(vs->subpq_mutex);

						while (vs->subpq_size) {
							free_subpicture(&vs->subpq[vs->subpq_rindex]);

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
								sp2 = &vs->subpq[(vs->subpq_rindex + 1
										) % SUBPICTURE_QUEUE_SIZE];

							} else {
								sp2 = NULL;
							}

							if ((vs->video_current_pts > (sp->pts + ((float)
									sp->sub.end_display_time / 1000))) ||
									(sp2 && vs->video_current_pts > (sp2->pts +
											((float)sp2->sub.start_display_time
													/ 1000)))) {

								free_subpicture(sp);

								/*
								 * update queue size and signal for
								 * next picture
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

				/* display picture */
				//video_display();
				hasPic = true;
				pictq_next_picture();
			}
		}
	}

	void* SDL2ffmpeg::t_video_refresh_decoder(void* ptr) {
		SDL2ffmpeg* dec = (SDL2ffmpeg*)ptr;

		while (dec->video_refresh_decoder() >= 0 &&
				dec->status != ST_STOPPED) {

			while (dec->vs->paused && !dec->vs->videoq.abort_request) {
				SystemCompat::uSleep(10000);
			}
		}

		clog << "SDL2ffmpeg::t_refresh_decoder all done" << endl;
		return NULL;
	}

	int SDL2ffmpeg::video_refresh_decoder() {
		int64_t pts_int = AV_NOPTS_VALUE, pos = -1;
		double pts;
		int ret;

		if (!vs->videoq.abort_request && !vs->paused) {
			ret = get_video_frame(&vFrame, &pts_int, &vPkt);
			if (ret < 0) {
				clog << "SDL2ffmpeg::video_refresh_decoder(" << vs->filename;
				clog << ") can't get video frame packet size = '";
				clog << vPkt.size << "'" << endl;
				return ret;
			}

			pos = vPkt.pos;
			av_free_packet(&vPkt);

			vs->frame_last_filter_delay = av_gettime() /
					1000000.0 - vs->frame_last_returned_time;

			if (fabs(vs->frame_last_filter_delay) > AV_NOSYNC_THRESHOLD / 10.0){
				vs->frame_last_filter_delay = 0;
			}

			pts = pts_int*av_q2d(vs->video_st->time_base);

			if (vs->step) {
				stream_toggle_pause();
			}

			ret = queue_picture(&vFrame, pts, pos);
			if (ret < 0) {
				return ret;
			}
		}

		return 0;
	}

	void* SDL2ffmpeg::t_read_content(void* ptr) {
		SDL2ffmpeg* dec;

		dec = (SDL2ffmpeg*)ptr;
		while (dec->status != ST_STOPPED) {
			if (!dec->reof) {
				dec->read_content();

			} else {
				if (dec->vs->audioq.size + dec->vs->videoq.size +
						dec->vs->subtitleq.size == 0) {

					dec->status = ST_STOPPED;
					break;

				} else {
					SystemCompat::uSleep(1000000/25);
				}
			}

			dec->video_refresh_content();
		}

		clog << "SDL2ffmpeg::t_read_content all done" << endl;
		return NULL;
	}

	int SDL2ffmpeg::read_content() {
		int ret;
		AVPacket pkt1, *pkt = &pkt1;
		int pkt_in_play_range = 0;

		if (!abortRequest) {
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

				SystemCompat::uSleep(10000);
				return 0;
			}

			if (vs->seek_req) {
				int64_t seek_target= vs->seek_pos;
				int64_t seek_min= vs->seek_rel > 0 ?
						seek_target - vs->seek_rel + 2: INT64_MIN;

				int64_t seek_max= vs->seek_rel < 0 ?
						seek_target - vs->seek_rel - 2: INT64_MAX;

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
						packet_queue_flush(&vs->audioq);
						packet_queue_put(&vs->audioq, &flush_pkt);
					}

					if (vs->subtitle_stream >= 0) {
						packet_queue_flush(&vs->subtitleq);
						packet_queue_put(&vs->subtitleq, &flush_pkt);
					}

					if (vs->video_stream >= 0) {
						packet_queue_flush(&vs->videoq);
						packet_queue_put(&vs->videoq, &flush_pkt);
					}
				}
				vs->seek_req = 0;
			}

			/* if the queue are full, no need to read more */
			if (vs->audioq.size + vs->videoq.size +
					vs->subtitleq.size > MAX_QUEUE_SIZE) {

				SystemCompat::uSleep(10000);
				return 0;
			}

			if ((vs->audioq.size > MIN_AUDIOQ_SIZE || vs->audio_stream < 0) &&
					(vs->videoq.nb_packets > MIN_FRAMES ||
							vs->video_stream < 0) &&
					(vs->subtitleq.nb_packets > MIN_FRAMES ||
							vs->subtitle_stream < 0)) {

				SystemCompat::uSleep(10000);
				return 0;
			}

			ret = av_read_frame(vs->ic, pkt);
			if (ret != 0) {
				if (ret == AVERROR_EOF || url_feof(vs->ic->pb)) {
					clog << "SDL2ffmpeg::read_content(" << vs->filename;
					clog << ") EOF";
					clog << endl;

					if (vs->video_stream >= 0) {
						av_init_packet(pkt);
						pkt->data=NULL;
						pkt->size=0;
						pkt->stream_index= vs->video_stream;
						packet_queue_put(&vs->videoq, pkt);
					}

					if (vs->audio_stream >= 0 &&
							(vs->audio_st->codec->codec->capabilities &
									CODEC_CAP_DELAY)) {

						av_init_packet(pkt);
						pkt->data = NULL;
						pkt->size = 0;
						pkt->stream_index = vs->audio_stream;
						packet_queue_put(&vs->audioq, pkt);
					}

					reof = true;
					return 0;

				} else {
					clog << "SDL2ffmpeg::read_content(" << vs->filename;
					clog << ") exception ret = '";
					clog << ret << "'" << endl;

					SystemCompat::uSleep(10000);
				}

				if (vs->ic->pb && vs->ic->pb->error) {
					return -1;
				}

				return -1;
			}

			/*
			 * check if packet is in play range specified by user,
			 * then queue, otherwise discard
			 */
			pkt_in_play_range = duration == AV_NOPTS_VALUE ||
					(pkt->pts - vs->ic->streams[pkt->stream_index]->start_time) *
					av_q2d(vs->ic->streams[pkt->stream_index]->time_base) -
					(double)(start_time != AV_NOPTS_VALUE ?
							start_time : 0)/1000000

					<= ((double)duration/1000000);

			if (pkt->stream_index == vs->audio_stream && pkt_in_play_range) {
				ret = packet_queue_put(&vs->audioq, pkt);

			} else if (pkt->stream_index == vs->video_stream &&
					pkt_in_play_range) {

				ret = packet_queue_put(&vs->videoq, pkt);

			} else if (pkt->stream_index == vs->subtitle_stream &&
					pkt_in_play_range) {

				ret = packet_queue_put(&vs->subtitleq, pkt);

			} else {
				av_free_packet(pkt);
				ret = -1;
			}

		} else {
			ret = -1;
		}

		return ret;
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

	/* prepare a new audio buffer */
	void SDL2ffmpeg::sdl_audio_callback(void* opaque, Uint8* stream, int len) {
		set<SDL2ffmpeg*>::iterator i;
		SDL2ffmpeg* dec;
		VideoState* is;

		int64_t audio_cb_time;
		int audio_size;
		int bytes_per_sec;
		int ret;
		double pts;

		int64_t uResponseTime;
		int64_t elapsedTime;

		int cvt_len;

		unsigned int sleepTime;

		//clog << "SDL2ffmpeg::sdl_audio_callback begin" << endl;

		if (wantedSpec.freq == 0 || wantedSpec.samples == 0) {
			clog << "SDL2ffmpeg::sdl_audio_callback Warning frequency is 0!";
			clog << " Exiting" << endl;
			return;
		}

		pthread_mutex_lock(&aiMutex);
		audio_cb_time = av_gettime();

		memset(stream, 0, len);

		i = aInstances.begin();
		while (i != aInstances.end()) {
			dec = (*i);
			is  = dec->vs;

			if (is->audio_stream >= 0 && is->audio_st != NULL &&
					dec->status == ST_PLAYING) {

				dec->audio_refresh_decoder();
				if (is->audio_main_buf_size[0] == is->audio_hw_buf_size) {

					if (dec->acvt.needed) {
						/*if (dec->monoStep == 0) {
							dec->acvt.len = is->audio_hw_buf_size;

							if (dec->acvt.buf == NULL) {
								dec->acvt.buf = (Uint8*)malloc(
										dec->acvt.len * dec->acvt.len_mult);
							}

							if (dec->acvt.buf != NULL) {
								SDL_ConvertAudio(&dec->acvt);

								clog << endl;
								clog << "Converting(cb len = '" << len;
								clog << "', Dec = '";
								clog << dec << "', bytes to convert = '";
								clog << dec->acvt.len << "', soundLevel = '";
								clog << dec->soundLevel << "')" << endl;
								clog << "FROM: ";
								clog << "format '" << dec->spec.format;
								clog << "' channels '";
								clog << (short)dec->spec.channels;
								clog << "' freq '" << dec->spec.freq;
								clog << "' samples '" << dec->spec.samples;
								clog << "' bufSize '" << is->audio_hw_buf_size;
								clog << "'";
								clog << endl;
								clog << "TO: ";
								clog << "format '" << wantedSpec.format << "' ";
								clog << "channels '";
								clog << (short)wantedSpec.channels;
								clog << "' freq '" << wantedSpec.freq << "' ";
								clog << "' samples '" << wantedSpec.samples;
								clog << "' converted size '";
								clog << dec->acvt.len_cvt;
								clog << "'";
								clog << "' converted length ratio '";
								clog << dec->acvt.len_ratio;
								clog << "'";
								clog << endl;
							}
						}

						clog << "Mixed '" << len << "' bytes" << endl;
						clog << "Total in this step '" << dec->monoStep;
						clog << "'" << endl;

						SDL_MixAudio(
								stream,
								dec->acvt.buf + dec->monoStep,
								len,
								dec->soundLevel);

						dec->monoStep = dec->monoStep + len;

						if (dec->monoStep >=
								dec->acvt.len_ratio * is->audio_hw_buf_size) {

						if (dec->acvt.buf == NULL) {
							dec->acvt.buf = (Uint8*)createCVT(
									is->audio_main_buf[0],
									is->audio_hw_buf_size,
									dec->acvt.len_ratio,
									dec->spec.samples);

							dec->acvt.buf = (Uint8*)interleave(
									is->audio_main_buf[0],
									is->audio_hw_buf_size,
									3);
						}*/

						SDL_MixAudio(
								stream,
								is->audio_main_buf[0],
								is->audio_hw_buf_size,
								dec->soundLevel);

						dec->monoStep++;

						if (dec->monoStep >= dec->acvt.len_ratio) {
							dec->monoStep = 0;
							if (dec->acvt.buf != NULL) {
								free(dec->acvt.buf);
								dec->acvt.buf = NULL;
							}

							clog << "CB_LEN = '" << len << "'" << endl;
							clog << "MY_LEN = '" << is->audio_write_buf_size;
							clog << "'" << endl;
							clog << "MULTI = '" << dec->acvt.len_mult;
							clog << "'" << endl;
							clog << "CVT_LEN = '" << dec->acvt.len_cvt;
							clog << "'" << endl;

							bytes_per_sec = is->audio_tgt_freq *
									is->audio_tgt_channels *
									av_get_bytes_per_sample(is->audio_tgt_fmt);

							is->audio_write_buf_size = is->audio_hw_buf_size;

							is->audio_current_pts = is->audio_clock -
									(double)(2 * is->audio_hw_buf_size +
											is->audio_write_buf_size) / bytes_per_sec;

							is->audio_current_pts_drift = is->audio_current_pts -
									audio_cb_time / 1000000.0;

							is->audio_main_buf_size[0] = 0;
						}

					} else {
						SDL_MixAudio(
								stream,
								is->audio_main_buf[0],
								is->audio_hw_buf_size,
								dec->soundLevel);

						bytes_per_sec = is->audio_tgt_freq *
								is->audio_tgt_channels *
								av_get_bytes_per_sample(is->audio_tgt_fmt);

						is->audio_write_buf_size = is->audio_hw_buf_size;

						is->audio_current_pts = is->audio_clock -
								(double)(2 * is->audio_hw_buf_size +
										is->audio_write_buf_size) / bytes_per_sec;

						is->audio_current_pts_drift = is->audio_current_pts -
								audio_cb_time / 1000000.0;

						is->audio_main_buf_size[0] = 0;
					}

				} else {
					/*clog << endl << endl;
					clog << "SDL2ffmpeg::sdl_audio_callback ";
					clog << "not this time for " << is->filename;
					clog << " audio buffer size = ";
					clog << is->audio_main_buf_size[0];
					clog << " HW buffer size = " << is->audio_hw_buf_size;
					clog << " len = " << len;
					clog << " samples = " << dec->spec.samples;
					clog << " freq = " << dec->spec.freq;
					clog << " channels = " << (short)dec->spec.channels;
					clog << endl;*/
				}
			}
			++i;
		}

		pthread_mutex_unlock(&aiMutex);

		/*elapsedTime = (av_gettime() - audio_cb_time) * (multi + 1);

		if (elapsedTime < 30000) {
			sleepTime = (unsigned int)(30000 - elapsedTime);

			SystemCompat::uSleep(sleepTime);
		}*/

		elapsedTime = av_gettime() - audio_cb_time;
		sleepTime   = 1000000/25;

		if (sleepTime > elapsedTime) {
			SystemCompat::uSleep(sleepTime - elapsedTime);
		}

		/*uResponseTime = ((double)((double)wantedSpec.samples /
				(double)wantedSpec.freq)) * 100000;

		elapsedTime   = av_gettime() - audio_cb_time;

		if (elapsedTime < uResponseTime) {
			sleepTime = (unsigned int)(uResponseTime - elapsedTime);

			clog << endl << endl;
			clog << "SLEEP TIME = '" << sleepTime << "'" << endl;
			clog << endl;
			pthread_mutex_unlock(&aiMutex);
			SystemCompat::uSleep(sleepTime);

		} else {
			clog << endl << endl;
			clog << "NO SLEEP RESPONSE = '" << uResponseTime << "' (";
			clog << "samples='" << wantedSpec.samples << "', freq='";
			clog << wantedSpec.freq << "') AND ";
			clog << "ELAPSED = '" << elapsedTime << "'" << endl;
			clog << endl;
			pthread_mutex_unlock(&aiMutex);
		}*/

		//clog << "SDL2ffmpeg::sdl_audio_callback end" << endl;
	}

	void* SDL2ffmpeg::subtitle_refresh(void *arg) {
		SDL2ffmpeg* thiz = (SDL2ffmpeg*)arg;
		VideoState* is   = thiz->vs;

		SubPicture *sp;
		AVPacket pkt1, *pkt = &pkt1;
		int got_subtitle;
		double pts;
		int i, j;
		int r, g, b, y, u, v, a;
/*
		for(;;) {
			while (is->paused && !is->subtitleq.abort_request) {
				SystemCompat::uSleep(10000);
			}

			if (thiz->packet_queue_get(&is->subtitleq, pkt, 1) < 0) {
				break;
			}

			if (pkt->data == thiz->flush_pkt.data) {
				avcodec_flush_buffers(is->subtitle_st->codec);
				continue;
			}

			if (is->subtitleq.abort_request) {
				return 0;
			}

			sp = &is->subpq[is->subpq_windex];

			pts = 0;
			if (pkt->pts != AV_NOPTS_VALUE) {
				pts = av_q2d(is->subtitle_st->time_base)*pkt->pts;
			}

			avcodec_decode_subtitle2(
					is->subtitle_st->codec,
					&sp->sub,
					&got_subtitle,
					pkt);

			if (got_subtitle && sp->sub.format == 0) {
				sp->pts = pts;

				for (i = 0; i < sp->sub.num_rects; i++) {
					for (j = 0; j < sp->sub.rects[i]->nb_colors; j++) {
						RGBA_IN(r, g, b, a,
								(uint32_t*)sp->sub.rects[i]->pict.data[1] + j);

						y = RGB_TO_Y_CCIR(r, g, b);
						u = RGB_TO_U_CCIR(r, g, b, 0);
						v = RGB_TO_V_CCIR(r, g, b, 0);
						YUVA_OUT((uint32_t*)
								sp->sub.rects[i]->pict.data[1] + j, y, u, v, a);
					}
				}

				if (++is->subpq_windex == SUBPICTURE_QUEUE_SIZE) {
					is->subpq_windex = 0;
				}

				is->subpq_size++;
			}
			av_free_packet(pkt);
		}*/
		return NULL;
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
				SWS_BILINEAR,
				0,
				0,
				0);

		return ctx;
	}
}
}
}
}
}
}
