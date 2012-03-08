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

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace mb {

	set<SDL2ffmpeg*> SDL2ffmpeg::instances;
	pthread_mutex_t SDL2ffmpeg::iMutex;
	bool SDL2ffmpeg::init = false;

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
		error_recognition                    = FF_ER_CAREFUL;
		error_concealment                    = 3;
		decoder_reorder_pts                  = -1;
		framedrop                            = -1;
		rdftspeed                            = 20;
		texture                              = NULL;
		hasPic                               = false;
		state                                = ST_STOPPED;
		fixedSize                            = 0;

		setSoundLevel(0.5);

		if (!init) {
			init = true;
			pthread_mutex_init(&iMutex, NULL);
		}

		memset(&flush_pkt, 0, sizeof(flush_pkt));
		memset(&vPkt, 0, sizeof(vPkt));

		is = (VideoState*)av_mallocz(sizeof(VideoState));
		if (is) {
			av_strlcpy(is->filename, filename, sizeof(is->filename));

			pthread_mutex_lock(&iMutex);
			if (instances.empty()) {
				av_log_set_flags(AV_LOG_SKIP_REPEATED);

				/* register all codecs, demux and protocols */
				avcodec_register_all();
				av_register_all();
				avformat_network_init();

				if (av_lockmgr_register(lockmgr)) {
					fprintf(stderr, "Could not initialize lock manager!\n");
					release();
				}
			}
			pthread_mutex_unlock(&iMutex);

			av_init_packet(&flush_pkt);
			flush_pkt.data = (uint8_t*)"FLUSH";

			memset(st_index, -1, sizeof(st_index));
			is->video_stream    = -1;
			is->audio_stream    = -1;
			is->subtitle_stream = -1;

			is->ic = avformat_alloc_context();
			is->ic->interrupt_callback.callback =
					SDL2ffmpeg::decode_interrupt_cb;

			is->ic->interrupt_callback.opaque = this;

			err = avformat_open_input(&is->ic, is->filename, NULL, NULL);
			if (err >= 0) {
				if (genpts) {
					is->ic->flags |= AVFMT_FLAG_GENPTS;
				}

				err = avformat_find_stream_info(is->ic, NULL);
				if (err >= 0) {
					if (is->ic->pb) {
						is->ic->pb->eof_reached = 0;
					}

					if (seek_by_bytes < 0) {
						seek_by_bytes= !!(
								is->ic->iformat->flags & AVFMT_TS_DISCONT);
					}

					for (i = 0; i < is->ic->nb_streams; i++) {
						is->ic->streams[i]->discard = AVDISCARD_ALL;
					}

					st_index[AVMEDIA_TYPE_VIDEO] = av_find_best_stream(
							is->ic,
							AVMEDIA_TYPE_VIDEO,
							wanted_stream[AVMEDIA_TYPE_VIDEO],
							-1,
							NULL,
							0);

					st_index[AVMEDIA_TYPE_AUDIO] = av_find_best_stream(
							is->ic,
							AVMEDIA_TYPE_AUDIO,
							wanted_stream[AVMEDIA_TYPE_AUDIO],
							st_index[AVMEDIA_TYPE_VIDEO],
							NULL,
							0);

					st_index[AVMEDIA_TYPE_SUBTITLE] = av_find_best_stream(
							is->ic,
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
						cout << "SDL2ffmpeg::SDL2ffmpeg ";
						cout << "Can't select any audio stream in mrl '";
						cout << is->filename << "'" << endl;
					}

					if (st_index[AVMEDIA_TYPE_VIDEO] >= 0) {
						stream_component_open(st_index[AVMEDIA_TYPE_VIDEO]);

					} else {
						cout << "SDL2ffmpeg::SDL2ffmpeg ";
						cout << "Can't select any video stream in mrl '";
						cout << is->filename << "'" << endl;
					}

					if (st_index[AVMEDIA_TYPE_SUBTITLE] >= 0) {
						stream_component_open(st_index[AVMEDIA_TYPE_SUBTITLE]);

					} else {
						cout << "SDL2ffmpeg::SDL2ffmpeg ";
						cout << "Can't select any subtitle stream in mrl '";
						cout << is->filename << "'" << endl;
					}

					is->av_sync_type = av_sync_type;

				} else {
					cout << "SDL2ffmpeg::SDL2ffmpeg ";
					cout << "Can't find stream info for '" << filename;
					cout << "'" << endl;
				}

			} else {
				cout << "SDL2ffmpeg::SDL2ffmpeg ";
				cout << "Can't open '" << filename << "'" << endl;
			}
		}
	}

	SDL2ffmpeg::~SDL2ffmpeg() {
		set<SDL2ffmpeg*>::iterator i;

		release();

		if (texture != NULL) {
			SDL_DestroyTexture(texture);
			texture = NULL;
		}

		pthread_mutex_lock(&iMutex);
		i = instances.find(this);
		if (i != instances.end()) {
			instances.erase(i);
		}

		if (instances.empty()) {
			av_lockmgr_register(NULL);
			avformat_network_deinit();
			avio_set_interrupt_cb(NULL);
		}
		pthread_mutex_unlock(&iMutex);
	}

	void SDL2ffmpeg::release() {
		if (is != NULL) {
			if (is->ic) {
				avformat_close_input(&is->ic);
				is->ic = NULL; /* safety */
			}

			stream_close();
		}
	}

	bool SDL2ffmpeg::prepare() {
		SDL_AudioSpec obtained;
		int64_t wanted_channel_layout;

		if (is->audio_stream >= 0) {
/*			wanted_spec.format   = AUDIO_S16SYS;
			wanted_spec.silence  = 0;
			wanted_spec.samples  = SDL_AUDIO_BUFFER_SIZE;
			wanted_spec.callback = SDL2ffmpeg::sdl_audio_callback;
			wanted_spec.userdata = this;
*/

			wanted_channel_layout = is->audio_src_channel_layout;

			getAudioSpec(&spec, audioFreq, audioChannels);

			if (SDL_OpenAudio(&spec, &obtained) < 0) {
				memcpy(&spec, &obtained, sizeof(spec));

			} else {
				fixedSize = obtained.size;

				if (fixedSize == 0) {
					fixedSize = obtained.channels * obtained.samples * 2;
				}
			}

			if (obtained.channels == 0) {
				memcpy(&obtained, &spec, sizeof(spec));
				spec.size = fixedSize;

			} else {
				fixedSize = obtained.size;

				if (fixedSize == 0) {
					fixedSize = obtained.channels * obtained.samples * 2;
				}
			}

			if (spec.format != AUDIO_S16SYS) {
				cout << "SDL2ffmpeg::stream_component_open Warning! ";
				cout << "audio format '" << spec.format << "' not ";
				cout << "supported by SDL!";
				cout << endl;

				return false;
			}

			//is->audio_hw_buf_size = spec.channels * spec.samples * 2;
			is->audio_hw_buf_size = spec.size;

			is->audio_src_fmt = is->audio_tgt_fmt = AV_SAMPLE_FMT_S16;
			is->audio_src_freq = is->audio_tgt_freq = spec.freq;
			is->audio_src_channels = is->audio_tgt_channels = spec.channels;

			is->audio_diff_threshold = (
					2.0 * SDL_AUDIO_BUFFER_SIZE / spec.freq);

			/* init averaging filter */
			is->audio_diff_avg_coef  = exp(log(0.01) / AUDIO_DIFF_AVG_NB);
			is->audio_diff_avg_count = 0;
			/* since we do not have a precise anough audio fifo fullness,
			   we correct audio sync only if larger than this threshold */
			is->audio_src_channel_layout = is->audio_tgt_channel_layout =
					wanted_channel_layout;
		}

		return true;
	}

	void SDL2ffmpeg::play() {
		if (state == ST_STOPPED) {
			prepare();
			state = ST_PLAYING;
			pthread_mutex_lock(&iMutex);
			SDL_PauseAudio(0);
			instances.insert(this);
			pthread_mutex_unlock(&iMutex);
		}
	}

	void SDL2ffmpeg::stop() {
		is->abort_request           = 1;
		is->videoq.abort_request    = 1;
		is->audioq.abort_request    = 1;
		is->subtitleq.abort_request = 1;

		state = ST_STOPPED;
		pthread_mutex_lock(&iMutex);
		if (instances.size() == 1) {
			SDL_PauseAudio(0);
		}
		pthread_mutex_unlock(&iMutex);
	}

	void SDL2ffmpeg::pause() {
		if (state == ST_PLAYING) {
			state = ST_PAUSED;
			if (!is->paused) {
				toggle_pause();
			}
		}
	}

	void SDL2ffmpeg::resume() {
		if (state == ST_PAUSED) {
			state = ST_PLAYING;
			if (is->paused) {
				toggle_pause();
			}
		}
	}

	void SDL2ffmpeg::getOriginalResolution(int* w, int* h) {
		if (is->video_st && is->video_st->codec->width){
			*w = is->video_st->codec->width;
			*h = is->video_st->codec->height;

		} else {
			*w = 0;
			*h = 0;
		}
	}

	double SDL2ffmpeg::getDuration() {
		return is->ic->duration/1000LL;
	}

	double SDL2ffmpeg::getPosition() {
		if (is->audio_stream >= 0 && is->audio_st) {
			return is->audio_current_pts;

		} else if (is->video_stream >= 0 && is->video_st) {
			return is->video_current_pts;
		}

		return 0.0;
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
		if (hasPic) {
			hasPic = false;
			return true;
		}

		return false;
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

		if (!q->last_pkt) {
			q->first_pkt = pkt1;

		} else {
			q->last_pkt->next = pkt1;
		}

		q->last_pkt = pkt1;
		q->nb_packets++;
		q->size += pkt1->pkt.size + sizeof(*pkt1);

		/* XXX: should duplicate packet data in DV case */
		return 0;
	}

	void SDL2ffmpeg::packet_queue_init(PacketQueue *q) {
		memset(q, 0, sizeof(PacketQueue));

		packet_queue_put(q, &flush_pkt);
	}

	void SDL2ffmpeg::packet_queue_flush(PacketQueue *q) {
		AVPacketList* pkt;
		AVPacketList* pkt1;

		for (pkt = q->first_pkt; pkt != NULL; pkt = pkt1) {
			pkt1 = pkt->next;
			av_free_packet(&pkt->pkt);
			av_freep(&pkt);
		}

		q->last_pkt   = NULL;
		q->first_pkt  = NULL;
		q->nb_packets = 0;
		q->size       = 0;
	}

	void SDL2ffmpeg::packet_queue_end(PacketQueue *q) {
		packet_queue_flush(q);
	}

	void SDL2ffmpeg::packet_queue_abort(PacketQueue *q) {
		q->abort_request = 1;
	}

	/* return < 0 if aborted, 0 if no packet and > 0 if packet.  */
	int SDL2ffmpeg::packet_queue_get(PacketQueue *q, AVPacket *pkt, int block) {
		AVPacketList *pkt1;
		int ret;

		if (q->abort_request) {
			ret = -1;

		} else {
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

			} else if (!block) {
				ret = 0;
			}
		}

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

		vp = &is->pictq[is->pictq_windex];

		vp->width     = is->video_st->codec->width;
		vp->height    = is->video_st->codec->height;
		vp->pix_fmt   = is->video_st->codec->pix_fmt;
		vp->tex       = texture;

		vp->allocated = 1;
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

		vp = &is->pictq[is->pictq_rindex];
		if (vp->tex) {
			/* XXX: use variable in the frame */
			if (is->video_st->sample_aspect_ratio.num) {
				aspect_ratio = av_q2d(is->video_st->sample_aspect_ratio);

			} else if (is->video_st->codec->sample_aspect_ratio.num) {
				aspect_ratio = av_q2d(is->video_st->codec->sample_aspect_ratio);

			} else {
				aspect_ratio = 0;
			}

			if (aspect_ratio <= 0.0) {
				aspect_ratio = 1.0;
			}

			aspect_ratio *= (float)vp->width / (float)vp->height;

			if (is->subtitle_st) {
				if (is->subpq_size > 0) {
					sp = &is->subpq[is->subpq_rindex];

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
			is->no_background = 0;
			rect.x = x;
			rect.y = y;
			rect.w = FFMAX(width,  1);
			rect.h = FFMAX(height, 1);

			if (vp->tex) {
				if (is->img_convert_ctx == NULL) {
					is->img_convert_ctx = createContext(
							vp->width,
							vp->height,
							vp->pix_fmt,
							vp->width,
							vp->height,
							PIX_FMT_RGB24);
				}

				if (is->img_convert_ctx == NULL) {
					fprintf(stderr, "Can't initialize the conversion context\n");
					return;
				}

				if (vp->src_frame) {
					uint8_t* pixels[AV_NUM_DATA_POINTERS];
					int pitch[AV_NUM_DATA_POINTERS];

					AVPicture pict;

					SDL_LockTexture(vp->tex, NULL, (void**)&pixels, &pitch[0]);

					sws_scale(
							is->img_convert_ctx,
							(const uint8_t* const*)vp->src_frame->data,
							vp->src_frame->linesize,
							0, vp->height, pixels, pitch);

					SDL_UnlockTexture(vp->tex);
					hasPic = true;
				}
			}
		}
	}

	void SDL2ffmpeg::stream_close() {
		VideoPicture *vp;
		int i;

		is->abort_request = 1;

		for (i=0;i<VIDEO_PICTURE_QUEUE_SIZE; i++) {
			vp = &is->pictq[i];

			if (vp->tex) {
				vp->tex = NULL;
			}
		}

		if (is->img_convert_ctx) {
			sws_freeContext(is->img_convert_ctx);
		}

		av_free(is);
	}

	/* display the current picture, if any */
	void SDL2ffmpeg::video_display() {
		if (is->video_st) {
			video_image_display();
		}
	}

	/* get the current audio clock value */
	double SDL2ffmpeg::get_audio_clock() {
		if (!is->paused) {
			return is->audio_current_pts_drift + av_gettime() / 1000000.0;

		} else {
			return is->audio_current_pts;
		}
	}

	/* get the current video clock value */
	double SDL2ffmpeg::get_video_clock() {
		if (!is->paused) {
			return is->video_current_pts_drift + av_gettime() / 1000000.0;

		} else {
			return is->video_current_pts;
		}
	}

	/* get the current external clock value */
	double SDL2ffmpeg::get_external_clock() {
		int64_t ti;
		ti = av_gettime();
		return is->external_clock + ((ti - is->external_clock_time) * 1e-6);
	}

	/* get the current master clock value */
	double SDL2ffmpeg::get_master_clock() {
		double val;

		if (is->av_sync_type == AV_SYNC_AUDIO_MASTER) {
			if (is->audio_st) {
				val = get_audio_clock();

			} else {
				val = get_video_clock();
			}

		} else 	if (is->av_sync_type == AV_SYNC_VIDEO_MASTER) {
			if (is->video_st) {
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
		if (!is->seek_req) {
			is->seek_pos = pos;
			is->seek_rel = rel;
			is->seek_flags &= ~AVSEEK_FLAG_BYTE;
			if (seek_by_bytes) {
				is->seek_flags |= AVSEEK_FLAG_BYTE;
			}
			is->seek_req = 1;
		}
	}

	/* pause or resume the video */
	void SDL2ffmpeg::stream_toggle_pause() {
		if (is->paused) {
			is->frame_timer += av_gettime() / 1000000.0 +
					is->video_current_pts_drift - is->video_current_pts;

			if (is->read_pause_return != AVERROR(ENOSYS)) {
				is->video_current_pts = is->video_current_pts_drift +
						av_gettime() / 1000000.0;
			}

			is->video_current_pts_drift = is->video_current_pts -
					av_gettime() / 1000000.0;
		}

		is->paused = !is->paused;
	}

	double SDL2ffmpeg::compute_target_delay(double delay) {
		double sync_threshold, diff;

		/* update delay to follow master synchronisation source */
		if (((is->av_sync_type == AV_SYNC_AUDIO_MASTER && is->audio_st) ||
				is->av_sync_type == AV_SYNC_EXTERNAL_CLOCK)) {

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
		if (++is->pictq_rindex == VIDEO_PICTURE_QUEUE_SIZE) {
			is->pictq_rindex = 0;
		}

		is->pictq_size--;
	}

	void SDL2ffmpeg::update_video_pts(double pts, int64_t pos) {
		double time = av_gettime() / 1000000.0;
		/* update current video pts */
		is->video_current_pts = pts;
		is->video_current_pts_drift = is->video_current_pts - time;
		is->video_current_pos = pos;
		is->frame_last_pts = pts;
	}

	int SDL2ffmpeg::queue_picture(
			AVFrame *src_frame, double pts1, int64_t pos) {

		VideoPicture *vp;
		double frame_delay, pts = pts1;

		if (is->videoq.abort_request) {
			return -1;
		}

		/* compute the exact PTS for the picture if it is omitted in the stream
		 * pts1 is the dts of the pkt / pts of the frame */
		if (pts != 0) {
			/* update video clock with pts, if present */
			is->video_clock = pts;

		} else {
			pts = is->video_clock;
		}

		/* update video clock for next frame */
		frame_delay = av_q2d(is->video_st->codec->time_base);
		/* for MPEG2, the frame can be repeated, so we update the
		   clock accordingly */
		frame_delay += src_frame->repeat_pict * (frame_delay * 0.5);
		is->video_clock += frame_delay;

		vp = &is->pictq[is->pictq_windex];

		vp->duration = frame_delay;

		/* alloc or resize hardware picture buffer */
		if (!vp->tex || vp->reallocate ||
				vp->width != is->video_st->codec->width ||
				vp->height != is->video_st->codec->height) {

			vp->allocated  = 0;
			vp->reallocate = 0;

			alloc_picture();
			if (is->videoq.abort_request) {
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
			if (++is->pictq_windex == VIDEO_PICTURE_QUEUE_SIZE) {
				is->pictq_windex = 0;
			}

			is->pictq_size++;
		}
		return 0;
	}

	int SDL2ffmpeg::get_video_frame(
			AVFrame *frame, int64_t *pts, AVPacket *pkt) {

		int got_picture, i;

		if (is->pictq_size) {
			return -1;
		}

		if (packet_queue_get(&is->videoq, pkt, 0) < 0) {
			return -1;
		}

		if (pkt->data == flush_pkt.data) {
			avcodec_flush_buffers(is->video_st->codec);

			/*
			 * Make sure there are no long delay timers
			 * (ideally we should just flush the que but thats harder)
			 */
			for (i = 0; i < VIDEO_PICTURE_QUEUE_SIZE; i++) {
				is->pictq[i].skip = 1;
			}

			is->video_current_pos = -1;
			is->frame_last_pts = AV_NOPTS_VALUE;
			is->frame_last_duration = 0;
			is->frame_timer = (double)av_gettime() / 1000000.0;
			is->frame_last_dropped_pts = AV_NOPTS_VALUE;

			return -1;
		}

		avcodec_decode_video2(is->video_st->codec, frame, &got_picture, pkt);

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

			if (((is->av_sync_type == AV_SYNC_AUDIO_MASTER && is->audio_st) ||
					is->av_sync_type == AV_SYNC_EXTERNAL_CLOCK) &&
					(framedrop > 0 || (framedrop && is->audio_st))) {

				if (is->frame_last_pts != AV_NOPTS_VALUE && *pts) {
					double clockdiff = get_video_clock() - get_master_clock();
					double dpts      = av_q2d(is->video_st->time_base) * *pts;
					double ptsdiff   = dpts - is->frame_last_pts;
					if (fabs(clockdiff) < AV_NOSYNC_THRESHOLD &&
							ptsdiff > 0 && ptsdiff < AV_NOSYNC_THRESHOLD &&
							clockdiff + ptsdiff -
									is->frame_last_filter_delay < 0) {

						is->frame_last_dropped_pos = pkt->pos;
						is->frame_last_dropped_pts = dpts;
						is->frame_drops_early++;
						ret = 0;
					}
				}
			}

			if (ret) {
				is->frame_last_returned_time = av_gettime() / 1000000.0;
			}

			return ret;
		}

		return -1;
	}

	/* return the new audio buffer size (samples can be added or deleted
	   to get better sync if video or external master clock) */
	int SDL2ffmpeg::synchronize_audio(
			short *samples, int samples_size1, double pts) {

		int n, samples_size;
		double ref_clock;

		n = av_get_bytes_per_sample(is->audio_tgt_fmt) * is->audio_tgt_channels;
		samples_size = samples_size1;

		/*
		 * if not master,
		 * then we try to remove or add samples to correct the clock
		 */
		if (((is->av_sync_type == AV_SYNC_VIDEO_MASTER && is->video_st) ||
				is->av_sync_type == AV_SYNC_EXTERNAL_CLOCK)) {

			double diff, avg_diff;
			int wanted_size, min_size, max_size, nb_samples;

			ref_clock = get_master_clock();
			diff = get_audio_clock() - ref_clock;

			if (diff < AV_NOSYNC_THRESHOLD) {
				is->audio_diff_cum = diff + is->audio_diff_avg_coef *
						is->audio_diff_cum;

				if (is->audio_diff_avg_count < AUDIO_DIFF_AVG_NB) {
					/* not enough measures to have a correct estimate */
					is->audio_diff_avg_count++;

				} else {
					/* estimate the A-V difference */
					avg_diff = is->audio_diff_cum * (
							1.0 - is->audio_diff_avg_coef);

					if (fabs(avg_diff) >= is->audio_diff_threshold) {
						wanted_size = samples_size + ((int)(
								diff * is->audio_tgt_freq) * n);

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
								sizeof(is->audio_buf2))) {

							wanted_size = FFMIN3(
									max_size,
									samples_size,
									sizeof(is->audio_buf2));
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
				is->audio_diff_avg_count = 0;
				is->audio_diff_cum = 0;
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

		if (is->audio_stream < 0 || !is->audio_st) {
			return -1;
		}

		pkt_temp  = &is->audio_pkt_temp;
		pkt       = &is->audio_pkt;
		dec       = is->audio_st->codec;
		data_size = -1;

		/* NOTE: the audio packet can contain several frames */
		while (pkt_temp->size > 0 || (!pkt_temp->data && new_packet)) {
			if (!is->frame) {

				if (!(is->frame = avcodec_alloc_frame())) {
					return AVERROR(ENOMEM);
				}

			} else {
				avcodec_get_frame_defaults(is->frame);
			}

			if (flush_complete) {
				break;
			}

			new_packet = 0;
			len1 = avcodec_decode_audio4(
					dec, is->frame, &got_frame, pkt_temp);

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
						dec->codec->capabilities & CODEC_CAP_DELAY) {

					flush_complete = 1;
				}
				continue;
			}

			data_size = av_samples_get_buffer_size(
					NULL,
					dec->channels,
					is->frame->nb_samples,
					dec->sample_fmt, 1);

			dec_channel_layout = (dec->channel_layout &&
					dec->channels == av_get_channel_layout_nb_channels(
							dec->channel_layout)) ? dec->channel_layout :
									av_get_default_channel_layout(
											dec->channels);

			if (dec->sample_fmt != is->audio_src_fmt ||
					dec_channel_layout != is->audio_src_channel_layout ||
					dec->sample_rate != is->audio_src_freq) {

				if (is->swr_ctx) {
					swr_free(&is->swr_ctx);
				}

				is->swr_ctx = swr_alloc_set_opts(
						NULL,
						is->audio_tgt_channel_layout,
						is->audio_tgt_fmt,
						is->audio_tgt_freq,
						dec_channel_layout,
						dec->sample_fmt,
						dec->sample_rate,
						0, NULL);

				if (!is->swr_ctx || swr_init(is->swr_ctx) < 0) {
					cout << "SDL2ffmpeg::audio_decode_frame " << stderr;
					cout << " Can't create sample rate converter for ";
					cout << dec->sample_rate << "Hz ";
					cout << av_get_sample_fmt_name(dec->sample_fmt);
					cout << " " << dec->channels << " channels to ";
					cout << is->audio_tgt_freq << "Hz ";
					cout << av_get_sample_fmt_name(is->audio_tgt_fmt);
					cout << " " << is->audio_tgt_channels << " channels!";
					cout << endl;
					break;
				}

				is->audio_src_channel_layout = dec_channel_layout;
				is->audio_src_channels       = dec->channels;
				is->audio_src_freq           = dec->sample_rate;
				is->audio_src_fmt            = dec->sample_fmt;
			}

			resampled_data_size = data_size;
			if (is->swr_ctx) {
				const uint8_t *in[] = { is->frame->data[0] };
				uint8_t *out[] = {is->audio_buf2};
				len2 = swr_convert(
						is->swr_ctx,
						out,
						sizeof(is->audio_buf2) /
								is->audio_tgt_channels /
								av_get_bytes_per_sample(is->audio_tgt_fmt),
						in,
						data_size / dec->channels / av_get_bytes_per_sample(
								dec->sample_fmt));

				if (len2 < 0) {
					fprintf(stderr, "audio_resample() failed\n");
					break;
				}

				if (len2 == sizeof(is->audio_buf2) /
						is->audio_tgt_channels /
						av_get_bytes_per_sample(is->audio_tgt_fmt)) {

					fprintf(stderr, "warning: audio buffer is too small\n");
					swr_init(is->swr_ctx);
				}

				is->audio_buf = is->audio_buf2;
				resampled_data_size = len2 * is->audio_tgt_channels *
						av_get_bytes_per_sample(is->audio_tgt_fmt);

			} else {
				is->audio_buf = is->frame->data[0];
			}

			/* if no pts, then compute it */
			pts = is->audio_clock;
			*pts_ptr = pts;
			is->audio_clock += (double)data_size / (dec->channels *
					dec->sample_rate * av_get_bytes_per_sample(
							dec->sample_fmt));

			return resampled_data_size;
		}

		/* free the current packet */
		if (pkt->data) {
			av_free_packet(pkt);
		}

		memset(pkt_temp, 0, sizeof(*pkt_temp));

		if (is->paused || is->audioq.abort_request) {
			return -1;
		}

		/* read next packet */
		if ((new_packet = packet_queue_get(&is->audioq, pkt, 0)) < 0) {
			return -1;
		}

		if (pkt->data == flush_pkt.data) {
			avcodec_flush_buffers(dec);
		}

		*pkt_temp = *pkt;

		if (pkt->pts != AV_NOPTS_VALUE) {
			is->audio_clock = av_q2d(is->audio_st->time_base)*pkt->pts;
		}

		return data_size;
	}

	/* open a given stream. Return 0 if OK */
	int SDL2ffmpeg::stream_component_open(int stream_index) {
		AVFormatContext *ic = is->ic;
		AVCodecContext *avctx;
		AVCodec *codec;

		int64_t wanted_channel_layout = 0;

		if (stream_index < 0 || stream_index >= ic->nb_streams) {
			cout << "SDL2ffmpeg::stream_component_open ";
			cout << "invalid index";
			cout << endl;
			return -1;
		}

		avctx = ic->streams[stream_index]->codec;

		codec = avcodec_find_decoder(avctx->codec_id);
		if (!codec) {
			cout << "SDL2ffmpeg::stream_component_open ";
			cout << "Can't find codec for '" << avctx->codec_type << "'";
			cout << endl;
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
		avctx->error_recognition = error_recognition;
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

			is->audio_src_channel_layout = wanted_channel_layout;

			audioChannels = av_get_channel_layout_nb_channels(
					wanted_channel_layout);

			audioFreq = avctx->sample_rate;
			if (audioFreq <= 0 || audioChannels <= 0) {
				cout << "SDL2ffmpeg::stream_component_open ";
				cout << "Invalid sample rate or channel count!";
				cout << endl;

				return -1;
			}
		}

		if (avcodec_open2(avctx, codec, NULL) < 0) {
			cout << "SDL2ffmpeg::stream_component_open ";
			cout << "can't open codec '" << codec->name << "'";
			cout << endl;
			return -1;
		}

		ic->streams[stream_index]->discard = AVDISCARD_DEFAULT;
		switch(avctx->codec_type) {
			case AVMEDIA_TYPE_AUDIO:
				is->audio_stream      = stream_index;
				is->audio_st          = ic->streams[stream_index];
				is->audio_buf_size    = 0;
				is->audio_hw_buf_size = 0;

				memset(&is->audio_pkt, 0, sizeof(is->audio_pkt));

				packet_queue_init(&is->audioq);
				break;

			case AVMEDIA_TYPE_VIDEO:
				is->video_stream = stream_index;
				is->video_st = ic->streams[stream_index];

				packet_queue_init(&is->videoq);
				break;

			case AVMEDIA_TYPE_SUBTITLE:
				is->subtitle_stream = stream_index;
				is->subtitle_st = ic->streams[stream_index];

				packet_queue_init(&is->subtitleq);
				break;

			default:
				break;

		}
		return 0;
	}

	void SDL2ffmpeg::stream_component_close(int stream_index) {
		AVFormatContext *ic = is->ic;
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
				packet_queue_abort(&is->audioq);
				packet_queue_end(&is->audioq);

				if (is->swr_ctx) {
					swr_free(&is->swr_ctx);
				}

				av_free_packet(&is->audio_pkt);
				av_freep(&is->audio_buf1);
				is->audio_buf = NULL;
				av_freep(&is->frame);

				if (is->rdft) {
					av_rdft_end(is->rdft);
					av_freep(&is->rdft_data);
					is->rdft = NULL;
					is->rdft_bits = 0;
				}
				break;

			case AVMEDIA_TYPE_VIDEO:
				packet_queue_abort(&is->videoq);
				packet_queue_end(&is->videoq);
				break;

			case AVMEDIA_TYPE_SUBTITLE:
				packet_queue_abort(&is->subtitleq);

				is->subtitle_stream_changed = 1;

				packet_queue_end(&is->subtitleq);
				break;

			default:
				break;
		}

		ic->streams[stream_index]->discard = AVDISCARD_ALL;

		avcodec_close(avctx);
		switch(avctx->codec_type) {
			case AVMEDIA_TYPE_AUDIO:
				is->audio_st        = NULL;
				is->audio_stream    = -1;
				break;

			case AVMEDIA_TYPE_VIDEO:
				is->video_st        = NULL;
				is->video_stream    = -1;
				break;

			case AVMEDIA_TYPE_SUBTITLE:
				is->subtitle_st     = NULL;
				is->subtitle_stream = -1;
				break;

			default:
				break;

		}

		ic->streams[stream_index]->codec = NULL;
	}

	void SDL2ffmpeg::stream_cycle_channel(int codec_type) {
		AVFormatContext *ic = is->ic;
		int start_index, stream_index;
		AVStream *st;

		if (codec_type == AVMEDIA_TYPE_VIDEO) {
			start_index = is->video_stream;

		} else if (codec_type == AVMEDIA_TYPE_AUDIO) {
			start_index = is->audio_stream;

		} else {
			start_index = is->subtitle_stream;
		}

		if (start_index < (codec_type == AVMEDIA_TYPE_SUBTITLE ? -1 : 0)) {
			return;
		}
		stream_index = start_index;
		for(;;) {
			if (++stream_index >= is->ic->nb_streams) {
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
		is->step = 0;
	}

	void SDL2ffmpeg::step_to_next_frame() {
		/* if the stream is paused unpause it, then step */
		if (is->paused) {
			stream_toggle_pause();
		}
		is->step = 1;
	}

	void SDL2ffmpeg::refresh() {
		read_content();
		if (is->audio_stream >= 0 && is->audio_st != NULL) {
			audio_refresh_decoder();
		}

		if (is->video_stream >= 0 && is->video_st != NULL) {
			video_refresh_decoder();
			video_refresh_content();
		}
		//subtitle_thread();
	}

	int SDL2ffmpeg::audio_refresh_decoder() {
		int audio_size;
		double pts;

		if (is->audio_stream >= 0 && !is->abort_request &&
				is->audio_hw_buf_size != 0) {

			if (is->audio_buf_size != 0) {
				return 0;
			}

			audio_size = audio_decode_frame(&pts);

			if (audio_size < 0) {
				return -1;
			}

			audio_size = synchronize_audio(
					(int16_t *)is->audio_buf + is->audio_buf_size,
					audio_size,
					pts);

			if (audio_size > 0) {
				is->audio_buf_size =+ audio_size;

			} else {
				return -1;
			}
		}

		return 1;
	}

	/* called to display each frame */
	void SDL2ffmpeg::video_refresh_content() {
		VideoPicture *vp;
		SubPicture *sp, *sp2;

		if (is->video_st) {
			if (is->pictq_size == 0) {
				if (is->frame_last_dropped_pts != AV_NOPTS_VALUE &&
						is->frame_last_dropped_pts > is->frame_last_pts) {

					update_video_pts(
							is->frame_last_dropped_pts,
							is->frame_last_dropped_pos);

					is->frame_last_dropped_pts = AV_NOPTS_VALUE;
				}
				//nothing to do, no picture to display in the que

			} else {
				double time, last_duration, duration, delay;
				/* dequeue the picture */
				vp = &is->pictq[is->pictq_rindex];

				if (vp->skip) {
					pictq_next_picture();
					return;
				}

				/* compute nominal last_duration */
				last_duration = vp->pts - is->frame_last_pts;
				if (last_duration > 0 && last_duration < 10.0) {
					/*
					 * if duration of the last frame was sane,
					 * update last_duration in video state
					 */
					is->frame_last_duration = last_duration;
				}
				delay = compute_target_delay(is->frame_last_duration);

				time = av_gettime()/1000000.0;
				if (time < is->frame_timer + delay) {
					return;
				}

				if (delay > 0) {
					is->frame_timer += delay * FFMAX(
							1, floor((time-is->frame_timer) / delay));
				}

				update_video_pts(vp->pts, vp->pos);

				if (is->pictq_size > 1) {
					VideoPicture *nextvp = &is->pictq[(
							is->pictq_rindex+1)%VIDEO_PICTURE_QUEUE_SIZE];

					duration = nextvp->pts - vp->pts;

				} else {
					duration = vp->duration;
				}

				if ((framedrop > 0 ||
						(framedrop && is->audio_st)) &&
						time > is->frame_timer + duration) {

					if (is->pictq_size > 1) {
						is->frame_drops_late++;
						pictq_next_picture();
						return;
					}
				}

				if (is->subtitle_st) {
					if (is->subtitle_stream_changed) {
						while (is->subpq_size) {
							free_subpicture(&is->subpq[is->subpq_rindex]);

							/* update queue size and signal for next picture */
							if (++is->subpq_rindex == SUBPICTURE_QUEUE_SIZE) {
								is->subpq_rindex = 0;
							}

							is->subpq_size--;
						}
						is->subtitle_stream_changed = 0;

					} else {
						if (is->subpq_size > 0) {
							sp = &is->subpq[is->subpq_rindex];

							if (is->subpq_size > 1) {
								sp2 = &is->subpq[(is->subpq_rindex + 1
										) % SUBPICTURE_QUEUE_SIZE];

							} else {
								sp2 = NULL;
							}

							if ((is->video_current_pts > (sp->pts + ((float)
									sp->sub.end_display_time / 1000))) ||
									(sp2 && is->video_current_pts > (sp2->pts +
											((float)sp2->sub.start_display_time
													/ 1000)))) {

								free_subpicture(sp);

								/*
								 * update queue size and signal for
								 * next picture
								 */
								if (++is->subpq_rindex ==
										SUBPICTURE_QUEUE_SIZE) {

									is->subpq_rindex = 0;
								}

								is->subpq_size--;
							}
						}
					}
				}

				/* display picture */
				video_display();
				pictq_next_picture();
			}
		}
	}

	int SDL2ffmpeg::video_refresh_decoder() {
		int64_t pts_int = AV_NOPTS_VALUE, pos = -1;
		double pts;
		int ret;

		if (!is->videoq.abort_request && !is->paused) {
			ret = get_video_frame(&vFrame, &pts_int, &vPkt);
			if (ret < 0) {
				return ret;
			}

			pos = vPkt.pos;
			av_free_packet(&vPkt);

			is->frame_last_filter_delay = av_gettime() /
					1000000.0 - is->frame_last_returned_time;

			if (fabs(is->frame_last_filter_delay) > AV_NOSYNC_THRESHOLD / 10.0){
				is->frame_last_filter_delay = 0;
			}

			pts = pts_int*av_q2d(is->video_st->time_base);

			if (is->step) {
				stream_toggle_pause();
			}

			ret = queue_picture(&vFrame, pts, pos);
			if (ret < 0) {
				return ret;
			}
		}

		return 0;
	}

	int SDL2ffmpeg::read_content() {
		int ret;
		AVPacket pkt1, *pkt = &pkt1;
		int pkt_in_play_range = 0;

		if (is->video_stream < 0 && is->audio_stream < 0) {
			fprintf(stderr, "%s: could not open codecs\n", is->filename);
			ret = -1;
			release();
		}

		if (!is->abort_request) {
			if (is->paused != is->last_paused) {
				is->last_paused = is->paused;
				if (is->paused) {
					is->read_pause_return = av_read_pause(is->ic);

				} else {
					av_read_play(is->ic);
				}
			}

			if (is->paused &&
					(!strcmp(is->ic->iformat->name, "rtsp") ||
							(is->ic->pb &&
									!strncmp(is->filename, "mmsh:", 5)))) {

				return -1;
			}

			if (is->seek_req) {
				int64_t seek_target= is->seek_pos;
				int64_t seek_min= is->seek_rel > 0 ?
						seek_target - is->seek_rel + 2: INT64_MIN;

				int64_t seek_max= is->seek_rel < 0 ?
						seek_target - is->seek_rel - 2: INT64_MAX;

				ret = avformat_seek_file(
						is->ic,
						-1,
						seek_min,
						seek_target,
						seek_max,
						is->seek_flags);

				if (ret < 0) {
					fprintf(
							stderr,
							"%s: error while seeking\n",
							is->ic->filename);

				} else {

					if (is->audio_stream >= 0) {
						packet_queue_flush(&is->audioq);
						packet_queue_put(&is->audioq, &flush_pkt);
					}

					if (is->subtitle_stream >= 0) {
						packet_queue_flush(&is->subtitleq);
						packet_queue_put(&is->subtitleq, &flush_pkt);
					}

					if (is->video_stream >= 0) {
						packet_queue_flush(&is->videoq);
						packet_queue_put(&is->videoq, &flush_pkt);
					}
				}
				is->seek_req = 0;
			}

			/* if the queue are full, no need to read more */
			if (is->audioq.size + is->videoq.size +
					is->subtitleq.size > MAX_QUEUE_SIZE) {

				return -1;
			}

			if ((is->audioq.size > MIN_AUDIOQ_SIZE || is->audio_stream < 0) &&
					(is->videoq.nb_packets > MIN_FRAMES ||
							is->video_stream < 0) &&
					(is->subtitleq.nb_packets > MIN_FRAMES ||
							is->subtitle_stream < 0)) {

				return -1;
			}

			ret = av_read_frame(is->ic, pkt);
			if (ret < 0) {
				if (ret == AVERROR_EOF || url_feof(is->ic->pb)) {
					if (is->video_stream >= 0) {
						av_init_packet(pkt);
						pkt->data=NULL;
						pkt->size=0;
						pkt->stream_index= is->video_stream;
						packet_queue_put(&is->videoq, pkt);
					}

					if (is->audio_stream >= 0 &&
							(is->audio_st->codec->codec->capabilities &
									CODEC_CAP_DELAY)) {

						av_init_packet(pkt);
						pkt->data = NULL;
						pkt->size = 0;
						pkt->stream_index = is->audio_stream;
						packet_queue_put(&is->audioq, pkt);
					}

					if (is->audioq.size + is->videoq.size +
							is->subtitleq.size == 0) {

						/* EOF DETECTED */
					}

					return -1;
				}

				if (is->ic->pb && is->ic->pb->error) {
					return -1;
				}

				return -1;
			}

			/*
			 * check if packet is in play range specified by user,
			 * then queue, otherwise discard
			 */
			pkt_in_play_range = duration == AV_NOPTS_VALUE ||
					(pkt->pts - is->ic->streams[pkt->stream_index]->start_time) *
					av_q2d(is->ic->streams[pkt->stream_index]->time_base) -
					(double)(start_time != AV_NOPTS_VALUE ?
							start_time : 0)/1000000

					<= ((double)duration/1000000);

			if (pkt->stream_index == is->audio_stream && pkt_in_play_range) {
				ret = packet_queue_put(&is->audioq, pkt);

			} else if (pkt->stream_index == is->video_stream &&
					pkt_in_play_range) {

				ret = packet_queue_put(&is->videoq, pkt);

			} else if (pkt->stream_index == is->subtitle_stream &&
					pkt_in_play_range) {

				ret = packet_queue_put(&is->subtitleq, pkt);

			} else {
				av_free_packet(pkt);
				ret = -1;
			}
		}

		return ret;
	}

	bool SDL2ffmpeg::getAudioSpec(
			SDL_AudioSpec* spec, int sample_rate, uint8_t channels) {

		bool audioSpec = false;

		if (sample_rate > 0 && channels > 0) {
			spec->format   = AUDIO_S16SYS;
			spec->samples  = SDL_AUDIO_BUFFER_SIZE;
			spec->silence  = 0;
			spec->userdata = this;
			spec->callback = SDL2ffmpeg::sdl_audio_callback;
			spec->freq     = sample_rate;
			spec->channels = channels;

			audioSpec = true;

		} else {
			cout << "SDL2ffmpeg::getAudioSpec ";
			cout << "invalid parameters: ";
			cout << "spec address = '" << spec << "' ";
			cout << "sample rate = '" << sample_rate << "' ";
			cout << "channels = '" << (short)channels << "' ";
			cout << endl;

			audioSpec = false;
		}

		return audioSpec;
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

		int64_t audio_callback_time;
		int audio_size;
		int bytes_per_sec;
		double pts;

		SDL_AudioCVT acvt;
		SDL_AudioSpec destSpec;
		uint32_t capacity = 0;
		int ret;
		bool cvt = false;

		pthread_mutex_lock(&iMutex);
		audio_callback_time = av_gettime();

		/*i = instances.begin();
		while (i != instances.end()) {
			dec = *i;
			is  = dec->is;

			if (!is->paused) {
				if (capacity == 0) {
					capacity = is->audio_hw_buf_size;
					memcpy(&destSpec, &dec->spec, sizeof(dec->spec));

				} else if (destSpec.channels > dec->spec.channels) {
					if (capacity < is->audio_hw_buf_size) {
						cvt      = true;
						capacity = is->audio_hw_buf_size;
						memcpy(&destSpec, &dec->spec, sizeof(dec->spec));

					} else if (capacity > is->audio_hw_buf_size) {
						cvt = true;
					}
				}
			}

			++i;
		}*/

		memset(stream, 0, len);

		i = instances.begin();
		while (i != instances.end()) {
			ret = 1;
			dec = (*i);
			is  = dec->is;

			if (is->audio_stream >= 0 && dec->state == ST_PLAYING) {
				if (is->audio_buf_size == len) {

					/*
					 * FIXME: mixer and converter work only when all
					 *        sources have the same number of channels.
					 */
					if (cvt && capacity != is->audio_buf_size) {
						ret = SDL_BuildAudioCVT(
								&acvt,
								dec->spec.format,
								dec->spec.channels,
								dec->spec.freq,
								destSpec.format,
								destSpec.channels,
								destSpec.freq);

						if (ret != -1) {
							acvt.len = is->audio_buf_size;
							acvt.buf = (Uint8*)malloc(acvt.len * acvt.len_mult);

							if (acvt.buf != NULL) {
								memset(acvt.buf, 0, acvt.len * acvt.len_mult);

								memcpy(
										acvt.buf,
										is->audio_buf,
										is->audio_buf_size);

								SDL_ConvertAudio(&acvt);

								cout << endl;
								cout << "Converting(stream len = '" << len;
								cout << "' and Audio instance = '";
								cout << dec << "')" << endl;
								cout << "FROM: ";
								cout << "format '" << dec->spec.format;
								cout << "' channels '";
								cout << (short)dec->spec.channels;
								cout << "' freq '" << dec->spec.freq;
								cout << "' size '" << is->audio_buf_size;
								cout << "'";
								cout << "' and capacity '";
								cout << is->audio_hw_buf_size;
								cout << "'";
								cout << endl;
								cout << "TO: ";
								cout << "format '" << destSpec.format << "' ";
								cout << "channels '";
								cout << (short)destSpec.channels;
								cout << "' freq '" << destSpec.freq << "' ";
								cout << "size '";
								cout << acvt.len_cvt;
								cout << "' and capacity '";
								cout << capacity;
								cout << "'";
								cout << endl;

								clamp((short*)acvt.buf, acvt.len_cvt);

								SDL_MixAudioFormat(
										stream,
										acvt.buf,
										acvt.dst_format,
										acvt.len_cvt,
										dec->soundLevel);

								free(acvt.buf);
								acvt.buf = NULL;

								ret = 0;

							} else {
								ret = -2;
							}
						}
					}

					if (ret == 1) {
						SDL_MixAudioFormat(
								stream,
								is->audio_buf,
								dec->spec.format,
								is->audio_buf_size,
								dec->soundLevel);
					}

					bytes_per_sec = is->audio_tgt_freq *
							is->audio_tgt_channels *
							av_get_bytes_per_sample(is->audio_tgt_fmt);

					is->audio_write_buf_size = is->audio_buf_size;

					is->audio_current_pts = is->audio_clock -
							(double)(2 * is->audio_hw_buf_size +
									is->audio_write_buf_size) / bytes_per_sec;

					is->audio_current_pts_drift = is->audio_current_pts -
							audio_callback_time / 1000000.0;

					is->audio_buf_size = 0;

				} else {
					cout << endl;
					cout << "SDL2ffmpeg::sdl_audio_callback ";
					cout << "not this time for " << is->filename;
					cout << " audio buffer size = " << is->audio_buf_size;
					cout << " HW buffer size = " << is->audio_hw_buf_size;
					cout << " len = " << len;
					cout << " samples = " << dec->spec.samples;
					cout << " freq = " << dec->spec.freq;
					cout << " channels = " << (short)dec->spec.channels;
					cout << endl;
				}
			}
			++i;
		}
		pthread_mutex_unlock(&iMutex);

		//::usleep((unsigned int)(30000 - (av_gettime() - audio_callback_time)));
	}

	int SDL2ffmpeg::subtitle_thread(void *arg) {
		SDL2ffmpeg* thiz = (SDL2ffmpeg*)arg;
		VideoState* is   = thiz->is;

		SubPicture *sp;
		AVPacket pkt1, *pkt = &pkt1;
		int got_subtitle;
		double pts;
		int i, j;
		int r, g, b, y, u, v, a;
/*
		for(;;) {
			while (is->paused && !is->subtitleq.abort_request) {
				SDL_Delay(10);
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
		return 0;
	}

	int SDL2ffmpeg::lockmgr(void **mtx, enum AVLockOp op) {
		switch(op) {
			case AV_LOCK_CREATE:
				*mtx = (void*)SDL_CreateMutex();
				if (!*mtx) {
					return 1;
				}

				return 0;

			case AV_LOCK_OBTAIN:
				return !!SDL_LockMutex((SDL_mutex*)*mtx);

			case AV_LOCK_RELEASE:
				return !!SDL_UnlockMutex((SDL_mutex*)*mtx);

			case AV_LOCK_DESTROY:
				SDL_DestroyMutex((SDL_mutex*)*mtx);
				return 0;
		}

		return 1;
	}

	int SDL2ffmpeg::decode_interrupt_cb(void *ctx) {
		SDL2ffmpeg* thiz = (SDL2ffmpeg*)ctx;

		return thiz->is->abort_request;
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
