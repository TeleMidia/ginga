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

#include "config.h"

#include "mb/LocalScreenManager.h"
#include "mb/interface/dfb/DFBDeviceScreen.h"
#include "mb/interface/dfb/content/video/FFmpegVideoProvider.h"
#include "mb/interface/dfb/output/DFBSurface.h"

/* macro for a safe call to DirectFB functions */
#ifndef DFBCHECK
#define DFBCHECK(x...)											\
{																\
	DFBResult err = x;											\
	if (err != DFB_OK) {										\
		fprintf( stderr, "%s <%d>: \n\t", __FILE__, __LINE__ ); \
		DirectFBError( #x, err );                               \
	}															\
}
#endif /*DFBCHECK*/

static int av_read_callback(void *opaque, uint8_t *buf, int size) {
	IDirectFBVideoProvider_FFmpeg_data* data;
	unsigned int						len;
	DFBResult							ret;

	if (!buf || size < 0) {
		return -1;
	}

	data = (IDirectFBVideoProvider_FFmpeg_data*)opaque;
	len  = 0;
	if (size) {
		data->buffer->WaitForData(data->buffer, size);
		ret = data->buffer->GetData(data->buffer, size, buf, &len);
		if (ret && ret != DFB_EOF) {
			return -1;
		}
	}

	return len;
}

static int64_t av_seek_callback(
		void *opaque, int64_t offset, int whence) {

	IDirectFBVideoProvider_FFmpeg_data* data;
	unsigned int						pos;
	DFBResult							ret;

	data = (IDirectFBVideoProvider_FFmpeg_data*)opaque;
	pos  = 0;

	switch (whence) {
		case SEEK_SET:
			ret = data->buffer->SeekTo(data->buffer, offset);
			break;

		case SEEK_CUR:
			ret = data->buffer->GetPosition(data->buffer, &pos);
			if (ret == DFB_OK) {
				if (!offset) {
					return pos;
				}
				ret = data->buffer->SeekTo(data->buffer, pos+offset);
			}
			break;

		case SEEK_END:
			ret = data->buffer->GetLength(data->buffer, &pos);
			if (ret == DFB_OK) {
				ret = data->buffer->SeekTo(data->buffer, pos - offset);
			}
			break;

		default:
			ret = DFB_UNSUPPORTED;
			break;
	}

	if (ret != DFB_OK) {
		return -1;
	}

	data->buffer->GetPosition(data->buffer, &pos);

	return pos;
}

/*****************************************************************************/

static bool put_packet(PacketQueue *queue, AVPacket *packet) {
	PacketLink *p;

	p = new PacketLink;
	av_dup_packet(packet);
	p->packet = *packet;

	pthread_mutex_lock(&queue->lock);
	direct_list_append((DirectLink**)&queue->list, &p->link);
	queue->size += packet->size;
	pthread_mutex_unlock(&queue->lock);

	return true;
}

static bool get_packet(PacketQueue *queue, AVPacket *packet) {
	PacketLink *p = NULL;

	pthread_mutex_lock(&queue->lock);
	p = queue->list;
	if (p != NULL) {
		direct_list_remove((DirectLink**)&queue->list, &p->link);
		queue->size -= p->packet.size;
		*packet = p->packet;
		delete p;
	}
	pthread_mutex_unlock(&queue->lock);

	return (p != NULL);
}

static void flush_packets(PacketQueue *queue) {
	PacketLink* p;
	PacketLink* next;

	for (p = queue->list; p;) {
		next = (PacketLink*)p->link.next;

		direct_list_remove((DirectLink**)&queue->list, &p->link);
		av_free_packet(&p->packet);
		delete p;

		p = next;
	}

	queue->list = NULL;
	queue->size = 0;
}

/*****************************************************************************/

static inline void getclock(struct timespec *ret) {
	struct timeval t;
	gettimeofday(&t, NULL);
	ret->tv_sec  = t.tv_sec;
	ret->tv_nsec = t.tv_usec * 1000;
}

static inline s64 get_stream_clock(IDirectFBVideoProvider_FFmpeg_data *data) {
	if (data->audio.stream && data->audio.pts != -1) {
		int delay = 0;
		data->audio.stream->GetPresentationDelay(data->audio.stream, &delay);
		return data->audio.pts - delay*1000;
	}
	return data->video.pts;
}

/******************************************************************************/

static bool queue_is_full(PacketQueue *queue) {
	PacketLink *first, *last;

	if (!queue->list) {
		return false;
	}

	first = queue->list;
	last  = (PacketLink*)first->link.prev;

	if (last->packet.dts  != (int64_t)AV_NOPTS_VALUE &&
		first->packet.dts != (int64_t)AV_NOPTS_VALUE) {
		if ((last->packet.dts - first->packet.dts) >= queue->max_len) {
			return true;
		}
	}

	return (queue->size >= queue->max_size);
}

static void* FFmpegInput(DirectThread *self, void *arg) {
	IDirectFBVideoProvider_FFmpeg_data* data;

	data = (IDirectFBVideoProvider_FFmpeg_data*)arg;
	if (url_is_streamed(data->context->pb)) {
		data->input.buffering = true;
		pthread_mutex_lock(&data->video.queue.lock);
		pthread_mutex_lock(&data->audio.queue.lock);
	}

	data->audio.pts = -1;

	while (data->status != DVSTATE_STOP) {
		AVPacket packet;
		direct_thread_is_canceled(self);

		pthread_mutex_lock(&data->input.lock);

		if (data->input.seeked) {
			if (av_seek_frame(
					data->context,
					-1,
					data->input.seek_time,
					data->input.seek_flag ) >= 0) {

				pthread_mutex_lock(&data->video.lock);
				pthread_mutex_lock(&data->audio.lock);

				flush_packets(&data->video.queue);
				flush_packets(&data->audio.queue);
				if (!data->input.buffering &&
						url_is_streamed(data->context->pb)) {

					data->input.buffering = true;
					pthread_mutex_lock(&data->video.queue.lock);
					pthread_mutex_lock(&data->audio.queue.lock);
				}

				if (data->status == DVSTATE_FINISHED) {
					data->status = DVSTATE_PLAY;
				}

				data->audio.pts    = -1;
				data->video.seeked = true;
				data->audio.seeked = true;

				if (data->video.thread) {
					pthread_cond_signal(&data->video.cond);
				}

				pthread_mutex_unlock(&data->audio.lock);
				pthread_mutex_unlock(&data->video.lock);
			}
			data->input.seeked = false;
		}

		if (queue_is_full(&data->video.queue) ||
				queue_is_full(&data->audio.queue )) {

			if (data->input.buffering) {
				pthread_mutex_unlock(&data->audio.queue.lock);
				pthread_mutex_unlock(&data->video.queue.lock);
				data->input.buffering = false;
			}
			pthread_mutex_unlock(&data->input.lock);
			::usleep(20000);
			continue;

		} else if (data->video.queue.size == 0 ||
				data->audio.queue.size == 0) {

			if (!data->input.buffering &&
					url_is_streamed(data->context->pb)) {

				data->input.buffering = true;
				pthread_mutex_lock(&data->video.queue.lock);
				pthread_mutex_lock(&data->audio.queue.lock);
			}
		}

		if (av_read_frame(data->context, &packet) < 0) {
			if (url_feof(data->context->pb)) {
				if (data->input.buffering) {
					pthread_mutex_unlock(&data->audio.queue.lock);
					pthread_mutex_unlock(&data->video.queue.lock);
					data->input.buffering = false;
				}
				if (data->video.queue.size == 0 &&
					data->audio.queue.size == 0) {
					if (data->flags & DVPLAY_LOOPING) {
						data->input.seek_time = 0;
						data->input.seek_flag = 0;
						data->input.seeked	= true;

					} else {
						data->status = DVSTATE_FINISHED;
					}
				}
			}
			pthread_mutex_unlock(&data->input.lock);
			usleep(100);
			continue;
		}

		if (packet.stream_index == data->video.st->index) {
			put_packet(&data->video.queue, &packet);

		} else if (data->audio.stream &&
				packet.stream_index == data->audio.st->index) {

			put_packet(&data->audio.queue, &packet);

		} else {
			av_free_packet(&packet);
		}

		pthread_mutex_unlock(&data->input.lock);
	}

	if (data->input.buffering) {
		pthread_mutex_unlock(&data->audio.queue.lock);
		pthread_mutex_unlock(&data->video.queue.lock);
		data->input.buffering = false;
	}

	return NULL;
}

static inline DVCPixelFormat ff2dvc_pixelformat(int pix_fmt) {
	switch (pix_fmt) {
		case PIX_FMT_YUV420P:
		case PIX_FMT_YUVJ420P:
			return DVCPF_YUV420;

		case PIX_FMT_YUV422P:
		case PIX_FMT_YUVJ422P:
			return DVCPF_YUV422;

		case PIX_FMT_YUV444P:
		case PIX_FMT_YUVJ444P:
			return DVCPF_YUV444;

		case PIX_FMT_YUV411P:
			return DVCPF_YUV411;

		case PIX_FMT_YUV410P:
			return DVCPF_YUV410;

		case PIX_FMT_YUYV422:
			return DVCPF_YUYV_LE;

		case PIX_FMT_UYVY422:
			return DVCPF_YUYV_BE;

		case PIX_FMT_NV12:
			return DVCPF_NV12_LE;

		case PIX_FMT_NV21:
			return DVCPF_NV12_BE;

		case PIX_FMT_GRAY8:
			return DVCPF_Y8;

		case PIX_FMT_RGB8:
			return DVCPF_RGB8;

		case PIX_FMT_RGB555:
			return DVCPF_RGB15;

		case PIX_FMT_RGB565:
			return DVCPF_RGB16;

		case PIX_FMT_RGB24:
			return DVCPF_RGB24;

		case PIX_FMT_BGR24:
			return DVCPF_BGR24;

		case PIX_FMT_RGB32:
			return DVCPF_RGB32;

		case PIX_FMT_BGR32:
			return DVCPF_BGR32;

		default:
			clog << "unsupported picture format" << endl;
			break;
	}

	return DVCPF_UNKNOWN;
}

static void FFmpegPutFrame(IDirectFBVideoProvider_FFmpeg_data *data) {
	AVFrame	*src_frame = data->video.src_frame;
	DVCPicture  picture;

	picture.format   = ff2dvc_pixelformat(data->video.ctx->pix_fmt);
	picture.width    = data->video.ctx->width;
	picture.height   = data->video.ctx->height;

	picture.base[0]  = src_frame->data[0];
	picture.base[1]  = src_frame->data[1];
	picture.base[2]  = src_frame->data[2];
	picture.pitch[0] = src_frame->linesize[0];
	picture.pitch[1] = src_frame->linesize[1];
	picture.pitch[2] = src_frame->linesize[2];

	picture.palette      = NULL;
	picture.palette_size = 0;

	picture.separated     = false;
	picture.premultiplied = false;

	dvc_scale_to_surface(&picture, data->video.dest,
						data->video.rect.w ? &data->video.rect : NULL,
						data->video.colormap);
}

static void* FFmpegVideo(DirectThread *self, void *arg) {
	IDirectFBVideoProvider_FFmpeg_data *data;
	data = (IDirectFBVideoProvider_FFmpeg_data*)arg;

	AVStream* st          = data->video.st;
	s64	firtspts          = 0;
	unsigned int framecnt = 0;
	long duration         = 1000000.0/data->video.rate;
	int	drop              = 0;

	AVPacket pkt;
	struct timespec time, now;
	int done;

	while (data->status != DVSTATE_STOP) {
		done = 0;

		getclock(&time);

		direct_thread_testcancel(self);

		pthread_mutex_lock(&data->video.lock);

		if (data->input.buffering ||
				!get_packet(&data->video.queue, &pkt )) {

			pthread_mutex_unlock(&data->video.lock);
			::usleep(100);
			continue;
		}

		if (data->video.seeked) {
			avcodec_flush_buffers(data->video.ctx);
			data->video.seeked = false;
			framecnt = 0;
		}

		/*avcodec_decode_video(
				data->video.ctx,
				data->video.src_frame,
				&done, pkt.data, pkt.size);*/

		avcodec_decode_video2(
				data->video.ctx,
				data->video.src_frame,
				&done, &pkt);

		if (done && !drop) {
			FFmpegPutFrame(data);

			if (data->callback)
					data->callback(data->ctx);
		}

		if (pkt.dts != (int64_t)AV_NOPTS_VALUE) {
			data->video.pts = av_rescale_q(
					pkt.dts, st->time_base, AV_TIME_BASE_Q);

		} else {
			data->video.pts += duration;
		}

		av_free_packet(&pkt);

		if (!data->speed) {
			/* paused */
			pthread_cond_wait(&data->video.cond, &data->video.lock);

		} else {
			long length, delay;

			if (framecnt) {
				duration = (data->video.pts - firtspts) / framecnt;
			}

			length = (double)duration;
			if (data->speed != 1.0) {
				length = (double)duration / data->speed;
			}

			delay = data->video.pts - get_stream_clock(data);
			if (delay > -GAP_THRESHOLD && delay < +GAP_THRESHOLD) {
				delay = CLAMP(delay, -GAP_TOLERANCE, +GAP_TOLERANCE);
			}

			length += delay;

			time.tv_nsec += (length%1000000) * 1000;
			time.tv_sec  += (length/1000000);
			time.tv_sec  += (time.tv_nsec/1000000000);
			time.tv_nsec %= 1000000000;

			getclock( &now );
			if (time.tv_sec > now.tv_sec ||
					(time.tv_sec == now.tv_sec && time.tv_nsec > now.tv_nsec)) {
/*
				pthread_cond_timedwait(
						&data->video.cond, &data->video.lock, &time);
*/
				::usleep(1000);
				drop = false;

			} else {
				delay = (now.tv_sec - time.tv_sec ) * 1000000 +
						(now.tv_nsec - time.tv_nsec) / 1000;

				drop = (delay >= duration);
			}
		}

		pthread_mutex_unlock(&data->video.lock);

		if (framecnt++ == 0) {
			firtspts = data->video.pts;
		}
	}

	return NULL;
}

static void* FFmpegAudio(DirectThread *self, void *arg) {
	IDirectFBVideoProvider_FFmpeg_data *data;
	AVPacket  pkt;
	u8	*pkt_data;
	int	pkt_size;
	int	decoded;
	int	len;
	int	size;

	data = (IDirectFBVideoProvider_FFmpeg_data*)arg;
	AVStream *st = data->audio.st;
	u8		buf[AVCODEC_MAX_AUDIO_FRAME_SIZE];

	while (data->status != DVSTATE_STOP) {
		decoded = 0;
		len	    = AVCODEC_MAX_AUDIO_FRAME_SIZE;
		size	= 0;

		direct_thread_testcancel(self);

		pthread_mutex_lock(&data->audio.lock);

		if (!data->speed) {
			pthread_cond_wait(&data->audio.cond, &data->audio.lock);
			pthread_mutex_unlock(&data->audio.lock);
			continue;
		}

		if (data->input.buffering || !get_packet(&data->audio.queue, &pkt)) {
			pthread_mutex_unlock(&data->audio.lock);
			usleep(100);
			continue;
		}

		if (data->audio.seeked) {
			data->audio.stream->Flush(data->audio.stream);
			avcodec_flush_buffers(data->audio.ctx);
			data->audio.seeked = false;
		}

		for (pkt_data = pkt.data, pkt_size = pkt.size; pkt_size > 0;) {
			/*decoded = avcodec_decode_audio2(
					data->audio.ctx,
					(s16*)&buf[size],
					&len,
					pkt_data,
					pkt_size);*/

			if (pkt_size <= 0 || pkt.data == NULL) {
				break;
			}
			decoded = avcodec_decode_audio3(
					data->audio.ctx,
					(s16*)&buf[size],
					&len,
					&pkt);

			if (decoded < 0) {
				break;
			}

			pkt_data += decoded;
			pkt_size -= decoded;
			if (len > 0) {
				size += len;
			}
		}

		size /= data->audio.sample_size;

		if (pkt.pts != (int64_t)AV_NOPTS_VALUE) {
			data->audio.pts = av_rescale_q(
					pkt.pts, st->time_base, AV_TIME_BASE_Q);

		} else if (size && data->audio.pts != -1) {
			data->audio.pts += (s64)size * AV_TIME_BASE /
					data->audio.sample_rate;
		}

		av_free_packet(&pkt);
		pthread_mutex_unlock(&data->audio.lock);

		if (size) {
			data->audio.stream->Write(data->audio.stream, buf, size);
		} else {
			usleep(1000);
		}
	}

	return NULL;
}

/*****************************************************************************/

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace mb {

	bool FFmpegVideoProvider::_ffmpegInitialized = false;

	FFmpegVideoProvider::FFmpegVideoProvider(
			GingaScreenID screenId, const char* mrl) {

		myScreen   = screenId;
		rContainer = new IDirectFBVideoProvider_FFmpeg_data;
		resumePos  = 0;
		startPos   = 0;

		while (!initializeFFmpeg(mrl)) {
			::usleep(1000);
		}
	}

	FFmpegVideoProvider::~FFmpegVideoProvider() {
		if (rContainer->input.thread) {
			direct_thread_cancel(rContainer->input.thread);
			direct_thread_join(rContainer->input.thread);
			direct_thread_destroy(rContainer->input.thread);
		}

		if (rContainer->video.thread) {
			direct_thread_cancel(rContainer->video.thread);
			direct_thread_join(rContainer->video.thread);
			direct_thread_destroy(rContainer->video.thread);
		}

		if (rContainer->audio.thread) {
			direct_thread_cancel(rContainer->audio.thread);
			direct_thread_join(rContainer->audio.thread);
			direct_thread_destroy(rContainer->audio.thread);
		}

		if (rContainer->audio.playback)
			rContainer->audio.playback->Release(rContainer->audio.playback);

		if (rContainer->audio.stream)
			rContainer->audio.stream->Release(rContainer->audio.stream);

/*
		if (rContainer->audio.sound)
			rContainer->audio.sound->Release(rContainer->audio.sound);
*/
		if (rContainer->audio.ctx)
			avcodec_close(rContainer->audio.ctx);

		if (rContainer->video.ctx)
			avcodec_close(rContainer->video.ctx);

		if (rContainer->video.src_frame)
			av_free(rContainer->video.src_frame);

		if (rContainer->video.dest)
			rContainer->video.dest->Release(rContainer->video.dest);

		if (rContainer->video.colormap)
			delete rContainer->video.colormap;

		if (rContainer->context) {
			AVInputFormat *iformat = rContainer->context->iformat;
			if (!(iformat->flags & AVFMT_NOFILE)) {
				iformat->flags |= AVFMT_NOFILE;
				av_close_input_file(rContainer->context);
				iformat->flags ^= AVFMT_NOFILE;

			} else {
				av_close_input_file(rContainer->context);
			}
		}

		if (rContainer->buffer)
			rContainer->buffer->Release(rContainer->buffer);

		if (rContainer->iobuf) {
			delete (char*)(rContainer->iobuf);
		}

		flush_packets(&rContainer->video.queue);
		flush_packets(&rContainer->audio.queue);

		pthread_cond_destroy (&rContainer->audio.cond);
		pthread_cond_destroy (&rContainer->video.cond);
		pthread_mutex_destroy(&rContainer->audio.queue.lock);
		pthread_mutex_destroy(&rContainer->video.queue.lock);
		pthread_mutex_destroy(&rContainer->audio.lock);
		pthread_mutex_destroy(&rContainer->video.lock);
		pthread_mutex_destroy(&rContainer->input.lock);
	}

	bool FFmpegVideoProvider::initializeFFmpeg(const char* mrl) {
		AVProbeData	pd;
		AVInputFormat* fmt;
		unsigned char buf[2048];
		unsigned int len;
		unsigned int i = 0;
		unsigned int peeked = 0;
		DirectResult ret;
		DFBResult retB;

		IDirectFB* dfb = NULL;
		DFBDataBufferDescription desc;

		memset(rContainer, 0, sizeof(*rContainer));

		rContainer->ref         = 1;
		rContainer->status      = DVSTATE_STOP;
		rContainer->buffer      = NULL;
		rContainer->speed       = 1.0;
		rContainer->volume      = 1.0;
		rContainer->brightness  = 0x8000;
		rContainer->contrast    = 0x8000;
		rContainer->saturation  = 0x8000;

		desc.file          = mrl;
		/*
		desc.memory.data   = buf;
		desc.memory.length = sizeof(buf);*/
		desc.flags         = (DFBDataBufferDescriptionFlags)DBDESC_FILE;

		dfb = (IDirectFB*)(LocalScreenManager::getInstance()->getGfxRoot(
				myScreen));

		dfb->CreateDataBuffer(dfb, &desc, &rContainer->buffer);

		retB = rContainer->buffer->WaitForData(rContainer->buffer, sizeof(buf));
		if (retB == DFB_OK) {
			while (peeked < sizeof(buf)) {
				rContainer->buffer->PeekData(
						rContainer->buffer,
						sizeof(buf),
						peeked,
						&buf[0],
						&len);

				peeked = peeked + len;
			}

		} else {
			/*clog << "FFmpegVideoProvider::initializeFFmpeg ";
			clog << "Warning! can't peek '" << sizeof(buf) << "' bytes from '";
			clog << mrl << "'";
			clog << endl;*/

			rContainer->buffer->Release(rContainer->buffer);
			return false;
		}

		if (!_ffmpegInitialized) {
			_ffmpegInitialized = true;
			av_register_all();
		}

		pd.filename = mrl;
		pd.buf      = &buf[0];
		pd.buf_size = peeked;

		fmt = av_probe_input_format(&pd, 1);
		if (fmt == NULL) {
			/*clog << "FFmpegVideoProvider::initializeFFmpeg ";
			clog << "Warning! no format found";
			clog << endl;*/

			rContainer->buffer->Release(rContainer->buffer);
			return false;
		}

		rContainer->seekable = (
				rContainer->buffer->SeekTo(rContainer->buffer, 0) == DFB_OK);

		rContainer->iobuf = new char[IO_BUFFER_SIZE * 1024];
		if (!rContainer->iobuf) {
			/*clog << "FFmpegVideoProvider::initializeFFmpeg ";
			clog << "can't create io buffer (size = " << IO_BUFFER_SIZE * 1024;
			clog << ")" << endl;*/

			rContainer->buffer->Release(rContainer->buffer);
			delete (char*)(rContainer->iobuf);
			return false;
		}

		if (init_put_byte(
				&rContainer->pb,
				(unsigned char*)(rContainer->iobuf),
				IO_BUFFER_SIZE * 1024,
				0,
				(void*)rContainer, av_read_callback,
				NULL,
				rContainer->seekable ? av_seek_callback : NULL) < 0) {

			/*clog << "FFmpegVideoProvider::initializeFFmpeg";
			clog << "init_put_byte() failed!";
			clog << endl;*/

			rContainer->buffer->Release(rContainer->buffer);
			delete (char*)(rContainer->iobuf);
			return false;
		}

		rContainer->pb.is_streamed = (
				!rContainer->seekable					||
				!strncmp(pd.filename, "http://", 7 )	||
				!strncmp(pd.filename, "unsv://", 7 )	||
				!strncmp(pd.filename, "ftp://",  6 )	||
				!strncmp(pd.filename, "rtsp://", 7 ));

		if (av_open_input_stream(
				&(rContainer->context),
				&rContainer->pb,
				pd.filename,
				fmt,
				NULL) < 0) {

			/*clog << "FFmpegVideoProvider::initializeFFmpeg";
			clog << " av_open_input_stream() failed!";
			clog << endl;*/

			rContainer->buffer->Release(rContainer->buffer);
			delete (char*)(rContainer->iobuf);
			return false;
		}

		/*clog << "FFmpegVideoProvider::initializeFFmpeg ";
		clog << "Searching stream info";
		clog << endl;*/

		if (av_find_stream_info(rContainer->context) < 0) {
			/*clog << "FFmpegVideoProvider::initializeFFmpeg ";
			clog << "couldn't find stream info!";
			clog << endl;*/

			rContainer->buffer->Release(rContainer->buffer);
			delete (char*)(rContainer->iobuf);
			return false;
		}

		for (i = 0; i < rContainer->context->nb_streams; i++) {
			switch (rContainer->context->streams[i]->codec->codec_type) {
				case CODEC_TYPE_VIDEO:
					if (!rContainer->video.st ||
							rContainer->video.st->codec->bit_rate <
							rContainer->context->streams[i]->codec->bit_rate) {

						rContainer->video.st = rContainer->context->streams[i];
					}
					break;

				case CODEC_TYPE_AUDIO:
					if (!rContainer->audio.st ||
							rContainer->audio.st->codec->bit_rate <
							rContainer->context->streams[i]->codec->bit_rate) {

						rContainer->audio.st = rContainer->context->streams[i];
					}
					break;

				default:
					break;
			}
		}

		/*clog << "FFmpegVideoProvider::initializeFFmpeg ";
		clog << "Checking video stream";
		clog << endl;*/

		if (!rContainer->video.st) {
			/*clog << "FFmpegVideoProvider::initializeFFmpeg ";
			clog << "couldn't find video stream!";
			clog << endl;*/

			rContainer->buffer->Release(rContainer->buffer);
			delete (char*)(rContainer->iobuf);
			return false;
		}

		/*clog << "FFmpegVideoProvider::initializeFFmpeg ";
		clog << "Checking video codec";
		clog << endl;*/

		rContainer->video.ctx   = rContainer->video.st->codec;
		rContainer->video.codec = avcodec_find_decoder(
				rContainer->video.ctx->codec_id);

		if (!rContainer->video.codec ||
				avcodec_open(
						rContainer->video.ctx, rContainer->video.codec) < 0) {

			/*clog << "FFmpegVideoProvider::initializeFFmpeg ";
			clog << "error opening video codec!";
			clog << endl;*/

			rContainer->video.ctx = NULL;
			rContainer->buffer->Release(rContainer->buffer);
			delete (char*)(rContainer->iobuf);
			return false;
		}

		avcodec_thread_init(rContainer->video.ctx, 2);
		rContainer->video.ctx->thread_count = 2;

		/*clog << "FFmpegVideoProvider::initializeFFmpeg ";
		clog << "Creating video frame";
		clog << endl;*/

		rContainer->video.src_frame = avcodec_alloc_frame();
		if (rContainer->video.src_frame == NULL) {
			/*clog << "FFmpegVideoProvider::initializeFFmpeg ";
			clog << "CAN'T CREATE FRAME CODEC" << endl;*/

			rContainer->buffer->Release(rContainer->buffer);
			delete (char*)(rContainer->iobuf);
			return false;
		}

		/*clog << "FFmpegVideoProvider::initializeFFmpeg ";
		clog << "video frame created!";
		clog << endl;*/

		rContainer->video.rate = av_q2d(rContainer->video.st->r_frame_rate);
		if (!rContainer->video.rate || !finite(rContainer->video.rate)) {
			/*clog << "FFmpegVideoProvider::initializeFFmpeg ";
			clog << "assuming 25 framesXsecond.";
			clog << endl;*/
			rContainer->video.rate = 25.0;
		}

		/*clog << "FFmpegVideoProvider::initializeFFmpeg ";
		clog << "Checking audio stream";
		clog << endl;*/

		if (rContainer->audio.st) {
			rContainer->audio.ctx   = rContainer->audio.st->codec;
			rContainer->audio.codec = avcodec_find_decoder(
					rContainer->audio.ctx->codec_id);

			if (!rContainer->audio.codec ||
					avcodec_open(
						rContainer->audio.ctx, rContainer->audio.codec ) < 0) {

				rContainer->audio.st	= NULL;
				rContainer->audio.ctx   = NULL;
				rContainer->audio.codec = NULL;
			}
		}

		/*clog << "FFmpegVideoProvider::initializeFFmpeg ";
		clog << "Checking initializing fusion sound";
		clog << endl;*/

		if (FusionSoundAudioProvider::_fsSound == NULL) {
			FusionSoundAudioProvider::initialize();
		}

		if (rContainer->audio.st &&
				FusionSoundAudioProvider::_fsSound != NULL) {

			FSStreamDescription dsc;

			if (rContainer->audio.ctx->channels > FS_MAX_CHANNELS) {
				rContainer->audio.ctx->channels = FS_MAX_CHANNELS;
			}

			dsc.flags = (FSStreamDescriptionFlags)(
					FSSDF_BUFFERSIZE   | FSSDF_CHANNELS  |
					FSSDF_SAMPLEFORMAT | FSSDF_SAMPLERATE);

			dsc.channels     = rContainer->audio.ctx->channels;
			dsc.samplerate   = rContainer->audio.ctx->sample_rate;
			dsc.buffersize   = dsc.samplerate/10; /* 100(ms) */
			dsc.sampleformat = FSSF_S16;

			rContainer->audio.sound = FusionSoundAudioProvider::_fsSound;
			ret = rContainer->audio.sound->CreateStream(
					rContainer->audio.sound,
					&dsc, &rContainer->audio.stream);

			if (ret != DR_OK) {
				/*clog << "FFmpegVideoProvider::initializeFFmpeg ";
				clog << "IFusionSound::CreateStream() failed!";
				clog << endl;*/
				rContainer->audio.sound->Release(rContainer->audio.sound);
				rContainer->audio.sound = NULL;

			} else {
				rContainer->audio.stream->GetPlayback(
						rContainer->audio.stream,
						&rContainer->audio.playback);

				rContainer->audio.sample_size = 2 * dsc.channels;
				rContainer->audio.sample_rate = dsc.samplerate;
				rContainer->audio.buffer_size = dsc.buffersize;
			}

		} else if (rContainer->audio.st) {
			clog << "FFmpegVideoProvider::initializeFFmpeg ";
			clog << "couldn't get FusionSound interface!";
			clog << endl;
		}

		/*clog << "FFmpegVideoProvider::initializeFFmpeg ";
		clog << "Filling video context";
		clog << endl;*/

		rContainer->video.queue.max_len = av_rescale_q(
				MAX_QUEUE_LEN*AV_TIME_BASE,
				AV_TIME_BASE_Q,
				rContainer->video.st->time_base);

		if (rContainer->video.ctx->bit_rate > 0) {
			rContainer->video.queue.max_size = MAX_QUEUE_LEN *
					rContainer->video.ctx->bit_rate/8;

		} else {
			rContainer->video.queue.max_size = MAX_QUEUE_LEN * 256 * 1024;
		}

		/*clog << "FFmpegVideoProvider::initializeFFmpeg ";
		clog << "Filling audio context";
		clog << endl;*/

		if (rContainer->audio.st) {
			rContainer->audio.queue.max_len = av_rescale_q(
					MAX_QUEUE_LEN*AV_TIME_BASE,
					AV_TIME_BASE_Q,
					rContainer->audio.st->time_base);

			if (rContainer->audio.ctx->bit_rate > 0) {
				rContainer->audio.queue.max_size = MAX_QUEUE_LEN *
						rContainer->audio.ctx->bit_rate/8;

			} else {
				rContainer->audio.queue.max_size = MAX_QUEUE_LEN * 64 * 1024;
			}
		}

		/*clog << "FFmpegVideoProvider::initializeFFmpeg ";
		clog << "Determining start time";
		clog << endl;*/

		if (rContainer->context->start_time != (int64_t)AV_NOPTS_VALUE) {
			rContainer->start_time = rContainer->context->start_time;
		}

		/*clog << "FFmpegVideoProvider::initializeFFmpeg ";
		clog << "Initialing mutexes";
		clog << endl;*/

		direct_util_recursive_pthread_mutex_init(&rContainer->input.lock);
		direct_util_recursive_pthread_mutex_init(&rContainer->video.lock);
		direct_util_recursive_pthread_mutex_init(&rContainer->audio.lock);
		direct_util_recursive_pthread_mutex_init(&rContainer->video.queue.lock);
		direct_util_recursive_pthread_mutex_init(&rContainer->audio.queue.lock);
		pthread_cond_init (&rContainer->video.cond, NULL);
		pthread_cond_init (&rContainer->audio.cond, NULL);

		/*clog << "FFmpegVideoProvider::initializeFFmpeg ALL DONE";
		clog << endl;*/

		return true;
	}

	void FFmpegVideoProvider::setLoadSymbol(string symbol) {
		this->symbol = symbol;
	}

	string FFmpegVideoProvider::getLoadSymbol() {
		return this->symbol;
	}

	void FFmpegVideoProvider::setAVPid(int aPid, int vPid) {

	}

	void* FFmpegVideoProvider::getContent() {
		return NULL;
	}

	void FFmpegVideoProvider::feedBuffers() {
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

	void FFmpegVideoProvider::getVideoSurfaceDescription(
			DFBSurfaceDescription* desc) {

		if (!desc) {
			clog << "FFmpegVideoProvider::getVideoSurfaceDescription ";
			clog << "Warning! CAN'T DEFINE MAIN VIDEO SURFACE" << endl;
			return;
		}

		desc->flags = (DFBSurfaceDescriptionFlags)(
				DSDESC_WIDTH | DSDESC_HEIGHT | DSDESC_PIXELFORMAT);

		if (rContainer->video.src_frame->interlaced_frame) {
			desc->flags = (DFBSurfaceDescriptionFlags)(
					desc->flags | DSDESC_CAPS);

			desc->caps = DSCAPS_INTERLACED;
		}

		desc->caps = (DFBSurfaceCapabilities)(
				desc->caps | DSCAPS_VIDEOONLY | DSCAPS_STATIC_ALLOC);

		desc->width  = rContainer->video.ctx->width;
		desc->height = rContainer->video.ctx->height;
		if (desc->width < 1 || desc->height < 1) {
			desc->width  = 320;
			desc->height = 240;
		}

		switch (rContainer->video.ctx->pix_fmt) {
			case PIX_FMT_RGB8:
				desc->pixelformat = DSPF_RGB332;
				break;

			case PIX_FMT_RGB555:
				desc->pixelformat = DSPF_ARGB1555;
				break;

			case PIX_FMT_RGB565:
				desc->pixelformat = DSPF_RGB16;
				break;

			case PIX_FMT_RGB24:
			case PIX_FMT_BGR24:
				desc->pixelformat = DSPF_RGB24;
				break;

			case PIX_FMT_RGB32:
			case PIX_FMT_BGR32:
				desc->pixelformat = DSPF_RGB32;
				break;

			case PIX_FMT_UYVY422:
				desc->pixelformat = DSPF_UYVY;
				break;

			case PIX_FMT_YUV444P:
			case PIX_FMT_YUV422P:
			case PIX_FMT_YUV420P:
			case PIX_FMT_YUV411P:
			case PIX_FMT_YUV410P:
			case PIX_FMT_YUVJ420P:
			case PIX_FMT_YUVJ422P:
			case PIX_FMT_YUVJ444P:
				desc->pixelformat = DSPF_I420;
				break;

			default:
				desc->pixelformat = DSPF_YUY2;
				break;
		}
	}

	ISurface* FFmpegVideoProvider::getPerfectSurface() {
		DFBSurfaceDescription dsc;

		getVideoSurfaceDescription(&dsc);

		return new DFBSurface(
				myScreen,
				DFBDeviceScreen::createUnderlyingSurface(&dsc));
	}

	bool FFmpegVideoProvider::checkVideoResizeEvent(ISurface* frame) {
		IDirectFBSurface* s = NULL;
		DFBSurfaceDescription dsc;
		int w, h;

		getVideoSurfaceDescription(&dsc);

		frame->getSize(&w, &h);
		if (dsc.width != w || dsc.height != h) {
			clog << "FFmpegVideoProvider::checkVideoResizeEvent ";
			clog << "width = '" << w << "' height = '" << h << "'" << endl;

			s = DFBDeviceScreen::createUnderlyingSurface(&dsc);

			rContainer->video.dest = s;
			frame->setContent(s);
			updateVisualData(frame);
			return true;

		} else {
			clog << "FFmpegVideoProvider::checkVideoResizeEvent !update ";
			clog << "width = '" << w << "' height = '" << h << "'" << endl;
		}

		return false;
	}

	void FFmpegVideoProvider::getOriginalResolution(int* height, int* width) {
		*width  = rContainer->video.ctx->width;
		*height = rContainer->video.ctx->height;
	}

	double FFmpegVideoProvider::getTotalMediaTime() {
		if (rContainer->context->duration != (int64_t)AV_NOPTS_VALUE) {
			return (double)rContainer->context->duration/AV_TIME_BASE;
		}

		return 0.0;
	}

	double FFmpegVideoProvider::getMediaTime() {
		s64 position;
		double seconds;

		position = get_stream_clock(rContainer) - rContainer->start_time;
		seconds = (position < 0) ? 0.0 : ((double)position/AV_TIME_BASE);

		return seconds;
	}

	int64_t FFmpegVideoProvider::getVPts() {
		return rContainer->video.st->reference_dts;
	}

	void FFmpegVideoProvider::setMediaTime(double pos) {

	}

	bool FFmpegVideoProvider::updateVisualData(ISurface* surface) {
		rContainer->video.dest = (IDirectFBSurface*)(surface->getContent());
		return true;
	}

	void FFmpegVideoProvider::dynamicRenderCallBack(void* rendererContainer) {
		IDirectFBWindow* w					= NULL;
		IDirectFBSurface* s					= NULL;
		IDirectFBSurface* frame				= NULL;
		IDirectFBVideoProvider_FFmpeg_data* cont = NULL;
		IWindow* someWindow					= NULL;
		ISurface* someSurface				= NULL;

		bool fitTo;
		bool stretchTo;
		int winWidth, winHeight;
		int surWidth, surHeight;

		cont = (IDirectFBVideoProvider_FFmpeg_data*)rendererContainer;
		if (cont == NULL) {
			return;
		}

		someSurface = cont->surface;
		frame = (IDirectFBSurface*)(someSurface->getContent());
		if (frame == NULL) {
			return;
		}

		someWindow = (IWindow*)(someSurface->getParent());
		if (someWindow != NULL) {
			DFBCHECK(frame->GetSize(frame, &surWidth, &surHeight));

			someWindow->lock();
			fitTo = someWindow->getFit();
			stretchTo = someWindow->getStretch();
			w = (IDirectFBWindow*)(someWindow->getContent());

			if (w == NULL) {
				someWindow->unlock();
				return;
			}

			//DFBCHECK(w->GetSize(w, &winWidth, &winHeight));
			if (w->GetSurface(w, &s) != DFB_OK) {
				return;
			}

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

	void FFmpegVideoProvider::playOver(
			ISurface* surface, bool hasVisual, IProviderListener* listener) {

		DFBRectangle rect = { 0, 0, 0, 0 };

		IDirectFBSurface* dest;

		clog << "FFmpegVideoProvider::playOver" << endl;

		pthread_mutex_lock(&rContainer->input.lock);
		pthread_mutex_lock(&rContainer->video.lock);
		pthread_mutex_lock(&rContainer->audio.lock);

		if (rContainer->video.dest) {
			rContainer->video.dest->Release(rContainer->video.dest);
		}

		dest = (IDirectFBSurface*)(surface->getContent());

		rContainer->surface    = surface;
		rContainer->video.dest = dest;
		rContainer->video.rect = rect;
		rContainer->callback   = FFmpegVideoProvider::dynamicRenderCallBack;
		rContainer->ctx        = rContainer;

		if (rContainer->status == DVSTATE_FINISHED) {
			clog << "FFmpegVideoProvider::playOver status is finished ";
			clog << endl;
			rContainer->input.seek_time = 0;
			rContainer->input.seek_flag = 0;
			rContainer->input.seeked	= true;
		}

		rContainer->status = DVSTATE_PLAY;

		if (!rContainer->input.thread) {
			rContainer->input.thread = direct_thread_create(
					DTT_DEFAULT,
					FFmpegInput,
					(void*)rContainer,
					"FFmpeg Input");
		}

		if (!rContainer->audio.thread && rContainer->audio.stream) {
			rContainer->audio.thread = direct_thread_create(
					DTT_DEFAULT,
					FFmpegAudio,
					(void*)rContainer,
					"FFmpeg Audio");
		}

		if (!rContainer->video.thread) {
			rContainer->video.thread = direct_thread_create(
					DTT_DEFAULT,
					FFmpegVideo,
					(void*)rContainer,
					"FFmpeg Video");
		}

		pthread_mutex_unlock(&rContainer->audio.lock);
		pthread_mutex_unlock(&rContainer->video.lock);
		pthread_mutex_unlock(&rContainer->input.lock);
	}

	void FFmpegVideoProvider::resume(ISurface* surface, bool hasVisual) {
		//play();
	}

	void FFmpegVideoProvider::pause() {
		stop();
	}

	void FFmpegVideoProvider::stop() {
		if (rContainer->status == DVSTATE_STOP)
			return;

		pthread_mutex_lock(&rContainer->input.lock);

		rContainer->status = DVSTATE_STOP;

		if (rContainer->input.thread) {
			direct_thread_join(rContainer->input.thread);
			direct_thread_destroy(rContainer->input.thread);
			rContainer->input.thread = NULL;
		}

		if (rContainer->video.thread) {
			pthread_mutex_lock(&rContainer->video.lock);
			pthread_cond_signal(&rContainer->video.cond);
			pthread_mutex_unlock(&rContainer->video.lock);
			direct_thread_join(rContainer->video.thread);
			direct_thread_destroy(rContainer->video.thread);
			rContainer->video.thread = NULL;
		}

		if (rContainer->audio.thread) {
			pthread_mutex_lock(&rContainer->audio.lock);
			pthread_cond_signal(&rContainer->audio.cond);
			pthread_mutex_unlock(&rContainer->audio.lock);
			direct_thread_join(rContainer->audio.thread);
			direct_thread_destroy(rContainer->audio.thread);
			rContainer->audio.thread = NULL;
		}

		if (rContainer->video.dest) {
			rContainer->video.dest->Release(rContainer->video.dest);
			rContainer->video.dest = NULL;
		}

		pthread_mutex_unlock(&rContainer->input.lock);
	}

	void FFmpegVideoProvider::setSoundLevel(float level) {
		DirectResult ret;

		if (level < 0.0) {
			return;
		}

		if (rContainer->audio.playback) {
			ret = rContainer->audio.playback->SetVolume(
					rContainer->audio.playback, level);

			if (ret == DR_OK) {
				rContainer->volume = level;
			}
		}
	}

	bool FFmpegVideoProvider::releaseAll() {
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
		createFFmpegVideoProvider(GingaScreenID screenId, const char* mrl) {

	return (new ::br::pucrio::telemidia::ginga::core::mb::FFmpegVideoProvider(
			screenId, mrl));
}

extern "C" void destroyFFmpegVideoProvider(
		::br::pucrio::telemidia::ginga::core::mb::
		IContinuousMediaProvider* cmp) {

	delete cmp;
}
